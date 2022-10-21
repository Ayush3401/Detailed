#include <iostream>
#include <vector>
#include <stack>
#include <pthread.h>
#include <semaphore.h>
#include <sstream>

using namespace std;

#include "mem_defs.cpp"
#include "inter_connect.cpp"

#include "remote_mem_allocator.cpp"
#include "remote_access_handler.cpp"
#include "stats.cpp"

// unsigned long page_count=0;
pthread_mutex_t lock;
pthread_barrier_t b;
sem_t sem1, sem2;

string mem;
void print_mem_stats(int);
uint64_t common_clock = 0;

uint64_t page_counter[num_nodes] = {0};
vector<uint64_t> remote_pages[num_nodes];
vector<uint64_t> local_pages[num_nodes];

// page-walk and page allocation combined (just for ease)
// page table walk for getting/allocating physical memory address or
// telling if it remote address or not
long int local_page_table_walk(pgd &_pgd, long int vaddr, local_addr_space &L, int node_no)
{
	// used to check weather this page should be allocated on local or remote memory
	// pages are allocated alternately on local/emote memory until local is available

	pud *_pud;
	pmd *_pmd;
	pte *_pte;
	page *_page;

	unsigned long pgd_vaddr = 0L, pud_vaddr = 0L, pmd_vaddr = 0L, pte_vaddr = 0L, page_offset_addr = 0L;
	split_vaddr(pgd_vaddr, pud_vaddr, pmd_vaddr, pte_vaddr, page_offset_addr, vaddr);

	// this logic is used to allocate pages between local and remote, rather than completely using local memory before,
	// it can be allocated in a round-robin manner, one in local, one in remote
	long int virt_page_addr = get_page_addr(vaddr);

	bool remote_found = find(remote_pages[node_no].begin(), remote_pages[node_no].end(), virt_page_addr) != remote_pages[node_no].end();
	bool local_found = find(local_pages[node_no].begin(), local_pages[node_no].end(), virt_page_addr) != local_pages[node_no].end();

	if (remote_found == true || local_found == true)
	{
		if (remote_found == true)
			return -1;
		else if (local_found == true)
			goto A;
	}
	else if (remote_found == false || local_found == false)
	{
		if (remote_found == false)
		{
			if (page_counter[node_no] % 2 == 0)
			{
				remote_pages[node_no].push_back(virt_page_addr);
				page_counter[node_no]++;
				return -1;
			}
		}
		if (local_found == false)
		{
			if (page_counter[node_no] % 2 == 1)
			{
				local_pages[node_no].push_back(virt_page_addr);
				page_counter[node_no]++;
			}
		}
	}

A:

	// cout<<"\npmd_addr  "<<pmd_vaddr<<"\n";

	/*	cout<<pgd_vaddr<<" "<<pud_vaddr<<" "<<pmd_vaddr<<" " <<pte_vaddr<<" "<<page_offset_addr<<" "<<page_counter[node_no];
		cin.get();*/
	_pud = _pgd.access_in_pgd(pgd_vaddr);
	if (_pud == nullptr && L.free_local()) // && page_counter[node_no]%2==1)
	{
		_pgd.add_in_pgd(pgd_vaddr);
		_pud = _pgd.access_in_pgd(pgd_vaddr);
	}
	else if (_pud == nullptr && (!L.free_local())) // || page_counter[node_no]%2==0))
		return -1;

	_pmd = _pud->access_in_pud(pud_vaddr);
	if (_pmd == nullptr && L.free_local()) // && page_counter[node_no]%2==1)
	{
		_pud->add_in_pud(pud_vaddr);
		_pmd = _pud->access_in_pud(pud_vaddr);
	}
	else if (_pmd == nullptr && (!L.free_local())) // || page_counter[node_no]%2==0))
		return -1;

	_pte = _pmd->access_in_pmd(pmd_vaddr);
	if (_pte == nullptr && L.free_local()) // && page_counter[node_no]%2==1)
	{
		_pmd->add_in_pmd(pmd_vaddr);
		_pte = _pmd->access_in_pmd(pmd_vaddr);
	}
	else if (_pte == nullptr && (!L.free_local())) // || page_counter[node_no]%2==0))
		return -1;

	_page = _pte->access_in_pte(pte_vaddr);
	if (_page == nullptr && L.free_local()) // && page_counter[node_no]%2==1)
	{
		long int pte_paddr = L.allocate_local_page(); // will request a new page from local memory
		_pte->add_in_pte(pte_vaddr, pte_paddr);		  // new page entry at vaddr to paddr map
		_page = _pte->access_in_pte(pte_vaddr);
	}
	else if (_page == nullptr && (!L.free_local())) // || page_counter[node_no]%2==0))
		return -1;

	long int paddr = _page->get_page_base_addr(pte_vaddr);
	if (L.is_local(paddr))
	{
		// cout<<"\nLocal Page referred "<<paddr;
		paddr = paddr << 12;
		paddr = paddr + page_offset_addr;
		// cout<<"\nphysica_addr ="<<hex<<"0x"<<setfill('0')<<setw(16)<<(paddr);
		return paddr;
	}
	else
	{
		return -1;
	}
}

bool local_flag = false;
int node_complete_count = 0;
int mem_stat_window = 0;

// function to simulate memory access at each node for local memory accesses and
// pass remote accesses to remote_memory_handler
void *local_handler(void *node)
{
	vector<Trace> tra;
	long node_id = (long)node;
	// cout<<"\n"<<node_id;
	int obj_id = node_id - 1;
	// cout<<obj_id;
	// cin.get();
	ifstream trace_in;
	string in;
	std::ostringstream ss;
	ss << node_id;
	in = "../Output/Node" + ss.str() + "/TraceFile.trc";
	const char *in1 = in.c_str();
	trace_in.open(in1);

	int window_no = 1;
	uint64_t memory_access_count = 0;
	Trace temp;
	bool is_read = true;
	// used as transaction ID of local-memory request for each memory transaction
	uint64_t trans_id = 0;

	bool is_last = false;
	unsigned long last_cycle_no = 0;
	while (1)
	{
		memory_access_count++;
		if (node_id == 1)
		{
			common_clock++;
		}
		pthread_barrier_wait(&b);
		if (trace_in.eof() || window_no > max_windows)
		{
			pthread_mutex_lock(&lock);
			node_complete_count++;
			pthread_mutex_unlock(&lock);
			while (1)
			{
				pthread_barrier_wait(&b);

				if (node_id == 1)
				{
					mem_stat_window++;
					if (mem_stat_window % res_window == 0)
					{
						print_mem_stats(window_no);
					}

					// is_last = true;
					// to_remote_memory(is_last);
				}

				if (node_complete_count == num_nodes)
				{
					pthread_exit(NULL);
				}

				pthread_barrier_wait(&b);
			}
		}

		if (is_read == true)
		{

			trace_in.read((char *)&temp, sizeof(temp));
			is_read = false;
		}
	}
	// long double current_window = temp.cycle / window_cycles;
	uint64_t current_request_cycle = (double)(temp.cycle);

	if (current_request_cycle == common_clock && !trace_in.eof())
	{
		long int local_paddr = local_page_table_walk(_pgd[((node_id - 1) * 3) + temp.procid - 1], temp.addr, L[node_id - 1], (node_id - 1));
		bool isWrite = (temp.r == 'W' ? true : false);
		is_read = true;
		count_access[obj_id]++;
		if (local_paddr != -1)
		{
			local_access[node_id - 1]++;
			local_paddr = local_paddr & 0xffffffffffc0;
			// long int cycle_diff = current_request_cycle - last_cycle_no;
			// adding memory transaction
			// for (int i = 0; i < cycle_diff; i++)
			local_mem[obj_id]->update();
			obj.add_one_and_run(local_mem[obj_id], local_paddr, isWrite, trans_id, current_request_cycle, current_request_cycle, (node_id - 1));

			last_cycle_no = current_request_cycle;
			trans_id++;
		}
		else if (local_paddr == -1)
		{
			static uint64_t remote_trans_id = 0;
			// last_cycle_no=current_request_cycle;
			remote_access[node_id - 1]++;
			memory_stream remote_access;
			remote_access.source = node_id - 1;
			remote_access.procid = temp.procid;
			remote_access.threadid = temp.threadid;
			remote_access.addr = temp.addr;
			remote_access.r = temp.r;
			remote_access.trans_id = remote_trans_id;
			remote_access.cycle = current_request_cycle; //+dis_latency;	//adding 300 extra cycle to each remote access as disaggregated latency
			remote_access.miss_cycle_num = current_request_cycle;
			pthread_mutex_lock(&lock);
			rem_mem_stream.push_back(remote_access);
			pthread_mutex_unlock(&lock);
			pthread_barrier_wait(&b);
			remote_trans_id++;
		}

		//			cout<<"Node-ID"<<node_id<<"  "<<"pgd-index"<<node_id-1+temp.procid-1<<endl;
		//			cout<<tra[i].procid<<"  "<<tra[i].threadid<<"  "<<hex<<"0x"<<tra[i].addr<<"  "<<dec<<tra[i].cycle<<endl;
	}
	pthread_barrier_wait(&b);
	if (node_id == 1)
	{
		to_remote_memory();
		if (common_clock % window_cycles == 0)
		{
			print_mem_stats(window_no);
			window_no++;
		}
	}
	// else if (current_window > window_no)
	// {
	// all threads wait until all of them completes a window,
	// one thread will send collcted remote requests to remote_memory handling code
	// after returning, all threads will again start together for next window
	// 	pthread_barrier_wait(&b);

	// 	if (node_id == 1)
	// 	{
	// 		// to print stats after every time it reaches the result_window
	// 		mem_stat_window++;
	// 		if (mem_stat_window % res_window == 0)
	// 		{
	// 			print_mem_stats(window_no);
	// 		}

	// 		to_remote_memory(is_last);
	// 	}

	// 	trace_in.seekg(-32, ios::cur);
	// 	pthread_barrier_wait(&b);

	// 	// increase the window number, 1 million memory(default) accesses per node in each window
	//
	// }
}

int main(int argc, char *argv[])
{
	string dir;
	if (argc < 2)
		exit(0);
	else if (argc == 2)
		dir = argv[1];

	for (int i = 0; i < num_nodes; i++)
	{
		node_round_robin_last[i] = 1;
	}

	for (int i = 0; i < num_mem_pools; i++)
	{
		last_cycle[i] = 0;
		for (int j = 0; j < max_samples; j++)
			sample_access_count_buffer[i][j] = 0;
	}

	for (int i = 0; i < num_nodes; i++)
	{
		node_epoch_chunk_count[i] = 0;
		node_chunk_request_size[i] = 4;
		node_epoch_access_count[i] = 0;
	}

	declare_memory_variables(dir);

	pthread_mutex_init(&lock, NULL);
	pthread_mutex_init(&lock_queue, NULL);
	pthread_mutex_init(&lock_update, NULL);
	pthread_barrier_init(&b, NULL, num_nodes);

	sem_init(&sem1, 0, 0);
	sem_init(&sem2, 0, 0);
	pthread_t local_mem_threads[num_nodes];
	pthread_t remote_mem_threads[num_mem_pools];

	// threads for simulating local memory access to each node
	for (long i = 1; i <= num_nodes; i++)
	{
		// thread-1 is responsble for sending the per window remote memory accesses
		// to the remote pools
		pthread_create(&local_mem_threads[i - 1], NULL, local_handler, (void *)i);
	}

	// threads for simulating remote memory access to each pool
	for (long i = 1; i <= num_mem_pools; i++)
	{
		pthread_create(&remote_mem_threads[i - 1], NULL, remote_pool_handler, (void *)i);
	}

	for (long i = 1; i <= (num_nodes); i++)
	{
		pthread_join(local_mem_threads[i - 1], NULL);
	}

	// last batch of remote memory accesses has been sent and all accesses
	// from all nodes have been parsed
	rem_flag = true;

	for (int j = 0; j < num_nodes; j++)
	{
		for (int i = 0; i < 1000000; i++)
			local_mem[j]->update();
	}

	for (long i = 1; i <= (num_mem_pools); i++)
	{
		// give extra cycles to complete rest of the memory accesses
		for (int j = 0; j < 10000000; j++)
			remote_mem[i - 1]->update();
		pthread_join(remote_mem_threads[i - 1], NULL);
	}

	pthread_detach(pthread_self());
	pthread_mutex_destroy(&lock);
	pthread_mutex_destroy(&lock_update);
	pthread_mutex_destroy(&lock_queue);

	sem_destroy(&sem1);
	sem_destroy(&sem2);

	// print memory stats in DRAM simulator output files
	for (int i = 0; i < num_nodes; i++)
		local_mem[i]->printStats(true);

	for (int i = 0; i < num_mem_pools; i++)
		remote_mem[i]->printStats(true);

	print_mem_stats(-1);

	mem_stats.close();
	out.close();
	invalid.close();
	track.close();

	cout << "\n====================Simulation_Complete====================\n";
	//	pthread_exit(NULL);

	/*	L[0].display_mapping();
		R[0].display_mapping();

		// get a nice summary of this epoch
	//	mem->printStats(true);

		cout<<"\nFree pages left "<<L.has_free();//<<endl;
		cout<<"\n\nPages Reffered: \t"<<page_count<<endl;
		cout<<endl;*/
	return 0;
}

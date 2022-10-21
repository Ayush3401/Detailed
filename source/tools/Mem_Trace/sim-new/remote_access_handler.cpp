/*
epoch is one complete ciculation of 10000000 cycles

10 samples are taken per epoch to count memory accesses to each pool, used for finding variation

window is time between any two consecutive remote allocation requests
*/

#define max_samples 2800

// sampling done for access counts in each epoch, 10 samples taken per epoch to store memory access count for each pool in each sample
long double current_sample_access_count[num_mem_pools] =  {0};
long double sample_access_count_buffer[num_mem_pools][max_samples];

static int sample_num = 0;
void reset_sample_access_count()
{
	for (int i = 0; i < num_mem_pools; i++)
	{
		sample_access_count_buffer[i][sample_num] = current_sample_access_count[i];
		current_sample_access_count[i] = 0;
	}
	sample_num++;
}

// Page walk for remote addresses at local nodes for getting/allocate the local physical(actually
//  virtual) address then search the remote pool to forward this address to
long int extended_local_page_table_walk(pgd &_pgd, unsigned long vaddr, local_addr_space &L, int &remote_pool, int node_no)
{
	pud *_pud;
	pmd *_pmd;
	pte *_pte;
	page *_page;

	unsigned long pgd_vaddr = 0L, pud_vaddr = 0L, pmd_vaddr = 0L, pte_vaddr = 0L, page_offset_addr = 0L;
	split_vaddr(pgd_vaddr, pud_vaddr, pmd_vaddr, pte_vaddr, page_offset_addr, vaddr);

	// cout<<"\npmd_addr  "<<pmd_vaddr<<"\n";

	_pud = _pgd.access_in_pgd(pgd_vaddr);
	if (_pud == nullptr)
	{
		_pgd.add_in_pgd(pgd_vaddr);
		_pud = _pgd.access_in_pgd(pgd_vaddr);
	}

	_pmd = _pud->access_in_pud(pud_vaddr);
	if (_pmd == nullptr)
	{
		_pud->add_in_pud(pud_vaddr);
		_pmd = _pud->access_in_pud(pud_vaddr);
	}

	_pte = _pmd->access_in_pmd(pmd_vaddr);
	if (_pte == nullptr)
	{
		_pmd->add_in_pmd(pmd_vaddr);
		_pte = _pmd->access_in_pmd(pmd_vaddr);
	}

	_page = _pte->access_in_pte(pte_vaddr);
	if (_page == nullptr)
	{
		if (!L.free_remote())
		{
			// increament per node chunk requests per window
			node_epoch_chunk_count[node_no]++;
			//		use the required algorithm to decide the remote_pool for requesting new memory //global memory manager responsible for this
			remote_pool = smart_idle();
			//		if(window_no>=2)
			//		{cout<<"\nAllocating in rem pool "<<remote_pool;cin.get();}
			int chunk_size = 4;									  // node_chunk_request_size[node_no];
			request_remote_memory(L, R[remote_pool], chunk_size); // 4MBs
		}

		unsigned long pte_paddr = L.allocate_remote_page(); // will request a new page from Remote memory
		_pte->add_in_pte(pte_vaddr, pte_paddr);				// new page entry at vaddr to paddr map
		_page = _pte->access_in_pte(pte_vaddr);
	}

	unsigned long paddr = _page->get_page_base_addr(pte_vaddr);

	// bool test=false;
	if (remote_pool == -1)
		remote_pool = L.find_remote_pool(paddr);
	else
	{
		//	test=true;
		//	cout<<"\nLocal Page number  "<<paddr<<"in remote pool=  "<<remote_pool<<"\n";//<<endl;
	}

	// Cannot be true, already checked this condition before sending to remote handler.
	if (L.is_local(paddr))
	{
		cout << "\npage_num: " << paddr;
		cin.get();
		invalid << "\nwrong address in local";
	}

	// convert local physical page address to remote memory pool page address
	paddr = L.remote_page_addr(paddr, remote_pool);
	/*if(test==true)
	{
		cout<<"\nRemote Page number  "<<paddr;
		cin.get();
	}*/
	paddr = paddr << 12;
	paddr = paddr + page_offset_addr;

	if (remote_pool < 0 || remote_pool >= num_mem_pools)
	{
		invalid << "\nwrong";
		cin.get();
	}
	return paddr;
}

struct remote_pool_instance
{
	uint64_t addr;
	bool isWrite;
	uint64_t cycle;
	int node_id;
};

pthread_mutex_t lock_queue;

// to maintain last_cycle information for each remote pool (used for calculating memory update cycles)
static unsigned long long last_cycle[num_mem_pools] = {0};

bool rem_flag = false;
// thread that will simulate memory access for each pool
void *remote_pool_handler(void *pool)
{
	uint64_t trans_id = 0;
	long remote_pool = (long)pool - 1;
	while (1)
	{
		if (rem_flag == true && packet_queue_pool[remote_pool].size() == 0)
		{
			break;
		}
		if (packet_queue_pool[remote_pool].size() > 0)
		{

			long cycle_diff = 0;
			int64_t cycle = packet_queue_pool[remote_pool].front().mem.cycle;
			int64_t miss_cycle = packet_queue_pool[remote_pool].front().mem.miss_cycle_num;

			if (cycle < 0 || cycle > 999999999999)
			{
				invalid << "\nSkipping_fake_cycle----mem-id:" << remote_pool << "  tid:" << trans_id << "  start_c:" << cycle << "  \
				last_c:" << last_cycle[remote_pool]
						<< "  diff:" << cycle_diff << "  size" << packet_queue_pool[remote_pool].size();
				pthread_mutex_lock(&lock_queue);
				packet_queue_pool[remote_pool].pop_front();
				pthread_mutex_unlock(&lock_queue);
				continue;
			}

			uint64_t addr = packet_queue_pool[remote_pool].front().mem.addr;
			addr = addr & 0xffffffffffc0;
			uint64_t trans_id = packet_queue_pool[remote_pool].front().mem.trans_id;
			bool isWrite = (packet_queue_pool[remote_pool].front().mem.r == 'W' ? true : false);
			int node_no = packet_queue_pool[remote_pool].front().mem.source;

			cycle_diff = cycle - last_cycle[remote_pool];
			if (cycle_diff < 0)
			{
				invalid << "\nSkipping_fake_cycle----mem-id:" << remote_pool << "  tid:" << trans_id << "  start_c:" << cycle << "  \
				last_c:" << last_cycle[remote_pool]
						<< "  diff:" << cycle_diff << "  size" << packet_queue_pool[remote_pool].size();
				pthread_mutex_lock(&lock_queue);
				packet_queue_pool[remote_pool].pop_front();
				pthread_mutex_unlock(&lock_queue);
				continue;
			}

			pthread_mutex_lock(&lock_queue);

			// calculate network-stats using each packet

			total_packets_per_node[node_no]++;
			total_packets_per_pool[remote_pool]++;
			overall_total_packets++;

			uint64_t packet_network_delay = packet_queue_pool[remote_pool].front().mem.cycle - packet_queue_pool[remote_pool].front().mem.miss_cycle_num;
			uint64_t input_port_delay = packet_queue_pool[remote_pool].front().out_switch_input_port - packet_queue_pool[remote_pool].front().in_switch_input_port;
			uint64_t output_port_delay = packet_queue_pool[remote_pool].front().out_switch_output_port - packet_queue_pool[remote_pool].front().in_switch_output_port;
			uint64_t switch_delay = packet_queue_pool[remote_pool].front().out_switch_output_port - packet_queue_pool[remote_pool].front().in_switch_input_port;

			if (packet_network_delay >= 0 && input_port_delay >= 0 && output_port_delay >= 0 && switch_delay >= 0)
			{
				total_network_delay[node_no] = total_network_delay[node_no] + packet_network_delay;
				total_delay_input_ports[node_no] = total_delay_input_ports[node_no] + input_port_delay;
				total_delay_output_ports[remote_pool] = total_delay_output_ports[remote_pool] + output_port_delay;
				overall_total_network_delay = overall_total_network_delay + packet_network_delay;
				overall_total_switch_delay = overall_total_switch_delay + switch_delay;
			}
			else
			{
				cout << "\nTesting:wrong cycle info in packet";
				cout << "\nnode: " << node_no << "pool: " << remote_pool << " miss_cycle" << packet_queue_pool[remote_pool].front().mem.miss_cycle_num << " out cycle " << packet_queue_pool[remote_pool].front().mem.cycle;
				cout << "\ninput port in " << packet_queue_pool[remote_pool].front().in_switch_input_port << "input port out " << packet_queue_pool[remote_pool].front().out_switch_input_port;
				cout << "\noutput port in " << packet_queue_pool[remote_pool].front().in_switch_output_port << "output port out " << packet_queue_pool[remote_pool].front().out_switch_output_port;
				cin.get();
			}

			//track<<"\nmem-id:"<<remote_pool<<"  tid:"<<trans_id<<"  start_c:"<<cycle<<"  last_c:"<<last_cycle[remote_pool]<<"  \
			diff:"<<cycle_diff<<"  size"<<pool_queue[remote_pool].size();

			packet_queue_pool[remote_pool].pop_front();
			pthread_mutex_unlock(&lock_queue);

			for (int i = 0; i < cycle_diff; i++)
			{
				remote_mem[remote_pool]->update();
				memory_cycle_per_pool[remote_pool]++;
			}

			obj.add_one_and_run(remote_mem[remote_pool], addr, isWrite, trans_id, cycle, miss_cycle, node_no);

			last_cycle[remote_pool] = cycle;
		}
	}
	pthread_exit(NULL);
}

uint64_t last_cycle_no_in_this_epoch[num_mem_pools] = {0};

// function to read remote accesses of all the nodes and allocate/forward the access to
// respective memory pool(thread) after passing through the network interconnect
void simulate_remote_memory()
{
	// cout<<"\ngoing"<<window_no<<"--"<<rem_mem_stream.size();
	//  uint64_t cycle_size=rem_mem_stream.size()/10;
	//  uint64_t next_cycle=cycle_size;

	for (uint64_t i = 0; i < rem_mem_stream.size(); i++)
	{
		// if(i==next_cycle)
		// {
		// 	reset_sample_access_count();
		// 	next_cycle=next_cycle+cycle_size;
		// }

		int node_no = rem_mem_stream[i].source;
		int procid = rem_mem_stream[i].procid;
		int remote_pool = -1;
		int pgd_id = ((node_no)*3) + (procid - 1);

		//	cout<<"\nnode_id "<<node_no<<" procid "<<procid<<" Pgd_id"<<pgd_id;
		unsigned long remote_paddr = extended_local_page_table_walk(_pgd[pgd_id], rem_mem_stream[i].addr, L[node_no], remote_pool, node_no);

		if (remote_pool != -1)
		{
			unsigned long page_addr = get_page_addr(remote_paddr);
			// if(i==(rem_mem_stream.size()-1))
			if (!R[remote_pool].validate_addr(page_addr, node_no))
			{
				invalid << "\nInvalid Page";
				cin.get();
				break;
			}

			// bool isWrite=(rem_mem_stream[i].r == 'W' ? true : false);

			// collect remote memory access details to be sent in a packet
			memory_stream remote_access;
			remote_access.source = node_no;
			remote_access.dest = remote_pool;
			remote_access.procid = rem_mem_stream[i].procid;
			remote_access.threadid = rem_mem_stream[i].threadid;
			remote_access.addr = remote_paddr;
			remote_access.r = rem_mem_stream[i].r;
			remote_access.cycle = rem_mem_stream[i].cycle;
			remote_access.miss_cycle_num = rem_mem_stream[i].miss_cycle_num;
			remote_access.trans_id = rem_mem_stream[i].trans_id;

			// last cycle number for request sent to each remote pool is noted,
			// it is used to wait for memory updates, so that packets can be collected after waiting for this much time in each epoch and sent back to node

			last_cycle_no_in_this_epoch[remote_pool] = rem_mem_stream[i].cycle;

			// send memory request to network component
			to_trans_layer(remote_access, packet_queue_node);

			/*			//create an instance of remote pool access and send it to reespective thread
						remote_pool_instance pool_access;
						pool_access.addr=remote_paddr;
						pool_access.isWrite=isWrite;
						pool_access.node_id=node_no;
						pool_access.cycle=rem_mem_stream[i].cycle;

						pthread_mutex_lock(&lock_queue);
						pool_queue[remote_pool].push(pool_access);
						pthread_mutex_unlock(&lock_queue);
			*/

			// increament different counters for memory access after adding a new memory transaction
			// increament node-wise per pool access count
			per_pool_access_count[node_no][remote_pool]++;
			// increament per-window access count for this remote pool
			R[remote_pool].inc_access_count();
			current_sample_access_count[remote_pool]++;
			node_epoch_access_count[node_no]++;
		}
		else if (remote_pool == -1)
		{
			// this case won't happen ever
			invalid << "\nIn-valid remote pool returned";
			invalid << "hex" << remote_paddr << dec << remote_pool;
			L[node_no].display_mapping(invalid);
			//		cin.get();
			//		break;
		}
	}
	rem_mem_stream.clear();
}

// used by sort function to sort the remote memory accesses by cycle
// of all the nodes in this window
bool compare_by_cycle(const memory_stream &a, const memory_stream &b)
{
	return a.cycle < b.cycle;
}

// after each window(after getting memory access upto particular clock cycle from all local nodes)
// this function will simulate the remote memory access reached here after filtering out the
// local accesses

void to_remote_memory()
{
	// epoch_num++;

	// if(rem_mem_stream.size()==0)
	// {
	// 	return;
	// }

	// sort(rem_mem_stream.begin(), rem_mem_stream.end(), compare_by_cycle);

	simulate_remote_memory();

	simulate_network();

	// bool good_to_go;
	// while(good_to_go)
	// {
	// 	good_to_go=true;
	// 	for(int i=0;i<num_mem_pools;i++)
	// 	if(memory_cycle_per_pool[i]==last_cycle_no_in_this_epoch[i])
	// 		good_to_go=good_to_go & true;
	// 	else
	// 		good_to_go=good_to_go & false;
	// }

	simulate_network_reverse();

	calculate_stats_for_packets_received_at_node();

	// cout << "xyz";
	//	cin.get();
	// after this all packets are sent to remote memory pool and are sent to DRAM simulator in order of their arrival

	// modify_chunk_sizes1();

	// reset_chunk_count_and_access();
}

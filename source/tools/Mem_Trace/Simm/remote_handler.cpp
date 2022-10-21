/*
epoch is one complete ciculation of 10000000 cycles

10 samples are taken per epoch to count memory accesses to each pool, used for finding variation

window is time between any two consecutive remote allocation requests
*/

#define max_samples	2800


//sampling done for access counts in each epoch, 10 samples taken per epoch to store memory access count for each pool in each sample
long double current_sample_access_count[num_mem_pools]={0};//,0,0,0};
long double sample_access_count_buffer[num_mem_pools][max_samples];

static int sample_num=0;
void reset_sample_access_count()
{
	for(int i=0;i<num_mem_pools;i++)
	{
		sample_access_count_buffer[i][sample_num]=current_sample_access_count[i];
		current_sample_access_count[i]=0;
	}
	sample_num++;
}

//Page walk for remote addresses at local nodes for getting/allocate the local physical(actually
// virtual) address then search the remote pool to forward this address to
long int extended_local_page_table_walk(pgd &_pgd, unsigned long vaddr, local_addr_space &L, int &remote_pool, int node_no)
{	
	pud *_pud;
	pmd *_pmd;
	pte *_pte;
	page *_page;

	unsigned long pgd_vaddr=0L, pud_vaddr=0L, pmd_vaddr=0L, pte_vaddr=0L, page_offset_addr=0L;
	split_vaddr(pgd_vaddr, pud_vaddr, pmd_vaddr, pte_vaddr, page_offset_addr, vaddr);
	
	//cout<<"\npmd_addr  "<<pmd_vaddr<<"\n";
	
	_pud=_pgd.access_in_pgd(pgd_vaddr);
	if(_pud==nullptr)
	{
		_pgd.add_in_pgd(pgd_vaddr);
		_pud=_pgd.access_in_pgd(pgd_vaddr);
	}

	_pmd=_pud->access_in_pud(pud_vaddr);
	if(_pmd==nullptr)
	{
		_pud->add_in_pud(pud_vaddr);
		_pmd=_pud->access_in_pud(pud_vaddr);
	}

	_pte=_pmd->access_in_pmd(pmd_vaddr);
	if(_pte==nullptr)
	{
		_pmd->add_in_pmd(pmd_vaddr);
		_pte=_pmd->access_in_pmd(pmd_vaddr);
	}

	_page=_pte->access_in_pte(pte_vaddr);
	if(_page==nullptr)
	{
		if(!L.free_remote())
		{
			//increament per node chunk requests per window
			node_epoch_chunk_count[node_no]++;
	//		use the required algorithm to decide the remote_pool for requesting new memory //global memory manager responsible for this
			remote_pool=round_robin_pool_select();
	//		if(window_no>=2)
	//		{cout<<"\nAllocating in rem pool "<<remote_pool;cin.get();}
			int chunk_size=4;//node_chunk_request_size[node_no];
			request_remote_memory(L,R[remote_pool],chunk_size); //4MBs
		}

		unsigned long pte_paddr=L.allocate_remote_page();		//will request a new page from Remote memory
		_pte->add_in_pte(pte_vaddr,pte_paddr);		//new page entry at vaddr to paddr map
		_page=_pte->access_in_pte(pte_vaddr);
	}

	unsigned long paddr=_page->get_page_base_addr(pte_vaddr);

	//bool test=false;
	if(remote_pool==-1)
		remote_pool= L.find_remote_pool(paddr);
	else
	{
	//	test=true;
	//	cout<<"\nLocal Page number  "<<paddr<<"in remote pool=  "<<remote_pool<<"\n";//<<endl;
	}
	
	//Cannot be true, already checked this condition before sending to remote handler.
	if(L.is_local(paddr))
	{
		cout<<"\npage_num: "<<paddr;
		cin.get();
		invalid<<"\nwrong address in local";
	}

	//convert local physical page address to remote memory pool page address 
	paddr=L.remote_page_addr(paddr,remote_pool);
	/*if(test==true)
	{
		cout<<"\nRemote Page number  "<<paddr;
		cin.get();
	}*/
	paddr=paddr<<12;
	paddr=paddr + page_offset_addr;

	if(remote_pool<0 || remote_pool>=num_mem_pools)
	{
		invalid<<"\nwrong";
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
	uint64_t trans_id;
	uint64_t miss_cycle_num;
};

queue <remote_pool_instance> pool_queue[num_mem_pools];

pthread_mutex_t lock_queue;


//to maintain last_cycle information for each remote pool (used for calculating memory update cycles)
static unsigned long long last_cycle[num_mem_pools]={0};//,0,0,0};

bool rem_flag=false;
//thread that will simulate memory access for each pool
void *remote_pool_handler(void *pool)
{
	long remote_pool=(long)pool-1;
	while(1)
	{
		if(rem_flag==true && pool_queue[remote_pool].empty())
		{
			break;
		}
		if(!pool_queue[remote_pool].empty())
		{
			long cycle_diff=0;
			int64_t cycle=pool_queue[remote_pool].front().cycle;
			int64_t miss_cycle=pool_queue[remote_pool].front().miss_cycle_num;
			uint64_t trans_id=pool_queue[remote_pool].front().trans_id;
			
			if(cycle<0 || cycle>999999999999)
			{
				invalid<<"\nSkipping_fake_cycle----mem-id:"<<remote_pool<<"  tid:"<<trans_id<<"  start_c:"<<cycle<<"  last_c:"<<last_cycle[remote_pool]<<"  diff:"<<cycle_diff<<"  size"<<pool_queue[remote_pool].size();
				pthread_mutex_lock(&lock_queue);
				pool_queue[remote_pool].pop();
				pthread_mutex_unlock(&lock_queue);
				continue;
			}
			
			uint64_t addr=pool_queue[remote_pool].front().addr;
			addr= addr & 0xffffffffffc0;
			bool isWrite=pool_queue[remote_pool].front().isWrite;
			int node=pool_queue[remote_pool].front().node_id;
			
			cycle_diff=cycle-last_cycle[remote_pool];
			if(cycle_diff<0)
			{
				invalid<<"\nSkipping_fake_cycle----mem-id:"<<remote_pool<<"  tid:"<<trans_id<<"  start_c:"<<cycle<<"  last_c:"<<last_cycle[remote_pool]<<"  diff:"<<cycle_diff<<"  size"<<pool_queue[remote_pool].size();
				pthread_mutex_lock(&lock_queue);
				pool_queue[remote_pool].pop();
				pthread_mutex_unlock(&lock_queue);
				continue;
			}

			pthread_mutex_lock(&lock_queue);
			//track<<"\nmem-id:"<<remote_pool<<"  tid:"<<trans_id<<"  start_c:"<<cycle<<"  last_c:"<<last_cycle[remote_pool]<<"  diff:"<<cycle_diff<<"  size"<<pool_queue[remote_pool].size();
			pool_queue[remote_pool].pop();
			pthread_mutex_unlock(&lock_queue);

			// for(int i=0;i<cycle_diff;i++)
			// 	remote_mem[remote_pool]->update();

			// obj.add_one_and_run(remote_mem[remote_pool],addr,isWrite,trans_id,cycle,miss_cycle,node);
					
			last_cycle[remote_pool]=cycle;

		}
	}
	pthread_exit(NULL);
}

//function to read remote accesses of all the nodes and allocate/forward the access to 
//respective memory pool(thread)
void simulate_remote_memory()
{
	//cout<<"\ngoing"<<window_no<<"--"<<rem_mem_stream.size();
	uint64_t cycle_size=rem_mem_stream.size()/10;
	uint64_t next_cycle=cycle_size;

	for(uint64_t i=0;i<rem_mem_stream.size();i++)
	{
		if(i==next_cycle)
		{
			reset_sample_access_count();
			next_cycle=next_cycle+cycle_size;
		}
		int node_no=rem_mem_stream[i].source;
		int procid=rem_mem_stream[i].procid;
		int remote_pool=-1;
		int pgd_id=((node_no)*3)+(procid-1);

	//	cout<<"\nnode_id "<<node_no<<" procid "<<procid<<" Pgd_id"<<pgd_id;
		unsigned long remote_paddr=extended_local_page_table_walk(_pgd[pgd_id], rem_mem_stream[i].addr, L[node_no], remote_pool, node_no);
		
		if(remote_pool!=-1)
		{
			unsigned long page_addr=get_page_addr(remote_paddr);
			//if(i==(rem_mem_stream.size()-1))
			if(!R[remote_pool].validate_addr(page_addr,node_no))
			{
				invalid<<"\nInvalid Page";
				cin.get();
				break;
			}

			bool isWrite=(rem_mem_stream[i].r == 'W' ? true : false);
			
			//create an instance of remote pool access and send it to reespective thread
			remote_pool_instance pool_access;
			pool_access.addr=remote_paddr;
			pool_access.isWrite=isWrite;
			pool_access.node_id=node_no;
			pool_access.cycle=rem_mem_stream[i].cycle;
			pool_access.miss_cycle_num=rem_mem_stream[i].miss_cycle_num;
			pool_access.trans_id=rem_mem_stream[i].trans_id;
			pthread_mutex_lock(&lock_queue);
			pool_queue[remote_pool].push(pool_access);
			pthread_mutex_unlock(&lock_queue);


		/*	for(int j=0;j<cycle_diff;j++)
				remote_mem[remote_pool]->update();
			obj.add_one_and_run(remote_mem[remote_pool], remote_paddr , isWrite);*/

		//increament different counters for memory access after adding a new memory transaction	
			//increament node-wise per pool access count
			per_pool_access_count[node_no][remote_pool]++;
			//increament per-window access count for this remote pool
			R[remote_pool].inc_access_count();
			current_sample_access_count[remote_pool]++;
			node_epoch_access_count[node_no]++;
		}
		else if(remote_pool==-1)
		{
			//this case won't happen ever
			invalid<<"\nIn-valid remote pool returned";
			invalid<<"hex"<<remote_paddr<<dec<<remote_pool;
			L[node_no].display_mapping(invalid);
	//		cin.get();
	//		break;
		}
	}
	rem_mem_stream.clear();
}

//used by sort function to sort the remote memory accesses by cycle 
//of all the nodes in this window 
bool compare_by_cycle(const memory_stream &a, const memory_stream &b)
{
	return a.cycle < b.cycle;
}

//after each window(after getting memory access upto particular clock cycle from all local nodes)
//this function will simulate the remote memory access reached here after filtering out the 
//local accesses

void to_remote_memory()
{
		//if(epoch_num/reset_after_windows==0)
	/*{
		for(int i=0;i<num_mem_pools;i++)
		{
			window_access_sample[i][window_num]=window_access_count[i];
			window_access_count[i]=0;
			//_total_access_count[i]=_total_access_count[i]+R[i].get_count();
			//R[i].reset_count();
			//sample_num++;
		}
	}*/

	epoch_num++;
	if(rem_mem_stream.size()==0)
	{
		return;
	}
	

	sort(rem_mem_stream.begin(), rem_mem_stream.end(), compare_by_cycle);
	
	simulate_remote_memory();

	//modify_chunk_sizes1();

	//reset_chunk_count_and_access();

	//_remote_mem[remote_pool].display_mapping();
	//L.display_mapping();
	//L.get_stats();
}

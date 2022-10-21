void print_pool_wise_diff_memory_accesses_time()
{
	track<<"\n--------No. of memory access with access time less than 100ns:--------\n";
	for(int i=0;i<num_mem_pools;i++)
	{
		track<<"\nPool_no-"<<(i+1)<<" : "<<U_100ns[i];
	}
	track<<"\n\n--------No. of memory access with access time between 100ns and 300ns:--------\n";
	for(int i=0;i<num_mem_pools;i++)
	{
		track<<"\nPool_no-"<<(i+1)<<" : "<<B_100_300ns[i];
	}
	track<<"\n\n--------No. of memory access with access time between 300ns and 500ns:--------\n";
	for(int i=0;i<num_mem_pools;i++)
	{
		track<<"\nPool_no-"<<(i+1)<<" : "<<B_300_500ns[i];
	}
	track<<"\n\n--------No. of memory access with access time between 500ns and 700ns:--------\n";
	for(int i=0;i<num_mem_pools;i++)
	{
		track<<"\nPool_no-"<<(i+1)<<" : "<<B_500_700ns[i];
	}	
	track<<"\n\n--------No. of memory access with access time between 700ns and 1000ns:--------\n";
	for(int i=0;i<num_mem_pools;i++)
	{
		track<<"\nPool_no-"<<(i+1)<<" : "<<B_700_1000ns[i];
	}
	track<<"\n\n--------No. of memory access with access time gretaer than 1000ns:--------\n";
	for(int i=0;i<num_mem_pools;i++)
	{
		track<<"\nPool_no-"<<(i+1)<<" : "<<G_1000ns[i];
	}
}


void calculate_avg_mem_access_cycle()
{
	pthread_mutex_lock(&lock_update);

	mem_stats<<"\n\t\t\t\tAverage no. of cycles for memory access\n========================================================================================";
	
	for(int i=0;i<num_nodes;i++)
	{	
		avg_node_local[i]=0;
		avg_node_local[i] = total_node_local[i] / (long double)completed_trans_local[i];

		mem_stats<<"\nTotal completed local memory access on node-"<<i+1<<" : "<<completed_trans_local[i];
		mem_stats<<"\n\tAverage no. of cycles for local-memory access on node-"<<i+1<<" : "<<avg_node_local[i];
	}


	mem_stats<<"\n";
	
	mem_stats<<"\n========Disaggregated latency considered as: "<<(tx_dis_latency + rx_dis_latency)<<"========";
	for(int i=0;i<num_nodes;i++)
	{	
		avg_node_remote[i]=0;
		avg_node_remote[i] = total_node_remote[i] / (long double)completed_trans_node_to_remote[i];

		mem_stats<<"\nTotal completed remote-memory access of node-"<<i+1<<" : "<<completed_trans_node_to_remote[i];
		mem_stats<<"\n\tAverage no. of cycles for remote-memory access on node-"<<i+1<<" : "<<avg_node_remote[i];
	}

	mem_stats<<"\n";

	for(int i=0;i<num_nodes;i++)
	{
		avg_node_overall[i]=0;
		avg_node_overall[i]= (total_node_local[i] + total_node_remote[i]) / (long double) (completed_trans_local[i] + completed_trans_node_to_remote[i]);
		mem_stats<<"\nTotal completed memory accesses (Local + Remote) of node-"<<i+1<<" : "<<(completed_trans_local[i]+completed_trans_node_to_remote[i]);
		mem_stats<<"\n\tAverage no. of cycles for local+remote memory access on node-"<<i+1<<" : "<<avg_node_overall[i];
	}

	mem_stats<<"\n";

	for(int i=0;i<num_mem_pools;i++)
	{
		avg_remote_pool[i]=0;
		avg_remote_pool[i] = total_remote_pool[i] / (long double)completed_trans_remote[i];

		mem_stats<<"\nTotal memory accesses completed at remote pool-"<<i+1<<" : "<<completed_trans_remote[i];
		mem_stats<<"\n\tAverage no. of cycles for memory access on remote pool-"<<i+1<<" : "<<avg_remote_pool[i];
	}

	mem_stats<<endl<<endl;

	pthread_mutex_unlock(&lock_update);
}


void print_per_sample_access_count()
{
	track<<"\n\n--------Pool wise no. of memory accesses per sample(One window consists of 10 such samples and one windows includes all remote memory access within range of 10000000 cycles)--------\n";
	track<<"\nTotal samples:"<<max_samples<<"\n\n";
	for(int i=0;i<num_mem_pools;i++)
	{
		track<<"\n-------------Pool-no"<<i<<"-------------\n";
		for(int j=0;j<max_samples;j++)
		{
			track<<"\n"<<sample_access_count_buffer[i][j];
		}
	}
}


//print the collected stats till this window_no
void print_mem_stats(int window_no)
{

	unsigned long total_accesses=0;

	for(int i=0;i<num_nodes;i++)
		total_accesses=total_accesses+count_access[i];

	if(window_no==-1)
		mem_stats<<"\n\n"<<"=========================Final_epoch=========================\n";
	else
		mem_stats<<"\n\n"<<"=========================Epoch"<<(window_no/res_window)<<"=========================\n";	

	mem_stats<<"\nTotal Memory Accesses till this epoch of simulation= "<<total_accesses;
	mem_stats<<endl;


	mem_stats<<"\n\n\t\t\tNode wise total memory accesses";
	mem_stats<<"\n--------------------------------------------------------------\n";
	for(int j=0;j<num_nodes;j++)
		mem_stats<<"Memory accesses in NODE-"<<j+1<<" :"<<count_access[j]<<endl;


	mem_stats<<"\n\n\tMemory-Unit wise clock cycles completion until last request";
	mem_stats<<"\n--------------------------------------------------------------";
	//total cycles spent at node 'i' to simulate memory accesses
	for(int i=0;i<num_nodes;i++)
		mem_stats<<"\nTotal cycles until last request completion in Local Node Mem-"<<i+1<<" : "<<total_cycle[i];

	mem_stats<<"\n\n--------------------------------------------------------------";
	//total cycles spent at remote pool 'i' to simulate memory accesses
	for(int i=0;i<num_mem_pools;i++)
		mem_stats<<"\nCycle number until last request completion in Remote Mem Pool-"<<i+1<<" : "<<total_cycle[i+num_nodes];

	mem_stats<<endl;


//Print total accesses in each memory
	mem_stats<<"\n=========================================";
	for(int i=0;i<num_nodes;i++)
	{
		mem_stats<<"\n\t\t\t\tNode-"<<i+1;
		mem_stats<<"\n=========================================";
		mem_stats<<"\n\nLocal_Accesses-"<<(i+1)<<"\t:"<<local_access[i];
		mem_stats<<"\nRemote_Accesses-"<<(i+1)<<"\t:"<<remote_access[i];//  (count_access[i] - local_access[i]);

		mem_stats<<endl;
		for(int j=0;j<num_mem_pools;j++)
			mem_stats<<"\nTotal memory access in remote pool-"<<j+1<<" :"<<per_pool_access_count[i][j];

		mem_stats<<"\n\n";
		L[i].get_stats();

		mem_stats<<"\nPool wise page count";
		L[i].pool_wise_page_count(num_mem_pools);
		mem_stats<<"\n\n--------------------------------------------------------------";
		
	}
mem_stats<<"\n========================================================================================";
	calculate_avg_mem_access_cycle();
	
	for(int i=0;i<num_nodes;i++)
	{
		if(window_no==-1)
		{
			print_pool_wise_diff_memory_accesses_time();
			print_per_sample_access_count();
			mem_stats<<"\nLocal-Remote Mapping Enteries\n\n";
			L[i].display_mapping(mem_stats);
		}
	}
}
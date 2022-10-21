void calculate_stats_for_packets_received_at_node()
{
	for(int j=0;j<num_nodes;j++)
	{
		if(rx_packet_queue_node[j].size()>0)
		{
			for(int i=0;i<rx_packet_queue_node[j].size();i++)
			{
				int node_id=rx_packet_queue_node[j].front().mem.dest;
				int pool_id=rx_packet_queue_node[j].front().mem.source;
				
				rx_total_packets_per_node[node_id]++;
				rx_total_packets_per_pool[pool_id]++;
				rx_overall_total_packets++;

				long double start_cycle=rx_packet_queue_node[j].front().mem.memory_access_completion_cycle;
				long double end_cycle=rx_packet_queue_node[j].front().mem.cycle;
				rx_total_network_delay[j]=rx_total_network_delay[j] + end_cycle - start_cycle;

				rx_overall_total_network_delay=rx_overall_total_network_delay+end_cycle - start_cycle;

				long double input_port_delay=rx_packet_queue_node[j].front().out_switch_input_port -  rx_packet_queue_node[j].front().in_switch_input_port;
				rx_total_delay_input_ports[node_id]=rx_total_delay_input_ports[node_id]+input_port_delay;

				long double output_port_delay=rx_packet_queue_node[j].front().out_switch_output_port -  rx_packet_queue_node[j].front().in_switch_output_port;
				rx_total_delay_output_ports[pool_id]=rx_total_delay_output_ports[pool_id]+output_port_delay;

				long double switch_delay=rx_packet_queue_node[j].front().out_switch_output_port - rx_packet_queue_node[j].front().in_switch_input_port;
				rx_overall_total_switch_delay=rx_overall_total_switch_delay+switch_delay;

				long double miss_cycle=rx_packet_queue_node[j].front().mem.miss_cycle_num;
				total_node_remote[node_id]=total_node_remote[node_id]+(end_cycle - miss_cycle);
				completed_trans_node_to_remote[node_id]++;


				rx_packet_queue_node[j].pop_front();
			}
		}
	}
}


//Till memory access reach the memory pool
void calculate_and_print_average_network_delays(int window_no)
{
	if(window_no==-1)
		netstats<<"\n\n"<<"=========================Final_epoch=========================\n";
	else
		netstats<<"\n\n"<<"=========================Epoch"<<(window_no/res_window)<<"=========================\n";	

	netstats<<"\n---------Node-Wise Average Packet Delay----------\n";
	netstats<<"\n\t\tSending\t\tReceiving\t\tTotal\n";
	for(int i=0;i<num_nodes;i++)
	{
		avg_network_delay[i]=total_network_delay[i]/total_packets_per_node[i];
		rx_avg_network_delay[i]=rx_total_network_delay[i]/rx_total_packets_per_node[i];
		mem_access_avg_network_delay[i]=(avg_network_delay[i]+rx_avg_network_delay[i]);
		netstats<<"\nNode_no-"<<(i+1)<<"  : "<<avg_network_delay[i]<<"\t\t: "<<rx_avg_network_delay[i]<<"\t\t\t: "<<mem_access_avg_network_delay[i];
	}

	netstats<<"\n\n\n---------Average Packet Delay at each Input-Port----------\n";
	netstats<<"\n\t\tSending\t\tReceiving\n";
	for(int i=0;i<num_nodes;i++)
	{
		avg_delay_input_ports[i]=total_delay_input_ports[i]/total_packets_per_node[i];
		rx_avg_delay_input_ports[i]=rx_total_delay_input_ports[i]/rx_total_packets_per_node[i];
		netstats<<"\nInput-Port-"<<(i+1)<<" : "<<avg_delay_input_ports[i]<<"\t\t : "<<rx_avg_delay_input_ports[i];
	}

	netstats<<"\n\n\n---------Average Packet Delay at each Output-Port----------\n";
	netstats<<"\n\t\tSending\t\tReceiving\n";
	for(int i=0;i<num_mem_pools;i++)
	{
		avg_delay_output_ports[i]=total_delay_output_ports[i]/total_packets_per_pool[i];
		rx_avg_delay_output_ports[i]=rx_total_delay_output_ports[i]/rx_total_packets_per_pool[i];
		netstats<<"\nOutput-Port-"<<(i+1)<<" : "<<avg_delay_output_ports[i]<<"\t\t : "<<rx_avg_delay_output_ports[i];
	}

	netstats<<"\n\n\n---------Overall Network Delay for all the Packets from source to destination to source----------\n";
	{
		overall_avg_network_delay=overall_total_network_delay/overall_total_packets;
		rx_overall_avg_network_delay=rx_overall_total_network_delay/rx_overall_total_packets;
		long double total_average=overall_avg_network_delay+rx_overall_avg_network_delay;
		netstats<<"\nAverage Network Delay while transmitting= "<<overall_avg_network_delay;
		netstats<<"\nAverage Network Delay while Receiving= "<<rx_overall_avg_network_delay;
		netstats<<"\nTotal Average Network Delay= "<<total_average;
	}

	netstats<<"\n\n\n---------Overall Switch Delay for all the packets----------\n";
	{
		overall_avg_switch_delay=overall_total_switch_delay/overall_total_packets;
		rx_overall_avg_switch_delay=rx_overall_total_switch_delay/rx_overall_total_packets;
		long double total_average=overall_avg_switch_delay+rx_overall_avg_switch_delay;
		netstats<<"\nAverage Switch Delay while transmitting= "<<overall_avg_switch_delay;
		netstats<<"\nAverage Switch Delay while receiving= "<<rx_overall_avg_switch_delay;
		netstats<<"\nTotal Average Switch Delay while receiving and transmitting= "<<total_average;
	}

	netstats<<"\n\n-----------------------------------------------------------\n\n";

}


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
	
	mem_stats<<"\n========Disaggregated latency calculated by network simulation========";
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
	calculate_stats_for_packets_received_at_node();
	calculate_and_print_average_network_delays(window_no);

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
			mem_stats<<"\nLocal-Remote Mapping Enteries\n\n";
			L[i].display_mapping(mem_stats);
		}
	}

	print_pool_wise_diff_memory_accesses_time();
	print_per_sample_access_count();
	int epoch=window_no/res_window;
}
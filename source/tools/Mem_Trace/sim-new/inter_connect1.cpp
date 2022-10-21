
//implements single-side
//response packets can be implemented simultaneously (to be done)


#include<iostream>
#include<vector>
#include <stack>

using namespace std;

//cycle number for cycle-accurate interconnect simulation
uint64_t cycle_num=0;

#define packet_size 512 //bits (64-bytes)
#define rx_packet_size	1024 //bits (128-bytes)

#define nic_queue_size 128
#define input_port_queue_size 512
#define output_port_queue_size 512

#define nic_bandwidth 100 //Gbps
#define switch_bandwidth 400 //Gbps

/*int nic_trans_delay = (packet_size/(nic_bandwidth)); //nanoseconds //1ns=1cycle
int switch_trans_delay = (packet_size/(switch_bandwidth)); //nanoseconds //1ns=1cycle*/

int nic_trans_delay=0;
int switch_trans_delay=0;

#define nic_proc_delay 10
#define switch_proc_delay 10
#define prop_delay 5 //max 1-meter inside rack 
#define switching_delay 4


//switch input port arbitrator (transmitting/receiving)
static int tx_arbitrator=0;
static int rx_arbitrator=0;

//arbitrator for virtual queues at input port(transmitting/receiving)
int static tx_vq_arbitrator_input_queue[num_nodes]={0};
int static rx_vq_arbitrator_input_queue[num_mem_pools]={0};


//unlimited queue at node after link_layer to collect all packets in time-ordered manner from all CPUs 
//deque <packet> packet_queue_node[num_nodes];

//NIC queue at node with size nic_queue_size
deque <packet>	tx_nic_queue_node[num_nodes];
deque <packet>	rx_nic_queue_node[num_nodes];

//virtual queues at switch
deque <packet>	tx_input_port_queue[num_nodes][num_mem_pools];
deque <packet>	rx_input_port_queue[num_mem_pools][num_nodes];

//output queues at switch
deque <packet>  tx_output_port_queue[num_mem_pools];
deque <packet>  rx_output_port_queue[num_nodes];

////NIC queue at pool with size nic_queue_size
deque <packet>	tx_nic_queue_pool[num_mem_pools];
deque <packet>	rx_nic_queue_pool[num_mem_pools];

//unlimited queue at pool after NIC
//deque <packet> packet_queue_pool[num_mem_pools];


//void to_trans_layer(memory_stream m);
void to_network_layer(packet, deque <packet>*);
void to_link_layer(packet, deque <packet>*);
void simulate_network(bool last);
void simulate_network_reverse(bool last);
void send_to_source_nic_queue(deque <packet>*, deque <packet>*, int);
void source_to_switch_input_port(deque <packet>*, deque <packet>*, int , int);
void switch_input_port_to_output_port(deque <packet>*, deque <packet>*, int, int, int &, int *);
void switch_to_dest_nic_queue(deque <packet>*, deque <packet>*, int);
void send_to_dest_collector_queue(deque <packet>*, deque <packet>*, int);

void to_trans_layer(memory_stream m, deque <packet> *packet_queue)
{
	packet temp;
	temp.is_transmitting=0;
	temp.is_processing=0;
	temp.in_nic_source=-1;
	temp.out_nic_source=-1;
	temp.in_switch_input_port=-1;
	temp.out_switch_input_port=-1;
	temp.in_switch_output_port=-1;
	temp.out_switch_output_port=-1;
	temp.in_nic_dest=-1;
	temp.out_nic_dest=-1;
	temp.mem=m;
	temp.mem.cycle=m.cycle+5;
	
	to_network_layer(temp, packet_queue);
}

void to_network_layer(packet p, deque <packet>* packet_queue)
{
	p.mem.cycle=p.mem.cycle+5;
	to_link_layer(p, packet_queue);
}

void to_link_layer(packet p, deque <packet>* packet_queue)
{
	p.mem.cycle=p.mem.cycle+5;
	packet_queue[p.mem.source].push_back(p);
}

void send_to_source_nic_queue(deque <packet> *packet_queue_source, deque <packet> *nic_queue_source, int source_count)
{
	for(int i=0;i<source_count;i++)
	{
		if(packet_queue_source[i].size()>0)
		{
			if(packet_queue_source[i].front().mem.cycle==cycle_num && nic_queue_source[i].size()<nic_queue_size)
			{
				if(packet_queue_source[i].front().is_processing>0)
				{
					if(packet_queue_source[i].front().is_processing==nic_proc_delay)
					{
						packet_queue_source[i].front().in_nic_source=packet_queue_source[i].front().mem.cycle;
						packet_queue_source[i].front().mem.cycle=packet_queue_source[i].front().mem.cycle+nic_proc_delay;
						packet_queue_source[i].front().is_processing=0;
						nic_queue_source[i].push_back(packet_queue_source[i].front());
						packet_queue_source[i].pop_front();
						for(int j=0;j<packet_queue_source[i].size();j++)
						{
							if(packet_queue_source[i].at(j).mem.cycle==cycle_num)
								packet_queue_source[i].at(j).mem.cycle++;
						}
					}
					else if(packet_queue_source[i].front().is_processing<nic_proc_delay)
					{
						packet_queue_source[i].front().is_processing++;	
						for(int j=0;j<packet_queue_source[i].size();j++)
						{
							if(packet_queue_source[i].at(j).mem.cycle==cycle_num)
								packet_queue_source[i].at(j).mem.cycle++;
						}
					}
				}
				else if(packet_queue_source[i].front().is_processing==0)
				{
					packet_queue_source[i].front().is_processing++;	
					for(int j=0;j<packet_queue_source[i].size();j++)
					{
						if(packet_queue_source[i].at(j).mem.cycle==cycle_num)
							packet_queue_source[i].at(j).mem.cycle++;
					}

				}
			}
			if (nic_queue_source[i].size()==nic_queue_size)
			{
				for(int j=0;j<packet_queue_source[i].size();j++)
				{
					if(packet_queue_source[i].at(j).mem.cycle==cycle_num)
						packet_queue_source[i].at(j).mem.cycle++;
				}
			}
		}
	}
}

//check if space at input port queue
template <size_t rows, size_t cols>
bool virtual_queue_has_space(int port_no, int dest_count, deque <packet> (&input_port_queue)[rows][cols])
{
	int size=0;

	for(int i=0;i<dest_count;i++)
	{
		size=size + input_port_queue[port_no][i].size();
	}

	if(size<input_port_queue_size)
		return true;
	else if(size==input_port_queue_size)
		return false;
} 

template <size_t rows, size_t cols>
void source_to_switch_input_port(deque <packet> *nic_queue_source, deque <packet> (&input_port_queue)[rows][cols], int source_count, int dest_count)
{
	for(int i=0;i<source_count;i++)
	{
		//if space available at switch input port, select top packet from the node nic queue and transmit to switch.
		//next packet will wait until current packet is transmitted, appropriate cycles added to all waiting packets
		//propagation delay inside RACK is 5ns (max 1 meter assumed) 
		if(nic_queue_source[i].size()>0)
		{
			if(nic_queue_source[i].front().mem.cycle == cycle_num && virtual_queue_has_space(i, dest_count,input_port_queue))
			{
				if(nic_queue_source[i].front().is_transmitting>0)
				{
					if((nic_queue_source[i].front().is_transmitting == nic_trans_delay))
					{
						nic_queue_source[i].front().out_nic_source=nic_queue_source[i].front().mem.cycle;
						nic_queue_source[i].front().in_switch_input_port=nic_queue_source[i].front().mem.cycle;
						nic_queue_source[i].front().mem.cycle=nic_queue_source[i].front().mem.cycle+prop_delay+switch_trans_delay+switch_proc_delay;
						int vq_no=nic_queue_source[i].front().mem.dest;
						nic_queue_source[i].front().is_transmitting=0;
						input_port_queue[i][vq_no].push_back(nic_queue_source[i].front());
						nic_queue_source[i].pop_front();
						for(int j=0;j<nic_queue_source[i].size();j++)
						{
							if(nic_queue_source[i].at(j).mem.cycle==cycle_num)
								nic_queue_source[i].at(j).mem.cycle++;
						}
					}
					else if(nic_queue_source[i].front().is_transmitting < nic_trans_delay)
					{
						nic_queue_source[i].front().is_transmitting++;
						for(int j=0;j<nic_queue_source[i].size();j++)
						{
							if(nic_queue_source[i].at(j).mem.cycle==cycle_num)
								nic_queue_source[i].at(j).mem.cycle++;
						}
					}
				}
				else if(nic_queue_source[i].front().is_transmitting==0)
				{
					nic_queue_source[i].front().is_transmitting=1;
					for(int j=0;j<nic_queue_source[i].size();j++)
					{
						if(nic_queue_source[i].at(j).mem.cycle==cycle_num)
							nic_queue_source[i].at(j).mem.cycle++;
					}
				}
			}
		}

		if(!virtual_queue_has_space(i, dest_count,input_port_queue))
		{
			for(int j=0;j<nic_queue_source[i].size();j++)
			{
				if(nic_queue_source[i].at(j).mem.cycle==cycle_num)
					nic_queue_source[i].at(j).mem.cycle++;	//increament the cycle number of all packets waiting in the queue
			}
		}
	}
}

//this will tell the maximum number of packets that can be taken from input to output buffer in one cycle
void find_free_buffers_at_output_ports(int &count, int *output_queue_status, int dest_count, deque <packet> *output_port_queue)
{
	for(int i=0;i<dest_count;i++)
	{
		if(output_port_queue[i].size()<output_port_queue_size)
		{
			output_queue_status[i]=1;
			count++;
		}
	}
}


//check if a virtual queue has any packet available 
template <size_t rows, size_t cols>
bool packet_available_at_input_queue(int source_no, int vq_num, deque <packet>(&input_port_queue)[rows][cols])
{
	if(input_port_queue[source_no][vq_num].size()>0)
	{
		if(input_port_queue[source_no][vq_num].front().mem.cycle==cycle_num)
			return true;
		else
			return false;
	}
	else
		return false;	
}

//count all the ready packets in all the virtual queues of all the ports
template <size_t rows, size_t cols>
int total_ready_packets(int source_count, int dest_count, deque <packet>(&input_port_queue)[rows][cols])
{
	int ready_packets=0;
	for(int i=0;i<source_count;i++)
	{
		for(int j=0;j<dest_count;j++)
		{
			if(input_port_queue[i][j].size()>0)
			{
				if(input_port_queue[i][j].front().mem.cycle==cycle_num)
					ready_packets++;
			}
		}
	}
	return ready_packets;
}

//use arbitrator logic for multiple input ports as well as within virtual queues inside each input port
template <size_t rows, size_t cols>
void switch_input_port_to_output_port(deque <packet>(&input_port_queue)[rows][cols], deque <packet> *output_port_queue, int source_count, int dest_count, int &arbitrator, int *vq_arbitrator_input_queue)
{

	//only those packets which are ready can be sent towards output port (max. packets are equal to num of output ports with free queue space)
	int ready_packets=total_ready_packets(source_count, dest_count, input_port_queue);

	//packets only to be sent to those outputs port queues, which have empty space
	//maintain count for free output queues
	int free_output_buffers_count=0;

	//maintain full/empty status of each output queue
	int output_queue_status[dest_count]={0};

	find_free_buffers_at_output_ports(free_output_buffers_count, output_queue_status, dest_count, output_port_queue);

	//used to stop searching loop, if all input ports are searched and not enough ready packets
	int arbitration_count=1;

	//allow multiple packets from all input ports(equal to the number of output queues having queue space), in one cylce to go to output ports.
	//if not enough ready packets in this cycle, then try in next cycle
	while(free_output_buffers_count>0 && ready_packets>0 && arbitration_count<=source_count)
	{
		//check all virtual queues(equal to num_mem_pools) for ready packet, in one cycle allow only one virtual queue to send its packet
		//use the arbitrator to search next virtual queue next time
		for(int num_vqs=0;num_vqs<dest_count;num_vqs++)
		{
			int out_port_num=vq_arbitrator_input_queue[arbitrator];

			//checking available packets in a virtual queue where the arbitrator is

			if(packet_available_at_input_queue(arbitrator,out_port_num, input_port_queue))
			{
				//if same output port queue(as virtual queue number) space is also available, transmit the packet to output queue with switching delay 
				if(output_queue_status[out_port_num]==1)
				{
					input_port_queue[arbitrator][out_port_num].front().out_switch_input_port=input_port_queue[arbitrator][out_port_num].front().mem.cycle;
					input_port_queue[arbitrator][out_port_num].front().mem.cycle = input_port_queue[arbitrator][out_port_num].front().mem.cycle+switching_delay;
					input_port_queue[arbitrator][out_port_num].front().in_switch_output_port=input_port_queue[arbitrator][out_port_num].front().mem.cycle;
					output_port_queue[out_port_num].push_back(input_port_queue[arbitrator][out_port_num].front());
					input_port_queue[arbitrator][out_port_num].pop_front();
					arbitrator++;	//only one packet allowed per input port, so increamented arbitrator
					arbitration_count++;
					if(arbitrator==source_count)
						arbitrator=0;
					free_output_buffers_count--;
					ready_packets--;
					output_queue_status[out_port_num]=0;	//cannot send more packets to same output queue 
					break;	//no need to search furthur, as one packet already transmitted (max is 1 per port in a cycle)
				}
				//if same output queue doesn not have space, search in next virtual queue with different output port queue
				else
				{
					vq_arbitrator_input_queue[arbitrator]++;
					if(vq_arbitrator_input_queue[arbitrator]==dest_count)
					{
						vq_arbitrator_input_queue[arbitrator]=0;
					}

				}
			}
			//if ready packet not available at any virtual queue, increase the arbitrator and search next virtual queue (upto max no. of virtual queues)
			else
			{
				vq_arbitrator_input_queue[arbitrator]++;
				if(vq_arbitrator_input_queue[arbitrator]==dest_count)
				{
					vq_arbitrator_input_queue[arbitrator]=0;
				}
			}
		}

		//if loop didn't break because there was no packet which was able to get transmitted in this cycle, then search in next input-port queue
		//increament the arbitrator
		arbitrator++; 
		arbitration_count++;
		if(arbitrator==source_count)
			arbitrator=0;
	}

	for(int i=0;i<source_count;i++)
	{
		for(int j=0;j<dest_count;j++)
		{
			for(int k=0;k<input_port_queue[i][j].size();k++)
			{
				if(input_port_queue[i][j].front().mem.cycle==cycle_num)
				{
					input_port_queue[i][j].front().mem.cycle++;	
				}
			}
		}
	}

}

//send all ready packets from output port to mem_pool nic queue
void switch_to_dest_nic_queue(deque <packet> *output_port_queue, deque <packet> *nic_queue_dest, int dest_count)
{
	for(int i=0;i<dest_count;i++)
	{
		//if space available at nic at mem_pool, select top packet from the output port queue and transmit to pool nic.
		//next packet will wait until current packet is transmitted, appropriate cycles added to all waiting packets
		//propagation delay inside RACK is 5ns (max 1 meter assumed)
		if(output_port_queue[i].size()>0)
		{
			if(nic_queue_dest[i].size()<nic_queue_size && output_port_queue[i].front().mem.cycle==cycle_num)
			{
				if(output_port_queue[i].front().is_transmitting>0)
				{
					if (output_port_queue[i].front().is_transmitting == switch_trans_delay)
					{
						output_port_queue[i].front().out_switch_output_port=output_port_queue[i].front().mem.cycle;
						output_port_queue[i].front().mem.cycle=output_port_queue[i].front().mem.cycle+prop_delay+nic_trans_delay;
						output_port_queue[i].front().in_nic_dest=output_port_queue[i].front().mem.cycle;
						output_port_queue[i].front().is_transmitting=0;
						nic_queue_dest[i].push_back(output_port_queue[i].front());
						
#ifdef testing
//cout<<"\n cycle_num "<<cycle_num<<" start_cycle"<<output_port_queue[i].front().mem.miss_cycle_num<<" packet reach at remote pool nic "<<i<<" packet-id: "<<output_port_queue[i].front().mem.id<<" node "<<output_port_queue[i].front().mem.source;
#endif
						output_port_queue[i].pop_front();
						for(int j=0;j<output_port_queue[i].size();j++)
						{
							if(output_port_queue[i].at(j).mem.cycle==cycle_num)
								output_port_queue[i].at(j).mem.cycle++;
						}
					}
					else if(output_port_queue[i].front().is_transmitting < switch_trans_delay)
					{
						output_port_queue[i].front().is_transmitting++;
						for(int j=0;j<output_port_queue[i].size();j++)
						{
							if(output_port_queue[i].at(j).mem.cycle==cycle_num)
								output_port_queue[i].at(j).mem.cycle++;
						}
					}
				}
				else if(output_port_queue[i].front().is_transmitting==0)
				{
					output_port_queue[i].front().is_transmitting++;
					for(int j=0;j<output_port_queue[i].size();j++)
					{
						if(output_port_queue[i].at(j).mem.cycle==cycle_num)
							output_port_queue[i].at(j).mem.cycle++;
					}
				}

			}
		}

		if(nic_queue_dest[i].size()==nic_queue_size)
		{
			for(int j=0;j<output_port_queue[i].size();j++)
			{
				if(output_port_queue[i].at(j).mem.cycle==cycle_num)
					output_port_queue[i].at(j).mem.cycle++;
			}
		}
	}
}

void send_to_dest_collector_queue(deque <packet>*nic_queue_dest, deque <packet>*packet_queue_dest, int dest_count)
{
	for(int i=0;i<dest_count;i++)
	{
		if(nic_queue_dest[i].size()>0)
		{
			if(nic_queue_dest[i].front().mem.cycle==cycle_num)
			{
				if(nic_queue_dest[i].front().is_processing>0)
				{
					if(nic_queue_dest[i].front().is_processing==nic_proc_delay)
					{
						nic_queue_dest[i].front().mem.cycle=nic_queue_dest[i].front().mem.cycle;
						nic_queue_dest[i].front().is_processing=0;
						packet_queue_dest[i].push_back(nic_queue_dest[i].front());
						nic_queue_dest[i].front().out_nic_dest=nic_queue_dest[i].front().mem.cycle;
#ifdef testing
		cout<<"\n cycle_num "<<cycle_num<<" start_cycle"<<packet_queue_dest[i].front().mem.miss_cycle_num<<" packet reach at pool nic "<<i<<" packet-id: "<<packet_queue_dest[i].front().mem.id<<" node "<<packet_queue_dest[i].front().mem.source;
#endif			
						nic_queue_dest[i].pop_front();
						for(int j=0;j<nic_queue_dest[i].size();j++)
						{
							if(nic_queue_dest[i].at(j).mem.cycle==cycle_num)
								nic_queue_dest[i].at(j).mem.cycle++;
						}
					}
					else if(nic_queue_dest[i].front().is_processing < nic_proc_delay)
					{
						nic_queue_dest[i].front().is_processing++;
						for(int j=0;j<nic_queue_dest[i].size();j++)
						{
							if(nic_queue_dest[i].at(j).mem.cycle==cycle_num)
								nic_queue_dest[i].at(j).mem.cycle++;
						}
					}
				}
				else if(nic_queue_dest[i].front().is_processing==0)
				{
					nic_queue_dest[i].front().is_processing++;
					for(int j=0;j<nic_queue_dest[i].size();j++)
					{
						if(nic_queue_dest[i].at(j).mem.cycle==cycle_num)
							nic_queue_dest[i].at(j).mem.cycle++;
					}
				}
			}
		}
	}
}

#ifdef testing

#else


uint64_t last_run_cycle_count;
void simulate_network(bool last)
{

	nic_trans_delay=(packet_size/nic_bandwidth);
	switch_trans_delay=(packet_size/switch_bandwidth);

	cout<<nic_trans_delay<<"\t";
	cout<<switch_trans_delay;
	cin.get();
	uint64_t cycles_to_run=window_cycles*epoch_num;

	if(last==true)
	{
		cycles_to_run=cycles_to_run+1000000;
	}

	last_run_cycle_count=cycles_to_run-cycle_num;
	while(cycle_num<=cycles_to_run)
	{
				if(cycle_num%100000==0)
		{
			cout<<"\nCycles completed:"<<cycle_num;
		}
		send_to_source_nic_queue(packet_queue_node, tx_nic_queue_node, num_nodes);

		source_to_switch_input_port(tx_nic_queue_node, tx_input_port_queue, num_nodes, num_mem_pools);

		switch_input_port_to_output_port(tx_input_port_queue,tx_output_port_queue, num_nodes, num_mem_pools, tx_arbitrator, tx_vq_arbitrator_input_queue);

		switch_to_dest_nic_queue(tx_output_port_queue, tx_nic_queue_pool, num_mem_pools);

		send_to_dest_collector_queue(tx_nic_queue_pool, packet_queue_pool, num_mem_pools);

	/*	for(int i=0;i<num_nodes;i++)
		if(rx_nic_queue_node[i].size()>0)
			{cout<<"abc"<<cycle_num;cin.get();

			cout<<"\nstart-cycle :"<<start_cycle<<" end_cycle :"<<end_cycle<<" out_switch_inp_port :"<<rx_nic_queue_node[j].front().out_switch_input_port	\
		 <<" in_switch_inp_port :"<<rx_nic_queue_node[j].front().in_switch_input_port	\
		 <<" out_switch_out_port :"<<rx_nic_queue_node[j].front().out_switch_output_port	\
		 <<" in_switch_out_port :"<<rx_nic_queue_node[j].front().in_switch_output_port;
		 cin.get();}*/

		cycle_num++;

	}
cycle_num=cycle_num-last_run_cycle_count;
	cout<<"\none-side network_simulation_done this epoch, epoch-num: "<<epoch_num;
}


void simulate_network_reverse(bool last)
{
	nic_trans_delay=(rx_packet_size/nic_bandwidth);
	switch_trans_delay=(rx_packet_size/switch_bandwidth);

	cout<<nic_trans_delay;
	cout<<switch_trans_delay;
	cin.get();
//	pthread_t local_mem_threads[num_nodes];
			if(cycle_num%100000==0)
		{
			cout<<"\nCycles completed:"<<cycle_num;
		}

	uint64_t cycles_to_run=window_cycles*epoch_num;


	if(last==true)
	{
		cycles_to_run=cycles_to_run+1000000;
	}
	
	while(cycle_num<=cycles_to_run)
	{
		send_to_source_nic_queue(rx_packet_queue_pool, rx_nic_queue_pool, num_mem_pools);

		source_to_switch_input_port(rx_nic_queue_pool, rx_input_port_queue, num_mem_pools, num_nodes);

		switch_input_port_to_output_port(rx_input_port_queue,rx_output_port_queue, num_mem_pools, num_nodes, rx_arbitrator, rx_vq_arbitrator_input_queue);

		switch_to_dest_nic_queue(rx_output_port_queue, rx_nic_queue_node, num_nodes);

		send_to_dest_collector_queue(rx_nic_queue_node, rx_packet_queue_node, num_nodes);

	/*	for(int i=0;i<num_nodes;i++)
		if(rx_nic_queue_node[i].size()>0)
			{cout<<"abc"<<cycle_num;cin.get();

			cout<<"\nstart-cycle :"<<start_cycle<<" end_cycle :"<<end_cycle<<" out_switch_inp_port :"<<rx_nic_queue_node[j].front().out_switch_input_port	\
		 <<" in_switch_inp_port :"<<rx_nic_queue_node[j].front().in_switch_input_port	\
		 <<" out_switch_out_port :"<<rx_nic_queue_node[j].front().out_switch_output_port	\
		 <<" in_switch_out_port :"<<rx_nic_queue_node[j].front().in_switch_output_port;
		 cin.get();}*/
		cycle_num++;
	}

	cout<<"\nreverse network_simulation_done this epoch, epoch-num: "<<epoch_num;
}

#endif
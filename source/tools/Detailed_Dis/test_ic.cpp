#include <iostream>
#include <queue>
#include<fstream>
#include<math.h>
#include<pthread.h>

#define num_nodes 8
#define num_mem_pools 4

pthread_mutex_t lock_update;
pthread_mutex_t lock_queue;


using namespace std;

struct remote_memory_access
{
	uint64_t mem_access_addr;
	int source;
	int dest;
	uint64_t cycle;
	uint64_t miss_cycle_num;
	uint64_t memory_access_completion_cycle;
	uint64_t trans_id;
	bool isWrite;
};

//memory request/response to node/pool from pool/node as a network packet
struct packet
{
	remote_memory_access mem;
	int is_transmitting;
	int is_processing;
	int64_t in_nic_source;
	int64_t out_nic_source;
	int64_t in_switch_input_port;
	int64_t out_switch_input_port;
	int64_t in_switch_output_port;
	int64_t out_switch_output_port;
	int64_t in_nic_dest;
	int64_t out_nic_dest;
};

deque <packet> packet_queue_node[num_nodes];	//transmitting queue
void to_trans_layer(remote_memory_access,deque <packet>*);
int packets=0;
void fill_packet_queue_node(){

	uint64_t vaddr = 0x7fa507f75e98;
	int k=0;
	int trans_id = 1;
	while(k<100000){
		packets++;
		remote_memory_access temp;
		temp.mem_access_addr = vaddr;
		temp.source = rand()%num_nodes;
		temp.dest= rand()%num_mem_pools;
		temp.cycle=k+rand()%5;
		temp.miss_cycle_num=temp.cycle;
		temp.trans_id=trans_id++;
		k+=5;
		to_trans_layer(temp,packet_queue_node);
	}
}

deque <packet> rx_packet_queue_pool[num_mem_pools];	//receiving queue
vector<remote_memory_access> memory_completion_queue[num_nodes];

ofstream testing;
int count2=0;
#include "inter_connect.cpp"

int count1=0;
int main()
{
	testing.open("testing.txt");
	fill_packet_queue_node();
	while(cycle_num<100000)
	{
		simulate_network();
		for(int i=0;i<num_mem_pools;i++)
		{
			while(!packet_queue_pool[i].empty())
			{
				count1++;
				  int temp = packet_queue_pool[i].front().mem.source;
				  int temp1 = packet_queue_pool[i].front().mem.dest;
				//  if(packet_queue_pool[i].front().mem.dest>num_mem_pools)
				//  {	
				//  	cout<<"abc";
				//  }

				  packet_queue_pool[i].front().mem.source=packet_queue_pool[i].front().mem.dest;
				  packet_queue_pool[i].front().mem.dest=temp;
				//cout<<"\nsrc "<<packet_queue_pool[i].front().mem.source<<" dest "<<packet_queue_pool[i].front().mem.dest;
				//packet_queue_pool[i].front().mem.cycle=packet_queue_pool[i].front().mem.cycle+50;
				rx_packet_queue_pool[i].push_back(packet_queue_pool[i].front());
				packet_queue_pool[i].pop_front();
			}
		}
		simulate_network_reverse();
		cycle_num++;
		//cout<<"\n"<<cycle_num;
	}

	for(int i=0;i<num_mem_pools;i++)
	{
		{
			cout<<"\nrxpktpool"<<i<<" "<<rx_nic_queue_pool[i].size()<<" cycle "<<cycle_num;
		}
	}
	cout<<"\nsent: "<<packets<<"reached pool: "<<count1<<" reached node back:"<<count2;
	return 0;
}
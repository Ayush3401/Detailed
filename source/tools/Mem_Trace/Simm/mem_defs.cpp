#include"MMU.cpp"
#include"mmap.cpp"
#include"DRAMSim2/DRAMSim.h"
#include<algorithm>
#include<string>
#include<queue>
#include<sys/stat.h>
#include<unistd.h>

ofstream track;

static int epoch_num=0;


#define window_cycles 10000000
#define max_windows	160
#define res_window	1

#define num_mem_pools	1
#define num_nodes		1
//#define dis_latency		1000

#define local_mem_size	4
#define remote_mem_size 32

#define local_DRAM_size	4096
#define remote_DRAM_size (remote_mem_size * pow(2.0,10.0))

pthread_mutex_t lock_update;

using namespace DRAMSim;

long int tx_dis_latency=116;
long int rx_dis_latency=131;

//used to store average local memory access cycle time for different nodes
long double avg_node_local[num_nodes];
//used to store average remote memory access cycle time for different nodes
long double avg_node_remote[num_nodes];
//used to store total average memory access (local/remote) cycle time for different nodes
long double avg_node_overall[num_nodes];
//used to store average memory access cycle time for different remote memory pools
long double avg_remote_pool[num_mem_pools];

//total number of memory accesses at each memory unit 
unsigned long _total_access_count[num_mem_pools];

//total number of memory accesses pending at each memory unit 
unsigned long _pending_count[num_mem_pools]={0};

//address spaces for different type of memory units
local_addr_space L[num_nodes];
remote_addr_space R[num_mem_pools];

//total number of completed memory accesses at each memory unit 
uint64_t completed_trans_local[num_nodes];
uint64_t completed_trans_node_to_remote[num_nodes];
uint64_t completed_trans_remote[num_mem_pools];

//total cycle count for all memory accesses at each memory unit
uint64_t total_node_local[num_nodes];
uint64_t total_remote_pool[num_mem_pools];
uint64_t total_node_remote[num_nodes];

uint64_t total=0;
uint64_t avg=0;
uint64_t completed=0;


class some_object
{
	public: 
		void read_complete(int, uint64_t, uint64_t, uint64_t, int, unsigned, uint64_t, uint64_t);
		void write_complete(int, uint64_t, uint64_t, uint64_t, int, unsigned, uint64_t, uint64_t);
		int add_one_and_run(DRAMSim::MultiChannelMemorySystem *mem, uint64_t addr, bool isWrite, uint64_t tid, uint64_t start_cycle, uint64_t miss_cycle, int nid);
};

//used to store the total cycle number used at each memory unit
uint64_t total_cycle[num_nodes+num_mem_pools];

//used for just printing "simulation going on" message rather than memory access complete message 
uint64_t x=0;


//to maintain count of memory accesses with different access time on different memory pools
uint64_t U_100ns[num_mem_pools];
uint64_t B_100_300ns[num_mem_pools];
uint64_t B_300_500ns[num_mem_pools];
uint64_t B_500_700ns[num_mem_pools];
uint64_t B_700_1000ns[num_mem_pools];
uint64_t G_1000ns[num_mem_pools];

//function to update end-cycle of each memory request once a transaction is completed
void update(int node_id, int mem_id, uint64_t cycle, uint64_t tid, uint64_t start_cycle, uint64_t miss_cycle)
{
//	uint64_t size=0;
	int i=mem_id-1;

	total_cycle[i]=cycle;
	if(i<num_nodes)
	{
//		invalid<<"\nmem-id\t:"<<mem_id<<" tid\t:"<<tid;
		//calcuates memory transaction time and total cycles till now at memory unit with-in local node
		completed_trans_local[i]++;
		total_node_local[i]=total_node_local[i]+(cycle - start_cycle);
	}
	if(i>=num_nodes && i<(num_nodes+num_mem_pools))
	{
		int remote_pool=i-num_nodes;
		//calcuates memory transaction time and total cycles till now at memory unit at remote pool
		completed_trans_remote[remote_pool]++;
		total_remote_pool[remote_pool]=total_remote_pool[remote_pool]+(cycle - start_cycle);	
		
		_pending_count[remote_pool]=_total_access_count[remote_pool] + R[remote_pool].get_count() - completed_trans_remote[remote_pool];


	//increamenting the count of memory access with particular cycle number per memory pool 
		int diff=cycle - start_cycle;
		if(diff<100)
		{
			U_100ns[i-num_nodes]++;
		}
		else if(diff>100 && diff<300)
		{
			B_100_300ns[i-num_nodes]++;
		}
		else if(diff>300 && diff<500)
		{
			B_300_500ns[i-num_nodes]++;
		}
		else if(diff>500 && diff<700)
		{
			B_500_700ns[i-num_nodes]++;
		}
		else if(diff>700 && diff<1000)
		{
			B_700_1000ns[i-num_nodes]++;
		}
		else if(diff>1000)
		{
			G_1000ns[i-num_nodes]++;
		}


		//track<<"\nmem-id:"<<(i-num_nodes)<<"  tid:"<<tid<<"  sc:"<<start_cycle<<"  end-c"<<cycle<<"  diff:"<<(cycle - start_cycle)<<"  pending_count"<<_pending_count[i-num_nodes];


		//updates end-cycle for memory units at remote pool, with the knowldege of node-no whose 
		//transaction this pool is serving for. (this stat is managed in array node_to_remote_memory_cycle_time)

		//remote-local-map is used to know the node-id and trans-id of a node for which 
		//this remote pool had completed this memory request

		total_node_remote[node_id]=total_node_remote[node_id]+(cycle - miss_cycle) + rx_dis_latency; //reverse dis_latency
		completed_trans_node_to_remote[node_id]++;
	}
}

/* callback functors */
void some_object::read_complete(int nid, uint64_t tid, uint64_t start_cycle, uint64_t miss_cycle, int mem_id, unsigned id, uint64_t address, uint64_t clock_cycle)
{
	x++;
	if(x%10000==0)
		cout<<".";
	if(x%100000==0)
		cout<<"\nSimulation Going On";
	pthread_mutex_lock(&lock_update);
	update(nid,mem_id,clock_cycle,tid,start_cycle,miss_cycle);
	pthread_mutex_unlock(&lock_update);
	//printf("Mem_ID: %d [Callback] read complete: %d 0x%lx cycle=%lu\n", mem_id, id, address, clock_cycle);
}

void some_object::write_complete(int nid, uint64_t tid, uint64_t start_cycle, uint64_t miss_cycle, int mem_id, unsigned id, uint64_t address, uint64_t clock_cycle)
{
	x++;
	if(x%10000==0)
		cout<<".";
	if(x%100000==0)
		cout<<"\nSimulation Going On";
	pthread_mutex_lock(&lock_update);
	update(nid,mem_id,clock_cycle,tid,start_cycle,miss_cycle);
	pthread_mutex_unlock(&lock_update);
	//printf("Mem_ID: %d [Callback] write complete: %d 0x%lx cycle=%lu\n", mem_id, id, address, clock_cycle);
}

/* FIXME: this may be broken, currently */
void power_callback(double a, double b, double c, double d)
{
	//printf("power callback: %0.3f, %0.3f, %0.3f, %0.3f\n",a,b,c,d);
}

int some_object::add_one_and_run(MultiChannelMemorySystem *mem, uint64_t addr, bool isWrite, uint64_t tid, uint64_t start_cycle, uint64_t miss_cycle, int nid)
{
	/* create a transaction and add it */
	mem->addTransaction(isWrite, addr, nid, tid, start_cycle,miss_cycle);
	return 0;
}
						//One page-table for each process
pgd _pgd[num_nodes*3];	//a max of 3-processes assumed per node, should be increased 
						//when more processes are simulated

//used to store total number of accesses in local nodes and remote pools
unsigned long long local_access[num_nodes];
unsigned long long remote_access[num_nodes];
unsigned long long count_access[num_nodes];

//used to store each nodes total memory access count at each remote pool
unsigned long per_pool_access_count[num_nodes][num_mem_pools];

some_object obj;

TransactionCompleteCB *read_cb= new Callback<some_object, void, int, uint64_t, uint64_t, uint64_t, int, unsigned, uint64_t, uint64_t>(&obj, &some_object::read_complete);
TransactionCompleteCB *write_cb= new Callback<some_object, void, int, uint64_t, uint64_t, uint64_t, int, unsigned, uint64_t, uint64_t>(&obj, &some_object::write_complete);

/* Declare DRAMs memory to simulate */
MultiChannelMemorySystem *local_mem[num_nodes];
MultiChannelMemorySystem *remote_mem[num_mem_pools];

struct Trace
{
	int procid;
    int threadid;
    unsigned long long addr;
    char r;
    unsigned long long cycle;
};

struct memory_stream
{
	int source;
	int procid;
    int threadid;
    int dest;
    unsigned long long addr;
    char r;
    unsigned long long cycle;
    unsigned long long miss_cycle_num;
    uint64_t trans_id;
};

vector <memory_stream> rem_mem_stream;

void declare_memory_variables(string dir)
{
//initializing stat variables
	
	//node wise number of local, remote and total memory access 
	for(int i=0;i<num_nodes;i++)
	{
		local_access[i]=0;
		count_access[i]=0;
		remote_access[i]=0;

		total_node_local[i]=0;
		avg_node_local[i]=0;
		completed_trans_local[i]=0;

		total_node_remote[i]=0;
		avg_node_remote[i]=0;
		completed_trans_node_to_remote[i]=0;
	}

	for(int i=0;i<num_mem_pools;i++)
	{
		total_remote_pool[i]=0;
		avg_remote_pool[i]=0;
		completed_trans_remote[i]=0;
		_total_access_count[i]=0;
		_pending_count[i]=0;

		U_100ns[i]=0;
		B_100_300ns[i]=0;
		B_300_500ns[i]=0;
		B_500_700ns[i]=0;
		B_700_1000ns[i]=0;
		G_1000ns[i]=0;
	}

	for(int i=0;i<num_nodes;i++)
	{
		for(int j=0;j<num_mem_pools;j++)
		{
			per_pool_access_count[i][j]=0;
		}
	}

	for(int i=0;i<(num_nodes+num_mem_pools);i++)
		total_cycle[i]=0;

 	for(int i=0;i<num_nodes;i++)	//add local memory at each ndoe
 		L[i].add_local_memory(local_mem_size,i);//1,i);//0.0000038147,i);//(0.03125,i);

 	for(int i=0;i<num_mem_pools;i++)	//add remote memory pools
		R[i].add_remote_memory_pool(remote_mem_size,i);

	for(int i=0;i<num_nodes;i++)
	{
		local_mem[i]= getMemorySystemInstance(i+1, "ini/DDR4_x16_2400_1.ini", "system.ini", "./DRAMSim2", "abc", local_DRAM_size);
		local_mem[i]->RegisterCallbacks(read_cb,write_cb,power_callback);	//DRAM simulator function

//		local_mem[i]->setCPUClockSpeed(3601440555);
	}	

	for(int i=0;i<num_mem_pools;i++)
	{
		remote_mem[i]= getMemorySystemInstance(i+num_nodes+1, "ini/DDR4_x16_2400.ini", "system.ini", "./DRAMSim2", "abc", remote_DRAM_size);
		remote_mem[i]->RegisterCallbacks(read_cb,write_cb,power_callback);	//DRAM simulator function
		
//		remote_mem[i]->setCPUClockSpeed(3601440555);
	}

	string inv,tra,ou,mem;
	inv=dir+"/memory_access_completion_log.trc";
	tra=dir+"/Extra_stats.log";
	ou=dir+"/pool_select_trace.trc";
	mem=dir+"/mem_stats";

	cout<<inv<<endl;
	cout<<tra<<endl;
	cout<<ou<<endl;
	cout<<mem<<endl;

	const char * dirr=dir.c_str();

	mkdir(dirr,0777);
	
	const char * inv1=inv.c_str();
	invalid.open(inv1);
	invalid<<"\nLog Turned Off, Un-comment log statements in mem_defs.h read and write complete functions to turn it on";
	const char * tra1=tra.c_str();
	track.open(tra1);
	const char * ou1=ou.c_str();
	out.open(ou1);
	const char * mem1=mem.c_str();
	mem_stats.open(mem1);
	cin.get();

}

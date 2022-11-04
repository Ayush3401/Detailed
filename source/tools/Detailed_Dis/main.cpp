#include <iostream>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <queue>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>
using namespace std;
#define migration 10
uint64_t common_clock = 0;
#include "mem_defs.cpp"
const char *file2[num_nodes];
#include "allocator.cpp"
#include "inter_connect.cpp"
#include "branch_predictor.cpp"
#include "reverse_map.cpp"
#include "TLB_Cache.cpp"
ofstream inst_queu_out;
#include "OOO_core.cpp"

#define simulation_time 1000000000

#define Result_cycle 100000

// results_directory
string dir;

void print_stats(int);

// number of instructions to be read from a instruction stream sent by pintool in single tracefile
// once these instructions are executed, pintool will create new file and simulator will read the stream again
#define max_ins 1000000

pthread_barrier_t b;
pthread_mutex_t lock;

// node-level single instruction stream from PINTOOL
deque<INST> inst_stream[num_nodes];

uint64_t INS_id[num_nodes][core_count];

// convert per node instruction stream coming from Pintool to per core at each node
void Node_to_Core_Ins_Stream(int node)
{
	int core = (inst_stream[node].front().threadid) % core_count;
	int combined_count = 0;
	for (int i = 0; i < core_count; i++)
	{
		combined_count += core_inst_stream[node][i].size();
	}
	while (combined_count < 5000 * core_count && !inst_stream[node].empty())
	{
		combined_count++;
		inst_stream[node].front().ins_id = INS_id[node][core];
		INS_id[node][core]++;
		core_inst_stream[node][core].push(inst_stream[node].front());
		inst_stream[node].pop_front();
		core = (inst_stream[node].front().threadid) % core_count;
	}
}

std::ofstream ResultsFile;

// keep track of core-wise total instruction fetched on each node
uint64_t num_ins_fetched[num_nodes][core_count] = {0};

// one handler for each node, it will fetch the instruction stream of a workload and will simulate an OOO CPU, including caches and memory subsytem
void *node_stream_handler(void *node)
{

	inst_queu_out.open("testing.txt");
	// Data structure for accessing instruction stream (in shared memory) of a benchmark that is produced by the Pintool
	key_t ShmKEY1, ShmKEY3;
	int ShmID1, ShmID3;

	uint64_t *num_ins;
	int *main_start;

	long id = (long)(node);
	int nodeid = (int)id;
	std::ifstream TraceFile;
	std::ofstream page_migration;
	string in;
	std::ostringstream ss, tf;
	ss << nodeid;
	int fileid = 0;
	tf << fileid;
	string f1 = "./Output/Node" + ss.str() + "/TraceFile" + tf.str() + ".trc";

	string f2 = dir + "/page_migration_stats_Node" + ss.str() + ".txt";
	cout << f2 << endl;
	// cin.get();

	const char *file1 = f1.c_str();
	file2[nodeid] = f2.c_str();
	page_migration.open(file2[nodeid]);
	ShmKEY1 = 10 * nodeid + 1;
	ShmID1 = shmget(ShmKEY1, sizeof(int), IPC_CREAT | 0666);
	main_start = (int *)shmat(ShmID1, NULL, 0);

	ShmKEY3 = 10 * nodeid + 3;
	ShmID3 = shmget(ShmKEY3, sizeof(uint64_t), IPC_CREAT | 0666);
	num_ins = (uint64_t *)shmat(ShmID3, NULL, 0);

	int c = 0;

	nodeid = nodeid - 1;
	// fill_load_buffer(nodeid);//

	initialize_branch_predictor(nodeid);

	// fill_load_buffer(nodeid, 1);
	// simulation starts here, clock started
	while (common_clock < simulation_time)
	{
		pthread_barrier_wait(&b);
		// pintool writes a max_ins number of instructions in a file and creates a new file everytime, delete old once it is read
		//(this hack is used to save disk space to avoid large instruction traces)
		// here we close previous file and open new file everytime we go for reading more instructions from the stream
		if (inst_stream[nodeid].size() < 50)
		{
			INST temp;
			while (*num_ins <= max_ins && (*main_start) != 10)
			{
				sleep(1);
			}
			if ((*num_ins) > 0)
			{
				TraceFile.clear();
				TraceFile.open(file1);
				TraceFile.seekg(0, ios::end);
				TraceFile.seekg(0, ios::beg);

				while (TraceFile.read((char *)&temp, sizeof(temp)))
				{
					inst_stream[nodeid].push_back(temp);
				}
				TraceFile.close();
				remove(file1);
				*num_ins = 0;
				fileid++;
				tf.str("");
				tf << fileid;
				f1 = "./Output/Node" + ss.str() + "/TraceFile" + tf.str() + ".trc";
				file1 = f1.c_str();
			}
		}
		// Seperating the core-wise instruction stream
		int combined_count = 0;
		for (int i = 0; i < core_count; i++)
		{
			if (core_inst_stream[nodeid][i].size() < 500)
				Node_to_Core_Ins_Stream(nodeid); // core_inst_stream, nodeid);
		}

		simulate_network();
		simulate_network_reverse();
		local_mem[nodeid]->update();

		// if(common_clock%1000000==0)
		// {
		// 	cout << "\nNode-ID- " << nodeid;
		// cout << "\n Total issued instructions: " << num_inst_issued[nodeid];
		// 	cout << "\n Total executed instructions: " << num_inst_exec[nodeid];
		// 	cout << "\n Total committed instructions: " << num_inst_commited[nodeid];
		// }

		if (nodeid == 0)
		{
			for (int i = 0; i < num_mem_pools; i++)
			{
				remote_mem[i]->update();
			}
		}

		update_mshr(nodeid);

		for (int i = 0; i < core_count; i++)
		{
			itlb_search(nodeid, i); // in TLB.cpp
			dtlb_search(nodeid, i);
			update_core(nodeid, i);
		}

// call only after calling update core
#ifdef migration
		bool is_time_out = time_out(nodeid);

		if (common_clock == setUpPhase_cycle[nodeid])
		{
			hot_page_cache[nodeid].clear();
			per_epoch_page_access_counter[nodeid].clear();
			num_pages_to_migrate[nodeid] = 0;
			page_migration_threshold[nodeid] = 0;
			setUpPhase_cycle[nodeid] = common_clock;
			setting_params[nodeid] = 1;
			cout << "\n------------initialisation phase start---------\n";
		}

		if (common_clock == setUpPhase_cycle[nodeid] + initiliastion_epoch_length)
		{
			cout << "\ncache size " << hot_page_cache[nodeid].size();
			cout << "\nhash size " << per_epoch_page_access_counter[nodeid].size();
			cout << "\nnpm " << num_pages_to_migrate[nodeid];
			cout << "\npmt " << page_migration_threshold[nodeid] << endl;
			set_params(nodeid, page_migration);
		}
		if (is_time_out)
		{
			cout << "-----------Time Out ---------------" << endl;
			cout << "mig_flag: " << migration_flag[nodeid] << endl;
			migration_flag[nodeid] = 0;
			hot_page_cache[nodeid].clear();
			per_epoch_page_access_counter[nodeid].clear();
			num_pages_to_migrate[nodeid] = 0;
			page_migration_threshold[nodeid] = 0;
			setUpPhase_cycle[nodeid] = common_clock;
			setting_params[nodeid] = 1;
		}

		stall_processor(nodeid);				  // to stall the processor after swapping is complete and to perform actual migration
		resume_processor(nodeid, page_migration); // resume process once the migration is completed
#endif

		// 			if(common_clock%100000==0)
		// {		cout << "\n"
		// 			 << common_clock<<" cycles completed";
		// 			 			cout<<"\ncache size "<<hot_page_cache[nodeid].size();
		// 			cout<<"\nhash size "<<per_epoch_page_access_counter[nodeid].size();
		// 			int node_num_inst_issued=0;
		// 			int node_num_inst_comm=0;
		// 			for(int i=0;i<core_count;i++)
		// 			{
		// 			   node_num_inst_issued+=num_inst_issued[nodeid][i];
		// 			   node_num_inst_comm+=num_inst_commited[nodeid][i];
		// 			}

		// 			cout<<"\n issue  "<<reorder_buffer[nodeid][0].issue_ptr<<"  "<<reorder_buffer[nodeid][1].issue_ptr;
		// 			cout<<"\n commit "<<reorder_buffer[nodeid][0].commit_ptr<<" "<<reorder_buffer[nodeid][1].commit_ptr;
		// 			cout << "\n Total issued instructions: " << node_num_inst_issued;
		// 			cout<<"\n committ ins "<<node_num_inst_comm;
		// 			cout<<"\n background_pg_swap_clock "<<background_pg_swap_clock[nodeid];
		// 			cout<<"\n brach cycle "<<branch_misprediction_penalty[nodeid][0];
		// 			// cout<<"\nil1 size:"<<il1_miss_buffer[nodeid][0].size()<<"   "<<il1_miss_buffer[nodeid][1].size();
		// 			// for(int j=0;j<core_count;j++)
		// 			// {
		// 			// 	cout<<"\ncore "<<j;
		// 			// 	for(int i=0;i<il1_miss_buffer[nodeid][j].size();i++)
		// 			// 	cout<<"\nvaddr"<<il1_miss_buffer[nodeid][j][i].ins_vaddr<<" com cycle "<<il1_miss_buffer[nodeid][j][i].complete_cycle<<" ins_id"<<il1_miss_buffer[nodeid][j][i].ins_id<<" dl1 com "<<il1_miss_buffer[nodeid][j][i].L1_cache_miss_complete_cycle<<" source "<<il1_miss_buffer[nodeid][j][i].source_cache<<" req cyc "<<dl1_miss_buffer[nodeid][j][i].request_cycle;
		// 			// 	cout<<"\n\n";
		// 			// }
		// 			// cout<<"\ndl1 size:"<<dl1_miss_buffer[nodeid][0].size()<<"   "<<dl1_miss_buffer[nodeid][1].size();
		// 			// for(int j=0;j<core_count;j++)
		// 			// {
		// 			// 	cout<<"\ncore "<<j;
		// 			// 	for(int i=0;i<dl1_miss_buffer[nodeid][j].size();i++)
		// 			// 	cout<<"\nvaddr"<<dl1_miss_buffer[nodeid][j][i].ins_vaddr<<" com cycle "<<dl1_miss_buffer[nodeid][j][i].complete_cycle<<" ins_id"<<dl1_miss_buffer[nodeid][j][i].ins_id<<" dl1 com "<<dl1_miss_buffer[nodeid][j][i].L1_cache_miss_complete_cycle<<" source "<<dl1_miss_buffer[nodeid][j][i].source_cache<<" req cyc "<<dl1_miss_buffer[nodeid][j][i].request_cycle;
		// 			// 	cout<<"\n\n";
		// 			// }
		// 			// cout<<"\nl2 size:"<<l2_miss_buffer[nodeid][0].size()<<"  "<<l2_miss_buffer[nodeid][1].size();
		// 			// for(int j=0;j<core_count;j++)
		// 			// {
		// 			// 	cout<<"\ncore "<<j;
		// 			// 	for(int i=0;i<l2_miss_buffer[nodeid][j].size();i++)
		// 			// 	cout<<"\nvaddr"<<l2_miss_buffer[nodeid][j][i].ins_vaddr<<" com cycle "<<l2_miss_buffer[nodeid][j][i].complete_cycle<<" ins_id"<<l2_miss_buffer[nodeid][j][i].ins_id<<" l2 com "<<l2_miss_buffer[nodeid][j][i].L2_cache_miss_complete_cycle<<" source "<<l2_miss_buffer[nodeid][j][i].source_cache;
		// 			// 	cout<<"\n\n";
		// 			// }
		// 			// cout<<"\nl3 size:"<<l3_miss_buffer[nodeid].size();
		// 			// for(int i=0;i<l3_miss_buffer[nodeid].size();i++)
		// 			// cout<<"\nvaddr"<<l3_miss_buffer[nodeid][i].ins_vaddr<<" com cycle "<<l3_miss_buffer[nodeid][i].complete_cycle<<" ins_id"<<l3_miss_buffer[nodeid][i].ins_id<<" l3 com "<<l3_miss_buffer[nodeid][i].L3_cache_miss_complete_cycle<<" source "<<l3_miss_buffer[nodeid][i].source_cache;
		// 			cout<<"\niq size "<<inst_queue[nodeid][0].size()<<"   "<<inst_queue[nodeid][1].size();
		// 			cout<<"\nok to go "<<ok_to_go;
		// 			cout<<"\nresume stall clock "<<resume_stall_clock[nodeid];
		// 			cout<<"mig_stall: "<<migrate_stall[nodeid]<<endl;
		// 			cout<<"\nlsq sie "<<load_store_queue[nodeid][0].size()<<"   "<<load_store_queue[nodeid][1].size();
		// 			cout<<"\npending lsq size "<<pending_load_store[nodeid][0].size()<<"   "<<pending_load_store[nodeid][1].size();
		// 			cout<<"\n ls buffer size "<<load_store_buffer[nodeid][0].size()<<"   "<<load_store_buffer[nodeid][1].size();
		// 			// cout<<"\n rob size "<<
		// 			//cin.get();
		// }

		int node_num_inst_issued = 0;
		for (int j = 0; j < core_count; j++)
		{
			node_num_inst_issued += num_inst_issued[nodeid][j];
		}
		// if(ins_issue_last==node_num_inst_issued && node_num_inst_issued>4570000 && rs==0 && inst_queue[nodeid][0].size())
		if (common_clock % 100000 == 0)
		{ // && common_clock%100==0)

			// if(common_clock>6000000 && common_clock%100000==0)
			// {
			// 				cout<<"------------------RS-------------------------"<<endl;
			// 				//cout<<"\nrs size "<<RS_Size;
			// 		for(int j=0;j<core_count;j++)
			// 		{
			// 						cout<<"\ncore "<<j;
			// 		for (int i = 0; i < RS_Size; i++)
			// 		{

			// 			if (reservation_station[nodeid][j][i].valid == 1)
			// 			{

			// 				cout << "\nvalid " << reservation_station[nodeid][j][i].valid << "\t\trob_in " << reservation_station[nodeid][j][i].ROB_index << "\t\tins_id " << reservation_station[nodeid][j][i].ins_id
			// 					 << "\t\tRreg-" << reservation_station[nodeid][j][i].RR[0] << " " << reservation_station[nodeid][j][i].reg_read_dependencies[0]
			// 					 << " " << reservation_station[nodeid][j][i].RR[1] << " " << reservation_station[nodeid][j][i].reg_read_dependencies[1]
			// 					 << " " << reservation_station[nodeid][j][i].RR[2] << " " << reservation_station[nodeid][j][i].reg_read_dependencies[2]
			// 					 << " " << reservation_station[nodeid][j][i].RR[3] << " " << reservation_station[nodeid][j][i].reg_read_dependencies[3]
			// 					 << "\t\tRmemop-" << reservation_station[nodeid][j][i].mem_read_addr[0] << " " << reservation_station[nodeid][j][i].mem_read_dependencies[0]
			// 					 << " " << reservation_station[nodeid][j][i].mem_read_addr[1] << " " << reservation_station[nodeid][j][i].mem_read_dependencies[1]
			// 					 << "\t\t  branch mis " << reservation_station[nodeid][j][i].branch_miss_predicted;
			// 			}
			// 		}}

			// 		cout<<endl<<"------------------ROB-------------------------"<<endl;

			// 		cout<<"\niss " << reorder_buffer[nodeid][0].issue_ptr << " cmt " << reorder_buffer[nodeid][0].commit_ptr;

			// 		for(int j=0;j<core_count;j++)
			// 		{
			// 			cout<<"\ncore "<<j;
			// 			for (int i = 0; i < ROB_Size; i++)
			// 		{
			// 			if (reorder_buffer[nodeid][j].rob[i].status != -1)
			// 			{
			// 				cout << "\n ind "<<i<<"   iss " << reorder_buffer[nodeid][j].issue_ptr << " cmt " << reorder_buffer[nodeid][j].commit_ptr << " no.of dependencies " << reorder_buffer[nodeid][j].rob[i].INS_RS_Dependency.size()
			// 					 << " wop" << reorder_buffer[nodeid][j].rob[i].mem_write_operand << " wreg-" << reorder_buffer[nodeid][j].rob[i].write_reg_num[0] << " "
			// 					 << reorder_buffer[nodeid][j].rob[i].write_reg_num[1] << " " << reorder_buffer[nodeid][j].rob[i].write_reg_num[2] << " " << reorder_buffer[nodeid][j].rob[i].write_reg_num[3]
			// 					 << " wrt_com " << reorder_buffer[nodeid][j].rob[i].write_completed << " status " << reorder_buffer[nodeid][j].rob[i].status << " requested_cycle " << reorder_buffer[nodeid][j].rob[i].request_cycle<<" insid"<<reorder_buffer[nodeid][j].rob[i].ins_id;
			// 			}
			// 		}}
			// 			cout<<endl<<endl<<"------------------Execution units-------------------------"<<endl;

			// cout<<"\ncore 0";
			// 		for (int i = 0; i < num_exec_units; i++)
			// 		{
			// 			//if (execution_unit[nodeid][0][i].status == 1)
			// 				cout << "\nexecunit " << i << " rob_ind: " << execution_unit[nodeid][0][i].rob_index << " cyc: " << execution_unit[nodeid][0][i].cycle_completed<<endl;
			// 		}

			// cout<<"\ncore 1";
			// 				for (int i = 0; i < num_exec_units; i++)
			// 		{
			// 			//if (execution_unit[nodeid][0][i].status == 1)
			// 				cout << "\nexecunit " << i << " rob_ind: " << execution_unit[nodeid][1][i].rob_index << " cyc: " << execution_unit[nodeid][1][i].cycle_completed<<endl;
			// 		}
			// 		cout<<"\n lsq \n";

			// 				for(int j=0;j<core_count;j++)
			// 		{
			// 			cout<<"\ncore "<<j;
			// 		for(int i=0;i<load_store_queue[nodeid][0].size();i++)
			// 		{
			// 			cout<<"\n addr "<<load_store_queue[nodeid][0][i].ins_vaddr<<" id "<<load_store_queue[nodeid][0][i].ld_str_id<<" rs ind "<<load_store_queue[nodeid][0][i].rs_index<<" rob ind "<<load_store_queue[nodeid][0][i].rob_index;
			// 		}
			// 		}
			// 		cout<<"\nlsq sie "<<load_store_queue[nodeid][0].size()<<"  "<<load_store_queue[nodeid][1].size();
			// 		cout<<"\npending lsq size "<<pending_load_store[nodeid][0].size()<<"  "<<pending_load_store[nodeid][1].size();
			// 		cout<<"\n ls buffer size "<<load_store_buffer[nodeid][0].size()<<"  "<<load_store_buffer[nodeid][1].size();
			// 		cout<<"\nitlb size "<<itlb_miss_buffer[nodeid][0].size()<<"  "<<itlb_miss_buffer[nodeid][1].size();
			// 		cout<<"\nil size "<<il1_miss_buffer[nodeid][0].size()<<"  "<<il1_miss_buffer[nodeid][1].size();
			// 		cout<<"\ndtlb size "<<dtlb_miss_buffer[nodeid][0].size()<<"  "<<dtlb_miss_buffer[nodeid][1].size();
			// 		cout<<"\ndl size "<<dl1_miss_buffer[nodeid][0].size()<<"  "<<dl1_miss_buffer[nodeid][1].size();
			// 		cout<<"\n ready queue size "<<ready_queue[nodeid][0].size()<<"  "<<ready_queue[nodeid][1].size();
			// 		for(int j=0;j<core_count;j++)
			// 		{
			// 			cout<<"\n core"<<j;
			// 			for(int i=0;i<ready_queue[nodeid][j].size();i++)
			// 			{
			// 				cout<<"\n ready queue "<<i<<" "<<ready_queue[nodeid][j][i].first;
			// 			}
			// 		}
			// 		// for(int i=0;i<dl1_miss_buffer[nodeid][0].size();i++)
			// 		// 		cout<<"\nvaddr"<<dec<<dl1_miss_buffer[nodeid][0][i].ins_vaddr<<" com cycle "<<dl1_miss_buffer[nodeid][0][i].complete_cycle<<" ins_id"<<dl1_miss_buffer[nodeid][0][i].ins_id;
			// 					cout<<"\nl2 size "<<l2_miss_buffer[nodeid][0].size()<<"  "<<l2_miss_buffer[nodeid][1].size();
			// 		// for(int i=0;i<l2_miss_buffer[nodeid][0].size();i++)
			// 		// 		cout<<"\nvaddr"<<dec<<l2_miss_buffer[nodeid][0][i].ins_vaddr<<" com cycle "<<l2_miss_buffer[nodeid][0][i].complete_cycle<<" ins_id"<<l2_miss_buffer[nodeid][0][i].ins_id;
			// 					cout<<"\nl3 size "<<l3_miss_buffer[nodeid].size();
			// 		// for(int i=0;i<l3_miss_buffer[nodeid].size();i++)
			// 		// 		cout<<"\nvaddr"<<dec<<l3_miss_buffer[nodeid][i].ins_vaddr<<" com cycle "<<l3_miss_buffer[nodeid][i].complete_cycle<<" ins_id"<<l3_miss_buffer[nodeid][i].ins_id;
			// 					cout<<"\ncycle "<<common_clock<<"\n";
			cout << "\niq size ";
			for (int k = 0; k < core_count; k++)
			{
				cout << inst_queue[nodeid][k].size() << "  ";
			}
			cout << "\nnode stream ";
			for (int k = 0; k < core_count; k++)
			{
				cout << core_inst_stream[nodeid][k].size() << "  ";
			}
			// 				cout<<"\nresume stall clock "<<resume_stall_clock[nodeid];
			// 				cout<<"mig_stall: "<<migrate_stall[nodeid]<<endl;
			cout << "\n Total issued instructions: " << node_num_inst_issued;
			cout << "\n committ ins " << num_inst_commited[nodeid][0] << "  " << num_inst_commited[nodeid][1] << "  " << num_inst_commited[nodeid][2] << "  " << num_inst_commited[nodeid][3];
			cout << "\ncycle " << common_clock;
			// 				//ins_issue_last = node_num_inst_issued;
			// 				// cin.get();
		}

		pthread_barrier_wait(&b);
		if (nodeid == 0)
		{
			if (common_clock % Result_cycle == 0 && common_clock > 0)
			{
				for (int i = 0; i < num_nodes; i++)
				{
					
					print_stats(i);
				}
				/////////////////////////////////////////////////////////// updated /////////////////////////////////////////

				/////////////////////////////////////////////////////////// updated /////////////////////////////////////////
			}
			if (common_clock % 800000 == 0 && common_clock > 0)
			{
				for (uint64_t node_id = 0; node_id < num_nodes; node_id++)
				{
					cout << "epoch " << epoch_num << " " << history[node_id].size() << " ";
					history[node_id].clear();
					for (uint64_t a = 0; a < MAX_STRIDE; a++)
						strides[node_id][a] = 0;
					is_training_complete[node_id] = 0;
					cout << history.size() << "\n";
				}
			}
			common_clock++;
		}

		pthread_barrier_wait(&b);
	}

	shmctl(ShmID1, IPC_RMID, NULL);
	shmctl(ShmID3, IPC_RMID, NULL);
	shmdt(main_start);
	shmdt(num_ins);

	pthread_exit(NULL);
}

void print_stats(int node_id)
{
	std::ostringstream rf;
	rf << node_id;
	string result = dir + "/node_" + rf.str() + "result.txt";
	const char *result1 = result.c_str();
	ResultsFile.open(result1, ios::app);
	int node_local_access_time = 0, node_local_access_count = 0;
	int node_remote_access_time = 0, node_remote_access_count = 0;
	uint64_t total_ld_str = 0, total_num_ld_str_total = 0, total_num_ld_str_completed = 0;
	ResultsFile << "\n-------------Epoch_Number------------------" << (common_clock / Result_cycle) << "-------------";
	ResultsFile << "\n-------------Node-ID- " << node_id << "-------------";
	int total_num_inst_issued = 0, total_num_inst_exec = 0, total_num_inst_commited = 0;
	for (int j = 0; j < core_count; j++)
	{
		ResultsFile << "\n-------------Core Statistics-------------\n";
		ResultsFile << "\n-------------core-ID- " << j << "-------------";
		ResultsFile << "\n Total Issued instructions: " << num_inst_issued[node_id][j];
		ResultsFile << "\n Total executed instructions: " << num_inst_exec[node_id][j];
		ResultsFile << "\n Total committed instructions: " << num_inst_commited[node_id][j];
		total_num_inst_issued += num_inst_issued[node_id][j];
		total_num_inst_exec += num_inst_exec[node_id][j];
		total_num_inst_commited += num_inst_commited[node_id][j];
		total_ld_str = total_ld_str + ld_str_id[node_id][j];
		ResultsFile << "\n Total Load-store sent to memory:(include redundant loads to store address) " << num_ld_str_total[node_id][j];
		ResultsFile << "\n Total Load-store completed:(does not include redundant loads to store address) " << num_ld_str_completed[node_id][j];
		total_num_ld_str_total += num_ld_str_total[node_id][j], total_num_ld_str_completed += num_ld_str_completed[node_id][j];
		double IPC = ((double)num_inst_commited[node_id][j] / (double)common_clock);
		ResultsFile << "\nIPC is: " << IPC;
		ResultsFile << "\nCPI is: " << 1 / IPC;

		node_local_access_time += total_core_local_time[node_id][j];
		node_local_access_count += total_core_local_count[node_id][j];
		node_remote_access_time += total_core_remote_time[node_id][j];
		node_remote_access_count += total_core_remote_count[node_id][j];
		float temp1 = (float)total_core_local_time[node_id][j] / (float)total_core_local_count[node_id][j];
		float temp2 = (float)total_core_remote_time[node_id][j] / (float)total_core_remote_count[node_id][j];
		ResultsFile << "\navg local access time:" << temp1 << endl;
		ResultsFile << "avg remote access time:" << temp2 << endl;
	}

	ResultsFile << "\n\n----------------Node " << node_id << " Statistics---------------";
	ResultsFile << "\n Total Issued instructions: " << total_num_inst_issued;
	ResultsFile << "\n Total executed instructions: " << total_num_inst_exec;
	ResultsFile << "\n Total committed instructions: " << total_num_inst_commited;
	ResultsFile << "\n Total Load-store sent to memory:(include redundant loads to store address): " << total_num_ld_str_total;
	ResultsFile << "\n Total Load-store completed:(does not include redundant loads to store address): " << total_num_ld_str_completed;
	float IPC = ((double)total_num_inst_commited / (double)common_clock);
	ResultsFile << "\nIPC is: " << IPC;
	ResultsFile << "\nCPI is: " << 1 / IPC;
	ResultsFile << "\nTotal local memory accesses sent: " << num_local[node_id];
	ResultsFile << "\nTotal local memory accesses completed: " << node_local_access_count;
	ResultsFile << "\nTotal remote memory accesses sent: " << num_remote[node_id];
	ResultsFile << "\nTotal remote memory accesses completed: " << node_remote_access_count;
	float temp1 = (float)node_local_access_time / (float)node_local_access_count;
	float temp2 = (float)node_remote_access_time / (float)node_remote_access_count;
	ResultsFile << "\navg node local access time: " << temp1 << endl;
	ResultsFile << "\navg node remote access time: " << temp2 << endl;

	ResultsFile.close();

	L[node_id].get_stats();
	mem_stats << "\n\n------ Pool Wise Page Count --------------";
	L[node_id].pool_wise_page_count(0);
	mem_stats << "\n\n-------Local-Remote Mapping Entries--------------";
	L[node_id].display_mapping(mem_stats);
}

void fill_victim_list(int node_id)
{
	int cnt = 0;
	for (int i = 0; i < max_victim_pages; i++)
	{
		victim_pages[node_id].push_back(L[node_id].local_page_count() - cnt - 1);
		cnt++;
		// cout<<victim_pages[node_id][i]<<" ";
	}
}
int main(int argc, char *argv[])
{
	// for(int i=0;i<num_nodes;i++){
	// 	for(int j=0;j<core_count;j++){
	// 		for(int k=0;k<num_exec_units;k++){
	// 			execution_unit[i][j][k].status=0;
	// 			cout<<"exe_stat:"execution_unit[i][j][k].status<<endl;
	// 		}
	// 	}
	// }
	if (argc < 2)
		exit(0);
	else if (argc == 2)
		dir = argv[1];

	declare_memory_variables(dir);
	pthread_barrier_init(&b, NULL, num_nodes);
	pthread_mutex_init(&lock, NULL);
	pthread_t local_mem_threads[num_nodes];

	for (long i = 0; i < num_nodes; i++)
	{
		fill_victim_list(i); // Initilally filling out the victim list with random physical addresses
		for (int j = 0; j < core_count; j++)
		{
			INS_id[i][j] = 1;
			ld_str_id[i][j] = 1;
		}
		num_pages_to_migrate[i] = 0;
		background_pg_swap_clock[i] = -1;
	}
	// for(int i=0;i<max_victim_pages;i++){
	// 	cout<<victim_pages[0][i]<<" ";
	// }
	// cout<<endl;
	for (long i = 1; i <= num_nodes; i++)
	{
		// each thread for one node simulation
		pthread_create(&local_mem_threads[i - 1], NULL, node_stream_handler, (void *)i);
	}

	for (long i = 1; i <= (num_nodes); i++)
	{
		pthread_join(local_mem_threads[i - 1], NULL);
	}

	return 0;
}
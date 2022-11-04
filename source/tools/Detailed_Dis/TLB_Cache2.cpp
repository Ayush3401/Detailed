		if(common_clock>18000)
{  
			// cout << "\n"
			//  << common_clock<<" cycles completed";
	// cout<<"------------------RS-------------------------"<<endl;
	// 	for (int i = 0; i < RS_Size; i++)
	// 	{  
			
	// 		if (reservation_station[nodeid][0][i].valid == 1)
	// 		{
				
	// 			cout << "\nvalid " << reservation_station[nodeid][0][i].valid << "\t\trob_in " << reservation_station[nodeid][0][i].ROB_index << "\t\tins_id " << reservation_station[nodeid][0][i].ins_id
	// 				 << "\t\tRreg-" << reservation_station[nodeid][0][i].RR[0] << " " << reservation_station[nodeid][0][i].reg_read_dependencies[0]
	// 				 << " " << reservation_station[nodeid][0][i].RR[1] << " " << reservation_station[nodeid][0][i].reg_read_dependencies[1]
	// 				 << " " << reservation_station[nodeid][0][i].RR[2] << " " << reservation_station[nodeid][0][i].reg_read_dependencies[2]
	// 				 << " " << reservation_station[nodeid][0][i].RR[3] << " " << reservation_station[nodeid][0][i].reg_read_dependencies[3]
	// 				 << "\t\tRmemop-" << reservation_station[nodeid][0][i].mem_read_addr[0] << " " << reservation_station[nodeid][0][i].mem_read_dependencies[0]
	// 				 << " " << reservation_station[nodeid][0][i].mem_read_addr[1] << " " << reservation_station[nodeid][0][i].mem_read_dependencies[1]
	// 				 << "\t\t  branch mis " << reservation_station[nodeid][0][i].branch_miss_predicted;
	// 		}
	// 	}
// 		cout<<"------------------D-CACHE----------------"<<endl;
// 		for(auto it:dl1_miss_buffer[nodeid][0]){
			
// 			cout<<" vaddr: "<<it.ins_vaddr<<" paddr: "<<it.paddr<<" complete_cycle: "<<it.complete_cycle<<" ins_id: "<<it.ins_id<<endl;
// 		}
// 		cout<<"------------------L2-CACHE----------------"<<endl;
// 		for(auto it:l2_miss_buffer[nodeid][0]){
// 			cout<<" vaddr: "<<it.ins_vaddr<<" paddr: "<<it.paddr<<" complete_cycle: "<<it.complete_cycle<<endl;
// 		}
// 		cout<<"------------------L3-CACHE----------------"<<endl;
// 		for(auto it:l3_miss_buffer[nodeid]){
// 			cout<<" vaddr: "<<it.ins_vaddr<<" paddr: "<<it.paddr<<" complete_cycle: "<<it.complete_cycle<<endl;
// // 		}
// 		cout << "\n";
// 		cout<<endl<<"------------------ROB-------------------------"<<endl;
		
// 		for (int i = 0; i < ROB_Size; i++)
// 		{
// 			if (reorder_buffer[nodeid][0].rob[i].status != -1)
// 			{
// 				cout << "\n iss " << reorder_buffer[nodeid][0].issue_ptr << " cmt " << reorder_buffer[nodeid][0].commit_ptr << " no.of dependencies " << reorder_buffer[nodeid][0].rob[i].INS_RS_Dependency.size()
// 					 << " wop" << reorder_buffer[nodeid][0].rob[i].mem_write_operand << " wreg-" << reorder_buffer[nodeid][0].rob[i].write_reg_num[0] << " "
// 					 << reorder_buffer[nodeid][0].rob[i].write_reg_num[1] << " " << reorder_buffer[nodeid][0].rob[i].write_reg_num[2] << " " << reorder_buffer[nodeid][0].rob[i].write_reg_num[3]
// 					 << " wrt_com " << reorder_buffer[nodeid][0].rob[i].write_completed << " status " << reorder_buffer[nodeid][0].rob[i].status << " requested_cycle " << reorder_buffer[nodeid][0].rob[i].request_cycle;
// 			}
// 		}
// 			cout<<endl<<endl<<"------------------Execution units-------------------------"<<endl;

// 		for (int i = 0; i < num_exec_units; i++)
// 		{
// 			if (execution_unit[nodeid][0][i].status == 1)
// 				cout << "\nexecunit " << i << " rob_ind: " << execution_unit[nodeid][0][i].rob_index << " cyc: " << execution_unit[nodeid][0][i].cycle_completed<<endl;
// 		}
// 			cout<<endl<<endl<<"------------------instruction count-------------------------"<<endl;
  	

		// for (int i = 0; i < completed_inst_rob_index[nodeid][0].size(); i++)
		// {
		// 	cout << "\n com rob ind " << completed_inst_rob_index[nodeid][coreid][i];
		// }
 //     cout<<endl<<endl<<"------------------load store queue-------------------------"<<endl;
	// 	for (int i = 0; i < load_store_queue[nodeid][0].size(); i++)
	// 	{
	// 		cout <<hex<< "\nins_vaddr:" << load_store_queue[nodeid][0][i].ins_vaddr <<dec<< " ld_str_id: " << load_store_queue[nodeid][0][i].ld_str_id << " acc_size:" << load_store_queue[nodeid][0][i].access_size << " rs_ind:"
	// 			 << load_store_queue[nodeid][0][i].rs_index << " rob_ind:" << load_store_queue[nodeid][0][i].rob_index << " type:" << load_store_queue[nodeid][0][i].INS_type_ld_str;
	// 	}

	// cout<<"\n---------------l3----------------\n";
	// for(int i=0;i<dl1_miss_buffer[nodeid][0].size();i++)
	// {
	// 	cout<<"\nvaddr"<<hex<<dl1_miss_buffer[nodeid][0][i].ins_vaddr<<hex<<" paddr "<<dl1_miss_buffer[nodeid][0][i].paddr<<dec<<" com cycle "<<dl1_miss_buffer[nodeid][0][i].complete_cycle<<" id "<<dl1_miss_buffer[nodeid][0][i].ins_id;
	// }

	// cout<<"\n---------------l3----------------\n";
	// for(int i=0;i<l3_miss_buffer[nodeid].size();i++)
	// {
	// 	cout<<"\nvaddr"<<hex<<l3_miss_buffer[nodeid][i].ins_vaddr<<hex<<" paddr "<<l3_miss_buffer[nodeid][i].paddr<<dec<<" com cycle "<<l3_miss_buffer[nodeid][i].complete_cycle<<" id "<<l3_miss_buffer[nodeid][i].ins_id;
	// }
	// cout<<"\nmem_remote"<<num_remote[nodeid];
	// cin.get();

}


#include "pin_util.h"
#include <boost/preprocessor.hpp>
#define num_nodes 1
#define core_count 2
#include "TLB.h"
#include "ID_Cache.h"
#define TLB_HIT 9
#define TLB_MISS 60
#define L1_HIT 4
#define L2_HIT 12
#define L3_HIT 25

#define iqueue_size 50

// queue for per-core instruction stream on a node shared by Pintool
queue<INST> core_inst_stream[num_nodes][core_count];

using namespace std;

#define mshr 8 // no. of enetries in mshr in each cache-level

// fetches sent to cache
struct INST_FETCH_QUEUE
{
    INST ins;
    int64_t request_cycle;
    bool tlb_fetched;
    int64_t tlb_complete_cycle;
    bool mem_fetched;
    int64_t mem_complete_cycle;

    INST_FETCH_QUEUE()
    {
        tlb_fetched = false;
        mem_fetched = false;
        request_cycle = 0;
        tlb_complete_cycle = 0;
        mem_complete_cycle = 0;
    }

    bool operator<(const INST_FETCH_QUEUE &temp) const
    {
        return (ins.ins_id > temp.ins.ins_id);
    }
};

// this structure holds misses from tlb/cache in mshr
struct miss_queue
{
    INST_FETCH_QUEUE inst_fetch;
    uint64_t ins_id;
    uint64_t ins_vaddr;
    uint64_t paddr;
    int node_id;
    int thread_id;
    int64_t request_cycle;
    int64_t complete_cycle;
    char source_cache; // request coming from I or D cache
    int core_id;
    bool write;
    uint64_t mem_trans_id;
    int cache_access_size;
    uint64_t L1_cache_miss_complete_cycle;
    uint64_t L2_cache_miss_complete_cycle;
    uint64_t L2_cache_miss_complete_cycle;
    miss_queue()
    {
    }
};

queue<miss_queue> temp[num_nodes][core_count]; // temp to store the some instruction belonging to same block if there is no space in ins queue
queue<miss_queue> load_store_buffer[num_nodes][core_count];

void fill_load_buffer(int node_id, int core_id)
{
    miss_queue temp;
    int id = 0;
    uint64_t vaddr = 0x7fa507f75e98;
    for (int i = 0; i < 100000; i++)
    {
        temp.ins_id = id;
        temp.ins_vaddr = vaddr;
        temp.inst_fetch.ins.proc_id = 0;
        id++;
        uint64_t k = rand() % 100000;
        int j = rand() % 2;
        temp.write = j;
        if (j)
        {
            temp.cache_access_size = 1 + rand() % 5;
        }
        else
        {
            temp.cache_access_size = 1 + rand() % 5;
        }
        if (i % 2)
        {
            vaddr += k;
        }
        else
            vaddr -= k;
        temp.node_id = node_id;
        temp.core_id = core_id;
        load_store_buffer[node_id][core_id].push(temp);
    }
}

vector<miss_queue> itlb_miss_buffer[num_nodes][core_count];
vector<miss_queue> itlb_wait_buffer[num_nodes][core_count];

vector<miss_queue> il1_miss_buffer[num_nodes][core_count];

vector<miss_queue> dl1_miss_buffer[num_nodes][core_count];
vector<miss_queue> il1_wait_buffer[num_nodes][core_count];

vector<miss_queue> l2_miss_buffer[num_nodes][core_count];
vector<miss_queue> l3_miss_buffer[num_nodes];

vector<miss_queue> dtlb_miss_buffer[num_nodes][core_count];

INST_FETCH_QUEUE ITLB_HIT(INST &, int, int);
void ITLB_MISS(INST &, int, int);
void itlb_mshr_update(int, int);
void dtlb_search(int, int);
void DTLB_MISS(miss_queue &, int, int);
bool dcache_hit(int, int, uint64_t, uint64_t, miss_queue &);
void dcache_miss(int, int, uint64_t, uint64_t, miss_queue);
void dtlb_mshr_update(int, int);
void icache_search(int, int, uint64_t, uint64_t, INST_FETCH_QUEUE);
void l2_cache_search(int, int, uint64_t, uint64_t, miss_queue, bool);
void update_l2_mshr(int, int);
void l3_cache_search(int, uint64_t, uint64_t, miss_queue, bool);
void write_back_to_memory();
void update_l3_mshr(int);
void update_caches(int);
void get_ins_in_same_block(int, int, miss_queue);
void fetch_ins_in_same_block(int, int, INST_FETCH_QUEUE);
void update_icache_mshr(int, int);
void send_to_memory(miss_queue, int);
void write_back_to_memory(int, int, int);
bool page_table_walk(pgd &, uint64_t, uint64_t &);
void handle_page_fault(pgd &, int, uint64_t, uint64_t &, local_addr_space &, bool &);

// queue for fetched instructions
priority_queue<INST_FETCH_QUEUE> inst_queue[num_nodes][core_count];

// queue for storing (DTLB HIT OR MISS) AND DL1 HIT until it's complete cycle arrives.
queue<miss_queue> response_queue[num_nodes][core_count];

// used for giving a transaction-id to all the LLC cache misses
uint64_t mem_trans_id[num_nodes] = {0};

// used for sending the next instruction to iqueue, instructions are fetched out-of-order
// but are sent to ins_queue in-order
uint64_t last_ins_id[num_nodes][core_count] = {0};

void update_mshr(int node_id)
{
    update_caches(node_id);

    for (int i = 0; i < core_count; i++)
        itlb_mshr_update(node_id, i);

    update_l3_mshr(node_id);
    for (int i = 0; i < core_count; i++)
    {
        update_l2_mshr(node_id, i);
        update_icache_mshr(node_id, i);
    }
}

bool dtlb_to_dcache = 0;
void dtlb_search(int node_id, int core_id)
{

    miss_queue load_store_entry;
    if (!load_store_buffer[node_id][core_id].empty())
    {
        load_store_entry = load_store_buffer[node_id][core_id].front();
    }
    else
        return;
    uint64_t vaddr = load_store_entry.ins_vaddr;
    uint64_t paddr = NULL;
    int dtlb_id = (node_id * core_count) + core_id;
    bool dtlb_hit = false;
    dtlb_hit = dtlbs[dtlb_id].AccessTLB(vaddr, TLB_BASE::ACCESS_TYPE_LOAD, temp.proc_id, paddr);

    if (dtlb_hit)
    {
        if (dtlb_to_dcache)
        {
            dtlb_to_dcache = 0;
            return;
        }
        // if dtlb hit get the physical address from TLB and pass to cache
        uint64_t vaddr_offset = vaddr & 0xfff;
        paddr = paddr << 12;
        uint64_t cache_access_paddr = paddr + vaddr_offset;
        load_store_entry.inst_fetch.tlb_complete_cycle = common_clock + TLB_HIT;
        load_store_entry.inst_fetch.tlb_fetched = true;

        // access dcache n case of dtlb hit
        bool dl1hit = dcache_hit(node_id, core_id, cache_access_paddr, vaddr, load_store_entry);
        if (!dl1hit)
        {
            if (dl1_miss_buffer[node_id][core_id].size() < mshr)
            {
                dcache_miss(node_id, core_id, cache_access_paddr, vaddr, load_store_entry);
                load_store_buffer[node_id][core_id].pop();
            }
        }
    }
    else
    {
        for (auto i = 0; i < dtlb_miss_buffer[node_id][core_id].size(); i++)
        {
            if ((vaddr & 0xfffffffff000) == (dtlb_miss_buffer[node_id][core_id][0].ins_vaddr & 0xfffffffff000))
            {
                return;
            }
        }
        if (dtlb_miss_buffer[node_id][core_id].size() < mshr)
        {
            DTLB_MISS(load_store_entry, node_id, core_id);
        }
    }
}

void DTLB_MISS(miss_queue &miss_temp, int node_id, int core_id)
{
    uint64_t vaddr = miss_temp.addr;
    uint64_t paddr = NULL;
    bool page_found_in_page_table = false;
    page_found_in_page_table = page_table_walk(_pgd[node_id], vaddr, paddr);
    if (page_found_in_page_table)
    {
        miss_temp.complete_cycle = miss_temp.request_cycle + TLB_MISS;
    }
    else
    {
        bool new_remote_chunk_allocated = false;
        handle_page_fault(_pgd[node_id], node_id, vaddr, paddr, L[node_id], new_remote_chunk_allocated); // handle by allocating a new page in Local or Remote

        if (new_remote_chunk_allocated == false)
            miss_temp.complete_cycle = miss_temp.request_cycle + 10; // page_fault complete after 9ms if free page (local/remote) is there
        else
        {
            // if we are allocating remote page and remote memory is not available,
            miss_temp.complete_cycle = miss_temp.request_cycle + 20; // add 25ms, first 16ns to allocate a remote memory chunk by global memory,
                                                                     // then page allocation
        }
    }
    miss_temp.paddr = paddr;
    dtlb_miss_buffer[node_id][core_id].push_back(miss_temp);
    load_store_buffer[node_id][core_id].pop();
    // bool present_in_mshr=false;
    // for(auto miss_buffer_entry:itlb_miss_buffer[node_id][core_id])
    // {
    //     //36 bits same = same page
    //     //next 6 same = same block so 42 bits same
    //         present_in_mshr=miss_buffer_entry.ins_vaddr&(0xfffffffff000) == vaddr&(0xfffffffff000);
    // }
    // if(!present_in_mshr)
    // {
    //     itlb_miss_buffer[node_id][core_id].push_back(miss_temp);
    // }
}

void dtlb_mshr_update(int node_id, int core_id)
{
    for (auto i = dtlb_miss_buffer[node_id][core_id].begin(); i != dtlb_miss_buffer[node_id][core_id].end(); i++)
    {
        miss_queue miss_buffer_entry = (*i);
        if (miss_buffer_entry.complete_cycle <= common_clock)
        {
            uint64_t vaddr = miss_buffer_entry.ins_vaddr;
            int proc_id = miss_buffer_entry.inst_fetch.ins.proc_id;
            uint64_t paddr = miss_buffer_entry.paddr;
            int dtlb_id = (node_id * core_count) + core_id;
            dtlbs[dtlb_id].ReplaceTLB(vaddr, TLB_BASE::ACCESS_TYPE_LOAD, proc_id, paddr);
            miss_buffer_entry.inst_fetch.tlb_complete_cycle = common_clock + TLB_HIT;
            miss_buffer_entry.inst_fetch.tlb_fetched = true;
            bool dl1hit = dcache_hit(node_id, core_id, paddr, vaddr, miss_buffer_entry);
            if (!dl1hit)
            {
                if (dl1_miss_buffer[node_id][core_id].size() < mshr)
                {
                    dcache_miss(node_id, core_id, paddr, vaddr, miss_buffer_entry);
                    dtlb_miss_buffer[node_id][core_id].erase(i);
                }
            }
            dtlb_to_dcache = 1;
            break;
        }
    }
}

bool dcache_hit(int node_id, int core_id, uint64_t cache_access_paddr, uint64_t vaddr, miss_queue &load_store_entry)
{
    bool dl1hit = false;
    load_store_entry.source_cache = 'D';
    CACHE_BASE::ACCESS_TYPE acctype = CACHE_BASE::ACCESS_TYPE_LOAD;
    if (load_store_entry.write)
        acctype = CACHE_BASE::ACCESS_TYPE_STORE;
    if (load_store_entry.cache_access_size > 4)
    {
        int line_read = 0;
        bool linehit[2];
        dl1hit = dl1s[core_count * node_id + core_id].AccessMultiCacheLine(cache_access_paddr,
                                                                           load_store_entry.cache_access_size, acctype, load_store_entry.inst_fetch.ins.proc_id,
                                                                           line_read, line_hit);
    }
    else
    {
        dl1hit = dl1s[core_count * node_id + core_id].AccessSingleCacheLine(cache_access_paddr, acctype, load_store_entry.inst_fetch.ins.proc_id);
    }
    if (dl1hit)
    {
        load_store_entry.complete_cycle = common_clock + L1_HIT;
        response_queue[node_id][core_id].push(load_store_entry);
        load_store_buffer[node_id][core_id].pop();
    }
    return dl1hit;
}

void dcache_miss(int node_id, int core_id, uint64_t cache_access_paddr, uint64_t vaddr, miss_queue load_store_entry)
{
    load_store_entry.source_cache = 'D';
    load_store_entry.request_cycle = common_clock;
    load_store_entry.complete_cycle = -1;
    load_store_entry.L1_cache_miss_complete_cycle = common_clock + L1_HIT;
    dl1_miss_buffer[node_id][core_id].push(load_store_entry);

    // int line_read = 0;
    // bool linehit[2];
    // bool l2hit = l2s[core_count * node_id + core_id].AccessMultiCacheLine(cache_access_paddr,
    //                                                                       load_store_entry.cache_access_size, acctype, load_store_entry.inst_fetch.ins.proc_id,
    //                                                                       line_read, line_hit);
    // bool l3hit = false;

    // if (l2hit)
    // {
    //     load_store_entry.complete_cycle = common_clock + L2_HIT;
    // }
    // // also check for l3hit, directly adding complete cycle in limshr also
    // else if (!l2hit && l2_miss_buffer[node_id][core_id].size() < mshr)
    // {
    //     l3hit = ul3[node_id].AccessMultiCacheLine(cache_access_paddr, load_store_entry.cache_access_size, acctype, load_store_entry.inst_fetch.ins.proc_id, line_read, linehit);

    //     if (l3hit)
    //     {
    //         load_store_entry.complete_cycle = common_clock + L3_HIT;
    //     }
    //     else if (!l3hit && l3_miss_buffer[node_id].size() < mshr)
    //     {
    //         load_store_entry.complete_cycle = 0;
    //     }
    //     // Add entry in L2_mshr and L3_mshr if applicable
    //     l2_cache_search(node_id, core_id, cache_access_paddr, vaddr, load_store_entry, l3hit);
    // }
}

void dcache_mshr_update(int node_id, int core_id)
{

    // removing completed mshr enteries and replacing the victim cache blocks with new blocks and performing write-back
    // if modified block is replaced
    for (auto it = dl1_miss_buffer[node_id][core_id].begin(); it != dl1_miss_buffer.end(); it++)
    {
        if (it.complete_cycle <= common_clock && it.complete_cycle > 0)
        {
            int line_read = 0;
            bool linehit[2];
            CACHE_BASE::ACCESS_TYPE acctype = CACHE_BASE::ACCESS_TYPE_LOAD;
            if ((*it).write)
                acctype = CACHE_BASE::ACCESS_TYPE_STORE;
            dl1s[core_count * node_id + core_id].AccessMultiCacheLine(cache_access_paddr, load_store_entry.cache_access_size, acctype, load_store_entry.inst_fetch.ins.proc_id, line_read, linehit);
            // no block present simply replace and get the corresponding victim
            uint64_t paddr = (*it).cache_access_paddr;
            for (int i = 0; i < line_read; i++)
            {
                if (linehit[i] == 0)
                {
                    // address of the victim block to write back to memory
                    uint64_t victim_addr = NULL;
                    bool victim_modified = dl1s[core_count * node_id + core_id].ReplaceCACHE(paddr, acctype, (*it).inst_fetch.ins.proc_id, victim_addr);
                    if (victim_modified)
                    {
                        int cache_level = 1; // cache level from which write-back should happen
                        // need address of replaced block
                        write_back_to_memory(node_id, core_id, cache_level, victim_addr, (*it).inst_fetch.ins.proc_id, acctype);
                    }
                    GetNextBlockAddress(paddr, (*it).inst_fetch.ins.proc_id);
                }
            }
            dl1_miss_buffer[node_id][core_id].erase(it);
            if (dl1_miss_buffer[node_id][core_id].empty())
            {
                break;
            }
        }
    }
}

void itlb_search(int node_id, int core_id)
{
    INST temp;
    if (!core_inst_stream[node_id][core_id].empty())
        temp = core_inst_stream[node_id][core_id].front();
    else
        return;

    uint64_t vaddr = temp.addr;
    // int core_id=temp.threadid%core_count;
    int itlb_id = (node_id * core_count) + core_id;
    uint64_t paddr = NULL;

    // if(core_id==i)
    //     cout<<"good";
    // else
    //     cout<<"bad";

    bool itlb_hit = false;
    itlb_hit = itlbs[itlb_id].AccessTLB(vaddr, TLB_BASE::ACCESS_TYPE_LOAD, temp.proc_id, paddr);
    if (itlb_hit)
    {
        // perform action for ITLB hit
        INST_FETCH_QUEUE INST_temp = ITLB_HIT(temp, node_id, core_id);
        uint64_t vaddr_offset = vaddr & 0xfff;
        paddr = paddr << 12;
        uint64_t cache_access_paddr = paddr + vaddr_offset;

        // access_cache  //read instructions of whole cache-line,
        // don't access it for each instruction referring to same cacheline

        // search in core icache
        icache_search(node_id, core_id, cache_access_paddr, vaddr, INST_temp);
    }
    else
    {
        if (itlb_miss_buffer[node_id][core_id].size() < 1) // mshr for itlb is taken 1.
        {
            ITLB_MISS(temp, node_id, core_id);
        }
    }
}

INST_FETCH_QUEUE ITLB_HIT(INST &temp, int node_id, int coreid)
{
    INST_FETCH_QUEUE INST_temp;
    INST_temp.ins = temp;
    INST_temp.tlb_fetched = true;
    INST_temp.request_cycle = common_clock;
    INST_temp.tlb_complete_cycle = INST_temp.request_cycle + TLB_HIT;
    return INST_temp;
}

void ITLB_MISS(INST &temp, int node_id, int core_id)
{
    miss_queue miss_temp;
    miss_temp.ins_id = temp.ins_id;
    miss_temp.inst_fetch.ins.proc_id = temp.proc_id;
    miss_temp.ins_vaddr = temp.addr;
    miss_temp.node_id = node_id;
    miss_temp.thread_id = temp.threadid;
    miss_temp.request_cycle = common_clock;
    miss_temp.complete_cycle = -1;

    uint64_t vaddr = temp.addr;
    uint64_t paddr = NULL;
    bool page_found_in_page_table = false;
    page_found_in_page_table = page_table_walk(_pgd[node_id], vaddr, paddr);
    if (page_found_in_page_table)
    {
        miss_temp.complete_cycle = miss_temp.request_cycle + TLB_MISS;
    }
    else
    {
        bool new_remote_chunk_allocated = false;
        handle_page_fault(_pgd[node_id], node_id, vaddr, paddr, L[node_id], new_remote_chunk_allocated); // handle by allocating a new page in Local or Remote

        if (new_remote_chunk_allocated == false)
            miss_temp.complete_cycle = miss_temp.request_cycle + 10; // page_fault complete after 9ms if free page (local/remote) is there
        else
        {
            // if we are allocating remote page and remote memory is not available,
            miss_temp.complete_cycle = miss_temp.request_cycle + 20; // add 25ms, first 16ns to allocate a remote memory chunk by global memory,
                                                                     // then page allocation
        }
    }
    miss_temp.paddr = paddr;
    itlb_miss_buffer[node_id][core_id].push_back(miss_temp);
    // bool present_in_mshr=false;
    // for(auto miss_buffer_entry:itlb_miss_buffer[node_id][core_id])
    // {
    //     //36 bits same = same page
    //     //next 6 same = same block so 42 bits same
    //         present_in_mshr=miss_buffer_entry.ins_vaddr&(0xfffffffff000) == vaddr&(0xfffffffff000);
    // }
    // if(!present_in_mshr)
    // {
    //     itlb_miss_buffer[node_id][core_id].push_back(miss_temp);
    // }
}

void itlb_mshr_update(int node_id, int core_id)
{
    if (itlb_miss_buffer[node_id][core_id].size() == 1)
    {
        miss_queue miss_buffer_entry = itlb_miss_buffer[node_id][core_id][0];
        if (miss_buffer_entry.complete_cycle == common_clock)
        {
            uint64_t vaddr = miss_buffer_entry.ins_vaddr;
            int proc_id = miss_buffer_entry.inst_fetch.ins.proc_id;
            uint64_t paddr = miss_buffer_entry.paddr;
            int itlb_id = (node_id * core_count) + core_id;
            itlbs[itlb_id].ReplaceTLB(vaddr, TLB_BASE::ACCESS_TYPE_LOAD, proc_id, paddr);
            itlb_miss_buffer[node_id][core_id].erase(itlb_miss_buffer[node_id][core_id].begin());
        }
    }
}

void icache_search(int node_id, int core_id, uint64_t cache_access_paddr, uint64_t vaddr, INST_FETCH_QUEUE INST_temp)
{
    // cout<<"\nvaddr"<<hex<<vaddr<<" paddr "<<cache_access_paddr<<dec<<" id "<<INST_temp.ins.ins_id;
    // cin.get();
    // search in IL1 Cache
    INST inst = INST_temp.ins;

    bool il1hit = il1s[core_count * node_id + core_id].AccessSingleCacheLine(cache_access_paddr, CACHE_BASE::ACCESS_TYPE_LOAD, inst.proc_id);
    if (il1hit && il1_miss_buffer[node_id][core_id].empty() &&
        inst_queue[node_id][core_id].size() < iqueue_size &&
        il1_wait_buffer[node_id][core_id].empty())
    {
        cout << "\thit";
        // hit in IL1 Cache
        INST_temp.mem_fetched = true;
        INST_temp.mem_complete_cycle = common_clock + L1_HIT; // commom cycle should be added in place of request cycle
        if (last_ins_id[node_id][core_id] == core_inst_stream[node_id][core_id].front().ins_id - 1)
        {
            INST_temp.mem_complete_cycle = common_clock;
            inst_queue[node_id][core_id].push(INST_temp);
            last_ins_id[node_id][core_id]++;
            core_inst_stream[node_id][core_id].pop();

            // fetch subsequent instructions in the same block and add to ins_queue
            fetch_ins_in_same_block(node_id, core_id, INST_temp);
        }
    }
    else if (!il1hit)
    {
        cout << "\tmiss";
        miss_queue mshr_temp;
        // Add instr in wait/miss buffer and check in L2.
        if (il1_miss_buffer[node_id][core_id].size() < mshr)
        {
            // check present in missbuffer or not. If not present, add in miss_buffer, otherwise in wait_buffer
            bool miss_buffer_present = false;
            for (auto miss_buffer_entry : il1_miss_buffer[node_id][core_id])
            {
                // 36 bits same = same page
                // next 6 same = same block so 42 bits same
                if ((miss_buffer_entry.ins_vaddr & 0xffffffffffc0) == (vaddr & 0xffffffffffc0))
                {
                    miss_buffer_present = true;
                    mshr_temp = miss_buffer_entry;
                }
            }

            miss_queue miss_new_entry;
            miss_new_entry.inst_fetch = INST_temp;
            miss_new_entry.ins_id = INST_temp.ins.ins_id;
            miss_new_entry.paddr = cache_access_paddr;
            miss_new_entry.ins_vaddr = vaddr;
            miss_new_entry.node_id = node_id;
            miss_new_entry.thread_id = inst.threadid;
            miss_new_entry.request_cycle = INST_temp.request_cycle;
            miss_new_entry.source_cache = 'I';
            miss_new_entry.core_id = core_id;
            miss_new_entry.complete_cycle = -1;
            miss_new_entry.write = false;
            miss_new_entry.L1_cache_miss_complete_cycle = common_clock + L1_HIT;

            if (!miss_buffer_present)
            {
                // check in L2
                // bool l2hit = l2s[core_count * node_id + core_id].AccessSingleCacheLine(cache_access_paddr, CACHE_BASE::ACCESS_TYPE_LOAD, inst.proc_id);

                // bool l3hit = false;

                // if (l2hit)
                // {
                //     miss_new_entry.complete_cycle = common_clock + L2_HIT;
                // }
                // // also check for l3hit, directly adding complete cycle in limshr also
                // else if (!l2hit && l2_miss_buffer[node_id][core_id].size() < mshr)
                // {
                //     l3hit = ul3[node_id].AccessSingleCacheLine(cache_access_paddr, CACHE_BASE::ACCESS_TYPE_LOAD, inst.proc_id);
                //     if (l3hit)
                //     {
                //         miss_new_entry.complete_cycle = common_clock + L3_HIT;
                //     }
                //     else if (!l3hit && l3_miss_buffer[node_id].size() < mshr)
                //     {
                //         miss_new_entry.complete_cycle = 0;
                //     }
                //     // Add entry in L2_mshr and L3_mshr if applicable
                //     l2_cache_search(node_id, core_id, cache_access_paddr, vaddr, miss_new_entry, l3hit);
                // }

                // add the entry to the L1_mshr
                il1_miss_buffer[node_id][core_id].push_back(miss_new_entry);
                core_inst_stream[node_id][core_id].pop();
            }
            else if (miss_buffer_present)
            {
                // fetch all the subsequent instructions in same block if there and add to wait buffer
                get_ins_in_same_block(node_id, core_id, miss_new_entry);
            }
        }
    }
}

void l2_cache_search(int node_id, int core_id, uint64_t cache_access_paddr, uint64_t vaddr, miss_queue miss_new_entry, bool l3hit)
{
    l2_miss_buffer[node_id][core_id].push_back(miss_new_entry);
    if (l3_miss_buffer[node_id].size() < mshr)
        l3_cache_search(node_id, cache_access_paddr, vaddr, miss_new_entry, l3hit);
}

void l3_cache_search(int node_id, uint64_t cache_access_paddr, uint64_t vaddr, miss_queue miss_new_entry, bool l3hit)
{
    if (!l3hit)
    {
        mem_trans_id[node_id]++;
        miss_new_entry.mem_trans_id = mem_trans_id[node_id];
        miss_new_entry.complete_cycle = miss_new_entry.request_cycle + 60;
        l3_miss_buffer[node_id].push_back(miss_new_entry);
        // send_to_memory(miss_new_entry,node_id);
    }
}

// On cache-hit store all the instructions belonging to same cache-line in temp queue
// will be pushed directly to ins_queue if space available, otherwise will be pushed later
void fetch_ins_in_same_block(int node_id, int core_id, INST_FETCH_QUEUE INST_temp)
{
    INST temp1 = core_inst_stream[node_id][core_id].front();
    uint64_t cur_miss_blockaddr = INST_temp.ins.addr & (0xffffffffffc0);
    miss_queue miss_new_entry;
    miss_new_entry.inst_fetch = INST_temp;
    miss_new_entry.ins_id = INST_temp.ins.ins_id;
    while (temp1.addr & (0xffffffffffc0) == cur_miss_blockaddr)
    {
        INST_temp.ins = temp1;
        temp[node_id][core_id].push(miss_new_entry);
        core_inst_stream[node_id][core_id].pop();
        last_ins_id[node_id][core_id]++;
        if (!core_inst_stream[node_id][core_id].empty())
        {
            temp1 = core_inst_stream[node_id][core_id].front();
        }
        else
            break;
    }
}

// on cache-miss, if subsequent instructions belong to same block, don't add to mshr,
// rather add to this wait buffer
void get_ins_in_same_block(int node_id, int core_id, miss_queue miss_new_entry)
{
    INST temp = core_inst_stream[node_id][core_id].front();
    uint64_t cur_miss_blockaddr = (miss_new_entry.ins_vaddr & 0xffffffffffc0);
    while ((temp.addr & 0xffffffffffc0) == (cur_miss_blockaddr))
    {
        miss_new_entry.inst_fetch.ins = temp;
        miss_new_entry.ins_vaddr = temp.addr;
        miss_new_entry.ins_id = temp.ins_id;
        il1_wait_buffer[node_id][core_id].push_back(miss_new_entry);
        core_inst_stream[node_id][core_id].pop();
        if (!core_inst_stream[node_id][core_id].empty())
            temp = core_inst_stream[node_id][core_id].front();
        else
            break;
    }
}

void update_icache_mshr(int node_id, int core_id)
{
    // replace the cache entry whenever a hit is completed

    if (il1_miss_buffer[node_id][core_id].size() > 0)
        if (il1_miss_buffer[node_id][core_id][0].complete_cycle <= common_clock && il1_miss_buffer[node_id][core_id][0].complete_cycle > 0)
            il1s[core_count * node_id + core_id].ReplaceCACHE((il1_miss_buffer[node_id][core_id][0]).paddr, CACHE_BASE::ACCESS_TYPE_LOAD, (il1_miss_buffer[node_id][core_id][0]).inst_fetch.ins.proc_id);

    // sending fetched instructions from temp queue to ins_queue
    // temp queue holds all the instructions belonging to same cache-line
    while (!temp[node_id][core_id].empty() && inst_queue[node_id][core_id].size() < iqueue_size)
    {
        INST_FETCH_QUEUE ins1 = temp[node_id][core_id].front().inst_fetch;
        // if(last_ins_id[node_id][core_id]==ins1.ins.ins_id-1)
        {
            ins1.mem_complete_cycle = common_clock;
            inst_queue[node_id][core_id].push(ins1);
            temp[node_id][core_id].pop();
            // last_ins_id[node_id][core_id]++;
        }
    }
    // if still space left in ins_queue, send more fetched instructions from L1-mshr completed enteries
    if (inst_queue[node_id][core_id].size() < iqueue_size)
    {
        // find the mshr entry with completed memory-access

        if (!il1_miss_buffer[node_id][core_id].empty())
        {
            if (il1_miss_buffer[node_id][core_id][0].complete_cycle <= common_clock &&
                il1_miss_buffer[node_id][core_id][0].complete_cycle > 0 &&
                last_ins_id[node_id][core_id] == il1_miss_buffer[node_id][core_id][0].inst_fetch.ins.ins_id - 1)
            {
                // push to ins_queue
                INST_FETCH_QUEUE ins1 = il1_miss_buffer[node_id][core_id][0].inst_fetch;
                last_ins_id[node_id][core_id]++;
                ins1.mem_complete_cycle = common_clock;
                inst_queue[node_id][core_id].push(ins1);

                uint64_t vaddr = il1_miss_buffer[node_id][core_id][0].ins_vaddr;
                uint64_t ins_id = il1_miss_buffer[node_id][core_id][0].inst_fetch.ins.ins_id;
                il1_miss_buffer[node_id][core_id].erase(il1_miss_buffer[node_id][core_id].begin());

                // get all the instruction enteries from the wait buffer that belongs to same cache-line
                // a related miss of which is just popped from mshr and pushed to ins_queue
                while (!il1_wait_buffer[node_id][core_id].empty() && (vaddr & 0xffffffffffc0) == (il1_wait_buffer[node_id][core_id][0].ins_vaddr & 0xffffffffffc0))
                {
                    if (il1_wait_buffer[node_id][core_id][0].ins_id - 1 == last_ins_id[node_id][core_id])
                    {
                        last_ins_id[node_id][core_id]++;
                        temp[node_id][core_id].push(il1_wait_buffer[node_id][core_id][0]);
                        il1_wait_buffer[node_id][core_id].erase(il1_wait_buffer[node_id][core_id].begin());
                    }
                    if (il1_wait_buffer[node_id][core_id].empty())
                        break;
                }
            }
        }
    }

    int64_t tempid = -1;
    if (!il1_miss_buffer[node_id][core_id].empty())
    {
        tempid = il1_miss_buffer[node_id][core_id][0].ins_id;
    }
    if (last_ins_id[node_id][core_id] + 1 != tempid &&
        temp[node_id][core_id].empty() && !il1_wait_buffer[node_id][core_id].empty())
    {
        if (last_ins_id[node_id][core_id] == il1_wait_buffer[node_id][core_id][0].ins_id - 1)
        {
            uint64_t tempaddr = il1_wait_buffer[node_id][core_id][0].ins_vaddr;
            tempaddr = tempaddr & 0xffffffffffc0;
            while (tempaddr == (il1_wait_buffer[node_id][core_id][0].ins_vaddr & 0xffffffffffc0))
            {
                temp[node_id][core_id].push(il1_wait_buffer[node_id][core_id][0]);
                il1_wait_buffer[node_id][core_id].erase(il1_wait_buffer[node_id][core_id].begin());
                last_ins_id[node_id][core_id]++;
                if (il1_wait_buffer[node_id][core_id].empty())
                    break;
            }
        }
    }
    // from newly filled temp to ins_queue if space left in ins_queue, otherwise try next cycle
    while (inst_queue[node_id][core_id].size() < iqueue_size && !temp[node_id][core_id].empty())
    {
        // if(last_ins_id[node_id][core_id]==temp[node_id][core_id].front().ins_id-1)
        {
            temp[node_id][core_id].front().inst_fetch.mem_complete_cycle = common_clock;
            inst_queue[node_id][core_id].push(temp[node_id][core_id].front().inst_fetch);
            temp[node_id][core_id].pop();
            // last_ins_id[node_id][core_id]++;
        }
        // else
        //     break;
    }
}

void update_l2_mshr(int node_id, int core_id)
{

    // checking the completion of miss requests in the dcache and start L2 cache search
    // if hit in L2, update completion time in the dcachae, otherwise put in L2-mshr
    CACHE_BASE::ACCESS_TYPE acctype = CACHE_BASE::ACCESS_TYPE_LOAD;
    bool ok = 0;
    if (ok)
    {
        if (il1_miss_buffer[node_id][core_id].size() > 0)
        {
            for (auto it = il1_miss_buffer[node_id][core_id].begin(); it != il1_miss_buffer.end(); it++)
            {
                miss_queue load_store_entry = (*it);
                if (it.L1_cache_miss_complete_cycle == common_clock)
                {
                    bool l2hit = l2s[core_count * node_id + core_id].AccessSingleCacheLine(cache_access_paddr,
                                                                                           acctype, load_store_entry.inst_fetch.ins.proc_id, );

                    if (l2hit)
                    {
                        (*it).complete_cycle = common_clock + L2_HIT;
                        break;
                    }
                    else
                    {
                        (*it).L2_cache_miss_complete_cycle = common_clock + L2_HIT;
                        l2_miss_buffer[node_id][core_id].push_back((*it));
                        // miss
                    }
                }
            }
        }
    }
    else
    {
        if (dl1_miss_buffer[node_id][core_id].size() > 0)
        {
            for (auto it = dl1_miss_buffer[node_id][core_id].begin(); it != dl1_miss_buffer.end(); it++)
            {
                miss_queue load_store_entry = (*it);
                if (it.L1_cache_miss_complete_cycle == common_clock)
                {
                    int line_read = 0;
                    bool linehit[2];
                    if (load_store_entry.iswrite)
                        acctype = CACHE_BASE::ACCESS_TYPE_STORE;

                    bool l2hit = l2s[core_count * node_id + core_id].AccessMultiCacheLine(cache_access_paddr,
                                                                                          load_store_entry.cache_access_size, acctype, load_store_entry.inst_fetch.ins.proc_id,
                                                                                          line_read, line_hit);

                    if (l2hit)
                    {
                        (*it).complete_cycle = common_clock + L2_HIT;
                        break;
                    }
                    else
                    {
                        (*it).L2_cache_miss_complete_cycle = common_clock + L2_HIT;
                        l2_miss_buffer[node_id][core_id].push_back((*it));
                        // miss
                    }
                }
            }
        }
    }
    // removing from mshr whenever a pending miss completes
    for (auto it = l2_miss_buffer[node_id][core_id].begin(); it != l2_miss_buffer[node_id][core_id].end(); ++it)
    {
        auto miss_buffer_entry = (*it);
        // Replace the cache entery after a hit completes
        if (miss_buffer_entry.complete_cycle <= common_clock && miss_buffer_entry.complete_cycle > 0)
        {
            if (miss_buffer_entry.source_cache == 'I')
            {
                l2_miss_buffer[node_id][core_id].erase(it);
                l2s[core_count * node_id + core_id].ReplaceCACHE(miss_buffer_entry.paddr, CACHE_BASE::ACCESS_TYPE_LOAD, miss_buffer_entry.inst_fetch.ins.threadid);
                if (l2_miss_buffer[node_id][core_id].size() == 0)
                    break;
            }
            else if (miss_buffer_entry.source_cache == 'D')
            {
                int line_read = 0;
                bool line_hit[2];
                CACHE_BASE::ACCESS_TYPE acctype = CACHE_BASE::ACCESS_TYPE_LOAD;
                if ((*it).write)
                    acctype = CACHE_BASE::ACCESS_TYPE_STORE;
                l2s[core_count * node_id + core_id].AccessMultiCacheLine(cache_access_paddr, load_store_entry.cache_access_size, acctype, load_store_entry.inst_fetch.ins.proc_id, line_read, linehit);
                // no block present simply replace and get the corresponding victim
                uint64_t paddr = (*it).cache_access_paddr;
                for (int i = 0; i < line_read; i++)
                {
                    if (linehit[i] == 0)
                    {
                        // address of the victim block to write back to memory
                        uint64_t victim_addr = NULL;
                        bool victim_modified = l2s[core_count * node_id + core_id].ReplaceCACHE(paddr, acctype, (*it).inst_fetch.ins.proc_id, victim_addr);
                        if (victim_modified)
                        {
                            int cache_level = 2; // cache level from which write-back should happen
                            // need address of replaced block
                            write_back_to_memory(node_id, core_id, cache_level, victim_addr, (*it).inst_fetch.ins.proc_id, acctype);
                        }
                        GetNextBlockAddress(paddr, (*it).inst_fetch.ins.proc_id);
                    }
                }
                l2_miss_buffer[node_id][core_id].erase(it);
                if (l2_miss_buffer[node_id][core_id].size() == 0)
                    break;
            }
        }
    }

    // if empty mshr is there, add pending misses from IL1 to L2
    auto it = il1_miss_buffer[node_id][core_id].begin();
    while (l2_miss_buffer[node_id][core_id].size() < mshr && it != il1_miss_buffer[node_id][core_id].end()) // require condition to stop this loop, check l1 buffer has such entry
    {
        for (it = il1_miss_buffer[node_id][core_id].begin(); it != il1_miss_buffer[node_id][core_id].end(); ++it)
        {
            auto miss_buffer_entry = *it;

            if (miss_buffer_entry.complete_cycle == -1 && miss_buffer_entry.cache_access_complete_cycle <= commmon_clock)
            {
                (*it).complete_cycle = 0;
                bool l3hit = ul3[node_id].AccessSingleCacheLine((*it).paddr, CACHE_BASE::ACCESS_TYPE_LOAD, (*it).inst_fetch.ins.proc_id);
                if (l3hit)
                {
                    (*it).complete_cycle = common_clock + L3_HIT;
                    l2_miss_buffer[node_id][core_id].push_back((*it));
                }
                else
                {
                    if (l3_miss_buffer[node_id].size() < mshr)
                    {
                        l3_miss_buffer[node_id].push_back((*it));
                        l2_miss_buffer[node_id][core_id].push_back((*it));
                    }
                    else
                    {
                        (*it).complete_cycle = -1;
                        l2_miss_buffer[node_id][core_id].push_back((*it));
                        (*it).complete_cycle = 0;
                    }
                }
                break;
            }
        }
    }

    // if empty mshr is there, add pending misses from DL1 to L2
    while (l2_miss_buffer[node_id][core_id].size() < mshr && it != dl1_miss_buffer[node_id][core_id].end()) // require condition to stop this loop, check l1 buffer has such entry
    {
        for (it = dl1_miss_buffer[node_id][core_id].begin(); it != dl1_miss_buffer[node_id][core_id].end(); ++it)
        {
            auto miss_buffer_entry = *it;

            if (miss_buffer_entry.complete_cycle == -1)
            {
                (*it).complete_cycle = 0;
                int line_read = 0;
                bool linehit[2];
                bool l2hit = l2s[core_count * node_id + core_id].AccessMultiCacheLine(cache_access_paddr, load_store_entry.cache_access_size, acctype, load_store_entry.inst_fetch.ins.proc_id, line_read, linehit);
                if (l2hit)
                {
                    (*it).complete_cycle = common_clock + L2_HIT;
                }
                else
                {
                    bool l3hit = ul3[node_id].AccessMultiCacheLine(cache_access_paddr, load_store_entry.cache_access_size, acctype, load_store_entry.inst_fetch.ins.proc_id, line_read, linehit);
                    if (l3hit)
                    {
                        (*it).complete_cycle = common_clock + L3_HIT;
                        l2_miss_buffer[node_id][core_id].push_back((*it));
                    }
                    else
                    {
                        if (l3_miss_buffer[node_id].size() < mshr)
                        {
                            l3_miss_buffer[node_id].push_back((*it));
                            l2_miss_buffer[node_id][core_id].push_back((*it));
                        }
                        else
                        {
                            (*it).complete_cycle = -1;
                            l2_miss_buffer[node_id][core_id].push_back((*it));
                            (*it).complete_cycle = 0;
                        }
                    }
                }
                break;
            }
        }
    }
}

void update_l3_mshr(int node_id)
{
    for (auto it = l3_miss_buffer[node_id].begin(); it != l3_miss_buffer[node_id].end(); ++it)
    {
        miss_queue miss_buffer_entry = *it;
        if (miss_buffer_entry.complete_cycle <= common_clock && miss_buffer_entry.complete_cycle > 0)
        {
            if (miss_buffer_entry.source_cache == 'I')
            {
                l3_miss_buffer[node_id].erase(it);
                ul3[node_id].ReplaceCACHE(miss_buffer_entry.paddr, CACHE_BASE::ACCESS_TYPE_LOAD, miss_buffer_entry.inst_fetch.ins.threadid);
            }
            else if (miss_buffer_entry.source_cache == 'D')
            {
                int line_read = 0;
                bool line_hit[2];
                CACHE_BASE::ACCESS_TYPE acctype = CACHE_BASE::ACCESS_TYPE_LOAD;
                if ((*it).write)
                    acctype = CACHE_BASE::ACCESS_TYPE_STORE;
                ul3[node_id].AccessMultiCacheLine(cache_access_paddr, load_store_entry.cache_access_size, acctype, load_store_entry.inst_fetch.ins.proc_id, line_read, linehit);
                // no block present simply replace and get the corresponding victim
                uint64_t paddr = (*it).cache_access_paddr;
                for (int i = 0; i < line_read; i++)
                {
                    if (linehit[i] == 0)
                    {
                        // address of the victim block to write back to memory
                        uint64_t victim_addr = NULL;
                        bool victim_modified = ul3[node_id].ReplaceCACHE(paddr, acctype, (*it).inst_fetch.ins.proc_id, victim_addr);
                        if (victim_modified)
                        {
                            int cache_level = 3; // cache level from which write-back should happen
                            // need address of replaced block
                            write_back_to_memory(node_id, core_id, cache_level, victim_addr, (*it).inst_fetch.ins.proc_id, acctype);
                        }
                        GetNextBlockAddress(paddr, (*it).inst_fetch.ins.proc_id);
                    }
                }
                l3_miss_buffer[node_id].erase(it);
            }
        }

        if (l3_miss_buffer[node_id].size() == 0)
            break;
    }
    int core_id = 0;
    // to make sure the loop terminates when there are
    int complete_count[core_count];
    int total_count = -1;
    while (l3_miss_buffer[node_id].size() < mshr && total_count == 0)
    {
        total_count = 0;
        complete_count[core_id] = 0;
        for (auto it = l2_miss_buffer[node_id][core_id].begin(); it != l2_miss_buffer[node_id][core_id].end(); ++it)
        {
            auto miss_buffer_entry = *it;
            if (miss_buffer_entry.complete_cycle == -1)
            {
                complete_count[core_id] = 1;
                miss_buffer_entry.complete_cycle = 0;
                int line_read = 0;
                bool linehit[2];
                bool l3hit = ul3[node_id].AccessMultiCacheLine(cache_access_paddr, load_store_entry.cache_access_size, acctype, load_store_entry.inst_fetch.ins.proc_id, line_read, linehit);
                if (l3hit)
                {
                    (*it).complete_cycle = common_clock + L3_HIT;
                }
                else
                {
                    l3_cache_search(node_id, miss_buffer_entry.paddr, miss_buffer_entry.ins_vaddr, miss_buffer_entry, l3hit);
                    break;
                }
            }
        }

        for (int i = 0; i < core_count; i++)
        {
            total_count += complete_count[i];
        }
        core_id = (core_id + 1) % core_count;
    }
}

for (int i = 0; i < RS_Size; i++)
		{
			if (reservation_station[nodeid][0][i].valid == 1)
			{
				cout << "\nvalid " << reservation_station[nodeid][0][i].valid << " rob_ind " << reservation_station[nodeid][0][i].ROB_index << " ins_id " << reservation_station[nodeid][0][i].ins_id
					 << " Rreg-" << reservation_station[nodeid][0][i].RR[0] << " " << reservation_station[nodeid][0][i].reg_read_dependencies[0]
					 << " " << reservation_station[nodeid][0][i].RR[1] << " " << reservation_station[nodeid][0][i].reg_read_dependencies[1]
					 << " " << reservation_station[nodeid][0][i].RR[2] << " " << reservation_station[nodeid][0][i].reg_read_dependencies[2]
					 << " " << reservation_station[nodeid][0][i].RR[3] << " " << reservation_station[nodeid][0][i].reg_read_dependencies[3]
					 << " Rmemop-" << reservation_station[nodeid][0][i].mem_read_addr[0] << " " << reservation_station[nodeid][0][i].mem_read_dependencies[0]
					 << " " << reservation_station[nodeid][0][i].mem_read_addr[1] << " " << reservation_station[nodeid][0][i].mem_read_dependencies[1]
					 << " branch mis" << reservation_station[nodeid][0][i].branch_miss_predicted;
			}
		}
		cout << "\n\n";
		for (int i = 0; i < ROB_Size; i++)
		{
			if (reorder_buffer[nodeid][0].rob[i].status != -1)
			{
				cout << "\n iss " << reorder_buffer[nodeid][0].issue_ptr << " cmt " << reorder_buffer[nodeid][0].commit_ptr << " rs_ind " << reorder_buffer[nodeid][0].rob[i].INS_RS_Dependency.size()
					 << " wop" << reorder_buffer[nodeid][0].rob[i].mem_write_operand << " wreg-" << reorder_buffer[nodeid][0].rob[i].write_reg_num[0] << " "
					 << reorder_buffer[nodeid][0].rob[i].write_reg_num[1] << " " << reorder_buffer[nodeid][0].rob[i].write_reg_num[2] << " " << reorder_buffer[nodeid][0].rob[i].write_reg_num[3]
					 << " wrt_com " << reorder_buffer[nodeid][0].rob[i].write_completed << " status " << reorder_buffer[nodeid][0].rob[i].status << " cycle_req: " << reorder_buffer[nodeid][0].rob[i].request_cycle;
			}
		}
		for (int i = 0; i < num_exec_units; i++)
		{
			if (execution_unit[nodeid][0][i].status == 1)
				cout << "\nexecunit" << i << " rob_ind: " << execution_unit[nodeid][0][i].rob_index << "cyc:" << execution_unit[nodeid][0][i].cycle_completed;
		}
		cout << "\n";
		for (int i = 0; i < completed_inst_rob_index[nodeid][0].size(); i++)
		{
			cout << "\n com rob ind " << completed_inst_rob_index[nodeid][coreid][i];
		}

		for (int i = 0; i < load_store_queue[nodeid][0].size(); i++)
		{
			cout << "\nins_vaddr:" << load_store_queue[nodeid][0][i].ins_vaddr << "ld_str_id: " << load_store_queue[nodeid][0][i].ld_str_id << "acc_size:" << load_store_queue[nodeid][0][i].access_size << "rs_ind:"
				 << load_store_queue[nodeid][0][i].rs_index << "rob_ind:" << load_store_queue[nodeid][0][i].rob_index << "type:" << load_store_queue[nodeid][0][i].INS_type_ld_str;
		}

void update_caches(int node_id)
{
    miss_queue temp;
    temp.core_id = -1;
    for (auto it : l3_miss_buffer[node_id])
    {
        if (it.complete_cycle <= common_clock && it.complete_cycle > 0)
        {
            temp = it;
            break;
        }
    }

    if (temp.core_id != -1)
    {
        int size = l2_miss_buffer[node_id][temp.core_id].size();
        for (int i = 0; i < size; i++)
        {
            if (temp.ins_id == l2_miss_buffer[node_id][temp.core_id][i].ins_id)
            {
                l2_miss_buffer[node_id][temp.core_id][i].complete_cycle = temp.complete_cycle;
            }
        }

        size = il1_miss_buffer[node_id][temp.core_id].size();
        for (int i = 0; i < size; i++)
        {
            if (temp.ins_id == il1_miss_buffer[node_id][temp.core_id][i].ins_id)
            {
                cout << "\tid" << temp.ins_id << " com_cycle" << temp.complete_cycle;
                // cin.get();
                il1_miss_buffer[node_id][temp.core_id][i].complete_cycle = temp.complete_cycle;
            }
        }
    }
}

// write back the victim block to the next level of memory after completing the replacement
void write_back_to_memory(int node_id, int core_id, int cache_level, int16_t proc_id, uint64_t victim_addr, CACHE_BASE::ACCESS_TYPE acctype)
{
    bool l2hit = true, l3hit = true;
    if (cache_level == 1)
    {
        l2hit = l2s[core_count * node_id + core_id].AccessSingleCacheLine(victim_addr, acctype, proc_id);
        if (l2hit)
        {
            l2s[core_count * node_id + core_id].ReplaceCACHE(victim_addr, acctype, proc_id);
        }
    }
    if (cache_level == 2 || l2hit == false)
    {
        l3hit = ul3[node_id].AccessSingleCacheLine(victim_addr, acctype, proc_id);
        if (l3hit)
        {
            ul3[node_id].ReplaceCACHE(victim_addr, acctype, proc_id);
        }
    }
    if (cache_level == 3 || l3hit == false)
    {
        miss_queue miss_entry;
        miss_entry.paddr = victim_addr;
        miss_entry.mem_trans_id = 0;
        miss_entry.request_cycle = common_clock;
        miss_entry.write = true;
        send_to_memory(miss_entry, node_id);
    }
}

// access to memory is performed, either in local or remote
void send_to_memory(miss_queue miss_new_entry, int node_id)
{
    uint64_t mem_access_addr = miss_new_entry.paddr;
    bool isWrite = miss_new_entry.write;
    uint64_t start_cycle = miss_new_entry.request_cycle;
    uint64_t miss_cycle = miss_new_entry.request_cycle;
    uint64_t trans_id = miss_new_entry.mem_trans_id;

    unsigned long page_addr = get_page_addr(mem_access_addr);
    unsigned long page_offset_addr = mem_access_addr & (0x000000000fff);
    bool local = L[node_id].is_local(page_addr);

    if (local)
        obj.add_one_and_run(local_mem[node_id], mem_access_addr, isWrite, trans_id, start_cycle, miss_cycle, node_id);
    else
    {
        int remote_pool = L[node_id].find_remote_pool(page_addr);
        uint64_t remote_page_addr = L[node_id].remote_page_addr(page_addr, remote_pool);

        remote_page_addr = remote_page_addr << 12;
        uint64_t remote_mem_access_addr = remote_page_addr + page_offset_addr;
        remote_memory_access mem_request;
        mem_request.source = node_id;
        mem_request.dest = remote_pool;
        mem_request.mem_access_addr = remote_mem_access_addr;
        mem_request.cycle = start_cycle;
        mem_request.miss_cycle_num = miss_cycle;
        mem_request.trans_id = trans_id;
        mem_request.isWrite = isWrite;
        to_trans_layer(mem_request, packet_queue_node);
    }
}

void memory_request_completed()
{
}

bool page_table_walk(pgd &_pgd, uint64_t vaddr, uint64_t &paddr)
{
    pud *_pud;
    pmd *_pmd;
    pte *_pte;
    page *_page;

    unsigned long pgd_vaddr = 0L, pud_vaddr = 0L, pmd_vaddr = 0L, pte_vaddr = 0L, page_offset_addr = 0L;
    split_vaddr(pgd_vaddr, pud_vaddr, pmd_vaddr, pte_vaddr, page_offset_addr, vaddr);

    _pud = _pgd.access_in_pgd(pgd_vaddr);
    if (_pud == nullptr)
    {
        return false;
    }
    else if (_pud != nullptr)
    {
        _pmd = _pud->access_in_pud(pud_vaddr);
        if (_pmd == nullptr)
        {
            return false;
        }
        else if (_pmd != nullptr)
        {
            _pte = _pmd->access_in_pmd(pmd_vaddr);
            if (_pte == nullptr)
            {
                return false;
            }
            else if (_pte != nullptr)
            {
                _page = _pte->access_in_pte(pte_vaddr);
                if (_page == nullptr)
                {
                    return false;
                }
                else if (_page != nullptr)
                {
                    long int paddr = _page->get_page_base_addr(pte_vaddr);
                    // paddr=paddr<<12;
                    // paddr=paddr + page_offset_addr;
                    return true;
                }
            }
        }
    }
}

uint64_t page_counter[num_nodes] = {0};

void handle_page_fault(pgd &_pgd, int node_no, uint64_t vaddr, uint64_t &paddr, local_addr_space &L, bool &new_remote_chunk_allocated)
{
    pud *_pud;
    pmd *_pmd;
    pte *_pte;
    page *_page;

    unsigned long pgd_vaddr = 0L, pud_vaddr = 0L, pmd_vaddr = 0L, pte_vaddr = 0L, page_offset_addr = 0L;
    split_vaddr(pgd_vaddr, pud_vaddr, pmd_vaddr, pte_vaddr, page_offset_addr, vaddr);

    long int virt_page_addr = get_page_addr(vaddr);

    char page_local_remote = NULL; // tells that new page should be in local memory or remote

    if (page_counter[node_no] % 2 == 0 && L.free_local()) // follows alternate local-remote page allocation
    {
        page_local_remote = 'L';
        page_counter[node_no]++;
    }
    else if (page_counter[node_no] % 2 == 1 || !L.free_local())
    {
        page_local_remote = 'R';
        page_counter[node_no]++;
    }

    if (page_local_remote == 'R' && !L.free_remote())
    {
        new_remote_chunk_allocated = true;
        int remote_pool = round_robin_pool_select();
        int chunk_size = 4;
        request_remote_memory(L, R[remote_pool], chunk_size);
    }

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
        long int pte_paddr = L.allocate_local_page(); // will request a new page from local memory
        _pte->add_in_pte(pte_vaddr, pte_paddr);       // new page entry at vaddr to paddr map
        _page = _pte->access_in_pte(pte_vaddr);
    }

    paddr = _page->get_page_base_addr(pte_vaddr);
    // paddr=paddr<<12;
    // paddr=paddr + page_offset_addr;
}

// int main()
// {
//     uint64_t addr=0x177;
//     uint64_t paddr=NULL;
//     bool hit=itlbs[0].AccessTLB(addr,TLB_BASE::ACCESS_TYPE_LOAD,1,paddr);
//     uint64_t a=4;
//     itlbs[0].ReplaceTLB(addr,TLB_BASE::ACCESS_TYPE_LOAD,1,a);
// cout<<hit<<" paddr "<<paddr;
//     hit=itlbs[0].AccessTLB(addr,TLB_BASE::ACCESS_TYPE_LOAD,1,paddr);
//     cout<<"\n"<<hit<<" paddr "<<paddr;
//     return 0;

// }

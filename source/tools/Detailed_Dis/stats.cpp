int total_local_access[num_nodes][core_count]={0};
int total_remote_access[num_nodes][core_count]={0};


// add at line 1014 in tlb_cache
void add_local_access_time(miss_queue temp)
{
   int node_id = temp.node_id;
   int core_id = temp.core_id;

   int time_taken = temp.complete_cycle - temp.request_cycle;
   total_local_access[node_id][core_id]+=time_taken;
}

// 
void add_remote_access_time(miss_queue temp)
{
        
}


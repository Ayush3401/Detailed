// per node cache size on central memory manager fore hot remote pages
#define total_remote_page_list_size_per_node 500
#define page_swap_time 1000 // per-page swap (background) time (In nano seconds)
int64_t background_pg_swap_clock[num_nodes];
int64_t resume_stall_clock[num_nodes] = {0};
int64_t last_migration_cycle[num_nodes] = {0};

#define initiliastion_epoch_length 1e6 // For initilaisng page migration parameters for the next epoch

const int max_victim_pages = 500;
bool migration_flag[num_nodes] = {0};
bool migrate_stall[num_nodes] = {0};

// keep a list of victim pages in the local memory
vector<uint64_t> victim_pages[num_nodes];

// hold a list of physcial addresses of remote pages to migrate
vector<uint64_t> remote_pages_to_migrate[num_nodes];

// hold a list of local addresses of remote pages to migrate
vector<uint64_t> local_pages_to_migrate[num_nodes];

// factor representing benefit/overhead we got by migrating the pages in the previous epoch (range : [-5,5])
int8_t migration_benefit_factor[num_nodes];

#define migration_benefit_per_mem_access 150 // number of cycles saved by migrating page to local from remote
#define TLB_shootdown_cycles 4000
#define TLB_miss_overhead 60                  // per page
#define cache_miss_overhead 60                // per access to migrate page which was invalidated in cache
#define max_number_of_cached_blocks_in_page 16 // out of 64 blocks of size 64Byte in a memory page of size 4096 bytes
// calculate overhead of migration to compare it later with migration benefits

// store the number of migrations performed during the whole simulation on a node
int16_t migration_counter[num_nodes] = {0};

// store the total cycles for migration benefit
uint64_t migration_benefit[num_nodes] = {0};

// store the total cycles of migration overhead
uint64_t migration_overhead[num_nodes] = {0};

// will count the number in local memory for the migrated pages, to calculate overall benefit
uint64_t global_benefit_counter[num_nodes] = {0};

// will store the physdical address of pages migrated during a page-migration for which benefit factor is calculated
vector<uint64_t> pages_migrated_in_feedback_cycle[num_nodes];

uint64_t update_migration_benefit(int node_id, uint64_t page_accessed)
{
    bool in_migrate_list = 0;

    for (auto it : pages_migrated_in_feedback_cycle[node_id])
    {
        // cout<<"\npage accessed "<<page_accessed;
        // cout<<"\npage in migrated "<<it<<dec;
        // cin.get();
        if (it == page_accessed)
        {
            global_benefit_counter[node_id]++;
            break;
        }
    }
}

void update_migrate_list_of_feedback_cycle(int node_id, uint64_t pages_migrated_back)
{

    for (auto it : local_pages_to_migrate[node_id])
    {
        auto found = find(pages_migrated_in_feedback_cycle[node_id].begin(), pages_migrated_in_feedback_cycle[node_id].end(), it);
        if (found != pages_migrated_in_feedback_cycle[node_id].end() && it == *(found))
        {
            pages_migrated_in_feedback_cycle[node_id].erase(found);
        }
    }

    // else
    // {
    //     pages_migrated_in_feedback_cycle[node_id].clear();
    //     pages_migrated_in_feedback_cycle[node_id] = remote_pages_to_migrate[node_id];
    // }
}

double calculate_overhead(int node_id, int pages_to_migrate)
{
    uint64_t tlb_overhead = pages_to_migrate * TLB_miss_overhead;
    uint64_t cache_overhead = pages_to_migrate * max_number_of_cached_blocks_in_page * cache_miss_overhead;
    uint64_t migration_overhead = TLB_shootdown_cycles + tlb_overhead + cache_overhead;
    return migration_overhead;
}

// add benefit by counting the number of memory accesses done in migrated local pages
int count_benefit(int node_id, std::ofstream &page_migration)
{
    double usage = global_benefit_counter[node_id] * migration_benefit_per_mem_access;
    double overhead = calculate_overhead(node_id, pages_migrated_in_feedback_cycle[node_id].size());
    double score = (usage - overhead) / overhead;
    score = score * 100;

    if (score < 0)
        score = max(-10, (int)score);
    else
        score = min(10, (int)score);

    page_migration.open(file2[node_id],ios::app);
    page_migration << "----------Feedback Migration CYcle----------------" << endl;
    page_migration << "Benefit: " << usage << "Overhead: " << overhead << "score: " << score;
    cout << "----------Feedback Migration CYcle----------------" << endl;
    cout << "\nmig count: " << migration_counter[node_id] << endl;
    cout << "Benefit: " << usage << "Overhead: " << overhead << "score: " << score;
    //cin.get();
    page_migration.close();

    return score;
}

// cache structure at centralized memory controller
struct Hot_page_acc_count
{
    uint64_t paddr;
    int32_t page_acc_cnt = 0;
};

// cache at centralized memory controller
vector<Hot_page_acc_count> hot_page_cache[num_nodes];

// Finds the page with minimum counter (is required once the cache at central memory controller get full
// and we need to replace it with a new entry with more access count)
int find_page_with_min_counter(int node_id)
{
    int32_t mini = 1000000000;
    int index = -1;
    for (int i = 0; i < hot_page_cache[node_id].size(); i++)
    {
        if (hot_page_cache[node_id][i].page_acc_cnt < mini)
        {
            mini = hot_page_cache[node_id][i].page_acc_cnt;
            index = i;
        }
    }
    return index;
}

// finds the index of the paddr in the hot page cache
int find_page_index_in_cache(int node_id, uint64_t paddr)
{
    int index = -1;
    for (int i = 0; i < hot_page_cache[node_id].size(); i++)
    {
        if (hot_page_cache[node_id][i].paddr == paddr)
        {
            index = i;
        }
    }
    return index;
}

// when page is accessed if it is remote =>
// 1. already present in cache => then just update the page_counter
// 2. not present and space is there => just add in the cache & get it from memory
// 3. not present and space is not there => find page with minimum access count
//     i. if min < count of new page => new entry would be added and remove the old entry
//     ii. else just update in local list
void add_to_hot_page_cache(uint64_t paddr, int32_t counter, int node_id)
{
    int ind = find_page_index_in_cache(node_id, paddr);
    // already there in the cache just update the counter
    if (ind != -1)
    {
        hot_page_cache[node_id][ind].page_acc_cnt = counter;
    }
    else
    {
        int sz = hot_page_cache[node_id].size();
        if (sz < total_remote_page_list_size_per_node)
        {
            Hot_page_acc_count temp;
            temp.paddr = paddr;
            temp.page_acc_cnt = counter;
            hot_page_cache[node_id].push_back(temp);
        }
        else
        {
            int replac_ind = find_page_with_min_counter(node_id);
            if (hot_page_cache[node_id][replac_ind].page_acc_cnt < counter)
            {
                hot_page_cache[node_id][replac_ind].paddr = paddr;
                hot_page_cache[node_id][replac_ind].page_acc_cnt = counter;
            }
            else
            {
                // no need to do anything
            }
        }
    }
}

bool setting_params[num_nodes] = {0};
int page_migration_threshold[num_nodes] = {0}; // PMT (Page Migration Threshold) per node
int num_pages_to_migrate[num_nodes] = {0};     // PTM (Pages To be Migrated) per node

// For every node per epoch access counter for all pages that are acessed in current epoch  (stored as Hash Table)
unordered_map<uint64_t, int16_t> per_epoch_page_access_counter[num_nodes];

// Increments the counter whenever a remote memory access took place (calls this in remote mem request case of send to memory in TLB Cache.cpp)
void update_hash_table_access_count(uint64_t paddr, int node_id)
{
    // if(migration_flag[node_id]==1)return;
    if (per_epoch_page_access_counter[node_id].find(paddr) == per_epoch_page_access_counter[node_id].end())
    {
        per_epoch_page_access_counter[node_id][paddr] = 1;
    }
    else
    {
        per_epoch_page_access_counter[node_id][paddr]++;
    }
    if (per_epoch_page_access_counter[node_id][paddr] >= page_migration_threshold[node_id])
    {
        add_to_hot_page_cache(paddr, per_epoch_page_access_counter[node_id][paddr], node_id); // updates the counter
        if (hot_page_cache[node_id].size() == num_pages_to_migrate[node_id])
        {
            // make the migration now
            migration_flag[node_id] = 1; // do the swapping in background and still continue with current local and remote addresses for pages to migrate
            background_pg_swap_clock[node_id] = common_clock + num_pages_to_migrate[node_id] * page_swap_time;
        }
    }
}

bool comp_by_acc_cnt(Hot_page_acc_count &a, Hot_page_acc_count &b)
{
    return a.page_acc_cnt > b.page_acc_cnt;
}

uint64_t setUpPhase_cycle[num_nodes] = {2000000}; // this cycle represent warmup phase at start , will not do any parameter setting

// after first epoch completed (or page migration stops due to more overhead than benefit) reset parameters, get stats and compute the benefit
void set_params(int node_id, std::ofstream &page_migration)
{

    int avg_access_cnt = 0;

    for (int i = 0; i < hot_page_cache[node_id].size(); i++)
    {
        avg_access_cnt = avg_access_cnt + hot_page_cache[node_id][i].page_acc_cnt;
    }
    int sz = (hot_page_cache[node_id].size());
    if(sz==0)
        sz=1;
    avg_access_cnt = avg_access_cnt / sz;
    cout << "Avg Access Count of all: " << avg_access_cnt << endl;
    sort(hot_page_cache[node_id].begin(), hot_page_cache[node_id].end(), comp_by_acc_cnt);

    int t[10] = {0};
    int facto = hot_page_cache[node_id].size() / 10;
    if(facto<1){
        facto=1;
    }
    for (int i = 0; i < 10; i++)
    {
        for (int j = i * facto; j < (i + 1) * facto && j < hot_page_cache[node_id].size(); j++)
        {
            t[i] = t[i] + hot_page_cache[node_id][j].page_acc_cnt;
        }
        t[i] /= facto;
        cout << "\navg t" << i << " =" << t[i];
    }
    // calculate standard deviation
    double mean = avg_access_cnt;
    double standard_dev = 0.0;
    for (int i = 0; i < 10; i++)
    {
        standard_dev += (t[i] - mean) * (t[i] - mean);
    }
    standard_dev = standard_dev / 10;
    standard_dev = sqrt(standard_dev);
    if(standard_dev<1){
        standard_dev = 1;
    }
    // using mean and standard deviation to set pages to migrate for that node
    int pages = 10 + facto + double(1 / 2) * (double)(avg_access_cnt / standard_dev) + double(avg_access_cnt / 5);
    num_pages_to_migrate[node_id] = min(total_remote_page_list_size_per_node, pages);
    // setting the PMT based on PTM
    avg_access_cnt = 0;
    for (int i = 0; i < num_pages_to_migrate[node_id]; i++)
    {
        avg_access_cnt += hot_page_cache[node_id][i].page_acc_cnt;
    }
    avg_access_cnt /= num_pages_to_migrate[node_id];
    page_migration_threshold[node_id] = 0.3 * avg_access_cnt;
    if(page_migration_threshold[node_id]<50){
        page_migration_threshold[node_id]=22;
    }

    page_migration.open(file2[node_id],ios::app);
    page_migration << "-----------------Initialsiation done--------------------" << endl;
    page_migration << "Pages to Migrate: " << num_pages_to_migrate[node_id] << endl;
    page_migration << "Pages Migration Threshold: " << page_migration_threshold[node_id] << endl;
    page_migration << "Avg Access Count: " << avg_access_cnt << endl;
    page_migration << "Standard DEV: " << standard_dev << endl;
    page_migration << "Hot Page Cache size: " << hot_page_cache[node_id].size() << endl;
    page_migration.close();

    cout << "-----------------Initialsiation done--------------------" << endl;
    cout << "Pages to Migrate: " << num_pages_to_migrate[node_id] << endl;
    cout << "Pages Migration Threshold: " << page_migration_threshold[node_id] << endl;
    cout << "Avg Access Count: " << avg_access_cnt << endl;
    cout << "Standard DEV: " << standard_dev << endl;
    cout << "epoch counter size: " << per_epoch_page_access_counter[node_id].size() << endl;
    cout << "Hot Page Cache size: " << hot_page_cache[node_id].size() << endl;
    // cin.get();
    // clearing the counters after initialisation
    hot_page_cache[node_id].clear();
    per_epoch_page_access_counter[node_id].clear();
    global_benefit_counter[node_id] = 0;
    setting_params[node_id] = 0;
    pages_migrated_in_feedback_cycle[node_id].clear();
    migration_counter[node_id] = 0;
}

/////////////////

// get victim page list

int pgd_ptr = 0;
int pud_ptr = 0;
int pmd_ptr = 0;
int pte_ptr = 0;

// return a victim page paddr to be added in victim list (if available)
int64_t traverse_page_table(pgd &_pgd, int node_id)
{
    pud *_pud;
    pmd *_pmd;
    pte *_pte;
    page *_page;

    for (int i = 0; i < 512; i++)
    {
        _pud = _pgd.access_in_pgd(pgd_ptr);

        if (_pud == nullptr)
            goto pud;

        for (int j = 0; j < 512; j++)
        {
            _pmd = _pud->access_in_pud(pud_ptr);

            if (_pmd == nullptr)
                goto pmd;

            for (int k = 0; k < 512; k++)
            {
                _pte = _pmd->access_in_pmd(pmd_ptr);

                if (_pte == nullptr)
                    goto pte;

                for (int l = 0; l < 512; l++)
                {
                    _page = _pte->access_in_pte(pte_ptr);

                    if (_page != nullptr)
                    {
                        long unsigned paddr = _page->get_page_physical_addr();
                        bool r_bit, t_bit, v_bit;

                        r_bit = _page->referenced_bit;
                        t_bit = _page->TLB_present_bit;
                        v_bit = _page->in_victim_list_bit;

                        // check if this page is local, only then we select it as the victim page
                        // using TLB aware clock replacement

                        if (L[node_id].is_local(paddr))
                        {
                            if (r_bit == 0 && t_bit == 0 && v_bit == 0)
                            {
                                // select this page as victim and its victim bit
                                _page->referenced_bit = 0;
                                _page->TLB_present_bit = 0;
                                _page->in_victim_list_bit = 1;

                                return paddr;
                            }
                            else if (r_bit == 0 && t_bit == 0 && v_bit == 1)
                            {
                                // already present in victim, skip
                            }
                            else if (r_bit == 0 && t_bit == 1 && v_bit == 0)
                            {
                                // unreachable state
                                // r_bit cnnot be zero if t_bit is 1
                            }
                            else if (r_bit == 0 && t_bit == 1 && v_bit == 1)
                            {
                                // unreachable state
                                // r_bit cnnot be zero if t_bit is 1, v_bit becomes insignificant
                            }
                            else if (r_bit == 1 && t_bit == 0 && v_bit == 0)
                            {
                                // give second chance to this page before selecting it as victim
                                _page->referenced_bit = 0;
                            }
                            else if (r_bit == 1 && t_bit == 0 && v_bit == 1)
                            {
                                // unreachable state
                                // already in victim list, r_bit cannot be 1
                            }
                            else if (r_bit == 1 && t_bit == 1 && v_bit == 0)
                            {
                                // present in TLB, skip this page
                                // and don't yet implement second-chance logic
                                _page->TLB_present_bit = 0;
                            }
                            else if (r_bit == 1 && t_bit == 1 && v_bit == 1)
                            {
                                // unreachable state
                                // cannot be in victim list, if it is refereenced and in TLB
                            }
                        }
                    }
                    pte_ptr = (pte_ptr + 1) % 512;
                }
            pte:
                pmd_ptr = (pmd_ptr + 1) % 512;
            }
        pmd:
            pud_ptr = (pud_ptr + 1) % 512;
        }
    pud:
        pgd_ptr = (pgd_ptr + 1) % 512;
    }
    return -1;
}
bool present_in_victim_list(int node_id, uint64_t paddr)
{
    for (int i = 0; i < max_victim_pages; i++)
    {
        if (victim_pages[node_id][i] == paddr)
        {
            return true;
        }
    }
    return false;
}
//
int64_t get_victim_page(int node_id)
{
    int64_t paddr;
    paddr = L[node_id].find_unallocated_page();
    bool is_victim = present_in_victim_list(node_id,paddr);
    while( paddr!=-1 && is_victim){
        //if(common_clock>30900000)cout<<"get_vctim"<<endl;
        paddr = L[node_id].find_unallocated_page();
        is_victim = present_in_victim_list(node_id,paddr);
    }
    if(paddr!=-1){
        return paddr;
    }
    paddr = traverse_page_table(_pgd[node_id], node_id);

    while (paddr == -1)
    {
        //if(common_clock>30900000) cout<<"get_vctim2"<<endl;
        paddr = traverse_page_table(_pgd[node_id], node_id);
    }
    return paddr;
}



bool update_page_bits(pgd &_pgd, uint64_t vaddr, uint64_t paaddr, int node_id, bool is_victim)
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
        cout << "error in update_victim_tlb_bits, pud not found";
        return false;
    }
    else if (_pud != nullptr)
    {
        _pmd = _pud->access_in_pud(pud_vaddr);
        if (_pmd == nullptr)
        {
            cout << "error in update_victim_tlb_bits, pmd not found";
            return false;
        }
        else if (_pmd != nullptr)
        {
            _pte = _pmd->access_in_pmd(pmd_vaddr);
            if (_pte == nullptr)
            {
                cout << "error in update_victim_tlb_bits, pte not found";
                return false;
            }
            else if (_pte != nullptr)
            {
                _page = _pte->access_in_pte(pte_vaddr);
                if (_page == nullptr)
                {
                    cout << "error in update_victim_tlb_bits, page not found";
                    return false;
                }
                else if (_page != nullptr)
                {

                    if (is_victim)
                    {
                        _page->TLB_present_bit = 0;
                    }
                    else
                    {
                        _page->referenced_bit = 1;
                        _page->TLB_present_bit = 1;
                        _page->in_victim_list_bit = 0;
                        uint64_t paddr = _page->get_page_physical_addr();
                        if (present_in_victim_list(node_id, paddr))
                        {
                            // find a new victim and swap it out with the current one
                            for (int i = 0; i < max_victim_pages; i++)
                            {

                                if (victim_pages[node_id][i] == paddr)
                                {
                                    // update the victim_page _list
                                    // cout<<"i: "<<i<<" "<<victim_pages[node_id][i];
                                    // cin.get();
                                    uint64_t new_victim_paddr = get_victim_page(node_id);
                                    victim_pages[node_id][i] = new_victim_paddr;
                                }
                            }
                        }
                    }

                    return true;
                }
            }
        }
    }
}

void fill_local_pages_to_migrate(int node_id)
{
    for (auto it = victim_pages[node_id].begin(); it != victim_pages[node_id].end(); it++)
    {
        int64_t vaddr = get_virtual_address((*it));
        if (vaddr == -1)
        {
            local_pages_to_migrate[node_id].push_back((*it));
        }
        victim_pages[node_id].erase(it);
        if (it == victim_pages[node_id].end())
        {
            break;
        }
        if (local_pages_to_migrate[node_id].size() == num_pages_to_migrate[node_id])
        {
            break;
        }
    }
    while (local_pages_to_migrate[node_id].size() < num_pages_to_migrate[node_id] && victim_pages[node_id].size())
    {
        //if(common_clock>30900000) cout<<"filllocpages"<<endl;
        for (auto it = victim_pages[node_id].begin(); it != victim_pages[node_id].end(); it++)
        {
            int64_t vaddr = get_virtual_address((*it));
            if (vaddr != -1)
            {
                local_pages_to_migrate[node_id].push_back((*it));
            }
            victim_pages[node_id].erase(it);
            if (it == victim_pages[node_id].end())
            {
                break;
            }
            if (local_pages_to_migrate[node_id].size() == num_pages_to_migrate[node_id])
            {
                break;
            }
        }
    }
}

void fill_remote_pages_to_migrate(int node_id)
{
    for (auto it : hot_page_cache[node_id])
    {
        remote_pages_to_migrate[node_id].push_back(it.paddr);
    }
    hot_page_cache[node_id].clear();
}
// for filling victim list after a page-migration completes
void refill_victim_list(int node_id)
{
    while (victim_pages[node_id].size() < max_victim_pages)
    {
        //if(common_clock>30900000)cout<<"refil"<<endl;
        victim_pages[node_id].push_back(get_victim_page(node_id));
    }
}

// perform the current page migration and stall the CPU
void migrate(int node_id, uint64_t remote_paddr, uint64_t local_paddr)
{
    // for each migration we need to change two mappings
    int64_t remote_vaddr = get_virtual_address(remote_paddr);
    int64_t local_vaddr = get_virtual_address(local_paddr);
bool pg_tb_update=false;
//update page-table

uint64_t ap;
    if (local_vaddr != -1)
    {
        pg_tb_update = update_page_table(_pgd[node_id],remote_paddr, local_vaddr);
        page_table_walk(_pgd[node_id], (local_vaddr<<12), ap);
    }

    pg_tb_update = update_page_table(_pgd[node_id],local_paddr, remote_vaddr);
    L[node_id].update_page_allocation_status(local_paddr);
    page_table_walk(_pgd[node_id], (remote_vaddr<<12), ap);
    update_page_bits(_pgd[node_id], (remote_vaddr<<12) , local_paddr, node_id, 0);
    
    // updating the mapping in reverse mapp function provider
    if (local_vaddr != -1)
        add_reverse_map(remote_paddr, local_vaddr);

    add_reverse_map(local_paddr, remote_vaddr);


    // If per process Page Migration is required then update the procid accordingly
    int procid = 1;
    // invalidating the TLB
    for (int i = 0; i < core_count; i++)
    {
        dtlbs[node_id * core_count + i].InvalidateTLB(remote_vaddr, procid);
        dtlbs[node_id * core_count + i].InvalidateTLB(local_vaddr, procid);
        // Invalidating all the cache entries
        dl1s[node_id * core_count + i].InvalidateCACHE(remote_paddr, procid);
        l2s[node_id * core_count + i].InvalidateCACHE(remote_paddr, procid);
        dl1s[node_id * core_count + i].InvalidateCACHE(local_paddr, procid);
        l2s[node_id * core_count + i].InvalidateCACHE(local_paddr, procid);
    }
    ul3[node_id].InvalidateCACHE(local_paddr, procid);
    ul3[node_id].InvalidateCACHE(remote_paddr, procid);
}

void migrate_pages(int node_id, std::ofstream &page_migration)
{
    for (int i = 0; i < num_pages_to_migrate[node_id]; i++)
    {
        migrate(node_id, remote_pages_to_migrate[node_id][i], local_pages_to_migrate[node_id][i]);
    }
    page_migration.open(file2[node_id],ios::app);
    page_migration << "-------Migration " << migration_counter[node_id] << " completed-----------" << endl;
    page_migration << "Pages Migrated: " << num_pages_to_migrate[node_id] << endl;
    page_migration << "Pages Migration Threshold: " << page_migration_threshold[node_id] << endl;
    page_migration.close();
    cout << "-------Migration " << migration_counter[node_id] << " completed-----------" << endl;
    cout << "Pages Migrated: " << num_pages_to_migrate[node_id] << endl;
    cout << "Pages Migration Threshold: " << page_migration_threshold[node_id] << endl;
    refill_victim_list(node_id);
}

void stall_processor(int node_id)
{
    if (common_clock == background_pg_swap_clock[node_id])
    {
        migrate_stall[node_id] = 1;
    }
}

bool ok_to_go;
// resume processor after the migration.
void resume_processor(int node_id, std::ofstream &page_migration)
{
    if (migrate_stall[node_id] == 1)
    {
        ok_to_go = 1;
        for (int i = 0; i < core_count; i++)
        {
            for(int j=0;j<il1_miss_buffer[node_id][i].size();j++)
            {
                if(il1_miss_buffer[node_id][i][j].complete_cycle<1)
                {
                    ok_to_go=0;
                }
            }
            if (dl1_miss_buffer[node_id][i].size() != 0)
                ok_to_go = 0;
            // if (il1_wait_buffer[node_id][i].size() != 0)
            //     ok_to_go = 0;
        }
        if (ok_to_go && common_clock > resume_stall_clock[node_id])
        {
            // from now + 4000 cycles make it 0
            fill_local_pages_to_migrate(node_id);
            fill_remote_pages_to_migrate(node_id);
            if (migration_counter[node_id] % 3 == 0)
            {
                pages_migrated_in_feedback_cycle[node_id].clear();
                pages_migrated_in_feedback_cycle[node_id] = local_pages_to_migrate[node_id];

               
            }
            else
            {
                for (auto it : local_pages_to_migrate[node_id])
                {
                    update_migrate_list_of_feedback_cycle(node_id, it);
                }
                
            }
            migrate_pages(node_id, page_migration);
            if (migration_counter[node_id] % 3 == 2)
            {
                // count benefit and update params
                int score = count_benefit(node_id, page_migration);
                int page_to_incr = ceil(((double)num_pages_to_migrate[node_id]) * (double)score / 100.0);
                num_pages_to_migrate[node_id] += page_to_incr;
                if (num_pages_to_migrate[node_id] < 5)
                {
                    // reinitialisation
                    hot_page_cache[node_id].clear();
                    per_epoch_page_access_counter[node_id].clear();
                    num_pages_to_migrate[node_id] = 0;
                    setUpPhase_cycle[node_id] = common_clock;
                    setting_params[node_id] = 1;
                }
                if (num_pages_to_migrate[node_id] > 500)
                    num_pages_to_migrate[node_id] = 500;
            }
            local_pages_to_migrate[node_id].clear();
            remote_pages_to_migrate[node_id].clear();
            resume_stall_clock[node_id] = common_clock + TLB_shootdown_cycles;
            last_migration_cycle[node_id] = resume_stall_clock[node_id];
        }
    }
}

bool time_out(int node_id)
{
    if (common_clock == (last_migration_cycle[node_id] + (1 * initiliastion_epoch_length) + setUpPhase_cycle[node_id]) && !setting_params[node_id])
    {
        // reinitialise
        return true;
    }
    return false;
}
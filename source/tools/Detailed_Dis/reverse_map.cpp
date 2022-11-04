//#define max_pages 100

unordered_map<int64_t, int64_t> reverse_map;

// add or update reverse_map entry for a pair paddr,vaddr
void add_reverse_map(uint64_t paddr, uint64_t vaddr)
{
    reverse_map[paddr] = vaddr;
}

int64_t get_virtual_address(uint64_t paddr)
{
    if (reverse_map.find(paddr) == reverse_map.end())
    {
        return -1;
    }
    return reverse_map[paddr];
}

bool update_page_table(pgd &_pgd, uint64_t paddr, uint64_t vaddr)
{
    pud *_pud;
    pmd *_pmd;
    pte *_pte;
    page *_page;

    unsigned long pgd_vaddr = 0L, pud_vaddr = 0L, pmd_vaddr = 0L, pte_vaddr = 0L, page_offset_addr = 0L;
    vaddr = vaddr <<12;
    split_vaddr(pgd_vaddr, pud_vaddr, pmd_vaddr, pte_vaddr, page_offset_addr, vaddr);

    _pud = _pgd.access_in_pgd(pgd_vaddr);
    if (_pud == nullptr)
    {
        cout<<"\nerror pud";
        return false;
    }
    else if (_pud != nullptr)
    {
        _pmd = _pud->access_in_pud(pud_vaddr);
        if (_pmd == nullptr)
        {
            cout<<"\nerror pmd";
            return false;
        }
        else if (_pmd != nullptr)
        {
            _pte = _pmd->access_in_pmd(pmd_vaddr);
            if (_pte == nullptr)
            {
                cout<<"\nerror pte";
                return false;
            }
            else if (_pte != nullptr)
            {
                _page = _pte->access_in_pte(pte_vaddr);
                if (_page == nullptr)
                {
                    cout<<"\nerror page";
                    return false;
                }
                else if (_page != nullptr)
                {
                    vaddr=vaddr>>12;
                    _page->set_page_map(pte_vaddr,paddr);
                    //long int paddr = _page->get_page_base_addr(pte_vaddr);
                    // paddr=paddr<<12;
                    // paddr=paddr + page_offset_addr;
                    return true;
                }
            }
        }
    }
}
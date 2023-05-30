#include "PageTable.h"


int PageTable::isPagePresentInMainMemory(int page_index){
    return page_table[page_index].getPresent();
}

PageTable::PageTable(int virtual_frames, int physical_frames){
    page_table = (PageTableEntry *)malloc(virtual_frames * sizeof(PageTableEntry));
    this->virtual_frames = virtual_frames;
    this->physical_frames = physical_frames;
    for(int i = 0; i < virtual_frames; i++){
        page_table[i].modify(0,0,0,i,0,i);
    }
}

PageTable::PageTable(){}

PageTableEntry PageTable::get(int page_index){
    return page_table[page_index];
}

PageTableEntry PageTable::setUsingPTE(PageTableEntry pte){
    for(int i = 0; i < virtual_frames; i++){
        if(page_table[i].getSpecialPageIndex() == pte.getSpecialPageIndex()){
            page_table[i] = pte;
        }
    }
}

void PageTable::set(int page_index, PageTableEntry page_table_entry){
    page_table[page_index] = page_table_entry;
}

/* This method will create page table (initialize first), then fill it with Page Table Entries. Each entry will have a page frame number
that is calculated by considering the limits of physical memory (Number of physical frames).
 */
void PageTable::createPageTable(int virtual_frames,int physical_frames){
    page_table = (PageTableEntry *)malloc(virtual_frames * sizeof(PageTableEntry));
    this->virtual_frames = virtual_frames;
    this->physical_frames = physical_frames;
    for(int i = 0; i < virtual_frames; i++){
        page_table[i].modify(0,0,0,i,0,i);
    }
}

PageTableEntry * PageTable::getAllPresentPages(){
    PageTableEntry * pte = (PageTableEntry*)malloc(this->physical_frames * sizeof(PageTableEntry));
    int count = 0;
    for(int i = 0; i < this->virtual_frames; i++){
        if(this->page_table[i].getPresent() == 1 && count < this->physical_frames){
            pte[count] = this->page_table[i];
            count++;
        }
    }

    return pte;
}


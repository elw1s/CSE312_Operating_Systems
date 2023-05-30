#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <random>

// 5 x 4 bytes = 20 bytes for each Page Table Entry. It might be lower if 'int' values is changed to a single bit.
class PageTableEntry{
private:
    int referenced;
    int modified;
    int present;
    int page_frame_number;
    int time_of_the_last_use; //Required to implement WSClock
    int specialPageIndex; //Special index for each Page
public:
    PageTableEntry(){
        this->referenced = 0;
        this->modified = 0;
        this->present = 0;
        this->page_frame_number = 0;
        this->time_of_the_last_use = 0;
    }
    PageTableEntry(int referenced, int modified, int present, int page_frame_number, int time_of_the_last_use, int specialPageIndex){
        this->referenced = referenced;
        this->modified = modified;
        this->present = present;
        this->page_frame_number = page_frame_number;
        this->time_of_the_last_use = time_of_the_last_use;
        this->specialPageIndex = specialPageIndex;
    }
    // Getter methods
    int getReferenced() {
        return referenced;
    }

    int getSpecialPageIndex() {
        return specialPageIndex;
    }

    int getModified() {
        return modified;
    }

    int getPresent() {
        return present;
    }

    int getPageFrameNumber() {
        return page_frame_number;
    }

    int getTimeOfTheLastUse() {
        return time_of_the_last_use;
    }

    // Setter methods
    void setReferenced(int referenced) {
        this->referenced = referenced;
    }

    void setSpecialPageIndex(int specialPageIndex) {
        this->specialPageIndex = specialPageIndex;
    }

    void setModified(int modified) {
        this->modified = modified;
    }

    void setPresent(int present) {
        this->present = present;
    }

    void setPageFrameNumber(int page_frame_number) {
        this->page_frame_number = page_frame_number;
    }

    void setTimeOfTheLastUse(int time_of_the_last_use) {
        this->time_of_the_last_use = time_of_the_last_use;
    }
    //Modifies all the values of Page Table Entry
    void modify(int referenced, int modified, int present, int page_frame_number, int time_of_the_last_use, int specialPageIndex){
        this->referenced = referenced;
        this->modified = modified;
        this->present = present;
        this->page_frame_number = page_frame_number;
        this->time_of_the_last_use = time_of_the_last_use;
        this->specialPageIndex = specialPageIndex;
    }
    //Creates a string of the all values
    char * toString(){
        int bufferSize = snprintf(nullptr, 0, "%d-%d-%d-%d-%d-%d", referenced, modified, present, time_of_the_last_use, page_frame_number,specialPageIndex);
        char* result = new char[bufferSize + 1];
        snprintf(result, bufferSize + 1, "%d-%d-%d-%d-%d-%d", referenced, modified, present, time_of_the_last_use, page_frame_number,specialPageIndex);
        return result;
    }
};

class PageTable{
    private:
        PageTableEntry *page_table;
        int virtual_frames;
        int physical_frames;
    public:
        PageTable();
        PageTable(int virtual_frames, int physical_frames);
        PageTableEntry get(int page_index);
        void set(int page_index, PageTableEntry page_table_entry);
        int isPagePresentInMainMemory(int page_index);
        void createPageTable(int virtual_frames,int physical_frames);
        PageTableEntry *getAllPresentPages();
        PageTableEntry setUsingPTE(PageTableEntry pte);
};  

#endif
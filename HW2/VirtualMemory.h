#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include <iostream>
#include <string.h>
#include <random>
#include <fstream>
#include "PageTable.h"

using namespace std;

class VirtualMemory{

private:
    int frame_size;
    int virtual_frames;
    char *disk_file_path;
    int *virtual_memory;

public:
    VirtualMemory();
    VirtualMemory(int frame_size, int virtual_frames, char * disk_file_path);
    ~VirtualMemory(); //Destructor
    void createVirtualMemory(int frame_size, int virtual_frames, char * disk_file_path);
    int get(int address);
    void set(int address, int value);
    int getIndexForPageTable(int address);
    void fillWithRandomValues();
    void writeToTheDisk(PageTableEntry pte, int page_index);

};


#endif
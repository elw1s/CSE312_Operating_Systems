#ifndef MAIN_MEMORY_H
#define MAIN_MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "PageTable.h"
#include "LinkedList.cpp"
#include <fstream>
#include <limits>

using namespace std;


class MainMemory{

private:
    int frame_size;
    int physical_frames;
    char *disk_file_path;
    int *physical_memory;
    char *algorithm;
    LinkedList list;
public:
    MainMemory();
    MainMemory(int frame_size, int physical_frames, char * disk_file_path, char *algorithm);
    ~MainMemory(); //Destructor
    void createMainMemory(int frame_size, int physical_frames, char * disk_file_path, char *algorithm);
    int get(int address);
    void set(int address, int value);
    PageTableEntry applyPageReplacementAlgorithm(int page_index);
    PageTableEntry applyLRU(int page_index);
    PageTableEntry applySC(int page_index);
    //PageTableEntry applyWSClock(int page_index);
    PageTableEntry addPageIntoMainMemory(PageTableEntry pte);
    int hasEmptySpace();
    int getFromDisk(PageTableEntry pte);
    void writeToTheDisk(PageTableEntry pte, int page_index);
    int getEmptySpaceStartingIndex();
    void removePageFramesFromMemory(PageTableEntry pte);
    PageTableEntry modifyPageInMainMemory(PageTableEntry pte);
    PageTableEntry createEmptySpace(int page_index);
};



#endif
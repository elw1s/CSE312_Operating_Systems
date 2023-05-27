#include "VirtualMemory.h"
#include "MainMemory.h"
#include "PageTable.h"


VirtualMemory vm;
PageTable pt; 
MainMemory mm;
int frame_size, physical_frames, virtual_frames, isInverted, printAt;
char *algorithm, *disk_file_path;

int convertVirtualMemoryIndexToPageTableIndex(int virtual_memory_index){
    return virtual_memory_index / frame_size;
}

void run_threads(){
    for(int index = 32 * frame_size - 1; index >= 0; index-=frame_size){
        if(pt.isPagePresentInMainMemory(convertVirtualMemoryIndexToPageTableIndex(index))){
            cout << index << " Page is in Main Memory!\n";
        }
        else{
            cout << index << " Page is not in Main Memory!\n";
            cout <<  index << " Old PFN: " << pt.get(convertVirtualMemoryIndexToPageTableIndex(index)).getPageFrameNumber() << endl;
            PageTableEntry newly_loaded_page = mm.addPageIntoMainMemory(pt.get(convertVirtualMemoryIndexToPageTableIndex(index)),convertVirtualMemoryIndexToPageTableIndex(index));
            pt.set(convertVirtualMemoryIndexToPageTableIndex(index),newly_loaded_page);
            cout << index << " New PFN: " << pt.get(convertVirtualMemoryIndexToPageTableIndex(index)).getPageFrameNumber() << endl;
        }
    }
    int index = 8562;
    if(pt.isPagePresentInMainMemory(convertVirtualMemoryIndexToPageTableIndex(index))){
            cout << index << " Page is in Main Memory!\n";
        }
        else{
            cout << index << " Page is not in Main Memory!\n";
            cout <<  index << " Old PFN: " << pt.get(convertVirtualMemoryIndexToPageTableIndex(index)).getPageFrameNumber() << endl;
            PageTableEntry newly_loaded_page = mm.addPageIntoMainMemory(pt.get(convertVirtualMemoryIndexToPageTableIndex(index)),convertVirtualMemoryIndexToPageTableIndex(index));
            pt.set(convertVirtualMemoryIndexToPageTableIndex(index),newly_loaded_page);
            cout << index << " New PFN: " << pt.get(convertVirtualMemoryIndexToPageTableIndex(index)).getPageFrameNumber() << endl;
        }
}

void init(){

    vm.createVirtualMemory(frame_size,virtual_frames,disk_file_path);
    pt.createPageTable(virtual_frames, physical_frames);
    mm.createMainMemory(frame_size,physical_frames,disk_file_path,algorithm); //isInverted could be added here?
    vm.fillWithRandomValues();
    for(int i = 0; i < virtual_frames; i++){
        PageTableEntry pte = pt.get(i); 
        vm.writeToTheDisk(pte, i);
        /* printf("%d.th page is written to the disk\n",i);
        printf("PTE:%s\n", pte.toString()); */
    }

    run_threads();

}


int main(int argc, char* argv[]){
    
    //Getv values from terminal

    frame_size = 4096;
    physical_frames = 32;
    virtual_frames = 1024;
    disk_file_path = (char *)malloc(sizeof("SC"));
    algorithm = (char *)malloc(sizeof("diskFileName.dat"));
    strcpy(algorithm, "SC");
    isInverted = 1;
    strcpy(disk_file_path, "diskFileName.dat");

    init();


    return 0;
}
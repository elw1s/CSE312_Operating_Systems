#include "VirtualMemory.h"
#include "MainMemory.h"
#include "PageTable.h"
#include <cmath>
#include <thread>

VirtualMemory vm;
PageTable pt; 
MainMemory mm;
int frame_size, physical_frames, virtual_frames, isInverted, printAt;
char *algorithm, *disk_file_path;

int number_of_reads = 0, number_of_writes = 0, number_of_page_misses = 0, number_of_page_replacements = 0, number_of_disk_page_writes = 0,
number_of_disk_page_reads = 0;

int convertVirtualMemoryIndexToPageTableIndex(int virtual_memory_index){
    return virtual_memory_index / frame_size;
}

/* index = Virtual Memory Index for an "int" */
int getValueFromVirtualMemory(int index){
    if(!pt.isPagePresentInMainMemory(convertVirtualMemoryIndexToPageTableIndex(index))){
        number_of_page_misses++;
        PageTableEntry removedPage;
        if(!mm.hasEmptySpace()){
            removedPage = mm.createEmptySpace(convertVirtualMemoryIndexToPageTableIndex(index), &number_of_disk_page_writes);
            pt.setUsingPTE(removedPage);
        }
        PageTableEntry newly_loaded_page = mm.addPageIntoMainMemory(pt.get(convertVirtualMemoryIndexToPageTableIndex(index)),&number_of_disk_page_reads);
        number_of_page_replacements++;
        pt.set(convertVirtualMemoryIndexToPageTableIndex(index),newly_loaded_page);
    }
    number_of_reads++;
    return vm.get(index);
}

void setValueInVirtualMemory(int index, int val){
    if(!pt.isPagePresentInMainMemory(convertVirtualMemoryIndexToPageTableIndex(index))){
        number_of_page_misses++;
        PageTableEntry removedPage;   
        if(!mm.hasEmptySpace()){
            removedPage = mm.createEmptySpace(convertVirtualMemoryIndexToPageTableIndex(index), &number_of_disk_page_writes);
            pt.setUsingPTE(removedPage);
        }
        PageTableEntry newly_loaded_page = mm.addPageIntoMainMemory(pt.get(convertVirtualMemoryIndexToPageTableIndex(index)),&number_of_disk_page_reads);
        number_of_page_replacements++;
        pt.set(convertVirtualMemoryIndexToPageTableIndex(index),newly_loaded_page);
    }
    vm.set(index,val);
    int main_memory_index = index - convertVirtualMemoryIndexToPageTableIndex(index) * frame_size + frame_size * pt.get(convertVirtualMemoryIndexToPageTableIndex(index)).getPageFrameNumber();
    mm.set(main_memory_index,val);
    PageTableEntry modifiedPTE = pt.get(convertVirtualMemoryIndexToPageTableIndex(index));
    modifiedPTE.setModified(1);
    modifiedPTE.setReferenced(1);
    modifiedPTE.setPresent(1);
    pt.setUsingPTE(modifiedPTE);
    mm.modifyPageInMainMemory(modifiedPTE);
    number_of_writes++;
}


/* From 0 to middle - 1 */
void MatrixVectorMultiplication(){
    int start = 0;
    int middle = (virtual_frames * frame_size) / 2;



}
/* From middle to end - 1 */
void VectorVectorMultiplication(){
    int middle = (virtual_frames * frame_size) / 2;
    int end = (virtual_frames * frame_size) - 1;


}

void Summation(){
    
}

void LinearSearch(){
    
}

void BinarySearch(){

}

void run_threads(){

/*     std::thread mvm(MatrixVectorMultiplication);
    std::thread vvm(VectorVectorMultiplication);
    std::thread s(Summation);
    std::thread ls(LinearSearch);
    std::thread bs(BinarySearch);

    // Wait for all threads to complete
    mvm.join();
    vvm.join();
    s.join();
    ls.join();
    bs.join(); */

    for(int index = 32 * frame_size - 1; index >= 0; index-=frame_size){
        getValueFromVirtualMemory(index);
        setValueInVirtualMemory(index, 1001);
    }    
    cout << "Gelen sayi = " << getValueFromVirtualMemory(131073) << endl;
    //cout << "Gelen sayi = " << getValueFromVirtualMemory(135170) << endl;

}

void printStatistics(){
    cout << "Number of reads: " << number_of_reads << endl;
    cout << "Number of writes: " << number_of_writes << endl;
    cout << "Number of page misses: " << number_of_page_misses << endl;
    cout << "Number of page replacements: " << number_of_page_replacements << endl;
    cout << "Number of disk page writes: " << number_of_disk_page_writes << endl;
    cout << "Number of disk page reads: " << number_of_disk_page_reads << endl;
    //cout << "Estimated working set function w for each thread as a table: " << DOLDUR  << ;
}


void init(){

    vm.createVirtualMemory(frame_size,virtual_frames,disk_file_path);
    pt.createPageTable(virtual_frames, physical_frames);
    mm.createMainMemory(frame_size,physical_frames,disk_file_path,algorithm); //isInverted could be added here?
    vm.fillWithRandomValues();
    cout << "All pages are being written to the disk..." << endl;
    remove("diskFileName.dat");
    for(int i = 0; i < virtual_frames; i++){
        PageTableEntry pte = pt.get(i); 
        vm.writeToTheDisk(pte, i);
    }
    cout << "Finished..." << endl;
    run_threads();

    printStatistics();
}

int main(int argc, char* argv[]){
    
    frame_size = std::pow(2,atoi(argv[1]));
    physical_frames = std::pow(2,atoi(argv[2]));
    virtual_frames = std::pow(2,atoi(argv[3]));
    algorithm = (char *)malloc(sizeof(argv[4]));
    strcpy(algorithm, argv[4]);

    //Non-inverted
    if(argc == 7){
        isInverted = 0;
        printAt = atoi(argv[5]);
        disk_file_path = (char *)malloc(sizeof(argv[6]));
        strcpy(disk_file_path, argv[6]);
    }
    //Inverted
    else if(argc == 8){
        if(strcmp("inverted", argv[5])==0){
            isInverted = 1;
        }
        printAt = atoi(argv[6]);
        disk_file_path = (char *)malloc(sizeof(argv[7]));
        strcpy(disk_file_path, argv[7]);
    }
    
    init();

    free(algorithm);
    free(disk_file_path);
    return 0;
}
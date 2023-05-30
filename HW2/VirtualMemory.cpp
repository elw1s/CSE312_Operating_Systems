#include "VirtualMemory.h"



VirtualMemory::VirtualMemory(){

}

VirtualMemory::VirtualMemory(int frame_size, int virtual_frames, char * disk_file_path){
    virtual_memory = (int *)malloc(virtual_frames * frame_size * sizeof(int));
    this->frame_size = frame_size;
    this->virtual_frames = virtual_frames;
    this->disk_file_path = (char *)malloc(sizeof(disk_file_path));
    strcpy(this->disk_file_path, disk_file_path);
}

//Destructor
VirtualMemory::~VirtualMemory(){

}

void VirtualMemory::createVirtualMemory(int frame_size, int virtual_frames, char * disk_file_path){
    virtual_memory = (int *)malloc(virtual_frames * frame_size * sizeof(int));
    this->frame_size = frame_size;
    this->virtual_frames = virtual_frames;
    this->disk_file_path = (char *)malloc(sizeof(disk_file_path));
    strcpy(this->disk_file_path, disk_file_path);
}

void VirtualMemory::fillWithRandomValues(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 100); // Set the range from 0 to 100

    for (int i = 0; i < virtual_frames * frame_size; i++) {
        this->virtual_memory[i] = dist(gen);
    }}
/* At first run, the disk will have the Virtual Memory values in it. Therefore, they are copied into it by considering the page table entry. */
void VirtualMemory::writeToTheDisk(PageTableEntry pte, int page_index){
    std::ofstream outputFile(this->disk_file_path,std::ios::app); 
    if (outputFile.is_open()) {
        outputFile << pte.toString() << endl;
        int firstIndexOfIntValueInPage = page_index * this->frame_size; // Size of int x page_index will give the first index of the "int" in the given page.
        for(int i = 0; i < this->frame_size; i++){
            outputFile << this->virtual_memory[firstIndexOfIntValueInPage + i] << endl;
        }
        outputFile.close(); 
    } else {
        cout << "Error at opening the disk file.\n";
    }
}


int VirtualMemory::getIndexForPageTable(int address){
    return (address - 1) / virtual_frames; //Finds the page index in page table of the given address which is in VM.
}

int VirtualMemory::get(int address){
    return virtual_memory[address];
}

void VirtualMemory::set(int address, int value){
    virtual_memory[address] = value;
}






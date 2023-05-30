#include "MainMemory.h"

MainMemory::MainMemory(){

}

MainMemory::MainMemory(int frame_size, int physical_frames, char * disk_file_path, char *algorithm){
    physical_memory = (int *)malloc(physical_frames * frame_size * sizeof(int));
    this->frame_size = frame_size;
    this->physical_frames = physical_frames;
    this->disk_file_path = (char *)malloc(sizeof(disk_file_path));
    this->algorithm = (char *)malloc(sizeof(algorithm));
    strcpy(this->disk_file_path, disk_file_path);
    strcpy(this->algorithm, algorithm);
    for(int i = 0; i < this->physical_frames * this->frame_size; i++){
        this->physical_memory[i] = std::numeric_limits<int>::max();
    }
}

//Destructor
MainMemory::~MainMemory(){

}

void MainMemory::set(int address, int value){
    this->physical_memory[address] = value;
}


void MainMemory::createMainMemory(int frame_size, int physical_frames, char * disk_file_path, char *algorithm){
    physical_memory = (int *)malloc(physical_frames * frame_size * sizeof(int));
    this->frame_size = frame_size;
    this->physical_frames = physical_frames;
    this->disk_file_path = (char *)malloc(sizeof(disk_file_path));
    this->algorithm = (char *)malloc(sizeof(algorithm));
    strcpy(this->disk_file_path, disk_file_path);
    strcpy(this->algorithm, algorithm);
    for(int i = 0; i < this->physical_frames * this->frame_size; i++){
        this->physical_memory[i] = std::numeric_limits<int>::max();
    }    
}

PageTableEntry MainMemory::applyPageReplacementAlgorithm(int page_index,int * number_of_disk_page_writes){
    if(strcmp(this->algorithm, "LRU") == 0){
        return applyLRU(page_index,number_of_disk_page_writes);
    }
    else if(strcmp(this->algorithm, "SC") == 0){
        return applySC(page_index,number_of_disk_page_writes);
    }
    else{
        //(strcmp(this->algorithm, "WSClock") == 0){
        //return applyWSClock(page_index);
    }

}

/* Remove the latest page in the linked list
   If a page table entry is modified, then call removeAndAddAgain()
 */
PageTableEntry MainMemory::applyLRU(int page_index,int * number_of_disk_page_writes){
    PageTableEntry removedPTE = list.removeRear();
    if(removedPTE.getModified() == 1){
        removedPTE.modify(0,0,0,removedPTE.getPageFrameNumber(),0,removedPTE.getSpecialPageIndex());
        writeToTheDisk(removedPTE, page_index,number_of_disk_page_writes);
    }
    removePageFramesFromMemory(removedPTE);
}

PageTableEntry MainMemory::applySC(int page_index,int * number_of_disk_page_writes){

    while(list.getFirstLoadedPage().getReferenced() == 1){
        PageTableEntry firstEntryInList = list.remove();
        firstEntryInList.setReferenced(0);
        list.add(firstEntryInList);
    }

    PageTableEntry removedPTE = list.remove();
    if(removedPTE.getModified() == 1){
        removedPTE.modify(0,0,0,removedPTE.getPageFrameNumber(),0,removedPTE.getSpecialPageIndex());
        writeToTheDisk(removedPTE, removedPTE.getSpecialPageIndex(),number_of_disk_page_writes);
    }
    removedPTE.modify(0,0,0,removedPTE.getPageFrameNumber(),0,removedPTE.getSpecialPageIndex());
    removePageFramesFromMemory(removedPTE);
    return removedPTE;
    
}

void MainMemory::removePageFramesFromMemory(PageTableEntry pte){
    for(int i = pte.getPageFrameNumber() * this->frame_size; i < pte.getPageFrameNumber() + this->frame_size; i++){
        this->physical_memory[i] = std::numeric_limits<int>::max();
    }
}

int MainMemory::getEmptySpaceStartingIndex(){
    if(hasEmptySpace()){
        for(int i=0; i < this->physical_frames; i++){
            if(this->physical_memory[i * this->frame_size] == std::numeric_limits<int>::max()){
                return i * this->frame_size;
            }
        }
    }
}
/* Buraya silmesi gereken page'i degil, onun yerine getireceği page'in indexini gönderiyor!!!! düzelt */
void MainMemory::writeToTheDisk(PageTableEntry pte, int page_index, int * number_of_disk_page_writes){
    std::ifstream inputFile(this->disk_file_path);
    std::ofstream outputFile("tmp.dat");  
    if (inputFile.is_open() && outputFile.is_open()){
        std::string line;
        int countLine = 0;
        int firstIndexOfIntValueInDisk = page_index * (this->frame_size + 1) + 1; // Size of int x page_index will give the first index of the "int" in the given page.
        int firstIndexOfPageTableEntryInDisk = page_index * (this->frame_size + 1); 
        while(std::getline(inputFile, line)){
            if(firstIndexOfIntValueInDisk + this->frame_size > countLine && countLine >= firstIndexOfIntValueInDisk){
                outputFile << this->physical_memory[countLine] << endl;
            }
            else if(firstIndexOfPageTableEntryInDisk == countLine){
                outputFile << pte.toString() << endl;
            }
            else{
                outputFile << line;
                if(line.back() != '\n') outputFile << endl;
            }
            countLine++;
        }
        (*number_of_disk_page_writes)++;
        inputFile.close();
        outputFile.close();
        remove(this->disk_file_path);
        rename("tmp.dat",this->disk_file_path); 
    } else {
        cout << "Error at opening the disk file.\n";
    }
}

/* Gets the given page table entry from disk and puts into the main memory. Returns the page frame number where the page is put. */
int MainMemory::getFromDisk(PageTableEntry pte, int * number_of_disk_page_reads){
    int index;
    std::ifstream inputFile(this->disk_file_path);
    if (inputFile.is_open()){
        std::string line;
        while (std::getline(inputFile, line)){
            if(line[line.size() - 1] == '\n'){
                line.erase(line.length() - 1);
            }
            if(line == pte.toString()){
                int count = 0;
                index = getEmptySpaceStartingIndex();
                while(std::getline(inputFile, line)){
                    if(count == this->frame_size){
                        inputFile.close();
                        return index / this->frame_size;
                    }
                    this->physical_memory[index + count] = std::stoi(line);
                    count++; 
                }
            }
        }
        inputFile.close();
    }
    (*number_of_disk_page_reads)++;
    return index / this->frame_size;
}

/* Removes a Page from Main Memory, returns the removed page table entry */
PageTableEntry MainMemory::createEmptySpace(int page_index,int * number_of_disk_page_writes){
    if(!hasEmptySpace()){
        return applyPageReplacementAlgorithm(page_index,number_of_disk_page_writes);
    }
}
/* Adds page into Main Memory, returns the new added Page Table Entry */
PageTableEntry MainMemory::addPageIntoMainMemory(PageTableEntry pte, int * number_of_disk_page_reads){
    PageTableEntry newly_added_page = pte;
    int page_frame_number = getFromDisk(pte,number_of_disk_page_reads);
    newly_added_page.modify(1,0,1,page_frame_number,0,newly_added_page.getSpecialPageIndex());
    if(hasEmptySpace()){
        if(strcmp(this->algorithm, "LRU") == 0){
            list.removeAndAddAgain(newly_added_page);
        }
        else{
            list.add(newly_added_page);
        }
    }
    return newly_added_page;
}

int MainMemory::hasEmptySpace(){
    return list.size() != this->physical_frames;
}

PageTableEntry MainMemory::modifyPageInMainMemory(PageTableEntry pte){
    if(strcmp(this->algorithm, "LRU") == 0){
        list.removeAndAddAgain(pte);
    }
    else{
        list.replaceWithNewPTE(pte);
    }
}


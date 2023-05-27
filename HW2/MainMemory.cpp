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

PageTableEntry MainMemory::applyPageReplacementAlgorithm(PageTableEntry pte,int page_index){
    if(strcmp(this->algorithm, "LRU") == 0){
        return applyLRU(page_index);
    }
    else if(strcmp(this->algorithm, "SC") == 0){
        return applySC(page_index);
    }
    else{
        //(strcmp(this->algorithm, "WSClock") == 0){
        //return applyWSClock(page_index);
    }

}

/* Remove the latest page in the linked list
   If a page table entry is modified, then call removeAndAddAgain()
 */
PageTableEntry MainMemory::applyLRU(int page_index){
    PageTableEntry removedPTE = list.removeRear();
    if(removedPTE.getModified() == 1){
        removedPTE.modify(0,0,0,-1,0);
        writeToTheDisk(removedPTE, page_index);
    }
    removePageFramesFromMemory(removedPTE);
}

PageTableEntry MainMemory::applySC(int page_index){

    /* If the first loaded page's R bit is 0 */
    if(list.getFirstLoadedPage().getReferenced() == 0){
        PageTableEntry removedPTE = list.remove();
        if(removedPTE.getModified() == 1){
            removedPTE.modify(0,0,0,-1,0);
            writeToTheDisk(removedPTE, page_index);
        }
        removePageFramesFromMemory(removedPTE);

    }

    /* If the first loaded page's R bit is 1, then firstly remove and add it again it. If every page's R bit is 1, then remove the first one. */
    PageTableEntry firstEntry = list.getFirstLoadedPage();

    if(list.getFirstLoadedPage().getReferenced() == 1){
        list.add(list.remove());
    }

    while(list.getFirstLoadedPage().getReferenced() == 1){
        if(firstEntry.getPageFrameNumber() == list.getFirstLoadedPage().getPageFrameNumber()){
            PageTableEntry removedPTE = list.remove();
            if(removedPTE.getModified() == 1){
                removedPTE.modify(0,0,0,-1,0);
                writeToTheDisk(removedPTE, page_index);
            }
            removePageFramesFromMemory(removedPTE);
            return removedPTE;
        }
        list.add(list.remove());
    }

    PageTableEntry removedPTE = list.remove();
    if(removedPTE.getModified() == 1){
        removedPTE.modify(0,0,0,-1,0);
        writeToTheDisk(removedPTE, page_index);
    }
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

void MainMemory::writeToTheDisk(PageTableEntry pte, int page_index){
    std::ifstream inputFile(this->disk_file_path);
    std::ofstream outputFile("tmp.dat");  
    if (inputFile.is_open() && outputFile.is_open()){
        std::string line;
        int countLine = 0;
        int firstIndexOfIntValueInDisk = page_index * this->frame_size + 1; // Size of int x page_index will give the first index of the "int" in the given page.
        int firstIndexOfPageTableEntryInDisk = page_index * this->frame_size + 1;
        while(std::getline(inputFile, line)){
            if(firstIndexOfIntValueInDisk + this->frame_size > countLine && countLine >= firstIndexOfIntValueInDisk){
                outputFile << this->physical_memory[countLine] << endl;
            }
            else if(firstIndexOfPageTableEntryInDisk == countLine){
                outputFile << pte.toString() << endl;
            }
            else{
                outputFile << line;
            }
            countLine++;
        }
        outputFile.close(); 
    } else {
        cout << "Error at opening the disk file.\n";
    }
}

/* Gets the given page table entry from disk and puts into the main memory. Returns the page frame number where the page is put. */
int MainMemory::getFromDisk(PageTableEntry pte){
    int index;
    std::ifstream inputFile(this->disk_file_path);
    if (inputFile.is_open()){
        std::string line;
        while (std::getline(inputFile, line)){
            line.erase(line.length() - 1);
            if(line == pte.toString()){
                cout << line << " = " << pte.toString() << endl;
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
    return index / this->frame_size;
}

PageTableEntry MainMemory::addPageIntoMainMemory(PageTableEntry pte, int page_index){
    PageTableEntry newly_added_page;
    int page_frame_number;
    if(hasEmptySpace()){
        cout << "There are Empty Space...\n";
        if(strcmp(this->algorithm, "LRU") == 0){
            list.removeAndAddAgain(pte);
        }
        else{
            list.add(pte);
        }
    }
    else{
        cout << "There are no empty space...\n";
        applyPageReplacementAlgorithm(pte, page_index);
    }
    newly_added_page = pte;
    page_frame_number = getFromDisk(pte);
    newly_added_page.modify(0,0,1,page_frame_number,0);
    return newly_added_page;
}

int MainMemory::hasEmptySpace(){
    return list.size() != this->physical_frames;
}


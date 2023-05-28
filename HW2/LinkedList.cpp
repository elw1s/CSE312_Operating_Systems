#include "PageTable.h"

class Node {
public:
    PageTableEntry data; // Data stored in the node
    Node* next;         // Pointer to the next node

    // Constructor
    Node(PageTableEntry entry) : data(entry), next(nullptr) {}
};

// Linked list class
class LinkedList {
private:
    Node* head; // Pointer to the head (front) of the linked list
    Node* tail; // Pointer to the tail (back) of the linked list
    int count;  // Number of elements in the linked list

public:
    // Constructor
    LinkedList() {
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

    // Destructor to free memory
    ~LinkedList() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Add an element to the back of the linked list (FIFO)
    void add(PageTableEntry entry) {
        Node* newNode = new Node(entry);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    // Remove and return the front element from the linked list (FIFO)
    PageTableEntry remove() {

        Node* temp = head;
        PageTableEntry frontEntry = head->data;
        head = head->next;

        if (head == nullptr) {
            tail = nullptr;
        }

        delete temp;
        count--;

        return frontEntry;
    }

    PageTableEntry removeRear() {

        if (head == tail) {
            // Only one node in the list
            PageTableEntry rearEntry = head->data;
            delete head;
            head = nullptr;
            tail = nullptr;
            count = 0;
            return rearEntry;
        }

        // Find the second-to-last node
        Node* current = head;
        while (current->next != tail) {
            current = current->next;
        }

        // Remove the last node and update tail
        PageTableEntry rearEntry = tail->data;
        delete tail;
        tail = current;
        tail->next = nullptr;
        count--;

        return rearEntry;
    }


    // Retrieve the number of elements in the linked list
    int size() {
        return count;
    }

    PageTableEntry getFirstLoadedPage(){
        return head->data;
    }

        // Remove the given PageTableEntry element and add it again to the linked list
    void removeAndAddAgain(PageTableEntry entry) {
        // Find the node containing the given entry
        Node* current = head;
        Node* previous = nullptr;
        while (current != nullptr && current->data.getPageFrameNumber() != entry.getPageFrameNumber()) {
            previous = current;
            current = current->next;
        }

        if (current == nullptr) {
            // Entry not found in the list
            Node* newNode = new Node(entry);
            newNode->next = head;
            head = newNode;
            if (tail == nullptr) {
                tail = newNode;
            }
            count++;
            return;
        }

        // Remove the node containing the entry from its current position
        if (previous != nullptr) {
            previous->next = current->next;
        } else {
            head = current->next;
        }

        // Add the node containing the entry at the front of the list
        current->data = entry;
        current->next = head;
        head = current;

        // Update the tail if the entry was at the rear of the list
        if (tail == current) {
            tail = previous;
        }
    }

    void replaceWithNewPTE(PageTableEntry entry) {
        Node* current = head;
        while (current != nullptr) {
            if (current->data.getSpecialPageIndex() == entry.getSpecialPageIndex()) {
                current->data = entry;
                return;
            }
            current = current->next;
        }
    }

};
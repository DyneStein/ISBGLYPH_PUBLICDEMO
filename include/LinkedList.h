#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "String.h"
#include <iostream>
using namespace std;

// node for linked list (stores strings only)
struct Node {
    char* data;
    Node* next;
    
    Node(const char* val) {
        int len = stringLength(val);
        data = new char[len + 1];
        stringCopy(data, val);
        next = nullptr;
    }
    
    ~Node() {
        delete[] data;
    }
};

// linked list for strings
class LinkedList {
private:
    Node* head;
    Node* tail;  // added for O(1) append as per spec
    int size;

public:
    LinkedList() {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }
    
    // LinkedList::append(value)
    // Time: O(1) - insert at head
    // Space: O(n) for string copy where n = strlen(value)
    // NOW CASE-INSENSITIVE: stores uppercase version
    void append(const char* value) {
        // Normalize to uppercase for case-insensitive storage
        char upperValue[200];
        stringCopy(upperValue, value);
        toUpperCase(upperValue);
        
        Node* newNode = new Node(upperValue);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        size++; // Changed from count++ to size++ to match existing member variable
    }
    
    // LinkedList::prepend(value)
    // Time: O(1) - insert at head is constant time
    // Space: O(n) where n = value length for string storage
    void prepend(const char* value) {
        Node* newNode = new Node(value);
        newNode->next = head;
        head = newNode;
        if (tail == nullptr) {
            tail = newNode;
        }
        size++;
    }
    
    // LinkedList::get(index)
    // Time: O(n) - must traverse list to index position
    // Space: O(1) - no extra space allocated
    const char* get(int index) {
        if (index < 0 || index >= size) {
            return nullptr;
        }
        Node* temp = head;
        for (int i = 0; i < index; i++) {
            temp = temp->next;
        }
        return temp->data;
    }
    
    // LinkedList::contains(value)
    // Time: O(n) - must potentially scan entire list
    // Space: O(1) - no heap allocation
    // NOW CASE-INSENSITIVE
    bool contains(const char* value) {
        char upperValue[200];
        stringCopy(upperValue, value);
        toUpperCase(upperValue);
        
        Node* temp = head;
        while (temp != nullptr) {
            if (stringCompare(temp->data, upperValue) == 0) {
                return true;
            }
            temp = temp->next;
        }
        return false;
    }
    
    // get size
    int getSize() {
        return size;
    }
    
    // check if empty
    bool isEmpty() {
        return size == 0;
    }
    
    // display list
    void display() {
        Node* temp = head;
        while (temp != nullptr) {
            cout << temp->data;
            if (temp->next != nullptr) cout << " -> ";
            temp = temp->next;
        }
        cout << endl;
    }
    
    // get head for iteration
    Node* getHead() {
        return head;
    }
    
    // LinkedList::insertAfter(stopID, newStopID)
    // Time: O(n) - must search for stopID in list
    // Space: O(k) where k = newStopID string length
    bool insertAfter(const char* stopID, const char* newStopID) {
        Node* temp = head;
        while (temp != nullptr) {
            if (stringCompare(temp->data, stopID) == 0) {
                Node* newNode = new Node(newStopID);
                newNode->next = temp->next;
                temp->next = newNode;
                if (temp == tail) {
                    tail = newNode;
                }
                size++;
                return true;
            }
            temp = temp->next;
        }
        return false; // stopID not found
    }
    
    // LinkedList::removeStop(stopID)
    // Time: O(n) - must search for stopID to remove
    // Space:    
    // Remove node by value
    // NOW CASE-INSENSITIVE
    bool remove(const char* stopID) {
        char upperStopID[200];
        stringCopy(upperStopID, stopID);
        toUpperCase(upperStopID);
        
        if (head == nullptr) return false;
        
        // if head is the one to remove
        if (stringCompare(head->data, upperStopID) == 0) {
            Node* temp = head;
            head = head->next;
            if (head == nullptr) tail = nullptr;
            delete temp;
            size--; // Changed from count-- to size-- to match existing member variable
            return true;
        }
        
        // search for the node
        Node* current = head;
        while (current->next != nullptr) {
            if (stringCompare(current->next->data, upperStopID) == 0) {
                Node* toDelete = current->next;
                current->next = current->next->next;
                if (toDelete == tail) tail = current;
                delete toDelete;
                size--; // Changed from count-- to size-- to match existing member variable
                return true;
            }
            current = current->next;
        }
        return false;
    }
    
    ~LinkedList() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

#endif

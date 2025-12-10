#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "String.h"
#include <iostream>
using namespace std;

// hash node (stores void pointer to allow any data type)
struct HashNode {
    char* key;
    void* value;
    HashNode* next;
    
    HashNode(const char* k, void* val) {
        int len = stringLength(k);
        key = new char[len + 1];
        stringCopy(key, k);
        value = val;
        next = nullptr;
    }
    
    ~HashNode() {
        delete[] key;
        // note: we don't delete value here, ownership belongs to caller
    }
};

// hash table class (no templates)
class HashTable {
private:
    HashNode** table;
    int capacity;
    int count;
    
    // HashTable::hashFunction(key)
    // Time: O(k) where k = key length, iterates through each character
    // Space: O(1) - only stores hash accumulator
    int hashFunction(const char* key) {
        int hash = 0;
        int len = stringLength(key);
        for (int i = 0; i < len; i++) {
            hash = (hash * 31 + key[i]) % capacity;
        }
        return hash;
    }

public:
    HashTable(int size = 100) {
        capacity = size;
        count = 0;
        table = new HashNode*[capacity];
        for (int i = 0; i < capacity; i++) {
            table[i] = nullptr;
        }
    }
    
    // HashTable::insert(key, value)
    // Time: Average O(1 + α) where α = load factor; Worst O(n) on collision chain
    // Space: O(k) for key storage where k = key length
    // NOW CASE-INSENSITIVE: converts key to uppercase before storing
    void insert(const char* key, void* value) {
        // Normalize the key to uppercase for case-insensitive storage
        char upperKey[200];
        stringCopy(upperKey, key);
        toUpperCase(upperKey);
        
        int index = hashFunction(upperKey);
        
        // check if key already exists
        HashNode* current = table[index];
        while (current != nullptr) {
            if (stringCompare(current->key, upperKey) == 0) {
                current->value = value; // update existing
                return;
            }
            current = current->next;
        }
        
        // insert new node at beginning of chain
        HashNode* newNode = new HashNode(upperKey, value);
        newNode->next = table[index];
        table[index] = newNode;
        count++;
    }
    
    // HashTable::search(key)
    // Time: Average O(1 + α) where α = load factor; Worst O(n) traversing chain
    // Space: O(1) - no extra space allocated
    // NOW CASE-INSENSITIVE: converts search key to uppercase
    void* search(const char* key) {
        // Normalize the search key to uppercase for case-insensitive search
        char upperKey[200];
        stringCopy(upperKey, key);
        toUpperCase(upperKey);
        
        int index = hashFunction(upperKey);
        HashNode* current = table[index];
        
        while (current != nullptr) {
            if (stringCompare(current->key, upperKey) == 0) {
                return current->value;
            }
            current = current->next;
        }
        return nullptr; // not found
    }
    
    // check if key exists (case-insensitive)
    bool contains(const char* key) {
        return search(key) != nullptr;  // search is now case-insensitive
    }
    
    // get current count
    int getCount() { return count; }
    
    // HashTable::getAllKeys(keyArray, count)
    // Time: O(n + buckets) - must traverse all chains in all buckets
    // Space: O(1) - no extra allocation, uses provided array
    void getAllKeys(char** keyArray, int* keyCount) {
        *keyCount = 0;
        for (int i = 0; i < capacity; i++) {
            HashNode* node = table[i];
            while (node != nullptr) {
                keyArray[*keyCount] = node->key;
                (*keyCount)++;
                node = node->next;
            }
        }
    }
    
    // check if empty
    bool isEmpty() {
        return count == 0;
    }
    
    // get current size
    int getSize() {
        return count;
    }
    
    ~HashTable() {
        for (int i = 0; i < capacity; i++) {
            HashNode* current = table[i];
            while (current != nullptr) {
                HashNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] table;
    }
};

#endif

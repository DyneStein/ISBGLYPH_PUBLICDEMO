#ifndef MINHEAP_H
#define MINHEAP_H

#include <iostream>
using namespace std;

// heap node structure (stores void pointer and priority)
struct HeapNode {
    void* data;
    double priority;
    
    HeapNode() {
        data = nullptr;
        priority = 0.0;
    }
    
    HeapNode(void* d, double p) {
        data = d;
        priority = p;
    }
};

// binary min-heap
class MinHeap {
private:
    HeapNode* arr;
    int capacity;
    int heapSize;
    
    // MinHeap::parent(i)
    // Time: O(1) - simple arithmetic operation
    // Space: O(1) - no extra space needed
    int parent(int i) {
        return (i - 1) / 2;
    }
    
    // MinHeap::left(i)
    // Time: O(1) - simple arithmetic operation
    // Space: O(1) - no extra space needed
    int left(int i) {
        return 2 * i + 1;
    }
    
    // MinHeap::right(i)
    // Time: O(1) - simple arithmetic operation
    // Space: O(1) - no extra space needed
    int right(int i) {
        return 2 * i + 2;
    }
    
    // MinHeap::swap(i, j)
    // Time: O(1) - constant time swap
    // Space: O(1) - single temp variable
    void swap(int i, int j) {
        HeapNode temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
    
    // MinHeap::heapifyUp(index)
    // Time: O(log n) - worst case traverse height of tree
    // Space: O(1) - iterative implementation, no recursion stack
    void heapifyUp(int index) {
        while (index > 0 && arr[parent(index)].priority > arr[index].priority) {
            swap(index, parent(index));
            index = parent(index);
        }
    }
    
    // MinHeap::heapifyDown(index)
    // Time: O(log n) - worst case traverse height of tree
    // Space: O(log n) - recursive call stack depth
    void heapifyDown(int index) {
        int smallest = index;
        int l = left(index);
        int r = right(index);
        
        if (l < heapSize && arr[l].priority < arr[smallest].priority) {
            smallest = l;
        }
        if (r < heapSize && arr[r].priority < arr[smallest].priority) {
            smallest = r;
        }
        
        if (smallest != index) {
            swap(index, smallest);
            heapifyDown(smallest);
        }
    }

public:
    MinHeap(int size = 100) {
        capacity = size;
        heapSize = 0;
        arr = new HeapNode[capacity];
    }
    
    // MinHeap::insert(data, priority)
    // Time: O(log n) - insert at end then heapifyUp takes log n
    // Space: O(1) - only stores one new element
    void insert(void* data, double priority) {
        if (heapSize == capacity) {
            cout << "Heap is full!" << endl;
            return;
        }
        arr[heapSize] = HeapNode(data, priority);
        heapifyUp(heapSize);
        heapSize++;
    }
    
    // MinHeap::getMin()
    // Time: O(1) - root element always at index 0
    // Space: O(1) - no extra space needed
    void* getMin() {
        if (heapSize == 0) {
            return nullptr;
        }
        return arr[0].data;
    }
    
    // MinHeap::extractMin()
    // Time: O(log n) - remove root, replace with last, heapifyDown takes log n
    // Space: O(log n) - recursive heapifyDown call stack
    void* extractMin() {
        if (heapSize == 0) {
            return nullptr;
        }
        void* minData = arr[0].data;
        arr[0] = arr[heapSize - 1];
        heapSize--;
        heapifyDown(0);
        return minData;
    }
    
    // MinHeap::isEmpty()
    // Time: O(1) - simple comparison
    // Space: O(1) - no extra space
    bool isEmpty() {
        return heapSize == 0;
    }
    
    // MinHeap::size()
    // Time: O(1) - return stored value
    // Space: O(1) - no extra space
    int size() {
        return heapSize;
    }
    
    ~MinHeap() {
        delete[] arr;
    }
};

#endif

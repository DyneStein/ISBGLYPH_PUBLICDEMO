#ifndef MAXHEAP_H
#define MAXHEAP_H

#include <iostream>
using namespace std;

// heap node structure (stores void pointer and priority)
struct MaxHeapNode {
    void* data;
    double priority;
    
    MaxHeapNode() {
        data = nullptr;
        priority = 0.0;
    }
    
    MaxHeapNode(void* d, double p) {
        data = d;
        priority = p;
    }
};

// binary max-heap
class MaxHeap {
private:
    MaxHeapNode* arr;
    int capacity;
    int heapSize;
    
    // MaxHeap::parent(i)
    // Time: O(1) - simple arithmetic operation
    // Space: O(1) - no extra space needed
    int parent(int i) {
        return (i - 1) / 2;
    }
    
    // MaxHeap::left(i)
    // Time: O(1) - simple arithmetic operation
    // Space: O(1) - no extra space needed
    int left(int i) {
        return 2 * i + 1;
    }
    
    // MaxHeap::right(i)
    // Time: O(1) - simple arithmetic operation
    // Space: O(1) - no extra space needed
    int right(int i) {
        return 2 * i + 2;
    }
    
    // MaxHeap::swap(i, j)
    // Time: O(1) - constant time swap
    // Space: O(1) - single temp variable
    void swap(int i, int j) {
        MaxHeapNode temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
    
    // MaxHeap::heapifyUp(index)
    // Time: O(log n) - worst case traverse height of tree
    // Space: O(1) - iterative implementation, no recursion stack
    void heapifyUp(int index) {
        while (index > 0 && arr[parent(index)].priority < arr[index].priority) {
            swap(index, parent(index));
            index = parent(index);
        }
    }
    
    // MaxHeap::heapifyDown(index)
    // Time: O(log n) - worst case traverse height of tree
    // Space: O(log n) - recursive call stack depth
    void heapifyDown(int index) {
        int largest = index;
        int l = left(index);
        int r = right(index);
        
        if (l < heapSize && arr[l].priority > arr[largest].priority) {
            largest = l;
        }
        if (r < heapSize && arr[r].priority > arr[largest].priority) {
            largest = r;
        }
        
        if (largest != index) {
            swap(index, largest);
            heapifyDown(largest);
        }
    }

public:
    MaxHeap(int size = 100) {
        capacity = size;
        heapSize = 0;
        arr = new MaxHeapNode[capacity];
    }
    
    // MaxHeap::insert(data, priority)
    // Time: O(log n) - insert at end then heapifyUp takes log n
    // Space: O(1) - only stores one new element
    void insert(void* data, double priority) {
        if (heapSize == capacity) {
            cout << "Heap is full!" << endl;
            return;
        }
        arr[heapSize] = MaxHeapNode(data, priority);
        heapifyUp(heapSize);
        heapSize++;
    }
    
    // MaxHeap::getMax()
    // Time: O(1) - root element always at index 0
    // Space: O(1) - no extra space needed
    void* getMax() {
        if (heapSize == 0) {
            return nullptr;
        }
        return arr[0].data;
    }
    
    // MaxHeap::extractMax()
    // Time: O(log n) - remove root, replace with last, heapifyDown takes log n
    // Space: O(log n) - recursive heapifyDown call stack
    void* extractMax() {
        if (heapSize == 0) {
            return nullptr;
        }
        void* maxData = arr[0].data;
        arr[0] = arr[heapSize - 1];
        heapSize--;
        heapifyDown(0);
        return maxData;
    }
    
    // MaxHeap::isEmpty()
    // Time: O(1) - simple comparison
    // Space: O(1) - no extra space
    bool isEmpty() {
        return heapSize == 0;
    }
    
    // MaxHeap::size()
    // Time: O(1) - return stored value
    // Space: O(1) - no extra space
    int size() {
        return heapSize;
    }
    
    ~MaxHeap() {
        delete[] arr;
    }
};

#endif

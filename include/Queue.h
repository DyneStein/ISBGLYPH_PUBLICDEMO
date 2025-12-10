#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
using namespace std;

// circular queue for integers (keeping it simple for passenger handling)
class Queue {
private:
    int* arr;
    int front;
    int rear;
    int capacity;
    int count;

public:
    Queue(int size = 100) {
        capacity = size;
        arr = new int[capacity];
        front = 0;
        rear = -1;
        count = 0;
    }
    
    // Queue::enqueue(value)
    // Time: O(1) - circular queue insert at rear
    // Space: O(1) - single integer added
    void enqueue(int value) {
        if (count == capacity) {
            cout << "Queue is full!" << endl;
            return;
        }
        rear = (rear + 1) % capacity;
        arr[rear] = value;
        count++;
    }
    
    // Queue::dequeue()
    // Time: O(1) - circular queue remove from front
    // Space: O(1) - no allocation
    int dequeue() {
        if (count == 0) {
            return -1;
        }
        int value = arr[front];
        front = (front + 1) % capacity;
        count--;
        return value;
    }
    
    // peek front element
    int peek() {
        if (count == 0) {
            return -1;
        }
        return arr[front];
    }
    
    // check if empty
    bool isEmpty() {
        return count == 0;
    }
    
    // check if full
    bool isFull() {
        return count == capacity;
    }
    
    // get size
    int size() {
        return count;
    }
    
    ~Queue() {
        delete[] arr;
    }
};

#endif

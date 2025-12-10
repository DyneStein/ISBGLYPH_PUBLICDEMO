#ifndef STACK_H
#define STACK_H

#include "String.h"
#include <iostream>
using namespace std;

// stack for strings (no templates)
class Stack {
private:
    char** arr;
    int top;
    int capacity;

public:
    Stack(int size = 100) {
        capacity = size;
        arr = new char*[capacity];
        top = -1;
    }
    
    // Stack::push(value)
    // Time: O(n) where n = value length for string copy
    // Space: O(n) for string storage
    void push(const char* value) {
        if (top == capacity - 1) {
            cout << "Stack overflow!" << endl;
            return;
        }
        int len = stringLength(value);
        arr[++top] = new char[len + 1];
        stringCopy(arr[top], value);
    }
    
    // Stack::pop()
    // Time: O(1) - just returns top element pointer
    // Space: O(1) - no allocation, returns existing pointer
    const char* pop() {
        if (top == -1) {
            return nullptr;
        }
        return arr[top--];
    }
    
    // peek top element
    const char* peek() {
        if (top == -1) {
            return nullptr;
        }
        return arr[top];
    }
    
    // check if empty
    bool isEmpty() {
        return top == -1;
    }
    
    // get size
    int size() {
        return top + 1;
    }
    
    ~Stack() {
        for (int i = 0; i <= top; i++) {
            delete[] arr[i];
        }
        delete[] arr;
    }
};

#endif

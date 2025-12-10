#ifndef TREE_H
#define TREE_H

#include "String.h"
#include <iostream>
using namespace std;

// node for n-ary tree (no templates)
struct TreeNode {
    int data;  // simple int data for now
    char* name;
    TreeNode** children;
    int childCount;
    int childCapacity;
    
    TreeNode(int val, const char* nodeName) {
        data = val;
        int len = stringLength(nodeName);
        name = new char[len + 1];
        stringCopy(name, nodeName);
        toUpperCase(name);  // ✅ CONVERT TO UPPERCASE! This was missing!
        childCapacity = 10;
        childCount = 0;
        children = new TreeNode*[childCapacity];
        for (int i = 0; i < childCapacity; i++) {
            children[i] = nullptr;
        }
    }
    
    // add child to this node
    void addChild(TreeNode* child) {
        if (childCount == childCapacity) {
            // expand capacity
            childCapacity *= 2;
            TreeNode** newChildren = new TreeNode*[childCapacity];
            for (int i = 0; i < childCount; i++) {
                newChildren[i] = children[i];
            }
            for (int i = childCount; i < childCapacity; i++) {
                newChildren[i] = nullptr;
            }
            delete[] children;
            children = newChildren;
        }
        children[childCount++] = child;
    }
    
    ~TreeNode() {
        delete[] name;
        for (int i = 0; i < childCount; i++) {
            delete children[i];
        }
        delete[] children;
    }
};

// n-ary tree structure (no templates)
class Tree {
private:
    TreeNode* root;
    
    // helper to print tree
    void printHelper(TreeNode* node, int level) {
        if (node == nullptr) return;
        
        for (int i = 0; i < level; i++) {
            cout << "  ";
        }
        cout << node->name << endl;
        
        for (int i = 0; i < node->childCount; i++) {
            printHelper(node->children[i], level + 1);
        }
    }
    
    // Helper: find node with given name (recursive, case-insensitive)
    TreeNode* findNode(TreeNode* node, const char* name) {
        if (node == nullptr) return nullptr;
        
        // Names are already uppercase from insert, so direct compare is fine
        // The input 'name' should also be uppercase for comparison.
        // This helper assumes 'name' is already normalized to uppercase.
        if (stringCompare(node->name, name) == 0) return node;
        
        for (int i = 0; i < node->childCount; i++) {
            TreeNode* result = findNode(node->children[i], name);
            if (result != nullptr) return result;
        }
        return nullptr;
    }

    // Helper for recursive deletion
    void deleteTree(TreeNode* node) {
        if (node == nullptr) return;
        for (int i = 0; i < node->childCount; i++) {
            deleteTree(node->children[i]);
        }
        delete node; // Delete the node itself after its children
    }

public:
    Tree() {
        root = nullptr;
    }
    
    // set root node
    void setRoot(int data, const char* name) {
        if (root != nullptr) deleteTree(root); // Use helper for full tree deletion
        root = new TreeNode(data, name); // TreeNode constructor converts name to uppercase
    }
    
    // get root
    TreeNode* getRoot() {
        return root;
    }
    
    // Tree::insert(parentName, data, childName)
    // Time: O(n) where n = nodes in tree (must find parent)
    // Space: O(d) where d = depth (recursion stack)
    // NOW CASE-INSENSITIVE
    bool insert(const char* parentName, int data, const char* childName) {
        // Normalize to uppercase for comparison and storage
        char upperParent[256]; // Increased buffer size for safety
        char upperChild[256];
        stringCopy(upperParent, parentName);
        stringCopy(upperChild, childName);
        toUpperCase(upperParent);
        toUpperCase(upperChild);

        if (root == nullptr) {
            // If tree is empty, the first node added becomes the root.
            // The instruction's insert implies root must exist, but original addNode handled this.
            // We'll keep the original behavior for root creation if tree is empty.
            setRoot(data, childName); // setRoot handles uppercase conversion
            return true;
        }
        
        // CHECK FOR DUPLICATES - prevent adding same child twice (case-insensitive)
        TreeNode* existing = findNode(root, upperChild);
        if (existing != nullptr) {
            return false;  // Already exists (case-insensitive), don't add
        }
        
        TreeNode* parent = findNode(root, upperParent);
        if (parent == nullptr) {
            return false; // parent not found (case-insensitive)
        }
        
        TreeNode* newNode = new TreeNode(data, childName); // TreeNode constructor converts name to uppercase
        parent->addChild(newNode);
        return true;
    }
    
    // Tree::find(name)
    // Time: O(n) - must search entire tree in worst case
    // Space: O(h) for recursion stack where h = tree height
    TreeNode* find(const char* name) {
        // Normalize search name to uppercase for comparison
        char upperName[256];
        stringCopy(upperName, name);
        toUpperCase(upperName);
        return findNode(root, upperName);
    }
    
    // Backward compatibility: addNode calls insert
    bool addNode(const char* parentName, int data, const char* childName) {
        return insert(parentName, data, childName);
    }
    
    // print tree structure
    void display() {
        if (root == nullptr) {
            cout << "Tree is empty" << endl;
            return;
        }
        printHelper(root, 0);
    }
    
    ~Tree() {
        if (root != nullptr) {
            delete root;
        }
    }
};

#endif

#ifndef GRAPH_H
#define GRAPH_H

#include "String.h"
#include <iostream>
using namespace std;

// Portable maximum double value (compatible across all systems)
// Used this cuz we were programming in VS code primarily and DLB_MAX was giving error for the Visual Studio
// Using a large but safe value instead of platform-specific DBL_MAX
const double GRAPH_MAX_DISTANCE = 1.0e+300;


// edge structure for adjacency list
struct Edge {
    int destIndex;
    double weight;
    Edge* next;
    
    Edge(int dest, double w) {
        destIndex = dest;
        weight = w;
        next = nullptr;
    }
};

// vertex structure
struct Vertex {
    char* name;
    double latitude;
    double longitude;
    Edge* edges;
    
    Vertex() {
        name = nullptr;
        latitude = 0;
        longitude = 0;
        edges = nullptr;
    }
    
    void init(const char* n, double lat, double lon) {
        int len = stringLength(n);
        name = new char[len + 1];
        stringCopy(name, n);
        latitude = lat;
        longitude = lon;
        edges = nullptr;
    }
    
    ~Vertex() {
        if (name != nullptr) {
            delete[] name;
        }
        while (edges != nullptr) {
            Edge* temp = edges;
            edges = edges->next;
            delete temp;
        }
    }
};

// graph with adjacency list (no templates)
class Graph {
private:
    Vertex* vertices;
    int vertexCount;
    int capacity;
    
    // find vertex index by name - CASE INSENSITIVE
    int findVertexIndex(const char* name) {
        for (int i = 0; i < vertexCount; i++) {
            char vertexNameUpper[100], searchNameUpper[100];
            stringCopy(vertexNameUpper, vertices[i].name);
            stringCopy(searchNameUpper, name);
            toUpperCase(vertexNameUpper);
            toUpperCase(searchNameUpper);
            
            if (stringCompare(vertexNameUpper, searchNameUpper) == 0) {
                return i;
            }
        }
        return -1;
    }

public:
    Graph(int size = 100) {
        capacity = size;
        vertexCount = 0;
        vertices = new Vertex[capacity];
    }
    
    // Graph::addVertex(name, lat, lon)
    // Time: O(V) - findVertexIndex scans all vertices
    // Space: O(1) - single vertex added
    bool addVertex(const char* name, double lat, double lon) {
        if (vertexCount >= capacity) {
            cout << "Graph is full!" << endl;
            return false;
        }
        if (findVertexIndex(name) != -1) {
            return false; // already exists
        }
        vertices[vertexCount].init(name, lat, lon);
        vertexCount++;
        return true;
    }
    
    // Graph::addEdge(source, dest, weight)
    // Time: O(V) - findVertexIndex for source and dest takes O(V) each
    // Space: O(1) - single edge node created
    bool addEdge(const char* source, const char* dest, double weight) {
        int srcIndex = findVertexIndex(source);
        int destIndex = findVertexIndex(dest);
        
        if (srcIndex == -1 || destIndex == -1) {
            return false;
        }
        
        // add edge to source's adjacency list
        Edge* newEdge = new Edge(destIndex, weight);
        newEdge->next = vertices[srcIndex].edges;
        vertices[srcIndex].edges = newEdge;
        
        return true;
    }
    
    // add bidirectional edge
    bool addBidirectionalEdge(const char* v1, const char* v2, double weight) {
        return addEdge(v1, v2, weight) && addEdge(v2, v1, weight);
    }
    
    // get vertex by name
    Vertex* getVertex(const char* name) {
        int index = findVertexIndex(name);
        if (index == -1) return nullptr;
        return &vertices[index];
    }
    
    // Graph::dijkstra(startName, distances, previous)
    // Time: O(V²) - current array-based implementation; TODO: optimize to O((V+E)log V) with heap
    // Space: O(V) - arrays for distances, previous, visited
    void dijkstra(const char* startName, double* distances, int* previous) {
        int startIndex = findVertexIndex(startName);
        if (startIndex == -1) return;
        
        // initialize
        bool* visited = new bool[vertexCount];
        for (int i = 0; i < vertexCount; i++) {
            distances[i] = GRAPH_MAX_DISTANCE;
            previous[i] = -1;
            visited[i] = false;
        }
        distances[startIndex] = 0;
        
        // main loop
        for (int count = 0; count < vertexCount; count++) {
            // find minimum distance vertex
            double minDist = GRAPH_MAX_DISTANCE;
            int minIndex = -1;
            for (int i = 0; i < vertexCount; i++) {
                if (!visited[i] && distances[i] < minDist) {
                    minDist = distances[i];
                    minIndex = i;
                }
            }
            
            if (minIndex == -1) break;
            visited[minIndex] = true;
            
            // update distances of neighbors
            Edge* edge = vertices[minIndex].edges;
            while (edge != nullptr) {
                int dest = edge->destIndex;
                double weight = edge->weight;
                if (!visited[dest] && distances[minIndex] + weight < distances[dest]) {
                    distances[dest] = distances[minIndex] + weight;
                    previous[dest] = minIndex;
                }
                edge = edge->next;
            }
        }
        
        delete[] visited;
    }
    
    // get path from source to destination
    void getPath(int* previous, int destIndex, int* path, int& pathLength) {
        pathLength = 0;
        if (previous[destIndex] == -1 && destIndex != 0) {
            return; // no path
        }
        
        // trace back
        int current = destIndex;
        int tempPath[100];
        int tempLen = 0;
        while (current != -1) {
            tempPath[tempLen++] = current;
            current = previous[current];
        }
        
        // reverse
        for (int i = 0; i < tempLen; i++) {
            path[i] = tempPath[tempLen - 1 - i];
        }
        pathLength = tempLen;
    }
    
    // get vertex count
    int getVertexCount() {
        return vertexCount;
    }
    
    // get vertex name by index
    const char* getVertexName(int index) {
        if (index >= 0 && index < vertexCount) {
            return vertices[index].name;
        }
        return nullptr;
    }
    
    // get vertex coordinates
    void getVertexCoords(int index, double& lat, double& lon) {
        if (index >= 0 && index < vertexCount) {
            lat = vertices[index].latitude;
            lon = vertices[index].longitude;
        }
    }
    
    // get edge weight between two vertices (returns 0 if not connected)
    double getEdgeWeight(const char* source, const char* dest) {
        int srcIndex = findVertexIndex(source);
        int destIndex = findVertexIndex(dest);
        
        if (srcIndex == -1 || destIndex == -1) {
            return 0.0;
        }
        
        // search through edges
        Edge* edge = vertices[srcIndex].edges;
        while (edge != nullptr) {
            if (edge->destIndex == destIndex) {
                return edge->weight;
            }
            edge = edge->next;
        }
        
        return 0.0; // not connected
    }
    
    // display graph
    void display() {
        for (int i = 0; i < vertexCount; i++) {
            cout << vertices[i].name << " (" << vertices[i].latitude 
                 << ", " << vertices[i].longitude << "): ";
            Edge* edge = vertices[i].edges;
            while (edge != nullptr) {
                cout << vertices[edge->destIndex].name << "(" << edge->weight << ") ";
                edge = edge->next;
            }
            cout << endl;
        }
    }
    
    ~Graph() {
        delete[] vertices;
    }
};

#endif

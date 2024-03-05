// map.h

#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <malloc.h>
#include "pthread.h"

typedef struct {
    void* data;
    size_t size;
} ByteArray;

typedef struct Node {
    signed long int key;
    ByteArray value;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    size_t size;
    Node* tail;
    size_t maxSize;
    pthread_mutex_t mutex;
} Map;

void initializeMap(Map* map, size_t maxSize);
Node* createNode(signed long int key, ByteArray value);
void insert(Map* map, signed long int key, ByteArray value);
Node* findNode(Map* map, signed long int key);
ByteArray get(Map* map, signed long int key);
void removeNode(Map* map, signed long int key);
void clear(Map* map);
void freeMap(Map* map);
void setSize(Map* map, size_t newSize);

#endif // MAP_H

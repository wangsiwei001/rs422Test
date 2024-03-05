//
// Created by 10593 on 2023/11/27.
//

#include <stdint.h>
#include <malloc.h>
#include "pthread.h"
#include "map.h"

void initializeMap(Map* map, size_t maxSize) {
    map->head = NULL;
    map->size = 0;
    map->tail = NULL;
    map->maxSize = maxSize;
    pthread_mutex_init(&map->mutex, NULL);
}

Node* createNode(signed long int key, ByteArray value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->value = value;
    newNode->next = NULL;
    return newNode;
}

void insert(Map* map, signed long int key, ByteArray value) {
    pthread_mutex_lock(&map->mutex);

    if (map->size >= map->maxSize) {
        // 如果超过最大数量，删除最早插入的元素
        Node* toRemove = map->head;
        map->head = map->head->next;
        free(toRemove->value.data);
        free(toRemove);
        map->size--;
    }

    Node* newNode = createNode(key, value);
    if (map->head == NULL || key < map->head->key) {
        newNode->next = map->head;
        map->head = newNode;
        if (map->tail == NULL) {
            map->tail = newNode;
        }
    } else {
        Node* current = map->head;
        while (current->next != NULL && key >= current->next->key) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
        if (newNode->next == NULL) {
            map->tail = newNode;
        }
    }
    map->size++;

    pthread_mutex_unlock(&map->mutex);
}

Node* findNode(Map* map, signed long int key) {
    Node* current = map->head;
    while (current != NULL) {
        if (current->key == key) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

ByteArray get(Map* map, signed long int key) {
    pthread_mutex_lock(&map->mutex);

    Node* node = findNode(map, key);
    ByteArray emptyByteArray = {NULL, 0};
    ByteArray result = (node != NULL) ? node->value : emptyByteArray;

    pthread_mutex_unlock(&map->mutex);

    return result;
}

void removeNode(Map* map, signed long int key) {
    pthread_mutex_lock(&map->mutex);

    Node* current = map->head;
    Node* prev = NULL;
    while (current != NULL && current->key != key) {
        prev = current;
        current = current->next;
    }
    if (current != NULL) {
        if (prev != NULL) {
            prev->next = current->next;
        } else {
            map->head = current->next;
        }
        free(current->value.data);
        free(current);
        map->size--;
        if (current == map->tail) {
            map->tail = prev;
        }
    }

    pthread_mutex_unlock(&map->mutex);
}

void clear(Map* map) {
    pthread_mutex_lock(&map->mutex);

    Node* current = map->head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp->value.data);
        free(temp);
    }
    map->head = NULL;
    map->tail = NULL;
    map->size = 0;

    pthread_mutex_unlock(&map->mutex);
}

void freeMap(Map* map) {
    clear(map);
    pthread_mutex_destroy(&map->mutex);
}

void setSize(Map* map, size_t newSize) {
    pthread_mutex_lock(&map->mutex);

    while (map->size > newSize) {
        Node* toRemove = map->head;
        map->head = map->head->next;
        free(toRemove->value.data);
        free(toRemove);
        map->size--;
    }

    pthread_mutex_unlock(&map->mutex);
}
//
// Created by rock on 22/02/2021
//

#ifndef JNA_SYSTEM_H
#define JNA_SYSTEM_H

// memory management system

struct _MemoryManager {
    int size;           // size of the list in terms of items
    int dataSize;       // current capacity of the list
    void** list;        // the list
    int* allocatedSize; // how much we have allocated in this item
    // reverse fast lookup
    int mapSize;        // the size of this map
    long* keyArray;     // the keys
    int* dataArray;     // the index of this item in list
};

typedef struct _MemoryManager MemoryManager;

// create a new memory manager
MemoryManager* p_create(int initialCapacity);
// allocate n x size, member of a group
void* p_alloc(MemoryManager* mgr, int n, int size);
// get the size of all items allocated
long p_size(MemoryManager* mgr);
// free a particular member
void p_free(MemoryManager* mgr, void* data);
// free all members
void p_free_all(MemoryManager* mgr);
// free all members and destroy the manager
void p_destroy(MemoryManager* mgr);
// free a return-result
void freeReturnResult(void* data);

#endif //JNA_SYSTEM_H

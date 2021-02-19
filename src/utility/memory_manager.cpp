//
// Created by rock on 28/12/19.
//

#include "memory_manager.h"

#include <malloc.h>
#include <sys/time.h>
#include <assert.h>

long __t1 = 0;
const char* __currentTimer = "";

long getTimeInMilliseconds() {
    struct timeval start;
    gettimeofday(&start, NULL);
    return start.tv_sec * 1000 + start.tv_usec / 1000;
}

/////////////////////////////////////////////////////////////////////////////
// memory management system

// create the hash-value index for the given key
int memCalcHashIndex(long keyValue, int mapSize) {
    int keyHash = (int)((keyValue ^ (keyValue >> 32)) ^ (long)mapSize << 12);
    return ((int)keyHash & 0x7fffffff) % mapSize;
}

// lohRemap a value into a new array
int memRemap(long* newKeyArray, int* newDataArray, int newSize, long keyValue, int data) {
    int hashIndex = memCalcHashIndex(keyValue, newSize);
    while (newKeyArray[hashIndex] != -1L) {
        hashIndex = (hashIndex + 1) % newSize;
    }
    if (newKeyArray[hashIndex] == -1L) {
        newKeyArray[hashIndex] = keyValue;
        newDataArray[hashIndex] = data;
        return 1;
    }
    return 0;
}

// get a value from the map or -1
int memGet(MemoryManager* map, long keyValue) {
    int hashIndex = memCalcHashIndex(keyValue, map->dataSize);
    while (map->keyArray[hashIndex] != keyValue && map->keyArray[hashIndex] != -1L) {
        hashIndex = (hashIndex + 1) % map->dataSize;
    }
    if (map->keyArray[hashIndex] == keyValue) {
        return map->dataArray[hashIndex];
    }
    return -1;
}

// update an existing, or put a new item into the map and auto-grow the map if necessary
void memPut(MemoryManager* mgr, long keyValue, int data) {
    int hashIndex = memCalcHashIndex(keyValue, mgr->dataSize);
    while (mgr->keyArray[hashIndex] != keyValue && mgr->keyArray[hashIndex] != -1L) {
        hashIndex = (hashIndex + 1) % mgr->dataSize;
    }
    if (mgr->keyArray[hashIndex] == keyValue) {
        mgr->dataArray[hashIndex] = data;
    } else if (mgr->keyArray[hashIndex] == -1L) {
        mgr->keyArray[hashIndex] = keyValue;
        mgr->dataArray[hashIndex] = data;
    }
}

// create a new memory manager
MemoryManager* p_create(int initialCapacity) {
    MemoryManager* mgr = (MemoryManager*)calloc(1, sizeof(MemoryManager));
    mgr->size = 0;
    mgr->dataSize = initialCapacity;
    mgr->list = (void**)calloc(initialCapacity, sizeof(void*));
    mgr->allocatedSize = (int*)calloc(initialCapacity, sizeof(int));

    // reverse lookup;  void* -> int offset into list
    mgr->mapSize = ((initialCapacity * 3) / 2) + 1;
    mgr->keyArray = (long*)calloc(mgr->mapSize, sizeof(long));
    mgr->dataArray = (int*)calloc(mgr->mapSize, sizeof(int));
    for (int i = 0; i < mgr->mapSize; i++) {
        mgr->keyArray[i] = -1L;
        mgr->dataArray[i] = -1;
    }
    return mgr;
}

// grow a list 50%
void memGrow(MemoryManager* mgr, int newSize) {
    void** newList = (void**)calloc(newSize, sizeof(void*));
    assert(newList != NULL);
    int* newAllocatedSize = (int*)calloc(newSize, sizeof(int));
    assert(newAllocatedSize != NULL);

    int newMapSize = ((newSize * 3) / 2) + 1;
    long* newKeyArray = (long*)calloc(newMapSize, sizeof(long));
    assert(newKeyArray != NULL);
    int* newDataArray = (int*)calloc(newMapSize, sizeof(int));
    assert(newDataArray != NULL);

    for (int i = 0; i < newMapSize; i++) {
        newKeyArray[i] = -1L;
        newDataArray[i] = -1;
    }

    for (int i = 0; i < mgr->size; i++) {
        newList[i] = mgr->list[i];
        newAllocatedSize[i] = mgr->allocatedSize[i];
    }

    for (int i = 0; i < mgr->mapSize; i++) {
        if (mgr->keyArray[i] != -1L) {
            memRemap(newKeyArray, newDataArray, newMapSize, mgr->keyArray[i], mgr->dataArray[i]);
        }
    }

    free(mgr->list); // dispose of old list
    free(mgr->allocatedSize); // dispose of old list
    free(mgr->keyArray); // dispose of old list
    free(mgr->dataArray); // dispose of old list

    mgr->dataSize = newSize;
    mgr->list = newList;
    mgr->allocatedSize = newAllocatedSize;
    mgr->mapSize = newMapSize;
    mgr->keyArray = newKeyArray;
    mgr->dataArray = newDataArray;
}

// add an item to the list
void memAdd(MemoryManager* mgr, void* item, int size) {
    if (mgr->size + 1 >= mgr->dataSize) {
        int newSize = ((mgr->size * 3) / 2) + 1;
        memGrow(mgr, newSize);
    }
    mgr->list[mgr->size] = item;
    mgr->allocatedSize[mgr->size] = size;
    memPut(mgr, (long)item, mgr->size); // reverse map
    mgr->size += 1;
}

// allocate an amount of storage for a given memory group
void* p_alloc(MemoryManager* mgr, int n, int size) {
    assert(mgr != NULL);
    if (n > 0 && size > 0) {
        void *data = calloc(n, size);
        assert(data != NULL);
        memAdd(mgr, data, n * size);
        return data;
    }
    return NULL;
}

// free all items
void p_free_all(MemoryManager* mgr) {
    if (mgr != NULL) {
        for (int i = 0; i < mgr->size; i++) {
            if (mgr->list[i] != NULL) {
                free(mgr->list[i]);
                mgr->list[i] = NULL;
                mgr->allocatedSize[i] = 0;
            }
        }
        for (int i = 0; i < mgr->mapSize; i++) {
            mgr->keyArray[i] = -1L;
            mgr->dataArray[i] = -1;
        }
        mgr->size = 0;
    }
}

// free all items
void p_free(MemoryManager* mgr, void* data) {
    if (mgr != NULL && data != NULL) {
        int index = memGet(mgr, (long)data);
        if (index >= 0 && index < mgr->size) {
            if (mgr->list[index] != NULL) {
                free(mgr->list[index]);
                mgr->list[index] = NULL;
                mgr->allocatedSize[index] = 0;
            }
        }
    }
}

// return how much RAM we have allocated total
long p_size(MemoryManager* mgr) {
    long size = 0;
    for (int i = 0; i < mgr->size; i++) {
        size += mgr->allocatedSize[i];
    }
    return size;
}

// utterly destroy all memory used by the manager
void p_destroy(MemoryManager* mgr) {
    p_free_all(mgr);
    if (mgr->list != NULL) free(mgr->list);
    if (mgr->allocatedSize != NULL) free(mgr->allocatedSize);
    if (mgr->keyArray != NULL) free(mgr->keyArray);
    if (mgr->dataArray != NULL) free(mgr->dataArray);
    free(mgr);
}

// free a memory block
void freeReturnResult(void* data) {
    if (data != NULL) {
        free(data);
    }
}


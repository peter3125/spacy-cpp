//
// Created by rock on 18/12/19.
//

#ifndef JNA_ARRAY_LIST_H
#define JNA_ARRAY_LIST_H

#include "memory_manager.h"

struct _ArrayList {
    int size;           // size of the list in terms of items
    int dataSize;       // current capacity of the list
    void** list;        // the list
    MemoryManager* mgr; // the list's memory manager
};

typedef struct _ArrayList ArrayList;

// create a new list
ArrayList* listCreate(int initialCapacity, MemoryManager* mgr);

// add an item to the list
void listAdd(ArrayList* list, void* item);

// add all items of list2 to list1
void listAddAll(ArrayList* list1, ArrayList* list2);

#endif //JNA_ARRAY_LIST_H

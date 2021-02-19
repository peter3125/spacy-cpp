//
// Created by rock on 18/12/19.
//

#include "array_list.h"


// create a new list
ArrayList* listCreate(int initialCapacity, MemoryManager* mgr) {
    ArrayList* list = p_alloc(mgr, 1, sizeof(ArrayList));
    list->mgr = mgr;
    list->size = 0;
    list->dataSize = initialCapacity;
    list->list = p_alloc(mgr, initialCapacity, sizeof(void*));
    return list;
}

// grow a list 50%
void growList(ArrayList* list, int newSize) {
    void** newList = p_alloc(list->mgr, newSize, sizeof(void*));
    for (int i = 0; i < list->size; i++) {
        newList[i] = list->list[i];
    }
    p_free(list->mgr, list->list); // dispose of old list
    list->dataSize = newSize;
    list->list = newList;
}

// add an item to the list
void listAdd(ArrayList* list, void* item) {
    if (list->size + 1 >= list->dataSize) {
        int newSize = ((list->size * 3) / 2) + 1;
        growList(list, newSize);
    }
    list->list[list->size] = item;
    list->size += 1;
}

// add all items of list2 to list1
void listAddAll(ArrayList* list1, ArrayList* list2) {
    if (list1->size + list2->size >= list1->dataSize) {
        growList(list1, list1->size + list2->size);
    }
    for (int i = 0; i < list2->size; i++) {
        listAdd(list1, list2->list[i]);
    }
}

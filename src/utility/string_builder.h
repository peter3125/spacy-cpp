//
// Created by rock on 18/12/19.
//

#ifndef JNA_STRING_BUILDER_H
#define JNA_STRING_BUILDER_H

#include "memory_manager.h"

struct _StringBuilder {
    int size;           // current size of the sb
    int dataSize;       // the size of the data block
    char* data;         // the data block
};

typedef struct _StringBuilder StringBuilder;

// create a new string-builder
StringBuilder* sb_create(MemoryManager* mgr, int initialSize);

// add a string to the string builder
void append(StringBuilder* sb, const char* str, MemoryManager* mgr);

// add a long to the string builder
void append(StringBuilder* sb, long value, MemoryManager* mgr);

// convert sb to independent memory string
const char* toString(StringBuilder* sb);

// remove memory allocated by calloc
extern "C" void freeString(const char* str);

#endif //JNA_STRING_BUILDER_H

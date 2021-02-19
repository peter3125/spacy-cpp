//
// Created by rock on 19/02/2021.
//

#include <cstring>
#include <cstdio>
#include <malloc.h>

#include "string_builder.h"


// create a new string-builder
StringBuilder* sb_create(MemoryManager* mgr, int initialSize) {
    StringBuilder* sb = (StringBuilder*)p_alloc(mgr, 1, sizeof(StringBuilder));
    sb->size = 0;
    sb->dataSize = initialSize;
    sb->data = (char*)p_alloc(mgr, 1, initialSize);
    return sb;
}


// add a string to the string builder
void append(StringBuilder * sb, const char* str, MemoryManager* mgr) {
    if (str != nullptr && sb != nullptr) {
        int len = strlen(str);
        if (len > 0) {
            if (len + sb->size >= sb->dataSize) {
                // grow the string-builder
                int newSize = ((sb->dataSize * 3) / 2) + 1;
                if (newSize < len + sb->size) {
                    newSize = len + sb->size + 1;
                }
                char* newData = (char*)p_alloc(mgr, 1, newSize);
                if (sb->size > 0) {
                    memcpy(newData, sb->data, sb->size);
                }
                strcpy(&newData[sb->size], str);
                sb->data = newData;
                sb->size += (len + 1);
                sb->dataSize = newSize;
            }
        }
    }
}


void append(StringBuilder* sb, long value, MemoryManager* mgr) {
    char str[256];
    sprintf(str, "%ld", value);
    append(sb, str, mgr);
}


const char* toString(StringBuilder* sb) {
    if (sb != nullptr && sb->size > 0) {
        char* str = (char*)calloc(sb->size, sizeof(char));
        strcpy(str, sb->data);
        return str;
    }
    return "";
}



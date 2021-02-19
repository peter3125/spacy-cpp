//
// Created by rock on 19/02/2021.
//

#include <iostream>

#include "spacy.h"
#include "nlp.h"
#include "doc.h"
#include "token.h"
#include "span.h"
#include "../utility/memory_manager.h"
#include "../utility/array_list.h"
#include "../utility/string_builder.h"


// global allocated class
Spacy::Spacy spacy;
auto nlp = spacy.load("en_core_web_sm");


// helper - split a string beyond size maxLength into smaller chunks
ArrayList* largeTextToList(const char* _text, int maxLength, MemoryManager* mgr) {
    char* text = (char*)_text;
    int len = strlen(text);
    ArrayList* list = listCreate((len / maxLength) + 20, mgr);
    int maxLength90pc = (maxLength - (maxLength / 10));
    while (len + 1 > maxLength) {
        int location = maxLength;
        while (location + 1 > maxLength90pc && !(text[location] == '.' && text[location + 1] == ' ')) {
            location -= 1;
        }
        if (location + 1 > maxLength90pc) {
            // skip space and the full-stop we found
            location += 2;
        } else {
            location = maxLength;
        }
        // split the text around location
        char* part1 = (char*)p_alloc(mgr, 1, location + 1);
        memccpy((void*)part1, text, 1, location);
        part1[location] = '\0';
        listAdd(list, part1);

        int leftOverSize = len - location;
        char* part2 = (char*)p_alloc(mgr, 1, leftOverSize + 1);
        memccpy((void*)part2, &text[location], 1, leftOverSize);
        part2[leftOverSize] = 0;
        text = part2;
        len = leftOverSize;
    }
    if (len > 0) {
        listAdd(list, text);
    }
    return list;
}


/**
 * parse a string using spacy
 */
extern "C" void* parse(const char* text) {
    if (text != nullptr) {
        MemoryManager* mgr = p_create(2500);
        ArrayList* list = largeTextToList(text, 20000, mgr);
        StringBuilder* sb = sb_create(mgr, 163840);
        append(sb, "[", mgr);
        for (int i = 0; i < list->size; i++) {
            const char* text1 = (const char*)list->list[i];
            auto docs = nlp.parse(text1);
            append(sb, "[", mgr);
            for (auto& sentence : docs.sents()) {
                for (auto &token : sentence.tokens()) {
                    append(sb, "{", mgr);

                    append(sb, "\"token\":", mgr);
                    append(sb, "\"", mgr);
                    append(sb, token.text().c_str(), mgr);
                    append(sb, "\",", mgr);

                    append(sb, "\"i\":", mgr);
                    append(sb, token.i(), mgr);
                    append(sb, ",", mgr);

                    append(sb, "\"tag\":", mgr);
                    append(sb, "\"", mgr);
                    append(sb, token.tag_().c_str(), mgr);
                    append(sb, "\",", mgr);

                    append(sb, "\"dep\":", mgr);
                    append(sb, "\"", mgr);
                    append(sb, token.dep_().c_str(), mgr);
                    append(sb, "\",", mgr);

                    append(sb, "\"lemma\":", mgr);
                    append(sb, "\"", mgr);
                    append(sb, token.lemma_().c_str(), mgr);
                    append(sb, "\",", mgr);

                    append(sb, "},", mgr);
                }
            }
            append(sb, "],", mgr);
        }
        append(sb, "]", mgr);

        const char* str = toString(sb);
        p_destroy(mgr);
        return (void*)str;
    }
    return nullptr;
}


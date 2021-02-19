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
extern "C" const char* parse(const char* text) {
    if (text != nullptr) {
        MemoryManager* mgr = p_create(250000);
        ArrayList* list = largeTextToList(text, 20000, mgr);
        std::cout << "[";
        for (int i = 0; i < list->size; i++) {
            const char* text1 = (const char*)list->list[i];
            auto docs = nlp.parse(text1);
            std::cout << "[";
            for (auto& sentence : docs.sents()) {
                for (auto &token : sentence.tokens()) {
                    std::cout << "{";
                    std::cout << "\"token\":" << "\"" << token.text() << "\",";
                    std::cout << "\"i\":" << "\"" << token.i() << "\",";
                    std::cout << "\"tag\":" << "\"" << token.tag_() << "\",";
                    std::cout << "\"dep\":" << "\"" << token.dep_() << "\",";
                    std::cout << "\"lemma\":" << "\"" << token.lemma_() << "\",";
                    std::cout << "\"left\":" << "\"" << token.left_edge().i() << "\",";
                    std::cout << "\"right\":" << "\"" << token.right_edge().i() << "\",";
                    std::cout << "\"root\":" << "\"" << token.head().i() << "\",";
                    std::cout << "},";
                }
            }
            std::cout << "],";
        }
        std::cout << "]";
        p_destroy(mgr);
    }
    return "";
}

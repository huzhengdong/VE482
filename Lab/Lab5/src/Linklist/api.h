//
// Created by 13 on 2020/10/28.
//

#ifndef LINKLIST_API_H
#define LINKLIST_API_H
#include "kernel.h"


struct node* Search(struct list* List, void* key, const char* key_type);
void Print(struct list* List, char* key_type);
void Free(struct list* List);
struct list* ReadFile(const char* filename);
void Sort_List(struct list* List, int(*cmp)(const void*, const void*));
void WriteFile(struct list* List, char* datatype, char* sortingtype);

#endif //LINKLIST_API_H

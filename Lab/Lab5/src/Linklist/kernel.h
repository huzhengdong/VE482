//
// Created by 13 on 2020/10/28.
//

#ifndef LINKLIST_KERNEL_H
#define LINKLIST_KERNEL_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAXLINE 100
struct node{
    char* data;
    void* key;
    struct node* next;
};

struct list{
    struct node* head;
    int length;
};
int inc_int(const void* a, const void* b);
int dec_int(const void* a, const void* b);
int rand_int(const void* a, const void* b);
int inc_double(const void* a, const void* b);
int dec_double(const void* a, const void* b);
int rand_double(const void* a, const void* b);
int dec_char(const void* a, const void* b);
int inc_char(const void* a, const void* b);
int rand_char(const void* a, const void* b);
void InsertFirst(struct list* List, char* data, void* key);
void InsertLast(struct list* List, char* data, void* key);
void swap(struct node* a, struct node* b);
#endif //LINKLIST_KERNEL_H

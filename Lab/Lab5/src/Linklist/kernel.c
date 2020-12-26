//
// Created by 13 on 2020/10/28.
//

#include "kernel.h"

int inc_int(const void* a, const void* b){
    return *((int*)a)>*((int*)b);
}
int dec_int(const void* a, const void* b){
    return *((int*)a)<=*((int*)b);
}
int rand_int(const void* a, const void* b){
    return ((rand()%2)*2-1);
}
int inc_double(const void* a, const void* b){
    return *((double*)a)>*((double*)b);
}

int dec_double(const void* a, const void* b){
    return *((double*)a)<=*((double*)b);
}
int rand_double(const void* a, const void* b){
    return ((rand()%2)*2-1);
}
int dec_char(const void* a, const void* b){
    return strcmp((char*)a, (char*)b);
}
int inc_char(const void* a, const void* b){
    return dec_char(b,a);
}
int rand_char(const void* a, const void* b){
    return ((rand()%2)*2-1);
}

void InsertFirst(struct list* List, char* data, void* key)
{
    struct node* head=List->head;
    struct node* new=(struct node*)malloc(sizeof(struct node));
    new->data=(char*)malloc(sizeof(char)*(strlen(data)+1));
    memset(new->data, 0, (strlen(data)+1));
    memcpy(new->data, data, strlen(data));
    new->key=key;


    new->next=head;
    head=new;
    List->head=head;
    List->length++;
}
void InsertLast(struct list* List, char* data, void* key)
{
    struct node* head=List->head;
    struct node* new=(struct node*)malloc(sizeof(struct node));
    new->data=(char*)malloc(sizeof(char)*(strlen(data)+1));
    memset(new->data, 0, (strlen(data)+1));
    memcpy(new->data, data, strlen(data));
    new->key=key;

    struct node* last=head;
    while(last!=NULL&&last->next!=NULL)
        last=last->next;

    last->next=new;
    last=new;
    List->length++;
}


void swap(struct node* a, struct node* b)
{
    //swap the value
    char* datacopy=a->data;
    void* keycopy=a->key;
    a->data=b->data;
    a->key=b->key;
    b->data=datacopy;
    b->key=keycopy;
}


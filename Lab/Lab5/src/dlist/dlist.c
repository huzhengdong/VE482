//
// Created by 13 on 2020/10/28.
//
#include "dlist.h"
#define MAXLINE 100

dlist createDlist(dlistValueType type) {
    dlist new;
    if (type == DLIST_INT)
        new = (int*) malloc(sizeof(int));
    else if (type == DLIST_STR)
        new = (char*) malloc(MAXLINE * sizeof(char));
    else if (type == DLIST_DOUBLE)
        new = (double*)malloc(sizeof(double));
    else
        new=NULL;
    return new;
}

int dlistIsEmpty(dlist_const this){
    return (this->head==NULL);
}

void dlistAppend(dlist this, const char* key, dlistValue value){
    InsertLast(this, key, value);
}
void dlistSort(dlist_const this, dlist listDst, dlistSortMethod method)
// REQUIRES: argument 'this' is not null and is a valid list
// argument 'listDst' is not null and is a valid list
// EFFECTS: Sort 'listSrc' using method given by 'method'.
// Put the results in 'listDst' on success
// Leave 'listDst' unchanged on failure
// * Note 'listDst' may be non-empty when invoked.
// * Be very careful with memory management regarding string lists.
// MODIFIES: argument 'listDst'
{
    copy(this, listDst);//without implementation in libaries
    cmp=getcmp(method); //without implementation in libraries
    Sort_List(listDst, cmp);//without knowing the datatype to get cmp function
}

void dlistPrint(dlist_const this)
// REQUIRES: argument 'this' is not null and is a valid list
// EFFECTS: Print the content of 'this' in the required format to standard output.
// A newline must be followed after each line (including the last one).
{
    Print(this);//without knowing the datatype
}
void dlistFree(dlist this)
{
    Free(this);
}
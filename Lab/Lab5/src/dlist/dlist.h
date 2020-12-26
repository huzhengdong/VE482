//
// Created by 13 on 2020/10/28.
//

#ifndef LINKLIST_DLIST_H
#define LINKLIST_DLIST_H
// Data type stored in the dlist
typedef enum dlistValueType_t {
    DLIST_INT = 0x01, // list contains int values
    DLIST_STR = 0x02, // list contains char* values
    DLIST_DOUBLE = 0x03, // list contains double values
    DLIST_UNKOWN = 0x00
    } dlistValueType;
// Different options for sorting // String are sorted in lexical order
typedef enum dlistSortMethod_t {
    DLIST_SORT_RAND = 0x01, // Randomize entries
    DLIST_SORT_INC = 0x02, // Sort in ascending order
    DLIST_SORT_DEC = 0x03, // Sort in descending order
    DLIST_SORT_UNKOWN = 0x00
    } dlistSortMethod;
// The value stored in the dlist // Research online what an "union" is.
typedef union dlistValue_t {
    int intValue;
    double doubleValue;
    char* strValue;
    } dlistValue;
typedef void* dlist;
typedef const void* dlist_const;

dlist createDlist(dlistValueType type);
int dlistIsEmpty(dlist_const this);
void dlistAppend(dlist this, const char* key, dlistValue value);
void dlistSort(dlist_const this, dlist listDst, dlistSortMethod method);
void dlistPrint(dlist_const this);
void dlistFree(dlist this);

#endif //LINKLIST_DLIST_H

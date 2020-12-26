//
// Created by 13 on 2020/10/28.
//

#include "api.h"

struct node* Search(struct list* List, void* key, const char* key_type)
{
    struct node* iterate=List->head;
    while(iterate!=NULL)
    {
        if(strcmp(key_type, "char")==0)
        {
            if(strcmp((char*)key, (char*)(iterate->key))==0)
                return iterate;
        }
        else if(strcmp(key_type, "int")==0)
        {
            if(*((int*)key)==*((int*)iterate->key))
                return iterate;
        }
        else if(strcmp(key_type, "double")==0)
        {
            if(*((double*)key)==*((double*)iterate->key))
                return iterate;
        }
        else{}
        iterate=iterate->next;
    }
    return NULL;
}

void Print(struct list* List, char* key_type)
{
    struct node* iterate=List->head;
    if(iterate==NULL)
        printf("NULL\n");
    while(iterate!=NULL) {
        if (strcmp(key_type, "char") == 0) {
            printf("%s=%s\n", iterate->data, iterate->key);
        } else if (strcmp(key_type, "int") == 0) {
            printf("%s=%d\n", iterate->data, *(int *) (iterate->key));
        } else if (strcmp(key_type, "double") == 0) {
            printf("%s=%lf\n", iterate->data, *(double *) (iterate->key));
        } else {
        }
        iterate = iterate->next;
    }
}

void Free(struct list* List)
{
    struct node* iterate=List->head;
    while(iterate!=NULL)
    {
        free(iterate->data);
        free(iterate->key);
        struct node* copy=iterate;
        iterate=iterate->next;
        free(copy);
    }
    free(List);
}
struct list* ReadFile(const char* filename)
{
    struct list* List=malloc(sizeof(struct list));
    struct node* head=NULL;
    List->length=0;
    List->head=head;

    char datatype[10];
    char  sortingtype[10];
    char  filetype[10];
    memset(datatype,0,10);
    memset(sortingtype,0,10);
    memset(filetype, 0, 10);

    sscanf(filename,"%[a-zA-Z]_%[a-zA-Z].%[a-zA-Z]", sortingtype, datatype, filetype);

    FILE *fp=fopen(filename, "r");
    if(fp==NULL)
        printf("file open error\n");
    char buff[255];
    while(fgets(buff, 255, fp)!=NULL)
    {
        char somestring[255];
        memset(somestring, 0, 255);
        void* somedata;
        if(strcmp(datatype, "char")==0) {
            somedata=malloc(255);
            sscanf(buff, "%[^=]=%s\n", somestring, somedata);
        }
        else if(strcmp(datatype, "int")==0)
        {
            somedata=malloc(sizeof(int));
            sscanf(buff,"%[^=]=%d\n",somestring, somedata);
        }
        else if(strcmp(datatype, "double")==0)
        {
            somedata=malloc(sizeof(double));
            sscanf(buff,"%[^=]=%lf\n",somestring, somedata);
        }
        else{
            continue;
        }
        InsertFirst(List,somestring, somedata);
    }
    fclose(fp);
    return List;
}
void Sort_List(struct list* List, int(*cmp)(const void*, const void*))
{
    int HasSwap=1;
    while(HasSwap!=0){
        struct node* iterate=List->head;
        HasSwap=0;
        while(iterate!=NULL){
            if(iterate->next!=NULL){
                if(cmp(iterate->key, iterate->next->key)>0){
                    swap(iterate, iterate->next);
                    HasSwap=1;
                }
            }
            iterate=iterate->next;
        }

    }
}

void WriteFile(struct list* List, char* datatype, char* sortingtype)
{
    char filename[20];
    char line[255];
    memset(filename,0,20);
    sprintf(filename, "%s_%s.txt", sortingtype, datatype);
    FILE* fp=fopen(filename, "w");
    struct node* iterate=List->head;
    int (*cmp)(const void*, const void*);
    if(strcmp(datatype, "char")==0) {
        if(strcmp(sortingtype, "inc")==0)
            cmp=&inc_char;
        else if(strcmp(sortingtype,"dec")==0)
            cmp=&dec_char;
        else
            cmp=&rand_char;
    }
    else if(strcmp(datatype, "int")==0){
        if(strcmp(sortingtype, "inc")==0)
            cmp=&inc_int;
        else if(strcmp(sortingtype,"dec")==0)
            cmp=&dec_int;
        else
            cmp=&rand_int;
    }
    else if(strcmp(datatype, "double")==0){
        if(strcmp(sortingtype, "inc")==0)
            cmp=&inc_double;
        else if(strcmp(sortingtype,"dec")==0)
            cmp=&dec_double;
        else
            cmp=&rand_double;
    }
    else{
        cmp=&rand_char;
    }
    Sort_List(List,cmp);
    while(iterate!=NULL)
    {
        memset(line, 0,255);
        if(strcmp(datatype, "char")==0) {
            sprintf(line, "%s=%s\n", iterate->data, (char*)(iterate->key));
        }
        else if(strcmp(datatype, "int")==0){
            sprintf(line,"%s=%d\n",iterate->data, *(int*)(iterate->key));
        }
        else if(strcmp(datatype, "double")==0){
            sprintf(line,"%s=%lf\n",iterate->data, *(double*)(iterate->key));
        }
        else{
            continue;
        }
        fputs(line, fp);
        iterate=iterate->next;
    }
    fclose(fp);
}
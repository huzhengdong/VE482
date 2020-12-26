//
// Created by 13 on 2020/10/28.
//

#include "interface.h"
void Cmdinterface(int argc, char* argv[])
{
    struct list* l1=ReadFile(argv[1]);
    char type[10];
    char order[10];
    memset(order,0, 10);
    memset(type, 0, 10);
    sscanf(argv[1], "%[^_]_%[^.]%[*]", order,type);
    WriteFile(l1,type,argv[2]);
    Print(l1,type);
    Free(l1);
}

void Menuinterface(int argc, char* argv[])
{
    printf("Welcome to Linked List\n");
    char filename[20];
    char datatype[10];
    char sorttype[10];

    while(1) {
        int flag[4]={0,0,0,0};
        int option=0;
        memset(filename,0,20);
        memset(datatype, 0,10);
        memset(sorttype, 0, 10);
        printf("-------------------------------\n");
        printf("Options List: \n");
        printf("--0: exit from the menu\n");
        printf("--1: input the filename\n");
        printf("--2: input the sorting order\n");
        printf("--3: start to run the program\n");
        printf("-------------------------------\n");


        while(1)
        {
            printf("Choose the options by the corresponding number\n");
            printf(">>");
            scanf("%d",&option);
            if(option>=0&&option<5)
                flag[option]=1;
            else{
                printf("Wrong options\n");
                continue;
            }
            switch(option)
            {
                case 0:
                    break;
                case 1:
                    printf("Input the filename: rand_int.txt rand_char.txt rand_double \n");
                    memset(filename, 0, 10);
                    printf(">>");
                    scanf("%s",filename);
                    break;
                case 2:
                    printf("Input the sorting order: dec inc rand\n");
                    memset(sorttype,0,10);
                    printf(">>");
                    scanf("%s", sorttype);
                    break;
                case 3:
                    flag[3]=0;
                    if(flag[1]&&flag[2]) {
                        printf("start to run\n");
                        flag[3]=1;
                    }
                    else
                        printf("Lack of filename or sorting order\n");
                    break;
                default:
                    break;
            }
            if(flag[0]==1)
                break;
            if(flag[3]==1)
                break;
        }
        if(flag[0]==1)
            break;
        struct list *l1 = ReadFile(filename);
        char type[10];
        char order[10];
        char remain[10];
        memset(order, 0, 10);
        memset(type, 0, 10);
        memset(remain, 0,10);
        sscanf(filename, "%[^_]_%[^.]%[*]", order, type);
        WriteFile(l1, type, sorttype);
        Print(l1, type);
        Free(l1);
        printf("Run successfully\n");
    }
}
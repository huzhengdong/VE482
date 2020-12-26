#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h> 
#include <errno.h>
#define MAX_LINE 80
//static int print;
int execute(int argv, char** args, int order, int n);
void split(int *argv,char** args, char* copy);
int input(int* argv, int* n, char*** Args, int* IsBackground);
void Execute(int j,int n,int* i,char*** Args);



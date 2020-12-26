#include "shell.h"
#include <fcntl.h>

int execute(int argv, char **args, int order, int n)
{
	char * Args[argv+1];
	int j=0;
	int out_fd, in_fd;
	int double_input=0;
	int double_output=0;
	//int errnm;
	for(int i=0; i<argv; i++)
	{
		if(strcmp(args[i],">")==0)
		{
			close(1);
			
			if(double_output==1||(order<n&&n>0))
			{
				fprintf(stderr, "error: duplicated output redirection\n");
				fflush(stdout);
				return -1; 
			}
			out_fd=open(args[i+1], O_WRONLY|O_CREAT|O_TRUNC,0666);
			double_output=1;
			if(out_fd==-1)
			{
				fprintf(stderr, "%s: Permission denied\n",args[i+1]);
				fflush(stdout);
				return -1; 
			}
			i=i+1;
			dup2(out_fd, 1);
		}
		else if(strcmp(args[i], ">>")==0)
		{
			close(1);
			if(double_output==1||(order<n&&n>0))
			{
				fprintf(stderr, "error: duplicated output redirection\n");
				fflush(stdout);
				return -1; 
			}
			out_fd=open(args[i+1], O_WRONLY|O_CREAT|O_APPEND,0666);
			double_output=1;
			if(out_fd==-1)
			{
				fprintf(stderr, "%s: Permission denied\n",args[i+1]);
				fflush(stdout);
				return -1; 
			}
			i=i+1;
			dup2(out_fd, 1);
		}
		else if(strcmp(args[i], "<")==0)
		{
			/*if(double_input==1||order>0)
			{
				fprintf(stderr, "error: duplicated input redirection\n");
				fflush(stdout);
				return -1; 
			}*/
			close(0);
			in_fd=open(args[i+1], O_RDONLY);
			double_input=1;
			if(in_fd==-1)
			{
				fprintf(stderr,"%s: %s\n",args[i+1],strerror(errno));
				fflush(stdout);
				return -1; 
			}
			i=i+1;
			dup2(in_fd, 0);
		}
		else
		{
			Args[j]=args[i];
			j++;
		}
	} 
	Args[j]=NULL;
	if((strcmp(Args[0], "cd")==0&&argv==2)||(strcmp(Args[0], "jobs")==0&&argv==1))
		return 0;
	if(execvp(Args[0], Args)<0)
	{
		fprintf(stderr,"%s: command not found\n", Args[0]);
		fflush(stdout);
	}
	return 0;
}


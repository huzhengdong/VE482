#include "shell.h"
void split(int *argv,char** args, char* copy)	
{
	int i=*argv;	
	char left[MAX_LINE];
	char right[MAX_LINE];
	memset(left,0,MAX_LINE);
	memset(right,0,MAX_LINE);
	memcpy(right,copy,strlen(copy));
	for(int j=0; j<strlen(copy); j++)
	{
		char remain[MAX_LINE];
		memset(left,0,MAX_LINE);
		memset(remain,0,MAX_LINE);
		memcpy(remain, right, MAX_LINE);
		memset(right,0,MAX_LINE);

		if(copy[j]=='>')
		{
			int flag=1;
			if(j+1<strlen(copy)&&copy[j+1]=='>')
			{
				if (j==0)
					sscanf(remain,">>%s", right);
				else
					sscanf(remain,"%[^>>]>>%s", left,right);
				j++;
				flag=0;
			}
			else
			{	
				if (j==0)
						sscanf(remain,">%s", right);
				else
						sscanf(remain,"%[^>]>%s", left,right);
			}
			if(strlen(left)!=0)
			{
				args[i]=malloc(MAX_LINE*sizeof(char));
				memset(args[i],0,MAX_LINE);
				memcpy(args[i],left,MAX_LINE);
				i++;			
			}
			if(flag==1)
			{
				args[i]=malloc(MAX_LINE*sizeof(char));
				memset(args[i],0,MAX_LINE);
				sprintf(args[i],">");
				i++;		
			}
			else
			{
				args[i]=malloc(MAX_LINE*sizeof(char));
				memset(args[i],0,MAX_LINE);
				sprintf(args[i],">>");
				i++;		
			}
	
		}
		else if(copy[j]=='<')
		{		
			if (j==0)
				sscanf(remain,"<%s", right);
			else
				sscanf(remain,"%[^<]<%s", left,right);
			if(strlen(left)!=0)
			{
				args[i]=malloc(MAX_LINE*sizeof(char));
				memset(args[i],0,MAX_LINE);
				memcpy(args[i],left,MAX_LINE);
				i++;		
			}
			args[i]=malloc(MAX_LINE*sizeof(char));
			memset(args[i],0,MAX_LINE);
			sprintf(args[i],"<");
			i++;	
		}
		else
		{
			memcpy(right, remain, MAX_LINE);
			continue;
		}
	}
	if(strlen(right)!=0)
	{
		args[i]=malloc(MAX_LINE*sizeof(char));
		memset(args[i],0,MAX_LINE);
		memcpy(args[i],right,MAX_LINE);
		i++;
	}
	*argv=i;//
}

#include "shell.h"
static int print;

void inputHandler(int dummy) {
	if(print==0)
		printf("\n");
	else
		printf("\nmumsh $ ");
	fflush(stdout);
}

int input(int* argv, int* n, char*** Args, int* IsBackground)
{
	print=1;
	printf("mumsh $ ");
	fflush(stdout);
	signal(SIGINT, inputHandler);
	

	int i=0; 
	char copy[MAX_LINE];
	memset(copy,0,MAX_LINE);
	while(1)
	{
		copy[0]=getchar();
		if(copy[0]==EOF)
			return 1;
		if(copy[0]==' ')
			continue;
		else if(copy[0]=='\n')
			printf("mumsh $ ");
		else 
			break;
	}	
	
	int l=1;
	int single_quote=0;
	int double_quote=0;
	int NOT_NULL=1;
	int SPLIT=1;//check whther the char requires splitting
	int WAIT_IO=0;//check the uncompleted < or >
	int WAIT_Program=0;
	int FETCH=0;//check whether the char is fetched or not
	int USED=0;
	int Redirect_input=0;
	//int Redirect_output=0;

	Args[i]=(char**)malloc(sizeof(char*)*MAX_LINE);
	for(int j=0; j<MAX_LINE;j++)
		Args[i][j]=NULL;
	
	if(copy[0]=='\'')
	{
		single_quote=1;
		l=0;
	}
	else if(copy[0]=='\"')
	{
		double_quote=1;
		l=0;
	}
	else if(copy[0]=='>'||copy[0]=='<')
	{
		WAIT_IO=1;
		NOT_NULL=1;
	}
	else{}
	
	while(1)
	{
		if(!FETCH)
			copy[l]=getchar();
		else
			FETCH=0;
		if(USED)
		{
			USED=0;
			continue;
		}
		if(copy[l]==' ')
		{

			if(single_quote==1||double_quote==1)
				l++;
			else if(NOT_NULL==1)
			{
				copy[l]='\0';
				if(SPLIT==0)
				{
					Args[i][argv[i]]=malloc(MAX_LINE*sizeof(char));
					memset(Args[i][argv[i]],0,MAX_LINE);
					memcpy(Args[i][argv[i]],copy,MAX_LINE);
					argv[i]++;			
					SPLIT=1;
				}
				else
					split(&(argv[i]),Args[i],copy);
				memset(copy,0,MAX_LINE);
				NOT_NULL=0;
				l=0;
			}
			else
			{
				continue;
			}
		}
		else if(copy[l]=='\'')
		{
			WAIT_IO=0;
			WAIT_Program=0;
			if(single_quote==1)
			{
				single_quote=0;
				SPLIT=0;
			}
			else if(double_quote==1)
			{
				l++;
			}
			else
			{
				single_quote=1;
			}
		}
		else if(copy[l]=='\"')
		{
			WAIT_IO=0;
			WAIT_Program=0;
			if(single_quote==1)
			{
				l++;
			}
			else if(double_quote==1)
			{
				double_quote=0;
				SPLIT=0;
			}
			else
			{
				double_quote=1;
			}
		}
		else if(copy[l]=='\n')
		{
			if(single_quote==1|| double_quote==1)
			{
				printf("> ");
				l++;
				fflush(stdout);
			}
			else if(WAIT_IO==1||WAIT_Program==1)
			{
				printf("> ");
				fflush(stdout);
			}
			else
			{
				copy[l]='\0';
				if(NOT_NULL)
				{
					if(l>0&&copy[l-1]=='&')
					{
						*IsBackground=1;
						copy[l-1]='\0';
					}
					if(SPLIT==0)
					{
						Args[i][argv[i]]=malloc(MAX_LINE*sizeof(char));
						memset(Args[i][argv[i]],0,MAX_LINE);
						memcpy(Args[i][argv[i]],copy,MAX_LINE);
						argv[i]++;			
						SPLIT=1;
					}
					else
						split(&(argv[i]),Args[i],copy); 
				}
				break;
			}	
		}
		else if(copy[l]=='|'&&single_quote==0&&double_quote==0)
		{
			Redirect_input=1;
			if(WAIT_Program==1)
			{
				fprintf(stderr,"error: missing program\n");
				*n=i;
				while (getchar() != '\n');
				return -1;
			}
			if(WAIT_IO==1)
			{
				fprintf(stderr,"syntax error near unexpected token `|'\n");
				*n=i;
				while (getchar() != '\n');
				return -1;
			}
			
			copy[l]='\0';
			if(NOT_NULL)
			{
				if(SPLIT==0)
				{
					Args[i][argv[i]]=malloc(MAX_LINE*sizeof(char));
					memset(Args[i][argv[i]],0,MAX_LINE);
					memcpy(Args[i][argv[i]],copy,MAX_LINE);
					argv[i]++;			
					SPLIT=1;
				}
				else
					split(&(argv[i]),Args[i],copy); 
			}

			Args[i][argv[i]]=NULL;//The command list end up with a 
			i++;
			Args[i]=(char**)malloc(sizeof(char*)*MAX_LINE);//allocate the memory for the ith command list
			for(int j=0; j<MAX_LINE;j++)
				Args[i][j]=NULL;
			argv[i]=0;	
			memset(copy,0,MAX_LINE);
			NOT_NULL=0;
			l=0;
			WAIT_Program=1;
		}
		else if((copy[l]=='<'||copy[l]=='>')&&single_quote==0&&double_quote==0)
		{
			if(copy[l]=='<'&&Redirect_input==1)
			{
				fprintf(stderr, "error: duplicated input redirection\n");
				*n=i;
				while (getchar() != '\n');
				return -1;
			}
			if(copy[l]=='<')
				Redirect_input=1;
			if(WAIT_IO==1)
			{
				fprintf(stderr,"syntax error near unexpected token `%c'\n",copy[l]);
				*n=i;
				while (getchar() != '\n');
				return -1;
			}
			char c=copy[l];
			char d=getchar();
			FETCH=1;
			int append=0;
			copy[l]='\0';

			if(c=='>'&&d=='>')
				append=1;
			
			if(NOT_NULL)
			{
				if(SPLIT==0)
				{
					Args[i][argv[i]]=malloc(MAX_LINE*sizeof(char));
					memset(Args[i][argv[i]],0,MAX_LINE);
					memcpy(Args[i][argv[i]],copy,MAX_LINE);
					argv[i]++;			
					SPLIT=1;
				}
				else
					split(&(argv[i]),Args[i],copy); 
			}

			memset(copy,0,MAX_LINE);
			copy[0]=c;
			if(append)
			{
				copy[1]='>';
				USED=1;
			}

			Args[i][argv[i]]=malloc(MAX_LINE*sizeof(char));
			memset(Args[i][argv[i]],0,MAX_LINE);
			memcpy(Args[i][argv[i]],copy,MAX_LINE);
			argv[i]++;
			
			memset(copy,0,MAX_LINE);
			if(!USED)
				copy[0]=d;

			l=0;
			SPLIT=1;
			NOT_NULL=0;
			WAIT_IO=1;
			WAIT_Program=0;	
		}
		else
		{
			l++;
			WAIT_IO=0;
			WAIT_Program=0;
			NOT_NULL=1;
		}
	}
	*n=i;
	print=0;
	return 0;
}

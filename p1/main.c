#include "shell.h"
struct ProcessTable
{
	int ID;
	int Num;
	char name[MAX_LINE];
	pid_t pid[MAX_LINE];
	int status;
};

int main()
{
	char *** Args=(char ***)malloc(sizeof(char**)*MAX_LINE);
	for(int j=0; j<MAX_LINE; j++)
		Args[j]=NULL;
	struct ProcessTable jobs[MAX_LINE];
	for(int j=0;j<MAX_LINE;j++)
	{
		jobs[j].ID=0;
		jobs[j].status=0;
		jobs[j].Num=0;
		memset(jobs[j].name,0,MAX_LINE);
	}



	int flag=1;
	int n=0;          
	int i[MAX_LINE]; 
	for(int ii=0;ii<MAX_LINE; ii++)
			i[ii]=0;
	int ID=1;
	


	while(flag)
	{
	   
		int hstINOUT[2];
		hstINOUT[0]=dup(STDIN_FILENO);
        hstINOUT[1]=dup(STDOUT_FILENO);
		

	

		
		/*for(int j=0;j<n+1;j++)
		{
			for(int l=0;l<i[j];l++)
			{
				if(Args[j][l]!=NULL)
				{
					free(Args[j][l]);
					Args[j][l]=NULL;
				}
			}	
		}*/
		
		
		
		for(int ii=0;ii<MAX_LINE; ii++)
			i[ii]=0;
		n=0;


		int IsBackground=0;
		int input_status=input(i,&n,Args,&IsBackground);
		if(input_status==1)
		{
			printf("exit\n");
			break; 
		}
		else if(input_status==-1)
		{
			for(int j=0;j<n+1;j++)
			{
				for(int l=0;l<i[j];l++)
				{
					if(Args[j][l]!=NULL)
					{
						free(Args[j][l]);
						Args[j][l]=NULL;
					}
				}	
			}
			for(int j=0;j<MAX_LINE;j++)
			{
				if(Args[j]!=NULL)
				{
					free(Args[j]);
					Args[j]=NULL;
				}
			}
			continue;
		}
		else
		{}
		//printf("The IsBackground is %d\n", IsBackground);
		/*for(int j=0;j<n+1;j++)
		{
			for(int l=0;l<i[j];l++)
			{
				if(Args[j][l]!=NULL)
				{
					printf("%s\n",Args[j][l]);
				}
			}	
		}*/
		
		pid_t pid[MAX_LINE];;
		int j=0;
		
		
		int pd[MAX_LINE][2];
		for(j=0;j<=n-1;j++)
			pipe(pd[j]);


		for(j=0; j<=n;j++)
		{	
			
			pid[j]=fork();
			if(pid[j]==-1)
				printf("Fork error happened\n");
			if(pid[j]==0)
			{
				signal(SIGINT, SIG_DFL);
				if(IsBackground)
				{
					 setpgrp();  
					 setsid();  
				}
				for(int i=0;i<=n;i++)
				{
					if(i!=j&&i!=j-1)
					{
						close(pd[i][0]);
						close(pd[i][1]);
					}
				}
				
				if(j!=0)
				{
					close(pd[j-1][1]);
					dup2(pd[j-1][0],0); //read from previous
					close(pd[j-1][0]);
				}
				if(j!=n) //the last command
				{	
					close(pd[j][0]);
					dup2(pd[j][1],1);//write to the input
					close(pd[j][1]);
				}
				if(strcmp(Args[j][0], "exit")!=0)
				{
					execute(i[j],Args[j], j, n);			
				}
				if(strcmp(Args[j][0], "exit")!=0)
				{
					char cwd[1024];
					memset(cwd,0,1024);
					if(strcmp(Args[j][0], "pwd")==0&&i[j]==1)
					{
						getcwd(cwd, sizeof(cwd));
    					printf("%s\n", cwd);		
					}
					else{}	
				}
				_exit(1);	
			}
		}

		for(j=0;j<=n-1;j++)
		{
			close(pd[j][0]);
			close(pd[j][1]);
		}


		dup2(hstINOUT[0],STDIN_FILENO);
		dup2(hstINOUT[1],STDOUT_FILENO);


			if(IsBackground)
			{
				jobs[ID-1].ID=ID;
				jobs[ID-1].Num=n;
				for(int j=0;j<=n;j++)
				{
					jobs[ID-1].pid[j]=pid[j];
				}
				
				//sprintf(jobs[ID-1].name, "[%d] ",ID);
				for(int j=0;j<n+1;j++)
				{
					for(int l=0;l<i[j];l++)
					{
						if(Args[j][l]!=NULL)
						{
							sprintf(jobs[ID-1].name,"%s%s ", jobs[ID-1].name, Args[j][l]);
							//if(j!=n||l!=i[j]-1)
								//sprintf(jobs[ID-1].name, "%s ", jobs[ID-1].name);
						}
					}	
				if(j!=n)
					sprintf(jobs[ID-1].name, "%s| ", jobs[ID-1].name);
				else
					sprintf(jobs[ID-1].name, "%s&", jobs[ID-1].name);
				}
				printf("[%d] %s\n", ID, jobs[ID-1].name);
				ID++;
			}

			for(j=0;j<n+1;j++)
			{
				int status=-1;
				if(!IsBackground)
					waitpid(pid[j],&status,0);
				if(Args[j][0]!=NULL&&strcmp(Args[j][0], "exit")==0)
				{
					flag=0;
				}
				else if(strcmp(Args[j][0], "cd")==0&&i[j]==2&&flag==1)
				{
					if(chdir(Args[j][1])<0)
					{
						fprintf(stderr,"%s: %s\n",Args[j][1],strerror(errno));
					}
				}
				else if(strcmp(Args[j][0], "jobs")==0&&i[j]==1&&flag==1)
				{
					for(int j=0;j<ID-1;j++)
					{
						jobs[j].status=1;
						for(int jj=0;jj<=jobs[j].Num;jj++)
						{
							if(waitpid(jobs[j].pid[jj],NULL,WNOHANG) == 0)
								jobs[j].status=0;
						}
					}
					for(int j=0; j<ID-1;j++)
					{
						printf("[%d] ", jobs[j].ID);
						if(jobs[j].status==0)
							printf("running ");
						else
							printf("done ");
						printf("%s\n",jobs[j].name);
					}
				}		
			}
			if(flag==0)
				printf("exit\n");
			
            fflush(stdout);
		
		for(int j=0;j<n+1;j++)
		{
			for(int l=0;l<i[j];l++)
			{
				if(Args[j][l]!=NULL)
				{
					free(Args[j][l]);
					Args[j][l]=NULL;
				}
			}	
		}
		for(int j=0;j<MAX_LINE;j++)
		{
			if(Args[j]!=NULL)
			{
				free(Args[j]);
				Args[j]=NULL;
			}
		}

	}
	free(Args);
	return 0;
}

	



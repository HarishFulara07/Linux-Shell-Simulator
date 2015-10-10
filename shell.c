/*
	NOTE: PLEASE REFER TO readme.txt TO EXECUTE THE FOLLOWING SHELL PROGRAM
*/



/*
*
* Developer's Name : Harish Fulara
* Roll Number      : 2014143
* Description      : This C program implements a basic Linux shell
*
* All Rights Reserved :D
*
*/

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include "parse.h"
#include <signal.h>

/* 
* cur_PWD stores current working directory
* newcmdLine is used when history !x command is invoked
* new_flag maintains execution when history !x command is invoked
*/

static char cur_PWD[100] = "";
static char newcmdLine[1000];
static int new_flag = 0;

/*buffer stores history upto last 100 commands*/
static char * buffer[100];
static int buf_count = 0;
static int buf_flag = 0;

static char * cmdLine;

/*Executes reset screen command*/
void reset()
{
	int new_child = fork();
	
	if(new_child == 0)
	{
		char * myarg[2];
		myarg[0] = "/usr/bin/reset";
		myarg[1] = NULL;
		execv(myarg[0],myarg);
	}
	else
	{
		wait(NULL);
	}
	return;
}

/*Executes clear screen command*/
void clear()
{
	int new_child = fork();
	
	if(new_child == 0)
	{
		DIR * dir = opendir(cur_PWD);
		char * myarg[2];
		myarg[0] = "/usr/bin/clear";
		myarg[1] = NULL;
		execv(myarg[0],myarg);
	}
	else
	{
		wait(NULL);
	}
	return;
}

/*Executes ls command when invoked by the user*/
void LS(int type, int flag, char * path)
{
	DIR * dp = NULL;
	struct dirent * sd;
	dp=opendir(cur_PWD);
	if(flag == 0)
	{
		if(type == 1)
		{
			while((sd=readdir(dp))!=NULL)
			{
				if(sd->d_name[0] != '.')
				{
					fprintf(stdout,"%s\n",sd->d_name);
				}
			}
		}
		else if(type == 2)
		{
			while((sd=readdir(dp))!=NULL)
			{
				fprintf(stdout,"%s\n",sd->d_name);
			}
		}
	}
	else
	{
		FILE * fp = fopen(path,"w");
		if(type == 1)
		{
			while((sd=readdir(dp))!=NULL)
			{
				if(sd->d_name[0] != '.')
				{
					fprintf(fp,"%s\n",sd->d_name);
				}
			}
		}
		else if(type == 2)
		{
			while((sd=readdir(dp))!=NULL)
			{
				fprintf(fp,"%s\n",sd->d_name);
			}
		}
		fclose(fp);
	}
	closedir(dp);
}

/* Executes ll command when invoked by the user*/
void LL(int type, int flag, char * p)
{
	DIR * dp = NULL;
	FILE * fp;
	char whoami[20], * mod;
	struct dirent * sd;
	char path[100] , tmp[100], format[200];
	
	fp = fopen("userName","r");
	fscanf(fp,"%s\n",whoami);
	fclose(fp);
	
	if(flag == 1)
	{
		fp = fopen(p,"w");
	}
	
	strcpy(path,cur_PWD);
	strcat(path,"/");
	strcpy(tmp,path);
	dp=opendir(cur_PWD);
	
	while((sd=readdir(dp))!=NULL)
	{
		struct stat buf;
		strcpy(format,"");
		strcpy(path,tmp);
		if(sd->d_name[0] == '.' && type == 1)
		{
			continue;
		}
		strcat(path,sd->d_name);
		stat(path,&buf);
		if(S_ISDIR(buf.st_mode))
		{
			strcat(format,"d-");
		}
		else if(S_ISDIR(buf.st_mode))
		{
			strcat(format,"f-");
		}
		else
		{
			strcat(format,"--");
		}
		if((buf.st_mode & S_IRUSR) == S_IRUSR)
		{
			strcat(format,"r");
		}
		else
		{
			strcat(format,"-");
		}
		if((buf.st_mode &S_IWUSR) == S_IWUSR)
		{
			strcat(format,"w");
		}
		else
		{
			strcat(format,"-");
		}
		if((buf.st_mode &S_IXUSR) == S_IXUSR)
		{
			strcat(format,"x-");
		}
		else
		{
			strcat(format,"-");
		}
		if((buf.st_mode & S_IRGRP) == S_IRGRP)
		{
			strcat(format,"r");
		}
		else
		{
			strcat(format,"-");
		}
		if((buf.st_mode &S_IWGRP) == S_IWGRP)
		{
			strcat(format,"w");
		}
		else
		{
			strcat(format,"-");
		}
		if((buf.st_mode &S_IXGRP) == S_IXGRP)
		{
			strcat(format,"x-");
		}
		else
		{
			strcat(format,"-");
		}
		if((buf.st_mode & S_IROTH) == S_IROTH)
		{
			strcat(format,"r");
		}
		else
		{
			strcat(format,"-");
		}
		if((buf.st_mode &S_IWOTH) == S_IWOTH)
		{
			strcat(format,"w");
		}
		else
		{
			strcat(format,"-");
		}
		if((buf.st_mode &S_IXOTH) == S_IXOTH)
		{
			strcat(format,"x-");
		}
		else
		{
			strcat(format,"-");
		}
		
		if(flag == 1)
		{
			fprintf(fp,"%14s",format);
			fprintf(fp,"%5ld",buf.st_nlink);
			fprintf(fp,"  %s  ",whoami);
			fprintf(fp,"%8ld  ",buf.st_size);
			mod = ctime(&buf.st_mtime);
			mod[strlen(mod) - 1] = '\0';
			fprintf(fp,"%s  ",mod);
			fprintf(fp,"%s\n",path);
		}
		else
		{
			fprintf(stdout,"%14s",format);
			fprintf(stdout,"%5ld",buf.st_nlink);
			fprintf(stdout,"  %s  ",whoami);
			fprintf(stdout,"%8ld  ",buf.st_size);
			mod = ctime(&buf.st_mtime);
			mod[strlen(mod) - 1] = '\0';
			fprintf(stdout,"%s  ",mod);
			fprintf(stdout,"%s\n",path);
		}
	}
	if(flag == 1)
	{
		fclose(fp);
	}
	closedir(dp);
}

/*Retrieve user info: username, computer name & present working directory*/
void retrieveInfo()
{
	int new_child = fork();
	
	if(new_child < 0)
	{
		fprintf(stderr,"Shell Crashed\n");
		exit(0);
	}
	else if(new_child == 0)
	{
		char * myarg[2];
		myarg[0] = "basicInfoReader";
		myarg[1] = NULL;
		execv(myarg[0],myarg);
	}
	else
	{
		wait(NULL);
	}
	return;
}

/* Print user info: username, computer name & present working directory*/
void printInfo()
{
	FILE * fp1 = fopen("userName","r");
	FILE * fp2 = fopen("hostName","r");
	FILE * fp3 = fopen("pwd","w");
	
	char ch[100];
	
	fscanf(fp1,"%s\n",ch); 
	fprintf(stdout,"%s@",ch);
	fscanf(fp2,"%s\n",ch); 
	fprintf(stdout,"%s:",ch);
	fprintf(fp3,"%s\n",cur_PWD); 
	fprintf(stdout,"%s",cur_PWD);
	
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	
	return;
}

/* Opens appropriate directory when cd command is invoked*/

void open_directory(struct commandType * com)
{
	DIR * dp = NULL;
	int i = 0;
	char ch[100];
	char tmp[100];
	char PWD[100] = "";
	char last_PWD[100];
	int count = 0;
			
	if(com->VarNum == 1)
	{
		strcpy(cur_PWD,"");
		return;
	}
	if(strcmp(com->VarList[1],"..") == 0)
	{
		strcpy(ch,cur_PWD);
		i = 0;
		while(ch[i] != '\0')
		{
			if(ch[i] == '/')
			{
				count++;
			}
			i++;
		}
		if(count == 0 || count == 1)
		{
			strcpy(cur_PWD,"");
		}
		else
		{
			i = 0;
			while(ch[i] != '\0')
			{
				if(ch[i] == '/')
				{
					count--;
					if(count == 0)
					{
						tmp[i] = '\0';
						break;
					}
				}
				tmp[i] = ch[i];
				i++;
			}
			strcpy(cur_PWD,tmp);
		}
	}
	else if(strcmp(com->VarList[1],"~") == 0)
	{
		strcpy(cur_PWD,"");
	} 
	else
	{
		strcpy(last_PWD,cur_PWD); 
		for(i = 1; i < com->VarNum; i++)
		{
			strcat(PWD,"/");
			strcat(PWD,com->VarList[i]);	
		}
			
		strcat(last_PWD,PWD);
				
		if(NULL != (dp = opendir(PWD)))
		{		
			strcpy(cur_PWD,PWD);
		}
		else if(NULL != (dp = opendir(last_PWD)))
		{
			strcpy(cur_PWD,last_PWD);
		}
		else
		{
			fprintf(stderr, "Input directory: %s is invalid\n",PWD);
			fprintf(stderr, "Error: Unable to open the directory\n");
		}
	}	
}

/* Handles redirecting stdout(>) command*/

char * redirect_stdout(parseInfo * info)
{
	DIR * dp = NULL;
	char * path1, * path2, *tmp;
	path1 = (char*)malloc(100 * sizeof(char));
	path2 = (char*)malloc(100 * sizeof(char));
	tmp = (char*)malloc(100 * sizeof(char));
	int i = 0, count = 0, flag = 0;
	strcpy(path1,info->outFile);
	while(path1[i] != '\0')
	{
		if(path1[i] == '/')
		{
			count++;
			flag = 1;
		}
		i++;
	}
	i = 0;
	while(count > 0)
	{
		if(path1[i] == '/')
		{
			count--;
		}
		tmp[i] = path1[i];
		i++;
	}
	strcpy(path2,cur_PWD);
	strcat(path2,"/");
	strcat(path2,info->outFile);
	if(NULL != (dp = opendir(tmp)))
	{
		return(path1);
	}
	else if(flag == 1)
	{
		return(NULL);
	}
	else if(NULL != (dp = opendir(cur_PWD)))
	{
		return(path2);
	}
	else
	{
		return(NULL);
	}
}

/* Handles redirecting stdin(<) command*/

char * redirect_stdin(parseInfo * info)
{
	DIR * dp = NULL;
	char * path1, * path2, * tmp;
	path1 = (char*)malloc(100 * sizeof(char));
	path2 = (char*)malloc(100 * sizeof(char));
	tmp = (char*)malloc(100 * sizeof(char));
	int i = 0, count = 0, flag = 0;
	strcpy(path1,info->inFile);
	while(path1[i] != '\0')
	{
		if(path1[i] == '/')
		{
			count++;
			flag = 1;
		}
		i++;
	}
	i = 0;
	while(count > 0)
	{
		if(path1[i] == '/')
		{
			count--;
		}
		tmp[i] = path1[i];
		i++;
	}
	strcpy(path2,cur_PWD);
	strcat(path2,"/");
	strcat(path2,info->inFile);
	if(NULL != (dp = opendir(tmp)))
	{
		return(path1);
	}
	else if(flag == 1)
	{
		return(NULL);
	}
	else if(NULL != (dp = opendir(cur_PWD)))
	{
		return(path2);
	}
	else
	{
		return(NULL);
	}
}

char * buildPrompt()
{
	return ":) ";
}

/*Sorts the lines of a file when sort command is invoked */
void QuickSort(char ch[][1000], int f, int l)
{
	if(f < l)
	{
		int pivot = f;
		int i = f;
		int j = l;
		char tmp[1000];
		while(i < j)
		{
			while(strcmp(ch[i],ch[pivot]) <= 0 && i <= l)
			{
				if(i != l)
				{
					i++;
				}
				else
				{
					break;
				}
			}
			while(strcmp(ch[j],ch[pivot]) > 0)
			{
				j--;
			}
			if(i < j)
			{
				strcpy(tmp,ch[i]);
				strcpy(ch[i],ch[j]);
				strcpy(ch[j],tmp);
			}
		}
		strcpy(tmp,ch[j]);
		strcpy(ch[j],ch[pivot]);
		strcpy(ch[pivot],tmp);
		QuickSort(ch,f,j-1);
		QuickSort(ch,j+1,l);
	}
}

/*This function handles the action performed when CTRL + C is pressed*/
void sigintHandler(int sig_num)
{
	signal(SIGINT, sigintHandler);
	printf("\n\n<---- Cannot terminate using Ctrl+C ---->\n");
	printf("1. Use exit command to terminate, or\n");
	printf("2. Press enter to return\n\n");
	fflush(stdout);
}

void executeMyShellCommand(parseInfo * info, struct commandType * com)
{
	if (info == NULL)
	{
		return;
	}
    		
	//print_info(info);
		    		
	if ((com == NULL)  || (com->command == NULL))
	{
			return;
	}

	/*com->command tells the command name of com*/
	if (strcmp(com->command,"exit") == 0)	
	{
		exit(1);
	}
		
	if(strcmp(com->command,"cd") == 0)
	{
		open_directory(com);
	}
	else if(strcmp(com->command,"history") == 0)
	{
		FILE * fp;
		int flag = 0;
		char * path = NULL;
		if(info->boolOutfile == 1)
		{
			path = redirect_stdout(info);
			if(path == NULL)
			{
				fprintf(stderr, "Invalid Command: Path does not exist\n");
				return;
			}
			else
			{
				fp = fopen(path,"w");
				flag = 1;
			}
		}
		if(com->VarNum == 1)
		{
			int i = 0, max;
			if(buf_flag == 0)
			{
					max = buf_count;
			}
			else
			{
				max = 100;
			}
			for(i=0;i<max;i++)
			{
				if(flag == 1)
				{
					fprintf(fp,"%d. %s\n",i+1,buffer[i]);
				}
				else
				{
					fprintf(stdout,"%d %s\n",i+1,buffer[i]);
				}
			}
		}
		else if(com->VarNum == 2)
		{
			if(com->VarList[1][0] == '!' && com->VarList[1][1] != '-')
			{
				int l = strlen(com->VarList[1]);
				char ch;
				int i = 1, j = 0, num = 0;
				for(;i<l;i++)
				{
					ch = com->VarList[1][i];
					num = num * pow(10,j);
					num += (int)(ch) - 48;
					j++;
				}
				if(num > 100)
				{
					fprintf(stdout,"Invalid Command: !%d event not found\n",num);
					return;
				}
				else if(buf_flag == 0 && num > buf_count)
				{
					fprintf(stdout,"Invalid Command: !%d event not found\n",num);
					return;
				}
				else
				{
					int count = 0;
					strcpy(newcmdLine,buffer[num-1]);
					strcat(newcmdLine," ");
					int l = strlen(newcmdLine);
					for(i=0;i<strlen(cmdLine);i++)
					{
						if(cmdLine[i] == '!')
						{
							count = 1;
						}
						else if(cmdLine[i] == ' ' && count == 1)
						{
							i++;
							break;
						}
					}
					for(j=i;j<strlen(cmdLine);j++)
					{
						newcmdLine[l] = cmdLine[j];
						l++; 
					}
					new_flag = 1;
				}
			}
		}
		else
		{
			fprintf(stderr, "Invalid Command: Command does not exist\n");
		}
	}
	else if(strcmp(com->command,"kill") == 0)
	{
		if(strcmp(com->VarList[1],"-l") == 0)
		{
			if(info->boolOutfile == 1)
			{
				FILE * fp;
				char * path = redirect_stdout(info);
				if(path == NULL)
				{
					fprintf(stderr, "Invalid Command: Directory does not exist\n");
				}
				else
				{
					fp = fopen(path,"w");
					fprintf(    fp," 1) SIGHUP	 2) SIGINT	 3) SIGQUIT	 4) SIGILL	 5) SIGTRAP\n");
 					fprintf(    fp," 6) SIGABRT	 7) SIGBUS	 8) SIGFPE	 9) SIGKILL	10) SIGUSR1\n");
					fprintf(    fp," 11) SIGSEGV	12) SIGUSR2	13) SIGPIPE	14) SIGALRM	15) SIGTERM\n");
					fprintf(    fp," 16) SIGSTKFLT	17) SIGCHLD	18) SIGCONT	19) SIGSTOP	20) SIGTSTP\n");
					fprintf(    fp," 21) SIGTTIN	22) SIGTTOU	23) SIGURG	24) SIGXCPU	25) SIGXFSZ\n");
					fprintf(    fp," 26) SIGVTALRM	27) SIGPROF	28) SIGWINCH	29) SIGIO	30) SIGPWR\n");
					fprintf(    fp," 31) SIGSYS	34) SIGRTMIN	35) SIGRTMIN+1	36) SIGRTMIN+2	37) SIGRTMIN+3\n");	
					fprintf(    fp," 38) SIGRTMIN+4	39) SIGRTMIN+5	40) SIGRTMIN+6	41) SIGRTMIN+7	42) SIGRTMIN+8\n");	
					fprintf(    fp," 43) SIGRTMIN+9	44) SIGRTMIN+10	45) SIGRTMIN+11	46) SIGRTMIN+12	47) SIGRTMIN+13\n");
					fprintf(    fp," 48) SIGRTMIN+14 49) SIGRTMIN+15 50) SIGRTMAX-14 51) SIGRTMAX-1352) SIGRTMAX-12\n");	
					fprintf(    fp," 53) SIGRTMAX-11 54) SIGRTMAX-10 55) SIGRTMAX-9	56) SIGRTMAX-8	57) SIGRTMAX-7\n");	
					fprintf(    fp," 58) SIGRTMAX-6	59) SIGRTMAX-5	60) SIGRTMAX-4	61) SIGRTMAX-3	62) SIGRTMAX-2\n");
					fprintf(    fp," 63) SIGRTMAX-1	64) SIGRTMAX\n");
					fclose(fp);
				}
			}	
			else
			{
	
				fprintf(stdout," 1) SIGHUP	 2) SIGINT	 3) SIGQUIT	 4) SIGILL	 5) SIGTRAP\n");
 				fprintf(stdout," 6) SIGABRT	 7) SIGBUS	 8) SIGFPE	 9) SIGKILL	10) SIGUSR1\n");
				fprintf(stdout," 11) SIGSEGV	12) SIGUSR2	13) SIGPIPE	14) SIGALRM	15) SIGTERM\n");
				fprintf(stdout," 16) SIGSTKFLT	17) SIGCHLD	18) SIGCONT	19) SIGSTOP	20) SIGTSTP\n");
				fprintf(stdout," 21) SIGTTIN	22) SIGTTOU	23) SIGURG	24) SIGXCPU	25) SIGXFSZ\n");
				fprintf(stdout," 26) SIGVTALRM	27) SIGPROF	28) SIGWINCH	29) SIGIO	30) SIGPWR\n");
				fprintf(stdout," 31) SIGSYS	34) SIGRTMIN	35) SIGRTMIN+1	36) SIGRTMIN+2	37) SIGRTMIN+3\n");
				fprintf(stdout," 38) SIGRTMIN+4	39) SIGRTMIN+5	40) SIGRTMIN+6	41) SIGRTMIN+7	42) SIGRTMIN+8\n");
				fprintf(stdout," 43) SIGRTMIN+9	44) SIGRTMIN+10	45) SIGRTMIN+11	46) SIGRTMIN+12	47) SIGRTMIN+13\n");
				fprintf(stdout," 48) SIGRTMIN+14 49) SIGRTMIN+15 50) SIGRTMAX-14 51) SIGRTMAX-1352) SIGRTMAX-12\n");
				fprintf(stdout," 53) SIGRTMAX-11 54) SIGRTMAX-10 55) SIGRTMAX-9	56) SIGRTMAX-8	57) SIGRTMAX-7\n");
				fprintf(stdout," 58) SIGRTMAX-6	59) SIGRTMAX-5	60) SIGRTMAX-4	61) SIGRTMAX-3	62) SIGRTMAX-2\n");
				fprintf(stdout," 63) SIGRTMAX-1	64) SIGRTMAX\n");
			}
		}
		else if(strcmp(com->VarList[1],"-9") == 0 || strcmp(com->VarList[1],"SIGKILL") == 0)
		{
			kill(atoi(com->VarList[2]),SIGKILL);
		}
		else if(strcmp(com->VarList[1],"-15") == 0 || strcmp(com->VarList[1],"SIGTERM") == 0)
		{
			kill(atoi(com->VarList[2]),SIGTERM);
		}
		else
		{
			fprintf(stderr,"Invalid Command: Command not found\n");
		}
	}
	else if(strcmp(com->command,"pwd") == 0)
	{
		if(info->boolOutfile == 1)
		{
			FILE * fp;
			char * path = redirect_stdout(info);
			if(path == NULL)
			{
				fprintf(stderr, "Invalid Command: Directory does not exist\n");
			}
			else
			{
				fp = fopen(path,"w");
				fprintf(fp, "%s\n", cur_PWD);
				fclose(fp);
			}
		}	
		else
		{
			if(strcmp(cur_PWD,"")==0)
			{
				fprintf(stdout,"/\n");
			}
			else
			{
				fprintf(stdout, "%s\n", cur_PWD);
			}
		}
	}
	else if(strcmp(com->command,"whoami") == 0)
	{
		char ch[100];
		FILE * fp1, * fp2;
		if(info->boolOutfile == 1)
		{
				
			redirect_stdout(info);
			char * path = redirect_stdout(info);
			if(path == NULL)
			{
				fprintf(stderr, "Invalid Command: Path does not exist\n");
			}
			else
			{
				fp1 = fopen("userName","r");
				fp2 = fopen(path,"w");
				retrieveInfo();
				fscanf(fp1,"%s\n",ch);
				fprintf(fp2,"%s\n", ch);
				fclose(fp1);
				fclose(fp2);
			}	
		}
		else
		{
			fp1 = fopen("userName","r");
			retrieveInfo();
			fscanf(fp1,"%s\n",ch);
			fprintf(stdout,"%s\n", ch);
			fclose(fp1);
		}
	}
	else if(strcmp(com->command,"hostname") == 0)
	{
		char ch[100];
		FILE * fp1, * fp2;
		if(info->boolOutfile == 1)
		{	
			redirect_stdout(info);
			char * path = redirect_stdout(info);
			if(path == NULL)
			{
				fprintf(stderr, "Invalid Command: Path does not exist\n");
			}
			else
			{
				fp1 = fopen("hostName","r");
				fp2 = fopen(path,"w");
				retrieveInfo();
				fscanf(fp1,"%s\n",ch);
				fprintf(fp2,"%s\n", ch);
				fclose(fp1);
				fclose(fp2);
			}
		}
		else
		{
			fp1 = fopen("hostName","r");
			retrieveInfo();
			fscanf(fp1,"%s\n",ch);
			fprintf(stdout,"%s\n", ch);
			fclose(fp1);
		}
	}
	else if(strcmp(com->command,"clear") == 0)
	{
		clear();
	}
	else if(strcmp(com->command,"reset") == 0)
	{
		reset();
	}
	else if(strcmp(com->command,"ls") == 0)
	{
		int flag = 0;
		char * path = NULL;
		if(info->boolOutfile == 1)
		{
			path = redirect_stdout(info);
			if(path == NULL)
			{
				fprintf(stderr, "Invalid Command: Path does not exist\n");
				return;
			}
			else
			{
				flag = 1;
			}
		}
		if(strcmp(cur_PWD,"") == 0)
		{
			return;
		}
		else if(com->VarNum == 2 && strcmp(com->VarList[1],"-a") == 0)
		{
			LS(2,flag,path);
		}
		else if(com->VarNum == 2 && strcmp(com->VarList[1],"-l") == 0)
		{
			LL(1,flag,path);
		}
		else if(com->VarNum == 1)
		{
			LS(1,flag,path);
		}
		else
		{
			fprintf(stderr, "Invalid Command: Command Not Found\n");
		}
	}
	else if(strcmp(com->command,"ll") == 0)
	{
		int flag = 0;
		char * path = NULL;
		if(info->boolOutfile == 1)
		{
			path = redirect_stdout(info);
			if(path == NULL)
			{
				fprintf(stderr, "Invalid Command: Directory does not exist\n");
				return;
			}
			else
			{
				flag = 1;
			}
		}
		if(strcmp(cur_PWD,"") == 0)
		{
			return;
		}
		else if(com->VarNum == 1)
		{
			LL(2,flag,path);
		}
		else
		{
			fprintf(stderr, "Invalid Command: Command Not Found\n");
		}
	}
	else if(strcmp(com->command,"cat") == 0)
	{
		char ch;
		char file[100], * ipath = NULL;
		int fd, iflag = 0, oflag = 0;
		
		if(com->VarNum == 2)
		{
			strcpy(file,cur_PWD);
			strcat(file,"/");
			strcat(file,com->VarList[1]);
			fd = open(file,O_RDONLY);
			if(fd == -1)
			{	
				fprintf(stderr, "Invalid Command: File does not exist\n");
				return;
			}
			iflag = 1;	
		}
		else if(info->boolInfile == 1)
		{
			ipath = redirect_stdin(info);
			if(ipath == NULL)
			{	
				fprintf(stderr, "Invalid Command: Path does not exist\n");
				return;
			}
			fd = open(ipath,O_RDONLY);
			iflag = 1;
		}
		if(iflag == 1)
		{
			FILE * fp;
			char * opath = NULL;
			if(info->boolOutfile == 1)
			{
				opath = redirect_stdout(info);
				if(opath == NULL)
				{
					fprintf(stderr, "Invalid Command: Path does not exist\n");
					return;
				}
				else
				{
					oflag = 1;
				}
			}
				
			if(oflag == 1)
			{
				FILE * fp = fopen(opath,"w");
				fprintf(fp,"Opening File from path: %s\n\n",file);
				while(read(fd,&ch,1) > 0)
				{
					fprintf(fp,"%c",ch);
				}
				fclose(fp);
			}
			else
			{
				fprintf(stdout,"Opening File from path: %s\n\n",file);
				while(read(fd,&ch,1) > 0)
				{
					fprintf(stdout,"%c",ch);
				}
			}
		}
	}
	else if(strcmp(com->command,"wc") == 0)
	{
		char ch;
		char file[100], * ipath = NULL;
		int fd, iflag = 0, oflag = 0;
		long int new_line, word, byte, charcount;
			
		if(com->VarNum == 2)
		{
			strcpy(file,cur_PWD);
			strcat(file,"/");
			strcat(file,com->VarList[1]);
			fd = open(file,O_RDONLY);
			if(fd == -1)
			{	
				fprintf(stderr, "Invalid Command: File does not exist\n");
				return;
			}
			iflag = 1;	
		}
		else if(info->boolInfile == 1)
		{
			ipath = redirect_stdin(info);
			if(ipath == NULL)
			{	
				fprintf(stderr, "Invalid Command: File does not exist\n");
				return;
			}
			fd = open(ipath,O_RDONLY);
			iflag = 1;
		}
		if(iflag == 1)
		{
			FILE * fp;
			char * opath = NULL;
			if(info->boolOutfile == 1)
			{
				opath = redirect_stdout(info);
				if(opath == NULL)
				{
					fprintf(stderr, "Invalid Command: Path does not exist\n");
					return;
				}
				else
				{
					oflag = 1;
				}
			}
				
			new_line = word = byte = charcount = 0;
			while(read(fd,&ch,1) > 0)
			{
				if(ch == '\n')
				{
					new_line++;
					word++;
					charcount--;
				}
				if(ch == ' ')
				{
					word++;
					charcount--;
				}
				byte++;
				charcount++;
			}
				
			if(oflag == 1)
			{
				fp = fopen(opath,"w");
				fprintf(fp,"Newline count = %ld\n",new_line);
				fprintf(fp,"Word count    = %ld\n",word);
				fprintf(fp,"Character count = %ld\n",charcount);
				fprintf(fp,"Byte count = %ld\n",byte);
				fclose(fp);
			}
			else
			{
				fprintf(stdout,"Newline count = %ld\n",new_line);
				fprintf(stdout,"Word count    = %ld\n",word);
				fprintf(stdout,"Character count = %ld\n",charcount);
				fprintf(stdout,"Byte count = %ld\n",byte);	
			}
				
		}
		else
		{
			fprintf(stderr,"Invalid Command: Supply complete command with filename\n");
		}
	}
	else if(strcmp(com->command,"sort") == 0)
	{
		char ch;
		char file[100];
		int fd, iflag = 0, oflag = 0, new_line = 0, i = 0, j = 0;
		if(com->VarNum == 2)
		{
			strcpy(file,cur_PWD);
			strcat(file,"/");
			strcat(file,com->VarList[1]);
			fd = open(file,O_RDONLY);
			if(fd == -1)
			{	
				fprintf(stderr, "Invalid Command: File does not exist\n");
				return;
			}
			iflag = 1;
		}
		else if(info->boolInfile == 1)
		{
			char * ipath = redirect_stdin(info);
			if(ipath == NULL)
			{	
				fprintf(stderr, "Invalid Command: File does not exist\n");
				return;
			}
			fd = open(ipath,O_RDONLY);
			strcpy(file,ipath);
			iflag = 1;
		}
		if(iflag == 1)
		{
			FILE * fp;
			char * opath = NULL;
			if(info->boolOutfile == 1)
			{
				opath = redirect_stdout(info);
				if(opath == NULL)
				{
					fprintf(stderr, "Invalid Command: Path does not exist\n");
					return;
				}
				else
				{
					oflag = 1;
				}
			}
				
			while(read(fd,&ch,1) > 0)
			{
				if(ch == '\n')
				{
					new_line++;
				}
			}
			char ch1[new_line][1000];
			for(i=0;i<new_line;i++)
			{
				strcpy(ch1[i],"\0");
			}
			fd = open(file,O_RDONLY);
			while(read(fd,&ch,1) > 0)
			{
				if(ch == '\n')
				{
					ch1[j][i] = '\0';
					j++;
					i=0;
				}
				else
				{
					ch1[j][i] = ch;
					i++;
				}
			}
			int i;
			QuickSort(ch1,0,new_line-1);
				
			if(oflag == 1)
			{
				fp = fopen(opath,"w");
				for(i=0;i<new_line;i++)
				{
					fprintf(fp,"%s\n",ch1[i]);
				}
				fclose(fp);
			}
			else
			{
				for(i=0;i<new_line;i++)
				{
					fprintf(stdout,"%s\n",ch1[i]);
				}
			}
		}
		else
		{
			fprintf(stderr,"Invalid Command: Supply complete command with filename\n");
		}
	}
	else if(strcmp(com->command,"help") == 0)
	{
		FILE * fp = fopen("help.txt","r");
		char read;
		while(!feof(fp))
		{
			fscanf(fp,"%c",&read);
			fprintf(stdout,"%c",read);
		}
		fclose(fp);
	}
	else
	{
		fprintf(stderr, "Invalid Command: Command Not Found\n");
	}
}

int main (int argc, char * argv[])
{
	int loop;
	
	/*	
	* info stores all information returned by parser
	* com stores command name and arguments associated with the command
	*/
	parseInfo * info;
	struct commandType * com;

	printf("*********************************************************\n");
	printf("*                   WELCOME TO MY SHELL                 *\n");
	printf("*********************************************************\n");
	
	/*Retrieve info of the user*/
	retrieveInfo();
	
	while(1)
	{
	
		/* Print info of the user on STDOUT*/
		fprintf(stdout,"\n");
		printInfo();
		fprintf(stdout,"\n");
		
		if(new_flag == 1)
		{
			cmdLine = (char *)malloc(1000 * sizeof(char));
			strcpy(cmdLine,newcmdLine);
			strcpy(newcmdLine,"");
			new_flag = 0;
		}
		else
		{
			signal(SIGINT, sigintHandler);
			cmdLine = readline(buildPrompt());	
		}
		
		if (cmdLine == NULL) 
		{
			fprintf(stderr, "Unable to read command\n");
			return(0);
		}
		else
		{
			add_history(cmdLine);
			
			if(buf_count < 100 && buf_flag == 0)
			{
				buffer[buf_count] = (char*)malloc(100 * sizeof(char));
				strcpy(buffer[buf_count],cmdLine);
				buf_count++;
			}
			else if(buf_count < 100 && buf_flag == 1)
			{
				strcpy(buffer[buf_count],cmdLine);
				buf_count++;
			}
			else
			{
				buf_count = 0;
				buf_flag = 1;
				strcpy(buffer[buf_count],cmdLine);
				buf_count++;
			}
		}
		
		/* parse function parses the current command and extracts useful information out from it*/
		info = parse(cmdLine);
		
		if(info->pipeNum == 0)
		{
			com = &(info->CommArray[0]);
			executeMyShellCommand(info,com);
		}
		/*else
		{
			int p[2];
			pipe(p);
			
			for(loop = 0; loop <= info->pipeNum; loop++)
			{
				
				int new_child = fork();
				if(new_child == -1)
				{
					fprintf(stderr,"Shell crashed\n");
					getchar();
					exit(-1);
				}
				else if(new_child == 0)
				{
					close(p[0]);
					dup2(p[1],1);
					close(p[1]);
					
				}
				else
				{
					wait(NULL);
					char * myarg[2];
					myarg[0] = "/usr/bin/wc";
					myarg[1] = 
					myarg[1] = NULL;
					close(p[1]);
					dup2(p[0],1);
					close(p[0]);
					execv(myarg[0],myarg);
				}
			}
		}*/
		
		/*memory allocated to info and cmdLine is freed*/
		free_info(info);
		free(cmdLine);
	}
}

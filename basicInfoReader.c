/*
* Reads basic info such as username, hostname and current working directory.
* The read information is stored in three different files - "userName" , "hostName" and "pwd",
* using open and dup2.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main()
{
	int new_child = fork();
	if(new_child == 0)
	{
		char * myarg[2];
		myarg[0] = "/usr/bin/whoami";
		myarg[1] = NULL;
		int f = open("userName",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		dup2(f,1);
		close(f);
		execv(myarg[0],myarg);
	}
	else
	{
		wait(NULL);
		char * myarg[2];
		myarg[0] = "/bin/hostname";
		myarg[1] = NULL;
		int f = open("hostName",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		dup2(f,1);
		close(f);
		execv(myarg[0],myarg);
	}
	return(0);
}

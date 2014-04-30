/* A simple echo server using TCP */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <strings.h>
#include "stdafx.h"
#include <unistd.h>
#include <errno.h>
#include <dirent.h> 

#define SERVER_TCP_PORT 3000	/* well-known port */
#define BUFLEN		257	/* buffer length */

int service(int);
void reaper(int);

int main(int argc, char **argv)
{
	int 	sd, new_sd, client_len, port;
	struct	sockaddr_in server, client;

	switch(argc){
	case 1:
		port = SERVER_TCP_PORT;
		break;
	case 2:
		port = atoi(argv[1]);
		break;
	default:
		fprintf(stderr, "Usage: %d [port]\n", argv[0]);
		exit(1);
	}

	/* Create a stream socket	*/	
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Can't creat a socket\n");
		exit(1);
	}

	/* Bind an address to the socket	*/
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
		fprintf(stderr, "Can't bind name to socket\n");
		exit(1);
	}
int n;
char recv[BUFLEN], send[BUFLEN],file_name[BUFLEN],dbuf[252],empty[1];;

	/* queue up to 5 connect requests  */
	listen(sd, 5);

	(void) signal(SIGCHLD, reaper);

	while(1) {
	  client_len = sizeof(client);
	  new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
	  if(new_sd < 0){
	    fprintf(stderr, "Can't accept client \n");
	    exit(1);
	  }
	  switch (fork()){
	  case 0:		/* child */
		(void) close(sd);
		exit(service(new_sd));
	  default:		/* parent */
		(void) close(new_sd);
		break;
	  case -1:
		fprintf(stderr, "fork: error\n");
	  }
	}
}

/*	service program	*/
int service(int sd)
{
	while(1)
	{
		responder();
	}

	return(0);
}

int responder()  //determine what action to take
{
	if ((n=read(new_sd,recv, BUFLEN))) > 0 ) 
	{
		if (recv[0] == 'D' || recv[0] == 'd')
		{
			sendfile();
		}
		else if (recv[0] == 'U' || recv[0] == 'u')
		{
			uploadfile();
		}
		else if (recv[0] == 'P' || recv[0] == 'p')
		{
			changedir();
		}
		else if (recv[0] == 'L' || recv[0] == 'l')
		{	
			listdir();
		}

	}
	else  //read failed
	{
		senderror();
	}  	


	return ; 
}

void sendfile() //open filename read file then send file
{	
	FILE *f1;
	for(i;i<(n-4),i++) //getting file name
	{
		file_name[i] = recv[i+4];
	}
	f1 = fopen(file_name, "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s", file_name);
	}	
	else //READING FILE AND SENDING
	
	{
		while ((fread(dbuf,252,1,f1)) > 0)
		preparepfu(dbuf);
		sendpdu(sbuf);
	}
}

void uploadfile()
{
FILE *file;
char filename1[252],data1[252];
int i=0,i1=0,n1;
	//get file name
	for  (i;i<252;i++)
	{
		filename1[i] = rbuf[i+4]; 
	}
	file= fopen(filename,"w")
	if (!file)
	{
	senderror();
	printf("Erorr: File cannot be opened \n");
	exit(0);	
	}
	preparepdu('R',0,empty); //sending ready PDU
	sendpdu(sbuf);
	
	while((n1 = read(new_sd,recv,BUFLEN))> 0 )
	{
		 if (recv[0] == 'F')
		{
			for (i1;i1<252;i1++)
			{
				data1[i1] = recv[i1+4];
			}		
		fwrite(data1,1,sizeof(data1),file);		
		}
	}
}


void changedir()
{
int i =0,n1=0;
FILE *file;
char filename1[252],path[300];
	for (i;i<n1;i++)
	{
		filename1[i] = recv[i+4];	
	}
	path[0] = '/';
	strcat[path,filename1];
	if (chdir(path)<-1)
		printf("%s\n",errno)
	preparepdu('R',0,empty);
	sendpdu(sbuf);
}

void listdir()
{

/*
 * This program displays the names of all files in the current directory.
 */

#include <dirent.h> 
#include <stdio.h> 

int main(void)
{
  DIR           *d;
  struct dirent *dir;
  d = opendir(".");
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      printf("%s\n", dir->d_name);
    }

    closedir(d);
  }

  return(0);
}






	FiLE *file;
	char data1[252];
	int length;
	file = fopen ("listfiles.txt","r");
	length = fread(data1,sizeof(252),1,file);
	preparepdu('I',length,data1);		
}

void senderror()
{
	//send error pdu
	perror(errno);
}
void dump_buffer(char buffer[], int buffer_size)//Used for debugging purpose, prints contents of buffer
{
  int i=0;

  for(i;i < buffer_size;i++)
     printf("%c", buffer[i]);
}

int filesize(FILE *r) 
{
	int len;
	fseek(r,0,SEEK_END);
	len = ftell(r);
	return len;
}

void sendpdu(char pdu[]) //could be used for ready type
{
	while (true)
	{
		if ((n2 = write(sd,pdu,BUFLEN))<0)
		{				
			printf("Error writing to socket\n");
			exit(0);
		}
		else if (n2 == 0)
			printf("Nothing to write\n");				
			break;
			
	}
}



void preparepdu (char type, int length, char data[])
{
	char str[3];
	int i=0,i1=0, i2=0, i3=0,i4=0,i5=0,i6=0;
	sprintf(str, "%d",length);
	if (type =='F' || type == 'f')
	{
		sbuf[0] = type; //PDU for sending file
		for (i1;i1<length,i1++)
		{
			if (i1 < 3)
				sbuf[i2+1] = str[i2];
			else
				sbuf[i2 + 4] = data[i2]; 
		}
	}
	else if (type =='R' || type == 'r')
	{
		
		sbuf[0] = 'R'; //PDU to send ready status
		for (i2;i2<length;i2++)
		{
			if (i2 < 3)
				sbuf[i2+1] = str[i2];
			else
				sbuf[i2 + 4] = data[i2]; 	
		}
		
	}
	else if (type =='I' || type == 'i')
	{
		sbuf[0] = 'I'; //PDU to send list of files in current directory
		for (i3;i3<length,i3++)
		{
			if (i3 < 3)
				sbuf[i3+1] = str[i3];
			else
				sbuf[i3 + 4] = data[i3]; 
		}
	}
	else if (type =='E' || type == 'e') 
	{
		sbuf[0] = 'E'; //PDU to send error
		for (i4;i4<length,i4++)
		{
			if (i4 < 3)
				sbuf[i4+1] = str[i4];
			else
				sbuf[i4 + 4] = data[i4]; 
		}
	}
}

/*	reaper		*/
void	reaper(int sig)
{
	int	status;
	while(wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
}

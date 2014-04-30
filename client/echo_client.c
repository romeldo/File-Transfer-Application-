/* A simple echo client using TCP */
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>



#define SERVER_TCP_PORT 3000	/* well-known port */
#define BUFLEN		256	/* buffer length */

int main(int argc, char **argv)
{
	int 	n, i, bytes_to_read;
	int 	sd, port;
	struct	hostent		*hp;
	struct	sockaddr_in server;
	char	*host, *bp, rbuf[BUFLEN], sbuf[BUFLEN];

	switch(argc){
	case 2:
		host = argv[1];
		port = SERVER_TCP_PORT;
		break;
	case 3:
		host = argv[1];
		port = atoi(argv[2]);
		break;
	default:
		fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
		exit(1);
	}

	/* Create a stream socket	*/	
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Can't creat a socket\n");
		exit(1);
	}

	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (hp = gethostbyname(host)) 
	  bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	else if ( inet_aton(host, (struct in_addr *) &server.sin_addr) ){
	  fprintf(stderr, "Can't get server's address\n");
	  exit(1);
	}

	/* Connecting to the server */
	if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
	  fprintf(stderr, "Can't connect \n");
	  exit(1);
	}
	
	//***** ALL VARIABLES NEEDED HERE *****
	char action,str[3];
	char dir[BUFLEN],data[252];
	int i1=0,i2=0,i3=0,n1=0,n2=0,n3;
	
	//print action menu
	while(true)
	{
	printf("Enter letter for action to take\n");
	printf("'L' - Request list of files in directory\n");
	printf("'P' - Request change of directory on server\n");
	printf("'D' - Download file from server\n")
	printf("'U' - Upload file to server\n");
	printf("'E' - Exit\n");
	
	scanf("%c",&action);
	bzero(sbuf,BUFLEN);bzero(rbuf,BUFLEN);
	dispatcher(action);
	}
	//Separate functions for each possible action
	
	//Dispatcher function which decides which action to take based on user input
	void dispatcher(char a)
	{
		if (a =='L' || a == 'l')
		{
			filelist(); //print list of file on server
		}
		else if (a =='P' || a == 'p')
		{
			changedir();//change directory on server
		}
		else if (a =='D' || a == 'd')
		{
			download();//download file on server
		}
		else if (a =='U' || a == 'u') 
		{
			upload(); //upload file to server
		}
		else if (a =='E' || a == 'e')
		{
			terminate();//terminate connection
			printf("Exiting application\n");
			exit(0);
		}

	}	
	
	void filelist()//list of files on server
	{
	int i1=0,i2=0,i3=0,counter=1;
	char opt[250],temp[252];
		preparepdu('L',length,"");
		sendpdu(sbuf);
		while((n2 = read(sd,rbuf,BUFLEN))>0)
		{
			if (rbuf[0] == 'I' || rbuf[0] == 'i')
			{
				for (i3 ; i3 <(n2-4); i3++)
				{
					data[i3]=rbuf[i3+4]; 
				}
				for (i2; i2<(n2-4); i2++)
				{
					if (rbuf[i2]!= ';')
					{					
						temp[i2] = rbuf[i2];
					}
					else
						printf("Files: %d. %s\n",counter,temp); 
						counter++;
				}
			}	
	
		}

	}
	
	void changedir()//change directory
	{
	char data1[252];
	int len=0,n1 = 0;
		printf("Enter directory to change to\n");
		len = scanf("%s",&data1);
		printf("Preparing PDU\n");
		preparepdu('P',len,data1);
		sendpdu(sbuf);
		while ((n1 = read(sd,rbuf,BUFLEN)>0))
		{
			if (rbuf[0] == 'R')
			printf("Directory has been changed.\n");
		}
		
	}
	
	void download()//download file
	{
	int len=0,n1=0;
	char data1[252],file_name[252];
	
		printf("Specify the file you want to download\n");
		len = scanf("%s",&file_name);
		preparepdu('D',len,file_name);
		while ((n1 = read(sd,rbuf,BUFLEN))>0)
		{
			
		}
	
	
	}
	void upload() //upload file
	{
		
		
		
	}


	void terminate()//terminate connection
	{
		printf("Terminating connection\n");		
		close(sd);
	}

	void senderror()//might not be needed***********
	{
		
	}		
	
	void preparepdu (char type, int length, char data[])
{
	char str[3];
	int i=0,i1=0, i2=0, i3=0,i4=0,i5=0,i6=0;
	sprintf(str, "%d",length);
	if (type =='D' || type == 'd')
	{
		sbuf[0] = 'D'; //PDU to request file download
		for (i1;i1<length,i1++)
		{
			if (i1 < 3)
				sbuf[i2+1] = str[i2];
			else
				sbuf[i2 + 4] = data[i2]; 
		}
	}
	else if (type =='U' || type == 'u')
	{
		
		sbuf[0] = 'U'; //PDU to request file upload
		for (i2;i2<length;i2++)
		{
			if (i2 < 3)
				sbuf[i2+1] = str[i2];
			else
				sbuf[i2 + 4] = data[i2]; 	
		}
		
	}
	else if (type =='F' || type == 'f')
	{
		sbuf[0] = 'F'; //PDU for uploading file 
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
	else if (type =='P' || type == 'p') 
	{
		sbuf[0] = 'P'; //PDU to send error
		for (i5;i5<length,i5++)
		{
			if (i5 < 3)
				sbuf[i5+1] = str[i5];
			else
				sbuf[i5 + 4] = data[i5]; 
		}
	}
	else if (type =='L' || type == 'l') 
	{
		sbuf[0] = 'L'; //PDU to send error
		for (i6;i6<length,i6++)
		{
			if (i6 < 3)
				sbuf[i6+1] = str[i6];
			else
				sbuf[i6 + 4] = data[i6]; 
		}
	}
}

	
	void sendpdu(char pdu[])
	{
		while (true)
		{
			if (n2 == write(sd,pdu,BUFLEN)<0)
			{				
				printf("Error writing to socket\n");
				exit(0);
			}
			else if (n2 == 0)
				break;
				
		}
		
				
	}

	void receivepdu(char pdu[])
	{
		read(sd,pdu,BUFLEN); 
	}



		
	printf("Transmit: \n");
	while(n=read(0, sbuf, BUFLEN)){	/* get user message */
	  write(sd, sbuf, n);		/* send it out */
	  printf("Receive: \n");
	  bp = rbuf;
	  bytes_to_read = n;
	  while ((i = read(sd, bp, bytes_to_read)) > 0){
		bp += i;
		bytes_to_read -=i;
	  }
	  write(1, rbuf, n);
	  printf("Transmit: \n");
	}

	close(sd);
	return(0);
}

//SERVER.C
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h> 
#include <string.h>
#include <ctype.h>

#define SERVER_TCP_PORT 3000	/* well-known port */
#define BUFLEN		150	/* buffer length */

int service(int);
void reaper(int);
void preparepdu (char, int, char [],char []);
void sendpdu(int, char []);
int returnlength(char []);
void responder(int);
void upload (int, char []);
void download(int,char []);
void getonlydata(char [],char []);
void changedirectory(int,char []);
void listfiles(int, char []);

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
	while (1){
	responder(sd);
	}
	return(0);
}

void responder (int sd)
{
	int n1=0,n2=0,n3=0,n4=0;
	int i1=0,i2=0,i3=0,i4=0;
	int len_of_data=0,len1=0,len2=0,len3=0;
	char rbuf[BUFLEN];
	char file_name[146],dir_path[146],dir_path1[146];
	bzero(rbuf,sizeof(rbuf));bzero(file_name,sizeof(file_name)); //clearing buffers
	n1 = recv(sd, rbuf, BUFLEN, 0);//read client request
	printf("what did i receive: %s\n",rbuf);
	printf("what is n1: %d\n",n1);	
	if (n1<0){
		printf("error reading from socket\n");
		exit(0);
	}
	bzero(file_name,sizeof(file_name));	
	if(rbuf[0]== 'D'){ //client wants to download
		len_of_data = returnlength(rbuf);
		printf("len_of_data : %d\n",len_of_data);
		for (i1;i1<len_of_data;i1++){
			file_name[i1] = rbuf[i1+4];
		}
		printf("file name : %s\n",file_name);
		printf("strlen(file_name): %d\n",strlen(file_name));
		download(sd,file_name);		
	}
		
	else if(rbuf[0] == 'U'){ //Client wants to upload
		len1=returnlength(rbuf);
		for (i2;i2<len1;i2++)
			file_name[i2] = rbuf[i2+4];
		printf("Uploading file name: %s\n",file_name);
		upload(sd,file_name);
	}
	else if (rbuf[0] == 'P'){ //client wants to change directory
		len2 = returnlength(rbuf);
		printf("what is len2: %d\n",len2);
		bzero(dir_path,sizeof(dir_path));
		for (i3;i3<len2;i3++){
			dir_path[i3] = rbuf[i3+4];
		}
	
	printf("Client specify cd to: %s \n", dir_path);
	changedirectory(sd,dir_path);
	}
	else if (rbuf[0] == 'L'){//client wants to list files in directory
		len3=returnlength(rbuf);
		for (i4;i4<len3;i4++){
			dir_path1[i4] = rbuf[i4+4];
		}
		listfiles(sd,dir_path1);	
	}	
}

void download(int sd, char file_name[])
{
	FILE *fp1;
	int len1=0,bytes_read=0;
	char dbuf1[146],sbuf[BUFLEN];
	char errormsg[146] = "File doesn't exist";
	int len2=strlen(errormsg);
	if ((fp1 = fopen (file_name, "r")) == NULL){
		preparepdu('E',len2,errormsg,sbuf);
		sendpdu(sd,sbuf);
	}
		
		
	else{
		bzero(sbuf,BUFLEN);
		while((bytes_read = fread(dbuf1,sizeof(char),146,fp1))>0){
			printf("sending %d bytes to client\n",bytes_read);
			preparepdu('F',bytes_read,dbuf1,sbuf);
			sendpdu(sd,sbuf);
		}
		printf("closing file...\n");
		fclose(fp1);
	}
}



void upload (int sd, char file_name[])
{
	FILE *fp;
	char error1[146] = "File doesn't exist";
	char empty1[1],empty2[1];
	char sbuf[BUFLEN],rbuf[BUFLEN],data[146];
	int bytes_received=0,len_of_data=0,write_size = 0;
	int len1 =0;	

	if ((fp = fopen (file_name,"w"))== NULL){
		len1 = strlen(error1);
		preparepdu('E',len1,error1,sbuf);//prepare and send error pdu
		sendpdu(sd,sbuf);
	}
	else{	
		preparepdu('R',0,empty2,sbuf); //prepare and send READY PDU
		sendpdu(sd, sbuf);
		while ((bytes_received = recv(sd, rbuf, BUFLEN, 0))> 0)
		{
			if (rbuf[0]=='F'){
				getonlydata(rbuf,data);
				len_of_data = returnlength(rbuf);
				write_size = fwrite(data, sizeof(char),len_of_data, fp);
			}
			

			if (len_of_data < 146)
					break;


		}
			
		fclose(fp);
		printf("closing file\n");
	 } 	
	
}	

void changedirectory(int sd,char path[])
{
	int n1=0,n2=0,n3=0,n4=0;
	int i1=0,i2=0,i3=0;
	int len_of_data=0,len1=0,lenoferror=0;
	char rbuf[BUFLEN],empty1[1],empty2[1],sbuf[BUFLEN];
	char file_name[146];
	char error1[146] = "specified directory doesn't exist"; 
	//char fullpath[146] = "/home/student2/nnatta/Desktop/FTProject/server/";
	bzero(rbuf,sizeof(rbuf));bzero(file_name,sizeof(file_name)); //clearing buffers
	lenoferror = strlen(error1);
	//strcat(fullpath, path);
	printf("what is path: %s ", path);
	if (strcmp(path,"root")!=0){		
		if (chdir(path) == -1)
			printf("Error occured \n");
	}
	else if (strcmp(path,"root") == 0){
		chdir("..");//change directory go one level up
	}
	else{ 
		preparepdu('E',lenoferror,error1,sbuf);//send error packet
		sendpdu(sd,sbuf);
	}	
	preparepdu('R',1,empty2,sbuf);
	sendpdu(sd,sbuf);
}

void listfiles(int sd, char dir_path[])
{
	 char *filenames[146],data[146],sbuf[BUFLEN],cwd[146];
 	 int a;
 	 int i = 0,len=0;
	  DIR           *d;
 	 struct dirent *dir;
	FILE *fp;
	fp = fopen("list.txt","w");
	printf("in listfiles(): dir_path: %s",dir_path);
	getcwd(cwd,sizeof(cwd));
	strcat(cwd,"/");
	strcat(cwd,dir_path);
	printf("what is cwd: %s\n",cwd);
	d = opendir(cwd);
	
  	if (d)
 	 {
    	while ((dir = readdir(d)) != NULL)
    	{
      		filenames[i] = malloc(strlen(dir->d_name));
      		strcpy(filenames[i],dir->d_name);
      		printf("Filename is:%s\n", filenames[i]);
      		fprintf(fp,"%s\n",filenames[i]);
      		i++;
      
    	}
    	printf("\n");
    	closedir(d);
	fclose(fp);
 	 }
	fp = fopen("list.txt","r");
	while (len = fread(data,sizeof(char),146,fp)){
		preparepdu('I',len,data,sbuf);
		printf("what is data: %s",data);
		sendpdu(sd,sbuf);
	}
	fclose(fp);
}


void preparepdu (char type, int length, char data[],char sbuf[])
{
	char str[3];
	int i=0,i1=0, i2=0, i3=0,i4=0,i5=0,i6=0,i7=0,i8=0,i9=0,i10=0,i11=0,i12=0,i13=0;
	sprintf(str, "%d",length);
	for (i13;i13<3;i13++) //REPLACE white spaces in array with '.'
		{
			if (str[i13] == '\0'|| str[i13]== ' ')
				str[i13] = '.';
		}
	if (type =='F' || type == 'f')
	{
		sbuf[0] = 'F'; //TYPE
		for (i1;i1<3;i1++) //LENGTH
		{
			sbuf[i1+1] = str[i1];
		}
		for (i2; i2<length;i2++) //DATA
		{
			sbuf[i2+4] = data[i2];
		}
	}
	else if (type =='R' || type == 'r')
	{
		
		sbuf[0] = 'R'; //PDU to send ready
		for (i3;i3<3;i3++) //LENGTH
		{
			sbuf[i3+1] = str[i3];
		}
		for (i4; i4<length;i4++) //DATA
		{
			sbuf[i4+4]=data[i4];
		}
		
	}
	else if (type =='E' || type == 'e')
	{
		sbuf[0] = 'E'; //PDU for sending error file 
		for (i5;i5<3;i5++) //LENGTH
		{
			sbuf[i5+1] = str[i5];
		}
		for (i6; i6<length;i6++) //DATA
		{
			sbuf[i6+4]=data[i6];
		}
	}
	else if (type =='I' || type == 'i') 
	{
		sbuf[0] = 'I'; //PDU to send file list
		for (i7;i7<3;i7++) //LENGTH
		{
			sbuf[i7+1] = str[i7];
		}
		for (i8; i8<length;i8++) //DATA
		{
			sbuf[i8+4]=data[i8];
		}
	}

}

int returnlength(char pdu[])
{
	int i1=0,i2=0;
	int ret;
	char str15[3];
	for (i1;i1<3;i1++)
	{
		str15[i1] = pdu[i1+1];
	}
	
	printf("content of str: %s",str15);
	ret = atoi(str15);
	return ret;
}

void sendpdu(int sd, char pdu[])
{
	int n2=0;
	printf("In sendpdu ()\n");
	printf("Contents of pdu:%s \n",pdu);
	if ((n2 = send(sd,pdu,BUFLEN,0))<0){				
		printf("Error writing to socket\n");
		exit(0);
	}

}
void getonlydata(char pdu[],char data[])
{
	int i1=0,len_of_data=0;
	for (i1 ; i1<146; i1++)	
	data[i1]=pdu[i1+4];	
}



void	reaper(int sig)
{
	int	status;
	while(wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
}

#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>

#define BUFLEN 256

int main ()
{
char str[BUFLEN];
printf("Transmist file name\n");
scanf("%s",&str);
FILE *fp = fopen(str,"w");
printf("The string length: %d\n",(strlen(str))); //does not include null character
printf("the size of array %d\n",sizeof(str));


return 0;
}

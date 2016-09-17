#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int hostname_to_ip(char *, char *); 

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0, i;
    char recvBuff[1024];
    char sendBuff[100];	
    char *hostname = argv[4];
    char ip[100];
    char pflag[2]="-p";
    char hflag[2]="-h";
    char uflag[2]="-u";	
    struct sockaddr_in serv_addr; 

  /*  if(argc != 7 || strcmp(argv[1],pflag)!=0 || strcmp(argv[3],hflag)!=0 || strcmp(argv[5],uflag)!=0)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    }*/ 

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
 
    hostname_to_ip(hostname , ip);

    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 
    
    else
    {
	printf("Connected to the server at port %s host %s\n",argv[2],argv[4]);
    }

/*    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    } */

    if(n < 0)
    {
        printf("\n Read error \n");
    } 
    i=0;
    while(i<1)
    {
	i++;
        memset(sendBuff, '\0', sizeof(sendBuff));
        sprintf(sendBuff, "%s",argv[6]);
        send(sockfd, sendBuff, strlen(sendBuff)+1, 0);
	printf("Sent Url: %s\n",argv[6]);
        //sleep(1);
    }

    return 0;
}

int hostname_to_ip(char * hostname , char* ip)
{
   struct hostent *he;
   struct in_addr **addr_list;
   int i;
   if ( (he = gethostbyname( hostname ) ) == NULL) 
   {
     // get the host info
     herror("gethostbyname");
     return 1;
   }
   addr_list = (struct in_addr **) he->h_addr_list;
   for(i = 0; addr_list[i] != NULL; i++) 
   {
     //Return the first one;
     strcpy(ip , inet_ntoa(*addr_list[i]) );
     return 0;
   }
   return 1;
}

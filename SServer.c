#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdarg.h>
#include <err.h>
#include <sys/stat.h>

#define BSIZE 0x1000
static void fail (int test, const char * format, ...)
{
    if (test) {
	va_list args;
	va_start (args, format);
	vfprintf (stderr, format, args);
	va_end (args);
	exit (EXIT_FAILURE);
    }
}


static void get_page (int s,  const char * host,  const char * page)
{
    char * msg;	
    char * ret;
    const char * format =
	"GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: fetch.c\r\n\r\n";
    int status;	
    FILE *fp;
    status = asprintf (& msg, format, page, host);
    fail (status == -1 || ! msg, "asprintf failed.\n");
    status = send (s, msg, strlen (msg), 0);
    fail (status == -1, "send failed: %s\n", strerror (errno));
    fp=fopen("/home/vishalk/Documents/temp1","w");
    fputs("",fp);
    fclose(fp);		
    fp = fopen("/home/vishalk/Documents/temp1","a");
    while (1) {
	int bytes;
        char buf[BSIZE+1];
        memset(buf, 0, sizeof(buf));
        bytes = recvfrom (s, buf, BSIZE, 0, 0, 0);
        if (bytes == 0) {
            break;
        }
        fail (bytes == -1, "%s\n", strerror (errno));
	fputs (buf,fp);
    }
    fclose(fp);		
    fp=fopen("/home/vishalk/Documents/temp1","rb");
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, fp);
    fclose(fp);
    string[fsize] = 0;	
    ret=strstr(string,"<!DOCTYPE");
    fp=fopen("/home/vishalk/Documents/temp2","w");
    fputs (ret,fp);
    fclose(fp);		
    free (msg);		
}
char * md5path(char * urlpath){
    char * sendfile;
    char msg1[250];
    char buff[512];
    memset(&msg1[0],0,sizeof(msg1));
    memset(&buff[0],0,sizeof(buff));
    free(sendfile);
    FILE *in;
    extern FILE *popen();	
    sprintf(msg1,"echo -n %s | md5sum", urlpath);
    in=popen(msg1,"r");
    if(NULL==in){
      printf("popen failed");
      return "Error while opening file";
    }
    fputs(msg1,in);
    fgets(msg1,sizeof(msg1),in);
    sendfile=msg1;
    fclose(in);
    return sendfile;
}

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    char sendBuff[1025];
    char recvBuff[100];
    const char * content;
    char * urlpath;	
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[2])); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 
    
    printf("Waiting for clients\n");
    while(1)
    {   
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
	printf("A client has connected\n");
        memset(recvBuff, '\0', sizeof(recvBuff));
        recv(connfd, recvBuff, sizeof(recvBuff),0);
        printf( "Recieved URL : %s\n", recvBuff);
	char  * url=recvBuff;
        urlpath=url;
	printf("url:%s\n",url);
	printf("urlpath:%s\n",urlpath);
	char * pch;
	int j=0;
	char  resource[100]="";
	char  host[100];
	pch = strtok(url,"//");
	while(pch !=NULL)
	{  
	   j++;
	   if(j==2){
	      strcpy(host,(char*)pch);
	   }
	   if(j>2){
	       strcat(resource,"/");
	       strcat(resource,(char *)pch);		
	   }
	   pch=strtok(NULL,"//");			 	
	}
 	struct addrinfo hints, *res, *res0;
    int error;
    int s;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_INET;//PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    error = getaddrinfo (host, "http", & hints, & res0);
    fail (error, gai_strerror (error));
    s = -1;
    for (res = res0; res; res = res->ai_next) {
        s = socket (res->ai_family, res->ai_socktype, res->ai_protocol);
        fail (s < 0, "socket: %s\n", strerror (errno));
        if (connect (s, res->ai_addr, res->ai_addrlen) < 0) {
            fprintf (stderr, "connect: %s\n", strerror (errno));
            close (s);
            exit (EXIT_FAILURE);
        }
        break;
    }	
    const char * host1=host;
    const char * resource1=resource;			
    if (s != -1) {
        get_page (s, host1, resource1);
	  printf("Downloaded the page\n");
    }
 
    char * sendfile=md5path(urlpath);	
    char * oldname="temp2";
    int rnm=rename(oldname,sendfile);
    if(rnm == 0){
     printf("Storing to filename: %s\n",sendfile);
    }
    else
    {
     printf("Error: unable to rename the file");
    }		
    FILE * fp=fopen(sendfile,"r");
    int prev=ftell(fp);
    fseek(fp,0L,SEEK_END);
    int size = ftell(fp);
    fseek(fp,prev,SEEK_SET);
    printf("downloaded number of bytes: %d\n",size);
    freeaddrinfo (res0);	
    close(connfd);
    sleep(1);
   }
}
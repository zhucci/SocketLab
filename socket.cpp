#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <string.h>
#include <cstdlib>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <fstream>
typedef struct {
    int sd;
    sockaddr *addr;
    char *ip_addr;
    int port;
} MySocket;

MySocket *tcp_open(void *address, int port);
int getMessage(int sd,char *buf,int size);
int main(int argc, char **argv)
{
  if(argc<3)
    {
    printf("Mail utility are needed a message file and\n"
	   "and mail address of recipient\n"
	   "call: <function> <message file> <RCPT addr>\n");
    return 0;
    }


  size_t SIZE=100;
  char response[SIZE],query[SIZE];
  
  char *sender =(char *) "zhuchkov612@gmail.com";
  
  // char *str= new char[len];
  
  //  sprintf(str,"nc %s %s",argv[1],argv[2]);
  //  FILE *chanel = popen(str,"w");

    FILE *message = fopen(argv[1],"r");
    assert(message!=NULL);

  char *Domain = (char *) memchr(argv[2],'@',strlen(argv[2]));
   *Domain='\0';
   Domain++;
  char *recipient = argv[2];

 
   char yandexServ[]="mx.yandex.ru";
   char gmailServ[]="gmail-smtp-in.l.google.com";
   char yandex[]="yandex";
   char gmail[]="gmail";
  char *domainName;
  if(!strncmp(yandex,Domain,strlen(yandex)))
    {
      domainName = yandexServ;
    }
  else if(!strncmp(gmail,Domain,strlen(gmail)))
    {
      domainName=gmailServ;
    }
  else
    {
      domainName = Domain;
    }

  printf("%s\n%s\n",recipient,domainName);

  hostent *host = gethostbyname2(domainName,AF_INET);

#ifndef Release
  for(int i=0;host->h_addr_list[i];i++)
    printf("%d:%s\n",i,
	   	 inet_ntoa(*( in_addr *)host->h_addr_list[i]));
#endif


//

  //Create socket and bind it
  MySocket *sock=tcp_open(host->h_addr_list[1],25);

    getMessage(sock->sd,response,SIZE);
    int n;
  //Greeting
    n=sprintf(query,"HELO zhucci\n");
   printf("%s",query);
     write(sock->sd,query,n);
    
   getMessage(sock->sd,response,SIZE);
 
  //MAIL FROM
  n = sprintf(query,"MAIL FROM:<%s>\n",sender);

  printf("%s",query);

  write(sock->sd,query,n);
 
  getMessage(sock->sd,response,SIZE);

  //RCPT TO

  n=sprintf(query,"RCPT TO:<%s@%s>\n",recipient,Domain);
  printf("%s",query);
  n= write(sock->sd,query,n);
  getMessage(sock->sd,response,SIZE);
  
  //DATA
   n=sprintf(query,"DATA\n");
   n=write(sock->sd,query,n);
   printf("%s",query);
   getMessage(sock->sd,response,SIZE);

   //   n=sprintf(query,"Date: 8 Aug 1998 04:10:45 -0000\n");
   //  n= write(sock->sd,query,n);
   //  printf("%s",query);
   //Header
   n=sprintf(query,"From:%s\n",sender);
   n= write(sock->sd,query,n);
   printf("%s",query);
   // getMessage(sock->sd,response,SIZE);

   n=sprintf(query,"To:%s@%s\n",recipient,Domain);
n= write(sock->sd,query,n);
   printf("%s",query);
   //  getMessage(sock->sd,response,SIZE);

  n=sprintf(query,"Subject: Socket test\n");
  n= write(sock->sd,query,n);
  printf("%s",query);
  //  getMessage(sock->sd,response,SIZE);
  //New line
       n=sprintf(query,"\n");
       write(sock->sd,query,n);
       printf("%s",query);

   char *buf= (char *)malloc(SIZE+1);
   //MSG TEXT

   for(; ( n=getline(&buf,&SIZE,message) ) > 0;) 
     {
       n=sprintf(query,"%s",buf);
       write(sock->sd,query,n);
       printf("%s",query);
     }
   //END MSG \n'.'\n
      n=sprintf(query,"\n");
       write(sock->sd,query,n);
       printf("%s",query);

      n=sprintf(query,".\n");
       write(sock->sd,query,n);
      printf("%s",query);

      getMessage(sock->sd,response,SIZE);
      //QUIT
        n=sprintf(query,"QUIT\n");
        write(sock->sd,buf,n);
        printf("%s",query);

   //END
   //   fromServer.close();
  shutdown(sock->sd,SHUT_WR);
   close(sock->sd);
   
  return 0;
}
int getMessage(int sd,char *buf,int size)
{
  int n=size;
  int count=0;
  while(n==size)
    {
      n=read(sd,buf,size);
      if(n<0)
       {
	 printf("ERRNO = %d\n",n);
       }
      for(int i=0;i<n;i++)
       printf("%c",buf[i]);
      count+=n;
    }
     printf("\n"); 
     
     return count;
}
MySocket* tcp_open(void *address, int port) {

  
    MySocket *s = (MySocket *)malloc( sizeof(MySocket) );

     s->sd = socket(AF_INET,SOCK_STREAM,0);
    
     sockaddr_in *addr=(sockaddr_in*)malloc(sizeof(sockaddr_in));

    memset(addr, 0, sizeof(struct sockaddr_in));

      addr->sin_family = AF_INET;
      addr->sin_port = htons(port);
      addr->sin_addr.s_addr = *(in_addr_t *)address;
      addr->sin_port = htons(port);

      s->addr=(sockaddr *)addr;

      bind(s->sd,s->addr, sizeof(*s->addr)) ;
        
      s->port = port;

      s->ip_addr = inet_ntoa(*( in_addr *)address); 
    
      assert(s->sd>0);
      
     int res = connect(s->sd,s->addr,sizeof(*s->addr));
     assert(res==0);
    return s;
  
}

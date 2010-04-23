#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>

#define PORT_NUM1 3301
#define PORT_NUM2 3302

int main()
{
	char hostname[100] ;
	//gethostname(hostname,sizeof hostname) ;
	//printf("\nHostname=%s\n",hostname) ;
	sprintf(hostname,"dwingeloo") ;
	struct hostent *Host ;
	Host=gethostbyname(hostname) ;

	struct sockaddr_in host_addr ;
	host_addr.sin_family=AF_INET ;
	host_addr.sin_port=htons(PORT_NUM1) ;
	host_addr.sin_addr=*(struct in_addr*)Host->h_addr ;
	//my_addr.sin_addr.s_addr=INADDR_ANY ;
	printf("Address=%u\n",host_addr.sin_addr.s_addr) ;
	
	struct sockaddr_in host_addr2 ;
	host_addr2.sin_family=AF_INET ;
	host_addr2.sin_port=htons(PORT_NUM2) ;
	host_addr2.sin_addr=*(struct in_addr*)Host->h_addr ;
	//my_addr.sin_addr.s_addr=INADDR_ANY ;
	printf("Address=%u\n",host_addr2.sin_addr.s_addr) ;
	memset(host_addr2.sin_zero,'\0',sizeof host_addr2.sin_zero) ;

	char read_data[100],read_data2[100] ;
	int sockfd,sockfd2 ;
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket(): Error\n") ;
		exit(1) ;
	}
	printf("sockfd=%d\n",sockfd) ;

	if((sockfd2=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket(): Error\n") ;
		exit(1) ;
	}
	printf("sockfd2=%d\n",sockfd2) ;
	if(connect(sockfd,(struct sockaddr*)&host_addr,sizeof host_addr)==-1)
	{
		perror("connect(): Error\n") ;
		printf("connect(): Error %d\n",PORT_NUM1) ;
		exit(1) ;
	}
	write(sockfd,"Client up",20) ;
	
	if(connect(sockfd2,(struct sockaddr*)&host_addr2,sizeof host_addr2)==-1)
	{
		perror("connect(): Error\n") ;
		printf("connection error at port no. %d\n",PORT_NUM2) ;
		exit(1) ;
	}
	write(sockfd2,"Client 2 up",30) ;
	
	int rev_bytes=read(sockfd,read_data,sizeof read_data) ;
	printf("\tBytes read=%d  Message=%s\n",rev_bytes,read_data) ;

	int rev_bytes2=read(sockfd2,read_data2,sizeof read_data2) ;
	printf("\tBytes read2=%d  Message=%s\n",rev_bytes2,read_data2) ;
	
	int i=0;
	int send_bytes, send_bytes2 ;
	
	while(1)
	//for(int j=1;j<=4;j++)
	{
		fd_set writefds ;
		FD_ZERO(&writefds) ;
		FD_SET(sockfd,&writefds) ;
		FD_SET(sockfd2,&writefds) ;
		select(sockfd2+1,NULL,&writefds,NULL,NULL) ;

		sleep(3) ;
		printf("Write:") ;
		if (i%3 == 0)
		{
			if(FD_ISSET(sockfd,&writefds))
			{	
				if((send_bytes=write(sockfd,"Message from client",20))==-1)
					perror("write: Error\n") ;
			}
			else
				perror("FD_ISSET: Error\n") ;
			printf("\t%d Bytes send=%d\n",i,send_bytes) ;
		}
		else
		{
			if(FD_ISSET(sockfd,&writefds))
			{
				send_bytes2=write(sockfd2,"Message from client2",30) ;
				
				if(send_bytes==-1)
				{
					printf("Err 8\n") ;
					perror("write2: Error\n") ;
				}
			}
			else
				perror("FD_ISSET 2: Error\n") ;
			printf("\t%d Bytes send2=%d\n",i,send_bytes2) ;
		}
	
		//sleep(5) ;
		/*printf("Read:") ;
		int rev_bytes=read(sockfd,read_data,sizeof read_data) ;
		int rev_bytes2=read(sockfd2,read_data2,sizeof read_data2) ;
		printf("\tBytes read=%d  Message=%s\n",rev_bytes,read_data) ;
		printf("\tBytes read2=%d  Message2=%s\n",rev_bytes2,read_data2) ;*/
		i++;
	}
	printf("END\n") ;
	shutdown(sockfd,2) ;
	shutdown(sockfd2,2) ;
}

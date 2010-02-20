#include "CLASS_Sockets.h"

int SOCKET_CALLS:: Open_Socket(int PORT_NUM)
{
	Host_Addr.sin_family=AF_INET ;
	Host_Addr.sin_port=htons(PORT_NUM) ;
	Host_Addr.sin_addr=*(struct in_addr*)Host->h_addr ;
	printf("Address=%u\n",Host_Addr.sin_addr.s_addr) ;
	memset(Host_Addr.sin_zero,'\0',sizeof Host_Addr.sin_zero) ;

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket(): Error\n") ;
		exit(1) ;
	}
	return sockfd ;
}

void SOCKET_CALLS:: Connect_Socket(int PORT_NUM)
{
	if(connect(sockfd,(struct sockaddr*)&Host_Addr,sizeof Host_Addr)==-1)
	{
		perror("connect(): Error\n") ;
		printf("connect(): Error %d\n",PORT_NUM) ;
		exit(1) ;
	}
}

void SOCKET_CALLS:: Bind_Socket()
{
	int on=1 ;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) ;	//socket reuse
	if(bind(sockfd,(struct sockaddr *)&Host_Addr,sizeof Host_Addr)==-1)
	{
		perror("bind(): Error\n") ;
		//printf("Port number %d\n",3301) ;
		exit(1) ;
	}
}

void SOCKET_CALLS:: Listen_Socket() 
{
	if(listen(sockfd,1)==-1)
	{
		perror("listen(): Error\n") ;
		exit(1) ;
	}
}

int SOCKET_CALLS:: Accept_Socket()
{
	struct sockaddr_storage their_addr ;
	socklen_t addr_size ;
	addr_size=sizeof their_addr ;
	
	if((newsockfd=accept(sockfd,(struct sockaddr*) &their_addr,&addr_size))==-1)
	{
		perror("accept(): Error\n") ;
		exit(1) ;
	}
	return newsockfd ;
}

int SOCKET_CALLS:: Write_Data(unsigned char* data)
{
	write_bytes=write(sockfd,data,sizeof (data)) ;
	printf("write bytes=%d\n",write_bytes) ;
	return(write_bytes) ;	
}

void SOCKET_CALLS:: Read_Data()
{
	read_bytes=read(sockfd,read_data,sizeof read_data) ;
}

void SOCKET_CALLS:: Close_Socket()
{
	close(newsockfd) ;
	close(sockfd) ;
}

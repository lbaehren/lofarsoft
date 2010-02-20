#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <fcntl.h>

#define PORT_NUM1 3301
#define PORT_NUM2 3302
int main()
{
	char hostname[100] ;
	gethostname(hostname,sizeof hostname) ;
	printf("\nHostname=%s\n",hostname) ;
	struct hostent *h ;
	h=gethostbyname(hostname) ;

	struct sockaddr_in my_addr ;
	my_addr.sin_family=AF_INET ;
	my_addr.sin_port=htons(PORT_NUM1) ;
	my_addr.sin_addr=*(struct in_addr*)h->h_addr ;
	//my_addr.sin_addr.s_addr=INADDR_ANY ;
	printf("Address=%u\n",my_addr.sin_addr.s_addr) ;
	memset(my_addr.sin_zero,'\0',sizeof my_addr.sin_zero) ;
	
	struct sockaddr_in my_addr2 ;
	my_addr2.sin_family=AF_INET ;
	my_addr2.sin_port=htons(PORT_NUM2) ;
	my_addr2.sin_addr=*(struct in_addr*)h->h_addr ;
	//my_addr.sin_addr.s_addr=INADDR_ANY ;
	printf("Address=%u\n",my_addr2.sin_addr.s_addr) ;
	memset(my_addr2.sin_zero,'\0',sizeof my_addr2.sin_zero) ;
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

	if(bind(sockfd,(struct sockaddr *)&my_addr,sizeof my_addr)==-1)
	{
		perror("bind(): Error\n") ;
		printf("Port number %d\n",3301) ;
		exit(1) ;
	}

	if(bind(sockfd2,(struct sockaddr *)&my_addr2,sizeof my_addr2)==-1)
	{
		perror("bind(): Error\n") ;
		exit(1) ;
	}

	if(listen(sockfd,1)==-1)
	{
		perror("listen(): Error\n") ;
		exit(1) ;
	}

	if(listen(sockfd2,1)==-1)
	{
		perror("listen(): Error\n") ;
		exit(1) ;
	}

	int newsockfd,newsockfd2 ;
	struct sockaddr_storage their_addr, their_addr2 ;
	struct sockaddr_in sa,sa2 ;
	socklen_t addr_size,addr_size2 ;
	addr_size=sizeof their_addr ;
	addr_size2=sizeof their_addr2 ;
	char read_data[100000],read_data2[100000] ;
	int retbytes,retbytes2;
		if((newsockfd=accept(sockfd,(struct sockaddr*) &their_addr,&addr_size))==-1)
		{
			perror("accept(): Error\n") ;
			exit(1) ;
		}
		printf("newsockfd=%d\n",newsockfd) ;
		//retbytes=read(newsockfd,read_data,sizeof (read_data)) ;
		//printf("\tBytes read=%d  Message=%s\n",retbytes,read_data) ;
		
		if((newsockfd2=accept(sockfd2,(struct sockaddr*) &their_addr2,&addr_size2))==-1)
		{
			perror("accept(): Error\n") ;
			exit(1) ;
		}
		printf("newsockfd2=%d\n",newsockfd2) ;
		//retbytes2=read(newsockfd2,read_data2,sizeof (read_data2)) ;
		//printf("\tBytes read2=%d  Message=%s\n",retbytes2,read_data2) ;
	
		/*FILE *control ;
		unsigned char keyword[100],value[100],definition[100] ;
		control=fopen("Control_Settings.dat","r") ;
		while(!feof(control))
		{
			fscanf(control,"%s%s%s",keyword,value,definition) ;
			printf("%s\t%s\t%s\n",keyword,value,definition) ;
		}
		fclose(control) ;

		int bytes_send=write(newsockfd,"Hello ...",20) ;
		printf("\tBytes send=%d\n",bytes_send) ;
		
		int bytes_send2=write(newsockfd2,"Hello 2 ...",20) ;
		printf("\tBytes send2=%d\n",bytes_send2) ;*/
		
		int i=1;
		int out_handle1,out_handle2 ;
		out_handle1=open("u11",O_CREAT|O_WRONLY,0444) ;
		out_handle2=open("u22",O_CREAT|O_WRONLY,0444) ;

		//for(int j=1;j<=5;j++)
		while(1)
		{
		fd_set readfds ;
		FD_ZERO(&readfds) ;
		FD_SET(newsockfd,&readfds) ;
		FD_SET(newsockfd2,&readfds) ;
		select(newsockfd2+1,&readfds,NULL,NULL,NULL) ;
		//if(FD_ISSET(newsockfd,&readfds) && FD_ISSET(newsockfd2,&readfds))
		//printf("Read:") ;

		if(FD_ISSET(newsockfd,&readfds))
		{
			if((retbytes = read(newsockfd,read_data,sizeof (read_data)))==-1)
				perror("read(): Error\n") ;

			//printf("\t%d: Bytes read=%d  Message=%s\n",i,retbytes,read_data) ;
			printf("\t%d: Bytes read=%d\n",i,retbytes) ;
			
			if(retbytes==0)	exit(1) ;
				
			write(out_handle1,read_data,retbytes) ;
		} 
		if (FD_ISSET(newsockfd2,&readfds)) 
		{
			if((retbytes2 = read(newsockfd2,read_data2,sizeof (read_data2)))==-1)
				perror("read2(): Error\n") ;

			//printf("\t%d: Bytes read2=%d  Message2=%s\n",i,retbytes2,read_data2) ;
			printf("\t%d: Bytes read2=%d\n",i,retbytes2) ;
			
			if(retbytes2==0) exit(1) ;

			write(out_handle2,read_data2,retbytes2) ;

			/*printf("Write:") ;
			int bytes_send=write(newsockfd,"Message from server",20) ;
			int bytes_send2=write(newsockfd2,"Message from server",20) ;
			printf("\tBytes send=%d\n",bytes_send) ;
			printf("\tBytes send2=%d\n",bytes_send2) ;*/
		}
		i++;
		}

		close(out_handle1) ;
		close(out_handle2) ;
		close(newsockfd) ;
		close(newsockfd2) ;
	close(sockfd) ;
	close(sockfd2) ;
}

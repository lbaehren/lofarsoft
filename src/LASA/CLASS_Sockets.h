#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>

class SOCKET_CALLS
{
	private:
		struct hostent *Host ;
		struct sockaddr_in Host_Addr ;
		int sockfd,newsockfd,write_bytes,read_bytes ;
		char read_data[200] ;
		
	public:
		int Open_Socket(int) ;
		void Connect_Socket(int) ;
		void Bind_Socket() ;
		void Listen_Socket() ;
		int Accept_Socket() ;
		int Write_Data(unsigned char*) ;
		void Read_Data() ;
		void Close_Socket() ;

		SOCKET_CALLS()
		{
			Host=gethostbyname(Hostname) ;
		}
} ;

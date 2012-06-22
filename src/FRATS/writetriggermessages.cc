/* udpserver.c */ 

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string>
#include "FRATcoincidence.h"
#include "FRATcoincidence.cc"
#include <map>
#include <time.h>
#include <ctime>
#include "sys/time.h"


using namespace FRAT::coincidence;
using namespace std;



int main(int argc , char *argv[])
{

         if(argc<2){
             
             cout << "usage: " << argv[2] << " <filename> " << endl;
             return 200;
         }
         char* filename=argv[1];
	    
        ofstream * trfile = new ofstream;
        trfile->open(filename, ios::binary  | ios::out); 
        
	    int sock, out_sock;
        int bytes_read;
        socklen_t addr_len;
        char recv_data[sizeof(struct triggerEvent)];
        struct sockaddr_in server_addr , client_addr, out_server_addr;
        triggerEvent * trigger;
        trigger = new triggerEvent;
        int latestindex;
	    

//        int subband;
//        int time;
//        int max;
        string temp;
 
    
        //long int *utc_sec = new long int;
        //long int *utc_nanosec = new long int;
        //utc_sec = 20000;
        //utc_nanosec= 10000;
        //TBBdumpMessage.utc_second=  &utc_sec;
        //TBBdumpMessage.utc_nanosecond= &utc_nanosec;
        
        //data[2]--> data[5] event time in UTC
        //data[6]--> data[9] event time in nanoseconds
        //out_hostname="10.135.252.101";


        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(FRAT_TRIGGER_PORT_0);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        bzero(&(server_addr.sin_zero),8);

        if (bind(sock,(struct sockaddr *)&server_addr,
            sizeof(struct sockaddr)) == -1)
        {
            perror("Bind");
            exit(1);
        }

        addr_len = sizeof(struct sockaddr);
	
	    int obsID=-1;
		
        printf("\nUDPServer Waiting for client on port %d", FRAT_TRIGGER_PORT_0);
        fflush(stdout);
        int number=1;

	while (1)
	{
          cout << " Waiting for new event ..." << number << endl;

          bytes_read = recvfrom(sock,recv_data,sizeof(struct triggerEvent),0, (struct sockaddr *)&client_addr, &addr_len);
	      number++; 
          
	      recv_data[bytes_read] = '\0';
         
          
          trigger = (triggerEvent*) recv_data;
		  trfile->write((const char*)&trigger->time, sizeof(struct triggerEvent));
          cout << sizeof(trigger->time) << " " << sizeof(struct triggerEvent) << " " << trigger->time << " " << trigger->utc_second << " " << trigger->utc_nanosecond << endl; 

          trfile->flush();

    }
    return 0;
}


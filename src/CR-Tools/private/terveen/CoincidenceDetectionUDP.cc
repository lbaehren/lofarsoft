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

         if(argc<4){
             cout << "usage: " << argv[4] << " <CoincidenceNumber> <CoincidenceTime> <nrbeams> <nrDMs>" << endl;
             return 200;
         }
         int CoinNr=atoi(argv[1]);
         int CoinTime=atoi(argv[2]);
	     int nrbeams=atoi(argv[3]);
	     int nrDMs=atoi(argv[4]);
         int mincoinbeams=2;
         int coinbeams=nrbeams-1;
         CoinCheck* cc[nrbeams][nrDMs];
	     for(int i=0;i<nrbeams;i++){
			 for(int j=0;j<nrDMs;j++){
                 cc[i][j] = new CoinCheck();
			 }
	     }
			 
	     CoinCheck* RFIcc[nrDMs];
	     for(int j=0;j<nrDMs;j++){
		     RFIcc[j] = new CoinCheck();
	     }
	    
        
	    int sock, out_sock;
        int bytes_read;
        socklen_t addr_len;
        char recv_data[sizeof(struct triggerEvent)];
        struct sockaddr_in server_addr , client_addr, out_server_addr;
        triggerEvent * trigger;
        trigger = new triggerEvent;
        int latestindex;
	    
	    typedef std::map<float,int> FloatToIntMap;
	    FloatToIntMap DMtoID;
	    FloatToIntMap::iterator iter=DMtoID.begin();
	    int DMid=0;

//        int subband;
//        int time;
//        int max;
        string temp;
        unsigned char TBBdumpMessage[11];        
        TBBdumpMessage[0]=0x99;
        TBBdumpMessage[1]=0xA0;
        TBBdumpMessage[2]='F';
        TBBdumpMessage[3]='U';
        TBBdumpMessage[4]='L';
        TBBdumpMessage[5]='L';
        TBBdumpMessage[6]='D';
        TBBdumpMessage[7]='U';
        TBBdumpMessage[8]='M';
        TBBdumpMessage[9]='P';
        //data[2]--> data[5] event time in UTC
        //data[6]--> data[9] event time in nanoseconds
        TBBdumpMessage[10]=0x68;
        char* out_hostname;
        out_hostname="10.135.252.101";
        if((out_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
            perror("Socket");
            exit(1);
        }



        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(FRAT_TRIGGER_PORT_0);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        bzero(&(server_addr.sin_zero),8);
        struct hostent *out_host;
        out_host= (struct hostent *) gethostbyname(out_hostname);
        out_server_addr.sin_family = AF_INET;
        out_server_addr.sin_port = htons(FRAT_TRIGGER_PORT_1);
        out_server_addr.sin_addr = *((struct in_addr *)out_host->h_addr);
        bzero(&(out_server_addr.sin_zero),8);

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

	while (1)
	{
          cout << " Waiting for new event ..." << endl;

          bytes_read = recvfrom(sock,recv_data,sizeof(struct triggerEvent),0, (struct sockaddr *)&client_addr, &addr_len);
	  
          
	      recv_data[bytes_read] = '\0';
         
          trigger = (triggerEvent*) recv_data;
		  if(obsID!=trigger->obsID){
		     if(obsID < 0) {
			    obsID=trigger->obsID;
				
		     } else {
                cerr << "Receiving triggers from two obsIDs not supported: " << obsID << " " << trigger->obsID << " . Discarding trigger " << endl;
				continue; 
			 }
	      }
          /*if(trigger->beam >= nrbeams) {
              cerr << "Receiving trigger from a higher beam number than expected, discarding trigger" << trigger->beam << " > " nrbeams << endl;
              continue;
          }*/
		  
		  iter=DMtoID.find(trigger->DM);
		  if ( iter==DMtoID.end() ) {
              if ( DMid == nrDMs ){
              cerr << "Received triggers on more DMs than expected.  discarding trigger" << endl;
              continue;
              }
              else{
                 cout << "Adding DM " << trigger->DM << " at ID " << DMid << endl;
			     DMtoID[trigger->DM]=DMid;
			     DMid++;
              }
		  }

		 
		  printf("%d \n",trigger->subband);
          latestindex=cc[trigger->beam][  DMtoID[trigger->DM]   ]->add2buffer(*trigger);
          if(cc[trigger->beam][  DMtoID[trigger->DM]   ]->coincidenceCheck(latestindex, CoinNr, CoinTime)) {
                timeval t;
                gettimeofday (&t, NULL);

                time_t _timestamp;
                long int _timestamp_msec;

                _timestamp      = t.tv_sec;
                _timestamp_msec = t.tv_usec/1000;


                cout << _timestamp << "." << _timestamp_msec  << " Trigger found in beam " << trigger->beam << " at time: " << trigger->time << " with DM: " << trigger->DM << endl;

                 int n = sendto(out_sock, TBBdumpMessage, sizeof(TBBdumpMessage), 0,
                                    (struct sockaddr *)&out_server_addr, sizeof(struct sockaddr));
                 if ( n < 0 ) {
                     cerr << "Failed to send message to request dump." << endl; }
                //cout << cc->printEvent();
			  if(nrbeams > mincoinbeams) {
			    trigger->subband=trigger->beam;
			    latestindex=RFIcc[  DMtoID[trigger->DM]   ]->add2buffer(*trigger);
			    if(RFIcc[  DMtoID[trigger->DM]   ]->coincidenceCheck(latestindex, coinbeams, CoinTime)){
					cout << _timestamp << "." << _timestamp_msec << " Trigger is probably RFI at time " << trigger->time << " and DM " << trigger->DM << endl;
				}
			  }
          }

   

	  fflush(stdout);

    }
    return 0;
}


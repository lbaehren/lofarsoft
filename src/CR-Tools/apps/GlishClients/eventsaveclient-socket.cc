
/*!
  \file eventsaveclient-socket.cc
  
  \ingroup CR_Glish

  \brief send events to the KASCADE-xevb.

  <h3>Synopsis</h3>

  adapted from fdaq -> workersocket.c
*/

/* $Id: eventsaveclient-socket.cc,v 1.2 2006/12/20 13:00:50 horneff Exp $ */

#ifndef _workersock_c_
#define _workersock_c_

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// custom includes
#include <Data/lopesevent_def.h>

typedef struct {
  unsigned int length, tag, magic;
  unsigned int leer, lengthI;
  unsigned int dgroup, dprocessor, dprocess;
  unsigned int sgroup, sprocessor, sprocess;
  transev1 data;
  unsigned int magic2;
} lopesmessage;

// global variables: listen and send file descriptors 
int listenfd=-1, sendfd=-1;

//------------------------------------------------------------------------------

/*!
  \fn int CloseSocket(int *fd)

  \brief close a socket

  \param fd -- pointer to the file descriptor of the socket

  \return 1 (success) or 0 (fail)

*/
int CloseSocket(int *fd)
{
  int i;
 
  std::cout << "Schliesse ein Socket\n";
  i = close(*fd);
  *fd = 0;
  return 1 + i;
}


//-----------------------------------------------------------------------
/*!
  \fn int OpenListenSocket(int port){

  \brief open a listening socket

  \param port   = port number to listen on

  \return >0 : file descriptor of the listening socket
          <0 : Error

 */
//-----------------------------------------------------------------------

int OpenListenSocket(int port){
  struct sockaddr_in addr;
  int RetVal, serv_fd;
  
  if ((serv_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cout << "Syscall socket() fehlgeschlagen.\n";
    return -1;
  };
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons (port);
  if((RetVal = bind(serv_fd, (struct sockaddr *)&addr, 
                    sizeof(struct sockaddr_in)) ) < 0 ){
    std::cout << "Syscall bind() fehlgeschlagen.\n";
    close(serv_fd);
    return -1;
  };
  if (listen(serv_fd, 1) < 0 ){
    std::cout << "Syscall listen() fehlgeschlagen.\n";
    close(serv_fd);
    return -1;
  };
  if (fcntl(serv_fd,F_SETFL,O_NONBLOCK) < 0) {
    std::cout <<"Syscall fnctl(...,O_NONBLOCK) fehlgeschlagen.\n";
    close(serv_fd);
    return -1;
  };
  return serv_fd;
}


//-----------------------------------------------------------------------
/*!
  \fn int AcceptSocket(int serv_fd)


  \brief accept a connection on a listening socket

  \param serv_fd   = file descriptor of listening socket

  \return >0 : file descriptor of the accepted connection
          <0 : Error

 */
//-----------------------------------------------------------------------
 
int AcceptSocket(int serv_fd){
  int work_fd;

  if (serv_fd <= 0) {
    return -1;
  };
  work_fd = accept(serv_fd, NULL, NULL);
  if (work_fd < 0){
    if ((errno == ENOTSOCK) || (errno == ENOBUFS) || (errno == ENOMEM)){
      std::cout << "Server Socket kaputt.\n";
      return -2;
    };
    return -1; //socket not active, e.g. no pending connection
  };
  if (fcntl(work_fd,F_SETFL,O_SYNC) < 0) {
    std::cout << "Syscall fcntl(...,O_SYNC) fehlgeschlagen.\n";
    close(work_fd);
    return -1;
  };

  return work_fd;  
};


//-----------------------------------------------------------------------
/*!
  \fn int SendMessage(int fd, transev1 *ev)

  \brief send the event "ev" over the socket "fd"

  \param fd   = file descriptor of socket
  \param ev   = pointer to the event to be sent

  \return 1 (success) or 0 (fail)

 */
//-----------------------------------------------------------------------
 
int SendMessage(int fd, transev1 *ev){
  lopesmessage mess;
  int tmp;
  
  mess.tag = LOPES_EVENT;
  mess.magic = 1234567890; /* CMessage-Magic */
  mess.sgroup = LOPES_GRP;
  mess.sprocessor = 0;
  mess.sprocess = 0;
  mess.dgroup     = GID_EVB;
  mess.dprocessor = TID_EVB;
  mess.dprocess   = PID_EVB_KERNEL;

  /* Datenblock kopieren, falls mal asyncrones I/O verwendet wird. */
  memcpy(&mess.data, ev, ev->length); 

  // Eigentlich so nicht noetig aber so von fdaq uebernommen.
  // Schreibt unterm Strich nach mess.magic2 
  ((unsigned int *)&mess.data)[(ev->length/4)] = 1234567890; /* CMessage-Magic */

  // die beiden "1*4" sind fuer den Platz von "magic2"
  mess.lengthI  = mess.length = ev->length + 1*4;
  tmp = mess.length + sizeof(lopesmessage) - 1*4 - sizeof(transev1);
  
  if (write(fd,&mess,tmp) <0) { return 0; };
  return 1;
};

//-----------------------------------------------------------------------
/*!
  \fn int check_xevb_connection()

  \brief If necessary, check for incoming connections on the xevb-socket
         and handle them. 

  \param none

  \return 1 (success) (or 0 (fail))

 */
//-----------------------------------------------------------------------
 
int check_xevb_connection(){

  //check for a pending connection (if not connected)
  if (sendfd <=0) {
    sendfd = AcceptSocket(listenfd);
    if (sendfd > 0) { std::cout << "Accepted connection from xevb!\n";};
  };

  return 1;
};

//-----------------------------------------------------------------------
/*!
  \fn int handle_xevb_event(lopesevent *event)

  \brief If necessary, check for incoming connections on the xevb-socket
         and handle them. If an established connection to the xevb exists,
         then send the event.

  \param event   = pointer to the event that may be sent

  \return 1 (success) or 0 (fail)

 */
//-----------------------------------------------------------------------
 
int handle_xevb_event(lopesevent *event){
  transev1 sendevent;

  //check for a pending connection (if not connected)
  if (sendfd <=0) {
    sendfd = AcceptSocket(listenfd);
    if (sendfd > 0) { std::cout << "Accepted connection from xevb!\n";};
  };

  //if connected, send the event
  if (sendfd > 0) {
    sendevent.length = sizeof(sendevent);
    sendevent.version = 1;
    sendevent.JDR = event->JDR;
    sendevent.TL = event->TL;
    sendevent.type = event->type;
    sendevent.evclass = event->evclass;
    if (SendMessage(sendfd, &sendevent) != 1) { 
      std::cout << "Lost connection to the xevb.\n";
      close(sendfd);
      sendfd =-1; 
    }; 
  };

  return 1;
};


//-----------------------------------------------------------------------
/*!
  \fn int open_xevb_socket()

  \brief open the listening socket for connections from the xevb

  \return 1 (success) or 0 (fail)

 */
//-----------------------------------------------------------------------

int open_xevb_socket(){
  listenfd = OpenListenSocket(LOPES_PORT);
  if (listenfd <= 0) {
    std::cout << "Unable to open listening socket!\n";
    listenfd = -1;
    return 0;
  };
  std::cout << "Opened listening socket!\n";
  return 1;
};

#endif /* _workersock_c_ */

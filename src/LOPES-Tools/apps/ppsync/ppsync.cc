
/* $Id: ppsync.cc,v 1.3 2005/05/09 09:27:41 bahren Exp $ */

// Standard header files
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sched.h>
#include <sys/io.h> 

// AIPS++/CASA library
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/stdio.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/IO.h>
#include <tasking/Glish.h>

#define HISTBINS 100

#include <casa/namespace.h>

/*!
  \file ppsync.cc

  \brief Synchronization of parallel port CLI.

  \author Andreas Horneffer

  \todo This file does not compile with Darwin, since <tt>sys/io.h</tt> is
  not available; can this be solved using the AIPS++/CASA I/O classes?
 */

struct timeval settime,remtime;
int deadhisto[HISTBINS];


Bool setall (GlishSysEvent &event)
{
  GlishSysEventSource *glishBus = event.glishSource();

  outb(255,0x378);
  gettimeofday(&settime, NULL);
  
  if (glishBus->replyPending()) {
    glishBus->reply(GlishArray(True));
  };
  return True;
}

Bool remall (GlishSysEvent &event)
{
  GlishSysEventSource *glishBus = event.glishSource();
  int tdiff (0);
  
  outb(0,0x378);
  gettimeofday(&remtime, NULL);
  
  if (glishBus->replyPending()) {
    glishBus->reply(GlishArray(True));
  };
  
  tdiff = (remtime.tv_sec-settime.tv_sec)*1000000+
    remtime.tv_usec-settime.tv_usec;
  tdiff /= 100000; //make the bins 100ms wide
  if (tdiff<0) {
    tdiff =0;
  }
  if (tdiff>(HISTBINS-1)) {
    tdiff =(HISTBINS-1);
  } 
  deadhisto[tdiff]++;
  
  return True;
}

Bool gensync(GlishSysEvent &event)
{
  GlishSysEventSource *glishBus = event.glishSource();
  
  outb(255,0x378);
  usleep(100000);
  outb(0,0x378);
  
  if (glishBus->replyPending()) {
    glishBus->reply(GlishArray(True));
  };
  return True;
}


int main (int argc, char *argv[])
{
  fd_set rfds;
  struct timeval tv;
  Bool running (True);
  GlishSysEventSource gsrc(argc,argv);
  GlishSysEventTarget gensync(gensync);
  GlishSysEventTarget setall(setall);
  GlishSysEventTarget remall(remall);
  SysEvent event;
  int retval (0);
  FILE *fd;


  if (ioperm(0x378, 3, 1)) {
    cerr << "[ppsync] Can't get I/O permissions" << endl; 
    return -1;
  };

  for (retval=0; retval < HISTBINS; retval++) {
    deadhisto[retval] = 0;
  };

  while (running) {  
    while ((! gsrc.waitingEvent()) && gsrc.connected()) { 
      FD_ZERO(&rfds);
      gsrc.addInputMask(&rfds);
      tv.tv_sec = 5;tv.tv_usec = 0;
      retval = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);
      //cout << "select returned:" << retval <<endl;
      //sched_yield(); 
    };
    if (! gsrc.connected()) running = False;
    if (gsrc.waitingEvent()) {        
      event = gsrc.nextEvent();
      if (event.type() == "shutdown") {
	running = False;
      } else if (event.type() == "gensync") {
	event.dispatch(gensync);           
      } else if (event.type() == "setall") {
	event.dispatch(setall);           
      } else if (event.type() == "remall") {
	event.dispatch(remall);           
      };
    } else { cout << "no waiting event!" << endl; };
  };

  ioperm(0x378,3,0);

  fd = fopen("/data/deadhisto.tab","w");
  if (fd != NULL) {
    for (retval=0; retval < HISTBINS; retval++) {
      fprintf(fd,"%d    %d \n",retval,deadhisto[retval]);
    };
    fclose(fd);
  };

  return 0;
}

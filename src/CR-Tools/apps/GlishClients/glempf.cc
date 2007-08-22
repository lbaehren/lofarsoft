/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Andreas Horneffer ()                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sched.h>

#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>

#include <ApplicationSupport/Glish.h>

#include <casa/namespace.h>

int recieveblocks,recievedblocks;
struct timeval starttime;

Bool getdata(GlishSysEvent &event) {
   GlishSysEventSource *glishBus = event.glishSource();
   int numblocks;

   GlishArray gtmp = event.val(); 
   gtmp.get(numblocks);

   if ((numblocks<0) || (numblocks>2097152)) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   recieveblocks = numblocks;
   recievedblocks = 0;

   if (glishBus->replyPending()) {
     glishBus->reply(GlishArray(True));
   };
   return True;
}
 



Bool glishempf(GlishSysEvent &event) {
   GlishSysEventSource *glishBus = event.glishSource();
   struct timeval tv;


   if (event.val().type() != GlishValue::ARRAY) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   GlishArray gtmp = event.val();
   Vector<short int> data;
   gtmp.get(data);
   if (data(1) != recieveblocks) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   if (data(0) == 1) {
     gettimeofday(&starttime,NULL);
     //cout << starttime.tv_sec <<"  "<< starttime.tv_usec <<endl;
   };     
   if (data(0) == recieveblocks) {
     gettimeofday(&tv,NULL);
     //cout << tv.tv_sec << "  "<< tv.tv_usec <<endl;
     //cout << starttime.tv_sec <<"  "<< starttime.tv_usec <<endl;
     gtmp = (float)((tv.tv_sec-starttime.tv_sec)+((0.+tv.tv_usec-starttime.tv_usec)/1000000));
     glishBus->postEvent("empferg",gtmp);
   };
   
   if (glishBus->replyPending()) {
     glishBus->reply(GlishArray(True));
   };
   return True;
}


int main(int argc, char *argv[]) {
  Bool running = True;
  GlishSysEventSource gsrc(argc,argv);
  GlishSysEventTarget getdata(getdata);
  GlishSysEventTarget glishempf(glishempf);
  SysEvent event;

  recieveblocks = recievedblocks =0;

  while (running) {  
    while (! gsrc.waitingEvent()) { sched_yield(); };
    if (gsrc.waitingEvent()) {        
      event = gsrc.nextEvent();
      //cout << "glempf: event.type " << event.type() << endl;
      if (event.type() == "getdata") {
	event.dispatch(getdata);           
      } else if (event.type() == "glishempf") {
	event.dispatch(glishempf);           
      } else if (event.type() == "shutdown") {
	running = False;
      };
    } else { cout << "no waiting event!" << endl;};
  };
}

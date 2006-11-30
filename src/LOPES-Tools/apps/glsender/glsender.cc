
/* $Id: glsender.cc,v 1.1 2005/06/29 14:54:16 bahren Exp $ */

#include <stdio.h>
#include <iostream.h>
#include <unistd.h>

#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <tasking/Glish.h>

#include <casa/namespace.h>

//global vars
short int *datablock;

//-----------------------------------------------------------------------
//Name: meanfilter
//
//Type: Event Callback Function
//Doc: apply a meanvalue filter to the data
//ret-data: GlishArray with the data
//inp-data: filtrec =    Record with the data
//            filtrec.ary      = data to be filtered
//            filtrec.strength = strength of the filter (default =3)
//-----------------------------------------------------------------------

Bool meanfilter(GlishSysEvent &event) {
   GlishSysEventSource *glishBus = event.glishSource();
   int i;

   if (event.val().type() != GlishValue::RECORD) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   GlishRecord filtrec = event.val();
   // get data; mandatory!
   if (! filtrec.exists("ary") ) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   GlishArray gtmp = filtrec.get("ary");
   Vector<Double> ary;
   gtmp.get(ary);
   int size = ary.nelements();
   int fs=3;
   if ( filtrec.exists("strength") ) {
     gtmp = filtrec.get("strength");
     gtmp.get(fs);
     if ((fs < 1) ||(fs > size)) {fs=1;};
   };
   Matrix<Double> mary((size+fs),fs);
   for (i=0; i<fs; i++) {
     mary(Slice(i,size),Slice(i)) = ary;
   };
   

   GlishArray retval;
   //retval = partialMeans(mary,IPosition(1,1))(IPosition(1,(fs/2)),IPosition(1,(size+fs/2)));
   for (i=0; i<size; i++) {
     ary(i) = mean(mary.row(i+fs/2));
   };
   retval = ary;
    

   if (glishBus->replyPending()) {
     glishBus->reply(retval);
   };
   return True;   
}



Bool glishsender(GlishSysEvent &event) {
   GlishSysEventSource *glishBus = event.glishSource();
   int i,numblocks;


   
   GlishArray gtmp = event.val(); 
   gtmp.get(numblocks);
   datablock[1] = numblocks;
   datablock[0] = 0;

   if ((numblocks<0) || (numblocks>2097152)) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };

   Vector<short int> vtmp;
   //Vector<short int> vtmp(IPosition(1,(512*1024)), datablock,SHARE);
   //gtmp = vtmp;
   for (i=0; i<numblocks; i++) {
     datablock[0]++;
     
     vtmp = Vector<short int>(IPosition(1,512*80), datablock,SHARE); gtmp = vtmp;
     glishBus->postEvent("gsenderdata",gtmp);
   };
   if (glishBus->replyPending()) {
     glishBus->reply(GlishArray(True));
   };

   return True;
}


int main(int argc, char *argv[]) {
  Bool running = True;
  GlishSysEventSource gsrc(argc,argv);
  GlishSysEventTarget meantarg(meanfilter);
  GlishSysEventTarget glishsender(glishsender);
  SysEvent event;
  int i;

  datablock = (short int *)malloc(1024*80); //Get 1kB of data
  for (i=0;i<10;i++) {datablock[i] = i;};

  while (running) {  
    while (! gsrc.waitingEvent()) { sleep(1); };
    if (gsrc.waitingEvent()) {        
      event = gsrc.nextEvent();
      //cout << "glsender: event.type " << event.type() << endl;
      if (event.type() == "meanfilter") {
	event.dispatch(meantarg);           
      } else if (event.type() == "glishsender") {
	event.dispatch(glishsender);           
      } else if (event.type() == "shutdown") {
	running = False;
      };
    } else { cout << "no waiting event!" << endl;};
  };
}


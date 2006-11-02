
// C++ Standard library
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sched.h>

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Quanta/QC.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <tasking/Glish.h>

// Custom header files
#include <timu.h>

#define DEBUG 21

#include <casa/namespace.h>

TIM *board = NULL;
unsigned long syncaddress=1,readaddress=1;
short *tmppoint = NULL; // For i386 Linux. May be different on other machines.

#define MAXBLOCKSIZE 24*1024*1024 // 96 MB temporary buffer!

void expand_sign(short *in, unsigned long  size) {
  short  *tmp=in;
  for (;tmp<(in+size);tmp++) {
    if ((*tmp)&0x0800) { *tmp |= 0xf800; 
    } else {
      *tmp &= 0x0fff;
    };
  };
};

Bool getdata(GlishSysEvent &event) {
   GlishSysEventSource *glishBus = event.glishSource();
   int blocklen;

   syncaddress=1;
   if (board == NULL) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   
   GlishArray gtmp = event.val(); 
   gtmp.get(blocklen);
   if ((blocklen>MAXBLOCKSIZE) || (blocklen<0)) { blocklen = MAXBLOCKSIZE;};

   if (DEBUG>20)
     cout << "Starting write to the memory..." << endl;
   board->Modereg(  (board->Modereg() & 0xf0) | 0x4);
   board->Segmentreg(0x0);
   board->write(0x0, (void*) &blocklen, 4);
   usleep(100);
   if (DEBUG>20)
     cout << "Waiting for write to finish" <<endl;;
   board->read(0x0,tmppoint,blocklen*sizeof(*tmppoint) );
   board->read((1024*1024*1024UL),tmppoint+blocklen,blocklen*sizeof(*tmppoint) );
   expand_sign(tmppoint,blocklen*2);
   if (DEBUG>20)
     cout << "sending data" << endl;
   if (glishBus->replyPending()) {
     glishBus->reply(GlishArray(Array<short>(IPosition(2,blocklen,2),tmppoint,SHARE)));
   };
   syncaddress = 0;
   readaddress = blocklen*sizeof(*tmppoint);
   if (DEBUG>15)
     cout << "done with getdata" << endl;
   return True;
}

Bool getdata_sync_neu(GlishSysEvent &event) {
   GlishSysEventSource *glishBus = event.glishSource();
   GlishArray gtmp; 
   int blocklen,presync=0;
//   int i, diff;
//   unsigned long fpgaddress, mastertic, pciaddress;
   short *tmpbuffpoint = tmppoint+(2*MAXBLOCKSIZE-32*1024*1024/sizeof(*tmppoint));
   unsigned long startaddress, tsize;

   syncaddress=1;
   if (board == NULL) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };

   if (event.val().type() != GlishValue::RECORD) {
     gtmp = event.val();
     gtmp.get(blocklen);
   } else {
     GlishRecord getrec = event.val();   
     if (! getrec.exists("blocklen") ) {
       if (glishBus->replyPending()) {
	 glishBus->reply(GlishArray(False));
       };
       return True;
     };
     gtmp = getrec.get("blocklen");
     gtmp.get(blocklen);
     if ( getrec.exists("presync") ) {
       gtmp = getrec.get("presync");
       gtmp.get(presync);
     };
   };
   if ((blocklen>(8*1024*1024)) || (blocklen<0)) { 
     blocklen = (8*1024*1024);
     cout << "getdata_sync_neu: blocklen truncated to 8 MegaSamples" <<endl;;
   };

   board->Modereg(  (board->Modereg() & 0xf0) | 0x6);
   board->Segmentreg(0x0);
   board->write(0x0, (void*) &blocklen, 4);
   usleep(100);
   if (DEBUG>20)
     cout << "getdata_sync_neu: Waiting for write to finish" <<endl;;
   if (glishBus->replyPending()) {
     glishBus->reply(GlishArray(True));
   };
   board->Clockreg(0x0); // wait for write to finish
   syncaddress=board->Syncaddress();
   startaddress=syncaddress-presync*sizeof(*tmppoint);
   startaddress=startaddress & 0x3fffffff;
   tsize = blocklen*sizeof(*tmppoint);

   readaddress = startaddress & 0x3f000000;
   board->read( readaddress , tmpbuffpoint, 0x1000000  );
   if ((startaddress-readaddress)>(0x1000000-tsize)){
     board->read( (readaddress+0x1000000) & 0x3f000000, 
		  tmpbuffpoint+(0x1000000/sizeof(*tmpbuffpoint)), 0x1000000  );
   };
   memcpy(tmppoint,
	  tmpbuffpoint+(startaddress-readaddress)/sizeof(*tmpbuffpoint),tsize);
   startaddress |= 0x40000000;
   readaddress = startaddress & 0xff000000;
   board->read( readaddress , tmpbuffpoint, 0x1000000  );
   if ((startaddress-readaddress)>(0x1000000-tsize)){
     board->read( (readaddress+0x1000000) | 0x40000000, 
		  tmpbuffpoint+(0x1000000/sizeof(*tmpbuffpoint)), 0x1000000  );

   };
   memcpy(tmppoint+blocklen,
	  tmpbuffpoint+(startaddress-readaddress)/sizeof(*tmpbuffpoint),tsize);

   expand_sign(tmppoint,blocklen*2);
   glishBus->postEvent("newdata",GlishArray(Array<short>(IPosition(2,blocklen,2),tmppoint,SHARE)));
   if (DEBUG>15)
     cout << "getdata_sync_neu: done" << endl;
   return True;
}

Bool getdata_sync(GlishSysEvent &event) {
   GlishSysEventSource *glishBus = event.glishSource();
   GlishArray gtmp; 
   int blocklen,presync=0;
//   int i, diff;
//   unsigned long fpgaddress, mastertic, pciaddress;
   unsigned long startaddress, tsize;

   syncaddress=1;
   if (board == NULL) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };

   if (event.val().type() != GlishValue::RECORD) {
     gtmp = event.val();
     gtmp.get(blocklen);
   } else {
     GlishRecord getrec = event.val();   
     if (! getrec.exists("blocklen") ) {
       if (glishBus->replyPending()) {
	 glishBus->reply(GlishArray(False));
       };
       return True;
     };
     gtmp = getrec.get("blocklen");
     gtmp.get(blocklen);
     if ( getrec.exists("presync") ) {
       gtmp = getrec.get("presync");
       gtmp.get(presync);
     };
   };
   if ((blocklen>MAXBLOCKSIZE) || (blocklen<0)) { blocklen = MAXBLOCKSIZE;};

   if (DEBUG>20)
     cout << "Starting write to the memory..." << endl;
   board->Modereg(  (board->Modereg() & 0xf0) | 0x6);
   board->Segmentreg(0x0);
   board->write(0x0, (void*) &blocklen, 4);
   usleep(100);
   if (DEBUG>20)
     cout << "Waiting for write to finish" <<endl;;
   if (glishBus->replyPending()) {
     glishBus->reply(GlishArray(True));
   };
   board->Clockreg(0x0); // wait for write to finish
   syncaddress=board->Syncaddress();
   startaddress=syncaddress-presync*sizeof(*tmppoint);
   startaddress=startaddress & 0x3fffffff;
   tsize = blocklen*sizeof(*tmppoint);
   if (startaddress+tsize <= (1024*1024*1024UL)) {
     board->read( startaddress , tmppoint, tsize );
   } else {
     tsize = (1024*1024*1024UL)-startaddress;
     board->read( startaddress , tmppoint, tsize );
     board->read( 0x0 , tmppoint+tsize/sizeof(*tmppoint), blocklen*sizeof(*tmppoint)-tsize );
   };
   readaddress = (startaddress+blocklen*sizeof(*tmppoint))& 0x3fffffff;
   startaddress += (1024*1024*1024UL);
   board->read( startaddress, tmppoint+blocklen, tsize );
   if (tsize < blocklen*sizeof(*tmppoint)) {
     board->read( (1024*1024*1024UL) , tmppoint+blocklen+tsize/sizeof(*tmppoint), blocklen*sizeof(*tmppoint)-tsize );
   };
   expand_sign(tmppoint,blocklen*2);
   if (DEBUG>20)
     cout << "sending data" << endl;
   glishBus->postEvent("newdata",GlishArray(Array<short>(IPosition(2,blocklen,2),tmppoint,SHARE)));
   if (DEBUG>15)
     cout << "done with getdata_sync" << endl;
   return True;
}

Bool get_next_data(GlishSysEvent &event) {
   GlishSysEventSource *glishBus = event.glishSource();
   GlishArray gtmp; 
   int blocklen,presync=0x80000000;
   unsigned long startaddress;

   if ((board == NULL)||(syncaddress%2)) {
     if (syncaddress%2) cerr<< "Invalid syncaddress, start read before getting data" << endl;
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };

   if (event.val().type() != GlishValue::RECORD) {
     gtmp = event.val();
     gtmp.get(blocklen);
   } else {
     GlishRecord getrec = event.val();   
     if (! getrec.exists("blocklen") ) {
       if (glishBus->replyPending()) {
	 glishBus->reply(GlishArray(False));
       };
       return True;
     };
     gtmp = getrec.get("blocklen");
     gtmp.get(blocklen);
     if ( getrec.exists("presync") ) {
       gtmp = getrec.get("presync");
       gtmp.get(presync);
     };
     if ( getrec.exists("channel") ) {
       gtmp = getrec.get("channel");
       gtmp.get(presync);
     };
   };
   if ((blocklen>MAXBLOCKSIZE) || (blocklen<0)) { blocklen = MAXBLOCKSIZE;};

   if ((unsigned)presync==0x80000000) {
     startaddress=readaddress;
   } else { 
     startaddress=syncaddress-presync*sizeof(*tmppoint);
   };
   startaddress=startaddress & 0x3fffffff;
   board->read( startaddress , tmppoint, blocklen*sizeof(*tmppoint) );
   readaddress = (startaddress+blocklen*sizeof(*tmppoint))& 0x3fffffff;
   startaddress += (1024*1024*1024UL);
   board->read( startaddress, tmppoint+blocklen, blocklen*sizeof(*tmppoint) );
   expand_sign(tmppoint,blocklen*2);
   if (glishBus->replyPending()) {
     glishBus->reply(GlishArray(Array<short>(IPosition(2,blocklen,2),tmppoint,SHARE)));
   };
   return True;
};
 

// #define COPYSIZE 16*1024*1024
#define COPYSIZE 128*1024
#define FILESIZE  1024*1024*1024

int board2files(TIM *board, FILE **fds, unsigned long blocksize){
  unsigned long startaddress, readaddress;
  unsigned long xfered=0,size;

  if ((fileno(fds[0]) == -1) || (fileno(fds[1]) == -1)) {
    if (DEBUG>10)
      cout << "board2file: Invalid file descriptor! Aborting" << endl;
    return -1;
  };
  startaddress=board->Syncaddress();
  startaddress=startaddress & 0x3fffffff;
  if (DEBUG>20)
    cout << "Writing channel 1" << endl;
  readaddress=startaddress;
  size=(COPYSIZE)-(readaddress%(COPYSIZE));
  if (size > blocksize) size=blocksize;
  board->read( readaddress , tmppoint, size );
  expand_sign(tmppoint,size/2);
  fwrite(tmppoint, 1, size, fds[0]);
  xfered = size;
  readaddress+=size;
  while (xfered < blocksize){
    if ((xfered+COPYSIZE) > blocksize) {
      size = blocksize-xfered;
    } else {
      size = COPYSIZE;
    };
    board->read( readaddress , tmppoint, size);
    expand_sign(tmppoint,size/2);
    fwrite(tmppoint, 1, size, fds[0]);
    xfered += size;
    readaddress+=size;
    readaddress =  readaddress & 0x3fffffff; // don't cross DQM-boundary!
  }
  if (DEBUG>20)
    cout << "Writing channel 2" << endl;
  readaddress=startaddress+ (1024*1024*1024UL);
  size=(COPYSIZE)-(readaddress%(COPYSIZE));
  if (size > blocksize) size=blocksize;
  board->read( readaddress , tmppoint, size );
  expand_sign(tmppoint,size/2);
  fwrite(tmppoint, 1, size, fds[1]);
  xfered = size;
  readaddress+=size;
  while (xfered < blocksize){
    if ((xfered+COPYSIZE) > blocksize) {
      size = blocksize-xfered;
    } else {
      size = COPYSIZE;
    };
    board->read( readaddress , tmppoint, size);
    expand_sign(tmppoint,size/2);
    fwrite(tmppoint, 1, size, fds[1]);
    xfered += size;
    readaddress+=size;
    readaddress =  (readaddress & 0x3fffffff)+ (1024*1024*1024UL);
  }
  return 1;
};


Bool data2disk(GlishSysEvent &event) {
  GlishSysEventSource *glishBus = event.glishSource();
  
  int i;
  GlishArray gtmp = event.val(); 
  const char *filename;
  Vector<String> tmpStrings;
  FILE *fds[2];
  
  gtmp.get(tmpStrings);
  for (i=0;i<2;i++) {
    filename = tmpStrings(i).c_str(); 
    fds[i] = fopen(filename,"wb");
    if (fds[i]==NULL) {
      if (DEBUG>20)
	cout<<"data2disk: Canot open file "<<filename <<" for writing"<<endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      }; 
      return True;
    };
  };
  
  if (DEBUG>20)
    cout << "data2disk: Starting write to the memory..." << endl;
  board->Modereg(  (board->Modereg() & 0xf0) | 0x6);
  board->Segmentreg(0x0);
  board->write(0x0, (void*) fds, 4);
  usleep(100);
  if (DEBUG>20)
    cout << "data2disk: Waiting for write to finish" <<endl;;
  if (glishBus->replyPending()) {
    glishBus->reply(GlishArray(True));
  };
  board->Clockreg(0x0); // wait for write to finish

  if (board2files(board, fds, FILESIZE) > 0) {
    if (DEBUG>20)
      cout << "data2disk: data saved" << endl;
    glishBus->postEvent("todisk_done",True);
  } else {
    if (DEBUG>20)
      cout << "data2disk: Error saving the data!" << endl;
    glishBus->postEvent("todisk_done",False);
  };
  fclose(fds[0]); fclose(fds[1]);
  if (DEBUG>15)
    cout << "data2disk: done with data2disk" << endl;
  return True;
};


Bool cardinit(GlishSysEvent &event) {
   GlishSysEventSource *glishBus = event.glishSource();

   GlishArray gtmp = event.val(); 
   String tmpString;
   gtmp.get(tmpString);
   const char *filename = tmpString.c_str();

   if (board != NULL) { delete board; };

   board = new TIM(filename);

   if (board == NULL) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
   };
     
   };

   board->Initram();
   usleep(100);
   board->prefcnt(0);
   board->Clockreg(0x0);
   board->Segmentreg(0x0);
   board->Modereg(0x94);

   if (0) { //an error has occured? (currently not implemented)
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };

   if (glishBus->replyPending()) {
     glishBus->reply(GlishArray(True));
   };
   return True;
}

Bool setconf(GlishSysEvent &event) {
   GlishSysEventSource *glishBus = event.glishSource();
   GlishRecord  timreg = event.val(); 
   GlishArray gtmp;
   int regdata;

   if (board == NULL) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };

   // clock-register
   if ( timreg.exists("clockreg") ) {
     gtmp = timreg.get("clockreg");
     gtmp.get(regdata);
     board->Clockreg(regdata);
   };
   // segment-register
   if ( timreg.exists("segmentreg") ) {
     gtmp = timreg.get("segmentreg");
     gtmp.get(regdata);
     board->Segmentreg(regdata);
   };
   // control-register
   if ( timreg.exists("controlreg") ) {
     gtmp = timreg.get("controlreg");
     gtmp.get(regdata);
     board->Segmentreg(regdata);
   };
   // Mode-register
   if ( timreg.exists("modereg") ) {
     gtmp = timreg.get("modereg");
     gtmp.get(regdata);
     board->Modereg(regdata);
   };
   // Blocklegth
   if ( timreg.exists("blocklen") ) {
     gtmp = timreg.get("blocklen");
     gtmp.get(regdata);
     board->Blocklength(TIM::blocklength(regdata));
   };

   if (glishBus->replyPending()) {
     glishBus->reply(GlishArray(True));
   };
   return True;
}

Bool getconf(GlishSysEvent &event) {
   GlishSysEventSource *glishBus = event.glishSource();
   GlishRecord erg;

   if (board == NULL) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };

   erg.add("ModeRegister",board->Modereg() );
   erg.add("StatusRegister",board->Controlreg() );
   erg.add("SegmentRegister",board->Segmentreg() );
   erg.add("ClockRegister",board->Clockreg() );

   if (glishBus->replyPending()) {
     glishBus->reply(erg);
   };
   return True;
}




int main(int argc, char *argv[]) {
  fd_set rfds;
  struct timeval tv;
  Bool running = True;
  GlishSysEventSource gsrc(argc,argv);
  GlishSysEventTarget getdata(getdata);
  GlishSysEventTarget getdata_sync(getdata_sync);
  GlishSysEventTarget getdata_sync_neu(getdata_sync_neu);
  GlishSysEventTarget cardinit(cardinit);
  GlishSysEventTarget setconf(setconf);
  GlishSysEventTarget getconf(getconf);
  GlishSysEventTarget get_next_data(get_next_data);
  GlishSysEventTarget data2disk(data2disk);
  SysEvent event;
  int retval;

  tmppoint = (short *)malloc(2*MAXBLOCKSIZE* sizeof(*tmppoint) );
  if (tmppoint == NULL ) {
    cerr << "Could not get temporary buffer! Aborting" << endl;
    gsrc.postEvent("done",GlishArray(False));
    return 1;
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
      } else if (event.type() == "getdata") {
	event.dispatch(getdata);           
      } else if (event.type() == "getdata_sync") {
	event.dispatch(getdata_sync);           
      } else if (event.type() == "cardinit") {
	event.dispatch(cardinit);           
      } else if (event.type() == "setconf") {
	event.dispatch(setconf);           
      } else if (event.type() == "getconf") {
	event.dispatch(getconf);           
      } else if (event.type() == "getnextdata") {
	event.dispatch(get_next_data);           
      } else if (event.type() == "data2disk") {
	event.dispatch(data2disk);    
      } else if (event.type() == "getdata_sync_neu") {
	event.dispatch(getdata_sync_neu);           
      } else {  //  got an unrecognized event.
	gsrc.unrecognized(); 
      };
    } else { cout << "no waiting event!" << endl; };
  };
  return 0;
}



/*!
  \file eventsaveclient.cc
  
  \brief Save/Read a LOPES-event to/from disk
  
  \todo Implement the possibility to send a LOPES-event to the
  KASCADE-eventduilder if required
*/

/* $Id: eventsaveclient.cc,v 1.3 2006/12/20 13:00:50 horneff Exp $ */

// Standard includes
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sched.h>

// AIPS++/CASA includes
#include <casa/string.h>
#include <casa/Arrays.h>

// custom includes
#include <ApplicationSupport/Glish.h>
#include <Data/lopesevent_def.h>

// #define DOSOCKET
#ifdef DOSOCKET
#include "eventsaveclient-socket.cc"
#endif

#include <casa/namespace.h>

lopesevent *tmpevent;

//------------------------------------------------------------------------------

/*!
  \fn Bool readevent(GlishSysEvent &event)

  \brief Event Callback Function to read event data from disk

  \param evdesc          = Record describing the file:
  \param evdesc.filename = Name of the file to be read [String]
  \param evdesc.readall  = return all data [Boolean]

  \retval eventrec            = Record describing the file:
  \retval eventrec.filename   = Name of the file [String]
  \retval eventrec.(JDR|TL)   = KASCADE-style timestamp
  \retval eventrec.LTL        = 40MHz timestamp from menable card
  \retval eventrec.observatory = Observatory number (empty or 0 LOPES, 1 LORUN, etc...)
  \retval eventrec.evclass    = cosmic / simulation / solar-flare etc.
  \retval eventrec.blocksize  = size of one channel (number of shorts) 
  \retval eventrec.presync    = first entry is presync before the sync signal
  \retval eventrec.chanids    = array with the channel-IDs
  \retval eventrec.ch(n)      = record with the channel data
  \retval eventrec.ch(n).ID   = channel ID
  \retval eventrec.ch(n).data = channel data

  \return True or False
 */
Bool readevent(GlishSysEvent &event)
{
  GlishSysEventSource *glishBus = event.glishSource();
  unsigned int i;
  unsigned int chno;
  unsigned int chlen;
  char tmpstr[10];

  if (event.val().type() != GlishValue::RECORD) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  GlishRecord evdesc = event.val();
  // get filename; mandatory!
  if (! (evdesc.exists("filename") && evdesc.exists("readall")) ) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  GlishArray gtmp = evdesc.get("filename");
  String tmpString;
  gtmp.get(tmpString);
  const char *filename = tmpString.c_str();
  FILE *fd = fopen(filename,"r");
  if (fd == NULL) {
    cout << "Can't open file :" << filename << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  Bool readall;
  gtmp = evdesc.get("readall");
  gtmp.get(readall);
  GlishRecord eventrec;
  tmpevent->length = 0;
  if (readall) {
    i = fread(tmpevent, 1, sizeof(lopesevent), fd);
    if ( (i < 48) || (i != tmpevent->length)  ||
	 (tmpevent->type != TIM40) ) {
      cout << "readevent: Inconsitent file " << filename << endl;
      if (i==sizeof(lopesevent)) {
	cout << "readevent: File " << filename << " too large!" << endl;
      };
    };
    // same for all versions?!?
    eventrec.add("filename",evdesc.get("filename"));
    eventrec.add("version",(int)tmpevent->version);
    eventrec.add("type",(int)tmpevent->type);
    eventrec.add("JDR",(int)tmpevent->JDR).add("TL",(int)tmpevent->TL);
    //New field since file format version 2
    if (tmpevent->version > 1) eventrec.add("LTL",(int)tmpevent->LTL);
    eventrec.add("evclass",(int)tmpevent->evclass);
    //New field in file format version 3
    if (tmpevent->version > 2) {
      eventrec.add("observatory",(int)tmpevent->observatory);
    };
    if (tmpevent->blocksize>0) 
      eventrec.add("blocksize",(int)tmpevent->blocksize);
    if (tmpevent->presync != 0) 
      eventrec.add("presync",tmpevent->presync);
    short int *datapoint, *endpoint;
    GlishRecord tmprec;
    datapoint = tmpevent->data;
    endpoint = (short *)tmpevent + tmpevent->length/sizeof(short);
    chno = 0;
    while ( datapoint < endpoint) {
      tmprec = GlishRecord(); //clear tmprec
      chno++;
      sprintf(tmpstr,"ch%i",chno);
      tmprec.add("ID",*((int *)datapoint));
      datapoint += sizeof(int)/sizeof(short);
      chlen = *((unsigned int *)datapoint);
      datapoint += sizeof(int)/sizeof(short);
      if (datapoint + chlen > endpoint) {
	cout << "readevent: Inconsitend data, channel " << tmpstr << "too large" <<endl;
	break;
      };
      tmprec.add("data",Vector<short>(IPosition(1,chlen),datapoint,SHARE));
      datapoint += chlen;
      eventrec.add(tmpstr,tmprec);
    };
    if (glishBus->replyPending()) {
      glishBus->reply(eventrec);
    };
  } else {
    cout << "Sorry not implemented jet! " << endl;
  };

  fclose(fd);
  return True;
};


//------------------------------------------------------------------------------


/*!
  \fn Bool writeSMevent(GlishSysEvent &event)

  \brief Event Callback Function 

  Writes a small event to a datafile (and send it to the xevb)

  \param eventrec           =    Record describing the file:
  \param eventrec.filename  = Name of the file to be written [String]
  \param eventrec.(JDR|TL)  = KASCADE-style timestamp
  \param eventrec.LTL       = 40MHz timestamp from menable card
  \param eventrec.observatory = Observatory number (empty or 0 LOPES, 1 LORUN, etc...)
  \param eventrec.evclass   = cosmic / simulation / solar-flare etc.
  \param eventrec.blocksize = size of one channel (number of shorts) (opt)
  \param eventrec.presync   = first entry is presync before the sync signal 
  \param eventrec.chanids   = array with the channel-IDs
  \param eventrec.data      = 2 dim array with the data (type short!)
  
  \return True or False

 */
Bool writeSMevent(GlishSysEvent &event)
{
  GlishSysEventSource *glishBus = event.glishSource();
  
  if (event.val().type() != GlishValue::RECORD) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  GlishRecord eventrec = event.val();
  // get filename; mandatory!
  if (! eventrec.exists("filename") ) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
     return True;
  };
  GlishArray gtmp = eventrec.get("filename");
  String tmpString;
  gtmp.get(tmpString);
  const char *filename = tmpString.c_str();
  FILE *fd = fopen(filename,"w");
  if (fd == NULL) {
    cout << "Can't open file :" << filename << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  
  Int tmpint;
  // get the JDR
  if (! eventrec.exists("JDR") ) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  gtmp = eventrec.get("JDR");
  if (! gtmp.get(tmpint,0) ) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  tmpevent->JDR = tmpint;
  // get the TL
  if (! eventrec.exists("TL") ) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  gtmp = eventrec.get("TL");
  if (! gtmp.get(tmpint,0) ) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  tmpevent->TL = tmpint;
  // get the LTL
  if (! eventrec.exists("LTL") ) {
    //No big problem, if the LTL is not set!
    tmpint = 0;
  };
  gtmp = eventrec.get("LTL");
  if (! gtmp.get(tmpint,0) ) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  tmpevent->LTL = tmpint;
  // get the Observatory
  if (! eventrec.exists("observatory") ) {
    //No big problem, if the observatory is not set, then it is LOPES.
    tmpevent->observatory = 0;
  } else {
    gtmp = eventrec.get("observatory");
    if (! gtmp.get(tmpint,0) ) {
      if (glishBus->replyPending()) {
        glishBus->reply(GlishArray(False));
      };
      return True;
    };
    tmpevent->observatory = tmpint;
  };
    // get the evclass
  if (! eventrec.exists("evclass") ) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  gtmp = eventrec.get("evclass");
  if (! gtmp.get(tmpint,0) ) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  tmpevent->evclass = tmpint;
  // get the blocksize
  if ( eventrec.exists("blocksize") ) {
    gtmp = eventrec.get("blocksize");
    gtmp.get(tmpint,0);
    tmpevent->blocksize = tmpint;    
  };
  // get the presync
  if ( eventrec.exists("presync") ) {
    gtmp = eventrec.get("presync");
    gtmp.get(tmpint,0);
    tmpevent->presync = tmpint;    
  };
  // set the event-type and version
  tmpevent->type = TIM40;
  tmpevent->version = VERSION;

  // get the channel-ids
  if (! eventrec.exists("chanids") ) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  gtmp = eventrec.get("chanids");
  Vector<Int> chanids;
  gtmp.get(chanids);
  // get the data
  if (! eventrec.exists("data") ) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  gtmp = eventrec.get("data");
  Matrix<Short> data;
  if (! gtmp.get(data) ) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  if (data.nrow() != chanids.nelements() ) {
    cerr << "writeSMevent: length of chanid and shape of data don't match!" << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  }
  unsigned int chan,i;
  short int *datapoint;
  datapoint = tmpevent->data;
  // copy that data into the temporary storage
  for (chan=0; chan < data.nrow(); chan++) {
    // Some dirty typecasting and pointer calculations! 
    // If you know a better way -> tell me!
    *((unsigned int *)datapoint) = chanids(chan);
    datapoint += sizeof(int)/sizeof(short);
    *((unsigned int *)datapoint) = data.ncolumn();
    datapoint += sizeof(int)/sizeof(short);
    for (i=0; i<data.ncolumn(); i++) {
      *datapoint = data(chan,i);
      datapoint++;
    };
  }; //for (chan=0;...)
  tmpevent->length = sizeof(lopesevent)-MAXDATASIZE+(datapoint-tmpevent->data)*sizeof(short);
// ************************************************************
// event-data is now completly copied to temporary storage.
// ************************************************************
  // returning to the glish-interpreter
  if (glishBus->replyPending()) {
    glishBus->reply(GlishArray(True));
  };
  // write to the file
  i = fwrite(tmpevent, 1, tmpevent->length, fd);
  fclose(fd); // close the file

#ifdef DOSOCKET
  //send the event to the xevb
  handle_xevb_event(tmpevent);
#endif
  
  return True;   
};


// --- Main routine ------------------------------------------------------------


int main(int argc, char *argv[]) {
  fd_set rfds;
  struct timeval tv;
  Bool running = True;
  GlishSysEventSource gsrc(argc,argv);
  GlishSysEventTarget writeSMevent(writeSMevent);
  GlishSysEventTarget readevent(readevent);
  SysEvent event;
  int retval;

  tmpevent = (lopesevent *)malloc(sizeof(lopesevent));
  if (tmpevent == NULL ) {
    cerr << "Could not get temporary buffer! Aborting" << endl;
    cerr << "Kill all other clients and try again." << endl;
    cerr << "If this doesn't work, then maybe you need to reboot. :-(" << endl;
    gsrc.postEvent("done",GlishArray(False));
    return 1;
  };

#ifdef DOSOCKET
  //open a socket, to that the xevb can connect
  retval = open_xevb_socket();
  if (retval <1) {
    cout << "xevb-socket not open! xevb will not be able to connect\n";
  };
#endif

  while (running) {  
    while ((! gsrc.waitingEvent()) && gsrc.connected()) { 
      FD_ZERO(&rfds);
      gsrc.addInputMask(&rfds);
      tv.tv_sec = 5;tv.tv_usec = 0;
      retval = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);
      //cout << "select returned:" << retval <<endl;
      //sched_yield(); 
#ifdef DOSOCKET
      //Without this we would only accept connections from the xevb
      //when we get a trigger, i.e. the array is running.
      check_xevb_connection();
#endif
    };
    if (! gsrc.connected()) running = False;
    if (gsrc.waitingEvent()) {        
      event = gsrc.nextEvent();
      if (event.type() == "shutdown") {
	running = False;
      } else if (event.type() == "writeSMevent") {
	event.dispatch(writeSMevent);           
      } else if (event.type() == "readevent") {
	event.dispatch(readevent);           
      } else {  //  got an unrecognized event.
	gsrc.unrecognized(); 
      };
    } else { cout << "no waiting event!" << endl; };
  };
  return 0;
}

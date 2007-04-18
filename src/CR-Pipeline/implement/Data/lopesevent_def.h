
/*!
  \file lopesevent.h

  \brief Include-file with the descriptions of the lopesevent datastructure

  <b> Also used by the eventsaveclient! Be really carefull before you change anything!</b>
*/

#ifndef LOPESEVENT_H
#define LOPESEVENT_H

#define lopesevent lopesevent_v1

//! Number of channels
#define NUMCHANNELS 30

//! About 80MB maximal eventsize
#define MAXDATASIZE 0x5001000
#define MAXSAMPLESIZE ((MAXDATASIZE-0x1000) / 2 / NUMCHANNELS)

// The header of a lopesevent is 12 (insigend) intergers long
#define LOPESEV_HEADERSIZE (12*sizeof(int))

#define VERS1 001
#define VERS2 002
#define VERS3 003
#define VERS4 004
#define VERSION VERS3

enum datatypes {TIM40 = 1, TIM80 = 2};
enum lopesclass {cosmic = 1,  simulation= 2, test=3, solar=4};

typedef struct {
  unsigned int length;  // length of the dataset (in bytes)
  unsigned int version;   
  unsigned int JDR,TL; // KASCADE-style timestamp
  unsigned int type;   // type of data. (currently only TIM-40 supported)
  unsigned int evclass;  // cosmic / simulation / solar-flare etc.
  unsigned int blocksize; // size of one channel (number of shorts)  (optional)
  int presync;   // first entry is presync before the sync signal (optional)
// This is the change from version 1 to version 2
  unsigned int LTL; // 40MHz timestamp from menable card
// This is the change from version 2 to version 3
  int observatory;  // ID of the observatory (0:LOPES; 1:LORUN)
  unsigned int frei[2];
  short int data[MAXDATASIZE/2]; // unstructured raw data with 
                                      // variable length
} lopesevent_v1;

typedef struct {
  unsigned int channid; // channel ID: pcnum*10000 + boardnum*100 + chnum
  unsigned int length;  // length of the data (number of shorts)
  short int *data;      // the data itself.
} channeldata;


// definition of transev 
// this is the structure, that will be sent to te KASCADE xevb
typedef struct {
  unsigned int length;   // length of the dataset (in bytes)
  unsigned int version;   
  unsigned int JDR,TL;   // KASCADE-style timestamp
  unsigned int type;     // type of data. (currently only TIM-40 supported)
  unsigned int evclass;  // cosmic / simulation / solar-flare etc.
} transev1;


// KASCADE-xevb stuff
#define LOPES_GRP   11 /* Wie in ?/kascadeinc/xevbdefines.h :  "GroupIndex" */
#define LOPES_EVENT  0 /* Wie in ?/kascadeinc/lopestags.h */
#define LOPES_PORT  14711 /* portnumber, for xevb from lopesdaq. */

#define PID_EVB_KERNEL            0x02
#define TID_EVB                   0x02
#define GID_EVB                   0x00

#endif /* LOPESEVENT_H */

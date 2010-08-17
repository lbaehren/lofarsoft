#ifndef __BPSR_DEF_H
#define __BPSR_DEF_H

/* default port to connect to udpdb command interface */
#define BPSR_DEFAULT_UDPDB_PORT 4001

#define BPSR_DEFAULT_PWC_PORT     51022
#define BPSR_DEFAULT_PWC_LOGPORT  51023

/* default port to connect to udpdb statistics interface */
#define BPSR_DEFAULT_UDPDB_STATS  51012

/* default port to connect to udpdb statistics interface */
#define BPSR_DEFAULT_DBAVG_LOG 40001

#define BPSR_DEFAULT_DBMON_LOG 40002

/* default port for UDP packets */
#define BPSR_DEFAULT_UDPDISK_PORT 4001 

/* default accumulation length for the iBob board */
#define BPSR_DEFAULT_ACC_LEN  25

/* Number of UDP packets to be recived for a called to buffer_function
 * 32 MB = 2048 * 16384 */
#define BPSR_NUM_UDP_PACKETS 16384

#define BPSR_TEST_TRIWAVEUDP_LOG 21342

#define BPSR_TEST_TRIWAVEUDP_COMMAND 12324


#endif


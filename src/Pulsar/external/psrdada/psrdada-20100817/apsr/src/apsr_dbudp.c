/***************************************************************************
 *
 *   Copyright (C) 2009 by Andrew Jameson 
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/***************************************************************************
 *
 *  Send data in a datablock out via UDP packets 
 * 
 ****************************************************************************/

#include "dada_client.h"
#include "dada_hdu.h"
#include "apsr_def.h"
#include "dada_def.h"
#include "apsr_udp.h"
#include "sock.h"

#include "node_array.h"
#include "string_array.h"
#include "ascii_header.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

#include <netinet/in.h>
#include <netdb.h> 
#include <sys/types.h>
#include <sys/socket.h>

/* 3rd party includes for StopWatch library */
#include "arch.h"
#include "Statistics.h"
#include "RealTime.h"
#include "StopWatch.h"

/* structures dbudp datatype  */
typedef struct{
  int verbose;                      /* verbosity flag */
  int expect_header;                /* to ingnore header in io_function*/
  int header_sent;                  /* to ingnore header in io_function*/
  int port;                         /* port to send udp data to */
  char *hostname;                   /* hostname to send udp data to */
  char *udpbuffer;                  /* for sending udp data */
  struct sockaddr_in udpsocket;     /* socket for sending udp data */
  header_struct header;             /* custom udp header */
  StopWatch *wait_sw;               /* StopWatch for packet throttling */
  int sequence_number;
  time_t current_time;
  time_t prev_time;
  uint64_t total_bytes_sent;        /* total bytes sent to udp socket */
  uint64_t prev_bytes_sent;         /* total bytes sent to udp socket */
  FILE* sockin;                     /* for feedback from control commands */
  FILE* sockout;                    /* for sending control commands */
  int udpdb_fd;                     /* file descriptor for control commands */
}dbudp_t;

int create_control_socket(dbudp_t *dbudp);
void get_line(FILE * sock, char * inmsg, int bufsize);

void usage()
{
  fprintf (stdout,
     "dada_dbudp [options] desthost\n"
     " -v         be verbose\n"
     " -p num     port to send udp data to [default %d]\n"
     " -1         1 transfer only\n\n"
     " desthost   udp packets sent to this hostname\n\n",
    APSR_DEFAULT_UDPDB_PORT);
}


/*! Function that opens the data transfer target */
int dbudp_open_function (dada_client_t* client)
{

  dbudp_t *dbudp = client->context;
  
  if (dbudp->verbose) fprintf(stderr,"dbudp_open_function\n");

  dbudp->expect_header = 1;
  /* status and error logging facility */
  multilog_t* log;

  /* the header */
  char* header = 0;

  assert (client != 0);

  log = client->log;
  assert (log != 0);

  header = client->header;
  assert (header != 0);

  if (!(dbudp->header_sent)) {
  
    /* Setup the fd and socket */
    client->fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (client->fd < 0) {  
      perror("udp socket");
      return -1;
    } 

    /* assign the ip address and port */ 
    struct in_addr *addr;
    dbudp->udpsocket.sin_family = AF_INET;           
    dbudp->udpsocket.sin_port = htons(dbudp->port);  
    addr = atoaddr(dbudp->hostname);
    dbudp->udpsocket.sin_addr.s_addr = addr->s_addr;
    bzero(&(dbudp->udpsocket.sin_zero), 8);          

    /* Setup udp header information */
    dbudp->header.length = (int) UDPHEADERSIZE;
    dbudp->header.source = (char) 0;    // Not sure what this would be
    dbudp->header.sequence = 0;
    dbudp->header.bits = 0;
    dbudp->header.channels = 0;
    dbudp->header.bands = 0;
    dbudp->header.bandID[0] = 0;
    dbudp->header.bandID[1] = 0;
    dbudp->header.bandID[2] = 0;
    dbudp->header.bandID[3] = 0;

    dbudp->sequence_number = 1;
  }
  return 0;
}

/*! Function that closes the data file */
int dbudp_close_function (dada_client_t* client, uint64_t bytes_written)
{

  /* status and error logging facility */
  multilog_t* log;

  dbudp_t *dbudp = client->context;

  assert (client != 0);

  log = client->log;
  assert (log != 0);

  if (bytes_written < client->transfer_bytes) {
    multilog (log, LOG_INFO, "Transfer stopped early at %"PRIu64" bytes\n",
	      bytes_written);
  }


  if (dbudp->verbose) fprintf(stderr,"Stopping dbudp transfer\n");

  /* close control and udp sockets */
  // TODO need fix for the header 
  //close(client->dbudp_fd);      
  //close(client->fd);

  //TODO proper messaging and return values 
  return 0;

}

/*! Pointer to the function that transfers data to/from the target */
int64_t dbudp_io_function (dada_client_t* client, 
			    void* data, uint64_t data_size)
{

  /* keep track of the data as we loop through it... */
  uint64_t data_pos = 0;
  uint64_t send_size = 0;
  size_t bytes_sent = 0;
  size_t bytes_to_send = 0;

  dbudp_t *dbudp = client->context;
  assert(client->fd >0);

  int verbose = dbudp->verbose;
  
  //if (verbose) fprintf(stderr,"dbudp_io_function: %"PRIu64"\n",
  //                             data_size);

  /* If we are asked to read the header, just ignore and return */
  if (dbudp->expect_header) {
    //if (verbose) fprintf(stderr,"dbudp_io_function: read the "
    //                            "header of %"PRIu64" bytes\n", data_size);
    dbudp->expect_header = 0;

    if (!(dbudp->header_sent)) {

      /* Now that we have the header, we must send the header information
       * to the reciever, so that they know about the observation information
       * we must also tell the receiver to enter the rec_start state so
       * that data is clocked into their Data Block */
   
      /* code ripped from telnecho.c */
      dbudp->udpdb_fd = create_control_socket(dbudp);

            
      int i = 0;
      char *tmpdata = (char *) data;

      if (verbose) fprintf(stderr,"header to send is:%s\n",tmpdata);

      while (tmpdata[i] != '\0') {
        if (tmpdata[i] == '\n') tmpdata[i] = '\\';
        i++;
      }
      
      int bufsize = 4096;
      char* inmsg = (char*) malloc ( bufsize);
      assert (inmsg != 0);

      get_line(dbudp->sockin, inmsg, bufsize);
      if (verbose) fprintf (stderr, "Read: '%s'\n", inmsg);

      if (verbose) fprintf(stderr, "Send: 'header %s'\n",tmpdata);
      fprintf(dbudp->sockout,"header %s\n",tmpdata);
      get_line(dbudp->sockin, inmsg, bufsize);
      if (verbose) fprintf (stderr, "Read: '%s'\n", inmsg);
    
      //sleep(1);

      if (verbose) fprintf(stderr,"Send: 'clock'\n");
      fprintf(dbudp->sockout,"clock\n");
      get_line(dbudp->sockin, inmsg, bufsize);
      if (verbose) fprintf (stderr, "Read: '%s'\n", inmsg);

      //sleep(1);
   
      time_t now = time(0); 
      unsigned buffer_size = 64;
      static char* buffer = 0;
      if (!buffer)
        buffer = malloc (buffer_size);
      assert (buffer != 0);
      strftime (buffer, buffer_size, DADA_TIMESTR, gmtime(&now));
      
      if (verbose) fprintf(stderr,"Send: 'rec_start %s'\n",buffer);
      fprintf(dbudp->sockout,"rec_start %s\n",buffer);
      get_line(dbudp->sockin, inmsg, bufsize);
      if (verbose) fprintf (stderr, "Read: '%s'\n", inmsg);

      sleep(1);
      
      dbudp->header_sent = 1;
    }
      
    data_pos = data_size;

  } else {

     /* if the data_size is larger than the default packet size, we must
      * break it up into multiple udp packets */ 

    send_size = DEFAULT_UDPDATASIZE;

    /* set the sequence number to 1 */
    dbudp->header.sequence = 1;

    while (((data_pos + send_size) <= data_size) && (send_size > 0)) {

      //fprintf(stderr,"while (%"PRIu64" <= %"PRIu64")\n",(data_pos+send_size),data_size);

      StopWatch_Start(dbudp->wait_sw);
            
      bytes_to_send = (size_t) (send_size+UDPHEADERSIZE);

      /* increment the sequence number */
      dbudp->header.sequence = dbudp->sequence_number;
      encode_header(dbudp->udpbuffer, &(dbudp->header));

      /* copy the data into the udpbuffer, keeping the header intact */
      memcpy(dbudp->udpbuffer+UDPHEADERSIZE,(data+data_pos),send_size);

      bytes_sent = sendto(client->fd,(void *) dbudp->udpbuffer, bytes_to_send,
                          0, (struct sockaddr *)&(dbudp->udpsocket),
                          sizeof(struct sockaddr));
      dbudp->total_bytes_sent += bytes_sent;

      if ((int) bytes_sent < 0) {
         fprintf(stderr,"Error with sendto\n");
         perror("sendto");
         multilog (client->log, LOG_ERR, "Error on sendto\n");
         return data_pos;
      } 

      if (bytes_sent < (send_size+UDPHEADERSIZE)) {
        multilog (client->log, LOG_ERR, "tried to send %"PRIu64" bytes, "
                  "but only %"PRIu64" were sent\n", (send_size+UDPHEADERSIZE), 
                  bytes_sent);
      }

      /* increment the data counter by the actual bytes sent */
      data_pos += (bytes_sent-UDPHEADERSIZE);

      if ((data_pos + DEFAULT_UDPDATASIZE) > data_size) {
        send_size = data_size - data_pos;
      } else {
        send_size = DEFAULT_UDPDATASIZE;
      }

      dbudp->prev_time = dbudp->current_time;
      dbudp->current_time = time(0);

      if (dbudp->prev_time != dbudp->current_time) {
        int bytes_per_second = dbudp->total_bytes_sent - dbudp->prev_bytes_sent;
        dbudp->prev_bytes_sent = dbudp->total_bytes_sent;
        float rate = (float)bytes_per_second / (1024*1024);
        fprintf(stderr,"%5.2f MB/s\n",rate);
      }
  
      /* 22 usecs ~= 64 MB/s for packets of ~1500 bytes */
      StopWatch_Delay(dbudp->wait_sw, 30.0); 

      dbudp->sequence_number++;
    }

  }    

  //fprintf(stderr,"sn end = %d\n",dbudp->sequence_number);

  assert(data_pos == data_size);
  return data_pos;

}


int main (int argc, char **argv)
{

  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;

  /* DADA Primary Read Client main loop */
  dada_client_t* client = 0;

  /* DADA Logger */
  multilog_t* log = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* Quit flag */
  char quit = 0;

  /* Port to send to*/
  int port = APSR_DEFAULT_UDPDB_PORT;

  int arg = 0;

  /* transfer n files flag */
  int onefile = 0;

  /* StopWatch */
  StopWatch stopwatch;

  /* actual struct with info */
  dbudp_t dbudp;

  while ((arg=getopt(argc,argv,"p:v1")) != -1)
    switch (arg) {
      
    case 'p':
      port=atoi(optarg);
      break;

    case 'v':
      verbose=1;
      break;
      
    case '1':
      onefile=1;
      break;
      
    default:
      usage ();
      return 0;
      
    }

  if ((argc - optind) != 1) {
    fprintf(stderr,"Error, a single hostname must be specified\n\n");
    usage();
    return EXIT_FAILURE;
  }
          
  dbudp.hostname = (char *) argv[optind];

  log = multilog_open ("dada_dbudp", 0);

  multilog_add (log, stderr);

  hdu = dada_hdu_create (log);

  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_lock_read (hdu) < 0)
    return EXIT_FAILURE;

  client = dada_client_create ();

  client->log = log;

  client->data_block = hdu->data_block;
  client->header_block = hdu->header_block;

  client->context        = &dbudp;
  client->open_function  = dbudp_open_function;
  client->io_function    = dbudp_io_function;
  client->close_function = dbudp_close_function;
  client->direction      = dada_client_reader;
  client->fd             = -1;

  client->transfer_bytes = 536870912;
  client->optimal_bytes = 1024*8*DEFAULT_UDPDATASIZE;
  
  dbudp.expect_header = 1;
  dbudp.header_sent = 0;
  dbudp.port = port;
  dbudp.verbose = verbose;
  dbudp.udpbuffer = malloc(sizeof(char)*(UDPHEADERSIZE+DEFAULT_UDPDATASIZE));
  dbudp.sequence_number = 1;
  dbudp.prev_time = time(0);
  dbudp.current_time = time(0);
  dbudp.total_bytes_sent = 0;
  dbudp.prev_bytes_sent = 0;

  /* Timer/Stopwatch initialise */
  int ret;
  int quiet = 1;
  ret = RealTime_Initialise(quiet);
  ret = StopWatch_Initialise(quiet);
  dbudp.wait_sw = &stopwatch;

  int bufsize = 4096;
  char* inmsg = (char*) malloc ( sizeof(char) * bufsize);
  assert (inmsg != 0);

  
  while (!quit) {
          
    if (dada_client_read (client) < 0)
      multilog (log, LOG_ERR, "Error during transfer\n");

    /* Need to tell udpdb that the data has been sent */

    //get_line(dbudp.sockin, inmsg, bufsize);
    //if (dbudp.verbose) fprintf(stderr, "Read: %s\n",inmsg);

  
    /* Return to idle state */
    if (dbudp.verbose) fprintf(stderr,"Send: 'stop'\n");
    fprintf(dbudp.sockout,"stop\n");
    get_line(dbudp.sockin, inmsg, bufsize);
    if (dbudp.verbose) fprintf(stderr, "Read: '%s'\n",inmsg);

    if (onefile)
      break;

    /* If we may send another, reinit */

    dbudp.header_sent = 0;
    dbudp.expect_header = 1;
    client->fd = -1;

  }

  close(dbudp.udpdb_fd);
  close(client->fd);

  if (dada_hdu_unlock_read (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

int create_control_socket(dbudp_t *dbudp) {

  /* code ripped from telnecho.c */

  dbudp->sockin = 0;
  dbudp->sockout = 0;

  int fd = -1;
  int sleep_time = 1;
  int port = DADA_DEFAULT_PWC_PORT;

  while (fd < 0) {

    if (dbudp->verbose)
      fprintf (stderr, "dbudp_create_control_socket: call sock_open (%s,%d)\n",
               dbudp->hostname, port);

    fd = sock_open(dbudp->hostname, port);

    if (fd < 0)  {
      fprintf (stderr, "dbudp_create_control_socket: Error connecting with "
                       "%s on %d\n\tsleeping %u seconds before retrying\n",
                       dbudp->hostname, port, sleep_time);
      sleep (sleep_time); 
    }
  }

  if (dbudp->verbose) 
    fprintf (stderr, "dbudp_create_control_socket: connected with %s\n", 
             dbudp->hostname);

  dbudp->sockin  = fdopen (fd, "r");
  dbudp->sockout = fdopen (fd, "w");

  /* dont buffer I/O */
  setbuf (dbudp->sockin, 0);
  setbuf (dbudp->sockout, 0);

  return fd;

}

void get_line(FILE * sock, char * inmsg, int bufsize) {

  char* rgot = NULL;
  rgot = fgets (inmsg, bufsize, sock);

  if (rgot && !feof(sock)) {
    inmsg[strlen(inmsg)-1] = '\0';
  }

}


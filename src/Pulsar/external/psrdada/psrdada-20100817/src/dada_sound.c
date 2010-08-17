#include "dada_client.h"
#include "dada_hdu.h"
#include "dada_def.h"

#include "disk_array.h"
#include "ascii_header.h"
#include "daemon.h"
#include "/home/pulsar/packages/linux/pgplot/cpgplot.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>

/* socket stuff */
#include <arpa/inet.h>     
#include <netinet/in.h>   
#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/stat.h>

/* socket stuff for audio */
int sock;         /* Socket for sending the data to audio server */
int stopSendingData;           /* Flag used if connection fails */
struct sockaddr_in audioserver;        /* The audio server to connect with */
char *ip_host=NULL;                      /* IP of audio server */
unsigned int portnr;                   /* port number of audio server */
int output_error=1;

void usage()
{
  fprintf (stdout,
	   "dada_disp [options]\n"
	   " -D <path>   add a disk to which data will be written\n"
	   " -H <host>   host to send avg data\n"
	   " -W          overwrite files\n"
	   " -P <port>   server port to connect\n"
	   " -d          run as daemon\n");
}

typedef struct {

  /* the set of disks to which data may be written */
  disk_array_t* array;

  /* a temporary data holder */
  float data[80000];

  /* variable to remember the array index*/
  int index;

  /* current observation id, as defined by OBS_ID attribute */
  char obs_id [DADA_OBS_ID_MAXLEN];

  /* current filename */
  char file_name [FILENAME_MAX];

  /* file offset from start of data, as defined by FILE_NUMBER attribute */
  unsigned file_number;
  
  /* pulse folding parameters */
  double P, phase_0;
  int band;

  /* folded data */
  double fold_data[4096];

} dada_disp_t;

#define DADA_DISP_INIT { 0, "", 0.0, 0, "", "", 0 }
#define xviewportSize 0.9
#define yviewportSize 0.9

/*! Function that opens the data transfer target */
int file_open_function (dada_client_t* client)
{
  /* the dada_disp specific data */
  dada_disp_t* disp = 0;
  
  /* status and error logging facility */
  multilog_t* log;

  /* the header */
  char* header = 0;

  /* observation id, as defined by OBS_ID attribute */
  char obs_id [DADA_OBS_ID_MAXLEN] = "";

  /* size of each file to be written in bytes, as determined by FILE_SIZE */
  uint64_t file_size = 0;

  /* the optimal buffer size for writing to file */
  uint64_t optimal_bytes = 0;

  /* the open file descriptor */
  int fd = -1;

  assert (client != 0);

  disp = (dada_disp_t*) client->context;
  assert (disp != 0);

  log = client->log;
  assert (log != 0);

  header = client->header;
  assert (header != 0);

  /* Get the observation ID */
  if (ascii_header_get (client->header, "OBS_ID", "%s", obs_id) != 1) {
    multilog (log, LOG_WARNING, "Header with no OBS_ID\n");
    strcpy (obs_id, "UNKNOWN");
  }

  /* check to see if we are still working with the same observation */
  if (strcmp (obs_id, disp->obs_id) != 0) {
    disp->file_number = 0;
    multilog (log, LOG_INFO, "New OBS_ID=%s -> file number=0\n", obs_id);
  }
  else {
    disp->file_number++;
    multilog (log, LOG_INFO, "Continue OBS_ID=%s -> file number=%lu\n",
	      obs_id, disp->file_number);
  }

  /* Set the file number to be written to the header */
  if (ascii_header_set (header, "FILE_NUMBER", "%u", disp->file_number)<0) {
    multilog (log, LOG_ERR, "Error writing FILE_NUMBER\n");
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "disp: copy the obs id\n");
#endif

  /* set the current observation id */
  strcpy (disp->obs_id, obs_id);

#ifdef _DEBUG
  fprintf (stderr, "disp: create the file name\n");
#endif

  /* create the current file name */
  snprintf (disp->file_name, FILENAME_MAX,
	    "%s.%06u.disp", obs_id, disp->file_number);

  /* Get the file size */
  if (ascii_header_get (header, "FILE_SIZE", "%"PRIu64, &file_size) != 1) {
    multilog (log, LOG_WARNING, "Header with no FILE_SIZE\n");
    file_size = DADA_DEFAULT_FILESIZE;
  }

#ifdef _DEBUG
  fprintf (stderr, "disp: open the file\n");
#endif

  /* Open the file */
  fd = disk_array_open (disp->array, disp->file_name,
			file_size, &optimal_bytes, 0);
  
  if (fd < 0) {
    multilog (log, LOG_ERR, "Error opening %s: %s\n", disp->file_name,
              strerror (errno));
    return -1;
  }

  /* clear index variable - displays can proceed as normal */
  disp->index = 0;
  
  multilog (log, LOG_INFO, "%s opened for writing %"PRIu64" bytes\n",
	    disp->file_name, file_size);

  client->fd = fd;
  client->transfer_bytes = file_size;
  client->optimal_bytes = 4000000;

  return 0;
}

/*! Function that closes the data file */
int file_close_function (dada_client_t* client, uint64_t bytes_written)
{
  /* the dada_disp specific data */
  dada_disp_t* disp = 0;

  /* status and error logging facility */
  multilog_t* log;

  assert (client != 0);

  disp = (dada_disp_t*) client->context;
  assert (disp != 0);

  log = client->log;
  assert (log != 0);

  /* dirty hack - write file, and indicate */
  write(client->fd,disp->data,80000*sizeof(float));

  if (close (client->fd) < 0)
    multilog (log, LOG_ERR, "Error closing %s: %s\n", 
	      disp->file_name, strerror(errno));

  if (!bytes_written)  {

    multilog (log, LOG_ERR, "Removing empty file: %s\n", disp->file_name);

    if (chmod (disp->file_name, S_IRWXU) < 0)
      multilog (log, LOG_ERR, "Error chmod (%s, rwx): %s\n", 
		disp->file_name, strerror(errno));
    
    if (remove (disp->file_name) < 0)
      multilog (log, LOG_ERR, "Error remove (%s): %s\n", 
		disp->file_name, strerror(errno));
    
  }

  return 0;
}

/*! Pointer to the function that transfers data to/from the target */
int64_t file_write_function (dada_client_t* client, 
			     void* data, uint64_t data_size)
{
  int i=0,j=0,I;
  dada_disp_t* disp = 0;
  char *x = data;
  int N,N1,N2;
  /* status and error logging facility */
  multilog_t* log;

  assert (client != 0);

  disp = (dada_disp_t*) client->context;
  assert (disp != 0);

  log = client->log;
  assert (log != 0);

  /* if data_size is 4096, it is header, write and return*/

  if(data_size < 5000){
    return write (client->fd, data, data_size);
  }

  /* 0.125ms datapoint, is 5000 points of 25ns data. The buffer size is*/
  /* a multiple of 5000 (buffer_size forced to 4000000) */
  /* remember the index value across different calls to this function*/ 
  /* The buffer contains data from both polns. */

  N = data_size/10000;
  I = disp->index;

  /* average detected signal, with 0.125ms resolution */
  /* remember the index somewhere in the dada_disp_t */
  /* calculate a running average, always retaining 5 sec history*/
  /* shift data in array every 50ms, and calculate only the latest 50ms*/
  /* latest 50ms data is from index 39600 in the disp->data buffer*/

  for (j=0;j<N;j++){
    N1 = 10000*j;
    N2 = 10000*(j+1);
    disp->data[I]=0;
    for (i=N1; i<N2; i+=2)
      disp->data[I] = disp->data[I] + (x[i]*x[i] + x[i+1]*x[i+1]);
    I++; 
    //if ((I>340) && ((I%358)==0)) {
    //disp->data[J] = 10*disp->data[J];
       // disp->data[J-1] = 10*disp->data[J];
    // }
  }


  /* save index for the next entry of this function*/

  disp->index = I;
  
  /* send the last calculated 400 points */

  if(stopSendingData == 0) {                        /* Check if connected */
    if(send(sock, &disp->data[I-400], 1600, MSG_NOSIGNAL) != 1600) { 
      multilog (log, LOG_ERR, "failed to send to audio server\n");
      stopSendingData = 1;      /* Connection failed, try to reconnect  */ 
    }
  }else if(stopSendingData == 1) {          /* Connection failed, so close socked */
    close(sock);
    stopSendingData++;
  }else if(stopSendingData == 2) {        /* Try to open new socket */
    printf("S");
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      multilog (log, LOG_ERR, "failed to create socket\n");
    }else {
      multilog (log, LOG_INFO, "recreated socket\n");
      memset(&audioserver, 0, sizeof(audioserver));       /* Clear struct */
      audioserver.sin_family = AF_INET;                  /* Internet/IP */
      audioserver.sin_addr.s_addr = inet_addr(ip_host);  /* IP address */
      audioserver.sin_port = htons(portnr);       /* server port */
      stopSendingData++;
    }
  }else if(stopSendingData == 3) {       /* Try to reconnect to audio server */
    if (connect(sock, (struct sockaddr *) &audioserver, sizeof(audioserver)) < 0) {
      if((I>7700)&& (I%8000) == 0)
	multilog (log, LOG_ERR, "failed to connect to server\n");
    }else {
      multilog (log, LOG_ERR, "reconnected to server\n");
      stopSendingData = 0;
      output_error = 1;
    }
  }

  return  data_size;
}


int main (int argc, char **argv)
{
  /* DADA Data Block to Disk configuration */
  dada_disp_t disp;
    //= DADA_DISP_INIT;

  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;

  /* DADA Primary Read Client main loop */
  dada_client_t* client = 0;

  /* DADA Logger */
  multilog_t* log = 0;

  /* Flag set in daemon mode */
  char daemon = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* Quit flag */
  char quit = 0;

  /* pipe related stuff */
  //FILE *pp;
  //char cmd[1024],str[1024];

  /* pgplot display screen */
  int arg = 0;
 
  disp.array = disk_array_create ();
  
  while ((arg=getopt(argc,argv,"dD:P:H:Wv")) != -1)
    switch (arg) {
      
    case 'd':
      daemon=1;
      break;

    case 'D':
      if (disk_array_add (disp.array, optarg) < 0) {
	fprintf (stderr, "Could not add '%s' to disk array\n", optarg);
	return EXIT_FAILURE;
      }
      break;
 
    case 'H':
      if(optarg) ip_host=optarg;
      else ip_host=0;
      break;

    case 'v':
      verbose=1;
      break;

    case 'P':
      if (optarg)
	portnr=atoi(optarg);
      break;  

    case 'W':
      disk_array_set_overwrite (disp.array, 1);
      break;

    default:
      usage ();
      return 0;
      
    }

  log = multilog_open ("dada_sound", daemon);
  
  if (daemon) {
    be_a_daemon ();
    multilog_serve (log, DADA_DEFAULT_DBDISK_LOG); 
  }
  else
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

  client->open_function  = file_open_function;
  client->io_function    = file_write_function;
  client->close_function = file_close_function;
  client->direction      = dada_client_reader;

  client->context = &disp;

  memset(disp.data, 0, sizeof(disp.data));

  /* Check if the audioserver is specified */
  if(!ip_host) {                                     
    printf("specify host\n");
    usage();
    multilog (log, LOG_ERR, "no valid host specified\n");
    return 0;
  }

  /* open socket here */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    multilog (log, LOG_ERR, "cannot open socket\n");
    return -1;
  }
  
  memset(&audioserver, 0, sizeof(audioserver)); /* Clear struct */
  audioserver.sin_family = AF_INET;               /* Internet/IP */
  audioserver.sin_addr.s_addr = inet_addr(ip_host); /* IP address */
  audioserver.sin_port = htons(portnr);                 /* server port */

  if (connect(sock, (struct sockaddr *) &audioserver, sizeof(audioserver)) < 0) {
    multilog (log, LOG_ERR, "cannot connect to %s\n",ip_host);
    stopSendingData = 3;      
  }else {
    multilog (log, LOG_INFO, "Opened connection to audioserver\n");
    stopSendingData = 0;     
    /* Default is an established connection to audio server */
  }

  while (!client->quit) {

    if (dada_client_read (client) < 0)
      multilog (log, LOG_ERR, "Error during transfer\n");

  }

  if (dada_hdu_unlock_read (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

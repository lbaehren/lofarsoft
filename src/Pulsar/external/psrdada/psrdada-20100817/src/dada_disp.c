#include "dada_client.h"
#include "dada_hdu.h"
#include "dada_def.h"

#include "disk_array.h"
#include "ascii_header.h"
#include "daemon.h"
//#include "gnuplot_i.h"
#include "/home/pulsar/packages/linux/pgplot/cpgplot.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>

long int KK = 0;
float miny,maxy;

void usage()
{
  fprintf (stdout,
	   "dada_disp [options]\n"
	   " -D <path>   add a disk to which data will be written\n"
	   " -s <screen> screen number for display \n"
	   " -W          overwrite files\n"
	   " -b <band>   WSRT Band number 0-7\n"
	   " -d          run as daemon\n");
}

typedef struct {

  /* the set of disks to which data may be written */
  disk_array_t* array;

  /* a temporary data holder */
  float data[40000];

  /* variable to remember the array index*/
  int index;

  /* current observation id, as defined by OBS_ID attribute */
  char obs_id [DADA_OBS_ID_MAXLEN];

  /* current filename */
  char file_name [FILENAME_MAX];

  /* file offset from start of data, as defined by FILE_NUMBER attribute */
  unsigned file_number;
  
  /* gnuplot handler */
  //gnuplot_ctrl* h1;

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

void plotStuff(float *samples, int nrsamples, 
	       int nrsamplesSP, float SamplesPerSec,
	       char *title)
{
  int j;
  float stretchfac, dy, fracpart;
  long int integerpart;
 
  cpgeras();
  cpgsch(1.5);
  cpgslw(5);
  cpgsvp(0.1, xviewportSize, 0.15, yviewportSize);

  if ( KK <= 25 || ((KK%100) == 0)){
  maxy = samples[0];
  miny = samples[0];  
  for(j = 0; j < nrsamples; j++) {
    if(samples[j] > maxy)
      maxy = samples[j];
    if(samples[j] < miny)
      miny = samples[j];
  }
  }
  KK++;
  cpgsci(1);
  cpgswin(0,nrsamples/SamplesPerSec,miny,miny+2*(maxy-miny));
  cpgbox("bcnst",0.0,0,"bc",0.0,0);
  cpglab("Time (sec)", "Radio Intensity", title);

  /* to erase graphics  - this is supposed to be faster*/
 
  cpgslw(1);
  cpgsci(1);
  cpgmove(0, samples[0]);
  for(j = 0; j < nrsamples; j++) {
    cpgdraw(j/SamplesPerSec, samples[j]);
  }

  stretchfac = nrsamples/(float)nrsamplesSP;
  dy = 1.0*(maxy-miny);

  fracpart = KK*500.0/(2*358.0);
  integerpart = fracpart;
  fracpart -= integerpart;
  integerpart = fracpart*358*2;
  cpgsci(2);
  cpgslw(3);
  cpgmove(0, samples[nrsamples-nrsamplesSP]+dy);
  for(j = nrsamples-nrsamplesSP-integerpart; j < nrsamples-integerpart; j++) {
    cpgdraw(stretchfac*(j-(nrsamples-nrsamplesSP-integerpart))/SamplesPerSec, samples[j]+dy);
  }
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
  write(client->fd,disp->data,10000*sizeof(float));

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
  int i=0,j=0,I,J;
  dada_disp_t* disp = 0;
  char *x = data;
  double xval[10000], yval[10000];
  int N,N1,N2;
  unsigned long bin,pha_bin;
  char *title[]={"Band 0", "Band 1","Band 2", "Band 3",
		 "Band 4", "Band 5", "Band 6", "Band 7",
		 "Unknown Band"};

  assert (client != 0);

  disp = (dada_disp_t*) client->context;
  assert (disp != 0);


  /* if data_size is 4096, it is header, write and return*/

  if(data_size < 5000){
    return write (client->fd, data, data_size);
  }

  /* 1ms datapoint, is 40000 points of 25ns data. The buffer size is*/
  /* a multiple of 40000 (buffer_size forced to 4000000) */
  /* remember the index value across different calls to this function*/ 

  N = data_size/80000;
  I = disp->index;
  J = 9950;
  /* average detected signal, with 1 ms resolution */
  /* remember the index somewhere in the dada_disp_t */
  /* calculate a running average, always retaining 10 sec history*/
  /* shift data in array every 50ms, and calculate only the latest 50ms*/

  memmove(&disp->data[1],&disp->data[50],9950*sizeof(float));

  for (j=0;j<N;j++){
    N1 = 80000*j;
    N2 = 80000*(j+1);
    disp->data[J]=0;
    for (i=N1; i<N2; i+=2)
      disp->data[J] = disp->data[J] + (x[i]*x[i] + x[i+1]*x[i+1]);
    I++; 
    J++;
    //if ((I>340) && ((I%358)==0)) {
    //disp->data[J] = 10*disp->data[J];
       // disp->data[J-1] = 10*disp->data[J];
    // }
  }


  /* phase bin data */
  /* get initial phase - phase_0 from ppredict, using a pipe*/
  /* get F0 from ppredict */
  
  for (i=disp->index;i<I;i++){
    bin = (long int) ((i*0.001)/disp->P + disp->phase_0);
    pha_bin = (4096*bin)%4096;
    disp->fold_data[pha_bin] = disp->fold_data[pha_bin] + disp->data[i];
    // printf("%"PRIu64" %"PRIu64" ",bin,pha_bin);
  }

  /* save index for the next entry of this function*/

  disp->index = I;
  
  if ( (I>150) && ((I%2000)==0) ) {
       //printf("write_function : disp->index=%d\n",disp->index);
       for (i=0;i<5000;i++) { 
	 xval[i] = i*0.001;
	 yval[i] = (double)(disp->data[i+5000]);
       }

       //strcpy(title, "Frequency band 0");
       plotStuff(&disp->data[5000], 5000, 358, 1000.0, title[disp->band]);
       //gnuplot_resetplot(disp->h1) ; 
       //gnuplot_plot_xy(disp->h1, xval, yval, 5000, "Detected Power");
       //gnuplot_plot_xy(disp->h1, disp->x, disp->fold_data, 4096, " ");
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
  int dis=0;
  char PlotDevice[100];
  int deviceID;

  int arg = 0;
  int band=8;

  disp.array = disk_array_create ();
  
  while ((arg=getopt(argc,argv,"dD:s:Wb:v")) != -1)
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
 
    case 'b':
      if(optarg) band=atoi(optarg);
      else band=8;
      break;

    case 'v':
      verbose=1;
      break;

    case 's':
      if (optarg)
	dis=atoi(optarg);
      break;  

    case 'W':
      disk_array_set_overwrite (disp.array, 1);
      break;

    default:
      usage ();
      return 0;
      
    }

  if (!dis) {
    printf("Give a valid display number\n");
    usage();
    exit(0);
  }
  
  

  log = multilog_open ("dada_disp", daemon);

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

  /* find pulse phase and period */
  /* use ascii_header_get to get time */

  //sprintf(cmd,"ppredict -u 2005-12-12-15:50:40 0329+54"
//	  "| tail -1 | awk -F\" \" '{print $7}'");
//  pp = popen(cmd, "r");

//  if (!pp) {
//    multilog(log,LOG_ERR,"popen failed");
//    return EXIT_FAILURE;
//  }
//  fgets(str, sizeof(str),pp);
//  disp.phase_0 = atof(str);

//  multilog(log,LOG_INFO,"phase_0 %s",str);

//  sprintf(cmd,"ppredict -u 2005-12-12-15:50:40 0329+54"
//	  "| tail -1 | awk -F\" \" '{print $9}'");
//  pp = popen(cmd, "r");
  
//  if (!pp) {
//    multilog(log,LOG_ERR,"popen failed");
//    return EXIT_FAILURE;
//  }
//  fgets(str, sizeof(str),pp);
//  disp.P = atof(str);
//  multilog(log,LOG_INFO,"period %s",str);

  /* The device used for plotting */

  sprintf(PlotDevice, "%d/xs",dis);
  deviceID = cpgopen(PlotDevice);
  cpgpap(4.7,0.75);
  
  if (band!=8) disp.band=band;

  if (!deviceID) {
    multilog(log,LOG_ERR,"cannot open display device");
    return EXIT_FAILURE;
  }
    
  cpgask(0);
 
  //disp.h1 = gnuplot_init() ;
  //gnuplot_setstyle(disp.h1, "lines") ;

  memset(disp.data, 0, sizeof(disp.data));

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

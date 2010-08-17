
#include "puma2_dmadb.h"

#define DADA_TIMESTR "%Y-%m-%d-%H:%M:%S"

static char* default_header =
"OBS_ID      Test.0001\n"
"OBS_OFFSET  0\n"
"FILE_SIZE   800000000\n"
"FILE_NUMBER 0\n";

int usage(char *prg_name)
{
    fprintf (stdout,
	   "%s [options]\n"
	     " -d             run as daemon\n"
	     " -i idstr       observation id string\n"	
	     " -m mode        1 for independent mode, 2 for controlled mode\n"	
	     " -n nbuf        # of buffers to acquire\n"
	     " -s sec         # of seconds to acquire\n"
	     " -v             verbose messages\n"
             " -H filename    ascii header information in file\n"
	     " -S             file size in bytes\n"
	     " -W             flag buffers readable\n",
	     prg_name
	     );
    exit(0);
    
}



int pic_edt_init(dmadb_t* dmadb)
{
  int verbose = dmadb->verbose; /* saves some typing */

  /* open PiC card - makes the device available for further writes  */

  if (verbose) fprintf(stderr,"opening PiC Device");
  if ( (pic_open(&dmadb->pic, 0)) < 0 ){
    fprintf(stderr,"PiC open failed\n");
    return -1;
  }    

  /* disarm pic as a precautionary measure */
 
  if ( (pic_configure(&dmadb->pic, DISARM)) < 0 ){
    fprintf(stderr,"Cannot disarm PiC\n");
    return -1;
  }  
  if (verbose) fprintf(stderr,"...done\n");
 
  /* setup EDT DMA Card */
  
  if (verbose) fprintf(stderr,"open EDT device....");
  if ((dmadb->edt_p = edt_open(EDT_INTERFACE, 0)) == NULL)
    {
      fprintf(stderr,"edt_open error\n") ;
      return -1;
    }
  if (verbose) fprintf(stderr,"...done\n");
  
  
  if (verbose) fprintf(stderr,"Configuring EDT kernel ring buffers....");
  if (edt_configure_ring_buffers(dmadb->edt_p, BUFSIZE, 16, EDT_READ, NULL) == -1)
    {
      fprintf(stderr,"edt_configure_ring_buffers error\n") ;
      return -1;
    }
  if (verbose) fprintf(stderr,"...done\n");
  
  return 0;

}

time_t arm_pic(dada_pwc_main_t* pwcm, time_t start_utc)
{
  /* Arm the PuMaII Interface card, at the given UTC */
  /* Wait for the right 10-sec boundary, before arming PiC */
  time_t now;
  unsigned sleep_time = 0;
  /* get our context here, all required parameters are here */
  dmadb_t* dmadb = (dmadb_t*)pwcm->context;
  int verbose = dmadb->verbose; /* saves some typing */

  /* wait for atleast 10-second before start time */
  now = time(0);

  if ( (start_utc - now) >= 10) {

    /* sleep for atleast 10 seconds before start */
    sleep_time = start_utc - now - 10;
    multilog (pwcm->log, LOG_INFO, "sleeping for %u sec\n", sleep_time);
    fprintf (stderr,"sleeping for %d secs",sleep_time);
    sleep (sleep_time);
  } 
  else {

    /* check if we're within the 'allowed window' , if not sleep */

    if (verbose) fprintf(stderr,"Going to start");
    while ( (now % 10 <= 1) | (now % 10 >= 8) ) {
      usleep (250000);
      fprintf (stderr,"%d .",(int)now % 10);
      now = time(0);
    }
    fprintf(stderr,"\n");
  }  

 /* flush stale buffers in the EDT card */
  edt_flush_fifo(dmadb->edt_p) ;     
 
  /* reset EDT kernel buffers */
  if ( edt_reset_ring_buffers(dmadb->edt_p, 0) == -1) 
    fprintf(stderr,"error resetting ring buffers");
  
  usleep(50000);

  /* setup EDT to do continuous transfers from now on */
  if ( edt_start_buffers(dmadb->edt_p, 0) == -1){
    fprintf(stderr,"error starting ring buffers") ; 
    return EXIT_FAILURE;
  }

  /*PiC is arm'ed now. When the next 10-Sec pulse arrives */
  /*EDT will receive both clock and enable data*/

  if ( (pic_configure(&dmadb->pic, ARM)) < 0 ){
    multilog(pwcm->log, LOG_ERR,"Cannot arm PiC\n");
    return 0;
  }    

  /* The actual start time is the next 10-second tick UTC */
  now = time(0);
  start_utc = (1+now/10)*10; /* integer division plus one to get next 10-sec */
  return start_utc;

}

void* get_next_buf(dada_pwc_main_t* pwcm, uint64_t* size)
{
  dmadb_t* dmadb = (dmadb_t*)pwcm->context;
  int verbose = dmadb->verbose; /* saves some typing */

  /* size of buffer to be acquired */
  *size = BUFSIZE;

  dmadb->buf = edt_done_count(dmadb->edt_p);
  if (verbose) fprintf(stderr,"buf count %d\n",dmadb->buf);

  if (verbose) fprintf(stderr,"Waiting for buffers...");
  dmadb->data = edt_wait_for_buffers(dmadb->edt_p, 1);
  if ( dmadb->data == NULL){
    edt_perror("edt_wait");
    multilog(pwcm->log,LOG_ERR,"edt_wait error\n");
    return 0;
  }
  if (verbose) fprintf(stderr,"Done");

  
  /* check for overflows */
  if ( (edt_ring_buffer_overrun(dmadb->edt_p)) != 0){
    fprintf(stderr,"Overrun Error\n");
    multilog(pwcm->log,LOG_ERR,"EDT Overrun\n");
  }
  
  /* return the acquired data buffer */
  return (void*) dmadb->data;

}

int stop_acq(dada_pwc_main_t* pwcm)
{
  dmadb_t* dmadb = (dmadb_t*)pwcm->context;
  int verbose = dmadb->verbose; /* saves some typing */

  /* stop the EDT buffers */
  if (verbose) fprintf(stderr,"Stopping EDT Device...");
  edt_stop_buffers(dmadb->edt_p) ; 

  /* disarm pic now*/
  if (verbose) fprintf(stderr,"Stopping PiC Device...");
  if ( (pic_configure(&dmadb->pic, DISARM)) < 0 ){
    multilog(pwcm->log, LOG_ERR,"Cannot configure PiC\n");
    return -1;
  }  
  if (verbose) fprintf(stderr,"done\n");

  return 0;
}

int main (int argc, char **argv)
{
    /* DADA Header plus Data Unit */
    dada_hdu_t* hdu;

    /* primary write client main loop  */
    dada_pwc_main_t* pwcm; 

    /* header */
    char* header = default_header; 
    char* header_buf = 0;
    unsigned header_strlen = 0;/* the size of the header string */
    uint64_t header_size = 0;/* size of the header buffer */
    char writemode='w';

    /* flags */
    char daemon = 0; /*daemon mode */

    int verbose = 0;/* verbose mode */

    int mode = 0;

    /* dmadb stuff, all EDT/PiC stuff is in here */
    
    dmadb_t dmadb;
    
    time_t utc;

    unsigned buffer_size = 64;
    
    static char* buffer = 0;
 
    char *src;
    int nSecs = 100,nbufs=0;
    int wrCount,rdCount;
    char *ObsId = "Test";
    unsigned long fSize=800000000;

    /* the filename from which the header will be read */
    char* header_file = 0;

    int arg = 0;
    while ((arg=getopt(argc,argv,"di:m:n:s:vH:S:W")) != -1) {
	switch (arg) {
	    
	    case 'd':
		daemon=1;
		break;
		
	    case 'v':
		verbose=1;
		break;

            case 'H':
                header_file = optarg;
                break;

	    case 'i':
		if (verbose) fprintf(stderr,"ObsId is %s \n",optarg);
		if(optarg) {
		    ObsId = strdup (optarg);
                    assert (ObsId != 0);
		}
		else usage(argv[0]);
		break;

	    case 'm':
	         if (verbose) fprintf(stderr,"Mode is %s \n",optarg);
		 if(optarg) {
		   mode = atoi (optarg);
		   if ( (mode == 1) || (mode == 2)){
		     fprintf(stderr,"Using mode %d\n",mode);
		   }
		   else {
		     fprintf(stderr,"Specify a valid mode\n");
		     usage(argv[0]);
		   }
		 }
		 break;

	    case 'n':
		if (optarg){
		    nbufs = atoi(optarg);
		    fprintf(stderr,"Acquiring %d buffers\n",nbufs);
		}
		else usage(argv[0]);
		break;
	    
	    case 's':
		if (optarg){
		    nSecs = atoi(optarg);
		    fprintf(stderr,"Will acquire for %d seconds\n",nSecs);
		}
		else usage(argv[0]);
		break;

	    case 'S':
		if (optarg){
		    fSize = atoi(optarg);
		    fprintf(stderr,"File size will be %lu\n",fSize);
		}
		else usage(argv[0]);
		break; 

	    case 'W':
	        writemode='W';
		break;
		
	    default:
		usage(argv[0]);
		return 0;
		
	}
    }
    
    /* register our options in dmadb structure*/

    dmadb.verbose = verbose;
    dmadb.nbufs = nbufs;
    dmadb.buf = 0;
    dmadb.nSecs = nSecs;
    dmadb.daemon = daemon;
    dmadb.ObsId = ObsId;

    if (!mode ){
      fprintf(stderr,"Please specify mode\n");
      usage(argv[0]);
    }


   /* Initialize DADA/PWC structures*/
    pwcm = dada_pwc_main_create ();
    
    pwcm->context = &dmadb;
      
    pwcm->log = multilog_open ("puma2_dmadb", daemon);

    /* set up for daemon usage */	  
    
    if (daemon) {
      be_a_daemon ();
      multilog_serve (pwcm->log, DADA_DEFAULT_PWC_LOG);
    }
    else{
      //multilog_add (pwcm->log, stderr);
      multilog_serve (pwcm->log, DADA_DEFAULT_PWC_LOG);
    }

    /* Initializing PiC/EDT cards*/

    if (verbose) fprintf(stderr,"Going to initialize PiC/EDT...");
    if ( pic_edt_init(&dmadb) < 0) {
      fprintf(stderr,"Cannot initialize EDT/PiC\n");
      return EXIT_FAILURE;
    }
    if (verbose) fprintf(stderr,"pic/edt init done\n");
    
   
    /* create the header/data blocks */
    
    hdu = dada_hdu_create (pwcm->log);

    if (dada_hdu_connect (hdu) < 0)
      return EXIT_FAILURE;    

    /* make data buffers readable */
    
    if (dada_hdu_lock_write_spec (hdu,writemode) < 0)
      return EXIT_FAILURE;
    
    /* if we are a stand alone application */
    
    if ( mode == 1){
      
      header_size = ipcbuf_get_bufsz (hdu->header_block);
      multilog (pwcm->log, LOG_INFO, "header block size = %llu\n", header_size);
      
      header_buf = ipcbuf_get_next_write (hdu->header_block);
      
      if (!header_buf)  {
	multilog (pwcm->log, LOG_ERR, "Could not get next header block\n");
	return EXIT_FAILURE;
      }
      
      /* if header file is presented, use it. If not set command line attributes */ 
      if (header_file)  {
	if (fileread (header_file, header_buf, header_size) < 0)  {
	  multilog (pwcm->log, LOG_ERR, "Could not read header from %s\n", header_file);
	  return EXIT_FAILURE;
	}
      }
      else {
	header_strlen = strlen(header);
	memcpy (header_buf, header, header_strlen);
	memset (header_buf + header_strlen, '\0', header_size - header_strlen);
	
	if (ascii_header_set (header_buf, "HDR_SIZE", "%llu", header_size) < 0) {
	  multilog (pwcm->log, LOG_ERR, "Could not write HDR_SIZE to header\n");
	  return -1;
	}
	
	if (verbose) fprintf(stderr,"Observation ID is %s\n",ObsId);
	if (ascii_header_set (header_buf, "OBS_ID", "%s", ObsId) < 0) {
	  multilog (pwcm->log, LOG_ERR, "Could not write OBS_ID to header\n");
	  return -1;
	}
	
	if (verbose) fprintf(stderr,"File size is %lu\n",fSize);
	if (ascii_header_set (header_buf, "FILE_SIZE", "%lu", fSize) < 0) {
	  multilog (pwcm->log, LOG_ERR, "Could not write FILE_SIZE to header\n");
	  return -1;
	}
      }
      
      /* if number of buffers is not specified, */
      /* find it from number of seconds, based on 80 MB/sec sample rate*/
      if (nbufs == 0)  {
	float tmp;
	tmp = (float)((80000000 * nSecs)/BUFSIZE);
	nbufs = (int) tmp;
	if (verbose) fprintf(stderr,"Number of bufs is %d %f\n",nbufs,tmp);
      }
      
      /* Arm PiC now and set actual start time in header */
      
      utc = arm_pic(pwcm, 0);

      if (!buffer)
	buffer = malloc (buffer_size);
      assert (buffer != 0);
      
      strftime (buffer, buffer_size, DADA_TIMESTR, gmtime(&utc));

      /* write UTC_START to the header */
      if (ascii_header_set (header_buf, "UTC_START", "%s", buffer) < 0) {
	multilog (pwcm->log, LOG_ERR, "failed ascii_header_set UTC_START\n");
	return -1;
      }

      /* donot set header parameters anymore - acqn. doesn't start */
      if (ipcbuf_mark_filled (hdu->header_block, header_size) < 0)  {
	multilog (pwcm->log, LOG_ERR, "Could not mark filled header block\n");
	return EXIT_FAILURE;
      }
        
      /* The write loop: repeat for n buffers */ 
      while(dmadb.buf < nbufs -1 ){
	
	uint64_t bsize = BUFSIZE;
	
	src = (char *)get_next_buf(pwcm, &bsize);
	
	/* write data to datablock */
	if (( ipcio_write(hdu->data_block, src, BUFSIZE) ) < BUFSIZE ){
	  multilog(pwcm->log, LOG_ERR, "Cannot write requested bytes to SHM\n");
	  return EXIT_FAILURE;
	}
	
	wrCount = ipcbuf_get_write_count ((ipcbuf_t*)hdu->data_block);
	rdCount = ipcbuf_get_read_count ((ipcbuf_t*)hdu->data_block);
	if (verbose) fprintf(stderr,"%d %d %d\n",dmadb.buf,wrCount,rdCount);
	
      }
      
      if ( stop_acq(pwcm) != 0)
	fprintf(stderr, "Error stopping acquisition");
      
      if (dada_hdu_unlock_write (hdu) < 0)
	return EXIT_FAILURE;
      
      if (dada_hdu_disconnect (hdu) < 0)
	return EXIT_FAILURE;
      
    }

    /* we are controlled by PWC control interface */

    else{
      
      /*setup PWCM stuff. */
      pwcm->start_function  = arm_pic;
      pwcm->buffer_function = get_next_buf;
      pwcm->stop_function   = stop_acq;
      pwcm->data_block      = hdu->data_block;
      pwcm->header_block    = hdu->header_block;
      
      if (verbose) fprintf (stderr, "Creating dada pwc control interface\n");
      pwcm->pwc = dada_pwc_create ();
           
      if (verbose) fprintf (stderr, "Creating dada server\n");
      if (dada_pwc_serve (pwcm->pwc) < 0) {
	fprintf (stderr, "puma2_dmadb: could not start server\n");
	return -1;
      }
      
      if (verbose) fprintf (stderr, "Getting into PWC Control\n");
      if (dada_pwc_main (pwcm) < 0) {
	fprintf (stderr, "puma2_dmadb: error in PWC main loop\n");
	return -1;
      }
      
      fprintf (stderr, "Destroying pwc\n");
      dada_pwc_destroy (pwcm->pwc);
    }
    
    fprintf (stderr, "Destroying pwc main\n");
    dada_pwc_main_destroy (pwcm);
  
    /* disable ring buffers, and release DMA resources */
    if (verbose) fprintf(stderr,"disabling ring buffers EDT Device...");
    edt_disable_ring_buffers(dmadb.edt_p) ; 
  
    edt_close(dmadb.edt_p);
    if (verbose) fprintf(stderr,"done\n");
    
    return EXIT_SUCCESS;
}


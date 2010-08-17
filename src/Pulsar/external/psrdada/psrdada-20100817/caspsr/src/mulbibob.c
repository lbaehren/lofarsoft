/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "mulbibob.h"
#include "dada_def.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

// #define _DEBUG 1

void bibob_thread_init (bibob_thread_t* bibob, int number)
{
  bibob->bibob = bibob_construct ();
  //TODO remove this hack!!!
  //bibob_set_number (bibob->bibob, number);
  bibob_set_number (bibob->bibob, 14);

  pthread_mutex_init(&(bibob->mutex), NULL);
  pthread_cond_init (&(bibob->cond), NULL);

  bibob->id = 0;
  bibob->bramdump = 1;
  bibob->bramdisk = 0;
  bibob->bramplot = 1;
  bibob->alive = 0;

  bibob->quit = 1;
}

/*! allocate and initialize a new bibob_t struct */
mulbibob_t* mulbibob_construct (unsigned nbibob)
{
  mulbibob_t* mulbibob = malloc (sizeof(mulbibob_t));

  mulbibob->threads = malloc (sizeof(bibob_thread_t) * nbibob);
  mulbibob->nthread = nbibob;

  unsigned bibob = 0;
  bibob_thread_t* thread;

  for (bibob = 0; bibob < nbibob; bibob++) {
    bibob_thread_init (mulbibob->threads + bibob, bibob + 1);
    thread = mulbibob->threads + bibob;
  }

  mulbibob->parser = command_parse_create ();
  mulbibob->server = 0;
  mulbibob->port = 0;

  command_parse_add (mulbibob->parser, mulbibob_cmd_state, mulbibob,
                     "state", "get the current state", NULL);

  command_parse_add (mulbibob->parser, mulbibob_cmd_hostport, mulbibob,
                     "hostport", "set the hostname and port number", NULL);

  command_parse_add (mulbibob->parser, mulbibob_cmd_hostports, mulbibob,
                     "hostports", "set n hostnames and ports number", NULL);

  command_parse_add (mulbibob->parser, mulbibob_cmd_mac, mulbibob,
                     "mac", "set the target MAC address", NULL);

  command_parse_add (mulbibob->parser, mulbibob_cmd_macs, mulbibob,
                     "macs", "set multiple target MAC addresses", NULL);

  command_parse_add (mulbibob->parser, mulbibob_cmd_open, mulbibob,
                     "open", "open command interface connections", NULL);

  command_parse_add (mulbibob->parser, mulbibob_cmd_close, mulbibob,
                     "close", "close command interface connections", NULL);

  command_parse_add (mulbibob->parser, mulbibob_cmd_arm, mulbibob,
                     "arm", "reset packet count", NULL);

  command_parse_add (mulbibob->parser, mulbibob_cmd_gain, mulbibob,
                     "caspsrgain", "set specific channel/pol gain", NULL);

  command_parse_add (mulbibob->parser, mulbibob_cmd_levels, mulbibob,
                     "levels", "set all bibob levels", NULL);

  command_parse_add (mulbibob->parser, mulbibob_cmd_quit, mulbibob,
                     "quit", "quit", NULL);

  command_parse_add (mulbibob->parser, mulbibob_cmd_listdev, mulbibob,
                     "listdev", "listdev", NULL);

  return mulbibob;
}

/*! free all resources reserved for bibob communications */
int mulbibob_destroy (mulbibob_t* mulbibob)
{

  command_parse_destroy(mulbibob->parser);
  command_parse_server_destroy(mulbibob->server);

  unsigned bibob=0;
  for (bibob = 0; bibob < mulbibob->nthread; bibob++) {
    bibob_destroy (mulbibob->threads[bibob].bibob);
  }

  free(mulbibob->threads);
  free(mulbibob);
}

/* contact the bibob and accumulate a bramdump */
int bramdump (bibob_t* bibob)
{
  return bibob_bramdump(bibob);
}

/* write the accumulated bramdumps to disk, copying to the NFS server */
int bramdisk(bibob_t* bibob)
{
  return bibob_bramdisk(bibob);
}

/* write the accumulated bramdumps to disk, copying to the NFS server */
int bramplot(bibob_t* bibob, char * device)
{
  return bibob_bramplot(bibob, device);
}

/* reset the accumulated bramdumps */
int bramreset(bibob_t* bibob)
{
  bibob_bramdump_reset(bibob);
}



/*!
  The monitor thread simply sits in a loop, opening the connection
  as necessary, and polling the connection every second.  Polling
  can be either a simple ping or a bramdump.  On failure, the
  connection is closed and re-opened ad infinitum every five seconds.
*/

void* mulbibob_monitor (void* context)
{
  if (!context)
    return 0;

  bibob_thread_t* thread = context;
  bibob_t* bibob = thread->bibob;

#ifdef _DEBUG
  fprintf (stderr, "mulbibob_monitor: thread starting\n");
#endif

  while (!thread->quit)
  {
    pthread_mutex_lock (&(thread->mutex));

    fprintf (stderr, "mulbibob_monitor: opening %s:%d\n",
             bibob->host, bibob->port);

    if ( !bibob_is_open(bibob) && bibob_open (bibob) < 0 )
    {
      fprintf (stderr, "mulbibob_monitor: could not open %s:%d - %s\n",
	       bibob->host, bibob->port, strerror(errno));

      pthread_mutex_unlock (&(thread->mutex));

      if (thread->quit)
        break;

      sleep (5);
      continue;
    }

    pthread_mutex_unlock (&(thread->mutex));

    double disk_interval = 1.0;  // 10 seconds between disk dumps
    unsigned n_dump = 1;         // number of bram dumps per disk dump

#ifdef _DEBUG
    fprintf (stderr, "mulbibob_monitor: %s bibob alive\n", bibob->host);
#endif

    while (!thread->quit)
    {
      int retval = 0;

      struct timeval time1;
      gettimeofday (&time1, 0);

      pthread_mutex_lock (&(thread->mutex));

      fprintf(stderr,"main\n");

      if ( (thread->bramdump) && (bibob_is_open(bibob)) )
      {
        fprintf(stderr,"bramdump\n");
        retval = bramdump (bibob);
        if ( (thread->bramplot) && (thread->bibob->n_brams >= n_dump) ) {
          fprintf(stderr,"bramplot\n");
          bramplot (thread->bibob, "/xs");
          bramreset (thread->bibob);
        }
        //if ( (thread->bramdisk) && (thread->bibob->n_brams >= n_dump) )
        //  bramdisk (thread->bibob);
      }
      else
        retval = bibob_ping (bibob);

      pthread_mutex_unlock (&(thread->mutex));

      struct timeval time2;
      gettimeofday (&time2, 0);

      if (retval < 0)
      {
        fprintf (stderr, "mulbibob_monitor: communicaton failure on %s:%d\n",
                 bibob->host, bibob->port);
        break;
      }

      thread->alive = time(0);

      if (thread->quit)
        break;

      // the time required to do the bramdump
      double time_taken = diff_time (time1, time2);

      // no memory -> assume all dumps take as long as last one
      double sleep_in_sec = disk_interval / n_dump - time_taken;

      // sleep at least 10 milliseconds
      double ten_ms = 0.01;
      if (sleep_in_sec < ten_ms)
        sleep_in_sec = ten_ms;

      fprintf(stdout, "starting pause\n");
      // bibob_pause accepts sleep in milliseconds
      bibob_pause( (int)(sleep_in_sec * 1e3) );
      fprintf(stdout, "starting over\n");
    }

    fprintf (stderr, "mulbibob_monitor: closing connection with %s:%d\n",
             bibob->host, bibob->port);

    bibob_close (bibob);
  }

  return 0;
}

/*! open the command connections to all of the bibobs */
int mulbibob_cmd_open (void* context, FILE* fptr, char* args)
{
  if (!context)
    return -1;

  mulbibob_t* mulbibob = context;

  unsigned bibob = 0;
  for (bibob = 0; bibob < mulbibob->nthread; bibob++)
  {
    bibob_thread_t* thread = mulbibob->threads + bibob;

    // if thread is currently running, then connection is open
    if (!thread->quit)
      continue;

    // if thread has been spawned but is not running, clean up its resources
    if (thread->id)
    {
      void* result = 0;
      pthread_join (thread->id, &result);
    }

    thread->id = 0;
    thread->quit = 0;

    int err = pthread_create (&(thread->id), 0, mulbibob_monitor, thread);
    if (err)
      fprintf (stderr, "mulbibob_cmd_open: error starting thread %d - %s\n",
	       bibob, strerror (err));
  }
}


/*! close the command connections to all of the bibobs */
int mulbibob_cmd_close (void* context, FILE* fptr, char* args)
{
  if (!context)
    return -1;

  unsigned bibob = 0;
  mulbibob_t* mulbibob = context;

  for (bibob = 0; bibob < mulbibob->nthread; bibob++)
    mulbibob->threads[bibob].quit = 1;
}

/*! reset packet counter on next UTC second, returned */
int mulbibob_cmd_state (void* context, FILE* fptr, char* args)
{
  unsigned bibob = 0;
  mulbibob_t* mulbibob = context;

  time_t current = time(0);

  for (bibob = 0; bibob < mulbibob->nthread; bibob++)
  {
    bibob_thread_t* thread = mulbibob->threads + bibob;
    fprintf (fptr, "bibob%02d %s:%d ", bibob+1, 
	     thread->bibob->host, thread->bibob->port);

    if (thread->quit)
      fprintf (fptr, "closed");

    else if (current - thread->alive < 5)
      fprintf (fptr, "alive");

    else
      fprintf (fptr, "dead");

    fprintf (fptr, "\n");
  }
}

/*! set the host and port number of the specified bibob */
int mulbibob_cmd_hostport (void* context, FILE* fptr, char* args)
{
  mulbibob_t* mulbibob = context;

  unsigned bibob = 0;
  char host[64];
  unsigned port = 0;

  if (!args)
  {
    fprintf (fptr, "no args specified. usage: bibob host port\n");
    return -1;
  }

  if (sscanf (args, "%u %s %u", &bibob, host, &port) != 3) 
  {
    fprintf (fptr, "hostport command failed. Argument mismatch\n");
    return -1;
  } 

  if ((bibob < 0) || (bibob >= mulbibob->nthread)) 
  {
    fprintf (fptr, "bibob number mismatch [0 - %d]\n",mulbibob->nthread);
    return -1;
  }

  fprintf(stderr, "mulbibob_cmd_hostport ibibob=%d\n", bibob);
  bibob_thread_t* thread = mulbibob->threads + bibob;

  /* tell monitor_thread for this bibob to exit, closing the bibob */
  thread->quit = 1;
  void* result = 0;
  pthread_join (thread->id, &result);

  /* update the host and port for this bibob */
  if ( bibob_set_host (thread->bibob, host, port) < 0 )
  {
    fprintf (fptr, "failed to set bibob host:port %s:%d\n",host,port);
    return -1;
  }

  /* restart the bibob */

  thread->id = 0;
  thread->quit = 0;

  int err = pthread_create (&(thread->id), 0, mulbibob_monitor, thread);
  if (err)
    fprintf (fptr, "mulbibob_cmd_open: error starting thread %d - %s\n",
       bibob, strerror (err));

  return 0;

}

/*! set the host and port number of the specified bibob */
int mulbibob_cmd_hostports (void* context, FILE* fptr, char* args)
{
  mulbibob_t* mulbibob = context;

  bibob_thread_t* thread;
  
  unsigned bibob  = 0;
  unsigned nbobs = 0;
  int * bibobs;
  char ** hosts;
  int * ports;
  unsigned i = 0;

  /* if no args specified, print current settings */
  if (!args)
  {
    char response[1024];
    int j=0;
    j += sprintf(response, "%d", mulbibob->nthread);

    for (i=0; i< mulbibob->nthread; i++)
    {
      thread = mulbibob->threads + i;
      j += sprintf(response + j, " %d %s %d", i, thread->bibob->host, thread->bibob->port);
    }
    response[j] = '\n';
    response[j+1] = '\0';
    fprintf(fptr, response);

    return 0;
  }

  // extract arguemnts of the form "nbob [bibob bibob_host bibob_port]+"
  const char *sep = " ";
  char *line;

  line = strtok(args, sep);
  nbobs = atoi(line);

  if ((nbobs <= 0) || (nbobs > mulbibob->nthread)) 
  {
    fprintf (fptr, "hostports command failed. Argument mismatch\n");
    return -1;
  }

  bibobs = malloc(sizeof(int) * nbobs);
  hosts = malloc(sizeof(char*) * nbobs);
  ports = malloc(sizeof(int) * nbobs);
  
  for (i=0; i<nbobs; i++) 
  {

    line = strtok(NULL, sep);
    bibobs[i] = atoi(line);

    if ((bibobs[i] < 0) || (bibobs[i] >= mulbibob->nthread))
    {
      fprintf (fptr, "bibob number mismatch [0 - %d]\n",mulbibob->nthread);
      return -1;
    }

    line = strtok(NULL, sep);
    hosts[i] = strdup(line);

    line = strtok(NULL, sep);
    ports[i] = atoi(line);

  }

  /* ask specified bibob threads to quit */
  for (i=0; i < nbobs; i++) 
  {
    thread = mulbibob->threads + bibobs[i];
    if (bibob_is_open(thread->bibob)) 
    {
      thread->quit = 1;
      void* result = 0;
      pthread_join (thread->id, &result);
    }
  }

  unsigned failure = 0;
  /* update the host and port for specified bibob threads */
  for (i=0; i < nbobs; i++)
  {
    thread = mulbibob->threads + bibobs[i];
    if ( bibob_set_host (thread->bibob, hosts[i], ports[i]) < 0 ) 
    {
      fprintf (fptr, "failed to set bibob host:port %s:%d\n",hosts[i],ports[i]);
      failure = 1;
    }
  } 

  /* restart the specified bibob threads */
  for (i=0; i<nbobs; i++)
  {
    thread = mulbibob->threads + bibobs[i];
    thread->id = 0;
    thread->quit = 0;

    int err = pthread_create (&(thread->id), 0, mulbibob_monitor, thread);
    if (err)
      fprintf (fptr, "mulbibob_cmd_hostports: error starting thread %d - %s\n",
        bibobs[i], strerror (err));
  }

  /* cleanup mallocs */
  for (i=0; i<nbobs; i++)
  {
    free(hosts[i]);
  }

  free(bibobs);
  free(hosts);
  free(ports);

  return 0;
                                                                                                                                        
}

/*! set the target MAC address of the specified bibob */
int mulbibob_cmd_mac (void* context, FILE* fptr, char* args)
{
  mulbibob_t* mulbibob = context;

  unsigned bibob = 0;
  char mac_address[16];

  if (!args)
  {
    fprintf (fptr, "no args specified\n");
    return -1;
  }

  if (strlen(args) < 3)
  {
    fprintf(fptr, "not enough arguements\n");
    return -1;
  }

  if (sscanf (args, "%u %s", &bibob, mac_address) != 2)
  {
    fprintf (fptr, "mac command failed. Argument mismatch\n");
    return -1;
  }

  if ((bibob < 0) || (bibob >= mulbibob->nthread))
  {
    fprintf (fptr, "bibob number mismatch [0 - %d]\n",mulbibob->nthread);
    return -1;
  }

  if (strlen(mac_address) != 12)
  {
    fprintf (fptr, "mac address was not 12 chars long\n");
    return -1;
  }

  bibob_thread_t* thread = mulbibob->threads + bibob;

#ifdef _DEBUG
  fprintf (stderr, "mulbibob_cmd_mac: lock bibob %u\n", bibob);
#endif

  /* lock the bibob for communication */
  if (pthread_mutex_lock (&(thread->mutex)) != 0)
  {
    fprintf (fptr, "mulbibob_cmd_mac: could not lock bibob\n");
    return -1;
  }

  if ( !bibob_is_open(thread->bibob) )
  {
    fprintf (fptr, "bibob %u is not open\n", bibob);
    pthread_mutex_unlock (&(thread->mutex));
    return -1;
  }

  if ( bibob_configure (thread->bibob) < 0 )
  {
    fprintf (fptr, "failed to configure bibob with mac address %s\n",mac_address);
    pthread_mutex_unlock (&(thread->mutex));
    return -1;
  }
  
#ifdef _DEBUG
  fprintf (stderr, "mulbibob_cmd_mac: unlock bibob %u\n", bibob);
#endif
 
  pthread_mutex_unlock (&(thread->mutex));

  return 0;
                                                                                                                                        
}
                                                                                                                                        

/*! set the target MAC address of the specified bibobs */
int mulbibob_cmd_macs (void* context, FILE* fptr, char* args)
{
  mulbibob_t* mulbibob = context;
  
  unsigned bibob = 0;
  unsigned nbobs = 0;
  int * bibobs;
  char ** macs;

  char mac_address[16];

  /* if no args specified, print current settings */
  if (!args)
  {
    fprintf(fptr, "No args specified\n");
    return -1;
  }

  // extract arguemnts of the form "nbob [bibob bibob_host bibob_port]+"
  const char *sep = " ";
  char *saveptr;
  char *line;

  line = strtok_r(args, sep, &saveptr);
  nbobs = atoi(line);

  if ((nbobs <= 0) || (nbobs > mulbibob->nthread))
  {
    fprintf (fptr, "macs command failed. Argument mismatch\n");
    return -1;
  }

  bibobs = malloc(sizeof(int) * nbobs);
  macs = malloc(sizeof(char*) * nbobs);

  unsigned i=0;
  for (i=0; i<nbobs; i++)
  {
    line = strtok_r(NULL, sep, &saveptr);
    if (line == NULL)
    {
       fprintf (fptr, "syntax error for bibob %d\n", i);
       return -1;
    }

    bibobs[i] = atoi(line);

    if ((bibobs[i] < 0) || (bibobs[i] >= mulbibob->nthread))
    {
      fprintf (fptr, "bibob number %d mismatch [0 - %d]\n",bibobs[i],mulbibob->nthread);
      return -1;
    }
                                                                                                                                        
    line = strtok_r(NULL, sep, &saveptr);
    if (line == NULL)
    {
      fprintf (fptr, "syntax error for bibob %u\n", bibobs[i]);
      return -1;
    }

    macs[i] = strdup(line);

    if (strlen(macs[i]) != 12) 
    {
      fprintf (fptr, "bibob %u mac address incorrect %s\n", bibobs[i], macs[i]);
      return -1;
    }
  }

#ifdef _DEBUG
  fprintf (stderr, "mulbibob_cmd_macs: lock\n");
#endif

  /* lock all bibob monitor threads */
  mulbibob_lock (mulbibob);

  unsigned failure=0;
  for (i=0; i<nbobs; i++)
  {
    bibob_thread_t* thread = mulbibob->threads + bibobs[i];

#ifdef _DEBUG
  fprintf (stderr, "mulbibob_cmd_macs: configuring bibob %u with %s\n",
                   bibobs[i], macs[i]);
#endif
 
    if ( bibob_configure (thread->bibob) < 0 )
    {
      fprintf (fptr, "failed to configure bibob %u with mac address %s\n",
                     bibobs[i], macs[i]);
      failure = 1;
    }
  }

  mulbibob_unlock (mulbibob);
    
#ifdef _DEBUG
  fprintf (stderr, "mulbibob_cmd_macs: unlock\n");
#endif

  if (failure)
    return -1;
  else
    return 0;

}


/*! set the gain of a channel and pol */
int mulbibob_cmd_gain(void* context, FILE* fptr, char* args)
{
  mulbibob_t* mulbibob = context;

  int chan = -1;
  int pol  = -1;
  int val  = -1;

  /* parse the arguements of form <chan> <pol> [val] */
  const char *sep = " ";
  char *saveptr;
  char *line;

  line = strtok_r(args, sep, &saveptr);
  chan = atoi(line);

  line = strtok(NULL, sep);
  pol  = atoi(line);  

  /* if no val is present, this is considered a request for gains */
  line = strtok(NULL, sep);
  if (line != NULL)
    val  = atoi(line);  
  else
    val = -1;

  /* now apply this value to the specified channel */
  char buffer[128];
  if (val > 0) 
    sprintf(buffer, "regwrite pasp/reg_coeff_pol%d_chan%d %d", pol, chan, val);
  else 
    sprintf(buffer, "regread pasp/reg_coeff_pol%d_chan%d", pol, chan);

  mulbibob_lock (mulbibob);

  mulbibob_send (mulbibob, buffer);



  mulbibob_unlock (mulbibob);

  return 0;
}


/*! set the bit levels on all bibobs */
int mulbibob_cmd_levels(void* context, FILE* fptr, char* args)
{
  mulbibob_t* mulbibob = context;

#ifdef _DEBUG 
  fprintf (stderr, "mulbibob_cmd_levels: lock\n");
#endif
  mulbibob_lock (mulbibob);

  unsigned bibob = 0;
  pthread_t * bram_threads = malloc (sizeof(pthread_t) * mulbibob->nthread);

  bibob_thread_t* thread;

  /* test that all bibobs are open */
  for (bibob=0; bibob < mulbibob->nthread; bibob++)
  {
    thread = mulbibob->threads + bibob;
    if (! bibob_is_open(thread->bibob) )
    {
      fprintf (fptr, "bibob %s was not open\n", thread->bibob->host);
      mulbibob_unlock (mulbibob);
      return -1;
    }
  }

  for (bibob=0; bibob < mulbibob->nthread; bibob++)
  {
    thread = mulbibob->threads + bibob;

#ifdef _DEBUG 
    fprintf (stderr, "mulbibob_cmd_levels: creating thread for bibob %s\n",thread->bibob->host);
#endif

    int err = pthread_create (&(bram_threads[bibob]), 0, (void *) bibob_level_setter, thread->bibob);
    if (err)
      fprintf (stderr, "mulbibob_cmd_levels: error creating thread\n");

  } 

#ifdef _DEBUG 
  fprintf (stderr, "mulbibob_cmd_levels: joining threads\n");
#endif

  for (bibob=0; bibob < mulbibob->nthread; bibob++)
  {
    thread = mulbibob->threads + bibob;
    void* result = 0;
#ifdef _DEBUG
    fprintf (stderr, "mulbibob_cmd_levels: joining thread for bibob %s\n",thread->bibob->host);
#endif
    int err = pthread_join (bram_threads[bibob], &result);
    if (err) 
      fprintf (stderr, "mulbibob_cmd_levels: error joining thread for bibob %s\n",thread->bibob->host);

    fprintf (fptr, "%s : %u [%u,%u]\n",thread->bibob->host, 
             thread->bibob->pol1_bit, thread->bibob->pol1_coeff, 
             thread->bibob->pol2_coeff);

  }

#ifdef _DEBUG
  fprintf (stderr, "mulbibob_cmd_levels: unlock\n");
#endif
  mulbibob_unlock (mulbibob);

  free(bram_threads);

  return 0;
}




int mulbibob_send (mulbibob_t* mulbibob, const char* message)
{
  ssize_t length = 0;

#ifdef _DEBUG
  fprintf (stderr, "async send\n");
#endif

  unsigned bibob = 0;
  for (bibob = 0; bibob < mulbibob->nthread; bibob++)
  {
    bibob_thread_t* thread = mulbibob->threads + bibob;
    if (bibob_is_open (thread->bibob))
      length = bibob_send (thread->bibob, message);

#ifdef _DEBUG
    fprintf (stderr, "sent %d len=%u\n", bibob, length);
#endif
  }

  for (bibob = 0; bibob < mulbibob->nthread; bibob++)
  {
#ifdef _DEBUG
    fprintf (stderr, "recv %d\n", bibob);
#endif

    bibob_thread_t* thread = mulbibob->threads + bibob;
    if (bibob_is_open (thread->bibob)) 
    {
      bibob_recv (thread->bibob);
    }
  }
}

int mulbibob_arm (void* context)
{
  mulbibob_t* mulbibob = context;

#ifdef _DEBUG
  fprintf (stderr, "regwrite reg_arm 0\n");
#endif
  mulbibob_send (mulbibob, "regwrite reg_arm 0");

#ifdef _DEBUG
  fprintf (stderr, "regwrite reg_arm 1\n");
#endif
  mulbibob_send (mulbibob, "regwrite reg_arm 1");

}

/* defined in start_observation.c */
time_t start_observation( int(*start_routine)(void*), void* arg );

/*! reset packet counter on next UTC second, returned */
int mulbibob_cmd_arm (void* context, FILE* fptr, char* args)
{
  unsigned bibob = 0;
  mulbibob_t* mulbibob = context;

#ifdef _DEBUG
fprintf (stderr, "locking\n");
#endif

  mulbibob_lock (mulbibob);

#ifdef _DEBUG
fprintf (stderr, "locked\n");
#endif

  time_t utc = start_observation (mulbibob_arm, context);

#ifdef _DEBUG
fprintf (stderr, "started\n");
#endif
  mulbibob_unlock (mulbibob);

#ifdef _DEBUG
fprintf (stderr, "unlocked\n");
#endif

  char date[64];
  strftime (date, 64, DADA_TIMESTR, gmtime(&utc));

  fprintf (fptr, "%s\n", date);
}

/*! shutdown all bibobs and exit gracefully */
int mulbibob_cmd_quit (void* context, FILE* fptr, char* args)
{

  mulbibob_t* mulbibob = context;
  
  bibob_thread_t * thread;

  unsigned i = 0;

  /* ask all bibob threads to quit */
  for (i = 0; i < mulbibob->nthread; i++)
  {
    thread = mulbibob->threads + i;
    if (thread->id) 
      thread->quit = 1;
  }

  /* join specified bibob threads */
  for (i=0; i < mulbibob->nthread; i++)
  {
    thread = mulbibob->threads + i;
    if (thread->id) 
    {
      void* result = 0;
      pthread_join (thread->id, &result);
    }
  }

  mulbibob->server->quit = 1;

  return 0;
}

int mulbibob_cmd_listdev (void* context, FILE* fptr, char* args)
{
  mulbibob_t* mulbibob = context;

  fprintf (stderr, "listdev\n");
  mulbibob_send (mulbibob, "listdev");

  return 0;

}        

void* lock_thread (void* context)
{
  if (!context)
    return 0;

  bibob_thread_t* thread = context;
  if (pthread_mutex_lock (&(thread->mutex)) != 0)
    return 0;

  return context;
}

/*! mutex lock all of the bibob interfaces */
void mulbibob_lock (mulbibob_t* mulbibob)
{
  unsigned bibob = 0;

  /* quickly grab all locks at once */
  for (bibob = 0; bibob < mulbibob->nthread; bibob++)
  {
#ifdef _DEBUG
    fprintf (stderr, "mulbibob_lock: launch lock_thread %u\n", bibob);
#endif

    bibob_thread_t* thread = mulbibob->threads + bibob;
    int err = pthread_create (&(thread->lock), 0, lock_thread, thread);
    if (err)
      fprintf (stderr, "mulbibob_lock: error starting lock_thread %d - %s\n",
	       bibob, strerror (err));
  }

  /* wait for the locks to be made */
  for (bibob = 0; bibob < mulbibob->nthread; bibob++)
  {
    bibob_thread_t* thread = mulbibob->threads + bibob;

#ifdef _DEBUG
    fprintf (stderr, "mulbibob_lock: join lock_thread %u\n", bibob);
#endif

    void* result = 0;
    pthread_join (thread->lock, &result);
    if (result != thread)
      fprintf (stderr, "mulbibob_lock: error in lock_thread\n");
  }
}

/*! mutex unlock all of the bibob interfaces */
void mulbibob_unlock (mulbibob_t* mulbibob)
{
  unsigned bibob = 0;

  /* quickly grab all locks at once */
  for (bibob = 0; bibob < mulbibob->nthread; bibob++)
  {
    bibob_thread_t* thread = mulbibob->threads + bibob;
    pthread_mutex_unlock (&(thread->mutex));
  }
}

/*! */
int mulbibob_serve (mulbibob_t* mulbibob)
{
  if (!mulbibob)
    return -1;

  if (mulbibob->port)
  {
    if (mulbibob->server)
    {
      fprintf (stderr, "mulbibob_serve: server already launched \n");
      return -1;
    }

    mulbibob->server = command_parse_server_create (mulbibob -> parser);

    command_parse_server_set_welcome (mulbibob -> server,
				      "mulbibob command");

    /* open the command/control port */
    command_parse_serve (mulbibob->server, mulbibob->port);

    void* result = 0;
    pthread_join (mulbibob->server->thread, &result);
  }
  else
  {
    fprintf (stderr, "mulbibob_serve: stdin/out interface not implemented \n");
    return -1;
  }
}

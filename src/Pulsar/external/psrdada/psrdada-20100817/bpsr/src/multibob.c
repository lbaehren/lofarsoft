/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "multibob.h"
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


// #define _DEBUG 1

void ibob_thread_init (ibob_thread_t* ibob, int number)
{
  ibob->ibob = ibob_construct ();
  ibob_set_number (ibob->ibob, number);

  pthread_mutex_init(&(ibob->mutex), NULL);
  pthread_cond_init (&(ibob->cond), NULL);

  ibob->id = 0;
  ibob->bramdump = 1;
  ibob->bramdisk = 1;
  ibob->alive = 0;

  ibob->quit = 1;
}

/*! allocate and initialize a new ibob_t struct */
multibob_t* multibob_construct (unsigned nibob)
{
  multibob_t* multibob = malloc (sizeof(multibob_t));

  multibob->threads = malloc (sizeof(ibob_thread_t) * nibob);
  multibob->nthread = nibob;

  unsigned ibob = 0;
  ibob_thread_t* thread;

  for (ibob = 0; ibob < nibob; ibob++) {
    ibob_thread_init (multibob->threads + ibob, ibob + 1);
    thread = multibob->threads + ibob;
  }

  multibob->parser = command_parse_create ();
  multibob->server = 0;
  multibob->port = 0;

  command_parse_add (multibob->parser, multibob_cmd_state, multibob,
                     "state", "get the current state", NULL);

  command_parse_add (multibob->parser, multibob_cmd_hostport, multibob,
                     "hostport", "set the hostname and port number", NULL);

  command_parse_add (multibob->parser, multibob_cmd_hostports, multibob,
                     "hostports", "set n hostnames and ports number", NULL);

  command_parse_add (multibob->parser, multibob_cmd_mac, multibob,
                     "mac", "set the target MAC address", NULL);

  command_parse_add (multibob->parser, multibob_cmd_macs, multibob,
                     "macs", "set multiple target MAC addresses", NULL);

  command_parse_add (multibob->parser, multibob_cmd_acclen, multibob,
                     "acclen", "set accumulation length", NULL);

  command_parse_add (multibob->parser, multibob_cmd_open, multibob,
                     "open", "open command interface connections", NULL);

  command_parse_add (multibob->parser, multibob_cmd_close, multibob,
                     "close", "close command interface connections", NULL);

  command_parse_add (multibob->parser, multibob_cmd_arm, multibob,
                     "arm", "reset packet count", NULL);

  command_parse_add (multibob->parser, multibob_cmd_levels, multibob,
                     "levels", "set all ibob levels", NULL);

  command_parse_add (multibob->parser, multibob_cmd_quit, multibob,
                     "quit", "quit", NULL);

  return multibob;
}

/*! free all resources reserved for ibob communications */
int multibob_destroy (multibob_t* multibob)
{

  command_parse_destroy(multibob->parser);
  command_parse_server_destroy(multibob->server);

  unsigned ibob=0;
  for (ibob = 0; ibob < multibob->nthread; ibob++) {
    ibob_destroy (multibob->threads[ibob].ibob);
  }

  free(multibob->threads);
  free(multibob);
}

/* contact the iBoB and accumulate a bramdump */
int bramdump (ibob_t* ibob)
{
  return ibob_bramdump(ibob);
}

/* write the accumulated bramdumps to disk, copying to the NFS server */
int bramdisk(ibob_t* ibob)
{
  return ibob_bramdisk(ibob);
}

/*!
  The monitor thread simply sits in a loop, opening the connection
  as necessary, and polling the connection every second.  Polling
  can be either a simple ping or a bramdump.  On failure, the
  connection is closed and re-opened ad infinitum every five seconds.
*/

void* multibob_monitor (void* context)
{
  if (!context)
    return 0;

  ibob_thread_t* thread = context;
  ibob_t* ibob = thread->ibob;

  while (!thread->quit)
  {
    pthread_mutex_lock (&(thread->mutex));

    fprintf (stderr, "multibob_monitor: opening %s:%d\n",
             ibob->host, ibob->port);

    if ( !ibob_is_open(ibob) && ibob_open (ibob) < 0 )
    {
      fprintf (stderr, "multibob_monitor: could not open %s:%d - %s\n",
	       ibob->host, ibob->port, strerror(errno));

      pthread_mutex_unlock (&(thread->mutex));

      if (thread->quit)
        break;

      sleep (5);
      continue;
    }

    pthread_mutex_unlock (&(thread->mutex));

    double disk_interval = 10.0;  // 10 seconds between disk dumps
    unsigned n_dump = 32;         // number of bram dumps per disk dump

#ifdef _DEBUG
    fprintf (stderr, "multibob_monitor: %s ibob alive\n", ibob->host);
#endif

    while (!thread->quit)
    {
      int retval = 0;

      struct timeval time1;
      gettimeofday (&time1, 0);

      pthread_mutex_lock (&(thread->mutex));

      if ( (thread->bramdump) && (ibob_is_open(ibob)) )
      {
        retval = bramdump (ibob);
        if ( (thread->bramdisk) && (thread->ibob->n_brams >= n_dump) )
          bramdisk (thread->ibob);
      }
      else
        retval = ibob_ping (ibob);

      pthread_mutex_unlock (&(thread->mutex));

      struct timeval time2;
      gettimeofday (&time2, 0);

      if (retval < 0)
      {
        fprintf (stderr, "multibob_monitor: communicaton failure on %s:%d\n",
                 ibob->host, ibob->port);
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

      // ibob_pause accepts sleep in milliseconds
      ibob_pause( (int)(sleep_in_sec * 1e3) );
    }

    fprintf (stderr, "multibob_monitor: closing connection with %s:%d\n",
             ibob->host, ibob->port);

    ibob_close (ibob);
  }

  return 0;
}

/*! open the command connections to all of the ibobs */
int multibob_cmd_open (void* context, FILE* fptr, char* args)
{
  if (!context)
    return -1;

  multibob_t* multibob = context;

  unsigned ibob = 0;
  for (ibob = 0; ibob < multibob->nthread; ibob++)
  {
    ibob_thread_t* thread = multibob->threads + ibob;

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

    int err = pthread_create (&(thread->id), 0, multibob_monitor, thread);
    if (err)
      fprintf (stderr, "multibob_cmd_open: error starting thread %d - %s\n",
	       ibob, strerror (err));
  }
}

/*! close the command connections to all of the ibobs */
int multibob_cmd_close (void* context, FILE* fptr, char* args)
{
  if (!context)
    return -1;

  unsigned ibob = 0;
  multibob_t* multibob = context;

  for (ibob = 0; ibob < multibob->nthread; ibob++)
    multibob->threads[ibob].quit = 1;
}

/*! reset packet counter on next UTC second, returned */
int multibob_cmd_state (void* context, FILE* fptr, char* args)
{
  unsigned ibob = 0;
  multibob_t* multibob = context;

  time_t current = time(0);

  for (ibob = 0; ibob < multibob->nthread; ibob++)
  {
    ibob_thread_t* thread = multibob->threads + ibob;
    fprintf (fptr, "IBOB%02d %s:%d ", ibob+1, 
	     thread->ibob->host, thread->ibob->port);

    if (thread->quit)
      fprintf (fptr, "closed");

    else if (current - thread->alive < 5)
      fprintf (fptr, "alive");

    else
      fprintf (fptr, "dead");

    fprintf (fptr, "\n");
  }
}

/*! set the host and port number of the specified ibob */
int multibob_cmd_hostport (void* context, FILE* fptr, char* args)
{
  multibob_t* multibob = context;

  unsigned ibob = 0;
  char host[64];
  unsigned port = 0;

  if (!args)
  {
    fprintf (fptr, "no args specified. usage: ibob host port\n");
    return -1;
  }

  if (sscanf (args, "%u %s %u", &ibob, host, &port) != 3) 
  {
    fprintf (fptr, "hostport command failed. Argument mismatch\n");
    return -1;
  } 

  if ((ibob < 0) || (ibob >= multibob->nthread)) 
  {
    fprintf (fptr, "ibob number mismatch [0 - %d]\n",multibob->nthread);
    return -1;
  }

  ibob_thread_t* thread = multibob->threads + ibob;

  /* tell monitor_thread for this ibob to exit, closing the ibob */
  thread->quit = 1;
  void* result = 0;
  pthread_join (thread->id, &result);

  /* update the host and port for this ibob */
  if ( ibob_set_host (thread->ibob, host, port) < 0 )
  {
    fprintf (fptr, "failed to set ibob host:port %s:%d\n",host,port);
    return -1;
  }

  /* restart the ibob */

  thread->id = 0;
  thread->quit = 0;

  int err = pthread_create (&(thread->id), 0, multibob_monitor, thread);
  if (err)
    fprintf (fptr, "multibob_cmd_open: error starting thread %d - %s\n",
       ibob, strerror (err));

  return 0;

}

/*! set the host and port number of the specified ibob */
int multibob_cmd_hostports (void* context, FILE* fptr, char* args)
{
  multibob_t* multibob = context;

  ibob_thread_t* thread;
  
  unsigned ibob  = 0;
  unsigned nbobs = 0;
  int * ibobs;
  char ** hosts;
  int * ports;
  unsigned i = 0;

  /* if no args specified, print current settings */
  if (!args)
  {
    char response[1024];
    int j=0;
    j += sprintf(response, "%d", multibob->nthread);

    for (i=0; i< multibob->nthread; i++)
    {
      thread = multibob->threads + i;
      j += sprintf(response + j, " %d %s %d", i, thread->ibob->host, thread->ibob->port);
    }
    response[j] = '\n';
    response[j+1] = '\0';
    fprintf(fptr, response);

    return 0;
  }

  // extract arguemnts of the form "nbob [ibob ibob_host ibob_port]+"
  const char *sep = " ";
  char *line;

  line = strtok(args, sep);
  nbobs = atoi(line);

  if ((nbobs <= 0) || (nbobs > multibob->nthread)) 
  {
    fprintf (fptr, "hostports command failed. Argument mismatch\n");
    return -1;
  }

  ibobs = malloc(sizeof(int) * nbobs);
  hosts = malloc(sizeof(char*) * nbobs);
  ports = malloc(sizeof(int) * nbobs);
  
  for (i=0; i<nbobs; i++) 
  {

    line = strtok(NULL, sep);
    ibobs[i] = atoi(line);

    if ((ibobs[i] < 0) || (ibobs[i] >= multibob->nthread))
    {
      fprintf (fptr, "ibob number mismatch [0 - %d]\n",multibob->nthread);
      return -1;
    }

    line = strtok(NULL, sep);
    hosts[i] = strdup(line);

    line = strtok(NULL, sep);
    ports[i] = atoi(line);

  }

  /* ask specified ibob threads to quit */
  for (i=0; i < nbobs; i++) 
  {
    thread = multibob->threads + ibobs[i];
    if (ibob_is_open(thread->ibob)) 
    {
      thread->quit = 1;
      void* result = 0;
      pthread_join (thread->id, &result);
    }
  }

  unsigned failure = 0;
  /* update the host and port for specified ibob threads */
  for (i=0; i < nbobs; i++)
  {
    thread = multibob->threads + ibobs[i];
    if ( ibob_set_host (thread->ibob, hosts[i], ports[i]) < 0 ) 
    {
      fprintf (fptr, "failed to set ibob host:port %s:%d\n",hosts[i],ports[i]);
      failure = 1;
    }
  } 

  /* restart the specified ibob threads */
  for (i=0; i<nbobs; i++)
  {
    thread = multibob->threads + ibobs[i];
    thread->id = 0;
    thread->quit = 0;

    int err = pthread_create (&(thread->id), 0, multibob_monitor, thread);
    if (err)
      fprintf (fptr, "multibob_cmd_hostports: error starting thread %d - %s\n",
        ibobs[i], strerror (err));
  }

  /* cleanup mallocs */
  for (i=0; i<nbobs; i++)
  {
    free(hosts[i]);
  }

  free(ibobs);
  free(hosts);
  free(ports);

  return 0;
                                                                                                                                        
}

/*! set the target MAC address of the specified ibob */
int multibob_cmd_mac (void* context, FILE* fptr, char* args)
{
  multibob_t* multibob = context;

  unsigned ibob = 0;
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

  if (sscanf (args, "%u %s", &ibob, mac_address) != 2)
  {
    fprintf (fptr, "mac command failed. Argument mismatch\n");
    return -1;
  }

  if ((ibob < 0) || (ibob >= multibob->nthread))
  {
    fprintf (fptr, "ibob number mismatch [0 - %d]\n",multibob->nthread);
    return -1;
  }

  if (strlen(mac_address) != 12)
  {
    fprintf (fptr, "mac address was not 12 chars long\n");
    return -1;
  }

  ibob_thread_t* thread = multibob->threads + ibob;

#ifdef _DEBUG
  fprintf (stderr, "multibob_cmd_mac: lock iBoB %u\n", ibob);
#endif

  /* lock the ibob for communication */
  if (pthread_mutex_lock (&(thread->mutex)) != 0)
  {
    fprintf (fptr, "multibob_cmd_mac: could not lock iBoB\n");
    return -1;
  }

  if ( !ibob_is_open(thread->ibob) )
  {
    fprintf (fptr, "ibob %u is not open\n", ibob);
    pthread_mutex_unlock (&(thread->mutex));
    return -1;
  }

  if ( ibob_configure (thread->ibob, mac_address) < 0 )
  {
    fprintf (fptr, "failed to configure ibob with mac address %s\n",mac_address);
    pthread_mutex_unlock (&(thread->mutex));
    return -1;
  }
  
#ifdef _DEBUG
  fprintf (stderr, "multibob_cmd_mac: unlock iBoB %u\n", ibob);
#endif
 
  pthread_mutex_unlock (&(thread->mutex));

  return 0;
                                                                                                                                        
}
                                                                                                                                        

/*! set the target MAC address of the specified ibobs */
int multibob_cmd_macs (void* context, FILE* fptr, char* args)
{
  multibob_t* multibob = context;
  
  unsigned ibob = 0;
  unsigned nbobs = 0;
  int * ibobs;
  char ** macs;

  char mac_address[16];

  /* if no args specified, print current settings */
  if (!args)
  {
    fprintf(fptr, "No args specified\n");
    return -1;
  }

  // extract arguemnts of the form "nbob [ibob ibob_host ibob_port]+"
  const char *sep = " ";
  char *saveptr;
  char *line;

  line = strtok_r(args, sep, &saveptr);
  nbobs = atoi(line);

  if ((nbobs <= 0) || (nbobs > multibob->nthread))
  {
    fprintf (fptr, "macs command failed. Argument mismatch\n");
    return -1;
  }

  ibobs = malloc(sizeof(int) * nbobs);
  macs = malloc(sizeof(char*) * nbobs);

  unsigned i=0;
  for (i=0; i<nbobs; i++)
  {
    line = strtok_r(NULL, sep, &saveptr);
    if (line == NULL)
    {
       fprintf (fptr, "syntax error for ibob %d\n", i);
       return -1;
    }

    ibobs[i] = atoi(line);

    if ((ibobs[i] < 0) || (ibobs[i] >= multibob->nthread))
    {
      fprintf (fptr, "ibob number %d mismatch [0 - %d]\n",ibobs[i],multibob->nthread);
      return -1;
    }
                                                                                                                                        
    line = strtok_r(NULL, sep, &saveptr);
    if (line == NULL)
    {
      fprintf (fptr, "syntax error for ibob %u\n", ibobs[i]);
      return -1;
    }

    macs[i] = strdup(line);

    if (strlen(macs[i]) != 12) 
    {
      fprintf (fptr, "ibob %u mac address incorrect %s\n", ibobs[i], macs[i]);
      return -1;
    }
  }

#ifdef _DEBUG
  fprintf (stderr, "multibob_cmd_macs: lock\n");
#endif

  /* lock all ibob monitor threads */
  multibob_lock (multibob);

  unsigned failure=0;
  for (i=0; i<nbobs; i++)
  {
    ibob_thread_t* thread = multibob->threads + ibobs[i];

#ifdef _DEBUG
  fprintf (stderr, "multibob_cmd_macs: configuring ibob %u with %s\n",
                   ibobs[i], macs[i]);
#endif
 
    if ( ibob_configure (thread->ibob, macs[i]) < 0 )
    {
      fprintf (fptr, "failed to configure ibob %u with mac address %s\n",
                     ibobs[i], macs[i]);
      failure = 1;
    }
  }

  multibob_unlock (multibob);
    
#ifdef _DEBUG
  fprintf (stderr, "multibob_cmd_macs: unlock\n");
#endif

  if (failure)
    return -1;
  else
    return 0;

}


/*! set the bit levels on all ibobs */
int multibob_cmd_levels(void* context, FILE* fptr, char* args)
{
  multibob_t* multibob = context;

#ifdef _DEBUG 
  fprintf (stderr, "multibob_cmd_levels: lock\n");
#endif
  multibob_lock (multibob);

  unsigned ibob = 0;
  pthread_t * bram_threads = malloc (sizeof(pthread_t) * multibob->nthread);

  ibob_thread_t* thread;

  /* test that all ibobs are open */
  for (ibob=0; ibob < multibob->nthread; ibob++)
  {
    thread = multibob->threads + ibob;
    if (! ibob_is_open(thread->ibob) )
    {
      fprintf (fptr, "ibob %s was not open\n", thread->ibob->host);
      multibob_unlock (multibob);
      return -1;
    }
  }

  for (ibob=0; ibob < multibob->nthread; ibob++)
  {
    thread = multibob->threads + ibob;

#ifdef _DEBUG 
    fprintf (stderr, "multibob_cmd_levels: creating thread for ibob %s\n",thread->ibob->host);
#endif

    int err = pthread_create (&(bram_threads[ibob]), 0, (void *) ibob_level_setter, thread->ibob);
    if (err)
      fprintf (stderr, "multibob_cmd_levels: error creating thread\n");

  } 

#ifdef _DEBUG 
  fprintf (stderr, "multibob_cmd_levels: joining threads\n");
#endif

  for (ibob=0; ibob < multibob->nthread; ibob++)
  {
    thread = multibob->threads + ibob;
    void* result = 0;
#ifdef _DEBUG
    fprintf (stderr, "multibob_cmd_levels: joining thread for ibob %s\n",thread->ibob->host);
#endif
    int err = pthread_join (bram_threads[ibob], &result);
    if (err) 
      fprintf (stderr, "multibob_cmd_levels: error joining thread for ibob %s\n",thread->ibob->host);

    fprintf (fptr, "%s : %u [%u,%u]\n",thread->ibob->host, thread->ibob->bit_window, 
                                       thread->ibob->pol1_coeff, thread->ibob->pol2_coeff);

  }

#ifdef _DEBUG
  fprintf (stderr, "multibob_cmd_levels: unlock\n");
#endif
  multibob_unlock (multibob);

  free(bram_threads);

  return 0;
}




/*! set the target MAC address of the specified ibobs */
int multibob_cmd_acclen (void* context, FILE* fptr, char* args)
{
  multibob_t* multibob = context;

  unsigned acclen = 0;
  uint64_t sync_period = 0;
  char command[64];

  if (!args)
  {
    fprintf (fptr, "no args specified. usage: acclen length\n");
    return -1;
  }
                                                                                                   
  if (sscanf (args, "%u", &acclen) != 1)
  {
    fprintf (fptr, "acclen command failed. Argument mismatch\n");
    return -1;
  }

  sync_period = 100 * acclen * 2048;

  multibob_lock (multibob);

  sprintf(command, "regwrite reg_acclen %d", (acclen-1));
  multibob_send (multibob, command);

  sprintf(command, "regwrite reg_sync_period %"PRIu64, sync_period);
  multibob_send (multibob, command);

  multibob_unlock (multibob);

  return 0;
}



int multibob_send (multibob_t* multibob, const char* message)
{
  ssize_t length = 0;

#ifdef _DEBUG
  fprintf (stderr, "async send\n");
#endif

  unsigned ibob = 0;
  for (ibob = 0; ibob < multibob->nthread; ibob++)
  {
    ibob_thread_t* thread = multibob->threads + ibob;
    if (ibob_is_open (thread->ibob))
      length = ibob_send_async (thread->ibob, message);

#ifdef _DEBUG
    fprintf (stderr, "sent %d len=%u\n", ibob, length);
#endif
  }

#ifdef _DEBUG
  fprintf (stderr, "receive echo length %u\n", length);
#endif

  for (ibob = 0; ibob < multibob->nthread; ibob++)
  {
#ifdef _DEBUG
    fprintf (stderr, "recv %d: %u\n", ibob, length);
#endif

    ibob_thread_t* thread = multibob->threads + ibob;
    if (ibob_is_open (thread->ibob)) 
    {
      ibob_recv_echo (thread->ibob, length);
      ibob_ignore (thread->ibob);
    }
  }
}

int multibob_arm (void* context)
{
  multibob_t* multibob = context;

#ifdef _DEBUG
  fprintf (stderr, "regwrite reg_arm 0\n");
#endif
  multibob_send (multibob, "regwrite reg_arm 0");

#ifdef _DEBUG
  fprintf (stderr, "regwrite reg_arm 1\n");
#endif
  multibob_send (multibob, "regwrite reg_arm 1");

}

/* defined in start_observation.c */
time_t start_observation( int(*start_routine)(void*), void* arg );

/*! reset packet counter on next UTC second, returned */
int multibob_cmd_arm (void* context, FILE* fptr, char* args)
{
  unsigned ibob = 0;
  multibob_t* multibob = context;

#ifdef _DEBUG
fprintf (stderr, "locking\n");
#endif

  multibob_lock (multibob);

#ifdef _DEBUG
fprintf (stderr, "locked\n");
#endif

  time_t utc = start_observation (multibob_arm, context);

#ifdef _DEBUG
fprintf (stderr, "started\n");
#endif
  multibob_unlock (multibob);

#ifdef _DEBUG
fprintf (stderr, "unlocked\n");
#endif

  char date[64];
  strftime (date, 64, DADA_TIMESTR, gmtime(&utc));

  fprintf (fptr, "%s\n", date);
}

/*! shutdown all ibobs and exit gracefully */
int multibob_cmd_quit (void* context, FILE* fptr, char* args)
{

  multibob_t* multibob = context;
  
  ibob_thread_t * thread;

  unsigned i = 0;

  /* ask all ibob threads to quit */
  for (i = 0; i < multibob->nthread; i++)
  {
    thread = multibob->threads + i;
    if (thread->id) 
      thread->quit = 1;
  }

  /* join specified ibob threads */
  for (i=0; i < multibob->nthread; i++)
  {
    thread = multibob->threads + i;
    if (thread->id) 
    {
      void* result = 0;
      pthread_join (thread->id, &result);
    }
  }

  multibob->server->quit = 1;

  return 0;
}

void* lock_thread (void* context)
{
  if (!context)
    return 0;

  ibob_thread_t* thread = context;
  if (pthread_mutex_lock (&(thread->mutex)) != 0)
    return 0;

  return context;
}

/*! mutex lock all of the ibob interfaces */
void multibob_lock (multibob_t* multibob)
{
  unsigned ibob = 0;

  /* quickly grab all locks at once */
  for (ibob = 0; ibob < multibob->nthread; ibob++)
  {
#ifdef _DEBUG
    fprintf (stderr, "multibob_lock: launch lock_thread %u\n", ibob);
#endif

    ibob_thread_t* thread = multibob->threads + ibob;
    int err = pthread_create (&(thread->lock), 0, lock_thread, thread);
    if (err)
      fprintf (stderr, "multibob_lock: error starting lock_thread %d - %s\n",
	       ibob, strerror (err));
  }

  /* wait for the locks to be made */
  for (ibob = 0; ibob < multibob->nthread; ibob++)
  {
    ibob_thread_t* thread = multibob->threads + ibob;

#ifdef _DEBUG
    fprintf (stderr, "multibob_lock: join lock_thread %u\n", ibob);
#endif

    void* result = 0;
    pthread_join (thread->lock, &result);
    if (result != thread)
      fprintf (stderr, "multibob_lock: error in lock_thread\n");
  }
}

/*! mutex unlock all of the ibob interfaces */
void multibob_unlock (multibob_t* multibob)
{
  unsigned ibob = 0;

  /* quickly grab all locks at once */
  for (ibob = 0; ibob < multibob->nthread; ibob++)
  {
    ibob_thread_t* thread = multibob->threads + ibob;
    pthread_mutex_unlock (&(thread->mutex));
  }
}

/*! */
int multibob_serve (multibob_t* multibob)
{
  if (!multibob)
    return -1;

  if (multibob->port)
  {
    if (multibob->server)
    {
      fprintf (stderr, "multibob_serve: server already launched \n");
      return -1;
    }

    multibob->server = command_parse_server_create (multibob -> parser);

    command_parse_server_set_welcome (multibob -> server,
				      "multibob command");

    /* open the command/control port */
    command_parse_serve (multibob->server, multibob->port);

    void* result = 0;
    pthread_join (multibob->server->thread, &result);
  }
  else
  {
    fprintf (stderr, "multibob_serve: stdin/out interface not implemented \n");
    return -1;
  }
}

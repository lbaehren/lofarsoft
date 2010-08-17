#include "monitor.h"

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include <stdlib.h>
#include <assert.h>

// #define _DEBUG 1

/*! Create a new monitor */
monitor_t* monitor_create ()
{
  monitor_t* monitor = (monitor_t*) malloc (sizeof(monitor_t));
  assert (monitor != 0);

  monitor->nexus = 0;
  monitor->log = 0;
  monitor->handle_message = 0;

  return monitor;
}

/*! Destroy a monitor */
int monitor_destroy (monitor_t* monitor)
{
  free (monitor);
  return 0;
}

void* monitor_thread (void* context)
{
  monitor_t* monitor = (monitor_t*) context;
  unsigned nnode = 0;
  unsigned inode = 0;
  node_t* node = 0;

  fd_set readset;
  int maxfd = 0;
  int fd = 0;

  unsigned buffer_size = 1024;
  char* buffer = malloc (buffer_size);
  assert (buffer != 0);

  /* For logging to local file */
  FILE* pwcc_logfile = 0;   

  /* Create a multi log to view PWC collated PWC messages */
  monitor->log = multilog_open ("dada_pwc_command_logger", 0);

  /* Turn of timestamping as they will already be stamped by the pwc */
  monitor->log->timestamp = 0;

  multilog_serve (monitor->log, monitor->nexus->multilog_port); 

#ifdef _DEBUG
  fprintf (stderr, "monitor_thread start nexus=%p\n", monitor->nexus);
#endif

  /* If we are logging messages to file */
  if (monitor->nexus->logfile_dir) {
    sprintf(buffer,"%s/nexus.pwc.log",monitor->nexus->logfile_dir);
    pwcc_logfile = fopen(buffer,"a");
    if (!pwcc_logfile) {
      fprintf (stderr, "Could not open pwcc log file: ");
      perror(buffer);
    }
  }

  while (monitor->nexus) {

    FD_ZERO (&readset);
    maxfd = 0;

    pthread_mutex_lock (&(monitor->nexus->mutex));
    nnode = monitor->nexus->nnode;

#ifdef _DEBUG
    fprintf (stderr, "monitor_thread %u nodes\n", nnode);
#endif

    /* First we add the nodes to readset for select polling */
    for (inode = 0; inode < nnode; inode++) {
     
      node = monitor->nexus->nodes[inode];
      if (node->from) {
#ifdef _DEBUG
        fprintf (stderr, "monitor_thread add %d\n", fileno(node->from));
#endif
        if (feof(node->from)) {
          fprintf(stderr, "monitor_thread: lost connection with %d\n",fileno(node->from));
          node->to = 0;
          node->from = 0;
          node->log = 0;
        } else {
          fd = fileno(node->from);
          if (fd > maxfd)
            maxfd = fd;
          FD_SET (fd, &readset);
        }
      }
    }

    pthread_mutex_unlock (&(monitor->nexus->mutex));

    if (maxfd > 0) {
      if (select (maxfd + 1, &readset, NULL, NULL, NULL) < 0) {
        perror ("monitor_thread: select");
        free (buffer);
        if (pwcc_logfile) fclose(pwcc_logfile);
        return 0;
      }

      pthread_mutex_lock (&(monitor->nexus->mutex));
      nnode = monitor->nexus->nnode;
      for (inode = 0; inode < nnode; inode++) {

        node = monitor->nexus->nodes[inode];
        
        /* If this node has been setup yet */
        if (node->from) {
#ifdef _DEBUG
          fprintf (stderr, "monitor_thread check %d\n", fileno(node->from));
#endif
          if (FD_ISSET (fileno(node->from), &readset))
            break;
        }
      }
      pthread_mutex_unlock (&(monitor->nexus->mutex));

      if (inode == nnode)
        fprintf (stderr, "monitor_thread: select returns, but no FD_ISSSET\n");
      else {
        fgets (buffer, buffer_size, node->from);

        /* If the connection dies to the pwc, reset the to/from FILE ptrs */
        if (feof(node->from)) {
          fprintf(stderr, "lost connection with %d\n",fileno(node->from));
          node->to = 0;
          node->from = 0;
          node->log = 0;
        }

#ifdef _DEBUG
        fprintf (stderr, "%u: %s", node->host, buffer);
#endif
        if (node->log) {
          fprintf(node->log,"%s",buffer);
          fflush(node->log);
        }
  
        if (pwcc_logfile) {
          fprintf(pwcc_logfile,"%s: %s",node->host,buffer);
          fflush(pwcc_logfile);
        }
        
        if (monitor->log)
          multilog (monitor->log, LOG_INFO, "%s: %s", node->host, buffer);

        if (monitor->handle_message)
          monitor->handle_message (monitor->context, inode, buffer);
      }

    } else {
      sleep(1);
    }

  }
  
#ifdef _DEBUG
  fprintf (stderr, "monitor_thread exit\n");
#endif

  free (buffer);
  if (pwcc_logfile) fclose(pwcc_logfile);
  return 0;
}


/*! Start another thread to monitor messages from the nexus */
int monitor_launch (monitor_t* monitor)
{
  pthread_t tmp_thread;

  if (pthread_create (&tmp_thread, 0, monitor_thread, monitor) < 0) {
    perror ("monitor_launch: Error creating new thread");
    return -1;
  }

  /* thread cannot be joined; resources will be destroyed on exit */
  pthread_detach (tmp_thread);

  return 0;
}


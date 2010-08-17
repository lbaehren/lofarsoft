#include "dada_def.h"
#include "dada_msg.h"

#include "nexus.h"
#include "ascii_header.h"
#include "futils.h"
#include "sock.h"
#include "multilog.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>

// #define _DEBUG 1

void node_init (node_t* node)
{
  node -> host = 0;
  node -> port = 0;
  node -> id = -1;
  node -> to = 0;
  node -> from = 0;
  node -> log = 0;
}

/*! Create a new node */
node_t* node_create ()
{
  node_t* node = (node_t*) malloc (sizeof(node_t));
  assert (node != 0);
  node_init (node);
  return node;
}

void nexus_init (nexus_t* nexus)
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init (&attr);
  pthread_mutexattr_settype (&attr, DADA_MUTEX_RECURSIVE);
  pthread_mutex_init(&(nexus->mutex), &attr);

  /* no nodes */
  nexus -> nodes = 0;
  nexus -> nnode = 0;

  /* default prefix */
  nexus -> node_prefix = strdup ("NODE");
  assert (nexus->node_prefix != 0);

  /* default polling interval */
  nexus -> polling_interval = 10;

  /* no default port */
  nexus -> node_port = 0;

  /* default receive buffer */
  nexus -> recv_buffer = malloc (NEXUS_DEFAULT_RECV_BUFSZ);
  nexus -> recv_bufsz = NEXUS_DEFAULT_RECV_BUFSZ;

  /* default creator */
  nexus -> node_create = &node_create;

  /* default parser */
  nexus -> nexus_parse = &nexus_parse;

  /* no node initialization by default */
  nexus -> node_init = 0;

  /* no mirror by default */
  nexus -> mirror = 0;

  /* no file logging be default */
  nexus -> logfile_dir = 0;
  nexus -> multilog_port = 0;
  
}

/*! Create a new nexus */
nexus_t* nexus_create ()
{
  nexus_t* nexus = (nexus_t*) malloc (sizeof(nexus_t));
  assert (nexus != 0);
  nexus_init (nexus);
  return nexus;
}


/*! Destroy a nexus */
int nexus_destroy (nexus_t* nexus)
{
  node_t* node = 0;
  unsigned inode = 0;

  if (!nexus)
    return -1;

  for (inode = 0; inode < nexus->nnode; inode++) {
    node = (node_t*) nexus->nodes[inode];
    if (node->host)
      free (node->host);
    if (node->to)
      fclose (node->to);
    if (node->from)
      fclose (node->from);
    if (node->log)
      fclose (node->log);
    free (node);
  }

  if (nexus->nodes)
    free (nexus->nodes);

  pthread_mutex_destroy (&(nexus->mutex));

  return 0;
}

unsigned nexus_get_nnode (nexus_t* nexus)
{
  return nexus->nnode;
}


/* Parses the supplied config file to extract required params */
int nexus_parse (nexus_t* n, const char* buffer)
{
  char node_name [16];
  char host_name [64];

  unsigned inode, nnode = 0;

  // Get the PWC control port from the config file
  sprintf (node_name, "%s_PORT", n->node_prefix);
  if (ascii_header_get (buffer, node_name, "%d", &(n->node_port)) < 0) {
    fprintf (stderr, "nexus_parse: %s not specified.", node_name);
    if (n->node_port)
      fprintf (stderr, " using default=%d\n", n->node_port);
    else {
      fprintf (stderr, " no default available\n");
      return -1;
    }
  }

  if (ascii_header_get (buffer, "COM_POLL", "%d", &(n->polling_interval)) <0) {
    multilog_fprintf (stderr, LOG_INFO, "nexus_parse: using default COM_POLL\n");
    n->polling_interval = 10;
  }

  sprintf (node_name, "NUM_%s", n->node_prefix);
  if (ascii_header_get (buffer, node_name, "%u", &nnode) < 0) {
    fprintf (stderr, "nexus_parse: no %s keyword in config!\n", node_name);
    nnode = 0;
  }

  if (!nnode)
    fprintf (stderr, "nexus_parse: WARNING no Nodes!\n");

  for (inode=0; inode < nnode; inode++) {

    sprintf (node_name, "%s_%u", n->node_prefix, inode);

    if (ascii_header_get (buffer, node_name, "%s", host_name) < 0)
      fprintf (stderr, "nexus_parse: WARNING no host name for %s\n", 
	       node_name);
    else if (nexus_add (n, inode, host_name) < 0) 
      fprintf (stderr, "nexus_parse: Error adding %s %s\n", 
	       node_name, host_name);
    
  }

  return 0;
}


int nexus_configure (nexus_t* nexus, const char* filename)
{
  int error = 0;
  char* buffer = 0;
  long fsize = filesize (filename);

  buffer = (char *) malloc (fsize + 1);
  assert (buffer != 0);

#ifdef _DEBUG
  fprintf (stderr, "nexus_configure filename='%s'\n", filename);
#endif

  if (fileread (filename, buffer, fsize+1) < 0)
    return -1;

#ifdef _DEBUG
  fprintf (stderr, "nexus_configure call nexus_parse\n");
#endif

  error = nexus->nexus_parse (nexus, buffer);
  free (buffer);

  return error;
}

typedef struct {
    nexus_t* nexus;
    int id;
} node_open_t;


/*
 * Opens a connection to the specified node in a background thread
 */
void* node_open_thread (void* context)
{
  node_open_t* request = (node_open_t*) context;
  nexus_t* nexus = request->nexus;
  node_t* node = 0;
  int id = request->id;

  /* Name of the host on which the node is running */
  char* host_name = 0;

  /* Port on which the NODE is listening */
  int port = nexus->node_port;

  int fd = -1;
  FILE* to = 0;
  FILE* from = 0;
  FILE* log = 0;

  unsigned inode = 0;

  free (context);

  pthread_mutex_lock (&(nexus->mutex));
  for (inode = 0; inode < nexus->nnode; inode++) {
    node = (node_t*) nexus->nodes[inode];
    if (id == node->id) {
      host_name = strdup (node->host);
      assert (host_name != 0);
    }
  }
  pthread_mutex_unlock (&(nexus->mutex));

  if (!host_name) {
    fprintf (stderr, "node_open_thread: no NODE with id=%d\n", id);
    return 0;
  }

  while (fd < 0) {

#ifdef _DEBUG
  fprintf (stderr, "nexus_open_thread: call sock_open (%s,%d)\n",
	   host_name, port);
#endif

    fd = sock_open (host_name, port);

    if (fd < 0)  {
      fprintf (stderr, "open_thread: Error connecting with %s on %d\n"
	       "\tsleeping %u seconds before retrying\n",
	       host_name, port, nexus->polling_interval);
      sleep (nexus->polling_interval);
    }

  }

#ifdef _DEBUG
  fprintf (stderr, "nexus_open_thread: connected with %s\n", host_name);
#endif

  from = fdopen (fd, "r");
  if (!from)  {
    perror ("node_open_thread: Error creating input stream");
    return 0;
  }

  to = fdopen (fd, "w");
  if (!to)  {
    perror ("node_open_thread: Error creating output stream");
    return 0;
  }


  /* If we are the mirror i.e. have a logfile_dir specified */
  if ((nexus->node_port != 0) && (nexus->logfile_dir)) {

    char *buffer = 0;
    int buffer_size = strlen(nexus->logfile_dir) + 1 + strlen(host_name) + 5;
    buffer = malloc(buffer_size * sizeof(char));
    sprintf(buffer,"%s/%s.pwc.log",nexus->logfile_dir,host_name);

#ifdef _DEBUG
    fprintf(stderr,"node->log_file = %s\n",buffer);
#endif

    log = fopen(buffer,"a");
    if (!log) {
      fprintf (stderr, "node_open_thread: Error creating logfile stream: %s", buffer);
      return 0;
    }
  }

  /* If we are the base nexus, connect the log file. This will wait for the
   * log file of the mirror node to be connected first */
  if (nexus->mirror && nexus->mirror->logfile_dir) {
    assert(log == 0);
    sleep(1);
    while (log == 0) {
      /* Set our log FILE* to the mirrors matching one */
      pthread_mutex_lock (&(nexus->mirror->mutex));
      node = (node_t*) nexus->mirror->nodes[id];
      log = node->log;
      pthread_mutex_unlock (&(nexus->mirror->mutex));
      if (!(log)) {
        fprintf (stderr, "open_thread: Waiting for FILE * from mirror nexus "
                         "for node %d, sleeping 1 seconds before retrying\n",
                         id);
        sleep (1);
      }
    }
  }
 
  free (host_name);

  /* do not buffer the I/O */
  setbuf (to, 0);
  setbuf (from, 0);

  pthread_mutex_lock (&(nexus->mutex));
  for (inode = 0; inode < nexus->nnode; inode++) {
          
    node = (node_t*) nexus->nodes[inode];
    if (id == node->id) {
      node->to = to;
      node->from = from;
      node->log = log;
      to = from = log = 0;
      if (nexus->node_init)
        nexus->node_init (nexus, node);
      break;
    }
  }
  pthread_mutex_unlock (&(nexus->mutex));

  if (to || from)
    fprintf (stderr, "node_open_thread: no NODE with id=%d\n", id);

#ifdef _DEBUG
  fprintf (stderr, "nexus_open_thread: return\n");
#endif

  return 0;
}

int nexus_connect (nexus_t* nexus, unsigned inode)
{
  node_open_t* request = 0;
  pthread_t tmp_thread;
  node_t* node = 0;

  if (!nexus)
    return -1;

  if (inode >= nexus->nnode) {
    fprintf (stderr, "nexus_connect: inode=%d >= nnode=%d",
	     inode, nexus->nnode);
    return -1;
  }
  
  node = (node_t*) nexus->nodes[inode];

  /* start a new thread to open a socket connection with the host */
  request = (node_open_t*) malloc (sizeof(node_open_t));
  assert (request != 0);

  request->nexus = nexus;
  request->id = node->id;

#ifdef _DEBUG
  fprintf (stderr, "nexus_connect: pthread_create node_open_thread\n");
#endif

  if (pthread_create (&tmp_thread, 0, node_open_thread, request) < 0) {
    perror ("nexus_connect: Error creating new thread");
    return -1;
  }

  /* thread cannot be joined; resources will be destroyed on exit */
  pthread_detach (tmp_thread);

  return 0;
}

/*! Add a node to the nexus */
int nexus_add (nexus_t* nexus, int id, char* host_name)
{
  node_t* node = 0;
  unsigned inode = 0;

  /* ensure that each node in nexus has a unique id */
  for (inode = 0; inode < nexus->nnode; inode++) {
    node = (node_t*) nexus->nodes[inode];
    if (id == node->id) {
      fprintf (stderr, "nexus_add: id=%d equals that of %s\n",
	       id, node->host);
      return -1;
    }
  }

  pthread_mutex_lock (&(nexus->mutex));

  nexus->nodes = realloc (nexus->nodes, (nexus->nnode+1)*sizeof(void*));
  assert (nexus->nodes != 0);

  node = nexus->node_create();
  assert (node != 0);
  
  node->host = strdup (host_name);
  assert (node->host != 0);

  node->id = id;
  node->to = 0;
  node->from = 0;
  node->log = 0;

  nexus->nodes[nexus->nnode] = node;
  nexus->nnode ++;

  pthread_mutex_unlock (&(nexus->mutex));

  if (nexus_connect (nexus, nexus->nnode-1) < 0)
    return -1;

  if (nexus->mirror)
    return nexus_add (nexus->mirror, id, host_name);

  return 0;
}



int nexus_restart (nexus_t* nexus, unsigned inode)
{
  node_t* node = (node_t*) nexus->nodes[inode];

  if (node->to)
    fclose (node->to);
  node->to = 0;

  if (node->from)
    fclose (node->from);
  node->from = 0;

  if (node->log)
    fclose (node->log);
  node->log = 0;

  return nexus_connect (nexus, inode);
}

/*! Send a command to the specified node */
int nexus_send_node (nexus_t* nexus, unsigned inode, char* command)
{
  int status = 0;

  if (!nexus)
    return -1;

  if (inode >= nexus->nnode) {
    fprintf (stderr, "nexus_send_node: node %d >= nnode=%d",
	     inode, nexus->nnode);
    return -1;
  }

  status = node_send (nexus->nodes[inode], command);

  if (status == NEXUS_NODE_IO_ERROR) {
    if (nexus_restart (nexus, inode) < 0)
      fprintf (stderr, "nexus_send_node: error restart node %d\n", inode);
    return -1;
  }

  return status;
}

/*! Send a command to the node */
int node_send (node_t* node, char* command)
{
  FILE* to = node->to;

  if (!to) {
#ifdef _DEBUG
    fprintf (stderr, "node_send: node not online\n");
#endif
    return -1;
  }

  if (ferror (to)) {
#ifdef _DEBUG
    fprintf (stderr, "node_send: error on node");
#endif
    return NEXUS_NODE_IO_ERROR;
  }

#ifdef _DEBUG
  fprintf (stderr, "node_send: sending to node\n");
#endif

  if (fprintf (node->to, "%s\n", command) < 0) {
    fprintf (stderr, "node_send: '%s'\n\t%s",
	     command, strerror(errno));
    return -1;
  }

  return 0;
}

/*! Receive a reply from the specified node */
int nexus_recv_node (nexus_t* nexus, unsigned inode)
{
  int status = 0;

  if (!nexus)
    return -1;

  if (inode >= nexus->nnode) {
    fprintf (stderr, "nexus_recv_node: node %d >= nnode=%d",
	     inode, nexus->nnode);
    return -1;
  }
    
  status = node_recv (nexus->nodes[inode],
		      nexus->recv_buffer,
		      nexus->recv_bufsz);

  if (status == NEXUS_NODE_IO_ERROR) {
    if (nexus_restart (nexus, inode) < 0)
      fprintf (stderr, "nexus_send_node: error restart node %d\n", inode);
    return -1;
  }

  return status;
}

/*! Receive a reply from the node */
int node_recv (node_t* node, char* buffer, unsigned size)
{
  char* buf = buffer;
  FILE* from = node->from;

  char* found = 0;
  char c = 0;

  if (!from) {
#ifdef _DEBUG
    fprintf (stderr, "node_recv: node not online\n");
#endif
    return -1;
  }

  if (ferror (from)) {
#ifdef _DEBUG
    fprintf (stderr, "node_recv: error on node");
#endif
    return NEXUS_NODE_IO_ERROR;
  }

  while ( (buf=fgets (buf, size, from)) ) {

#ifdef _DEBUG
    fprintf (stderr, "node_recv: '%s'\n", buf);
#endif

    if ( (c=fgetc (from))=='>' )
      break;
    else 
      ungetc (c, from);

    buf += strlen(buf);

  }

  /* remove the space following the prompt */
  fgetc (from);

  if ( (found=strchr (buf, '\r')) )
    *found = '\n';

  if (strstr (buf, "fail\n"))
    return -1;

  /* null-terminate before the ok message */
  found = strstr (buf, "ok\n");
  if (found)
    *found = '\0';

  /* remove the final newline */
  found = strrchr (buffer, '\n');
  if (found)
    *found = '\0';

  return 0;
}

/*! Send a command to all selected nodes */
int nexus_send (nexus_t* nexus, char* command)
{
  unsigned inode = 0;
  int status = 0;

#ifdef _DEBUG
    fprintf (stderr, "nexus_send command '%s'\n", command);
#endif

  pthread_mutex_lock (&(nexus->mutex));

  for (inode = 0; inode < nexus->nnode; inode++) {
    if (nexus_send_node (nexus, inode, command) < 0) {
      fprintf (stderr, "nexus_send error");
    }
  }

  for (inode = 0; inode < nexus->nnode; inode++) {
    if (nexus_recv_node (nexus, inode) < 0) {
      node_t * tmpPtr = (node_t *) nexus->nodes[inode];
      fprintf (stderr, "%s returned 'fail' for command %s\n",
                       tmpPtr->host, command);
      status = -1;
    }
  }

  pthread_mutex_unlock (&(nexus->mutex));

  return status;
}

#include "dada_client.h"
#include "dada_hdu.h"
#include "dada_def.h"
#include "dada_msg.h"

#include "node_array.h"
#include "string_array.h"
#include "ascii_header.h"
#include "daemon.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

void usage()
{
  fprintf (stdout,
	   "dada_dbnic [options]\n"
	   " -N <name>[:port]  add a node to which data will be written [default port: %d]\n"
     " -k <key>          hexadecimal shared memory key  [default: %x]\n"
     " -s                single transfer only\n"
	   " -d                run as daemon\n" 
     " -h                print help text\n", DADA_DEFAULT_NICDB_PORT, 
     DADA_DEFAULT_BLOCK_KEY);
}

typedef struct {

  /* the set of nodes to which data will be written */
  node_array_t* array;

  /* the names of nodes requested on the command line */
  string_array_t* usr_node_names;

  /* the names of nodes requested in the header */
  string_array_t* hdr_node_names;

  /* current observation id, as defined by OBS_ID attribute */
  char obs_id [DADA_OBS_ID_MAXLEN];

} dada_dbnic_t;

#define DADA_DBNIC_INIT { 0, 0, 0, "" }

int setup_nodes (dada_client_t* client)
{
  /* the dada_dbnic specific data */
  dada_dbnic_t* dbnic = 0;
  
  /* the currently open connections */
  node_array_t* array = 0;

  /* the list of hosts with which a data connection should be established */
  string_array_t* hosts = 0;

  /* status and error logging facility */
  multilog_t* log;

  /* the header */
  char* header = 0;

  /* the destination, host[:port] */
  char* dest = 0;

  /* the host name */
  char* name = 0;

  int port = DADA_DEFAULT_NICDB_PORT;

  /* target nodes, as defined by TARGET_NODES attribute */
  char target_nodes [256];

  /* loop counters */
  unsigned inode=0;

  assert (client != 0);

  dbnic = (dada_dbnic_t*) client->context;
  assert (dbnic != 0);

  array = dbnic->array;
  assert (array != 0);

  log = client->log;
  assert (log != 0);

  header = client->header;
  assert (header != 0);

  /* Get the target nodes */
  if (ascii_header_get (header, "TARGET_NODES", "%s", target_nodes) == 1) {

    multilog (log, LOG_INFO, "TARGET_NODES=%s\n", target_nodes);

    if (!dbnic->hdr_node_names)
      dbnic->hdr_node_names = string_array_create ();

    /* remove entries not in target_nodes string */
    string_array_filter (dbnic->hdr_node_names, target_nodes);

    /* add nodes from target_nodes string */
    string_array_tok (dbnic->hdr_node_names, target_nodes, " ,\t\n");

    hosts = dbnic->hdr_node_names;

  }

  else if (dbnic->usr_node_names) {

    multilog (log, LOG_INFO, "Using specified nodes\n");
    hosts = dbnic->usr_node_names;

  }

  else {

    multilog (log, LOG_WARNING, "Header with no TARGET_NODES"
	      " and no specified nodes\n");

    return -1;

  }

  /* close the connections that are no longer required */
  while (inode < node_array_size (array)) {
    name = node_array_get (array, inode)->name;
    if (!string_array_search (hosts, name)) {
      multilog (log, LOG_INFO, "Closing %s\n", name);
      node_array_remove (array, inode);
    }
    else
      inode ++;
  }

  /* open the connections that are not already open */
  for (inode=0; inode < string_array_size (hosts); inode++) {
    dest = string_array_get (hosts, inode);

    /* if a port was optionally specified, extract it */

    char * pch = strtok(dest, ":");

    if (pch != NULL) {
    
      name = (char *) malloc((strlen(pch)+1) * sizeof(char));
      strcpy(name, pch);

      pch = strtok(NULL, ":");
      if (pch != NULL) {
        port = atoi(pch);
      } else {
        multilog (log, LOG_ERR, "Could not parse port correctly\n");
        port = DADA_DEFAULT_NICDB_PORT;
      }

    } else {
      multilog (log, LOG_ERR, "Could not parse name/port correctly\n");
      name = dest;
      port = DADA_DEFAULT_NICDB_PORT;
    }

    if (!node_array_search (array, name)) {
      if (node_array_add (array, name, port) < 0) {
        multilog (log, LOG_ERR, "Could not add (%s,%d) to node array\n",
		              name, DADA_DEFAULT_NICDB_PORT);
	      return -1;
      }
    }
  }

  return 0;
}


/*! Function that opens the data transfer target */
int sock_open_function (dada_client_t* client)
{
  /* the dada_dbnic specific data */
  dada_dbnic_t* dbnic = 0;
  
  /* status and error logging facility */
  multilog_t* log;

  /* the header */
  char* header = 0;

  /* observation id, as defined by OBS_ID attribute */
  char obs_id [DADA_OBS_ID_MAXLEN] = "";

  /* size of each transfer in bytes, as defined by TRANSFER_SIZE attribute */
  uint64_t xfer_size = 0;

  /* the optimal buffer size for writing to file */
  uint64_t optimal_bytes = 0;

  /* the open file descriptor */
  int fd = -1;


  assert (client != 0);

  dbnic = (dada_dbnic_t*) client->context;
  assert (dbnic != 0);
  assert (dbnic->array != 0);

  log = client->log;
  assert (log != 0);

  header = client->header;
  assert (header != 0);

  /* Get the observation ID */
  if (ascii_header_get (header, "OBS_ID", "%s", obs_id) != 1) {
    multilog (log, LOG_WARNING, "Header with no OBS_ID\n");
    strcpy (obs_id, "UNKNOWN");
  }

  /* check to see if we are still working with the same observation */
  if (strcmp (obs_id, dbnic->obs_id) == 0)
    multilog (log, LOG_INFO, "Continue OBS_ID=%s\n", obs_id);

  else {

    multilog (log, LOG_INFO, "New OBS_ID=%s\n", obs_id);
    if (setup_nodes (client) < 0)
      return -1;

  }

  /* Here maybe we should change the obs id or in some way ensure that
     files produced on nodes have unique names (otherwise, they all count
     from zero. */

#ifdef _DEBUG
  fprintf (stderr, "dbnic: copy the obs id\n");
#endif

  /* set the current observation id */
  strcpy (dbnic->obs_id, obs_id);

#ifdef _DEBUG
  fprintf (stderr, "dbnic: create the file name\n");
#endif

  /* Get the file size */
  if (ascii_header_get (header, "TRANSFER_SIZE", "%"PRIu64, &xfer_size) != 1)
  {
    xfer_size = DADA_DEFAULT_XFERSIZE;

    multilog (log, LOG_WARNING, "Header with no TRANSFER_SIZE. Setting to %"
              PRIu64"\n", xfer_size);

    if (ascii_header_set (header, "TRANSFER_SIZE", "%"PRIu64, xfer_size) < 0)
    {
      multilog (log, LOG_ERR, "Could not set TRANSFER_SIZE in Header\n");
      return -1;
    }
  }

#ifdef _DEBUG
  fprintf (stderr, "dbnic: open the socket\n");
#endif

  /* Get the socket connection */
  fd = node_array_open (dbnic->array, xfer_size, &optimal_bytes);

  if (fd < 0) {
    multilog (log, LOG_ERR, "Error node_array_open: %s\n", strerror (errno));
    return -1;
  }

  multilog (log, LOG_INFO, "Ready for writing %"PRIu64" bytes\n", xfer_size);

  client->fd = fd;
  client->transfer_bytes = xfer_size;
  client->optimal_bytes = 512 * optimal_bytes;

  return 0;
}

/*! Function that closes the data file */
int sock_close_function (dada_client_t* client, uint64_t bytes_written)
{
  /* the dada_dbnic specific data */
  dada_dbnic_t* dbnic = 0;

  /* status and error logging facility */
  multilog_t* log;

  /* received header */
  static char* header = 0;

  /* received header size */
  static uint64_t header_size = 0;

  assert (client != 0);

  dbnic = (dada_dbnic_t*) client->context;
  assert (dbnic != 0);

  log = client->log;
  assert (log != 0);

  if (bytes_written < client->transfer_bytes) {
    multilog (log, LOG_INFO, "Transfer stopped early at %"PRIu64" bytes\n",
	      bytes_written);

    if (ascii_header_set (client->header, "TRANSFER_SIZE", "%"PRIu64,
			  bytes_written) < 0)  {
      multilog (client->log, LOG_ERR, "Could not set TRANSFER_SIZE\n");
      return -1;
    }
  }

  if (shutdown (client->fd, SHUT_WR) < 0) {
    multilog (client->log, LOG_ERR, "Could not set shutdown write socket\n");
    return -1;
  }

  if (header_size < client->header_size) {
    header = realloc (header, client->header_size);
    assert (header != 0);
    header_size = client->header_size;
  }

  /* receive the acknowledgement header from the target node */
  if (recv (client->fd, header, client->header_size, 
	    DADA_MSG_FLAGS) < client->header_size)
    {
      multilog (log, LOG_ERR, "Could not recv acknowledgement Header: %s\n",
		strerror(errno));
      return -1;
    }

  if (close (client->fd) < 0)  {
    multilog (log, LOG_ERR, "Could not close socket\n");
    /* return -1; */
  }

  client->fd = -1;

  if (strcmp (client->header, header) != 0) {
    multilog (log, LOG_ERR, "Invalid acknowledgement Header:\n");
    multilog (log, LOG_ERR, "START HEADER\n%s\nEND HEADER\n", header);
    multilog (log, LOG_ERR, "Should be:\n");
    multilog (log, LOG_ERR, "START HEADER\n%s\nEND HEADER\n", client->header);
    return 0;
  }

  return 0;
}

/*! Pointer to the function that transfers data to/from the target */
int64_t sock_send_function (dada_client_t* client, 
			    void* data, uint64_t data_size)
{
#ifdef _DEBUG
  fprintf (stderr, "sock_send_function %p %"PRIu64"\n", data, data_size);
#endif
  return send (client->fd, data, data_size, 0);
}


int main (int argc, char **argv)
{
  /* DADA Data Block to Node configuration */
  dada_dbnic_t dbnic = DADA_DBNIC_INIT;

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

  /* Destination port */
  int port = DADA_DEFAULT_NICDB_PORT;

  /* hexadecimal shared memory key */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;

  int arg = 0;

  dbnic.array = node_array_create ();

  while ((arg=getopt(argc,argv,"dN:k:svh")) != -1)
    switch (arg) {
      
    case 'd':
      daemon=1;
      break;
      
    case 'N':
      if (!dbnic.usr_node_names)
	      dbnic.usr_node_names = string_array_create ();
      string_array_append (dbnic.usr_node_names, optarg);
      break;

    case 'p':
      port = atoi (optarg);
      break;

    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr,"dada_dbnic: could not parse key from %s\n",optarg);
        return -1;
      }
      break;

    case 's':
      quit = 1;
      break;

      
    case 'v':
      verbose=1;
      break;

    case 'h':
      usage ();
      return 0;
      
    default:
      usage ();
      return 0;
      
    }

  log = multilog_open ("dada_dbnic", daemon);

  if (daemon) {
    be_a_daemon ();
    multilog_serve (log, DADA_DEFAULT_DBNIC_LOG);
  }
  else
    multilog_add (log, stderr);

  hdu = dada_hdu_create (log);

  dada_hdu_set_key(hdu, dada_key);

  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_lock_read (hdu) < 0)
    return EXIT_FAILURE;

  client = dada_client_create ();

  client->log = log;

  client->data_block = hdu->data_block;
  client->header_block = hdu->header_block;

  client->open_function  = sock_open_function;
  client->io_function    = sock_send_function;
  client->close_function = sock_close_function;
  client->direction      = dada_client_reader;

  client->context = &dbnic;

  while (!client->quit)
  {
    if (dada_client_read (client) < 0)
      multilog (log, LOG_ERR, "Error during transfer\n");

    if (quit) 
      client->quit = 1;
  }

  if (dada_hdu_unlock_read (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

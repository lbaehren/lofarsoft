#include "dada_ni.h"
#include "sock.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*! Create a new DADA network interface */
dada_ni_t* dada_ni_create (int fd)
{
  dada_ni_t* net = malloc (sizeof(dada_ni_t));
  assert (net != 0);

  net->fd = fd;
  net->header = 0;
  net->header_size = 0;
  net->total_data = 0;
  net->total_transfered = 0;
  net->state = 0;

  return net;
}

/*! Destroy a DADA network interface */
void dada_ni_destroy (dada_ni_t* net)
{
  if (!net)
    return;

  if (net->fd > 0)
    sock_close (net->fd);

  free (net);
}

/*! Prepare for sending data over the DADA network interface */
int dada_ni_open_send (dada_ni_t* net, uint64_t total_data)
{
  assert (net != 0);
  if (!net->header || !net->header_size) {
    fprintf (stderr, "dada_ni_open_send: no header!\n");
    return -1;
  }

  /* here we write the total_data to the header, send it, and get reply */

  net->state = 1;
  net->total_data = total_data;
  net->total_transfered = 0;

  return 0;
}

int dada_ni_check_xfer (dada_ni_t* net, int s, uint64_t size, const char* dir)
{
  if (net->state != s) {
    fprintf (stderr, "dada_ni_%s: not %ser!\n", dir, dir);
    return -1;
  }

  if (net->total_transfered + size > net->total_data) {
    fprintf (stderr, "dada_ni_%s: transfered=%"PRIu64
	     " + buffer=%"PRIu64" > total=%"PRIu64"\n", dir,
	     net->total_transfered, size, net->total_data);
    return -1;
  }

  return 0;
}

/*! Send data over the DADA network interface */
int dada_ni_send (dada_ni_t* net, void* data, uint64_t data_size)
{
  assert (net != 0);
  if (dada_ni_check_xfer (net, 1, data_size, "send") < 0)
    return -1;


  return 0;
}

/*! Prepare for receiving data over the DADA network interface */
int dada_ni_open_recv (dada_ni_t* net)
{
  uint64_t total_data = 0;

  assert (net != 0);
  if (!net->header || !net->header_size) {
    fprintf (stderr, "dada_ni_open_send: no header!\n");
    return -1;
  }

  /* here we receive the header, parse the total_data and send reply */

  net->state = -1;
  net->total_data = total_data;
  net->total_transfered = 0;

  return 0;
}

/*! Receive data over the DADA network interface */
int dada_ni_recv (dada_ni_t* net, void* data, uint64_t data_size)
{
  assert (net != 0);
  if (dada_ni_check_xfer (net, -1, data_size, "recv") < 0)
    return -1;


  return 0;
}



#include "node_array.h"
#include "sock.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

// #define _DEBUG 1

/*! Create a new node array */
node_array_t* node_array_create ()
{
  node_array_t* array = (node_array_t*) malloc (sizeof(node_array_t));
  assert (array != 0);

  array -> nodes = 0;
  array -> nnode = 0;
  array -> cnode = 0;

  pthread_mutex_init(&(array->mutex), NULL);

  return array;
}

/*! Destroy a node array */
int node_array_destroy (node_array_t* array)
{
  if (!array)
    return -1;

  if (array->nodes)
    free (array->nodes);

  pthread_mutex_destroy (&(array->mutex));

  return 0;
}

/*! Return the requested node */
node_t* node_array_get (node_array_t* array, unsigned pos)
{
  if (pos < array->nnode)
    return &(array->nodes[pos]);
  return 0;
}

/*! Return the nnode of the array */
unsigned node_array_size (node_array_t* array)
{
  return array->nnode;
}

/*! Add a node to the array */
int node_array_add (node_array_t* array, const char* name, int port)
{
  /* pointer to new node_t structure in array */
  node_t* new_node = 0;

  /* array counter */
  unsigned inode;

  pthread_mutex_lock (&(array->mutex));

  /* ensure that each node in array is a unique device */
  for (inode = 0; inode < array->nnode; inode++) {
    if (!strcmp (name, array->nodes[inode].name)) {
      fprintf (stderr, "node_array_add: %s is already in array\n", name);
      pthread_mutex_unlock (&(array->mutex));
      return -1;
    }
  }

  array->nodes = realloc (array->nodes, (array->nnode+1)*sizeof(node_t));
  assert (array->nodes != 0);

  new_node = array->nodes + array->nnode;
  array->nnode ++;

  new_node->space = 0;
  new_node->port = port;
  new_node->name = strdup (name);

  assert (new_node->name != 0);

  pthread_mutex_unlock (&(array->mutex));

  return 0;
}

/*! Remove the specified node from the array */
int node_array_remove (node_array_t* array, unsigned inode)
{
  /* array counter */
  unsigned jnode = 0;

  /* the new file descriptor for the open socket */
  int status = 0;

  assert (array != 0);
  assert (inode < array->nnode);

  pthread_mutex_lock (&(array->mutex));

  free (array->nodes[inode].name);
  array->nnode --;

  /* ensure that each node in array is a unique device */
  for (jnode = inode; jnode < array->nnode; jnode++)
    array->nodes[jnode] = array->nodes[jnode+1];

  array->nodes = realloc (array->nodes, (array->nnode)*sizeof(node_t));
  assert (array->nodes != 0);

  pthread_mutex_unlock (&(array->mutex));

  return status;
}

/*! Get the available amount of node space */
uint64_t node_array_get_available (node_array_t* array)
{
  uint64_t available_space = 0;
  unsigned inode;

  pthread_mutex_lock (&(array->mutex));

  for (inode = 0; inode < array->nnode; inode++)
    available_space += array->nodes[inode].space;

  pthread_mutex_unlock (&(array->mutex));

  return available_space;
}

/*! Open a file on the node array, return the open file descriptor */
int node_array_open (node_array_t* array, uint64_t filesize,
		     uint64_t* optimal_buffer_size)
{
  int fd = -1;
  node_t* node = 0;

  pthread_mutex_lock (&(array->mutex));
  array->cnode %= array->nnode;

  node = array->nodes + array->cnode;

  /* the new file descriptor for the open socket */
  fd = sock_open (node->name, node->port);
  if (fd < 0)
    fprintf (stderr, "node_array_open: error sock_open(%s,%d) %s \n",
             node->name, node->port, strerror(errno));
  else
    array->cnode ++;
  pthread_mutex_unlock (&(array->mutex));

  *optimal_buffer_size = 4096;
  return fd;
}

node_t* node_array_search (node_array_t* array, const char* match)
{
  unsigned i;
  for (i=0; i < array->nnode; i++)
    if (strcmp(array->nodes[i].name, match) == 0)
      return &(array->nodes[i]);
  return 0;
}

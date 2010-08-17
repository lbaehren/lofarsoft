#ifndef __DADA_NODE_ARRAY_H
#define __DADA_NODE_ARRAY_H

/* ************************************************************************

   ************************************************************************ */

#include <pthread.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {

    char*    name;     /* hostname of node */
    int      port;     /* port number on hostname */
    uint64_t space;    /* number of bytes space left on node */

  } node_t;

  typedef struct {
    
    node_t*  nodes;    /* nodes to which data will be written */
    unsigned nnode;    /* number of nodes */
    unsigned cnode;    /* the current node */

    /* for multi-threaded use of the dbnode struct */
    pthread_mutex_t mutex;
    
  } node_array_t;

  /*! Create a new node array */
  node_array_t* node_array_create ();

  /*! Destroy a node array */
  int node_array_destroy (node_array_t*);

  /*! Return the specified node */
  node_t* node_array_get (node_array_t*, unsigned inode);

  /*! Return the size of the array */
  unsigned node_array_size (node_array_t*);

  /*! Add a node to the array */
  int node_array_add (node_array_t*, const char* name, int port);

  /*! Remove the specified node from the array */
  int node_array_remove (node_array_t*, unsigned inode);

  /*! Search for node with given host name */
  node_t* node_array_search (node_array_t*, const char* host_name);

  /*! Return the open file descriptor of a node in the array */
  int node_array_open (node_array_t*, uint64_t size, uint64_t* bs);

  /*! Get the total amount of node space */
  uint64_t node_array_get_total (node_array_t*);

  /*! Get the available amount of node space */
  uint64_t node_array_get_available (node_array_t*);

#ifdef __cplusplus
}
#endif

#endif

#ifndef __NEXUS_H
#define __NEXUS_H

/* ************************************************************************

   nexus_t - a struct and associated routines for creation and
   management of a dada network

   ************************************************************************ */

#include <stdio.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {

    /*! The name of the host on which node is running */
    char* host;

    /*! The port on which node is listening */
    int port;

    /*! The node identifier */
    int id;

    /*! The I/O stream to the node */
    FILE* to;

    /*! The I/O stream from the node */
    FILE* from;

    /*! The log stream from the node */
    FILE* log;

  } node_t;

  /*! Create a new node */
  node_t* node_create ();

  /*! For use by derived classes during construction */
  void node_init (node_t* node);

  /*! Send a command to the node */
  int node_send (node_t* node, char* command);

  /*! Receive a reply from the node */
  int node_recv (node_t* node, char* buffer, unsigned size);

  typedef struct nexus_struct {

    /*! The nodes */
    void** nodes;

    /*! The number of nodes */
    unsigned nnode;

    /*! This prefix is used when parsing configuration from a text file */
    char* node_prefix;

    /*! The default port on which node is listening */
    int node_port;

    /*! The polling interval for connecting with nodes */
    unsigned polling_interval;

    /*! The buffer used for receiving messages from nodes */
    char* recv_buffer;

    /*! The size of the receive buffer */
    unsigned recv_bufsz;

    /*! Pointer to function that creates new nodes */
    node_t* (*node_create) ();

    /*! Pointer to function that parses configuration */
    int (*nexus_parse) (struct nexus_struct* n, const char* buffer);

    /*! Pointer to function that initializes a new connection with a node */
    int (*node_init) (struct nexus_struct*, node_t*);

    /*! If specified, make a mirror of this nexus */
    struct nexus_struct* mirror;

    /* for multi-threaded use of the nexus */
    pthread_mutex_t mutex;

    /* The base directory for multilog messages */
    char* logfile_dir;
                                                                                                                                                                              
    /* Port on which multilog messages are available (only for mirror) */
    int multilog_port;

  } nexus_t;

#define NEXUS_DEFAULT_RECV_BUFSZ 1024
#define NEXUS_NODE_IO_ERROR -2

  /*! Create a new nexus */
  nexus_t* nexus_create ();

  /*! Destroy a nexus */
  int nexus_destroy (nexus_t* nexus);

  /*! Read the nexus configuration from the specified filename */
  int nexus_configure (nexus_t* nexus, const char* filename);

  /*! Add a node to the nexus */
  int nexus_add (nexus_t* nexus, int id, char* host_name);

  /*! Send a command to all selected nodes */
  int nexus_send (nexus_t* nexus, char* command);

  /*! Send a command to the specified node */
  int nexus_send_node (nexus_t* nexus, unsigned inode, char* command);

  /*! Receive a reply from the specified node */
  int nexus_recv_node (nexus_t* nexus, unsigned inode);

  /*! Get the number of nodes in the nexus */
  unsigned nexus_get_nnode (nexus_t* nexus);

  /*! For use by derived classes during construction */
  void nexus_init (nexus_t* nexus);

  /*! For use by derived classes during configuration */
  int nexus_parse (nexus_t* n, const char* buffer);

#ifdef __cplusplus
	   }
#endif

#endif

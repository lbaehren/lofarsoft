#include "dada_pwc_nexus.h"
#include "futils.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* #define _DEBUG 1 */

/*! parse a configuration into unique headers for each primary write client */
int dada_pwc_nexus_header_parse (dada_pwc_nexus_t* n, FILE* fptr);

/*! defined in dada_pwc.c */
int dada_pwc_parse_bytes_per_second (dada_pwc_t* primary,
				     FILE* fptr, const char* header);

int dada_pwc_nexus_cmd_config (void* context, FILE* output, char* args)
{
  dada_pwc_nexus_t* nexus = (dada_pwc_nexus_t*) context;
  dada_pwc_node_t* node = 0;

  unsigned inode, nnode = nexus_get_nnode ((nexus_t*) nexus);

  char* filename = args;
  char* hdr = 0;
  FILE* fptr = 0;

  int error = 0;

  if (nexus->pwc->state != dada_pwc_idle && 
      nexus->pwc->state != dada_pwc_pending)
  {
    fprintf (output, "Cannot config when not IDLE or PENDING; state=%s\n",
             dada_pwc_state_to_string(nexus->pwc->state));
    return -1;
  }

  if (!args) {
    fprintf (output, "Please provide specification filename\n");
    return -1;
  }

  filename = strsep (&args, " \t\n\r");

  if (!filename || filename[0] == '\0') {
    fprintf (output, "Please specify config filename\n");
    return -1;
  } 

#ifdef _DEBUG
  fprintf (stderr, "dada_pwc_nexus_config fopen (%s, 'r')\n", filename);
#endif

  fptr = fopen (filename, "r");
  if (!fptr) {
    fprintf (output, "Cannot open '%s'\n", filename);
    return -1;
  }

  error = dada_pwc_nexus_header_parse (nexus, fptr);

  if (error)
    return error;

  for (inode=0; inode < nnode; inode++) {

    node = (dada_pwc_node_t*) nexus->nexus.nodes[inode];
    strcpy (nexus->pwc->header, "header ");
    strcat (nexus->pwc->header, node->header);

    hdr = nexus->pwc->header;
    /* replace new line with \\ */
    while ( (hdr = strchr(hdr, '\n')) != 0 )
      *hdr = '\\';

    if (nexus_send_node ((nexus_t*) nexus, inode, nexus->pwc->header) < 0)
      return -1;

    if (inode==0)
      dada_pwc_parse_bytes_per_second (nexus->pwc, output, nexus->pwc->header);

  }

  for (inode=0; inode < nnode; inode++) {
    if (nexus_recv_node ((nexus_t*) nexus, inode) < 0) {
      fprintf (stderr, "nexus_send error inode=%d\n", inode);
      error = -1;
    }
  }

  if (error == 0)
    nexus->pwc->state = dada_pwc_prepared;

  return error;
}


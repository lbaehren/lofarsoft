#include "dada_pwc_nexus.h"
#include "ascii_header.h"
#include "futils.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* #define _DEBUG 1 */

int dada_pwc_specify_header (char keep, const char* filter,
			     char* header, FILE* fptr)
{
  // each line read from fptr
  char file_line [256];

  // result of search for '#' in file_line
  char* comment = 0;

  // parameter name and value strings
  char param_name  [128] = "";
  char param_value [128] = "";

  // set true if the parameter name prefix matches the filter
  char match = 0;

  // length of filter
  unsigned filter_length;

  // string length of param_value
  int param_value_length;

  // parameter name copied from specification to header
  char* param = 0;

  if (!filter) {
    fprintf (stderr, "dada_pwc_specify_header no filter");
    return -1;
  }
  if (!header) {
    fprintf (stderr, "dada_pwc_specify_header no header");
    return -1;
  }
  if (!fptr) {
    fprintf (stderr, "dada_pwc_specify_header no specification");
    return -1;
  }

  rewind (fptr);

  filter_length = strlen(filter);

  while ( fgets( file_line, 256, fptr ) ) {

    comment = strchr( file_line, '#' );
    if (comment)
      *comment = '\0';

#ifdef _DEBUG
    fprintf (stderr, "line=%s\n", file_line);
#endif

    //if ( sscanf( file_line, "%s %s", param_name, param_value ) != 2 )
    if ( sscanf( file_line, "%s %128[^\n]", param_name, param_value ) != 2 )
      continue;

    // Remove trailing whitespace
    param_value_length = strlen(param_value) - 1;
    while ((param_value[param_value_length] == ' ') && (param_value_length >= 0)) {
      param_value[param_value_length] = '\0';
      param_value_length--;
    }
    
#ifdef _DEBUG
    fprintf (stderr, "param filter=%s name=%s value=%s\n", 
             filter, param_name, param_value);
#endif


    match = strncmp( param_name, filter, filter_length ) == 0;

#ifdef _DEBUG
    fprintf (stderr, "match=%d keep=%d\n", (int)match, (int)keep);
#endif

    // "NOT (keep XOR match)" == "(keep AND match) OR (NOT keep AND NOT match)"
    if ( ! (keep ^ match) ) {

      param = param_name;

      if (match)
        param += filter_length;

#ifdef _DEBUG
      fprintf (stderr, "%s: set name=%s value=%s\n", filter, param, param_value);
#endif

      if (ascii_header_set (header, param, "%s", param_value) < 0) {
	fprintf (stderr, "dada_pwc_specify_header ERROR setting %s=%s\n",
		 param, param_value);
	return -1;
      }

    }
      
  }

  return 0;
}

int dada_pwc_nexus_header_parse (dada_pwc_nexus_t* n, FILE* fptr)
{
  char node_name [16] = "Band";
  dada_pwc_node_t* node = 0;

  unsigned inode, nnode = nexus_get_nnode ((nexus_t*) n);

  if (n->header_template)
    /* Read the header template */
    fileread (n->header_template, n->pwc->header, n->pwc->header_size);
  else
    /* Start with empty header */
    n->pwc->header[0] = '\0';

  if (dada_pwc_specify_header (0, node_name, n->pwc->header, fptr) < 0)
    return -1;

  for (inode=0; inode < nnode; inode++) {

    node = (dada_pwc_node_t*) n->nexus.nodes[inode];
    if (node->header_size < n->pwc->header_size) {
      node->header = realloc (node->header, n->pwc->header_size);
      assert (node->header != 0);
      node->header_size = n->pwc->header_size;
    }

    strcpy (node->header, n->pwc->header);
    sprintf (node_name, "Band%d_", inode);
    if (dada_pwc_specify_header (1, node_name, node->header, fptr) < 0)
      return -1;
    
  }
    
  return 0;
}


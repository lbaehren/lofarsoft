
#include "dada_def.h"
#include "ascii_header.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/* #define _DEBUG 1 */

void usage()
{
  fprintf (stdout,
	   "dada_edit - query or change DADA header parameters in file[s]\n"
	   "\n"
	   "USAGE: dada_edit [options] filename[s]\n"
	   "\n"
	   "WHERE options are:\n"
	   " -c KEY=VAL     set KEY to VAL \n"
           " -s size        set HDR_SIZE size in old headers \n"
	   " -i header.txt  over-write header with text in specified file\n");
}


int main (int argc, char **argv)
{
  /* Flag set in verbose mode */
  char verbose = 0;

  /* command string to be parsed into key=val pair */
  char* key = 0;
  char* val = 0;

  /* name of file to be installed in header */
  char* install = 0;
  FILE* install_file = 0;

  /* descriptor of currently open file */
  FILE* current_file = 0;

  /* header read from current_file */
  char* current_header = 0;
  unsigned current_header_size = 0;

  /* header size read from current file */
  unsigned hdr_size = DADA_DEFAULT_HEADER_SIZE;
  unsigned set_hdr_size = 0;

  int arg = 0;

  while ((arg=getopt(argc,argv,"c:i:hs:v")) != -1)

    switch (arg)
    {      
    case 'c':
      key = strdup (optarg);
      val = strchr (key, '=');
      if (val)
      {
        /* terminate the key and advance to the value */
        *val = '\0';
        val ++;
      }

      break;

    case 'i':
      install = optarg;
      break;

    case 's':
      hdr_size = set_hdr_size = atoi (optarg);
      fprintf (stderr, "dada_edit: will set header size to %u\n", hdr_size);
      break;

    case 'v':
      verbose=1;
      break;
   
    case 'h':
    default:
      usage ();
      return 0;
      
    }

  char* mode = "r";
  if (val || install || set_hdr_size)
    mode = "r+";

  for (arg = optind; arg < argc; arg++)
  {
    current_file = fopen (argv[arg], mode);
    if (!current_file)
    {
      fprintf (stderr, "dada_edit: could not open '%s': %s\n",
	       argv[arg], strerror(errno));
      continue;
    }

    do
    {
      rewind (current_file);

      current_header = realloc (current_header, hdr_size);
      current_header_size = hdr_size;

      if (fread (current_header, 1, current_header_size, current_file)
	  != current_header_size)
      {
	fprintf (stderr, "dada_edit: could not read %u bytes: %s\n",
		 current_header_size, strerror(errno));
	return -1;
      }

      if (set_hdr_size)
      {
        /* Set the header size */
        if (ascii_header_set (current_header, "HDR_SIZE", "%u", hdr_size) != 0)
        {
          fprintf (stderr, "dada_edit: could not set HDR_SIZE\n");
          return -1;
        }
      }

      /* Get the header size */
      else if (ascii_header_get (current_header, "HDR_SIZE", "%u", &hdr_size) != 1)
      {
	fprintf (stderr, "dada_edit: could not parse HDR_SIZE\n");
	return -1;
      }

      /* Ensure that the incoming header fits in the client header buffer */
    }
    while (hdr_size > current_header_size);

    if (val && ascii_header_set (current_header, key, val) != 0)
    {
      fprintf (stderr, "dada_edit: could not set %s = %s\n", key, val);
      return -1;
    }

    if (key && !val)
    {
      char value[1024];
      if (ascii_header_get (current_header, key, "%s", value) == 1)
      {
        if (optind + 1 == argc)
          printf ("%s\n", value);
        else
          printf ("%s %s\n", argv[arg], value);
      }
    }

    if (install)
    {
      install_file = fopen (install, "r");
      if (!install_file)
      {
        fprintf (stderr, "dada_edit: could not open '%s': %s\n", install,
                 strerror(errno));
        return -1;
      }

      size_t r = fread (current_header, 1, current_header_size, install_file);
      if (r == 0)
      {
        fprintf (stderr, "dada_edit: could not read %u bytes: %s\n",
                 current_header_size, strerror(errno));
        return -1;
      }
      if (!feof(install_file))
      {
        fprintf (stderr, "dada_edit: %s contains more than %u bytes\n",
                 install, current_header_size);
        return -1;
      }
      if (r < current_header_size)
        memset (current_header + r, 0, current_header_size - r);
    }

    if (val || install || set_hdr_size)
    {
      rewind (current_file);

      if (fwrite (current_header, 1, current_header_size, current_file)
	  != current_header_size)
      {
	fprintf (stderr, "dada_edit: could not write %u bytes: %s\n",
		 current_header_size, strerror(errno));
	return -1;
      }
    }

    else if (!key)
    {
      fprintf (stdout, current_header);
    }

    fclose (current_file);
  }

  return EXIT_SUCCESS;
}


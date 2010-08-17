#include "bpsr_def.h"
#include "bpsr_udp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <math.h>

#include <cpgplot.h>

void get_scale (int from, int to, float* width, float * height);

void usage()
{
  fprintf (stdout,
     "bpsr_diskplot [options] [files]+ \n"
     " files       pol0 and pol1 files\n"
     " -D device   pgplot device name [default ?]\n"
     " -p          plain image only, no axes, labels etc\n"
     " -v          be verbose\n"
     " -g XXXxYYY  set plot resoltion in XXX and YYY pixels\n"
     " -d          run as daemon\n");
}


int main (int argc, char **argv)
{

  /* Flag set in verbose mode */
  char verbose = 0;

  /* PGPLOT device name */
  char * device = "?";

  /* file names of each data file */
  char ** fnames;

  /* float array for eahc data file */
  float ** data;

  /* file descriptors for each data file */
  int * fds;

  /* plot JUST the data */
  int plainplot = 0;

  /* unsigned int array for reading data files */
  unsigned int * input;

  /* unsigned int array for reading data files */
  float * input2;

  /* dimensions of plot */
  unsigned int width_pixels = 0;
  unsigned int height_pixels = 0;

  int arg = 0;

  while ((arg=getopt(argc,argv,"D:vg:p")) != -1)
    switch (arg) {
      
    case 'D':
      device = strdup(optarg);
      break;

    case 'v':
      verbose=1;
      break;

    case 'g':
      if (sscanf (optarg, "%ux%u", &width_pixels, &height_pixels) != 2) {
        fprintf(stderr, "bpsr_diskplot: could not parse dimensions from %s\n",optarg);
        return -1;
      }
      break;

    case 'p':
      plainplot = 1;
      break;

    default:
      usage ();
      return 0;
      
  }

  if ((argc - optind) < 1) {
    fprintf(stderr, "bpsr_diskplot: no data files specified\n");
    exit(EXIT_FAILURE);
  }

  int i=0;
  int j=0;
  int nfiles = (argc - optind);
  int flags = O_RDONLY;

  if (verbose)
    fprintf(stderr, "processing %d files\n", nfiles);

  fds    = (int *) malloc(sizeof(int) * nfiles);
  fnames = (char **) malloc(sizeof(char *) * nfiles);
  data   = (float **) malloc(sizeof(float *) * nfiles);
  //input  = (unsigned int *) malloc(sizeof(unsigned int *) * BPSR_IBOB_NCHANNELS);
  input2  = (float *) malloc(sizeof(float *) * BPSR_IBOB_NCHANNELS);

  if (verbose)
    fprintf(stderr, "mallocd data\n");

  for (i=0; i < nfiles; i++) {

    fnames[i] = strdup(argv[optind + i]);

    fds[i] = open(fnames[i], flags);
    if (fds[i] < 0) {
      fprintf(stderr, "Error opening %s: %s\n", fnames[i], strerror (errno));
      free(fds);
      free(data);
      return (EXIT_FAILURE);
    }

  }
  if (verbose)
    fprintf(stderr, "opened files\n");

  /* Read the data from each file into the array */
  for (i=0; i < nfiles; i++) {

    //read(fds[i], input, (size_t) (BPSR_IBOB_NCHANNELS*sizeof(unsigned int)));
    read(fds[i], input2, (size_t) (BPSR_IBOB_NCHANNELS*sizeof(float)));
    data[i] = malloc (sizeof(unsigned int ) * BPSR_IBOB_NCHANNELS);

    for (j=0; j<BPSR_IBOB_NCHANNELS; j++) {
      data[i][j] = input2[j];
    }

  }
  if (verbose)
    fprintf(stderr, "read data\n");

  /* Open pgplot device window */
  if (cpgopen(device) != 1) {
    fprintf(stderr, "bpsr_diskplot: error opening plot device\n");
    exit(EXIT_FAILURE);
  }

  /* Resolution */
  if (width_pixels && height_pixels) {

    float width_scale, height_scale;
                                                                                                 
    get_scale (3, 1, &width_scale, &height_scale);
                                                                                                 
    float width_inches = width_pixels * width_scale;
    float aspect_ratio = height_pixels * height_scale / width_inches;
                                                                                                 
    cpgpap( width_inches, aspect_ratio );
                                                                                                 
    float x1, x2, y1, y2;
    cpgqvsz (1, &x1, &x2, &y1, &y2);
                                                                                                 
    //if (rintf(x2) != width_pixels)
    //  fprintf(stderr, "set_dimensions width=%f != request=%d\n",x2,width_pixels);

    //if (rintf(y2) != height_pixels)
    //  fprintf(stderr, "set_dimensions height=%f != request=%d\n",y2,height_pixels);
  }



  float x_points[1024];
  float ymin = 0;
  float ymax = 1;

  /* Get the min/max  values for plotting*/
  for (i=0; i < BPSR_IBOB_NCHANNELS; i++) {

    x_points[i] = (float) i;

    for (j=0; j < nfiles; j++) {
      if (data[j][i] < ymin) ymin = data[j][i];
      if (data[j][i] > ymax) ymax = data[j][i];
    }
  }

  cpgask(0);

  for (i=0; i < nfiles/2; i++) {

    if (verbose)
      fprintf(stderr, "processing files %d & %d\n",(2*i), (2*i)+1);

    cpgbbuf();
    cpgeras();

    cpgsci(1);
    if (plainplot)
      cpgenv(0, 1024, 0, (1.1*ymax), 0, -2);
    else {
      cpgenv(0, 1024, 0, (1.1*ymax), 0, 0);
      cpglab("Frequency Channel", "Intensity", "Intensity vs Frequency Channel");
    }
                                                                                                         
    cpgsci(1);
                                                                                                         
    float x = 0;
    float y = 0;
    
    fprintf(stderr, "plotting %s & %s\n", fnames[(2*i)], fnames[(2*i)+1]);

    cpgsci(1);
    cpgmtxt("T", 0.5, 0.0, 0.0, fnames[(2*i)]);

    cpgsci(2);
    cpgline(1024, x_points, data[2*i]);

    cpgsci(1);
    cpgmtxt("T", 1.5, 0.0, 0.0, fnames[(2*i)+1]);
    cpgsci(3);

    cpgline(1024, x_points, data[(2*i)+1]);

    cpgebuf();

    ibob_pause(150);
  }

  cpgclos();

  return EXIT_SUCCESS;
}

void get_scale (int from, int to, float * width, float * height)
{

  float j = 0;
  float fx, fy;
  cpgqvsz (from, &j, &fx, &j, &fy);
                                                                                                 
  float tx, ty;
  cpgqvsz (to, &j, &tx, &j, &ty);
                                                                                                 
  *width = tx / fx;
  *height = ty / fy;
}


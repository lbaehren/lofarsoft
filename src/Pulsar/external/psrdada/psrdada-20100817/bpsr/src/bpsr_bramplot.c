#include "bpsr_def.h"
#include "bpsr_udp.h"
#include "ibob.h"

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
void set_dimensions (unsigned width_pixels, unsigned height_pixels);
void get_scale (int from, int to, float* width, float * height);
void createPlot (char* device, float * xvals, float * pol0, float * pol1,
                 unsigned width, unsigned height, int plainplot, 
                 unsigned bit_window);

extern float roundf(float);


void usage()
{
  fprintf (stdout,
     "bpsr_bramplot [options] file\n"
     "  file        bramdump file produced by the mulitbob_server\n"
     "  -D device   pgplot device name [default ?]\n"
     "  -p          plain image only, no axes, labels etc\n"
     "  -v          be verbose\n"
     "  -g XXXxYYY  only produce 1 plot in XXXxYYY dimensions\n"
     "  -d          run as daemon\n"
     "\n"
     "  default produces a 112x84, 400x300 and 1024x768 plot\n"
     "  -D, -p or -g will change this behaviour\n");
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

  /* pgplot float arrays */
  float * xval;
  float * pol1;
  float * pol2;

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

  if ((argc - optind) != 1) {
    fprintf(stderr, "bpsr_diskplot: one data file must be specified\n");
    usage();
    exit(EXIT_FAILURE);
  }

  int i=0;
  int j=0;
  int flags = O_RDONLY;

  int fd;
  char * fname = strdup(argv[optind]);

  fd = open(fname, flags);
  if (fd < 0) {
    fprintf(stderr, "Error opening %s: %s\n", fname, strerror (errno));
    free(fname);
    return (EXIT_FAILURE);
  }

  unsigned bit_window;
  xval = (float *) malloc (sizeof(float) * IBOB_BRAM_CHANNELS);
  pol1 = (float *) malloc (sizeof(float) * IBOB_BRAM_CHANNELS);
  pol2 = (float * )malloc (sizeof(float) * IBOB_BRAM_CHANNELS);

  read(fd, &bit_window, sizeof(unsigned));
  read(fd, pol1, (sizeof(float) * IBOB_BRAM_CHANNELS));
  read(fd, pol2, (sizeof(float) * IBOB_BRAM_CHANNELS));

  close(fd);

  /* extract data from input array */
  for (i=0; i<IBOB_BRAM_CHANNELS; i++) 
    xval[i] = (float) i*2;

  /* convert data to log base 2 */
  float logbase2 = logf(2.0);
  for (i=0; i<IBOB_BRAM_CHANNELS; i++)
  {
    if (pol1[i] > 0)
      pol1[i] = logf(pol1[i]) / logbase2;

    if (pol2[i] > 0)
      pol2[i] = logf(pol2[i]) / logbase2;
  }

  if ( (plainplot == 0) && (strcmp(device, "?") == 0) && 
       (width_pixels == 0)  && (height_pixels == 0) )

  {
    char png_file[128];
    char file_base[128];
    
    // strip the .bramdump from the end of the filename
    strncpy(file_base, fname, (strlen(fname)-9));
    file_base[(strlen(fname)-9)] = '\0';

    sprintf(png_file, "%s_112x84.png/png", file_base);
    createPlot(png_file, xval, pol1, pol2, 112, 84, 1, bit_window);

    sprintf(png_file, "%s_400x300.png/png", file_base);
    createPlot(png_file, xval, pol1, pol2, 400, 300, 1, bit_window);

    sprintf(png_file, "%s_1024x768.png/png", file_base);
    createPlot(png_file, xval, pol1, pol2, 1024, 768, 0, bit_window);
  }
  else 
  {
    createPlot(device, xval, pol1, pol2, width_pixels, height_pixels, 
               plainplot, bit_window);
  }

  free(xval);
  free(pol1);
  free(pol2);

  return EXIT_SUCCESS;
}


void createPlot (char* device, float * xvals, float * pol1, float * pol2, 
                 unsigned width_pixels, unsigned height_pixels, int plainplot,
                 unsigned bw)
{

  /* Open pgplot device window */
  if (cpgopen(device) != 1) {
    fprintf(stderr, "bpsr_diskplot: error opening plot device\n");
    exit(EXIT_FAILURE);
  }

  /* Resolution */
  if (width_pixels && height_pixels) 
    set_dimensions (width_pixels, height_pixels);

  float ymin = 0;
  float ymax = 32;

  /* Get the min/max values */
  unsigned i=0;
  float * bits_x[5];
  float * bits_y[5];

  for (i=0; i<5; i++) 
  {
    bits_x[i] = (float *) malloc(sizeof(float)*2);
    bits_y[i] = (float *) malloc(sizeof(float)*2);

    bits_x[i][0] = IBOB_BRAM_CHANNELS*2;
    bits_x[i][1] = 0;

    bits_y[i][0] = i*8;
    bits_y[i][1] = i*8;
  }


  cpgsci(1);

  if (plainplot) 
  {
    cpgsvp(0.0,1.0,0.0,1.0);
    cpgswin((IBOB_BRAM_CHANNELS*2), 0, 0, (1.1*ymax));
  }
  else 
  {
    cpgsvp(0.1,0.9,0.1,0.9);
    cpgswin((IBOB_BRAM_CHANNELS*2), 0, 0, (1.1*ymax));
    cpglab("IBoB Channel", "Activated Bits", "Pre Bit-selected Bandpass");
  }

  // grey out the background
  cpgsci(14);
  cpgsfs(1);
  cpgrect(1024, 0, 0, (1.1*ymax));

  // black in the active bit window
  cpgsci(0);
  cpgsfs(1);
  cpgrect(bits_x[bw][0], bits_x[bw+1][1], bits_y[bw][0], bits_y[bw+1][1]);

  cpgsci(1);
  if (!plainplot)
    cpgbox("BCNST", 0.0, 0.0, "BCNST", 0.0, 0.0);

  cpgsci(1);
  cpgline(2, bits_x[bw], bits_y[bw]);
  cpgline(2, bits_x[bw+1], bits_y[bw+1]);

  cpgsci(2);
  if (!plainplot) 
    cpgmtxt("T", 0.5, 0.0, 0.0, "Pol 1");
  cpgline(IBOB_BRAM_CHANNELS, xvals, pol1);

  cpgsci(3);
  if (!plainplot) 
    cpgmtxt("T", 1.5, 0.0, 0.0, "Pol 2");
  cpgline(IBOB_BRAM_CHANNELS, xvals, pol2);

  cpgclos();

}

void get_scale (int from, int to, float * width, float * height)
{

  float j = 0;
  float fx, fy;
  cpgqvsz (from, &j, &fx, &j, &fy);
                                                                                                 
  float tx, ty;
  cpgqvsz (to, &j, &tx, &j, &ty);

  /* you dont want to know why */
  fx *= 1.003;
  fy *= 1.003;
                                                                                                 
  *width = tx / fx;
  *height = ty / fy;
}


void set_dimensions (unsigned width_pixels, unsigned height_pixels)
{
  float width_scale, height_scale;
  const int Device = 0;
  const int Inches = 1;
  const int Millimetres = 2;
  const int Pixels = 3;
  const int World = 4;
  const int Viewport = 5;

  get_scale (Pixels, Inches, &width_scale, &height_scale);

  float width_inches = width_pixels * width_scale;
  float aspect_ratio = height_pixels * height_scale / width_inches;

  cpgpap( width_inches, aspect_ratio );

  float x1, x2, y1, y2;
  cpgqvsz (Pixels, &x1, &x2, &y1, &y2);

  if (roundf(x2) != width_pixels) {
    //fprintf (stderr,"set_dimensions: %f != request = %d\n",x2, width_pixels);
  }
}

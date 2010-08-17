/***************************************************************************/
/*                                                                         */
/* function get_commandline                                                */
/*                                                                         */
/* it reads the input values from the command line                         */
/*                                                                         */
/* Ver 2.0 RB 06 Oct 2008                                                  */
/* 		modified to take in options for resolution (pixel dimension)
		mode, plot bandpass on log, omit labels or box		   */
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"

void get_commandline(int argc, char *argv[], char *inpfile0, char *inpfile1, 
    char *inpdev, char *outputfile,
    float *zoomt_begin, float *zoomt_end,float *zoomf_begin, float *zoomf_end,
    int *dolog, int *dolabel,int *dobox, int *dommm, unsigned *width_pixels, unsigned *height_pixels)
{
  int i,j,nfiles;
  int device_selected=0; 
  char checkchar[2];
  
  // plot dimensions in pixels
  unsigned width = 0, height = 0;
  // plot zoomed interval
  float zbegin = 0.0;
  float zend = 1000000.0;
  // any character, e.g. 'x' in 640x480
  char c = 0, optarg[20];

  /* check number of command-line arguments and print help if requested */
  if (argc<2) {
    display_help(argv[0]);
    exit(-1);
  } else {
    j=2;
    while (j<=argc) {
      if (strings_compare(argv[j-1],"-h")) {
	/* print the help on line */
	display_help(argv[0]);
	exit(-1);
      }
    j++;
    }
  }

  /* work out how many files are on the command line */
  i=1;
  nfiles=0;
  for (i=1; i < argc; i++)
  {
    if (check_file_exists(argv[i]))
    {
      if (nfiles == 0) strcpy(inpfile0,argv[i]);
      if (nfiles == 1) strcpy(inpfile1,argv[i]);
      nfiles++;
    }
    else
      break;
  }

  if (!nfiles)
  {
    printf("An input file must be the FIRST argument in the command line!"); 
    printf(" Exiting...\n");
    exit(-1);
  }

  printf("\nData files: %s %s \n",inpfile0,inpfile1); 

  /* now parse any remaining command line parameters */
  if (argc>nfiles) {
    i=nfiles+1;
    while (i<argc) {
      if (strings_compare(argv[i],"-g")) {
        /* get graphic device for output */
	if (argc==i+1) {
	   printf("No argument specified for <device>. Exiting... \n");
	   exit(-1);
	}
        strcpy(inpdev,argv[++i]);
        strncpy(checkchar,inpdev,1);
	//if (!(strings_compare(checkchar[0],"/"))) {
	if (checkchar[0] != '/') {
    	 printf("Unknown argument %s given for <device>. Exiting...\n",inpdev);
         //strcpy(inpdev,"/png");
         exit(-1);
        }  
	device_selected=1;
      } else if (strings_compare(argv[i],"-G")) {
        strcpy(optarg,argv[++i]);
        if (sscanf(optarg, "%u%c%u", &width, &c, &height) != 3)
          {
            fprintf (stderr, "Could not parse dimensions from %s\n", optarg);
            exit(-1);
          } else {
            *width_pixels = width; 
            *height_pixels = height;
            fprintf (stderr, "Pixel dimensions: %d x %d \n", width, height);
          }
      } else if (strings_compare(argv[i],"-zt")) {
        strcpy(optarg,argv[++i]);
        if (sscanf(optarg, "%f%c%f", &zbegin, &c, &zend) != 3)
          {
            fprintf (stderr, "Could not parse zoomed time interval from %s\n", optarg);
            exit(-1);
          } else {
            *zoomt_begin = zbegin; 
            *zoomt_end = zend;
            fprintf (stderr, "Zoomed time interval: %f --> %f sec \n", *zoomt_begin, *zoomt_end);
            if (zbegin>=zend){
               fprintf(stderr, "Final time smaller than inital time! \n");
	       exit(-2);
	    }
          }
      } else if (strings_compare(argv[i],"-zf")) {
        strcpy(optarg,argv[++i]);
        if (sscanf(optarg, "%f%c%f", &zbegin, &c, &zend) != 3)
          {
            fprintf (stderr, "Could not parse zoomed freq interval from %s\n", optarg);
            exit(-1);
          } else {
            *zoomf_begin = zbegin; 
            *zoomf_end = zend;
            fprintf (stderr, "Zoomed freq interval: %f --> %f Hz \n", *zoomf_begin, *zoomf_end);
            if (zbegin>=zend){
               fprintf(stderr, "Upper freq smaller than lower freq! \n");
	       exit(-2);
	    }
          }
      } else if (strings_compare(argv[i],"-nolabel")) {
        *dolabel = 0;
        printf("Plot mode: thumbnail \n");
      } else if (strings_compare(argv[i],"-nobox")) {
        *dobox = 0;
        printf("Plot mode: No box \n");
      } else if (strings_compare(argv[i],"-log")) {
        *dolog = 1;
        printf("dolog set to %d \n",*dolog);
      } else if (strings_compare(argv[i],"-mmm")) {
        *dommm = 1;
        printf("dommm set to %d \n",*dommm);
      } else if (strings_compare(argv[i],"-h")) {
        /* print the help on line */
	display_help(argv[0]);
      } else {
        /* unknown argument passed down - stop! */
    	printf("\nArgument for display device should be -g <device> ");
        printf(" Exiting... \n\n");
	exit(-1);
      }
      i++;
    }
  }
  /* selecting the standard output device */
  if (!device_selected) strcpy(inpdev,STD_DEVICE);
}

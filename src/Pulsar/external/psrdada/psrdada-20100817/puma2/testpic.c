#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "pic.h"

void usage()
{
    fprintf (stdout,
	   "testpic [options]\n"
	     " -a arm pic\n"
	     " -d disarm pic \n");
    
}

int main (int argc, char **argv)
{
 	pic_t pic;	
	int arg = 0, start=0, stop=0;
	while ((arg=getopt(argc,argv,"ad")) != -1) {
	    switch (arg) {
		
		case 'a':
		    start=1;
		    break;
		    
		case 'd':
		    stop=1;
		    break;
		    
		default:
		    usage ();
		    return 0;
		    
	    }
	}
	
	
	/* open PiC card and configure -  */
	if ( (pic_open(&pic, 0)) < 0 ){
	    fprintf(stderr,"PiC open failed\n");
	    return -1;
	}
	
	if(start){
	    fprintf(stdout,"Arming pic ...\n");
	    if ( (pic_configure(&pic, 1)) < 0 ){
		fprintf(stderr,"Cannot configure PiC\n");
		return -1;
	    }
	}
	
	if(stop){
	    fprintf(stdout,"disarming pic ...\n");
	    if ( (pic_configure(&pic, 2)) < 0 ){
		fprintf(stderr,"Cannot configure PiC\n");
		return -1;
	    }
	}
}


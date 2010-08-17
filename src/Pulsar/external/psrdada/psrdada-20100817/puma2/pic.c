
#include "pic.h"

#include <sys/types.h>
#include <unistd.h>

/* function definitions for PuMa-II Interface Card */

int pic_open(pic_t* pic_ptr, unsigned int unit_no)
{
    if ( (pic_ptr->picdev = open("/dev/tim0",O_RDWR)) < 0) {
	perror("open: ");
	fprintf(stderr,"pic_open: cannot open /dev/tim0\n");
	return -1;
    }
    return 0;
}

int pic_configure(pic_t* pic_ptr, int option)
{
    unsigned long CONF_WORD;

    ioctl(pic_ptr->picdev, TIM_IOPREFCNT, 0x01);  

    switch(option){
    /* move to location where conf word has to be written */
	case 0:
	    if( (lseek(pic_ptr->picdev,CONF_ADDR,SEEK_SET)) < CONF_ADDR) {
		perror("lseek (pic_configure) : ");
		return -1;
	    }

	    /* write configuration word */
	    CONF_WORD = EN_LINK | EN_OUTPUT_HDCONN;
	    if ( (write(pic_ptr->picdev,&CONF_WORD,4)) < 4 ){
		perror("write (pic_configure) : ");
		return -1;
	    }
	    break;

	case 1: 

	    /* a temporary solution.....start pic, right away.... */
	    /* enables, clock and data to EDT on next SYSTICK */
	    
	    CONF_WORD = START;
	    lseek(pic_ptr->picdev,START_ADDR,SEEK_SET);
	    write(pic_ptr->picdev,(void *) &CONF_WORD,4);
	    break;

	case 2: /* stop pic */
	    CONF_WORD = STOP;
	    lseek(pic_ptr->picdev,STOP_ADDR,SEEK_SET);
	    write(pic_ptr->picdev,(void *) &CONF_WORD,4);
	    break;
	    
	default:
	    fprintf(stderr,"unrecognized parameter for pic");
	    return -1;
    }

    /* read status from PiC, and see if configration is active */
    return 1;
}

int pic_status(pic_t* pic_ptr)
{
    return 1;
}

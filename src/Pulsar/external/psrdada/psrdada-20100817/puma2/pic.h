
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>

#include "tim.h"

/* various locations in pic board */

#define BASE_ADDR 0x00
#define START_ADDR (BASE_ADDR + 0x04)
#define STOP_ADDR (BASE_ADDR + 0x08)

/* #define STOP_ADDR  (BASE_ADDR + 0x04) */
#define WRITE_SIZE 0x04
#define CONF_ADDR  (BASE_ADDR + 0x08)

#define OK 0x0001
#define NOT_OK 0x0000

/* PiC status stuff*/
#define SERIAL_LINK (0x0001 >> 0)
#define EDT_CONNECT (0x0002 >> 1)
#define SELF_TEST   (0x0004 >> 2)
#define POL_TYPE /*polarization state */
#define TP_DONE  /*total power computed? */

/* PiC configuration stuff */
#define EN_OUTPUT_HDCONN (0x0001)
#define EN_OUTPUT_CONN (0x0002)
#define EN_LINK (0x0004)
#define EN_LINK_TEST (0x0008)
#define GET_TP (0x0010) /* compute total power*/
#define START  (0x04)
#define STOP   (0x08)


typedef struct {
    int picdev;
    int pic_addr;
    int pic_data;
    unsigned int pic_status;
    unsigned int conf_word;
    unsigned int bufsize; /* a memory block to be used */
    /* for future version of driver .... */
} pic_t;

/* pic functions */
int pic_configure(pic_t* pic_ptr, int option);
//pic_t *pic_open(unsigned int unit_no);
int pic_open(pic_t* pic_ptr, unsigned int unit_no);
int pic_status(pic_t* pic_ptr);

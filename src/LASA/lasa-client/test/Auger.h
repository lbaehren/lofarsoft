#include <stdio.h>
#include "../ftd2xx.h"

#define SWAP(a) (a)=((a&0xff)<<8)+((a&0xff00)>>8); 
#define BUF_SIZE 300
 
FT_HANDLE init_usb();
void close_usb();

#ifndef __WAPP_HEADERS_H
#define __WAPP_HEADERS_H

#include <stdint.h>
// Include all header versions
#include "headers/wapp_header_02.h"
#include "headers/wapp_header_03.h"
#include "headers/wapp_header_04.h"
#include "headers/wapp_header_05.h"
#include "headers/wapp_header_06.h"
#include "headers/wapp_header_08.h"
#include "headers/wapp_header_07.h"
#include "headers/wapp_header_09.h"

// Specify which we want to use (typically the newest)
typedef struct WAPP_HEADER_v9 WAPP_HEADER;

#define WAPP_INTLAGS   0   /* 16 bit integers - searching only      */
#define WAPP_LONGLAGS  1   /* 32 bit integers - searching only      */
#define WAPP_FLOATLAGS 2   /* 32 bit float ACF/CCFs folding only    */
#define WAPP_FLOATSPEC 3   /* 32 bit float fftd ACFs folding only   */
#define WAPP_BYTELAGS  4   /* 8 bit integers ACF/CCF searching only */
#define WAPP_NIBBLAGS  5   /* 4 bit integers ACF/CCF searching only */
#define WAPP_FOLD32    8   /* bit mask with FLOATLAGS and FLOATSPEC */
                      /* if on 32 bit folding is turned on     */

#endif

/***************************************************************************
 *
 *   Copyright (C) 1998 by Stuart Anderson
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "ieee.h"
#include "machine_endian.h"

/*
 * From the ANSI IEEE 754-1985 floating point standard:
 * 128-bit long double:
 * [127] sign  [112-126] exponent  [0-111] fraction
 *
 * 64-bit double:
 * [63] sign  [52-62] exponent  [0-51] fraction
 *
 * Stuart Anderson
 * 17 Sep 1998
 */

/*
 * Byte swaping routines.
 */
void
swapx2( void *arg )
{
#if MACHINE_LITTLE_ENDIAN
    char tmp;
    char *ptr = (char*)arg;

    tmp     = ptr[1];
    ptr[1]  = ptr[0];
    ptr[0]  = tmp;
#endif
}



void
swapx8( void *arg )
{
#if MACHINE_LITTLE_ENDIAN
    char tmp;
    char *ptr = (char*)arg;

    tmp     = ptr[7];
    ptr[7]  = ptr[0];
    ptr[0]  = tmp;

    tmp     = ptr[6];
    ptr[6]  = ptr[1];
    ptr[1]  = tmp;

    tmp     = ptr[5];
    ptr[5]  = ptr[2];
    ptr[2]  = tmp;

    tmp     = ptr[4];
    ptr[4]  = ptr[3];
    ptr[3]  = tmp;
#endif
}



/*
 * Copy num bits from in to out starting with the specified bit offsets, e.g.,
 * copy_bits( out, 3, & 0xf31245, 2, 13)
 * out[0] = (out[0] & 0xe0) | 0x15
 * out[1] = 0x89
 */
int
copy_bits(u_char *out, u_int out_offset, u_char *in, u_int in_offset, int num)
{
    int i;
    u_char bit;


    if (in == NULL) {
	fprintf(stderr, "copy_bits(): in == NULL.\n");
	return -1;
    }
    if (out == NULL) {
	fprintf(stderr, "copy_bits(): out == NULL\n");
	return -1;
    }


    /*
     * Copy the bits on-by-one.
     */
    for (i=0; i<num; i++) {
	/*
	 * Remove any integral byte offsets.
	 */
	in += in_offset/8;
	in_offset %= 8;
	out += out_offset/8;
	out_offset %= 8;

	*out &= ~(0x80 >> out_offset);		/* erase out bit	*/
	bit = (*in) & (0x80 >> in_offset);	/* read input bit	*/
	bit <<= in_offset++;			/* shift to MSB		*/
	bit >>= out_offset++;			/* shift to out location*/
	*out |= bit;				/* write bit to out	*/
    }

    return 0;
}



int
cnvrt_long_double(u_char *data, double *integer, double *fraction)
{
    int i;
    long de;
    u_char s, f;
    u_short e;
    u_short *ldptr = (u_short*)data;
    u_short *dptr;


    s = data[0] & 0x80;			/* Sign bit -- leave in MSB	*/
    e = ldptr[0];			/* Exponent bits -- byte swap	*/
    swapx2((char*)&e);
    e &= 0x7fff;
    f = 0;				/* fractional bits -- 0 or not	*/
    for (i=2; i<16; i++)
	f |= data[i];

    switch (e) {
    case 0:
	if (f) {
	    /*
	     * Subnormal.
	     */
	    fprintf(stderr, "cnvrt_long_double(): subnormal number.\n");
	    return -1;
	} else {
	    /*
	     * Signed zero, just return two positive zeros.
	     */
	    *integer = 0;
	    *fraction = 0;
	    return 0;
	}
	break;
    case 32767:
	if (f) {
	    /*
	     * NaN
	     */
	    fprintf(stderr, "cnvrt_long_double(): subnormal number.\n");
	} else {
	    /*
	     * Infinity.
	     */
	    if (s) {
		/*
		 * -INF
		 */
	        fprintf(stderr, "cnvrt_long_double(): -INF.\n");
	    } else {
		/*
		 * +INF
		 */
	        fprintf(stderr, "cnvrt_long_double(): +INF.\n");
	    }
	}
	return -1;
	break;
    default:
	/*
	 * Normal.
	 */

	/*
	 * Integer double.
	 */
	*integer = 0;
	dptr = (u_short*)integer;
	*((u_char*)integer) = s;	/* sign bit	*/

	/*
	 * Exponent.
	 */
	de = e - 16383 - 1;
	if (de >= -1) {
	    if (de >= 1023) {
	        fprintf(stderr, "cnvrt_long_double(): integer overflow: de %ld.\n", de);
	        return -1;
	    }
	    e = de + 1023 + 1;
	    e <<= 4;
	    swapx2((char*)&e);
	    dptr[0] |= e;

	    /*
	     * Fraction, copy bits corresponding to integer part.
	     */
	    if (de >= 52) {
	        fprintf(stderr, "cnvrt_long_double(): integer underflow.\n");
	        return -1;
	    }
	    copy_bits((u_char*)dptr, 12, (u_char*)(&ldptr[1]), 0, de+1);
	    swapx8((char*)integer);
	}

	

	/*
	 * Fractional double.
	 */
	*fraction = 0;
	dptr = (u_short*)fraction;
	*((u_char*)fraction) = s;	/* sign bit	*/

	if (de >= -1) {
	    /*
	     * Exponent.
	     */
	    e = -1 + 1023 + 1;
	    e <<= 4;
	    swapx2((char*)&e);
	    dptr[0] |= e;

	    /*
	     * Fraction.
	     */
	    copy_bits((u_char*)dptr, 12, (u_char*)(&ldptr[1]), de+1, 52);
	    swapx8((char*)fraction);
	    *fraction -= (s) ? -1 : 1;
	} else {
	    /*
	     * There was no integer part.
	     */
	    if ((de < -1023) || (de >= 1023)) {
	        fprintf(stderr, "cnvrt_long_double(): fractional overflow: de %ld.\n", de);
	        return -1;
	    }
	    /*
	     * Exponent.
	     */
	    e = de + 1023 + 1;
	    e <<= 4;
	    swapx2((char*)&e);
	    dptr[0] |= e;

	    /*
	     * Fraction.
	     */
	    copy_bits((u_char*)dptr, 12, (u_char*)(&ldptr[1]), 0, 52);
	    swapx8((char*)fraction);
	}
	break;
    }

    return 0;
}


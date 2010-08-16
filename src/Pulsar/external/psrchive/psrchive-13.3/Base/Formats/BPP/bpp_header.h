/* header.h - CDRP header for sun c code */
/* 94jun12 - md  - created                */
/* 94jun17 - dcb - modified V1.0          */
/* 07dec03 - pbd - Updated for 64-bit machines: changed all longs
 *                 to int32_t (this should be portable). */

#ifndef __BPP_HEADER_H
#define __BPP_HEADER_H

/* note this is aligned for the SUN, the PC must align the same way ! */
struct bpp_header {
  char                  version[4];          /* of header/data format       */
  int32_t               telescope;           /* corresponds to xyz data file*/

  char                  pulsar_name[10];     /* J or B                      */
  char                  dummy1[6];
  double                apparent_period;     /* [s] at mid scan             */
  double                dispersion_measure;  /* pc cm**-3                   */

  int32_t               scan_number;
  int32_t               crate_id;            /* 0->3                        */
  int32_t               year;                /* [year or 0 if JD] start time*/
  int32_t               day;                 /* [yearday or JD] start time  */
  int32_t               seconds;             /* [s] start time              */
  char                  dummy2[4];
  double                second_fraction;     /* [s] start time              */

  double                RF_of_chan_0;        /* [Hz] RF of IF_0 + SRAM_0    */
  double                bandwidth;           /* [Hz] apparent bandwidth     */
  double                integration_time;    /* [s]                         */
  int32_t               bins;                /* samples per period          
                                                 1<bins<1024                */
  int32_t               bds;                 /* boards in use               */
  int32_t               chsbd;               /* channels per board          */
  int32_t               polns;               /* polarizations (1, 2 or 4)   */

/* data formats: 

   1<chsbd*bds<32 (or 64 in S mode which has polns=1) 

polns = 1:  (1,2,3,...bins)*(1,2,...chsbd<=8)*(1,2,...bds<=8)
polns = 2:  (1,2,3,...bins)*(1,2,...chsbd<=4)*(2)*(1,2,...bds<=8)
polns = 4:  (1,2,3,...bins)*(1,2,...chsbd<=4)*(4)*(1,2,...bds<=8)
*/

  double                IF0_gain;            /* [dB]                        */
  double                IF1_gain;            /* [dB]                        */
  double                IF2_gain;            /* [dB]                        */
  double                IF3_gain;            /* [dB]                        */

  char                  modes[4];            /* 1: Polarizations
                                                    P - 4, parallel and cross
                                                    I - 2, parallel only 
                                                    S - 1, single IF
                                                    V - 1, voltage data
                                                2: IF frequency inversion
                                                    D - direct
                                                    I - inverted
                                                3: Observing mode
                                                    O - observation
                                                    C - noise calibration
                                                    S - standard source
                                                4: Spare                    */
  char                  enabled_DBs[8];      /* U - in use; D - not in use  */
  char                  SRAM_bytes[4];
  char                  HARRIS_bytes[4];
  char                  PULFIR_bytes[4];

  double                RF_of_IFcenter_0;   /* [Hz]                        */
  double                RF_of_IFcenter_1;   /* [Hz]                        */
  double                RF_of_IFcenter_2;   /* [Hz]                        */
  double                RF_of_IFcenter_3;   /* [Hz]                        */
  double                SRAM_base_freq;     /* [Hz] LO of channel 0 on FBs */
  double                SRAM_freq_incr;     /* [Hz] spacing between FB LOs */

  int32_t               bytes;              /* number of data bytes follow */
  char                  dummy4[4];
};


#endif

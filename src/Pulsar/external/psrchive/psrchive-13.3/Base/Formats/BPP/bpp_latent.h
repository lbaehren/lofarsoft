#ifndef _BPP_LATENT_H
#define _BPP_LATENT_H
/* Stuff relating to bpp latentcy.  Table here is copied from 
 * /psr4/mpulsar/bpp/toa/test/latent.doc 
 *
 * I think we only need cols 2 and 5.
 */

/*
Total     Channel BW    Decimation   taps   Latency (s)
BW Hz       Hz                              Intensity        Poln
2.8e6      0.0875d6        16        1024   5.99500e-3    5.99500e-03
4.0e6      0.125d6         16        1024   4.19650E-3    4.19650E-3
5.6e6      0.175d6         16        1024   2.99750E-3    2.99750E-3
8.0e6      0.250d6         16        1024   2.09825E-3    2.09825E-3
11.2e6     0.350d6         16        1024   1.49875E-3    1.49875E-3
16.0e6     0.500d6         16        1024   1.049125E-3   1.049125E-3
22.4e6     0.700d6         16        1024   749.375E-6     749.375E-6
28.0e6     0.875d6         16        1024   599.500E-6     599.500E-6
44.8e6     1.400d6         16         512   192.768E-6     192.768E-6
56.0e6     1.750d6         16         512   154.214E-6     154.214E-6
89.6e6     2.800d6         12         256    52.232E-6       52.232E-6
112.0e6    3.500d6         10         256   41.8429E-6     41.8429E-6
*/

#define BPP_NLATENT 12

/* Channel bandwidths in mhz */
static const double bpp_chbw[BPP_NLATENT] = {
    0.0875,
    0.125,
    0.175,
    0.250,
    0.350,
    0.500,
    0.700,
    0.875,
    1.400,
    1.750,
    2.800,
    3.500
};

/* Latency in seconds */
static const double bpp_latent[BPP_NLATENT] =  {
    5.99500e-3,
    4.19650E-3,
    2.99750E-3,
    2.09825E-3,
    1.49875E-3,
    1.049125E-3,
    749.375E-6,
    599.500E-6,
    192.768E-6,
    154.214E-6,
    52.232E-6,
    41.8429E-6
};

#endif


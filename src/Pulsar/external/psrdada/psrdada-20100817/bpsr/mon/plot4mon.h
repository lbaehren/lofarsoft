/***************************************************************************/
/*                                                                         */
/* include file for plot4mon                                               */
/*                                                                         */
/* Ver 1.0                                                                 */
/*      AP 30 Sept 2008                                                    */
/*                                                                         */
/* Ver 2.0                                                                 */
/*      RB 04 Oct 2008                                                     */
/*	default plot device: png                                           */
/*                no longer need to set large stack sizes (use of malloc)  */
/*	RB 05 Oct 2008                                                     */
/*		  plots with x axes in proper units (e.g. MHz, secs, Hz)   */
/*      RB 06 Oct 2008                                                     */
/*		  added set_dimensions for high resolution mode            */
/*      WV 19 Nov 2008                                                     */
/*                plotting min mean and max values of the timeseries       */
/* Ver 3.0                                                                 */
/*      AP 21 Nov 2008                                                     */
/*                added zooming capabilities                               */
/*                                                                         */
/***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cpgplot.h"
#define BPSRMON_VERSION 3.0 
#define STD_DEVICE "/png"
#define MAX_PERIOD 4.0 /* max period to look at in FFT in sec */
#define MIN_PERIOD 0.001 /* min period to look at in FFT in sec */
#define MMM_REDUCTION 512

void get_commandline(int,char *argv[],char *,char *,char *,char *,
                     float *,float *,float *,float *,
                     int *,int *,int *,int *,unsigned *,unsigned *);
void display_help(char *);
void compute_extremes(float *, long, long, float *, float *);
void compute_margin(long, float, float, float *);
void read_params(char *, int *, float *, float *, float *, int *, 
                 float *, int *, int *, char *);
void read_stream(int , char *, float *, long *);
void work_on_data(char *, float *, float *, long, long *, 
                  float, float, int, char *, int, int);
void create_xaxis(char *, int, long, long, long *, long *, 
                  float, float, 
                  float, float, 
                  float, float, float, int, 
                  float, float *); 
void set_array_dim(long, int, int *, int *);
void create_outputname( char *, char *, char *, int, char *);
void create_labels(char *, int, char *, char *, char *);

void do_powerspec(float *, float *, long, long *, float);
void do_maxmin(float *, float *, long, long *, float);

void realft(float *, unsigned long, int);
void whiten(float *, long, long);
float selectmed(unsigned long, unsigned long, float *);
int check_file_exists(char *);
int strings_compare(char *, char *); 
int plot_stream_1D(float *, float *, float *, long, long, long,
                char *, char*, char *, char *,
		   int, int, int, unsigned, unsigned);
int plot_stream_2D(float *, int, int, float, int,
                char *, char*, char *, char *);
void set_dimensions (unsigned, unsigned);

/***************************************************************************/

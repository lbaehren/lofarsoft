/*
    corrsub.c   :  Miscellaneous library routines for handling Corr
                   includes text generation from binary,
                   Endian Conversion, etc...

                   EXTENSIVELY REVISED VERSION /crs 99apr08

                   Requires newcorr.h  which is incompatible
                   with the old corr.h file.  Many structures
                   have also undergone extensive revision

   Think carefully before editing Keywords defined in this file !
*/
# include <stdio.h>
# include <string.h>
# include <ctype.h>
# include <limits.h>
# include <stdlib.h>
# include "gmrt_newcorr.h"

typedef struct { int card, chip ; } MacChipType ;

enum {Len = 128, Hdrlen = 64*1024};
static int SysInit=0 ;
char Buf[Hdrlen], *Bufp ;
const char cont_char = '*', begin_char = '{', end_char = '}';

enum {Version, Ants, Chan_max, Pols, Samplers, Macs, F_step, Clock,
      Sta, StaTime, T_unit, Rf, First_lo, Bb_lo, Antmask, Bandmask,
      Mode, Seq, Object, Observer, Project, Code,
      Dra, Ddec, Ra_date, Dec_date, MJD, Date_obs,
      Lta, Samp_num, Chan_num, Mac_num, Integ, 
      Mac_mode, Fft_mode, Dpc_mux, Clk_sel, Net_sign,Vars} VarType;
char *varname[Vars] =
     {"VERSION","ANTS","CHAN_MAX","POLS","F_ENABLE","X_ENABLE","F_STEP","CLOCK",
      "STA","STATIME","T_UNIT","RF","FIRST_LO","BB_LO","ANTMASK","BANDMASK",
      "MODE","SEQ","OBJECT","OBSERVER","PROJECT","CODE",
      "DRA/DT","DDEC/DT","RA-DATE","DEC-DATE","MJD_SRC","DATE-OBS",
      "LTA","SAMP_NUM", "CHAN_NUM","MAC_NUM","INTEG",
      "MAC_MODE", "FFT_MODE", "DPC_MUX", "CLK_SEL", "NET_SIGN"};

char PrjCode[16];

static int DasCardId=-1;

static char *dpc_modes[DpcMuxVals] = 
       { "IndianPolar", "UsbPolar", "LsbPolar", "UsbCopy", "LsbCopy",    /* new modes */
	 "AllU130", "AllU175", "AllL130", "AllL175",
	 "arar_arar", "alal_alal", "brbr_brbr", "blbl_blbl", /* historical */
         "aral_brbl", "aral_alar", "brbl_blbr", "arbr_albl",
        } ;


#ifdef LTA_FILE_VERSION
static char LtaFileVersion[]=LTA_FILE_VERSION;
#else
static char LtaFileVersion[]="COR30x2 LTA1.10 DAS1.10 HST00";
#endif

void swap_bytes(unsigned short *p, int n)
{ while (n-- > 0) p[n] = p[n]/256 + (p[n] % 256)*256; }
void swap_short(unsigned *p, int n)
{ while (n-- > 0) p[n] = p[n]/65536 + (p[n] % 65536)*65536; }
void swap_long(void *p, int n)
{ swap_short(p, n); swap_bytes(p, 2*n); }
void swap_d(double *p, int n)
{ float temp, *v;
  swap_long(p, 2*n) ;
  while(n-- > 0)
  { v = (float *)p ;
    temp = *v; *v = v[1] ; v[1] = temp ;
    p++ ;
  }
} 
  
void flipAnt(AntennaParType *ant)
{
  swap_d(&(ant->bx),3);
  swap_d(ant->d0,MAX_BANDS);
  swap_d(ant->p0,MAX_BANDS);
}
void flipCorrPar(CorrParType *corrpar)
{
  swap_long(&(corrpar->macs),7);
}
void flipDasPar(DasParType *daspar)
{
  swap_long(&(daspar->antmask),5);
  swap_bytes((unsigned short *)&(daspar->bandmask),2);
  swap_bytes((unsigned short *)daspar->chan_num,MAX_CHANS);
  swap_d(&(daspar->mjd_ref),2);
}
void flipSourcePar(SourceParType *source)
{ 
  swap_long(&source->flux, 4) ;
  swap_d(&source->mjd0, 18) ;
  swap_long(&source->id, MAX_BANDS+4) ;
  swap_bytes((unsigned short *)&source->bandmask, 4) ;
}
void flipProject(ProjectType *proj)
{ 
  swap_long(&proj->antmask, 1) ;
  swap_bytes(&proj->bandmask, 2) ;
}
void flipScanInfo(ScanInfoType *scan)
{
  swap_long(&scan->status, 2) ;
  flipProject(&scan->proj) ;
  flipSourcePar(&scan->source);
}
void flipCorr(CorrType *corr)
{
  int i;
  if (corr->endian & LittleEndian)
       corr->endian &= ~LittleEndian;
  else corr->endian |= LittleEndian;
  for (i=0; i<MAX_ANTS; i++)flipAnt(&(corr->antenna[i]));
  flipCorrPar(&corr->corrpar);
  flipDasPar(&corr->daspar);
}

int print_range(char *str, short *val, int n )
{ int   i, j, k,d;
  char  s_tmp[1024],*q;

  /* check if adjacent values have uniform spacing */
  q=s_tmp;
  for (i=0; i < n-2; )
  { d = val[i+1] - val[i];
    for (j=i+2; j < n && (val[j] - val[j-1]) == d; j++);
    if (j > i+2)
    { q+=sprintf(q," %d:%d:%d", val[i], val[j-1], d); i = j; }
    else { q+=sprintf(q," %d", val[i]); i++; }
  }
  for (; i < n; i++) q += sprintf(q, " %d", val[i]);
  if(q-s_tmp<70){ strcpy(str,s_tmp); return q-s_tmp;}
  
  /* check if every alternate value has uniform spacing */
  q=s_tmp;
  for(i=0,k=0;i<n-4;)
  {  d=val[i+2]-val[i];
     for (j=i+4; j < n && (val[j] - val[j-2]) == d; j+=2);
     if(j>i+4)
     { q+=sprintf(q," %d:%d:%d", val[i], val[j-2], d); k+=(j-i)/2; i = j; }
     else { q+=sprintf(q," %d", val[i]); i+=2; k++; }
  }
  for (; i < n; i++){ q += sprintf(q, " %d", val[i]); k++;}
  
  for(i=1;i<n-4;)
  {  d=val[i+2]-val[i];
     for (j=i+4; j < n && (val[j] - val[j-2]) == d; j+=2);
     if(j>i+4)
     { q+=sprintf(q," %d:%d:%d", val[i], val[j-2], d); k+=(j-i)/2; i = j; }
     else { q+=sprintf(q," %d", val[i]); i+=2; k++;}
  }
  for (; i < n; i++){ q += sprintf(q, " %d", val[i]); k++; }
  if(k==n && q-s_tmp<70){ strcpy(str,s_tmp); return q-s_tmp;}

  /* give up --  do a brute force copy */
  q=s_tmp;
  for(i=0;i<n;i++)q += sprintf(q, " %d", val[i]);
  strcpy(str,s_tmp);
  return strlen(s_tmp);

}

int dpc2fft(int samp, int dpcmux, int *fft_id)
{ 
  int i,k, dpc = samp/4, chan = samp%4 ;
  unsigned char s,muxval=dpcmux;

  for(k=0;k<4;k++)fft_id[k]=MAX_FFTS;

  for (i=0,k=0; k<4; k++) 
  { s = muxval & 0x3; 
    if(s == chan){ fft_id[i]= dpc*4 + k;i++;}
    muxval=muxval>>2;
  }
  return i;
}
int put_antpar (CorrType *corr, char *p)
{
  int id ;
  char *p0 = p ;
  p+=sprintf(p,"{ Antenna.def \n");
  p+=sprintf(p,"*AntId  = Ant      bx        by        bz      "
               "delay0    delay1\n");
  for (id=0; id<MAX_ANTS ; id++)
  { AntennaParType ant = corr->antenna[id] ;
    p+=sprintf(p,"ANT%-2.2d   = %3.3s %9.2f %9.2f %9.2f %9.2f %9.2f\n",id,
            ant.name, ant.bx,ant.by,ant.bz,ant.d0[0],ant.d0[1] );
  }
  p+=sprintf(p,"} Antenna\n*   \n");
  return p-p0 ;
}
int put_bandnames(CorrType *corr, char *p)
{ int i ;
  char *p0 = p ;
  p+=sprintf(p,"{ Bandnames.def \n");
  for (i=0; i<4; i++)p+=sprintf(p,"BAND%02d  = %s\n",i, corr->bandname[i]);
  p+=sprintf(p, "} Bandnames\n*   \n");
  return p-p0 ;
}
int put_syspar(CorrType *corr, char *p)
{
  char *p0 = p ;
  p+=sprintf(p,"{ Corrsys.def\n");
  p+=sprintf(p,"%-8.8s= %s     \n", varname[Version], corr->version );
  p+=sprintf(p,"%-8.8s= %f Hz  \n",   varname[Clock], corr->corrpar.clock);
  p+=sprintf(p,"%-8.8s= %d Cycles\n",varname[Sta   ], corr->corrpar.sta);
  p+=sprintf(p,"%-8.8s= %d usec\n",varname[StaTime ], corr->corrpar.statime) ;
  p+=sprintf(p,"%-8.8s= %f  sec\n",varname[T_unit  ], corr->daspar.t_unit);
  p+=sprintf(p,"%-8.8s= %d     \n",varname[Ants    ], MAX_ANTS );
  p+=sprintf(p,"%-8.8s= %d     \n",varname[Samplers], corr->daspar.samplers);
  p+=sprintf(p,"%-8.8s= %d     \n",varname[Macs    ], corr->corrpar.macs);
  p+=sprintf(p,"%-8.8s= %d     \n",varname[Chan_max], corr->corrpar.channels);
  p+=sprintf(p,"%-8.8s= %d     \n",varname[Pols    ], corr->corrpar.pols);
  p+=sprintf(p,"} Corrsys\n*  \n");
  return p-p0 ;
}
int put_samppar(CorrType *corr, char *p)
{ int i,j,n ;
  char *p0 = p ;
  p+=sprintf(p, "{ Sampler.def \n");
  p+=sprintf(p,"* Even numbered are lower pipelines of the samplers.\n");
  p+=sprintf(p,"* Odd  numbered are upper pipelines of the samplers.\n");
  p+=sprintf(p,"*SampId = Ant BandId FftId\n");


  for(i=0,n=0;i<corr->daspar.samplers;i++) 
    if((j=corr->sampler[i].dpc + 1) >n)n=j;

  for (i=0; i < n ; i++)
  {
    p+=sprintf(p,"SMP%03d  = ", i);
    for (j=0; j<corr->daspar.samplers; j++)
    { SamplerType *samp = corr->sampler + j ;
      if (samp->dpc == i)
      { p+=sprintf(p,"%s %s %03d\n", corr->antenna[samp->ant_id].name,
                   corr->bandname[samp->band], samp->fft_id) ;
        break ;
      }
    }

    if (j == corr->daspar.samplers) p+=sprintf(p,"None\n");
  }
  p+=sprintf(p, "} Sampler\n*   \n");
  return p-p0 ;
}

int put_baselinepar( CorrType *corr, char *p)
{ int i, j ;
  char *p0 = p ;

#if 0
  p+=sprintf(p, "{ Mac.def \n");
  p+=sprintf(p,"* Value of %d for FFTi indicates no (ant,band) connected "
                 " to corres SAMP\n", MAX_FFTS);
  p+=sprintf(p,"* (FFT0,FFT1) and (FFT2,FFT3) are two baselines (pols)\n");
  p+=sprintf(p,"*MacId  = FFT0 FFT1 FFT2 FFT3\n");
  for (base=corr->baseline, i=0; i < corr->corrpar.macs; i++ )
  { p+=sprintf(p,"MAC%03d  =", i);
    for (j=0; j < 2; j++)
      p+=sprintf(p,"  %03d", base->samp[j].fft_id) ;
    if (base->word_incr == 1)
      p+=sprintf(p,"  %03d  %03d",MAX_FFTS, MAX_FFTS) ;
    else for (j=0; j<2; j++)
      p+=sprintf(p,"  %03d", base[corr->corrpar.macs].samp[j].fft_id) ;
 
    p+=sprintf(p,"\n");
    /* corr->daspar.mac_num[i] = i; */ /* redefine MAC numbers. WHY??  */
  }
  p+=sprintf(p, "} Mac\n*   \n");
#endif

  p+=sprintf(p, "{ Baseline.def \n");
  p+=sprintf(p,"* Baseline conf. that correspond to a pair of (ant,band)\n");
  p+=sprintf(p,"*BaseId = A0 B0 A1 B1 SMP0 SMP1 Ant0   Band0 Ant1   Band1\n");
  for (i=0; i < corr->daspar.baselines; i++ )
  { BaseParType *base = &corr->baseline[i];
    if(strstr(corr->version,"COR30x2")==NULL)
      p+=sprintf(p,"BAS%03d  =", i);  /* baseline would finally require 4d (crs pre 99) */
    else
      p+=sprintf(p,"BAS%04d =", i);   /* we do need 4d! (jnc 19jul05) */
    for (j=0; j < 2; j++)
      p+=sprintf(p," %02d %02d", base->samp[j].ant_id, base->samp[j].band);

    p+=sprintf(p,"  %03d  %03d", base->samp[0].dpc, base->samp[1].dpc);
    for (j=0; j < 2; j++)
      p+=sprintf(p,"  %s %s", corr->antenna [base->samp [j].ant_id].name,
                              corr->bandname[base->samp[j].band]);
    p+=sprintf(p,"\n");
    /* corr->daspar.base_num[i] = i; */ /* redefine baseline numbers. WHY?? */
  }
  p+=sprintf(p, "} Baseline\n*   \n");
  return p-p0 ;
}

int put_selection(CorrType *corr, char *p)
{ char str[1024] ;
  char *p0 = p;
  short samp_num[MAX_SAMPS], i ;
  int   l;

  for (i=0; i<corr->daspar.samplers; i++)samp_num[i] = corr->sampler[i].dpc ;
  for(i=0;i<1024;i++)str[i]=' ';

  p+=sprintf(p,"{ Corrsel.def \n");
  p+=sprintf(p,"%-8.8s= %hx \n",  varname[Mode    ], corr->daspar.mode);
  p+=sprintf(p,"%-8.8s= %d \n",  varname[Lta     ], corr->daspar.lta);

  l=print_range(str, samp_num, corr->daspar.samplers);
  if(l<70)
    p+=sprintf(p,"%-8.8s= %s\n",varname[Samp_num], str) ;
  else    /* add some more keywords so that the ASCII header has all the samplers */
  { char name[8],s[1024];
    int j=0,j0=0;
    for(i=0;i<11;i++)
    { if(i==0) sprintf(name,"SAMP_NUM");
      else sprintf(name,"SMP_NUM%1d",i-1);
      j0=j;j+=69;
      if(j>strlen(str))
      { p+=sprintf(p,"%-8.8s= %s\n",name, str+j0); break;}
      while(str[j]!=' ')j--;
      strcpy(s,str+j0);s[j-j0]=0;
      p+=sprintf(p,"%-8.8s= %s\n",name, s); 
    }
  }

  l=print_range(str, corr->daspar.chan_num, corr->daspar.channels);
  if(l>69)
  { str[69]=0;
    fprintf(stderr,"WARNING:print_range 80 chars exceeded!\n");
    fprintf(stderr,"WARNING:xtract may be unable to read this file\n");
  }
  p+=sprintf(p,"%-8.8s= %s\n",varname[Chan_num], str) ;

  p+=sprintf(p,"} Corrsel\n*  \n");
  return p-p0 ;
}

int put_scanpar (int id, int seq, ScanInfoType *scan, char *p)
{
  ProjectType *proj = &scan->proj ;
	SourceParType *source = &scan->source ;


  double rpd = (double)(3.14159265358979323846)/180.0;
  char *p0 = p ;

  p+=sprintf(p,"{ SubArray%1d.def \n",id);
  p+=sprintf(p,"%-8.8s= %08x\n",varname[Antmask ], proj->antmask );
  p+=sprintf(p,"%-8.8s= %04x\n",varname[Bandmask], proj->bandmask);
  p+=sprintf(p,"%-8.8s= %d\n",  varname[Seq     ], proj->seq     );

  p+=sprintf(p,"%-8.8s= %s \n", varname[Project ], proj->title   );
  p+=sprintf(p,"%-8.8s= %s \n", varname[Code    ], proj->code    );
  p+=sprintf(p,"%-8.8s= %s \n", varname[Object  ], source->object  );
  p+=sprintf(p,"%-8.8s= %f\n",  varname[Ra_date ], source->ra_mean/rpd );
  p+=sprintf(p,"%-8.8s= %f\n",  varname[Dec_date], source->dec_mean/rpd);

  p+=sprintf(p,"%-8.8s= %f\n",  varname[MJD], source->mjd0);

  p+=sprintf(p,"%-8.8s= %f \n",  varname[Dra     ], source->dra/rpd  );
  p+=sprintf(p,"%-8.8s= %f \n",  varname[Ddec    ], source->ddec/rpd );
  p+=sprintf(p,"%-8.8s= %f \n", varname[F_step   ], source->ch_width   );
  p+=sprintf(p,"%-8.8s= %d  %d \n",varname[Rf      ], (int)source->freq[0], (int)source->freq[1] ) ;
  p+=sprintf(p,"%-8.8s= %d %d\n",  varname[First_lo],(int)source->first_lo[0],(int)source->first_lo[1]) ;
  p+=sprintf(p,"%-8.8s= %d %d\n",  varname[Bb_lo   ], (int)source->bb_lo[0],(int)source->bb_lo[1] ) ;
  p+=sprintf(p,"%-8.8s= %d %d %d %d\n", varname[Net_sign], source->net_sign[0],
	     source->net_sign[1],source->net_sign[2],source->net_sign[3]);
  p+=sprintf(p,"} SubArray%1d  \n",id);
  return p-p0 ;
}


int corrgen(CorrType *corr, char *p)
{ char *p0 = p ;
  p+=put_syspar(corr, p) ;
  p+=put_antpar(corr, p);
  p+=put_bandnames(corr, p) ;
  p+=put_samppar(corr, p) ;
  p+=put_baselinepar(corr, p) ;
  p+=put_selection(corr, p) ;
  p+=sprintf(p,"END_OF_HEADER   \n");
  return p-p0 ;
}

int str_cmp(const char *s1, const char *s2)
{
  while (*s1 && toupper(*s1) == toupper(*s2)) { s1++; s2++; }
  return *s1 - *s2;
}
int search_str(char *s1, char **s2, int n)
{ int i;
  for (i=0; i < n; i++)
    if (str_cmp(s1, s2[i]) == 0) break;
  return i;
}
char *next_val(char *s)
{ static char *p;
  char *q;
  if (s != NULL) p = s;
  while (isspace(*p)) p++;
  if (*p == 0) return NULL;
  q = p;
  if (*p == ':') p++; else while (isdigit(*p)) p++;
  return q;
}
int get_range(char *str, short *arr, int max)
{ int i, j, k, l, m, n=0, ind=0, val[100] ;
  char *p ;

  p = next_val(str);
  while (p != NULL)
  { if (sscanf(p,"%d",&val[n++]) != 1)
    { fprintf(stderr,"get_range: Illegal range\n"); n--; break; }
    p = next_val(NULL); if (p == NULL) break;
    if (*p == ':') { val[n++] = INT_MIN ; p = next_val(NULL) ; }
  }
  if (val[n-1] == INT_MIN) n--;
  if (n <= 0) return 0;
  val[n] = 0;
  for (i=0, m=0; i < n; i = ind+1)
  { k = 1;
    ind = i ;      /* fixed on Sep 9, 1995 ! */
    if (i+2 < n && val[i+1] == INT_MIN)
    { j = val[ind = i+2];
      if (i+4 < n && val[i+3] == INT_MIN) k = val[ind = i+4];
    }
    else j = val[i];
    if (j >= max) j = max-1;
    for (l=val[i]; l <= j; l += k) arr[m++] = l;
  }
  return m;
}
void copy_text(FILE *f)
{ char *p;
  while (fgets(Bufp, Len, f))
  { p = Bufp;
    while (*Bufp) Bufp++;
    if (*p == end_char) break;
  }
}

void get_antenna(FILE *f, AntennaParType *antenna)
{ char *p;
  int seq, j, id;
  double fixed, bx,by,bz;
  AntennaParType *ant;
  Bufp = Buf;
  for (seq=0; seq < MAX_ANTS; seq++)
  { for (j=0; j < MAX_BANDS; j++)
    { antenna[seq].samp_id[j] = MAX_SAMPS;
      antenna[seq].p0[j] = 0 ;
    }
  }
  seq = 0;
  while (fgets(Bufp, Len, f))
  { p = Bufp;
    while (*Bufp) Bufp++;
    if (*p == cont_char) continue;
    if (*p == end_char) break;
    if (strncmp(p, "ANT", 3) != 0) continue;
    sscanf(p+3,"%d", &id); if (id >= MAX_ANTS) continue;
    ant = &antenna[id];
    sscanf(p+10,"%s %lf %lf %lf %lf", ant->name, &bx, &by, &bz, &fixed);
    ant->bx = bx ;      ant->by = by ;    ant->bz = bz ;
    if (id < seq) fprintf(stderr,"Antenna Sequence changed..\n");
    seq = id+1;
//  printf("the id is %lf \n", ant->bz);
    for (j=0; j < MAX_BANDS; j++) ant->d0[j] = fixed;
  }
}

void get_bands(FILE *f, char **band_name)
{ int i=0, id;
  char *p;
  while (fgets(Bufp, Len, f))
  { p = Bufp;
    while (*Bufp) Bufp++;
    if (*p == cont_char) continue;
    if (*p == end_char) break;
    if (strncmp(p, "BAND", 4) != 0) continue;
    sscanf(p+4,"%d", &id);
    sscanf(p+10,"%s", band_name[id]);
    if (id != i) fprintf(stderr,"Band Sequence changed..\n");
    i++;
  }
}

  
int print_macs(BaseParType *base, int macs)
{ int m ;
  FILE * f = fopen("MacTable","wt") ;
  for (m=0; m<macs; m++)
  {
    fprintf(f, "mac = %d,  card = %d,   chip = %d  col:row=%02d:%02d\n",
            m,base[m].card,base[m].chip,base[m].samp[0].fft_id/2,
              base[m].samp[1].fft_id/2) ;
  }
  return macs ;
}

typedef struct { int col[4], row[4] ; }MacCardType ;
static MacCardType MacCard[MAC_CARDS] =
{
       /*   CORR30 : fft=30 implies no connection   */
       /* mac.0rel  obtained from program retab */


  /* 0.00 hifg:5678 */ {  {16 , 17 , 14 , 15  } , {  4 ,  5 ,  6 ,  7 }  },
  /* 0.01 debc.5678 */ {  {12 , 13 , 10 , 11  } , {  4 ,  5 ,  6 ,  7 }  },
  /* 0.02 789a.5678 */ {  { 6 ,  7 ,  8 ,  9  } , {  4 ,  5 ,  6 ,  7 }  },
  /* 0.03 789a.1234 */ {  { 6 ,  7 ,  8 ,  9  } , {  0 ,  1 ,  2 ,  3 }  },
  /* 0.04 fghi.1234 */ {  {14 , 15 , 16 , 17  } , {  0 ,  1 ,  2 ,  3 }  },
  /* 0.05 debc.1234 */ {  {12 , 13 , 10 , 11  } , {  0 ,  1 ,  2 ,  3 }  },
  /* 0.06 debc.9abc */ {  {12 , 13 , 10 , 11  } , {  8 ,  9 , 10 , 11 }  },
  /* 0.07 hifg.9abc */ {  {16 , 17 , 14 , 15  } , {  8 ,  9 , 10 , 11 }  },
  /* 0.08 ..9a.9a.. */ {  {30 , 30 ,  8 ,  9  } , {  8 ,  9 , 30 , 30 }  },
  /* 0.09 hifg.defg */ {  {16 , 17 , 14 , 15  } , { 12 , 13 , 14 , 15 }  },
  /* 0.10 dehi.dehi */ {  {12 , 13 , 16 , 17  } , { 12 , 13 , 16 , 17 }  },

  /* 1.00 3412.jklm */ {  { 2 ,  3 ,  0 ,  1  } , { 18 , 19 , 20 , 21 }  },
  /* 1.01 7856.jklm */ {  { 6 ,  7 ,  4 ,  5  } , { 18 , 19 , 20 , 21 }  },
  /* 1.02 9abc.jklm */ {  { 8 ,  9 , 10 , 11  } , { 18 , 19 , 20 , 21 }  },
  /* 1.03 9abc.nopq */ {  { 8 ,  9 , 10 , 11  } , { 22 , 23 , 24 , 25 }  },
  /* 1.04 1234.nopq */ {  { 0 ,  1 ,  2 ,  3  } , { 22 , 23 , 24 , 25 }  },
  /* 1.05 7856.nopq */ {  { 6 ,  7 ,  4 ,  5  } , { 22 , 23 , 24 , 25 }  },
  /* 1.06 7856.rstu */ {  { 6 ,  7 ,  4 ,  5  } , { 26 , 27 , 28 , 29 }  },
  /* 1.07 3412.rstu */ {  { 2 ,  3 ,  0 ,  1  } , { 26 , 27 , 28 , 29 }  },
  /* 1.08 bc9a.rstu */ {  {10 , 11 ,  8 ,  9  } , { 26 , 27 , 28 , 29 }  },
  /* 1.09 3412.5612 */ {  { 2 ,  3 ,  0 ,  1  } , {  4 ,  5 ,  0 ,  1 }  },
  /* 1.10 5634.5634 */ {  { 4 ,  5 ,  2 ,  3  } , {  4 ,  5 ,  2 ,  3 }  },

  /* 2.00 turs.hijk */ {  {28 , 29 , 26 , 27  } , { 16 , 17 , 18 , 19 }  },
  /* 2.01 pqno.hijk */ {  {24 , 25 , 22 , 23  } , { 16 , 17 , 18 , 19 }  },
  /* 2.02 jklm.hijk */ {  {18 , 19 , 20 , 21  } , { 16 , 17 , 18 , 19 }  },
  /* 2.03 jklm.defg */ {  {18 , 19 , 20 , 21  } , { 12 , 13 , 14 , 15 }  },
  /* 2.04 rstu.defg */ {  {26 , 27 , 28 , 29  } , { 12 , 13 , 14 , 15 }  },
  /* 2.05 pqno.defg */ {  {24 , 25 , 22 , 23  } , { 12 , 13 , 14 , 15 }  },
  /* 2.06 pqno.lmno */ {  {24 , 25 , 22 , 23  } , { 20 , 21 , 22 , 23 }  },
  /* 2.07 turs.lmno */ {  {28 , 29 , 26 , 27  } , { 20 , 21 , 22 , 23 }  },
  /* 2.08 lm...lm.. */ {  {20 , 21 , 30 , 30  } , { 20 , 21 , 30 , 30 }  },
  /* 2.09 turs.pqrs */ {  {28 , 29 , 26 , 27  } , { 24 , 25 , 26 , 27 }  },
  /* 2.10 pqtu.pqtu */ {  {24 , 25 , 28 , 29  } , { 24 , 25 , 28 , 29 }  }
  /* 
         CORR8 ...
     {  { 0,1,2,3 }, { 0,1,2,3 } },
     {  { 4,5,6,7 }, { 4,5,6,7 } },
     {  { 0,1,2,3 }, { 4,5,6,7 } }
  */
} ;

int get_unique_macs(BaseParType *base, int *baselines, int macmode ) 
{ int macs=0, i,j,k, row,col, card, chip, inc_col,inc_row,pols ;
  int  ind_tab[MAX_BASE] ;

  if (macmode == RR__)pols = 1 ; else pols = 2 ;

  /* get full list of valid macs */
  for (macs=0,card=0; card<MAC_CARDS; card++)
    for (chip=0,i=0; i<4; i++)         /* These two loops signify the freq. */
      for (j=0; j<4; j++,chip++)
      { row=MacCard[card].row[j];
        col=MacCard[card].col[i] ;
        if (row == MAX_ANTS || col == MAX_ANTS)continue ;   /* 98aug21 */
        if(row  < col) ind_tab[macs] = row + 256*col;
                         /*chooses the min less freq*/
        else ind_tab[macs] = col + 256*row ;
        base[macs].card = card ;
        base[macs].chip = chip ;
              /* defn of chip depends on read-out logic in MAC control card*/
        base[macs].samp[0].fft_id = 4*col +2*DasCardId +1;
	base[macs].samp[1].fft_id = 4*row +2*DasCardId +1;
        base[macs].word_incr = pols ;
        base[macs].pol = 0 ;
        macs++ ;
      }

  /* discard redundant baselines and redefine valid macs */
  for (i=0,j=0; i<macs; i++)
  { for (k=0; k < j; k++) if (ind_tab[k] == ind_tab[i]) break;
    if (k == j) { ind_tab[j] = ind_tab[i] ; base[j] = base[i]; j++ ; }
  }
  printf("%d unique macs out of %d\n",j,macs) ;
  macs = j ;
  print_macs(base, macs) ;
  *baselines = pols * macs ;

  if ( pols == 1) return macs ;

  inc_col=-1;
  if (macmode == RRLL) inc_row=-1;
  else inc_row=0; 

  for (k=macs-1; k>0; k--) base[2*k] = base[k] ;

  for (k=0; k < *baselines ; k+=2)
  { base[k+1].card = base[k].card ;
    base[k+1].chip = base[k].chip ;
    base[k+1].pol = 1 ;
    base[k+1].word_incr=base[k].word_incr;
    base[k+1].samp[0]=base[k].samp[0];
    base[k+1].samp[1]=base[k].samp[1];
    base[k+1].samp[0].fft_id = base[k].samp[0].fft_id + inc_col ;
    base[k+1].samp[1].fft_id = base[k].samp[1].fft_id + inc_row ;
  }

  return macs ;
}
  
int get_baselines(CorrType *corr)
{ 
  int j, id, all;
  int fft_row, fft_col ;
  BaseParType *base, *bdest ;

  corr->corrpar.macs = get_unique_macs(corr->baseline, 
                       &all, corr->corrpar.macmode ) ;

  base = (BaseParType *) malloc(all*sizeof(BaseParType)) ;
  memcpy(base, corr->baseline, all*sizeof(BaseParType)) ;

  corr->daspar.baselines = 0;

  for ( id=0; id < all; id++)
  { int row_local=MAX_SAMPS, col_local=MAX_SAMPS ;
    fft_col = base[id].samp[0].fft_id ;
    fft_row = base[id].samp[1].fft_id ;

    for (j=0; j < corr->daspar.samplers; j++)
    { if (corr->sampler[j].fft_id == fft_col)col_local = j ;
      if (corr->sampler[j].fft_id == fft_row)row_local = j ;
    }

    if ( (row_local == MAX_SAMPS) || (col_local == MAX_SAMPS) ) continue ;

    bdest = corr->baseline + corr->daspar.baselines ;
    memcpy(bdest, base+id, sizeof(BaseParType)) ;

    memcpy(&bdest->samp[0],&corr->sampler[col_local], sizeof(SamplerType)) ;
    memcpy(&bdest->samp[1],&corr->sampler[row_local], sizeof(SamplerType)) ;

    corr->daspar.baselines++ ;
  }    
  return corr->daspar.baselines ;

}

int update_sampler(CorrType *corr, int *freesamp)
{ unsigned char s[4],muxval=corr->corrpar.dpcmux;
  int           i,j,k,card,free_dpc[4];
  DasParType   *daspar=&corr->daspar;
 

  for(i=0;i<4;i++){s[i]=muxval&0x3;muxval=muxval>>2;}
  for(i=0;i<4;i++)
  {  for(j=0;j<4;j++)if(i==s[j])break;
     free_dpc[i]= (j==4)? 1:0;
  }

  for(i=0;i<daspar->samplers;i++)
  { SamplerType  *samp=corr->sampler+i;
    if(samp->dpc != MAX_SAMPS) continue;
    card=samp->fft_id/4;
    for(j=0;j<4;j++)
    { if(free_dpc[j])
      { k=card*4+j;
	if(freesamp[k]){ samp->dpc=k; freesamp[k]=0;break;}
      }
    }
    if(samp->dpc==MAX_SAMPS)
    { fprintf(stderr,"Unable to find a free sampler to reassign!\n"); exit(1);}
  }

  return 0;
}


int get_sampler(FILE *f, CorrType *corr)
{  /*  revised: 97july27/crs          */
  int ant_id, seq=0, id, fft_id_list[4], fft_ids, band,i,j;
  unsigned long antmask = 0 ;
  char str1[16],str2[16], *p;
  AntennaParType* const antenna = corr->antenna;
  SamplerType   *samp    = corr->sampler;
  DasParType    *daspar  =&corr->daspar ;
  char *band_name[MAX_BANDS];
  int  freesamp[MAX_SAMPS];
  
/*band_name[0] =  corr->bandname[0];
  band_name[1] =  corr->bandname[1];
  band_name[2] =  corr->bandname[2];
  band_name[3] =  corr->bandname[3];
*/
  daspar->samplers = 0 ;
  
//daspar->bandmask = 0 ;
  for (id=0; id < MAX_SAMPS; id++)
  { 
    samp[id].ant_id = MAX_ANTS;
    samp[id].band = MAX_BANDS ;
    samp[id].fft_id = MAX_FFTS;
 // samp[id].dpc = MAX_SAMPS ;
 // freesamp[id]=1;
  }

  seq = 0 ;
  while ((p = fgets(Bufp, Len, f)) != NULL)
  { Bufp += strlen(Bufp);
    if (*p == cont_char) continue;
    if (*p == end_char) break;

    if (strncmp(p, "SMP", 3) != 0) continue;
    sscanf(p+3,"%d", &id);
    if (id >= MAX_SAMPS)
    { 
      fprintf(stderr,"ignoring BAD sampler %d\n",id) ;
      continue ;
    }
    sscanf(p+10,"%s %s", str1, str2 );
    /* antenna_name, band_name  to which it is connected */
    for (ant_id=0; ant_id < MAX_ANTS; ant_id++)
      if (str_cmp(str1, antenna[ant_id].name) == 0) break ;
      //printf("ant_id value inside get_sampler is %s\n", antenna[ant_id].name);
    if (ant_id == MAX_ANTS ) continue ;
  
    /*  fft_ids = dpc2fft(id,corr->corrpar.dpcmux,fft_id_list) ;
    for(i=0,j=0;i<fft_ids;i++)
    { int fft_id=fft_id_list[i];
      
      if(DasCardId==DAS_CARD0 &&  (fft_id%4)/2) continue; 
      if(DasCardId==DAS_CARD1 && !((fft_id%4)/2)) continue;

      band = search_str(str2, band_name, MAX_BANDS);
      if (band == MAX_BANDS)
      { fprintf(stderr,"IF_CON: Illegal band-name %s ignore\n",str2);
	continue ;
      }
*/
      antmask = antmask | (1 << ant_id) ;

      //    daspar->bandmask |= (1 << band) ;

      samp = corr->sampler + daspar->samplers ;
      samp->ant_id = ant_id;

      /*    samp->fft_id = fft_id;
      samp->band = band ;
      if(j==0){samp->dpc = id;freesamp[id]=MAX_SAMPS;j++;}
      else{samp->dpc=MAX_SAMPS;}
      antenna[ant_id].samp_id[band] = daspar->samplers ; // local ref */
  
      daspar->samplers++;
   // }
  }
  daspar->antmask = antmask ;
    
  /* assign samp_ids from free list to multiplexed data streams 
  samp=corr->sampler; 
  for(i=0,j=0;i<daspar->samplers;i++,samp++)
  { if(samp->dpc==MAX_SAMPS)
    for(k=j;k<MAX_SAMPS;k++)
      if(freesamp[k]!=MAX_SAMPS){ samp->dpc=k;j=k+1;break;}
  }

  */
//  update_sampler(corr,freesamp);

//  get_baselines( corr);

  return 0;
}


void update_syspar( CorrParType *corrpar)
{
  int fft_chan[4] = { 256,128,64,32 } ; 
  corrpar->clock = 512.0 * corrpar->sta * 1e6 /corrpar->statime ; /* SS 6Aug02*/
  if (corrpar->macmode == RR__)corrpar->pols = 1 ; else corrpar->pols = 2 ;
  corrpar->channels = fft_chan[corrpar->fftmode]/corrpar->pols ;
  corrpar->f_step = corrpar->clock/(corrpar->channels * 2<<corrpar->clksel) ;
  SysInit = 1 ;
}
int get_dpcmode(char *str,unsigned char *mode)
{ char s[4];  
  int i,val;

  val=search_str(str, dpc_modes, DpcMuxVals) ;  
  if(val<DpcMuxVals)
  { switch(val) /* translate predefined modes */
    { case arar_arar  : strcpy(str,"dpc_0000"); break;
      case alal_alal  : strcpy(str,"dpc_1111"); break;
      case brbr_brbr  : strcpy(str,"dpc_2222"); break;
      case blbl_blbl  : strcpy(str,"dpc_3333"); break;
      case aral_brbl  : strcpy(str,"dpc_0123"); break;
      case aral_alar  : strcpy(str,"dpc_0110"); break;
      case brbl_blbr  : strcpy(str,"dpc_2332"); break;
      case arbr_albl  : strcpy(str,"dpc_0213"); break;
      case IndianPolar: strcpy(str,"dpc_0123"); break;
      case LsbPolar   : strcpy(str,"dpc_0110"); break;
      case UsbPolar   : strcpy(str,"dpc_3223"); break;
      case LsbCopy    : strcpy(str,"dpc_0101"); break;
      case UsbCopy    : strcpy(str,"dpc_2323"); break;
      case AllL130    : strcpy(str,"dpc_0000"); break;
      case AllL175    : strcpy(str,"dpc_1111"); break;
      case AllU130    : strcpy(str,"dpc_2222"); break;
      case AllU175    : strcpy(str,"dpc_3333"); break;
    }
  }

  *mode=0;
  if(strncmp(str,"dpc_",4)) return -1;
  for(i=0;i<4;i++)
    if((s[i]=str[4+i]-48)>3||s[i]<0) return -1;
  for(i=0;i<4;i++) *mode |= s[i]<<(2*i);
  return 0;
}
int get_dpc_mode_name(unsigned char mode, char *name)
{  int i;
   unsigned char dpc_mux ;
   char  str[1024];

   for(i=0;i<DpcMuxVals;i++)
   { strcpy(str,dpc_modes[i]);
     get_dpcmode(str,&dpc_mux);
     if(mode==dpc_mux){strcpy(name,dpc_modes[i]); return 0;}
   }
   return 1;
}
int get_corrsys (FILE *f, char *version, CorrParType *corrpar)
{ char str[16], *p;
  int n, val, endian=LittleEndian ;
  char *mac_modes[MACMODES] = {"RRLL","RRRL","RR" } ;
  unsigned char mode;

  n = 1 ;
  if (*((unsigned char *)&n) == 1) endian = LittleEndian ;
  else endian = BigEndian ;



  while (fgets(Bufp, Len, f))
  { p = Bufp;
    while (*Bufp) Bufp++;
    if (*p == cont_char) continue;
    if (*p == end_char) break;
    if (sscanf(p,"%s", str) == 0) continue;
    str[8] = 0;
    n = search_str(str, varname, Vars);
    if (n == Vars) continue;
    switch (n)
    { 
      case Clk_sel  : sscanf(p+10,"%d", &val ) ;
                      corrpar->clksel = val ;  break ;

      case Dpc_mux  : sscanf(p+10,"%s", str ) ;
	              if((get_dpcmode(str,&mode))==0) corrpar->dpcmux = mode ;
		      else{ fprintf(stderr,"FATAL:Illegal dpcmode %s\n",str); exit(1);}
                      break ;

      case Mac_mode : sscanf(p+10,"%s", str ) ;
                      val = search_str(str, mac_modes, MACMODES) ;
                      if(val != MACMODES) corrpar->macmode = val ;
                      break ;

      case Fft_mode : sscanf(p+10,"%d", &val ) ;
                      corrpar->fftmode = val ;  break ;

      default       : break ;
    }
  }

  update_syspar( corrpar) ;
  return endian ;
}

void get_selection(FILE *f, CorrType *corr)
{ int n, val ;
  char str[128], *p;

  char *mac_modes[MACMODES] = {"RRLL","RRRL","RR" } ;
  unsigned char mode;

  DasParType *daspar = &corr->daspar ;
  CorrParType *corrpar = &corr->corrpar ;

  while (fgets(Bufp, Len, f))
  { p = Bufp;
    while (*Bufp) Bufp++;
    if (*p == cont_char) continue;
    if (*p == end_char) break;
    if (sscanf(p,"%s", str) == 0) continue;
    str[8] = 0;
    n = search_str(str, varname, Vars);
    if (n == Vars) continue;
    switch (n)
    {
      case Clk_sel  : sscanf(p+10,"%d", &val ) ;
                      corrpar->clksel = val ;  break ;

      case Dpc_mux  : sscanf(p+10,"%s", str ) ;
	              if((get_dpcmode(str,&mode))==0) corrpar->dpcmux = mode ;
		      else{ fprintf(stderr,"FATAL:Illegal dpcmode %s\n",str); exit(1);}
                      break ;

      case Mac_mode : sscanf(p+10,"%s", str ) ;
                      val = search_str(str, mac_modes, MACMODES) ;
                      if(val != MACMODES) corrpar->macmode = val ;
                      break ;

      case Fft_mode : sscanf(p+10,"%d", &val ) ;
                      corrpar->fftmode = val ;  break ;
      case Lta      : sscanf(p+10,"%d", &daspar->lta  ); continue ;
      case Chan_num : daspar->channels = get_range(p+10, daspar->chan_num,MAX_CHANS);
		      continue ;
      case Mode     : sscanf(p+10,"%hx",  &daspar->mode    ); break;

      default      : break ;
    }
  }
  update_syspar( corrpar) ;

}

void get_pre_selection(FILE *f, CorrType *corr)
{ int n, val ;
  char str[128], *p;

  char *mac_modes[MACMODES] = {"RRLL","RRRL","RR" } ;
  unsigned char mode;

  CorrParType *corrpar = &corr->corrpar ;

  corrpar->dpcmux  = IndianPolar ;
  corrpar->macmode = RRLL ;
  corrpar->fftmode = 0 ;
  corrpar->channels= MAX_CHANS/2;
  corrpar->clksel  = 0;

  while (fgets(Bufp, Len, f))
  { p = Bufp;
    while (*Bufp) Bufp++;
    if (*p == cont_char) continue;
    if (*p == end_char) break;
    if (sscanf(p,"%s", str) == 0) continue;
    str[8] = 0;
    n = search_str(str, varname, Vars);
    if (n == Vars) continue;
    switch (n)
    {
      case Dpc_mux  : sscanf(p+10,"%s", str ) ;
	              if((get_dpcmode(str,&mode))==0) corrpar->dpcmux = mode ;
		      else{ fprintf(stderr,"FATAL:Illegal dpcmode %s\n",str); exit(1);}
                      break ;

      case Mac_mode : sscanf(p+10,"%s", str ) ;
                      val = search_str(str, mac_modes, MACMODES) ;
                      if(val != MACMODES) corrpar->macmode = val ;
                      break ;

      case Fft_mode : sscanf(p+10,"%d", &val ) ;
                      corrpar->fftmode = val ;  break ;
      case Clk_sel  : sscanf(p+10,"%d", &val ) ;
                      corrpar->clksel = val ;  break ;
    }
  }
}

int corr_hdr(char *filename, CorrType *corr)
{ char *p, str[32];
  FILE *f;
  int i ;
  char *band_name[MAX_BANDS] =
       { "USB-130", "USB-175", "LSB-130", "LSB-175" } ;

  if((p=getenv("DAS_CARD_ID"))==NULL) /* which PC are we running on (corracqa, corracqb or corrctl)? */
  { fprintf(stderr,"FATAL: DAS_CARD_ID EnvVar not defined\n"); exit(1);}
  else
  { DasCardId=atoi(p);
    if(DasCardId<0||DasCardId>DAS_CARDS)
    { fprintf(stderr,"Illegal value (%s) for DAS_CARD_ID. (Legal values are 0-2 (inclusive)",p);
      exit(1);
    }
  }
  strncpy(corr->version, LtaFileVersion,NAMELEN);corr->version[NAMELEN-1]='\0';
  switch(DasCardId)
  { case 0: sprintf(corr->version+24,"HST00 "); break;
    case 1: sprintf(corr->version+24,"HST01 "); break;
   default: sprintf(corr->version+24,"HST?? "); break;
  }
       
  for (i=0; i<MAX_BANDS; i++) strcpy(corr->bandname[i],band_name[i]) ;

  Bufp = Buf ;
  f = fopen(filename, "rt");
  if (f == NULL) return -1;
  while (fgets(Bufp, Len, f))
  { if (*Bufp == cont_char) continue;
    while (isspace(*Bufp)) Bufp++;
    if (*Bufp == 0) continue;
    p = Bufp; Bufp += strlen(Bufp);
    if (*p == begin_char)
    { p++; sscanf(p,"%s",str);
      if (str_cmp(str,"Antenna.def") == 0)
        get_antenna(f, corr->antenna);
   /*
      else if (str_cmp(str,"Bandnames.def") == 0)
          get_bands(f, band_name) ;
   */
      else if (str_cmp(str,"Corrsys.def")==0)
      { int endian ;
        endian = get_corrsys(f, corr->version, &corr->corrpar) ;
        corr->endian = endian & 255 ;
      }
      else if (str_cmp(str,"Sampler.def") == 0)
      {
        get_sampler(f, corr);
      }

      else if (str_cmp(str,"Corrsel.def") == 0)
        get_selection(f, corr);
      else copy_text(f);
    }
    else if (strncmp(p,"END_OF",6) == 0) break;
  }
  fclose(f);
  return 0;
}

int corr_pre(char *filename, CorrType *corr)
{ char *p, str[32];
  FILE *f;
  Bufp = Buf;
  int n,endian;

  f = fopen(filename, "rt");
  if (f == NULL) return -1;

  n = 1 ;  
  if (*((unsigned char *)&n) == 1) endian = LittleEndian ;
  else endian = BigEndian ;
  corr->endian=endian; /* may also set by get_corrsys -- no harm if it is done twice */

  while (fgets(Bufp, Len, f))
  { if (*Bufp == cont_char) continue;
    while (isspace(*Bufp)) Bufp++;
    if (*Bufp == 0) continue;
    p = Bufp; Bufp += strlen(Bufp);
    if (*p == begin_char)
    { p++; sscanf(p,"%s",str);
      if (str_cmp(str,"Corrsel.def") == 0)
      { get_pre_selection (f, corr); }
      else copy_text(f);
    }
    else if (strncmp(p,"END_OF",6) == 0) break;
  }
  fclose(f);
  return 0;
}

int corr_rev(char *filename, CorrType *corr)
{ char *p, str[32];
  FILE *f;
  Bufp = Buf ;
  f = fopen(filename, "rt");
  if (f == NULL) return -1;
  while (fgets(Bufp, Len, f))
  { if (*Bufp == cont_char) continue;
    while (isspace(*Bufp)) Bufp++;
    if (*Bufp == 0) continue;
    p = Bufp; Bufp += strlen(Bufp);
    if (*p == begin_char)
    { p++; sscanf(p,"%s",str);
      if (str_cmp(str,"Antenna.def") == 0)
        get_antenna(f, corr->antenna);
      else if ( !SysInit && (str_cmp(str,"Corrsys.def")==0) )
        get_corrsys(f, corr->version, &corr->corrpar) ;
      else if (str_cmp(str,"Sampler.def") == 0)
      { get_sampler(f, corr);
      }
      else if (str_cmp(str,"Corrsel.def") == 0)
      {
        get_selection (f, corr);
        get_baselines( corr);
      }
      else copy_text(f);
    }
    else if (strncmp(p,"END_OF",6) == 0) break;
  }
  fclose(f);
  return 0;
}


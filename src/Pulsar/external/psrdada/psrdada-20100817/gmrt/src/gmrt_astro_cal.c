# include <stdio.h>
# include <math.h>
# include <time.h>
# include <string.h>

# include "gmrt_newcorr.h"

# define NCO_TICK 512 /* SS 06/Aug/02 -- unused */

int Nco_PI = (1<<29),Nco2Pi=(1<<30) ;  /* for 30-bit NCO, 2*pi => 2^30 */
static int DelayBias = 0x4000 ;  /* for SSB [SS 06/Aug/02] */
static double DelayStep, NcoTickTime,FstcScale ;
double StaTime;


double mjd_cal(struct tm *t)  /* gets mjd at 0h IST */
{ long year,month,day;
  double mjd ;
  year = t->tm_year;
  if(year < 1900) /* Y2K (!) */
    {if (year < 70)year += 2000; else year +=1900;}
  month= t->tm_mon + 1 ;   /* crs/98aug28  */
  day  = t->tm_mday;
  mjd  = (double) ((year - (12 - month) / 10 + 4712L) * 1461L / 4 + (
    (month + 9) % 12 * 306L + 5) / 10 - (year - (12 - month) / 10 +
          4900L) / 100 * 3 / 4 + day - 2399904L);
  //mjd += (t->tm_hour + t->tm_min/60.0 + t->tm_sec/3600.0)/24.0 ;
  mjd -= 5.5/24.0 ;  /* to correspond to 0h IST on the given date */
                     /*    crs/98sep7  */
  return mjd ;
}

double lmst(double mjd)
/* gets local mean sidereal time for time_of_day given by mjd  */
{ double ut,tu,res,lmstime ;
  ut = mjd ;
  tu = (ut - 51544.5) / 36525.;  /* centuries since J2000  */
  res = ut + 74.05/360. ; res = res - floor(res) ;
  lmstime = res + (((0.093104 - tu * 6.2e-6) * tu
                  + 8640184.812866) * tu + 24110.54841)/86400.0 ;
  lmstime = (lmstime - floor(lmstime))*2.0 * M_PI ;
  return lmstime ;
}

int initModel(CorrType *corr, ModelInfoType *model,ScanInfoType *scan)
{ int i,cards=0 ;
  int stream[MAX_FFTS] ;
  float p2fstc[MAX_FFTS] ;
  DasParType *daspar = &corr->daspar ;
  CorrParType *corrpar = &corr->corrpar ;
  SamplerType *samp = corr->sampler ;
  SourceParType *source=&scan->source;
  AntennaParType *ant ;
  FftDelayParType *fdpar ;



  /* work out if frequency increases or decreases with channel number */
  for(i=0;i<2;i++)
  if(source->freq[i] > source->first_lo[i])  
    {source->net_sign[i]=+1;source->net_sign[i+2]=-1;}
  else {source->net_sign[i]=-1;source->net_sign[i+2]=+1;}


  model->dpcmux = corrpar->dpcmux ;
  model->clksel = corrpar->clksel ;
  model->fftmode = corrpar->fftmode ;
  model->macmode = corrpar->macmode ;
  model->clock = corrpar->clock ;   /* in Hz */
  DelayStep = (double)(1<<corrpar->clksel)/model->clock ; /* sec */
  FstcScale = 16 ;  /* 2 clock cycles correspond to 32 in the FSTC RAM */
  NcoTickTime = (corr->corrpar.statime/1e6)/corr->corrpar.sta ;
  model->delay_step = DelayStep ;
  model->nco_tick_time = NcoTickTime ;
  model->fstc_scale = FstcScale ;
  for (i=0; i<MAX_FFTS; i++)  /* set defaults to impossible values */
  { stream[i] = MAX_SAMPS ;
    p2fstc[i] = 0 ;
  }

  for (i=0; i<daspar->samplers; i++)   /* update for the relevant samplers */
  { int band;
    samp = corr->sampler + i ;
    ant = corr->antenna + samp->ant_id ;
    band = samp->band;
    if(samp->fft_id != MAX_FFTS)
    { stream[samp->fft_id] = i ;
      p2fstc[samp->fft_id] = -corr->corrpar.f_step*StaTime*
	                     2*FstcScale*
	                     128/(2*M_PI*source->freq[band%2]) ;
            /* The formula may be wrong sirothia 13oct, 
	       Replace 128 by FFT_LEN/4 */
    }
  }
   /* 
      Arrange model parameters sorted in the order of FFT/delay card num.
      If one pipeline in a card is valid and the other valid,
      set both of them to point to the parameters revelant to
      the valid pipeline
   */

  for (cards=0,i=1; i<MAX_FFTS; i+=2)    /* get number of cards */
  { int s0,s1 ;
    if((stream[i] == MAX_SAMPS) && (stream[i-1] == MAX_SAMPS))continue ;
    fdpar = model->fdpar + cards ;
    fdpar->card = i/2 ;
    s0 = stream[i-1] ; s1 = stream[i] ;

      /* At least one stream must have been defined; otherwise,
         this card would not have found a place here
         When both exist, we prefer the odd stream for p2fstc.
      */
    if (s1 != MAX_SAMPS){fdpar->p2fstc = p2fstc[i];}
    else {fdpar->p2fstc = p2fstc[i-1] ;}
    fdpar->s0 = s0 ;  fdpar->s1 = s1 ;
    cards++ ;
  }
  model->cards = cards ;

  return cards ;
}

int updateModel(DataParType *datapar, ModelInfoType *model)
{ int i, card ;
  float fstc,delay ;
  int ndelay ;
  DataParType *par ;
  ModelParType *mpar ;
  FftDelayParType *fdpar ;
  static int PositiveRates=0 ;
      /*  We retain fringe rates as if the NCO range was +-2^30 ;
          this checks with the present version of corr_config;
          Both routines need to be revised simultaneously for a
          different convention.
                              
                      --crs/98sep16
      */
  /* FILE *fp=fopen("UpdateModelPar.out","w"); */

  for (i=0; i < model->cards ; i++)
  { 
    fdpar = model->fdpar + i ;
    card = fdpar->card ;

/* First get the settable delay and FSTC from theoretical estimate */

    if (fdpar->s1 != MAX_SAMPS) mpar = model->par +  fdpar->s1 ;
    else mpar = model->par + fdpar->s0 ;

    ndelay  = (int) (DelayBias - mpar->delay/DelayStep + 0.5) ;

    /*
       The theoretical delay (after adding fixed delays) differences
       embarrassingly turn out to potentially exceed the maximum
       settable delay in some cases.  The settable delay is restricted
       to a few clock cycles less than the hardware limit to prevent
       bus contention in dual port RAM when the read and write addresses
       are identical.  Pehaps we should disallow only the exact identity,
       but I have restricted a further 2 words (4 clock cycles equiv).
       When there is overlap between FFT blocks in the case of
       low bandwidth applications (clksel > 0), the data are moved
       out to FFT cards much faster than they arrive, so there is
       a need for allowing at least half a block (equiv 256 clock ticks)
       in the DPRAM before we start flushing things out.  Since we
       are likely to be restricted by hardware for maximum range of
       delay for the 16 MHz bandwidth (not for others), this offset
       is provided only when clksel != 0.
       The following code is written in a hurry and needs to be
       verified later based on the above criteria.  Also I have
       hardcoded for the 2k x 8-bit DPRAMs that have been used
       in the delay cards of our first generation correlator.
        
    */
       
    if ( ndelay < 2 ) ndelay = 2 ;  
    if (ndelay > 2*DelayBias-2)ndelay = 2*DelayBias-2 ;

    delay =  (DelayBias-ndelay)*DelayStep ; /* settable delay in sec */
    fstc = (delay - mpar->delay)/DelayStep ; /* rad/chan */
    /* if (fstc < 0)      bring it in the range 0 to 2  
    { delay += DelayStep ;
      ndelay -= 1 ;
      fstc += 1 ;
    } */
    if(fabs(fstc) > 1.0)fprintf(stderr,"\a Delay out of range on Card %d\n",card);
    fdpar->delay = ndelay ;  /* who moves lsb out ??  */
    fdpar->fstc = (int) (16 - fstc*FstcScale + 0.5) ;
    if (fdpar->fstc < 0)fdpar->fstc = 0 ;
    if (fdpar->fstc >= 2*FstcScale)fdpar->fstc = 2*FstcScale-1 ;
    fdpar->fstc_step = mpar->dp * fdpar->p2fstc  ;

    if(fdpar->fstc_step < -1.0)fdpar->fstc_step=-1.0; 
    if(fdpar->fstc_step > 1.0)fdpar->fstc_step=1.0; 
            /* Not debugged properly sirothia 13oct*/
    /*
       Now convert the fringe phase and rate into Nco parameters
       The following lines may need revision later /crs 98sep6
    */
    
    if (fdpar->s0 != MAX_SAMPS)
    { mpar = model->par + fdpar->s0 ;
      par = datapar + fdpar->s0 ;
      fdpar->p0 = (int) ((double)mpar->phase * Nco_PI/M_PI) ;
      if (fdpar->p0 < 0)fdpar->p0 += 2*Nco_PI ;  /* positive angle */
      fdpar->pstep0 = (int) ((double)mpar->dp * NcoTickTime*Nco_PI/M_PI) ;
      if(PositiveRates)
           if (fdpar->pstep0 < 0)fdpar->pstep0 += (2*Nco_PI) ;
  /*  Now make par compatible with our convention for record etc.  */
      par->phase = fdpar->p0 * M_PI/Nco_PI ;
      par->dp = fdpar->pstep0 *M_PI/(NcoTickTime*Nco_PI) ;
      par->delay = delay ;
      par->dslope = (fdpar->fstc/FstcScale)*DelayStep ;
    }
    else   { fdpar->p0 = 0 ; fdpar->pstep0 = 0 ; }

    if (fdpar->s1 != MAX_SAMPS)
    { mpar = model->par + fdpar->s1 ;
      par = datapar + fdpar->s1 ;
      fdpar->p1 = (int) ((double)mpar->phase * Nco_PI/M_PI) ;
      if (fdpar->p1 < 0)fdpar->p1 += 2*Nco_PI ;  /* positive angle */
      fdpar->pstep1 = (int) ((double)mpar->dp * NcoTickTime*Nco_PI/M_PI) ;
      if (PositiveRates)
            if (fdpar->pstep1 < 0)fdpar->pstep1 += (2*Nco_PI) ;

  /*  Now make par compatible with our convention for record etc.  */
      par->phase = fdpar->p1 * M_PI/Nco_PI ;
      par->dp = fdpar->pstep1 *M_PI/(NcoTickTime*Nco_PI) ;
      par->delay = delay ;
      par->dslope = (fdpar->fstc/FstcScale)*DelayStep ;
    }
    else   { fdpar->p1 = 0 ; fdpar->pstep1 = 0 ; }
  }

  /*
  for (i=0; i < model->cards ; i++)
  { fdpar = model->fdpar + i ;
    fprintf(fp,"FFT Card %d Stream 0 using model %d\n",fdpar->card,fdpar->s0);
    fprintf(fp,"FFT Card %d Stream 1 using model %d\n\n",fdpar->card,fdpar->s1);
  }
  fclose(fp);
  */
  return 0 ;
}

int useModel(double tm, int npar, CorrType *corr, ModelParType *par,
             ExtModelType *ext, AntennaFlagType *flag)
{ int i,k, count=0 ;
  for (k=0; k<npar; k++)
  { int ant_id = ext[k].ant_id, band = ext[k].band ;
    ModelParType *pext = &ext[k].par ;
    double tdif = tm - ext[k].t0 ;
    i = corr->antenna[ant_id].samp_id[band] ;

    if (i == MAX_SAMPS)continue ;
    if ((flag[band].ext_model & (1<<ant_id)) == 0)continue ;

    memcpy(par+i,pext, sizeof(ModelParType)) ;
    par[i].phase += tdif*pext->dp ;
    par[i].delay += tdif*pext->dd ;
    count++ ;
  }
  return count ;
}

void calModelPar(double tm, CorrType *corr, ModelParType *mpar,
                 SourceParType *source, int *antmask)
{ double w, wdot, ha,dec, ch,sh, cd,sd, lst, p;
  int samp_id ;
  double bxcd,bycd,bzsd, freq,dt ; 
  double instr_delay, instr_phase ;
  ModelParType *par ;
  /* FILE *fp=fopen("CalModelPar.out","w"); */

  for (samp_id=0; samp_id < corr->daspar.samplers ; samp_id++)
  { static double C = 2.99796e8 ; /* velocity of light in  m/s */
    int ant_id=corr->sampler[samp_id].ant_id;
    int band=corr->sampler[samp_id].band;
    AntennaParType *ant = &corr->antenna[ant_id] ;
    int mask = 1 << ant_id ;

    if ( (antmask[0] & mask) == 0)continue ;
    //dt = tm + (corr->daspar.mjd_ref - source->mjd0)*86400 ;
    

    lst = lmst(corr->daspar.mjd_ref + tm/86400);
    
// =================================================

  double ut,tu,res,lmstime ;
  ut = corr->daspar.mjd_ref + tm/86400 ;
  tu = (ut - 51544.5) / 36525.;  /* centuries since J2000  */
  res = ut + 74.05/360. ; res = res - floor(res) ;
  lmstime = res + (((0.093104 - tu * 6.2e-6) * tu
                  + 8640184.812866) * tu + 24110.54841)/86400.0 ;
  lmstime = (lmstime - floor(lmstime))*2.0 * M_PI ;
  lst = lmstime; 
  
// =================================================

    //ha = lst - source->ra_app - dt * source->dra  ;
    ha = lst - source->ra_app;
    ch = cos(ha); sh = sin(ha) ;
    //dec = source->dec_app + dt * source->ddec ;
    dec = source->dec_app;
    cd = cos(dec) ;  sd = sin(dec) ;
    bxcd = ant->bx *cd/C ; /* sec */ 
    bycd = ant->by *cd/C ;
    bzsd = ant->bz *sd/C ;
    //freq = source->freq[band%2]*source->net_sign[band];
    freq = source->freq[0];
    instr_delay = ant->d0[0]/C ;
//  printf("lst is value inside calModelpar %lf \n", lst);
          
    //instr_phase = ant->p0[band] ;
    //instr_phase = ant->p0[0] ;
  
    /*
      w  is the net delay (sec) suffered by the signal in the sky;
      nanosec is the net delay (in nanosec) including cable delays.
      wdot is the rate of change of phase (radians/sec) due to
      change in geometry, which is communicated to the Corrnode
      in units of cycles/StaTime .
      PhaseNext is the net phase (fractional cycle times 1024) 
      corresponding to the hour angle ha
      The correction to be applied to each of these three terms
      is the negative of the corresponding terms such that
      value + correction = 0 as closely as possible
    */
  
  
    w = ch*bxcd - sh*bycd + bzsd ;  /* sec */
    par = mpar + samp_id;
    /*    fprintf(fp,"Mpar%04d [Samp %04d Fft %04d] ) set to %s %d\n",samp_id,corr->sampler[samp_id].dpc, corr->sampler[samp_id].fft_id, ant->name,band); */
    par->delay = w + instr_delay ;
    wdot = par->dd = (-7.27e-5)*( sh*bxcd  + ch*bycd ) ;
     
    par->dp = -2*M_PI *  wdot * freq ; /* radians per sec */

    if (source->mode & TransitObs) p = -par->dp * lst * 0.9972695663 ;
    /* phase varies linearly, from 0 at lst = 0  */
    else p = w * freq + instr_phase ;
   
    /*Added this line to get the instrumental phase*/
    //par->phase = instr_phase ; 

    par->phase = 2*M_PI * (1 - fmod(p,1.0)) ;  /* radians to be added */
 }
  /*  fclose(fp); */
}

 

#ifndef _prepfold_h
#define _prepfold_h

/* Presto "position" struct, from include/presto.h */
struct position {
  float pow;
  double p1;
  double p2;
  double p3;
};

/* Presto folstats struct from include/presto.h */
struct foldstats {
  double numdata;     /* Number of data bins folded         */
  double data_avg;    /* Average level of the data bins     */
  double data_var;    /* Variance of the data bins          */
  double numprof;     /* Number of bins in the profile      */
  double prof_avg;    /* Average level of the profile bins  */
  double prof_var;    /* Variance of the profile bins       */
  double redchi;      /* Reduced chi-squared of the profile */
};

/* Slightly edited version of Scott's prepfold structure.  Mostly
 * corresponds to what is stored on disk.  Original is
 * in Presto's include/prepfold.h
 */
struct prepfoldinfo {
  double *rawfolds;   /* Raw folds (nsub * npart * proflen points) */
  double *dms;        /* DMs used in the trials */
  double *periods;    /* Periods used in the trials */
  double *pdots;      /* P-dots used in the trials */
  foldstats *stats;   /* Statistics for the raw folds */
  int numdms;         /* Number of 'dms' */
  int numperiods;     /* Number of 'periods' */
  int numpdots;       /* Number of 'pdots' */
  int nsub;           /* Number of frequency subbands folded */
  int npart;          /* Number of folds in time over integration */
  int proflen;        /* Number of bins per profile */
  int numchan;        /* Number of channels for radio data */
  int pstep;          /* Minimum period stepsize in profile phase bins */
  int pdstep;         /* Minimum p-dot stepsize in profile phase bins */
  int dmstep;         /* Minimum DM stepsize in profile phase bins */
  int ndmfact;        /* 2*ndmfact*proflen+1 DMs to search */
  int npfact;         /* 2*npfact*proflen+1 periods and p-dots to search */
  char *filenm;       /* Filename of the folded data */
  char *candnm;       /* String describing the candidate */
  char *telescope;    /* Telescope where observation took place */
  char *pgdev;        /* PGPLOT device to use */
  char rastr[16];     /* J2000 RA  string in format hh:mm:ss.ssss */
  char decstr[16];    /* J2000 DEC string in format dd:mm:ss.ssss */
  double dt;          /* Sampling interval of the data */
  double startT;      /* Fraction of observation file to start folding */
  double endT;        /* Fraction of observation file to stop folding */
  double tepoch;      /* Topocentric eopch of data in MJD */
  double bepoch;      /* Barycentric eopch of data in MJD */
  double avgvoverc;   /* Average topocentric velocity */
  double lofreq;      /* Center of low frequency radio channel */
  double chan_wid;    /* Width of each radio channel in MHz */
  double bestdm;      /* Best DM */
  position topo;      /* Best topocentric p, pd, and pdd */
  position bary;      /* Best barycentric p, pd, and pdd */
  position fold;      /* f, fd, and fdd used to fold the initial data */
  //orbitparams orb;    /* Barycentric orbital parameters used in folds */
};
#endif

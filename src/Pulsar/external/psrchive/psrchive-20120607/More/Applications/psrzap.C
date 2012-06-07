/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <cpgplot.h>

#include <iostream>
#include <fstream>

#include "Pulsar/psrchive.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"

#include "Pulsar/Plot.h"
#include "Pulsar/ProfilePlot.h"
#include "Pulsar/DynamicBaselineSpectrumPlot.h"
#include "Pulsar/PhaseVsFrequency.h"
#include "Pulsar/PhaseVsTime.h"
#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/IntegrationOrder.h"

#include "Pulsar/RemoveVariableBaseline.h"

#include "strutil.h"

using namespace std;
using namespace Pulsar;

enum cursor_type {
  freq_cursor,
  time_cursor,
  both_cursor
};

struct zap_range {
  double freq0;
  double freq1;
  int sub0;
  int sub1;
  enum cursor_type type;
};

void swap_zap_range(struct zap_range *z) {
  double tmp;
  int itmp;
  if (z->freq0 > z->freq1) { tmp=z->freq0; z->freq0=z->freq1; z->freq1=tmp; }
  if (z->sub0 > z->sub1) { itmp=z->sub0; z->sub0=z->sub1; z->sub1=itmp; }
}

float sub2time(Archive *arch,unsigned sub)
{
  unsigned rows = arch->get_nsubint();
  float t;
  double range = (arch->end_time() - arch->start_time()).in_days();

  const float mjd_hours = 1.0 / 24.0;
  const float mjd_minutes = mjd_hours / 60.0;
  const float mjd_seconds = mjd_minutes / 60.0;

  if (range>1.0)
    t=range*(float) sub/(float) rows;
  else if (range>mjd_hours)
    t=(range/mjd_hours)*(float) sub/(float) rows;
  else if (range>mjd_minutes)
    t=(range/mjd_minutes)*(float) sub/(float) rows;
  else
    t=(range/mjd_seconds)*(float) sub/(float) rows;

  return t;
}

int freq2chan(Archive *arch, double freq, unsigned sub=0) {
  int nchan = arch->get_nchan();
  double bw = arch->get_bandwidth();
  double cbw = fabs(bw) / (double)nchan;
  for (int ichan=0; ichan<nchan; ichan++) {
    double cfreq = arch->get_Integration(sub)->get_centre_frequency(ichan);
    if ((freq > (cfreq-cbw/2.0)) && (freq < (cfreq+cbw/2.0))) 
      return(ichan);
  }
  double cfreq0 = arch->get_Integration(sub)->get_centre_frequency(0);
  double cfreq1 = arch->get_Integration(sub)->get_centre_frequency(nchan-1);
  bool swap=false;
  if (cfreq1 < cfreq0) { 
    double tmp=cfreq0; 
    cfreq0=cfreq1; 
    cfreq1=tmp; 
    swap=true;
  }
  if (freq < cfreq0-cbw/2.0) { return(swap ? 0 : nchan-1); }
  if (freq > cfreq1+cbw/2.0) { return(swap ? nchan-1 : 0); }
  return(-1);
}

void apply_zap_range(Archive *arch, struct zap_range *z,
    bool undo=false, const Archive *orig=NULL) {
  swap_zap_range(z);
  int nsub = arch->get_nsubint();
  int chan0 = freq2chan(arch, z->freq0);
  int chan1 = freq2chan(arch, z->freq1);
  if (chan1<chan0) { int tmp=chan0; chan0=chan1; chan1=tmp; }
  if (z->sub0 < 0) z->sub0 = 0;
  if (z->sub1 >= nsub) z->sub1 = nsub-1;
  for (int isub=z->sub0; isub<=z->sub1; isub++) {
    Reference::To<Integration> subint = arch->get_Integration(isub);
    for (int ichan=chan0; ichan<=chan1; ichan++) {
      if (undo && orig!=NULL)
        subint->set_weight(ichan, 
            orig->get_Integration(isub)->get_weight(ichan));
      else
        subint->set_weight(ichan,0.0);
    }
  }
}

void zap_freq_range(Archive *arch, struct zap_range *z) {
  z->sub0 = 0;
  z->sub1 = arch->get_nsubint()-1;
  apply_zap_range(arch, z);
}

void zap_subint_range(Archive *arch, struct zap_range *z) {
  swap_zap_range(z);
  int nsub = arch->get_nsubint();
  int nchan = arch->get_nchan();
  z->freq0 = arch->get_Integration(0)->get_centre_frequency(0);
  z->freq1 = arch->get_Integration(0)->get_centre_frequency(nchan-1);
  if (z->sub0 < 0) z->sub0 = 0;
  if (z->sub1 >= nsub) z->sub1 = nsub-1;
  for (int isub=z->sub0; isub<=z->sub1; isub++) 
    arch->get_Integration(isub)->uniform_weight(0.0);
}

void apply_zap(Archive *arch, struct zap_range *z) {
  if (z->type==freq_cursor) 
    zap_freq_range(arch, z);
  else if (z->type==time_cursor) 
    zap_subint_range(arch, z);
  else if (z->type==both_cursor) 
    apply_zap_range(arch, z);
}

#define PROG "psrzap"

/* Extension for output file */
string output_ext = "zap";

void usage() {
  cout << "psrzap - Interactive RFI zapper using off-pulse dynamic spectra" 
    << endl
    << "Usage:  psrzap (filename)" << endl
    << "Command line options:" << endl
    << "  -h     Show this help screen" << endl
    << "  -v     Verbose mode" << endl
    << "  -E exp Mathematical expression to evaluate for each pixel" << endl
    << "  -e ext Extension for output file (default: " << output_ext 
      << ")" << endl
    << endl
    << "  Press 'h' during program for interactive usage info:" << endl
    << endl;
}

/* Interactive commands */
#define CMD_QUIT 'q'
#define CMD_HELP 'h'
#define CMD_SAVE 's'
#define CMD_PRINT 'P'
#define CMD_FREQMODE 'f'
#define CMD_TIMEMODE 't'
#define CMD_BOTHMODE 'b'
#define CMD_POL 'p'
#define CMD_VAR 'v'
#define CMD_LOG 'l'
#define CMD_UNDO 'u'
#define CMD_UNZOOM 'r'
#define CMD_UPDATE 'd'
#define CMD_PSRSH 'w'
#define CMD_BASELINE 'B'
#define CMD_METHOD 'm'
void usage_interactive() {
  cout << "pzrzap interactive commands" << endl
       << endl
       << "Mouse:" << endl
       << "  Left-click selects the start of a range" << endl
       << "    then left-click again to zoom, or right-click to zap." << endl
       << "  Right-click zaps current cursor location." << endl
       << "  Middle-click (or 'd') updates the diagnostic plots." << endl
    << "Keyboard:" << endl
    << "  " << CMD_HELP     << "  Show this help" << endl
    << "  " << CMD_FREQMODE << "  Use frequency select mode" << endl
    << "  " << CMD_TIMEMODE << "  Use time select mode" << endl
    << "  " << CMD_BOTHMODE << "  Use time/freq select mode" << endl
    << "  " << CMD_UPDATE   << "  Update diagnostic plots" << endl
    << "  " << CMD_VAR      << "  Switch plot between variance and mean" << endl
    << "  " << CMD_LOG      << "  Toggle logscale" << endl
    << "  " << CMD_METHOD   << "  Toggle method" << endl
    << "  " << CMD_POL      << "  Switch to next polarization" << endl
    << "  " << CMD_BASELINE << "  Toggle variable baseline removal" << endl
    << "  " << CMD_UNDO     << "  Undo last zap command" << endl 
    << "  " << CMD_UNZOOM   << "  Reset zoom and update dynamic spectrum" << endl
    << "  " << CMD_SAVE     << "  Save zapped version as (filename)." 
     << output_ext << endl
    << "  " << CMD_PRINT    << "  Print equivalent paz command" << endl
    << "  " << CMD_PSRSH    << "  Generate PSRSH script" << endl
    << "  " << CMD_QUIT     << "  Exit program" << endl
    << endl;
}

int main(int argc, char *argv[]) {

  /* Process any args */
  int opt=0;
  int verb=0;

  string expression;

  while ((opt=getopt(argc,argv,"hve:E:"))!=-1) {
    switch (opt) {

      case 'v':
        verb++;
        Archive::set_verbosity(verb);
        break;
        
      case 'e':
        output_ext.assign(optarg);
        break;

      case 'E':
	expression = optarg;
	break;

      case 'h':
      default:
        usage();
        usage_interactive();
        exit(0);
        break;

    }
  }

  if (optind==argc) {
    usage();
    cerr << PROG ": No filename given" << endl;
    exit(-1);
  }

  /* Load file */
  string filename = argv[optind];
  Reference::To<Archive> orig_arch = Archive::load(filename);
  Reference::To<Archive> arch = orig_arch->clone();
  arch->dedisperse();
  arch->remove_baseline();
  double bw = arch->get_bandwidth();
  string output_filename = replace_extension(filename, output_ext);
  string psrsh_filename = replace_extension(filename,"psh");

  // Create profile plots
  Reference::To<Archive> tot_arch=NULL,pf_arch=NULL,pt_arch=NULL;
  ProfilePlot *totplot=NULL;
  PhaseVsFrequency *pfplot=NULL;
  PhaseVsTime *ptplot=NULL;
  int totplot_id;

  totplot = new ProfilePlot;
  pfplot = new PhaseVsFrequency;
  ptplot= new PhaseVsTime;

  // Create window and subdivide
  totplot_id = cpgopen("/xs");
  cpgpap(0.0,1.5);
  cpgsubp(1,3);

  // Create Dynamic Spectrum Plot
  DynamicBaselineSpectrumPlot *dsplot = new DynamicBaselineSpectrumPlot;
  if (!expression.empty())
    dsplot->configure("exp="+expression);
  else
    dsplot->configure("var=1");

  dsplot->set_reuse_baseline();
  int dsplot_id = cpgopen("/xs");
  if (dsplot_id<=0) {
    cerr << PROG ": PGPLOT xwindows device open failed, exiting." << endl;
    exit(1);
  }
  cpgask(0);

  /* Input loop */
  char ch='\0';
  enum cursor_type curs=both_cursor;
  float x0=0.0, y0=0.0, x1, y1;
  int click=0, mode=0;
  int pol=0,izap=0;
  bool redraw=true, var=true, log=false, resum=true,remove_baseline=false,method=false;
  struct zap_range zap;
  vector<struct zap_range> zap_list;
  do {
    /* Redraw the plot if necessary */
    if (redraw) {

      char conf[256];
      sprintf(conf, "above:c=$file\\n%s %s, %s scale, %s baseline, pol %d.", 
	      method ? "total" : "off-pulse" ,
	      var ? "variance" : "mean",
	      log ? "log" : "linear",
	      remove_baseline ? "variable" : "constant",
          pol);

      dsplot->configure(conf);
      cpgslct(dsplot_id);
      //cpgeras();
      cpgpage();
      dsplot->plot(arch);
      if (resum) {
	// Total
	tot_arch = arch->total();
        cpgslct(totplot_id);
	cpgpanl(1,1);
        cpgeras();
        totplot->plot(tot_arch);

	// pulse vs frequency
	pf_arch = arch->clone();
	pf_arch->pscrunch();
	pt_arch = pf_arch->clone();
	pf_arch->tscrunch();
	//	pf_arch->remove_baseline();
	cpgpanl(1,2);
        cpgeras();
        pfplot->plot(pf_arch);

	// pulse vs time
	pt_arch->fscrunch();
	//	pt_arch->remove_baseline();
	cpgpanl(1,3);
        cpgeras();
        ptplot->plot(pt_arch);

        cpgslct(dsplot_id);
        resum = false;
      }
      redraw = false;
      cpgslct(dsplot_id);
    }

    // Mark zapped profiles
    for (unsigned i=izap; i<zap_list.size(); i++) {
      float df=zap_list[i].freq0-zap_list[i].freq1;
      float dt=zap_list[i].sub0-zap_list[i].sub1;

      cpgsfs(2);
      cpgsci(2);
      if (fabs(df)<0.001 && fabs(dt)<0.001)
	cpgpt1(zap_list[i].sub0+0.5,zap_list[i].freq0,2);
      else
	cpgrect(zap_list[i].sub0+0.5,zap_list[i].sub1+0.5,zap_list[i].freq0,zap_list[i].freq1);
      cpgsci(1);
    }

    /* On click 0 get start of a range */
    if (click==0) {
      if (curs==freq_cursor) mode=5;
      else if (curs==time_cursor) mode=6;
      else if (curs==both_cursor) mode=7;
      cpgband(mode,0,0,0,&x0,&y0,&ch);
    } 

    /* On click 1 get the end of a range */
    else if (click==1) {
      if (curs==freq_cursor) mode=3;
      else if (curs==time_cursor) mode=4;
      else if (curs==both_cursor) mode=2;
      cpgband(mode,0,x0,y0,&x1,&y1,&ch);
    }
    
#if 0 
    /* Debug */
    printf("x0=%.3f y0=%.3f x1=%.3f y1=%.3f ch='%c' click=%d\n",
        x0, y0, x1, y1, ch, click);
#endif

    /* Left mouse click = zoom*/
    if (ch=='A') {
      if (click==0) { click=1; continue; }
      if (click==1) {
        /* Do zoom here */
        char conf[256];
        float tmp,t0,t1;
        if (curs==freq_cursor || curs==both_cursor) {
          if (bw>0 && y0>y1) { tmp=y0; y0=y1; y1=tmp; }
          if (bw<0 && y0<y1) { tmp=y0; y0=y1; y1=tmp; }
          sprintf(conf,"y:win=(%.3f,%.3f)",y0,y1);
          dsplot->configure(string(conf));
          pfplot->configure(string(conf));
        }
        if (curs==time_cursor || curs==both_cursor) {
          if (x0>x1) { tmp=x0; x0=x1; x1=tmp; }
          sprintf(conf,"srange=(%d,%d)",(int)x0,(int)x1);
          dsplot->configure(string(conf));
	  t0=sub2time(pt_arch,(int) x0);
	  t1=sub2time(pt_arch,(int) x1);
	  sprintf(conf,"y:win=(%.3f,%.3f)",t0,t1);
	  ptplot->configure(string(conf));
        }
        redraw = true;
        click = 0;
        continue;
      }
    }

    /* Middle mouse click = redraw diagnostics */
    if (ch=='D' || ch==CMD_UPDATE) {
      redraw = true;
      resum = true;
      izap=zap_list.size();
      continue;
    }

    /* Right mouse click = zap */
    if (ch=='X') {

      /* Zap a single row or pixel */
      if (click==0) {
        zap.freq0 = zap.freq1 = y0;
        zap.sub0 = zap.sub1 = (int)x0;
      }

      /* Zap a range */
      if (click==1) {
        zap.freq0 = y0;
        zap.freq1 = y1;
        zap.sub0 = (int)x0;
        zap.sub1 = (int)x1;
      }

      /* Apply it */
      zap.type = curs;
      apply_zap(arch, &zap);

      zap_list.push_back(zap);
      redraw = false;
      resum = false;
      click=0;

      continue;
    }

    /* Undo last zap */
    if (ch==CMD_UNDO) {
      if (!zap_list.empty()) {
        zap = zap_list.back();
        apply_zap_range(arch, &zap, true, orig_arch);
        zap_list.pop_back();
        /* Reapply whole list in case of overlapping zaps */
        for (unsigned i=0; i<zap_list.size(); i++) {
          zap = zap_list[i];
          apply_zap_range(arch, &zap);
        }
        redraw = true;
        resum = false;
      }
      click = 0;
      continue;
    }

    /* Show help */
    if (ch==CMD_HELP) {
      usage_interactive();
      click = 0;
      continue; 
    }

    /* Esc = cancel */
    if (ch==27) {
      click=0;
      continue;
    }

    /* Switch to freq mode */
    if (ch==CMD_FREQMODE) {
      curs = freq_cursor;
      click = 0;
      continue;
    }

    /* Switch to time mode */
    if (ch==CMD_TIMEMODE) {
      curs = time_cursor;
      click = 0;
      continue;
    }

    /* Switch to time/freq mode */
    if (ch==CMD_BOTHMODE) {
      curs = both_cursor;
      click = 0;
      continue;
    }

    /* Toggle variance plot */
    if (ch==CMD_VAR) {
      var = !var;
      redraw = true;
      if (var) 
        dsplot->configure("var=1");
      else
        dsplot->configure("var=0");
      click = 0;
      continue;
    }

    // Toggle dynamic spectrum
    if (ch==CMD_METHOD) {
      method = !method;
      redraw = true;
      if (method) 
        dsplot->configure("method=1");
      else
        dsplot->configure("method=0");
      click = 0;
      continue;
    }

    /* toggle log scale */
    if (ch==CMD_LOG) {
      log = !log;
      redraw = true;
      if (log) 
        dsplot->configure("cmap:log=1");
      else
        dsplot->configure("cmap:log=0");
      click = 0;
      continue;
    }

    /* Flip through polarizations */
    if (ch==CMD_POL) {
      pol = (pol + 1) % arch->get_npol();
      char conf[256];
      sprintf(conf, "pol=%d", pol);
      dsplot->configure(conf);
      redraw = true;
      click = 0;
      continue;
    }

    /* Unset zoom */
    if (ch==CMD_UNZOOM) {
      dsplot->configure("srange=(-1,-1)");
      dsplot->configure("y:win=(0,0)");
      pfplot->configure("y:win=(0,0)");
      ptplot->configure("y:win=(0,0)");
      redraw = true;
      click = 0;
      izap=zap_list.size();
      continue;
    }

    /* Save file */
    if (ch==CMD_SAVE) {
      /* Apply zaps to original file */
      for (unsigned i=0; i<zap_list.size(); i++) {
        zap = zap_list[i];
        apply_zap_range(orig_arch, &zap);
      }
      cout << "Unloading '" << output_filename << "'..." << endl;
      orig_arch->unload(output_filename);
      click = 0;
      continue;
    }

    /* Print paz command */
    if (ch==CMD_PRINT) {
      char *cstr;
      cstr=new char [output_ext.size()+1];
      strcpy(cstr,output_ext.c_str());
      if (zap_list.size()>0) 
        printf("paz -e %s",cstr);
      /* Full time/freq zaps */
      for (unsigned i=0; i<zap_list.size(); i++) {
        if (zap_list[i].type == freq_cursor) {
          int chan0 = freq2chan(arch, zap_list[i].freq0);
          int chan1 = freq2chan(arch, zap_list[i].freq1);
          if (chan1<chan0) { int tmp=chan0; chan0=chan1; chan1=tmp; }
          if (chan0==chan1) 
	    printf(" -z %d", chan0);
          else 
	    printf(" -Z \"%d %d\"", chan0, chan1);
        } else if (zap_list[i].type == time_cursor) {
          if (zap_list[i].sub0==zap_list[i].sub1) 
            printf(" -w %d", zap_list[i].sub0);
          else 
            printf(" -W \"%d %d\"", zap_list[i].sub0, zap_list[i].sub1);
        } 
      }
      if (zap_list.size()>0) 
        printf(" %s\n", filename.c_str());
      /* Small ranges */
      for (unsigned i=0; i<zap_list.size(); i++) {
        if (zap_list[i].type == both_cursor) {
	  printf("paz -m -I");
	  int chan0 = freq2chan(arch, zap_list[i].freq0);
          int chan1 = freq2chan(arch, zap_list[i].freq1);
          if (chan1<chan0) { int tmp=chan0; chan0=chan1; chan1=tmp; }
          if (chan0==chan1)
            printf(" -z %d", chan0);
          else
            printf(" -Z \"%d %d\"", chan0, chan1);
	  if (zap_list[i].sub0==zap_list[i].sub1)
            printf(" -w %d", zap_list[i].sub0);
          else
            printf(" -W \"%d %d\"", zap_list[i].sub0, zap_list[i].sub1);
	  cstr=new char [output_filename.size()+1];
	  strcpy(cstr,output_filename.c_str());
	  printf(" %s\n", (char *) cstr);
	}
      }
    }

    // Toggle variable baseline
    if (ch==CMD_BASELINE) {
      // Set baselining strategy
      if (remove_baseline) {
	fprintf(stderr, "Unsetting remove_baseline_strategy.\n");
	Pulsar::Archive::remove_baseline_strategy.set (new Pulsar::RemoveBaseline::Total, &Pulsar::RemoveBaseline::Total::transform);
	remove_baseline=false;
	arch=orig_arch->clone();
	arch->dedisperse();
	arch->remove_baseline();
	for (unsigned i=0; i<zap_list.size(); i++) {
	  zap = zap_list[i];
	  apply_zap_range(arch, &zap);
	}
      } else {
	fprintf(stderr, "Set remove_baseline_strategy.\n");
	Pulsar::Archive::remove_baseline_strategy.set (new Pulsar::RemoveVariableBaseline, &Pulsar::RemoveVariableBaseline::transform);
	remove_baseline=true;
	arch=orig_arch->clone();
	arch->dedisperse();
	arch->remove_baseline();
	for (unsigned i=0; i<zap_list.size(); i++) {
	  zap = zap_list[i];
	  apply_zap_range(arch, &zap);
	}
      }
      redraw=true;
      resum=true;
    }
    
    // Generate PSRSH script
    if (ch==CMD_PSRSH) {
      FILE *file;
      file=fopen(psrsh_filename.c_str(),"w");
      fprintf(file,"#!/usr/bin/env psrsh\n\n# Run with psrsh -e <ext> <script>.psh <archive>.ar\n\n");

      // Full sub/chan zap
      for (unsigned i=0; i<zap_list.size(); i++) {
        if (zap_list[i].type == freq_cursor) {
          int chan0 = freq2chan(arch, zap_list[i].freq0);
          int chan1 = freq2chan(arch, zap_list[i].freq1);
          if (chan1<chan0) { int tmp=chan0; chan0=chan1; chan1=tmp; }
          if (chan0==chan1) 
	    fprintf(file,"zap chan %d\n",chan0);
          else 
	    fprintf(file,"zap chan %d-%d\n", chan0, chan1);
        } else if (zap_list[i].type == time_cursor) {
          if (zap_list[i].sub0==zap_list[i].sub1) 
            fprintf(file,"zap subint %d\n", zap_list[i].sub0);
          else 
            fprintf(file,"zap subint %d-%d\n", zap_list[i].sub0, zap_list[i].sub1);
        } 
      }

      // Subint/channel intersections
      for (unsigned i=0; i<zap_list.size(); i++) {
        if (zap_list[i].type == both_cursor) {
	  int chan0 = freq2chan(arch, zap_list[i].freq0);
          int chan1 = freq2chan(arch, zap_list[i].freq1);
	  int sub0 = zap_list[i].sub0;
	  int sub1 = zap_list[i].sub1;
          if (chan1<chan0) { int tmp=chan0; chan0=chan1; chan1=tmp; }
          if (chan0==chan1 && sub0==sub1) {
            fprintf(file,"zap such %d,%d\n",sub0,chan0);
	  } else {
	    fprintf(file,"zap such");
	    for (int sub=sub0;sub<=sub1;sub++) {
	      for (int chan=chan0;chan<=chan1;chan++) {
		fprintf(file," %d,%d",sub,chan);
	      }
	    }
	    fprintf(file,"\n");
	  }
	}
      }
      fclose(file);
      printf("PSRSH script written\n");
    }

  } while (ch!=CMD_QUIT);

}


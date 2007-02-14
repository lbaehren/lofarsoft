/***************************************************************************
 *   Copyright (C) 2005 by Sven Lafebre                                    *
 *   s.lafebre@astro.ru.nl                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <Detection/PeakSearch.h>
#include <Detection/SourceFit.h>

#define PS_MIN_BLOCKSIZE 2
#define PS_MAX_BLOCKSIZE (7*sizeof(uint))

using std::cout;
using std::cerr;
using std::endl;
using std::abs;

namespace CR { // Namespace CR -- begin
  
  PeakSearch::PeakSearch() {
    blocksize_ = 10;
    threshold_ = 5;
    delay_     = 0;
    window_    = 70;
  
  }
  
  uint& PeakSearch::blocksize(uint bs) {
    // Set the blocksize
    blocksize_ = bs;
    if (bs < PS_MIN_BLOCKSIZE) {
      cerr << "  WARNING: Block size too small. Using " << (1 << PS_MIN_BLOCKSIZE)
	   << "." << endl;
      blocksize_ = PS_MIN_BLOCKSIZE;
    }
    if (bs > PS_MAX_BLOCKSIZE) {
      cerr << "  WARNING: Block size too large. Using " << ( 1 << PS_MAX_BLOCKSIZE)
	   << "." << endl;
      blocksize_ = PS_MAX_BLOCKSIZE;
    }
    
    return blocksize_;
  }
  
  uint& PeakSearch::threshold(uint th) {
    // Set the threshold value
    threshold_ = th;
    if (th > 64) {
      cerr << "  WARNING: " << th << " is an awfully big threshold..." << endl;
    }
    //cout << "Initializing PeakSearch with block size " << (1 << blocksize_) << 
    //        " and threshold " << threshold_ << "." << endl;
    
    return threshold_;
  }
  
  int& PeakSearch::delay(int dl) {
    delay_ = dl;
    
    return delay_;
  }
  
  uint& PeakSearch::window(uint wn) {
    window_ = wn;
    
    return window_;
  }
  
  uint& PeakSearch::npeaks(uint np) {
    npeaks_ = np;
    
    return npeaks_;
  }
  
  PeakSearch::PeakSearch(uint bs,
			 uint th,
			 int dl) {
    blocksize(bs);
    threshold(th);
    delay(dl);
  }
  
  PeakList PeakSearch::findPeaks(Data* d) {
    if (d->length() < (uint)(1 << blocksize_) + abs(delay_)) {
      cerr << "  ERROR: Dataset contains too little samples to run findPeaks algorithm." << endl;
      return PeakList();
    }
    
    PeakList peakList;
    uint avg;
    uint sdv;
    uint k = 1 << blocksize_;
    uint limit = 0;
    for (int i = (1 << blocksize_) + delay_; i < (int)d->length(); i ++) {
      if (k == uint(1 << blocksize_)) {
	avg = 0;
	sdv = 0;
	for (int j = i - delay_ - 1; j > i - delay_ - (1 << blocksize_) - 1; j --) {
	  avg += abs(d->data(j));
	}
	avg >>= blocksize_;
	
	for (int j = i - delay_ - 1; j > i - delay_ - (1 << blocksize_) - 1; j --) {
	  sdv += (abs(d->data(j))-avg)*(abs(d->data(j))-avg);
	}
	sdv >>= blocksize_;
	sdv = (uint)std::sqrt((double)sdv);
	k = 0;
	limit = avg + threshold_*sdv;
      }
      k ++;
      
      int a;
      if ((a = abs(d->data(i)) - limit) > 0) {
	Peak p(i, d->data(i), a);
	peakList.addPeak(p);
      }
    }
    return peakList;
  }
  
  
  PeakList PeakSearch::findPeaks(Data* d,
				 uint bs,
				 uint th,
				 int dl) {
    blocksize(bs);
    threshold(th);
    delay(dl);
    
    return findPeaks(d);
  }
  
  void PeakSearch::corrPeaks(PeakList* pl, uint n) {
    uint i[n];
    bool good[n];
    uint j;
    Peak* p_low = NULL;
    bool cont = true;
    uint set, set_prev = 0, t_max, t_prev = 0;
    
    for (j = 0; j < n; j ++) {
      i[j] = 0;
    }
    
    while (cont) {
      p_low = NULL;
      
      // Find a new peak (any peak) to start from
      for (j = 0; j < n; j ++) {
	if (pl[j].length() > 0) {
	  p_low = pl[j].peak(i[j]);
	  j = n;
	}
      }
      // If there are no more peaks in the lists, exit loop
      if (p_low == NULL) {
	cont = false;
      }
      
      if (cont) {
	// Get the current lowest peak
	for (j = 0; j < n; j ++) {
	  good[j] = false;
	  if (pl[j].peak(i[j]) != NULL
	      && pl[j].peak(i[j])->time() < p_low->time())
	    p_low = pl[j].peak(i[j]);
	}
	//cout << "t = " << p_low->time() << endl;
	
	// Is there a coincedence in the current window?
	set = 0;
	t_max = p_low->time();
	for (j = 0; j < n; j ++) {
	  if (pl[j].peak(i[j]) != NULL
	      && pl[j].peak(i[j])->time() < p_low->time() + window_) {
	    set ++;
	    good[j] = true;
	    if (pl[j].peak(i[j])->time() > t_max)
	      t_max = pl[j].peak(i[j])->time();
	  }
	}
	
	if (t_max != t_prev
	    && set > set_prev
	    && set > npeaks_) {
	  uint t = 0;
	  uint s = 0;
	  for (j = 0; j < n; j ++) {
	    if (good[j]) {
	      t += pl[j].peak(i[j])->time() - (pl[j].peak(i[j])->duration() >> 1);
	      s += pl[j].peak(i[j])->significance();
	    }
	  }
	  t /= set;
	  //cout << "t = " << t << "\t" << set << "\t" << s << endl;
	  
	  PeakList detection;
	  for (j = 0; j < n; j ++) {
	    if (good[j]) {
	      detection.addPeak(*(pl[j].peak(i[j])));
	    } else {
	      detection.addPeak();
	    }
	  }
	  
	  // What to do with the obtained correlated data? A sourcefit is not
	  // (yet) robust enough...
	  //				SourceFit f;
	  //				f.minimize(&detection);
	  for (j = 0; j < detection.length(); j++) {
	    detection.peak(j)->print();
	  }
	  cout << endl;
	}
	
	for (j = 0; j < n; j ++) {
	  if (pl[j].peak(i[j]) != NULL
	      && pl[j].peak(i[j])->time() == p_low->time())
	    i[j] ++;
	}
	t_prev   = t_max;
	set_prev = set;
      }
    }
  }
  
  void PeakSearch::corrPeaks(PeakList* pl, uint n, uint wn, uint np) {
    window(wn);
    npeaks(np);
    
    return corrPeaks(pl, n);
  }
  
} // Namespace CR -- end

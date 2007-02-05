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
#include <Detection/Peak.h>
#include <Detection/PeakList.h>

#define PL_PEAK_INC_NUM 128

using namespace std;

namespace CR { // Namespace CR -- begin
  
  PeakList::PeakList() {
    length_ = 0;
    space_  = PL_PEAK_INC_NUM;
    peak_   = (Peak*)malloc(space_*sizeof(Peak));
  }
  
  PeakList::PeakList(uint length) {
    length_ = length;
    space_  = length - (length % PL_PEAK_INC_NUM) + PL_PEAK_INC_NUM;
    peak_   = (Peak*)malloc(space_*sizeof(Peak));
  }
  
  PeakList::~PeakList() {
    // This doesn't work! Why???
    /*try {
      free(peak_);
      } catch (std::bad_alloc) {
      cerr << "  FATAL: Error deallocating memory." << endl;
      exit (1);
      }*/
  }
  
  Peak* PeakList::peak(uint i) {
	if (i >= 0 && i < length_) {
	  return &peak_[i];
	} else {
	  return NULL;
	}
  }
  
  Peak* PeakList::addPeak() {
    Peak p;
    
    return addPeak(p);
  }
  
  Peak* PeakList::addPeak(Peak p) {
    length_ ++;
    if (length_ > space_) {
      space_ += PL_PEAK_INC_NUM;
      peak_ = (Peak*)realloc(peak_, space_*sizeof(Peak));
    }
    
    peak_[length_-1] = p;
    
    if (length_ > 2 && peak_[length_-1].time() == peak_[length_-2].time() + 1) {
      length_ --;
      peak_[length_-1].mergePeaks(&peak_[length_-1], 2);
    }
    
    return &peak_[length_-1];
  }
  
} // Namespace CR -- end

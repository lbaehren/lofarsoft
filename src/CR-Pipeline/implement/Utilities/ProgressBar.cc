/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#include <Utilities/ProgressBar.h>

namespace CR {
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ---------------------------------------------------------------- ProgressBar

  ProgressBar::ProgressBar (int const &loops)
  {
    int barwidth = 70;
    string symbol = "#";
    //
    ProgressBar::init (loops,
		       barwidth,
		       symbol);
  }
  
  // ---------------------------------------------------------------- ProgressBar

  ProgressBar::ProgressBar (int const &loops,
			    int const &barwidth)
  {
    string symbol = "#";
    ProgressBar::init (loops, barwidth, symbol);
  }
  
  // ---------------------------------------------------------------- ProgressBar

  ProgressBar::ProgressBar (int const &loops,
			    int const &barwidth,
			    string const &symbol)
  {
    ProgressBar::init (loops, barwidth, symbol);
  }
  
  // ============================================================================
  //
  //  Initialization
  //
  // ============================================================================
  
  void ProgressBar::init (int const &loops,
			  int const &barwidth,
			  string const &symbol)
  {
    barwidth_p = barwidth;
    //
    ProgressBar::setMaximumValue (loops);
    ProgressBar::setSymbol (symbol);
    //
    showTime_p = false;
    ProgressBar::showTime (false);
    // 
    ProgressBar::setStarttime ();
  }
  
  // =============================================================================
  //
  //  Properties of the progress bar
  //
  // =============================================================================
  
  void ProgressBar::showTime (bool showTime)
  {
    bool tmpShowTime;
    //
    tmpShowTime = showTime_p;
    //
    showTime_p = showTime;
    if (tmpShowTime && !showTime) {
      // if we switch from on -> off
      barwidth_p += 10;
    } else if (!tmpShowTime && showTime) {
      // if we switch from off -> on
      barwidth_p -= 10;
    } else {
      // nothing to do
    }
  }
  
  
  // =============================================================================
  //
  //  Drawing of the progress bar
  //
  // =============================================================================
  
  void ProgressBar::update (int numCompleted)
  {
    int i;
    float remaintime;
    float frac,barLength;
    clock_t currentTime;
    
    cout.precision(4);
    
    if (numCompleted < maximumValue_p) {
      frac = numCompleted;
      frac /= maximumValue_p;
    } else {
      frac = 1.0;
    }
    barLength=frac*barwidth_p;
    
    i=0;
    cout << "\r [";                 // the left delimiter
    while (i<barLength) {           // the progress bar itself
      cout << symbol_p;
      i++;
    }
    while (i<barwidth_p) {      // fill up with blank spaces until right delimiter
      cout << " ";
      i++;
    }
    cout << "]" << setw(8) << 100*frac << " %";
    
    if (showTime_p != true) {
      cout << flush;
    } else {
      currentTime = clock();
      remaintime = ((float)( currentTime - starttime_p )/(float)CLOCKS_PER_SEC )*(1/frac-1.0);
      cout << setw(8) << remaintime << " s" << flush;
    }
    
    // add line break if task completed
    if (numCompleted >= maximumValue_p) {
      cout << endl;  
    }
    
  }

  // ============================================================================
  //
  //  Feedback
  //
  // ============================================================================

  void ProgressBar::summary (std::ostream &os)
  {
    os << "[ProgressBar]"                      << endl;
    os << "-- # loops .. : " << maximumValue_p << endl;
    os << "-- Bar length : " << barwidth_p     << endl;
    os << "-- Bar symbol : " << symbol_p       << endl;
    os << "-- Start time : " << starttime_p    << endl;
    os << "-- Show time  : " << showTime_p     << endl;
  }
  
}

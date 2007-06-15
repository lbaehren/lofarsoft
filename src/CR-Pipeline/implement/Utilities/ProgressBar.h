/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

/* $Id$ */

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::setw;
using std::string;

namespace CR {

/*!
  \class ProgressBar

  \ingroup Utilities
  
  \brief A simple ASCII progress bar in the terminal window.

  \author Lars B&auml;hren

  \test tProgressBar.cc

  <h3>Example</h3>

  Initializing the ProgressBar using
  \verbatim
  int maximumValue = 100;
  int barwidth = 70;

  ProgressBar bar = ProgressBar (maximumValue,barwidth,"=");

  for (int n=0; n<maximumValue; ++n) {
    // your computation code
    bar.update (n+1);
  }
  \endverbatim
  you get a simple progress bar in the terminal window, where the bar is drawn
  using the symbol specified as third function parameter:
  \verbatim
   [======================================================================]   100 %
  \endverbatim

  <h3>Alternatives</h3>

  Instead of the custum ProgressBar one might use the AIPS++ ProgressMeter
  class; in such a case
  \verbatim
  void calculate(uInt n) {
    Int skip = n / 200;
    ProgressMeter meter(0, n, "Title", "Subtitle", "", "", True, skip);
    for (uInt i=0; i<n; i++) {
      ... calculate ...
      meter.update(i);
    }
  }
  \endverbatim
*/

class ProgressBar {
  
 private:
  
  // Total number of loops, etc...
  int maximumValue_p;
  // Total length of the progress bar (number of characters)
  int barwidth_p;
  // Symbol used in the progress bar
  string symbol_p;
  // The  startup time
  clock_t starttime_p;
  // Display expected remain time for monitored process?
  bool showTime_p;
  
 public:
  
  /*!
    \brief Argumented constructor.
    
    Using this constructor the progress bar itself will be drawn with the default
    symbol, #.
  
    \param loops -- Total number of loops
  */
  ProgressBar (int const &loops);
  
  /*!
    \brief Argumented constructor.

    Using this constructor the progress bar itself will be drawn with the default
    symbol, #.

    \param loops    -- Total number of loops
    \param barwidth -- Total length of the progress bar (number of characters)
  */
  ProgressBar (int const &loops,
	       int const &barwidth);
  
  /*!
    \brief Argumented constructor.

    \param loops    -- Total number of loops, etc...
    \param barwidth -- Total length of the progress bar (number of
                       characters)
    \param symbol   -- The Symbol used for the bar itself.
  */
  ProgressBar (int const &loops,
	       int const &barwidth,
	       string const &symbol);
  
  // === Progress bar options ========================================

  /*!
    \brief Get the total number of loops for which the bar has been initialized.

    \return maximumValue -- The total number of loops for which the bar has been
                            initialized.
  */
  inline int maximumValue () {
    return maximumValue_p;
  }
  
  /*!
    \brief Set the total number of loops for which the bar is initialized.

    \param loops -- The maximum number of loops, over which the progress will be
                    monitored
  */
  inline void setMaximumValue (int const &loops) {
    maximumValue_p = loops;
  }
  
  /*!
    \brief Get the symbol used for drawing the progress bar.

    \return symbol -- The symbol/character used for drawing the progress bar
  */
  inline string symbol () {
    return symbol_p;
  }

  /*!
    \fn void setSymbol (const string)

    \brief Set the symbol used for drawing the progress bar.
  */
  inline void setSymbol (string const &symbol) {
    symbol_p = symbol;
  }
  
  /*!
    \brief Get the width of the progress bar

    \return barwidth -- The width of the progress bar (in characters).
  */
  inline int barwidth () {
    return barwidth_p;
  }

  /*!
    \brief Set the width of the progress bar

    \return barwidth -- The width of the progress bar (in characters)
  */
  inline void setBarwidth (int const &barwidth) {
    barwidth_p = barwidth;
  }
  
  /*!
    \fn void showTime (bool)

    \brief Switch on/off display of remaining time.

    Switch on/off display of remaining time for the computation being monitored.
    The width of the progress bar itself will be adjusted depending on wether or
    not the additional space for the time information is printed.
  */
  void showTime (bool);

  // === Draw progress bar ===========================================
  
  /*!
    \brief Update the progress bar.

    \param numCompleted = Number of completed computation steps.
  */
  void update (int);

  // -- Summary of object parameters --------------------------------------------

  /*
    \brief Provide a summary of the object's internal parameters
  */
  inline void summary () {
    summary (std::cout);
  }
  
  /*
    \brief Provide a summary of the object's internal parameters

    \param os -- Output stream to which the summary is written
  */
  void summary (std::ostream &os);
  
 private:
  
  /*!
    \brief Initialize the internal parameters of the progress bar

    \param loops    -- 
    \param barwidth -- 
    \param symbol   -- 
  */
  void init (int const &loops,
	     int const &barwidth,
	     string const &symbol);

  /*!
    \brief Store the start time at which the progress bar has been initialized
  */
  inline void setStarttime () {
    starttime_p = clock();
  }
  
};

}

#endif


#if !defined(PROGRESSBAR_H)
#define PROGRESSBAR_H

/* $Id: ProgressBar.h,v 1.3 2006/10/31 18:24:08 bahren Exp $ */

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
using namespace std;

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
    \fn ProgressBar (int)

    \brief Argumented constructor.

    Using this constructor the progress bar itself will be drawn with the default
    symbol, #.
    \param maximumValue = Total number of loops
  */
  ProgressBar (int);
  
  /*!
    \fn ProgressBar (int,int)

    \brief Argumented constructor.

    Using this constructor the progress bar itself will be drawn with the default
    symbol, #.
    \param maximumValue = Total number of loops
    \param barwidth     = Total length of the progress bar (number of
                          characters)
  */
  ProgressBar (int,int);
  
  /*!
    \fn ProgressBar (int,int,string)

    \brief Argumented constructor.

    \param maximumValue = Total number of loops, etc...
    \param barwidth     = Total length of the progress bar (number of
                          characters)
    \param symbol       = The Symbol used for the bar itself.
  */
  ProgressBar (int,int,string);

  // === Progress bar options ========================================

  /*!
    \fn int maximumValue ()

    \brief Get the total number of loops for which the bar has been initialized.
   */
  int maximumValue ();

  /*!
    \fn void setNofLoops (const int)

    \brief Set the total number of loops for which the bar is initialized.
   */
  void setMaximumValue (const int);

  /*!
    \fn void symbol ()

    \brief Get the symbol used for drawing the progress bar.
   */
  string symbol ();

  /*!
    \fn void setSymbol (const string)

    \brief Set the symbol used for drawing the progress bar.
   */
  void setSymbol (const string);

  int barwidth ();

  void setBarwidth (const int);
  
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
    \fn void update (int)

    \brief Update the progress bar.

    Draw an updated progress bar.
    
    \param numCompleted = Number of completed computation steps.
  */
  void update (int);

 private:
  
  void init (int,int,string);
  void setStarttime ();
  
};

}

#endif


#if !defined(PROGRESSBAR_H)
#define PROGRESSBAR_H

/* $Id: ProgressBar.h,v 1.2 2006/07/05 13:32:32 bahren Exp $ */

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
using namespace std;

namespace lopestools {
  
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
    
    //! Total number of loops, etc...
    int maximumValue_p;
    //! Total length of the progress bar (number of characters)
    int barwidth_p;
    //! Symbol used in the progress bar
    std::string symbol_p;
    //! The  startup time
    clock_t starttime_p;
    //! Display expected remain time for monitored process?
    bool showTime_p;
    
  public:
    
    /*!
      \brief Argumented constructor.
      
      Using this constructor the progress bar itself will be drawn with the default
      symbol, #.
      \param maximumValue = Total number of loops
    */
    ProgressBar (int const &maximumValue);
    
    /*!
      \brief Argumented constructor.
      
      Using this constructor the progress bar itself will be drawn with the default
      symbol, #.
      \param maximumValue = Total number of loops
      \param barwidth     = Total length of the progress bar (number of
                            characters)
    */
    ProgressBar (int const &maximumValue,
		 int const &barwidth);
  
  /*!
    \brief Argumented constructor.

    \param maximumValue = Total number of loops, etc...
    \param barwidth     = Total length of the progress bar (number of
                          characters)
    \param symbol       = The Symbol used for the bar itself.
  */
  ProgressBar (int const &maximumValue,
	       int const &barwidth,
	       std::string const &symbol);

  // === Progress bar options ========================================

  /*!
    \brief Get the total number of loops for which the bar has been initialized.

    \return maximumValue -- 
  */
  int maximumValue () {
    return maximumValue_p;
  }
  
  /*!
    \brief Set the total number of loops for which the bar is initialized.

    \param maximumValue -- 
  */
  void setMaximumValue (int const &maximumValue);
  
  /*!
    \brief Get the symbol used for drawing the progress bar.
    
    \return symbol -- 
  */
  std::string symbol () {
    return symbol_p;
  }
  
  /*!
    \brief Set the symbol used for drawing the progress bar.

    \param symbol -- 
   */
  void setSymbol (std::string const &symbol);

  /*!
    \brief Width of the progress bar
    
    \return barwidth -- Width of the progress bar
  */
  int barwidth () {
    return barwidth_p;
  }
  
  /*!
    \brief Set the total width of the progress bar

    \param barwidth -- The total width of the progress bar
  */
  void setBarwidth (int const &barwidth);
  
  /*!
    \brief Switch on/off display of remaining time.

    Switch on/off display of remaining time for the computation being monitored.
    The width of the progress bar itself will be adjusted depending on wether or
    not the additional space for the time information is printed.
    
    \param showTime -- Set <tt>true</tt> to enable display of estimated remaining
                       time
  */
  void showTime (bool const &showTime);

  // === Draw progress bar ===========================================
  
  /*!
    \fn void update (int)

    \brief Update the progress bar.

    Draw an updated progress bar.
    
    \param num -- Number of completed computation steps.
  */
  void update (int const &num);

 private:
  
  /*!
    \brief Initialize internal parameters

    \param loops    -- Total number of loops, etc...
    \param barwidth -- Total length of the progress bar (number of
                       characters)
    \param symbol   -- The Symbol used for the bar itself.
   */
  void init (int const &loops,
	     int const &barwidth,
	     std::string const &symbol);
  
  void setStarttime ();
  
};

}

#endif

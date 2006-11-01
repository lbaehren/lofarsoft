
#include <Utilities/ProgressBar.h>

namespace LOPES { // Namespace LOPES -- begin
  
  // =============================================================================
  //
  //  Construction
  //
  // =============================================================================
  
  ProgressBar::ProgressBar (int const &loops)
  {
    int barwidth = 70;
    string symbol = "#";
    //
    ProgressBar::init (loops, barwidth, symbol);
  }
  
  ProgressBar::ProgressBar (int const &loops,
			    int const &barwidth)
  {
    string symbol = "#";
    ProgressBar::init (loops, barwidth, symbol);
  }
  
  ProgressBar::ProgressBar (int const &loops,
			    int const &barwidth,
			    std::string const &symbol)
  {
    ProgressBar::init (loops, barwidth, symbol);
  }
  
  // =============================================================================
  //
  //  Initialization
  //
  // =============================================================================
  
  void ProgressBar::init (int const &loops,
			  int const &barwidth,
			  std::string const &symbol)
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
    //
    //   cout << "[ProgressBar::init]" << endl;
    //   cout << " # loops .. : " << loops << " -> " << maximumValue_p << endl;
    //   cout << " Bar length : " << barwidth << " -> " << barwidth_p << endl;
    //   cout << " Bar symbol : " << symbol_p << endl;
    //   cout << " Start time : " << starttime_p << endl;
    //   cout << " Show time  : " << showTime_p << endl;
  }
  
  void ProgressBar::setStarttime ()
  {
    starttime_p = clock();
  }
  
  // =============================================================================
  //
  //  Properties of the progress bar
  //
  // =============================================================================
  
  void ProgressBar::setMaximumValue (int const &loops)
  {
    maximumValue_p = loops;
  }
  
  void ProgressBar::setSymbol (std::string const &symbol)
  {
    symbol_p = symbol;
  }
  
  void ProgressBar::setBarwidth (int const &barwidth)
  {
    barwidth_p = barwidth;
  }
  
  void ProgressBar::showTime (bool const &showTime)
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
  
  void ProgressBar::update (int const &num)
  {
    int i;
    float remaintime;
    float frac,barLength;
    clock_t currentTime;
    
    cout.precision(4);
    
    if (num < maximumValue_p) {
      frac = num;
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
    if (num >= maximumValue_p) {
      cout << endl;  
    }
    
  }
  
}


#include <Utilities/ProgressBar.h>

// =============================================================================
//
//  Construction
//
// =============================================================================

ProgressBar::ProgressBar (int loops)
{
  int barwidth = 70;
  string symbol = "#";
  //
  ProgressBar::init (loops, barwidth, symbol);
}

ProgressBar::ProgressBar (int loops, int barwidth)
{
  string symbol = "#";
  ProgressBar::init (loops, barwidth, symbol);
}

ProgressBar::ProgressBar (int loops, int barwidth, string symbol)
{
  ProgressBar::init (loops, barwidth, symbol);
}

// =============================================================================
//
//  Initialization
//
// =============================================================================

void ProgressBar::init (int loops, int barwidth, string symbol)
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

int ProgressBar::maximumValue ()
{
  return maximumValue_p;
}

void ProgressBar::setMaximumValue (const int loops)
{
  maximumValue_p = loops;
}

string ProgressBar::symbol ()
{
  return symbol_p;
}

void ProgressBar::setSymbol (const string symbol)
{
  symbol_p = symbol;
}

int ProgressBar::barwidth ()
{
  return barwidth_p;
}

void ProgressBar::setBarwidth (const int barwidth)
{
  barwidth_p = barwidth;
}

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

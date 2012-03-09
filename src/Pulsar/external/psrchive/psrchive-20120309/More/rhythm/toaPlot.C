/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "toaPlot.h"
#include "Error.h"
#include "templates.h"

#include <cpgplot.h>
#include <cmath>

using namespace std;

wrapper::wrapper () {  
  x = 0.0;
  y = 0.0;
  ex = 0.0;
  ey = 0.0;
  ci = 7;
  dot = 0;
  id = 0;
}

toaPlot::toaPlot (QWidget *parent, const char *name )
  : QPgplot (parent, name)
{
  data.resize(0);

  xmin = ymin = 0.0;
  xmax = ymax = 1.0;
  
  xq = yq = None;
  logx = logy = false;
  
  mode = 0;
  task = 1;
  clicks = 0;
  
  requestEvent(mode);
}

// define pure virtual method of QtPgplot base class
void toaPlot::plotter ()
{
  cpgsci (1);
  cpgsls (1);
  cpgslw (1);
  cpgsvp (0.1,0.9,0.1,0.9);
  
  cpgswin (xmin, xmax, ymin, ymax);
  
  if (logx && logy)
    cpgbox ("bcnstl",0.0,0,"bcnstl",0.0,0);
  else if (logx)
    cpgbox ("bcnstl",0.0,0,"bcnst",0.0,0);
  else if (logy)
    cpgbox ("bcnst",0.0,0,"bcnstl",0.0,0);
  else
    cpgbox ("bcnst",0.0,0,"bcnst",0.0,0);
  
  std::string xlab, ylab;
  
  switch (xq) {
  case None:
    xlab = " ";
    break;
  case ResidualMicro:
    xlab += "Residual Microseconds";
    break;
  case ResidualMilliTurns:
    xlab += "Residual Milliturns";
    break;
  case TOA_MJD:
    xlab += "Arrival Time (Days Since MJD 50000)";
    break;
  case BinaryPhase:
    xlab += "Binary Phase";
    break;
  case ObsFreq:
    xlab += "Observing Frequency";
    break;
  case DayOfYear:
    xlab += "Day of Year";
    break;
  case ErrorMicro:
    xlab += "Timing Error (Microseconds)";
    break;
  case SignalToNoise:
    xlab += "Signal / Noise Ratio";
    break;
  case Bandwidth:
    xlab += "Observed Bandwidth";
    break;
  case DispersionMeasure:
    xlab += "Dispersion Measure";
    break;
  case Duration:
    xlab += "Observation Length (Seconds)";
    break;
  case ParallacticAngle:
    xlab += "Parallactic Angle (degrees)";
    break;
  case PointNumber:
    xlab += "Integration Number";
    break;
  }
  
  switch (yq) {
  case None:
    ylab = " ";
    break;
  case ResidualMicro:
    ylab += "Residual Microseconds";
    break;
  case ResidualMilliTurns:
    ylab += "Residual Milliturns";
    break;
  case TOA_MJD:
    ylab += "Arrival Time (MJD)";
    break;
  case BinaryPhase:
    ylab += "Binary Phase";
    break;
  case ObsFreq:
    ylab += "Observing Frequency";
    break;
  case DayOfYear:
    ylab += "Day of Year";
    break;
  case ErrorMicro:
    ylab += "Timing Error (Microseconds)";
    break;
  case SignalToNoise:
    ylab += "Signal / Noise Ratio";
    break;
  case Bandwidth:
    ylab += "Observed Bandwidth";
    break;
  case DispersionMeasure:
    ylab += "Dispersion Measure";
    break;
  case Duration:
    ylab += "Observation Length (Seconds)";
    break;
  case ParallacticAngle:
    ylab += "Parallactic Angle (degrees)";
    break;
  case PointNumber:
    ylab += "TOA List Index";
    break;
  }

  cpgsci (3);
  cpglab (xlab.c_str(), ylab.c_str(), "");
  cpgsci (1);

  for (unsigned i = 0; i < data.size(); i++) {

    cpgsci (data[i].ci);
    cpgpt1 (float(data[i].x), float(data[i].y), data[i].dot);

    if (data[i].ex != 0.0)
      cpgerr1 (5, float(data[i].x), float(data[i].y), float(data[i].ex), 1.0);
    
    if (data[i].ey != 0.0)
      cpgerr1 (6, float(data[i].x), float(data[i].y), float(data[i].ey), 1.0);
  }

  cpgsci(1);
}

void toaPlot::handleEvent (float x, float y, char ch)
{ 
  //cerr << "toaPlot::handleEvent x=" << x 
  //     << " y=" << y << " ch=" << ch << endl;
  
  if (ch == '~') {
    clicks = 0;
    clearScreen();
    drawPlot();
  }
  else {
    switch (mode) {
    case 0:
      if (task == 1) {
	//Do Nothing
      }
      else if (task == 2) {

	if (data.empty()) 
	  break;
	
	int tempint = data[0].id;
	float distance = 0.0;

	float xmin = 0.0;
	float xmax = 0.0;
	float ymin = 0.0;
	float ymax = 0.0;
	
	cpgqwin(&xmin, &xmax, &ymin, &ymax);
	
	float myx = x - xmin;
	float myy = y - ymin;

	float yscl = ymax - ymin;
	float xscl = xmax - xmin;
	
	float min = sqrt(pow((myx-float(data[0].x-xmin))/xscl, float(2.0)) + 
			 pow((myy-float(data[0].y-ymin))/yscl, float(2.0)));

	for (unsigned i = 1; i < data.size(); i++) {
	  distance = sqrt(pow((myx-float(data[i].x-xmin))/xscl, float(2.0)) + 
			  pow((myy-float(data[i].y-ymin))/yscl, float(2.0)));

	  if (distance < min) {
	    min = distance;
	    tempint = data[i].id;
	  }
	}
	emit selected(tempint);
	emit ineednewdata();
      }
      break;
    case 4:
      if (task == 1) {
	if (clicks == 0) {
	  x1 = x;
	  clicks++;
	  requestEvent(mode,x,y);
	  return;
	}
	if (clicks == 1) {
	  x2 = x;
	  clicks = 0;
	  if (x1 > x2) {
	    xmin = x2;
	    xmax = x1;
	  }
	  else {
	    xmin = x1;
	    xmax = x2;
	  }
	  clearScreen();
	  drawPlot();
	  break;
	}
      }
      else if (task == 2) {
	if (clicks == 0) {
	  x1 = x;
	  clicks++;
	  requestEvent(mode,x,y);
	  return;
	}
	if (clicks == 1) {
	  x2 = x;
	  clicks = 0;
	  if (x1 > x2) {
	    float handy1 = x1;
	    float handy2 = x2;
	    x2 = handy1;
	    x1 = handy2;
	  } 
	  std::vector<int> mypts;
	  for (unsigned i = 0; i < data.size(); i++) {
	    if ((data[i].x > x1) && (data[i].x < x2))
	      mypts.push_back(data[i].id);
	  }
	  emit selected(mypts); 
	  emit ineednewdata();
	  break;
	}
      }
    case 3:
      if (task == 1) {
	if (clicks == 0) {
	  y1 = y;
	  clicks++;
	  requestEvent(mode,x,y);
	  return;
	}
	if (clicks == 1) {
	  y2 = y;
	  clicks = 0;
	  if (y1 > y2) {
	    ymin = y2;
	    ymax = y1;
	  }
	  else {
	    ymin = y1;
	    ymax = y2;
	  }
	  clearScreen();
	  drawPlot();
	  break;
	}
      }
      else if (task == 2) {
	if (clicks == 0) {
	  y1 = y;
	  clicks++;
	  requestEvent(mode,x,y);
	  return;
	}
	if (clicks == 1) {
	  y2 = y;
	  clicks = 0;
	  if (y1 > y2) {
	    float handy1 = y1;
	    float handy2 = y2;
	    y2 = handy1;
	    y1 = handy2;
	  } 
	  std::vector<int> mypts;
	  for (unsigned i = 0; i < data.size(); i++) {
	    if ((data[i].y > y1) && (data[i].y < y2))
	      mypts.push_back(data[i].id);
	  }
	  emit selected(mypts);
	  emit ineednewdata();
	  break;
	}
      }
    case 2:
      if (task == 1) {
	if (clicks == 0) {
	  x1 = x;
	  y1 = y;
	  clicks++;
	  requestEvent(mode,x,y);
	  return;
	}
	if (clicks == 1) {
	  x2 = x;
	  y2 = y;
	  clicks = 0;
	  if (y1 > y2) {
	    ymin = y2;
	    ymax = y1;
	  }
	  else {
	    ymin = y1;
	    ymax = y2;
	  }
	  if (x1 > x2) {
	    xmin = x2;
	    xmax = x1;
	  }
	  else {
	    xmin = x1;
	    xmax = x2;
	  }
	  clearScreen();
	  drawPlot();
	  handleEvent(0,0,'~');
	  break;
	}
      }
      else if (task == 2) {
	if (clicks == 0) {
	  x1 = x;
	  y1 = y;
	  clicks++;
	  requestEvent(mode,x,y);
	  return;
	}
	if (clicks == 1) {
	  x2 = x;
	  y2 = y;
	  clicks = 0;
	  if (x1 > x2) {
	    float handy1 = x1;
	    float handy2 = x2;
	    x2 = handy1;
	    x1 = handy2;
	  } 
	  if (y1 > y2) {
	    float handy1 = y1;
	    float handy2 = y2;
	    y2 = handy1;
	    y1 = handy2;
	  } 
	  std::vector<int> mypts;
	  for (unsigned i = 0; i < data.size(); i++) {
	    if ((data[i].y > y1) && (data[i].y < y2) && (data[i].x > x1) && (data[i].x < x2))
	      mypts.push_back(data[i].id);
	  }
	  emit selected(mypts);
	  emit ineednewdata();
	  handleEvent(0,0,'~');
	  break;
	}
      }
    }
  }
  
  requestEvent(0,x,y);
}

void toaPlot::ptselector ()
{
  mode = 0;
  task = 2;
  handleEvent(0,0,'~');
}

void toaPlot::xzoomer ()
{
  mode = 4;
  task = 1;
  handleEvent(0,0,'~');
}

void toaPlot::yzoomer ()
{
  mode = 3;
  task = 1;
  handleEvent(0,0,'~');
}

void toaPlot::boxzoomer ()
{
  mode = 2;
  task = 1;
  handleEvent(0,0,'~');
}

void toaPlot::xselector ()
{
  mode = 4;
  task = 2;
  handleEvent(0,0,'~');
}

void toaPlot::yselector ()
{
  mode = 3;
  task = 2;
  handleEvent(0,0,'~');
}

void toaPlot::boxselector ()
{
  mode = 2;
  task = 2;
  handleEvent(0,0,'~');
}

void toaPlot::setPoints(AxisQuantity _xq, AxisQuantity _yq, std::vector<wrapper> _data,
			bool _logx, bool _logy)
{
  if (_data.empty()) {
    clearScreen();
    return;
  }
  
  xq = _xq;
  yq = _yq;

  data = _data;

  logx = _logx;
  logy = _logy;

  if (logx) {
    for (unsigned i = 0; i < data.size(); i++) {
      if (data[i].x <= 0.0) {
	logx = false;
	break;
      }
      data[i].x = log10(data[i].x);
      if (data[i].ex > 0.0)
	data[i].ex = log10(data[i].ex);
    }
  }

  if (logy) {
    for (unsigned i = 0; i < data.size(); i++) {
      if (data[i].y <= 0.0) {
	logy = false;
	break;
      }
      data[i].y = log10(data[i].y);
      if (data[i].ey > 0.0)
	data[i].ey = log10(data[i].ey);
    }
  }

  clearScreen();
  drawPlot();
}

void toaPlot::autobin (int nbins)
{
  float* xvals = new float[nbins];
  float* yvals = new float[nbins];
  float* binerrors = new float[nbins];
  float* bincount = new float[nbins];
  
  // Zero everything.
  
  for (int i = 0; i < nbins; i++){
    xvals[i]     = 0.0;
    yvals[i]     = 0.0;
    binerrors[i] = 0.0;
    bincount[i]  = 0.0;
  }

  // compute sum(x(i)wt(i))
  for (unsigned i = 0; i < data.size(); i++){
    int binnumber = int((data[i].x - xmin)/(xmax - xmin)*nbins);
    if (binnumber == nbins) binnumber--;
    xvals[binnumber] += data[i].x / data[i].ey / data[i].ey;
    yvals[binnumber] += data[i].y / data[i].ey / data[i].ey;
    bincount[binnumber]+=1.0 / (data[i].ey * data[i].ey);
  }

  beginDrawing();
  cpgsci(11);
  cpgsch(2.0);

  float rms, wted_sum_sq, sum_wts, sum_sq;
  sum_sq = 0.0;
  sum_wts = 0.0;
  wted_sum_sq = 0.0;

  // reweight and draw if valid data
  for (int i = 0; i < nbins; i++){
    if (bincount[i] != 0.0) {
      xvals[i] /= bincount[i];
      yvals[i] /= bincount[i];
      binerrors[i] = sqrt(1.0 / bincount[i]);
      wted_sum_sq += pow(yvals[i],float(2.0))/pow(binerrors[i],float(2.0));
      sum_wts += 1.0/(binerrors[i]*binerrors[i]);
      cpgpt1  (xvals[i],yvals[i],17);
      cpgerr1 (6,xvals[i],yvals[i],binerrors[i],1.0);
    }
  }
  rms = sqrt(wted_sum_sq/sum_wts);

  std::cout << "Weighted RMS residual for autobin is " << rms << " us " 
            << std::endl; 

  cpgsci(1);
  cpgsch(1.0);
  endDrawing();

  delete[] xvals;
  delete[] yvals;
  delete[] binerrors;
  delete[] bincount;
}

void toaPlot::autoscale ()
{
  if (data.empty()) return;
  
  vector<float> xpts (data.size());
  vector<float> ypts (data.size());
  
  for (unsigned i = 0; i < data.size(); i++) {
    xpts[i] = float(data[i].x);
    ypts[i] = float(data[i].y);
  }
  
  minmax(xpts, xmin, xmax);
  minmax(ypts, ymin, ymax);
  
  float xdiff = xmax - xmin;
  float ydiff = ymax - ymin;
  
  xmin -= fabs(xdiff * 0.2);
  
  xmax += fabs(xdiff * 0.2);
  
  ymin -= fabs(ydiff * 0.2);
  
  ymax += fabs(ydiff * 0.2);
  
  clearScreen();
  drawPlot();
}









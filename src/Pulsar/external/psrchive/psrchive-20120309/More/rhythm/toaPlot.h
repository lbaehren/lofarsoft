/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <qhbox.h>
#include <qpushbutton.h>

#include "qpgplot.h"

class wrapper {

 public:

  wrapper();

  double x;
  double y;
  double ex;
  double ey;

  int ci;
  int dot;

  int id;
};

class toaPlot: public QPgplot {
  
  Q_OBJECT

 public:
  
  enum AxisQuantity { None, ResidualMicro, ResidualMilliTurns, 
		      TOA_MJD, BinaryPhase, ObsFreq, DayOfYear,
		      ErrorMicro, SignalToNoise, Bandwidth,
		      DispersionMeasure, Duration, ParallacticAngle,
		      PointNumber };
  
  toaPlot ( QWidget *parent=0, const char *name=0 );
  
  // define pure virtual methods of QtPgplot base class
  void plotter ();
  void handleEvent (float x, float y, char ch);
  
  void setPoints (AxisQuantity, AxisQuantity, std::vector<wrapper>, bool, bool);

  public slots:

  void xzoomer ();
  void yzoomer ();
  void ptselector ();
  void xselector ();
  void yselector ();
  void boxzoomer ();
  void boxselector ();

  void autoscale ();
  void autobin (int nbin = 10);

 signals:

  void selected (int);
  void selected (std::vector<int>);
  void ineednewdata ();

 protected:

  // Information about data display

  std::vector<wrapper> data;
  
  float xmin, xmax, ymin, ymax;
  AxisQuantity xq, yq;

  bool logx;
  bool logy;
  
  int mode;
  int task;

  // Interactivity buffers
  unsigned clicks;
  
  float x1;
  float x2;
  float y1;
  float y2;
};





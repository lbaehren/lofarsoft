/***************************************************************************
 *
 *   Copyright (C) 2008 by Aris Noutsos
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Error.h"

#if !HAVE_GSL

#include <vector>

float lookup_PA_err (float xint)
{
  throw Error (InvalidState, "lookup_RM_err",
	       "GSL not available for cubic spline interpolation");
}

float lookup_RM_err (std::vector<float>& xint)
{
  throw Error (InvalidState, "lookup_RM_err",
	       "GSL not available for cubic spline interpolation");
}

#else

#include "Pulsar/Config.h"
#include "Interpolation.h"

#include <string>
#include <fstream>

using namespace std;

static Pulsar::Option<string> default_pa_error_filename 
(
 "rmfit::pa_error", Pulsar::Config::get_runtime() + "/pa_error_matrix.asc",

 "nkjj08 PA error array filename",

 "The name of the file containing the position angle error array \n"
 "used in the njkk08 additions to rmfit."
);

float lookup_PA_err (float xint)
{
  string filename = default_pa_error_filename;
  ifstream data ( filename.c_str() );
  if (!data)
    throw Error (FileNotFound, "lookup_PA_err", filename + " not found");

  double x,y;
  vector<double> xs;
  vector<double> ys;
  
  //////////// Read in table ////////////

  while (1)
  {  
    data >> x >> y;
    
    if (data.eof()) break;
       
    xs.push_back(x);
    ys.push_back(y);     
  }

  data.close();

#if 0
  float yp1 = -0.523;
  float ypn = 0.;
#endif

  Interpolation interp;
  interp.init (xs, ys);
  return interp.eval (xint);
}

static Pulsar::Option<string> default_rm_error_filename 
(
 "rmfit::rm_error", Pulsar::Config::get_runtime() + "/2D_data.asc",

 "nkjj08 RM error matrix filename",

 "The name of the file containing the rotation measure error matrix \n"
 "used in the njkk08 additions to rmfit."
);

class Interpolate2D
{
  class Row
  {
  public:
    double x0;
    vector<double> x1;
    vector<double> y;
    Interpolation interp;
  };

  vector<Row*> rows;

public:

  ~Interpolate2D ();
  void add (double x0, const vector<double>& x1, const vector<double>& y);
  double eval (double x0, double x1);
};

Interpolate2D::~Interpolate2D ()
{
  for (unsigned i=0; i<rows.size(); i++)
    delete rows[i];
}

void Interpolate2D::add (double x0,
			 const vector<double>& x1, 
			 const vector<double>& y)
{
  // cerr << "Interpolate2D::add x0=" << x0 << endl;

  Row* row = new Row;
  rows.push_back ( row );

  row->x0 = x0;
  row->x1 = x1;
  row->y  = y;

  row->interp.init (row->x1, row->y);
}

double Interpolate2D::eval (double x0, double x1)
{
  unsigned size = rows.size();

  vector<double> x (size);
  vector<double> y (size);

  for (unsigned i=0; i<size; i++)
  {
    x[i] = rows[i]->x0;
    y[i] = rows[i]->interp.eval (x1); 
  }

  Interpolation interp;
  interp.init (x, y);
  return interp.eval (x0);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
float lookup_RM_err(vector<float>& xint)
{
  string filename = default_rm_error_filename;
  ifstream data2D (filename.c_str());
  if (!data2D)
    throw Error (FileNotFound, "lookup_RM_err", filename + " not found");

  //////////// Read in table ////////////

  Interpolate2D interp;

  vector<double> x1s;
  vector<double> fs;

  double current_x0 = -1;

  while(1)
  {
    double x0, x1, f;

    data2D >> x0 >> x1 >> f;
    
    if (data2D.eof()) break;

    if (current_x0 < 0)
      current_x0 = x0;

    else if (x0 != current_x0)
    {
      interp.add (current_x0, x1s, fs);
      x1s.clear();
      fs.clear();
      current_x0 = x0;
    }

    x1s.push_back (x1);
    fs.push_back (f);
  }

  data2D.close();

  return interp.eval (xint[0], xint[1]);
}

#endif

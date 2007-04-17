
// Custom header files
#include <Math/Shapelets2D.h>

// =============================================================================
//
// Construction / Deconstruction
//
// =============================================================================

Shapelets2D::Shapelets2D ()
  : Shapelets1D ()
{}

Shapelets2D::Shapelets2D (const int order)
  : Shapelets1D (order)
{}

Shapelets2D::Shapelets2D (const int order, 
			  const double beta)
  : Shapelets1D (order,beta)
{}

Shapelets2D::~Shapelets2D () {}


// =============================================================================
//
//  Function evaluation
//
// =============================================================================

double Shapelets2D::eval (const int l, 
			  const double x,
			  const int m,
			  const double y) 
{  
  return Shapelets1D::eval(l,x)*Shapelets1D::eval(m,y);
}

double Shapelets2D::eval (const vector<int>& l,
			  const vector<double>& x)
{
  return Shapelets1D::eval(l[0],x[0])*Shapelets1D::eval(l[1],x[1]);
}


// =============================================================================
//
//  Global function properties
//
// =============================================================================

double Shapelets2D::integral (const int l,
			      const int m)
{
  return Shapelets1D::integral(l)*Shapelets1D::integral(m);
}

double Shapelets2D::integral (const vector<int>& l)
{
  return Shapelets1D::integral(l[0])*Shapelets1D::integral(l[1]);
}

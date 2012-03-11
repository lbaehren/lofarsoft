/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/JonesMueller.h"
#include "Pauli.h"

MEAL::JonesMueller::JonesMueller (Complex2* xform) : composite (this)
{
  if (xform)
    set_transformation (xform);
}

std::string MEAL::JonesMueller::get_name () const
{
  std::string name = "JonesMueller";
  if (transformation)
    name += "[" + transformation->get_name() + "]";
  return name;
}


/*! Complex2his method unmaps the old transformation before mapping xform */
void MEAL::JonesMueller::set_transformation (Complex2* _transformation) try
{
  if (!_transformation)
    return;

  if (transformation)
  {
    if (Complex2::verbose)
      std::cerr << "MEAL::JonesMueller::set_transformation unmap old"
		<< std::endl;

    composite.unmap (transformation);
  }

  transformation = _transformation;

  if (Complex2::verbose)
    std::cerr << "MEAL::JonesMueller::set_transformation map new" 
	      << std::endl;

  composite.map (transformation);
}
catch (Error& error)
{
  throw error += "MEAL::JonesMueller::set_transformation";
}

//! Calculate the Mueller matrix and its gradient
void MEAL::JonesMueller::calculate (Matrix<4,4,double>& result,
				    std::vector<Matrix<4,4,double> >* grad)
{
  std::vector< Jones<double> > jones_grad;
  std::vector< Jones<double> >* jones_grad_ptr = 0;
  
  if (grad)
    jones_grad_ptr = &jones_grad;

  Jones<double> jones_result = transformation->evaluate (jones_grad_ptr);

  result = Mueller( jones_result );

  if (!grad)
    return;

  for (unsigned i=0; i<grad->size(); i++)
    (*grad)[i] = Mueller( jones_result, jones_grad[i] );
}


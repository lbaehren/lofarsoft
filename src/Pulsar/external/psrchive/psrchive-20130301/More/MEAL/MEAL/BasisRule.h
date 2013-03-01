//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/BasisRule.h,v $
   $Revision: 1.3 $
   $Date: 2008/04/07 00:38:12 $
   $Author: straten $ */

#ifndef __MEAL_BasisRule_H
#define __MEAL_BasisRule_H

//#include "MEAL/ProjectGradient.h"
#include "MEAL/Composite.h"
#include "MEAL/Null.h"
#include "Matrix.h"
#include "tostring.h"

namespace MEAL {

  //! Changes the basis of the parameterization

  /*! Given any Function, \f$ M({\bf a}) \f$, and a linear
    transformation, \f$ {\bf T} \f$, this class evaluates \f$ M({\bf
    Ta}) \f$ and its partial derivatives. */

  template<unsigned N, class T>
  class BasisRule : public T
  {

  public:

    typedef typename T::Result Result;

    //! Default constructor
    BasisRule () : composite (this) { init(); }

    //! Copy constructor
    BasisRule (const BasisRule& rule) : composite (this) { init(); }

    //! Assignment operator
    BasisRule& operator = (const BasisRule& rule);

    //! Set the Function with the parameters to be transformed
    void set_model (T* model);

    //! Set the basis transformation
    void set_transformation (const Matrix<N,N,double>& xform);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Return the Result and its gradient
    void calculate (Result& result, std::vector<Result>* gradient);

    //! The Function with the parameters to be transformed
    Project<T> model;

    //! The free parameters
    Null parameters;

    //! The projection of the free parameters
    Project<Null> project;

    //! The basis transformation
    Matrix<N,N,double> transformation;

  private:

    //! Composite parameter policy
    Composite composite;

    void init () 
    { parameters.resize(N); project = &parameters;
      composite.map (project); matrix_identity (transformation); }

  };

}

template<unsigned N, class T>
std::string MEAL::BasisRule<N,T>::get_name () const
{
  return "BasisRule<" + tostring(N) + "," + std::string(T::Name)+ ">";
}

template<unsigned N, class T>
MEAL::BasisRule<N,T>&
MEAL::BasisRule<N,T>::operator = (const BasisRule& rule)
{
  if (this == &rule)
    return *this;

  set_model (rule.model);
  set_transformation (rule.transformation);

  return *this;
}

template<unsigned N, class T>
void MEAL::BasisRule<N,T>::set_model (T* _model)
{
  if (model)
  {
    if (T::verbose)
      std::cerr << "MEAL::BasisRule::set_model"
	" unmap old model" << std::endl;
    composite.unmap (model);
  }

  model = _model;

  if (!_model)
    return;

  if (T::verbose)
    std::cerr << "MEAL::BasisRule::set_model"
      " map new model" << std::endl;

  composite.map (model);

  Vector<N,double> params;

  for (unsigned i=0; i<N; i++)
  {
    model->set_infit (i, false);
    params[i] = model->get_param(i);
  }

  params = inv(transformation) * params;

  for (unsigned i=0; i<N; i++)
    parameters.set_param(i, params[i]);

}

template<unsigned N, class T>
void MEAL::BasisRule<N,T>::set_transformation (const Matrix<N,N,double>& xform)
{
  transformation = xform;

  if (!model)
    return;

  Vector<N,double> params;

  for (unsigned i=0; i<N; i++)
    params[i] = model->get_param(i);

  params = inv(transformation) * params;

  for (unsigned i=0; i<N; i++)
    parameters.set_param(i, params[i]);
}

template<unsigned N, class T>
void MEAL::BasisRule<N,T>::calculate (Result& result,
				      std::vector<Result>* grad)
{
  if (!model)
    throw Error (InvalidState, "MEAL::BasisRule::calculate","no model");

  if (T::verbose)
    std::cerr << "MEAL::BasisRule::calculate" << std::endl;

  Vector<N,double> params;
  for (unsigned i=0; i<N; i++)
    params[i] = parameters.get_param(i);

  params = transformation * params;

  for (unsigned i=0; i<N; i++)
    model->set_param (i, params[i]);

  std::vector<Result> model_grad;
  std::vector<Result>* model_grad_ptr = 0;
  if (grad)
    model_grad_ptr = & model_grad;

  result = model->evaluate (model_grad_ptr);
  
  if (grad) {

    unsigned ngrad = this->get_nparam();
    grad->resize (ngrad);

    unsigned igrad;
    for (igrad=0; igrad<ngrad; igrad++)
      (*grad)[igrad] = 0.0;

    // map the model gradient
    ProjectGradient (model, model_grad, *(grad));

    // map the scalar gradients
    std::vector<Result> fgrad (N, 0);

    for (igrad=0; igrad<N; igrad++)
      for (unsigned idep=0; idep<N; idep++)
	fgrad[igrad] += model_grad[idep] * transformation[idep][igrad];

    ProjectGradient (project, fgrad, *(grad));

  }

  if (T::verbose) {
    std::cerr << "MEAL::BasisRule::calculate result\n"
      "   " << result << std::endl;
    if (grad) {
      std::cerr << "MEAL::BasisRule::calculate gradient\n";
      for (unsigned i=0; i<grad->size(); i++)
	std::cerr << "   " << i << ":" << this->get_infit(i) << "=" 
                  << (*grad)[i] << std::endl;
    }
  }

}

#endif

/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Depolarizer.h"
#include "MEAL/Parameters.h"
#include <assert.h>

using namespace std;

MEAL::Depolarizer::Depolarizer ()
{
  Parameters* params = new Parameters (this, 9);
  params->set_name (0, "p_1");
  params->set_name (1, "p_2");
  params->set_name (2, "p_3");
  params->set_name (3, "m_11");
  params->set_name (4, "m_12");
  params->set_name (5, "m_13");
  params->set_name (6, "m_22");
  params->set_name (7, "m_23");
  params->set_name (8, "m_33");
}

//! Return the name of the class
string MEAL::Depolarizer::get_name () const
{
  return "Depolarizer";
}

//! Clone operator
MEAL::Depolarizer* MEAL::Depolarizer::clone () const
{
  return new Depolarizer( *this );
}

//! Calculate the Jones matrix and its gradient
void MEAL::Depolarizer::calculate (Matrix<4,4,double>& result,
				   vector< Matrix<4,4,double> >* grad)
{
  result[0][0] = 1.0;

  unsigned iparam = 0;

  // the polarizance
  for (unsigned i=1; i<4; i++)
  {
    result[i][0] = get_param(iparam);
    result[0][i] = 0.0;
    iparam ++;
  }

  // the depolarization
  for (unsigned i=1; i<4; i++)
  {
    for (unsigned j=i; j<4; j++)
    {
      if (i==j)
	result[i][j] = 1.0 + get_param(iparam);
      else
	result[i][j] = result[j][i] = get_param(iparam);
      iparam ++;
    }
  }

  assert (iparam == 9);

  if (!grad)
    return;

  iparam = 0;

  // the polarizance
  for (unsigned i=1; i<4; i++)
  {
    Matrix<4,4,double> M;
    M[i][0] = 1.0;
    (*grad)[iparam] = M;
    iparam ++;
  }

  // the depolarization
  for (unsigned i=1; i<4; i++)
  {
    for (unsigned j=i; j<4; j++)
    {
      Matrix<4,4,double> M;
      M[i][j] = M[j][i] = 1.0;
      (*grad)[iparam] = M;
      iparam ++;
    }
  }

  assert (iparam == 9);

  if (verbose)
  {
    cerr << "MEAL::Depolarizer::calculate gradient" << endl;
    for (unsigned i=0; i<9; i++)
      cerr << "   " << (*grad)[i] << endl;
  }

}



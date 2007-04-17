/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* $Id: PluginBase.cc,v 1.10 2007/02/27 15:48:07 horneff Exp $*/

#include <Calibration/PluginBase.h>

/*!
  \class PluginBase
*/

// ==============================================================================
//
//  Construction
//
// ==============================================================================

template <class T>
PluginBase<T>::PluginBase ()
{;}

template <class T>
PluginBase<T>::PluginBase (const Matrix<T>& weights)
{
  Bool status (True);
  status = setWeights (weights);
}

template <class T>
PluginBase<T>::PluginBase (const PluginBase& other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

template <class T>
PluginBase<T>::~PluginBase ()
{
  destroy();
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

template <class T>
PluginBase<T>& PluginBase<T>::operator= (const PluginBase<T>& other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

template <class T>
void PluginBase<T>::copy (const PluginBase<T>& other)
{
  // weights
  weights_p.resize(other.weights_p.shape());
  weights_p = other.weights_p;

  // misc parameters
  parameters_p = other.parameters_p;
}

template <class T>
void PluginBase<T>::destroy ()
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

template <class T>
Record&  PluginBase<T>::parameters ()
{
  return parameters_p;
}

template <class T>
Bool  PluginBase<T>::setParameters (Record const &param)
{
  Bool status (True);
  try {
    uInt i,nFields = param.nfields();
    DataType type;
    for (i=0; i<nFields; i++){
      type = param.dataType(i);
      switch (type) {
      case TpString:
	parameters_p.define(param.name(i),param.asString(i));
      case TpBool:
	parameters_p.define(param.name(i),param.asBool(i));
      case TpInt:
	parameters_p.define(param.name(i),param.asInt(i));
      case TpUInt:
	parameters_p.define(param.name(i),param.asuInt(i));
      case TpFloat:
	parameters_p.define(param.name(i),param.asFloat(i));
      case TpDouble:
	parameters_p.define(param.name(i),param.asDouble(i));
      case TpComplex:
	parameters_p.define(param.name(i),param.asComplex(i));
      case TpDComplex:
	parameters_p.define(param.name(i),param.asDComplex(i));
      case TpArrayBool:
	parameters_p.define(param.name(i),param.asArrayBool(i));
      case TpArrayInt:
	parameters_p.define(param.name(i),param.asArrayInt(i));
      case TpArrayUInt:
	parameters_p.define(param.name(i),param.asArrayuInt(i));
      case TpArrayFloat:
	parameters_p.define(param.name(i),param.asArrayFloat(i));
      case TpArrayDouble:
	parameters_p.define(param.name(i),param.asArrayDouble(i));
      case TpArrayComplex:
	parameters_p.define(param.name(i),param.asArrayComplex(i));
      case TpArrayDComplex:
	parameters_p.define(param.name(i),param.asArrayDComplex(i));
      default:
	cerr << "Sorry, setting a parameter of type " << type << 
	  " not implemented yet." << endl <<
	  "Please complain to the author!" << endl;
	status = False;
      };
    };
  } catch (AipsError x) {
    status = False;
  }
  return status;
}


template <class T>
Bool PluginBase<T>::setWeights (const Matrix<T>& weights)
{
  Bool status (True);

  weights_p.resize(weights.shape());
  weights_p = weights;

  return status;
}

// ==============================================================================
//
//  Methods
//
// ==============================================================================

template <class T>
Bool PluginBase<T>::calcWeights (const Matrix<T>& data)
{
  Bool status (True);

  cout << "PluginBase<T>::calcWeights called!" << endl;

  try {
    weights_p.resize (data.shape());
    weights_p = T(1);
  } catch (AipsError x) {
    status = False;
  }

  return status;
}

template <class T>
Bool PluginBase<T>::apply (Matrix<T>& data,
			   const Bool& computeWeights)
{
  Bool status (True);
  
  if (computeWeights) {
    status = calcWeights (data);
  }
  
  try {
     data *= weights_p;
  } catch (AipsError x) {
    status = False;
  }

  return status;
}

template <class T>
Bool PluginBase<T>::apply (Matrix<T> &outputData,
			   const Matrix<T>& inputData,
			   const Bool& computeWeights)
{
  Bool status (True);
  
  if (computeWeights) {
    status = calcWeights (inputData);
  }
  
  try {
    outputData.resize(inputData.shape());
    outputData = inputData*weights_p;
  } catch (AipsError x) {
    status = False;
  }

  return status;
}


// =============================================================================
//
//  Template instantiation
//
// =============================================================================

template class PluginBase<Double>;
template class PluginBase<DComplex>;

// These are old versions which should no be used!
template class PluginBase<Int>;
template class PluginBase<Float>;
//template class PluginBase<Complex>;

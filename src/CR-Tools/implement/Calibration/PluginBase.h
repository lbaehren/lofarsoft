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

/* $Id$*/

#ifndef _PLUGINBASE_H_
#define _PLUGINBASE_H_

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Containers/Record.h>

#include <casa/namespace.h>

/*!
  \class PluginBase

  \ingroup Calibration

  \brief Brief description for class PluginBase

  \author Lars B&auml;hren, Andreas Horneffer, Kalpana Singh

  \date 2006/03/03

  \test tPluginBase.cc

  <h3>Prerequisite</h3>

  <ul>
    <li>[CASA] <a href="http://aips2.nrao.edu/docs/casa/implement/Containers/RecordInterface.html">RecordInterface</a>
  </ul>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

*/

template <class T> class PluginBase {

 protected:

  //! Weights applyed to the data; might be computed internally
  Matrix<T> weights_p;

  //! The record that holds the parameters
  Record parameters_p;
  
 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  PluginBase ();

  /*!
    \brief Argumented constructor
    
    \param weights -- Weights applyed to the data
  */
  PluginBase (const Matrix<T>& weights);

  /*!
    \brief Copy constructor

    \param other -- Another PluginBase object from which to create this new
                    one.
  */
  PluginBase (const PluginBase& other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  virtual ~PluginBase ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another PluginBase object from which to make a copy.
  */
  PluginBase& operator= (const PluginBase &other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Get the record that stores the parameters for this function

    \return parameters -- the parameters record

    The user can the e.g. do calls like that:
    \code
    PluginBase<T> PB;
    // Set the parameter called window 
    PB.parameters().define("window",20) //type of window is Int
    // Retrieve the current window parameter
    Int curr_window;
    curr_window = PB.parameters().asInt("window");
    \endcode 
    The definition of the fields in the parameters record is done in the
    derived classes.
  */
  virtual Record& parameters ();

  /*!
    \brief Set the internal parameters controlling the behaviour of the plugin
  */
  virtual Bool setParameters (Record const &param);

  /*!
    \brief Get the weights which are applied to the data

    \return weights -- Weights which are applied to the data
  */
  Matrix<T> weights () {
    return weights_p;
  }

  /*!
    \brief Set the weights which are applied to the data

    \param weights -- Weights which are applied to the data

    \return ok -- Status of the function.
   */
  Bool setWeights (const Matrix<T>& weights);

  /*!
    \brief Recompute the weights based on the input data

    \param data -- Input data to the filter
  */
  virtual Bool calcWeights (const Matrix<T>& data);

  // -------------------------------------------------------------------- Methods

  /*!
    \brief Apply the filter to a block of data

    \param data           -- Input data to the filter
    \param computeWeights -- Recompute the weights based on the input data

    \retval data -- The data after application of the filter.
   */
  virtual Bool apply (Matrix<T>& data,
		      const Bool& computeWeights=True);

  /*!
    \brief Apply the filter to a block of data

    \param inputData      -- Input data to the filter
    \param computeWeights -- Recompute the weights based on the input data

    \retval outputData -- The data after application of the filter.
  */
  virtual Bool apply (Matrix<T> &outputData,
		      const Matrix<T>& inputData,
		      const Bool& computeWeights=True);


 private:

  /*!
    \brief Unconditional copying
  */
  void copy (const PluginBase& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

};

#endif /* _PLUGINBASE_H_ */

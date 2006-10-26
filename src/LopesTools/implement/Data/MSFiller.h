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

/* $Id: MSFiller.h,v 1.1 2006/07/13 14:11:54 bahren Exp $*/

#ifndef MSFILLER_H
#define MSFILLER_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>
#include <casa/Quanta/MVDirection.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Quanta/Unit.h>
#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/ComponentType.h>
#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/ComponentShape.h>
#include <components/ComponentModels/SpectralModel.h>
#include <components/ComponentModels/Flux.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MeasConvert.h>
#include <ms/MeasurementSets/MSSimulator.h>
#include <tasking/Glish.h>
#include <tasking/Glish/GlishRecord.h>

#include <casa/namespace.h>

/*!
  \class MSFiller

  \ingroup Data

  \brief Fill data into a MeasurementSet

  \author Lars B&auml;hren

  \date 2006/07/10

  \test tMSFiller.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/ms/implement/MeasurementSets/MeasurementSet.html">MeasurementSet</a>
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/ms/implement/MeasurementSets/MSSimulator.html">MSSimulator</a>
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/components/implement/ComponentModels/ComponentList.html">ComponentList</a>
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/components/implement/ComponentModels/SkyComponent.html">SkyComponent</a>
    <li>[LOPES] ParameterFromRecord.h
    <li>[LOPES] beamshape.g
  </ul>

  <h3>Synopsis</h3>

  The <tt>MSFiller</tt> expands the functionality of the underlying
  <tt>MSSimulator</tt>, a tool to create an empty MeasurementSet from observation
  and telescope descriptions.

  <h3>Example(s)</h3>

*/

class MSFiller : public MSSimulator {
  
  //! Name of the created MeasurementSet
  string msFilename_p;

  //! Componentslist
  ComponentList components_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  MSFiller ();

  /*!
    \brief Default constructor

    \brief msFilename -- Name of the created MeasurementSet
  */
  MSFiller (string const &msFilename);

  /*!
    \brief Copy constructor

    \param other -- Another MSFiller object from which to create this new
                    one.
  */
  MSFiller (MSFiller const &other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~MSFiller ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another MSFiller object from which to make a copy.
  */
  MSFiller& operator= (MSFiller const &other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Name of the created MeasurementSet

    \return msFilename -- Name of the created MeasurementSet
  */
  string msFilename () const {
    return msFilename_p;
  }

  /*!
    \brief Get the components list

    \return components -- Components list
  */
  ComponentList components () const {
    return components_p;
  }

  /*!
    \brief Set the components list

    \param components -- New components list
  */
  void setComponents (ComponentList const &components) {
    components_p = components;
  }

  /*!
    \brief Add another component to the components list

    \param component -- Sky component to be added to the components list
   */
  void addComponent (SkyComponent const &component);

  // -------------------------------------------------------------------- Methods

  /*!
    \brief Set the elevation limit information from a record

    \param rec -- Glish record

    \param status -- Status information; returns <tt>False</tt> if an error
                     occured
   */
  bool setElevationLimit (GlishRecord &rec);

  /*!
    \brief Set the weights for the autocorrelation products from a record

    \param rec -- Glish record

    \param status -- Status information; returns <tt>False</tt> if an error
                     occured
   */
  bool setAutoCorrelationWt (GlishRecord &rec);

  /*!
    \brief Set the time information from a record

    \param rec -- Glish record

    \param status -- Status information; returns <tt>False</tt> if an error
                     occured
   */
  bool setTimes (GlishRecord &rec);
  
  /*!
    \brief Set the antenna information from a record
    
    \param rec -- Glish record
    
    \param status -- Status information; returns <tt>False</tt> if an error
                     occured
  */
  bool initAnt (GlishRecord &rec);
  
 private:

  /*!
    \brief Initialization of internal parameters to default values 
  */
  void init ();

  /*!
    \brief Unconditional copying
  */
  void copy (MSFiller const &other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  /*!
    \brief Default components list

    By default this is a single point source at the position of the local zenith,
    i.e. at (0,90) deg in AZEL coordinates.
  */
  void setComponents ();
};

#endif /* MSFILLER_H */

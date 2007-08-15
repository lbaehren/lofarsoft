/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B&auml;hren (bahren@astron.nl)                                   *
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

#ifndef PARAMETERFROMRECORD_H
#define PARAMETERFROMRECORD_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Containers/Record.h>
#include <casa/Exceptions/Error.h>
#include <casa/Quanta.h>
#include <casa/Quanta/QuantumHolder.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasureHolder.h>
#include <ApplicationSupport/Glish.h>
#include <ApplicationSupport/GlishRecord.h>
#include <tasking/Tasking/ForeignArrayParameterAccessor.h>

using casa::GlishRecord;
using casa::MDirection;
using casa::MeasureHolder;
using casa::MEpoch;
using casa::MPosition;
using casa::Quantity;
using casa::Quantum;
using casa::QuantumHolder;
using casa::Record;
using casa::String;

namespace CR {
  
  /*!
    \ingroup ApplicationSupport
    
    \brief Methods to extract non-atomic parameters from a GlishRecord
    
    \author Lars B&auml;hren
    
    \date 2006/07/11
    
    \test tParameterFromRecord.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[AIPS++] casa/Containers/Record
      <li>[AIPS++] casa/Quanta/QuantumHolder -- A holder for Quantities to enable
      record conversions
      <li>[AIPS++] measures/Measures/MeasureHolder -- A holder for Measures to 
      enable record conversions
      <li>[AIPS++] tasking/Tasking/Foreign2ParameterAccessor -- Quanta conversions
      <li>[AIPS++] tasking/Tasking/Foreign3ParameterAccessor -- Measures conversions
    </ul>
    
    <h3>Synopsis</h3>

  <h3>Example(s)</h3>

  <ol>
    <li>Extract a quantity from a GlishRecord
    \code
    #include <Utilities/ParameterFromRecord.h>

    // ... some stuff ...
    
    bool status;
    Quantity obsTime;
    
    status = QuantityFromRecord (obsTime,  "time",  glishRec);
    \endcode
    <li>Extract a MEpoch from a GlishRecord
    \code
    bool status;
    MEpoch date;

    status = MEpochFromRecord   (date,     "date",  glishRec);
    \endcode
    <li>Parsing of a record containing Quanta and Measures:
    <ul>
      <li>Setting up the Glish record via
      \code
      record := [=];
      
      record.time    := qnt.quantity (10,"h");
      record.date    := dm.epoch('utc',qnt.quantity(rec.head('Date')));
      record.j2000   := dm.measure(dm.direction('AZEL',  '0deg', '90deg'),'J2000')
      record.azel    := dm.measure(dm.direction('AZEL',  '0deg', '90deg'),'AZEL')
      \endcode
      at <tt>print record</tt> yields:
      \verbatim
      [
       time=[value=10, unit=h],
       date=[type=epoch, refer=UTC, m0=[value=52940.4624, unit=d]],
       j2000=[type=direction, refer=J2000, m1=[value=0.853937685, unit=rad], m0=[unit=rad, value=-2.59622851]],
       azel=[type=direction, refer=AZEL, m1=[value=1.57079633, unit=rad], m0=[unit=rad, value=0]]
      ]
      \endverbatim
      <li>After passing of this record as event value, and parsing it via
      \code
      bool status (True);
      Quantity obsTime;
      MEpoch date;
      MDirection j2000;
      MDirection azel;
      
      status = QuantityFromRecord   (obsTime,  "time",  glishRec);
      status = MEpochFromRecord     (date,     "date",  glishRec);
      status = MDirectionFromRecord (j2000,    "j2000", glishRec);
      status = MDirectionFromRecord (azel,     "azel",  glishRec);
      \endcode
      we recover
      \verbatim
      [paramExtraction]
      - obsTime = 10 h
      - date    = Epoch: 52940::11:05:49.0000
      - azel    = Direction: [6.12303e-17, 0, 1]
      - j2000   = Direction: [-0.561711, -0.340815, 0.753873]
      \endverbatim
    </ul>
  </ol>

*/

// ------------------------------------------------------------------- Quantities

/*!
  \brief Extract a Quantity from a GlishRecord

  \param out   -- Quantity extracted form the GlishRecord <tt>rec</tt>
  \param field -- Name of the field, which contains the data to be extracted
  \param rec   -- GlishRecord from which the quantity is about to be extracted

  \return status -- Status of the operation; returns <tt>False</tt> in case
                    an error occured along the way.
*/
  bool QuantityFromRecord (Quantity &out,
			   String const &field,
			   GlishRecord const &rec);
  
  /*!
  \brief Add a quantity to a GlishRecord

  \todo not yet implemented

  \param rec      -- Glish record from which to extract the parameters
  \param field    -- Name of the field, within which the parameter is stored
  \param quantity -- 

  \return status -- Status of the operation; returns <tt>False</tt> in case
                    an error occured along the way.
*/
bool QuantityToRecord (casa::GlishRecord &rec,
		       String const &field,
		       Quantity const &quantity);


// --------------------------------------------------------------------- Measures

/*!
  \brief Extract a MEpoch from a GlishRecord

  \todo not yet implemented

  \param out   -- An instance in time extracted form the GlishRecord <tt>rec</tt>
  \param field -- Name of the field, which contains the data to be extracted
  \param rec   -- GlishRecord from which the MEpoch is about to be extracted

  \return status -- Status of the operation; returns <tt>False</tt> in case
                    an error occured along the way.
*/
bool MEpochFromRecord (MEpoch &out,
		       String const &field,
		       casa::GlishRecord const &rec);

/*!
  \brief Extract a MDirection from a GlishRecord

  \todo not yet implemented

  \param out   -- An instance in time extracted form the GlishRecord <tt>rec</tt>
  \param field -- Name of the field, which contains the data to be extracted
  \param rec   -- GlishRecord from which the MDirection is about to be extracted

  \return status -- Status of the operation; returns <tt>False</tt> in case
                    an error occured along the way.
*/
bool MDirectionFromRecord (MDirection &out,
			   String const &field,
			   casa::GlishRecord const &rec);

/*!
  \brief Extract a MPosition from a GlishRecord

  \todo not yet implemented

  \param out   -- An instance in time extracted form the GlishRecord <tt>rec</tt>
  \param field -- Name of the field, which contains the data to be extracted
  \param rec   -- GlishRecord from which the MPosition is about to be extracted

  \return status -- Status of the operation; returns <tt>False</tt> in case
                    an error occured along the way.
*/
bool MPositionFromRecord (MPosition &out,
			  String const &field,
			  casa::GlishRecord const &rec);

}

#endif /* PARAMETERFROMRECORD_H */

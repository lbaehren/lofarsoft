/*-------------------------------------------------------------------------*
| $Id$ |
*-------------------------------------------------------------------------*
***************************************************************************
*   Copyright (C) 2007                                                    *
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

#ifndef DATAREADERTOOLS_H
#define DATAREADERTOOLS_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/stdio.h>
#include <casa/iostream.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Exceptions/Error.h>

#include <ApplicationSupport/Glish.h>
#include <ApplicationSupport/GlishRecord.h>
#include <IO/LopesEvent.h>
#include <Data/ITS_Beam.h>
#include <Data/ITS_Capture.h>
#include <Data/NDABeam.h>
#include <IO/DataReader.h>

#include <casa/namespace.h>

using CR::LopesEvent;

/*!
  \class DataReaderTools

  \ingroup ApplicationSupport

  \brief A collection of helpful tools for programs using the DataReader

  \author Lars B&auml;hren

  \date 2006/06/20

  \test tDataReaderTools.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>CR::DataReader
  </ul>

  <h3>Synopsis</h3>
  
  Until the LOPES-Tools program kernel is ported to C/C++ communication
  is required between new tools and the kernel. One of the situations
  where this is the case, is when using the framework provided by the
  DataReader (see online documentation for details). Step by step all the
  the elements in a data file will be made available via a set of classes,
  such that information can be exchanged directly among objects. Until 
  this is completed we have to ensure that the required input information
  for objects is extracted on the Glish side and passed on.
  
  \verbatim
  .
  |- basefile                 = String
  |
  |- blocksize                = uint
  |
  |- adc2voltage              = Vector<float>
  |
  |- fft2calfft               = Matrix<complex>
  |
  |- antennas                 = Vector<uint>
  |
  |- selectedAntennas         = Vector<uint>
  \endverbatim

  <h3>Example(s)</h3>

  <ul type="square">
    <li>After extraction of all required parameters from the data record via
    \code
    RecordForDataReader (data,newrec)
    \endcode
    the record passed to the client application looks like this:
    \verbatim
    [
     basefile         = /dop71_0/lars/research/coding/lopestools/lopescasa/code/lopes/oldGlish/code//example.event,
     blocksize        = 65536,
     antennas         = [0 1 2 3 4 5 6 7] ,
     selectedAntennas = [0 2 3 4 5 6] ,
     fx2voltage       = [0.00048828125 0.00048828125 0.00048828125 0.00048828125 0.00048828125 0.00048828125 0.00048828125 0.00048828125] ,
     fft2calfft       = [[1:32769,]
                          1+0i 1+0i 1+0i 1+0i 1+0i 1+0i 1+0i 1+0i
			  1+0i 1+0i 1+0i 1+0i 1+0i 1+0i 1+0i 1+0i
			  1+0i 1+0i 1+0i 1+0i 1+0i 1+0i 1+0i 1+0i
    ]
    \endverbatim

    <li>Creating DataReader object using parameters passed via a GlishRecord
    \code
    // [1] Pointer to a DataReader object.
    DataReader *dr;
    
    // [2] Pass the input GlishRecord to the assignFromRecord function; after 
    //     parsing and extraction of the parameters a pointer to a DataReader
    //     object will be returned. 
    dr = DataReaderTools::assignFromRecord (rec)

    // [3] Start working with the DatReader object
    Matrix<Double> voltage (dr->voltage());
    \endcode
    
  </ul>
*/

class DataReaderTools {

 public:

  // -- Assignment of pointer to object -----------------------------------------

  /*!
    \brief Assign a new pointer to a DataReader object

    \param rec     -- Glish record
    \param verbose -- Be verbose? If set <tt>true</tt> the values of the extracted
                      parameters will be reported.

    \return dr -- Pointer to a DataReader object.
  */
  static DataReader* assignFromRecord (GlishRecord &rec,
				       bool const &verbose=false);

  // -- Extraction of individual parameters ------------------------------------

  /*!
    \brief Extract the name of the data type (LopesEvent, ITSCapture, etc.)

    \param rec -- Glish record
  */
  static bool getDatatype (String &datatype,
			   GlishRecord &rec);
  
  /*!
    \brief Extract the name of the basefile
    
    \param rec -- Glish record
    
    \return basefile -- The full path to the basefile of the data set.
  */
  static bool getBasefile (String &basefile,
			   GlishRecord &rec);
  
  /*!
    \brief Extract blocksize [samples] from Glish record
    
    \param rec -- Glish record
    
    \return blocksize -- 
  */
  static bool getBlocksize (uint &blocksize,
			    GlishRecord &rec);

  /*!
    \brief Extract nyquistZone from Glish record
    
    \param rec -- Glish record
    
    \return nyquistZone -- 
  */
  static bool getNyquistZone (uint &nyquistZone,
			    GlishRecord &rec);
  
  /*!
    \brief Extract samplingRate [Hz] from Glish record
    
    \param rec -- Glish record
    
    \return samplingRate -- 
  */
  static bool getSamplingRate (Double &samplingRate,
			       GlishRecord &rec);
  
  /*!
    \brief Extract weights for conversion from ADC values to voltages
    
    \param rec -- Glish record
    
    \return adc2voltage -- 
  */
  static bool getAdc2voltage (Vector<Double> &adc2voltage,
			      GlishRecord &rec);
  
  /*!
    \brief Extract weights for conversion from raw to calibrated FFT
    
    \param rec -- Glish record
    
    \return fft2calfft -- 
  */
  static Matrix<DComplex> fft2calfft (GlishRecord &rec);

  /*!
    \brief Extract the number of the antennas in the data set
    
    \param rec -- Glish record
    
    \return antennas -- 
  */
  static bool getAntennas (Vector<uint> &antennas,
			   GlishRecord &rec);
  
  /*!
    \brief Extract the antenna selection flags
    
    \param rec -- Glish record
    
    \return antennaSelection -- Array with flags for the individual antennas
  */
  static bool getAntennaSelection (Vector<bool> &antennaSelection,
				   GlishRecord &rec);
  
  /*!
    \brief Extract the number of the currently selected antennas
    
    \param rec -- Glish record
    
    \return selectedAntennas -- Indices of the selected antennas
  */
  static bool getSelectedAntennas (Vector<uint> &selectedAntennas,
				   GlishRecord &rec);

  /*!
    \brief Extract the number of the currently selected frequency channels

    \param rec -- Glish record

    \return selectedChannels -- Indices of the selected frequency channels
  */
  static bool getSelectedChannels (Vector<uint> &selectedChannels,
				   GlishRecord &rec);

};
  
#endif /* DATAREADERTOOLS_H */

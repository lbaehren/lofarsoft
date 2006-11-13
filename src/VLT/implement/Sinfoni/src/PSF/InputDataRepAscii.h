//#  InputDataRepAscii.h: Representation of input data in ASCII format.
//#
//#  Copyright (C) 2002-2006
//#  ASTRON (Netherlands Foundation for Research in Astronomy)
//#  P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
//#
//#  This program is free software; you can redistribute it and/or modify
//#  it under the terms of the GNU General Public License as published by
//#  the Free Software Foundation; either version 2 of the License, or
//#  (at your option) any later version.
//#
//#  This program is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#  GNU General Public License for more details.
//#
//#  You should have received a copy of the GNU General Public License
//#  along with this program; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//#
//#  $Id: InputDataRepAscii.h,v 1.8 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_INPUTDATAREPASCII_H
#define SINFONI_PSF_INPUTDATAREPASCII_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/PSF/InputDataRep.h>
#include <vector>
#include <fstream>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      //
      // This class contains the input data that will be read from the Matlab
      // data file. The data must be in ASCII format. Records should be
      // separated by a newline character. Each record should have the
      // following format:
      // \verbatim
      //   TimeStamp        TIME    (8 bytes) 1
      //   InFocus          INTEGER (4 bytes) 60
      //   OutFocus         INTEGER (4 bytes) 60
      //   Curvature        REAL    (4 bytes) 60
      //   CWSZernike       REAL    (4 bytes) 60
      //   DMZernike        REAL    (4 bytes) 60
      //   DMVoltage        REAL    (4 bytes) 60
      //   TTMTiptilt       REAL    (4 bytes) 2
      //   LGSTiptilt       REAL    (4 bytes) 2
      //   EstPiston        REAL    (4 bytes) 1
      //   CWSTip           REAL    (4 bytes) 1
      //   CWSTilt          REAL    (4 bytes) 1
      //   CWSDefocus       REAL    (4 bytes) 1
      //   WFE              REAL    (4 bytes) 3
      //   Flux             REAL    (4 bytes) 1
      //   LoopMode         INTEGER (4 bytes) 1
      //   DMPiston         REAL    (4 bytes) 2
      //   DMDefocus        REAL    (4 bytes) 2
      // \endverbatim
      // \note The specification of the size (in bytes) of the different
      // fields is irrelevant, because the data are currently stored in ASCII
      // format.
      //
      class InputDataRepAscii : public InputDataRep
      {
      public:
        // Default constructor. Check if the data file \a filename exists and
        // can be open it for reading.
        // \throw IOException.
        InputDataRepAscii(const std::string& filename);

        // Close the data file.
        virtual ~InputDataRepAscii();

        virtual Integer nrOfSamples();
        virtual void sumCountsMean(VectorReal& v);
        virtual void sumCountsVariance(VectorReal& v);
        virtual void diffCountsMean(VectorReal& v);
        virtual void diffCountsVariance(VectorReal& v);
        virtual void curvatureMean(VectorReal& v);
        virtual void curvatureCovariance(MatrixReal& m);
        virtual void mirrorMean(VectorReal& v);
        virtual void mirrorCovariance(MatrixReal& m);
        virtual void gainMean(VectorReal& v);
        virtual void gainVariance(VectorReal& v);

      private:
        // Read the data from the input file.
        // \return The number of records read.
        // \throw IOException.
        Integer read();

        // Here's the struct that represents one data record.
        struct Record
        {
          Double  timeStamp;
          Integer inFocus[60];
          Integer outFocus[60];
          Real    curvature[60];
          Real    cwsZernike[60];
          Real    dmZernike[60];
          Real    dmVoltage[60];
          Real    ttmTipTilt[2];
          Real    lgsTipTilt[2];
          Real    estPiston;
          Real    cwsTip;
          Real    cwsTilt;
          Real    cwsDefocus;
          Real    wfe[3];
          Real    flux;
          Integer loopMode;
          Real    dmPiston[2];
          Real    dmDefocus[2];
        };
        
        // We need a friend operator>>() in order to access our data record.
        friend std::istream& operator>>(std::istream& is, Record& rec);

        // Here we store the data records that we've read from file.
        std::vector<Record> itsData;

        // The input file stream handle.
        std::ifstream itsFile;
        
     };
      
    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif

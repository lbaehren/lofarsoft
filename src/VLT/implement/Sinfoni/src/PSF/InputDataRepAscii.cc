//#  InputDataRepAscii.cc: Representation of input data in ASCII format.
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
//#  $Id: InputDataRepAscii.cc,v 1.14 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/InputDataRepAscii.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/Blitz/Math.h>
#include <Sinfoni/Exceptions.h>
#include <Sinfoni/Debug.h>
#include <sstream>

using namespace blitz;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      //############################################################//
      //#                      Public methods                      #//
      //############################################################//

      InputDataRepAscii::InputDataRepAscii(const std::string& filename)
      {
        itsFile.open(filename.c_str());
        if (!itsFile) {
          THROW (IOException, 
                 "Couldn't open file " << filename << " for reading");
        }
      }

 
      InputDataRepAscii::~InputDataRepAscii()
      {
        itsFile.close();
      }


      Integer InputDataRepAscii::nrOfSamples()
      {
        //# Read the data if we haven't already done that.
        if (itsData.empty()) read();

        //# The number of samples is the size of vector of records.
        return itsData.size();
      }


      void InputDataRepAscii::sumCountsMean(VectorReal& v)
      {
        //# Read the data if we haven't already done that.
        if (itsData.empty()) read();

        //# Resize the receiving variable \a v.
        v.resize(theNrOfSubApertures);

        //# Calculate the sumCounts for all records and store them in a
        //# temporary matrix \a data.
        MatrixInteger data(itsData.size(), theNrOfSubApertures);
        for (Unsigned i = 0; i < itsData.size(); i++) {
          VectorInteger in(itsData[i].inFocus, shape(theNrOfSubApertures), 
                           neverDeleteData);
          VectorInteger out(itsData[i].outFocus, shape(theNrOfSubApertures), 
                            neverDeleteData);
          data(i, Range::all()) = in + out;
        }
        
        //# Calculate the column-wise mean of the data in the temporary matrix
        //# \a data, and store it in the receiving variable \a v.
        v = mean(data);
      }


      void InputDataRepAscii::sumCountsVariance(VectorReal& v)
      {
        //# Read the data if we haven't already done that.
        if (itsData.empty()) read();
        
        //# Resize the receiving variable \a v.
        v.resize(theNrOfSubApertures);

        //# Calculate the sumCounts for all records and store them in a
        //# temporary matrix \a data.
        MatrixInteger data(itsData.size(), theNrOfSubApertures);
        for (Unsigned i = 0; i < itsData.size(); i++) {
          VectorInteger in(itsData[i].inFocus, shape(theNrOfSubApertures), 
                           neverDeleteData);
          VectorInteger out(itsData[i].outFocus, shape(theNrOfSubApertures), 
                            neverDeleteData);
          data(i, Range::all()) = in + out;
        }
        
        //# Calculate the column-wise variance of the data in the temporary
        //# matrix \a data, and store it in the receiving variable \a v.
        v = var(data);
      }


      void InputDataRepAscii::diffCountsMean(VectorReal& v)
      {
        //# Read the data if we haven't already done that.
        if (itsData.empty()) read();

        //# Resize the receiving variable \a v.
        v.resize(theNrOfSubApertures);

        //# Calculate the diffCounts for all records and store them in a
        //# temporary matrix \a data.
        MatrixInteger data(itsData.size(), theNrOfSubApertures);
        for (Unsigned i = 0; i < itsData.size(); i++) {
          VectorInteger in(itsData[i].inFocus, shape(theNrOfSubApertures), 
                           neverDeleteData);
          VectorInteger out(itsData[i].outFocus, shape(theNrOfSubApertures), 
                            neverDeleteData);
          data(i, Range::all()) = in - out;
        }
        
        //# Calculate the column-wise mean of the data in the temporary
        //# matrix \a data, and store it in the receiving variable \a v.
        v = mean(data);
      }


      void InputDataRepAscii::diffCountsVariance(VectorReal& v)
      {
        //# Read the data if we haven't already done that.
        if (itsData.empty()) read();

        //# Resize the receiving variable \a v.
        v.resize(theNrOfSubApertures);

        //# Calculate the diffCounts for all records and store them in a
        //# temporary matrix \a data.
        MatrixInteger data(itsData.size(), theNrOfSubApertures);
        for (Unsigned i = 0; i < itsData.size(); i++) {
          VectorInteger in(itsData[i].inFocus, shape(theNrOfSubApertures), 
                           neverDeleteData);
          VectorInteger out(itsData[i].outFocus, shape(theNrOfSubApertures), 
                            neverDeleteData);
          data(i, Range::all()) = in - out;
        }
        
        //# Calculate the column-wise variance of the data in the temporary
        //# matrix \a data, and store it in the receiving variable \a v.
        v = var(data);
      }


      void InputDataRepAscii::curvatureMean(VectorReal& v)
      {
        //# Read the data if we haven't already done that.
        if (itsData.empty()) read();

        //# Resize the receiving variable \a v.
        v.resize(theNrOfSubApertures);

        //# Calculate the curvature for all records and store it in the
        //# temporary matrix \a data. 
        MatrixReal data(itsData.size(), theNrOfSubApertures);
        for (Unsigned i = 0; i < itsData.size(); i++) {
          data(i, Range::all()) = 
            VectorReal(itsData[i].curvature, shape(theNrOfSubApertures),
                       neverDeleteData);
        }
        
        //# Calculate the column-wise mean of the data in the temporary
        //# matrix \a data, and store it in the receiving variable \a v.
        v = mean(data);
      }


      void InputDataRepAscii::curvatureCovariance(MatrixReal& m)
      {
        //# Read the data if we haven't already done that.
        if (itsData.empty()) read();

        //# Resize the receiving variable \a m.
        m.resize(theNrOfSubApertures, theNrOfSubApertures);

        //# Calculate the curvature for all records and store it in the
        //# temporary matrix \a data. 
        MatrixReal data(itsData.size(), theNrOfSubApertures);
        for (Unsigned i = 0; i < itsData.size(); i++) {
          data(i, Range::all()) = 
            VectorReal(itsData[i].curvature, shape(theNrOfSubApertures),
                       neverDeleteData);
        }
        
        //# Calculate the column-wise covariance matrix of the data in the
        //# temporary matrix \a data, and store it in the receiving variabla \a
        //# m.
        m = cov(data);
      }


      void InputDataRepAscii::mirrorMean(VectorReal& v)
      {
        //# Read the data if we haven't already done that.
        if (itsData.empty()) read();

        //# Resize the receiving variable \a v.
        v.resize(theNrOfSubApertures);

        //# Put the mirror data of all records in a temporary matrix \a data.
        MatrixReal data(itsData.size(), theNrOfSubApertures);
        for (Unsigned i = 0; i < itsData.size(); i++) {
          data(i, Range::all()) = 
            VectorReal(itsData[i].dmVoltage, shape(theNrOfSubApertures),
                       neverDeleteData);
        }

        //# Calculate the column-wise mean of the data in the temporary
        //# matrix \a data, and store it in the receiving variable \a v.
        v = mean(data);
      }

      void InputDataRepAscii::mirrorCovariance(MatrixReal& m)
      {
        //# Read the data if we haven't already done that.
        if (itsData.empty()) read();

        //# Resize the receiving variable \a m.
        m.resize(theNrOfSubApertures, theNrOfSubApertures);

        //# Put the mirror data of all records in a temporary matrix \a data.
        MatrixReal data(itsData.size(), theNrOfSubApertures);
        for (Unsigned i = 0; i < itsData.size(); i++) {
          data(i, Range::all()) = 
            VectorReal(itsData[i].dmVoltage, shape(theNrOfSubApertures),
                       neverDeleteData);
        }

        //# Calculate the column-wise covariance matrix of the data in the
        //# temporary matrix \a data, and store it in the receiving variable
        //# \a m.
        m = cov(data);
      }


      void InputDataRepAscii::gainMean(VectorReal& v)
      {
        //# Read the data if we haven't already done that.
        if (itsData.empty()) read();

        //# Resize the receiving variable \a v.
        v.resize(theNrOfSubApertures);

        //# Note: the gain is currently not stored in the ASCII file. For the
        //# time being it is considered constant and equal for all
        //# sub-apertures.
        const Real gm = 1.0;

        v = gm;
      }


      void InputDataRepAscii::gainVariance(VectorReal& v)
      {
        //# Read the data if we haven't already done that.
        if (itsData.empty()) read();

        //# Resize the receiving variable \a v.
        v.resize(theNrOfSubApertures);

        //# Note: the gain is currently not stored in the ASCII file. For the
        //# time being it is considered constant and equal for all
        //# sub-apertures.
        const Real gv = 0.0;

        v = gv;
      }


      //############################################################//
      //#                      Private methods                     #//
      //############################################################//

      Integer InputDataRepAscii::read()
      {
        DBG_TRACE("Reading ASCII input data ...");
        Integer count(0);
        char line[8192];
        Record rec;
        while(!itsFile.eof()) {
          //# Read the next line.
          itsFile.getline(line, 8192);
          //# Strip anything after the comment character '%'.
          char* cp;
          if ((cp = strchr(line,'%')) != 0) *cp = '\0';
          //# If there's some string left, assume it's a line with record data
          if (strlen(line) > 0) {
            count++;
            try {
              istringstream iss(line);
              iss >> rec;
            } catch (IOException& e) {
              THROW(IOException, "Error reading data record #" << 
                    count << ", field " << e.what());
            }
            itsData.push_back(rec);
          }
        }
        return count;
      }


      istream& operator>>(istream& is, InputDataRepAscii::Record& rec)
      {
        if (!(is >> rec.timeStamp)) {
          THROW(IOException, "timeStamp");
        }
        for (Integer i=0; i<60; i++) {
          if (!(is >> rec.inFocus[i])) {
            THROW(IOException, "inFocus[" << i << "]");
          }
        }
        for (Integer i=0; i<60; i++) {
          if (!(is >> rec.outFocus[i])) {
            THROW(IOException, "outFocus[" << i << "]");
          }
        }
        for (Integer i=0; i<60; i++) {
          if (!(is >> rec.curvature[i])) {
            THROW(IOException, "curvature[" << i << "]");
          }
        }
        for (Integer i=0; i<60; i++) {
          if (!(is >> rec.cwsZernike[i])) {
            THROW(IOException, "cwsZernike[" << i << "]");
          }
        }
        for (Integer i=0; i<60; i++) {
          if (!(is >> rec.dmZernike[i])) {
            THROW(IOException, "dmZernike[" << i << "]");
          }
        }
        for (Integer i=0; i<60; i++) {
          if (!(is >> rec.dmVoltage[i])) {
            THROW(IOException, "dmVoltage[" << i << "]");
          }
        }
        for (Integer i=0; i<2; i++) {
          if (!(is >> rec.ttmTipTilt[i])) {
            THROW(IOException, "ttmTipTilt[" << i << "]");
          }
        }
        for (Integer i=0; i<2; i++) {
          if (!(is >> rec.lgsTipTilt[i])) {
            THROW(IOException, "lgsTipTilt[" << i << "]");
          }
        }
        if (!(is >> rec.estPiston)) {
          THROW(IOException, "estPiston");
        }
        if (!(is >> rec.cwsTip)) {
          THROW(IOException, "cwsTip");
        }
        if (!(is >> rec.cwsTilt)) {
          THROW(IOException, "cwsTilt");
        }
        if (!(is >> rec.cwsDefocus)) {
          THROW(IOException, "cwsDefocus");
        }
        for (Integer i=0; i<3; i++) {
          if (!(is >> rec.wfe[i])) {
            THROW(IOException, "wfe[" << i << "]");
          }
        }
        if (!(is >> rec.flux)) {
          THROW(IOException, "flux");
        }
        if (!(is >> rec.loopMode)) {
          THROW(IOException, "loopMode");
        }
        for (Integer i=0; i<2; i++) {
          if (!(is >> rec.dmPiston[i])) {
            THROW(IOException, "dmPiston[" << i << "]");
          }
        }
        for (Integer i=0; i<2; i++) {
          if (!(is >> rec.dmDefocus[i])) {
            THROW(IOException, "dmDefocus[" << i << "]");
          }
        }
        return is;
      }


    } //# namespace PSF

  } //# namespace SINFONI
  
} //# namespace ASTRON

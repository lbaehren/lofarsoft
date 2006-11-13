//#  InputDataRepFits.cc: Representation of input data in FITS format.
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
//#  $Id: InputDataRepFits.cc,v 1.10 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/InputDataRepFits.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/Exceptions.h>
#include <Sinfoni/Assert.h>
#include <vector>

using namespace CCfits;
using namespace std;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      //############################################################//
      //#                      Public methods                      #//
      //############################################################//

      InputDataRepFits::InputDataRepFits(const std::string& file,
                                         const std::string& hdu) :
        itsFits(0), itsTable(0)
      {
        FITS::setVerboseMode(SINFONI_DEBUG);

        //# Open the FITS file for reading.
        try {
          itsFits = new FITS(file, Read, true);
        } 
        catch (FitsException&) {
          THROW (IOException, "FITS Error: Cannot open file `" << file << "'");
        } 
        
        //# Read the extension Header Data Unit (HDU) \a hdu.
        try {
          ExtHDU& ext = itsFits->extension(hdu);
          //# The HDU should be a Binary Table Extension, hence we need to
          //# dynamic_cast<> it.
          itsTable = dynamic_cast<BinTable*>(&ext);
          if (itsTable == 0) {
            THROW (IOException, "FITS Error: HDU `" << hdu << "' in file `" 
                   << file << "' is not a Binary Table Extension");
          }
        }
        catch (FitsException&) {
          THROW (IOException, "FITS Error: Cannot read HDU `" << hdu 
                 << "' in file `" << file << "'");
        }
        
        //# The data should all be in one row, according to Florence Rigal
        //# (Astron) and Enrico Fedrigo (ESO). Let's make this precondition
        //# explicit by using an Assert.
        Assert(itsTable->rows() == 1);
      }


      InputDataRepFits::~InputDataRepFits()
      {
        delete itsFits;
      }


      Integer InputDataRepFits::nrOfSamples()
      {
        THROW(NotImplemented, 
              "This information is not available in the FITS file");
      }

      void InputDataRepFits::sumCountsMean(VectorReal& v)
      {
        read(v, "sumCountsMean");
      }

      void InputDataRepFits::sumCountsVariance(VectorReal& v)
      {
        read(v, "sumCountsVariance");
      }

      void InputDataRepFits::diffCountsMean(VectorReal& v)
      {
        read(v, "diffCountsMean");
      }

      void InputDataRepFits::diffCountsVariance(VectorReal& v)
      {
        read(v, "diffCountsVariance");
      }

      void InputDataRepFits::curvatureMean(VectorReal& v)
      {
        read(v, "curvatureMean");
      }

      void InputDataRepFits::curvatureCovariance(MatrixReal& m)
      {
        read(m, "curvatureCovariance");
      }

      void InputDataRepFits::mirrorMean(VectorReal& v)
      {
        read(v, "mirrorMean");
      }

      void InputDataRepFits::mirrorCovariance(MatrixReal& m)
      {
        read(m, "mirrrorCovariance");
      }

      void InputDataRepFits::gainMean(VectorReal& v)
      {
        read(v, "gainMean");
      }

      void InputDataRepFits::gainVariance(VectorReal& v)
      {
        read(v, "gainVariance");
      }


      //############################################################//
      //#                      Private methods                     #//
      //############################################################//

      void InputDataRepFits::read(VectorReal& v, const std::string& name)
      {
        vector< valarray<double> > data;

        //# Read the data in the column \a name.
        //# Note: the data should all be in one row.
        try {
          Column& col = itsTable->column(name);
          col.readArrays(data, 1, 1);
        }
        catch (FitsException&) {
          THROW (IOException, "FITS Error: Cannot find column `" << name
                 << "' in table `" << itsTable->name() 
                 << "' in file `" << itsFits->name() << "'");
        }
        
        //# \c data[0] should have exactly theNrOfSubApertures elements.
        Assert(static_cast<int>(data[0].size()) == theNrOfSubApertures);
        
        //# Resize the receiving variable \a v.
        v.resize(theNrOfSubApertures);
        
        //# Copy the data into \a v.
        for (int i = 0; i < theNrOfSubApertures; i++) {
          v(i) = data[0][i];
        }
      }


      void InputDataRepFits::read(MatrixReal& m, const std::string& name)
      {
        vector< valarray<double> > data;

        //# Read the data in the column \a name.
        //# Note: the data should all be in one row.
        try {
          Column& col = itsTable->column(name);
          col.readArrays(data, 1, 1);
        }
        catch (FitsException&) {
          THROW (IOException, "FITS Error: Cannot find column `" << name
                 << "' in table `" << itsTable->name() 
                 << "' in file `" << itsFits->name() << "'");
        }
        
        //# \c data[0] should have exactly theNrOfSubApertures^2 elements.
        Assert(static_cast<int>(data[0].size()) == 
               theNrOfSubApertures * theNrOfSubApertures);
        
        //# Resize the receiving variable \a m.
        m.resize(theNrOfSubApertures, theNrOfSubApertures);
        
        //# Copy the data into \a m.
        //# Note: the data is stored as a vector, hence it must be reshaped
        //# to a matrix.
        for (int i = 0; i < theNrOfSubApertures; i++) {
          for (int j = 0; j < theNrOfSubApertures; j++) {
            m(i,j) = data[0][i*theNrOfSubApertures+j];
          }
        }
      }
      
    } //# namespace PSF
    
  } //# namespace SINFONI
  
} //# namespace ASTRON

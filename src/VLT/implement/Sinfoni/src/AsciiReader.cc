//#  AsciiReader.cc: implementation of the ASCII data reader class.
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
//#  $Id: AsciiReader.cc,v 1.4 2006/05/19 16:03:23 loose Exp $

#include <Sinfoni/AsciiReader.h>
#include <Sinfoni/Exceptions.h>

namespace ASTRON
{
  namespace SINFONI
  {
    AsciiReader::AsciiReader(const std::string& filename)
    {
      itsFile.open(filename.c_str());
      if (!itsFile) {
        THROW (IOException, 
               "Couldn't open file " << filename << " for reading");
      }
    }


    AsciiReader::~AsciiReader()
    {
      itsFile.close();
    }


    void AsciiReader::read(VectorInteger& v, int rows)
    {
      if (rows > 0) v.resize(rows); 
      rows = v.rows();
      for (int row = 0; row < rows; row++) {
        if (!(itsFile >> v(row))) {
          THROW (IOException, "Error reading vector element (" << row << ")");
        }
      }
    }
      

    void AsciiReader::read(VectorReal& v, int rows)
    {
      if (rows > 0) v.resize(rows); 
      rows = v.rows();
      for (int row = 0; row < rows; row++) {
        if (!(itsFile >> v(row)))
          THROW (IOException, "Error reading vector element (" << row << ")");
      }
    }
      

    void AsciiReader::read(MatrixInteger& m, int rows, int cols)
    {
      if (rows > 0 && cols > 0) m.resize(rows, cols); 
      rows = m.rows();
      cols = m.cols();
      for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
          if (!(itsFile >> m(row, col))) 
            THROW (IOException, "Error reading matrix element (" << 
                   row << "," << col << ")");
        }
      }
    }
      
      
    void AsciiReader::read(MatrixReal& m, int rows, int cols)
    {
      if (rows > 0 && cols > 0) m.resize(rows, cols);
      rows = m.rows();
      cols = m.cols();
      for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
          if (!(itsFile >> m(row, col))) 
            THROW (IOException, "Error reading matrix element (" << 
                   row << "," << col << ")");
        }
      }
    }

  } // namespace SINFONI
  
} // namespace ASTRON

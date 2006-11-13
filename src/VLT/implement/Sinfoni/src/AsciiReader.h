//#  AsciiReader.h: Implementation class for an ASCII data reader.
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
//#  $Id: AsciiReader.h,v 1.4 2006/05/19 16:03:23 loose Exp $

#ifndef SINFONI_ASCIIREADER_H
#define SINFONI_ASCIIREADER_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>
#include <string>
#include <fstream>

namespace ASTRON
{
  namespace SINFONI
  {
    class AsciiReader
    {
    public:
      // Constructor. Open the datafile \a filename for reading.
      // \throw IOException when the file cannot be opened for reading.
      AsciiReader(const std::string& filename);

      // Close the datafile.
      ~AsciiReader();

      // Read \a rows rows of integers and store them in the vector \a v. If
      // \a rows > 0, then the vector \a v will be resized to \a rows rows,
      // otherwise the current size of the vector \a v is used as the number
      // of rows.
      // \throw IOException when the data are in the wrong format, or when a
      // premature end-of-file occurs.
      void read(VectorInteger& v, int rows = 0);

      // Read \a rows rows of reals and store them in the vector \a v. If \a
      // rows > 0, then the vector \a v will be resized to \a rows,
      // otherwise the current size of the vector \a v is used as the number
      // of rows.
      // \throw IOException when the data are in the wrong format, or when a
      // premature end-of-file occurs.
      void read(VectorReal& v, int rows = 0);

      // Read \a rows rows of \a cols columns of integers and store them in
      // the matrix \a m. If \a rows > 0 and \a cols > 0, then the matrix \a
      // m will be resized to \a rows rows and \a cols columns, otherwise
      // the current size of the matrix \a m is used as the number of rows
      // and columns.
      // \throw IOException when the data are in the wrong format, or when a
      // premature end-of-file occurs.
      void read(MatrixInteger& m, int rows = 0, int cols = 0);

      // Read \a rows rows of \a cols columns of reals and store them in the
      // matrix \a m. If \a rows > 0 and \a cols > 0, then the matrix \a m
      // will be resized to \a rows rows and \a cols columns, otherwise the
      // current size of the matrix \a m is used as the number of rows and
      // columns.
      // \throw IOException when the data are in the wrong format, or when a
      // premature end-of-file occurs.
      void read(MatrixReal& m, int rows = 0, int cols = 0);

    private:
      // The input file stream handle.
      std::ifstream itsFile;
        
    };

  } // namespace SINFONI
  
} // namespace ASTRON

#endif

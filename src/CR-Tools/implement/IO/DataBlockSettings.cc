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

#include <IO/DataBlockSettings.h>

// --- Construction ------------------------------------------------------------

DataBlockSettings::DataBlockSettings ()
  : filesize_p(0), method_p (ITERATIONS)
{;}

DataBlockSettings::DataBlockSettings (const int& filesize)
  : filesize_p(filesize), method_p(ITERATIONS)
{;}

DataBlockSettings::DataBlockSettings (const int& filesize,
				      DataBlockSettings::method which)
  : filesize_p(filesize), method_p(which)
{ ; }

// --- Destruction -------------------------------------------------------------

DataBlockSettings::~DataBlockSettings ()
{;}

// --- Parameters --------------------------------------------------------------

string DataBlockSettings::methodName ()
{
  switch (method_p) {
  case ITERATIONS: return string ("ITERATIONS");
  case BLOCKS: return string ("BLOCKS");
  case SAMPLES: return string ("SAMPLES");
  default: 
    cerr << "[DataBlockSettings::adjustmentMethod] Unsupported method" << endl;
  }
  return string ("UNKNOWN");
}

// --- Check settings ----------------------------------------------------------

void DataBlockSettings::check (int& nofIterations,
			       int& nofBlocks,
			       int& nofSamples)
{
  int offset (0);
  //
  DataBlockSettings::check (nofIterations,
			    nofBlocks,
			    nofSamples,
			    offset);
}

void DataBlockSettings::check (int& nofIterations,
			       int& nofBlocks,
			       int& nofSamples,
			       int& startOffset)
{
  int totalSize (0);
  int endPoint (0);
  //
  totalSize = nofSamples*nofBlocks*nofIterations;
  endPoint = totalSize-1+startOffset;
  //
  if (endPoint > filesize_p) {
    //
//     cout << " - Accessed data volume too large : " << endPoint
// 	 << " (" << filesize_p << ")" << endl;
    //
    switch (method_p) {
    case ITERATIONS: 
      nofIterations -= 1;
      break;
    case BLOCKS:
      nofBlocks -= 1;
      break;
    case SAMPLES:
      nofSamples -= 1;
      break;
    }
    DataBlockSettings::check (nofIterations,
			      nofBlocks,
			      nofSamples,
			      startOffset); 
    
  }
}

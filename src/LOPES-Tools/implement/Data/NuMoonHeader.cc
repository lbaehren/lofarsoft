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

/* $Id: NuMoonHeader.cc,v 1.1 2006/07/05 16:22:59 bahren Exp $*/

#include <lopes/Data/NuMoonHeader.h>

// ==============================================================================
//
//  Construction
//
// ==============================================================================

NuMoonHeader::NuMoonHeader ()
{;}

NuMoonHeader::NuMoonHeader (const String& fileName)
  : fileName_p (fileName)
{
  parseHeader ();
}

NuMoonHeader::NuMoonHeader (NuMoonHeader const& other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

NuMoonHeader::~NuMoonHeader ()
{
  destroy();
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

NuMoonHeader &NuMoonHeader::operator= (NuMoonHeader const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void NuMoonHeader::copy (NuMoonHeader const& other)
{;}

void NuMoonHeader::destroy ()
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================



// ==============================================================================
//
//  Methods
//
// ==============================================================================

void NuMoonHeader::parseHeader ()
{
  String line ("a");
  ifstream infile;
  int position (0);
  int n(0);

  infile.open(fileName_p.c_str());

  if (infile.is_open()) {
    while (position<=528 && line != "") {
      getline (infile,line);
      position = infile.tellg();
      cout << "\t" << n << "\t" << position << "\t" << line << endl;
      n++;
    }
  }

  infile.close();
}

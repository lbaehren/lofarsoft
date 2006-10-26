/***************************************************************************
 *   Copyright (C) 2006                                                  *
 *   Lars Bahren (<mail>)                                                     *
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

/* $Id: nuMoon.cc,v 1.1 2006/07/05 16:22:59 bahren Exp $*/

#include <lopes/Data/nuMoon.h>

/*!
  \class nuMoon
*/

// ==============================================================================
//
//  Construction
//
// ==============================================================================

nuMoon::nuMoon ()
{;}

nuMoon::nuMoon (nuMoon const &other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

nuMoon::~nuMoon ()
{
  destroy();
}

void nuMoon::destroy ()
{;}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

nuMoon& nuMoon::operator= (nuMoon const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void nuMoon::copy (nuMoon const &other)
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

Bool nuMoon::setStreams ()
{
  Bool status (True);

  return status;
}


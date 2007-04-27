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

/* $Id: SkymapQuantity.cc,v 1.2 2006/09/20 10:21:59 bahren Exp $*/

#include <lopes/Skymap/SkymapQuantity.h>

/*!
  \class SkymapQuantity
*/

// ==============================================================================
//
//  Construction
//
// ==============================================================================

// --------------------------------------------------------------- SkymapQuantity

SkymapQuantity::SkymapQuantity ()
{
  setSkymapQuantity (SkymapQuantity::FREQ_POWER);
}

// --------------------------------------------------------------- SkymapQuantity

SkymapQuantity::SkymapQuantity (SkymapQuantity::Type const &quantity)
{
  setSkymapQuantity (quantity);
}

// --------------------------------------------------------------- SkymapQuantity

SkymapQuantity::SkymapQuantity (SkymapQuantity const &other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

SkymapQuantity::~SkymapQuantity ()
{
  destroy();
}

void SkymapQuantity::destroy ()
{;}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

SkymapQuantity& SkymapQuantity::operator= (SkymapQuantity const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

// ------------------------------------------------------------------------- copy

void SkymapQuantity::copy (SkymapQuantity const &other)
{
  quantity_p = other.quantity_p;
}

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

// ----------------------------------------------------------------------- domain

String SkymapQuantity::domain ()
{
  String domain ("UNDEFINED");
  
  switch (quantity_p) {
  case TIME_FIELD:
  case TIME_POWER:
  case TIME_CC:
  case TIME_X:
    domain = "Time";
    break;
  case FREQ_POWER:
  case FREQ_FIELD:
    domain = "Frequency";
    break;
  }
  
  return domain;
}

// --------------------------------------------------------------------- quantity

String SkymapQuantity::quantity () 
{
  String quantity ("UNDEFINED");
  
  switch (quantity_p) {
  case TIME_FIELD:
  case FREQ_FIELD:
    quantity = "Field";
    break;
  case TIME_POWER:
  case FREQ_POWER:
    quantity = "Power";
    break;
  case TIME_CC:
    quantity = "CC";
    break;
  case TIME_X:
    quantity = "X";
    break;
  }
  
  return quantity;
}

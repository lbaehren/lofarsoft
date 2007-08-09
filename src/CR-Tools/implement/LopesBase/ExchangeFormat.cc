/***************************************************************************
 *   Copyright (C) 2005                                                  *
 *   Sven Lafebre (<mail>)                                                     *
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

/* $Id: ExchangeFormat.cc,v 1.1 2005/07/22 10:02:31 sven Exp $*/

#include <LopesBase/ExchangeFormat.h>

namespace CR {  // Namespace CR -- begin
  
  // --- Construction ------------------------------------------------------------
  
  ExchangeFormat::ExchangeFormat () {
    format_p = 0;
  }
  
  ExchangeFormat::ExchangeFormat (const uInt &f) {
    format_p = f;
  }
  
  ExchangeFormat::ExchangeFormat (ExchangeFormat &f) {
    format_p = f.format();
  }
  
  // --- Destruction -------------------------------------------------------------
  
  ExchangeFormat::~ExchangeFormat ()
  {;}
  
  // --- Parameters --------------------------------------------------------------
  
  void ExchangeFormat::setFlag(const uInt fl, bool on) {
    if (on) {
      format_p = format_p | fl;
    } else {
      format_p = format_p & (~fl);
    }
  }
  
  // --- Functions ---------------------------------------------------------------
  
  bool ExchangeFormat::compare(const ExchangeFormat &f) {
    return format_p == f.format_p;
  }
  
}  // Namespace CR -- end

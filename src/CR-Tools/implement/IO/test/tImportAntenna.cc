/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

/* $Id: tImportAntenna.cc,v 1.5 2007/03/22 10:57:15 huege Exp $*/

#include <string>

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <IO/ImportAntenna.h>
#include <Math/ThreeVector.h>

using std::string;
using CR::ImportAntenna;
using CR::ThreeVector;

/*!
  \file tImportAntenna.cc
  
  \ingroup IO
  
  \brief A collection of test routines for ImportAntenna
  
  \author Tim Huege
  
  \date 2006/10/16
  
  <b>Note:</b> should be taken with care, since program used hard-coded path
  to the input data.
*/

// -----------------------------------------------------------------------------

int main ()
{
  string name("/home/huege/lopescasa/code/lopes/implement/reas2event/dummy.dat");
  ImportAntenna Test("00000", name, 0.0, 180.0);
  
  std::cout << "SamplingTimeScale: " << Test.getSamplingTimeScale() << std::endl;
  std::cout << "ValidData: " << Test.validData() << std::endl;
  std::cout << "ID: " << Test.getID() << std::endl;

  Vector<double> Ezeni, Eazi;
  
  bool negligibleRemainder = Test.getTimeSeries(Eazi, Ezeni);    

  std::cout << "Negligible remainder: " << negligibleRemainder << std::endl;

  for (long i=0; i<static_cast<long>(Eazi.size()); ++i)
  {
     std::cout << "Eazi: " << Eazi(i) << "   Ezeni: " << Ezeni(i) << std::endl;
  }

  return 0;
}

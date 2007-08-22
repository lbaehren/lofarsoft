/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

/*!
  \file Math.h
  
  \ingroup CR

  \defgroup Math CR-Pipeline: Math module
  \ingroup CR

  \brief Math module for the CR-pipeline
  
  \author Lars B&auml;hren
  
  \date 2007/01/16

  <h3>Synopsis</h3>

  Even though we make usage of the extensive functionality of the CASA math
  modules, there sometimes are operations missing which will be required 
  over and over again. The Math module of the CR-Pipeline is a collection of
  such additional functions.

  <h3>Usage</h3>

  Use this file in order to import all the prototypes for functions and classes
  in the <i>Math</i> module.
*/

#include <Math/BasicFilter.h>
#include <Math/Constants.h>
#include <Math/HanningFilter.h>
#include <Math/Hermite1D.h>
#include <Math/LinearRegression.h>
#include <Math/MathAlgebra.h>
#include <Math/MathFFT.h>
#include <Math/MedianFilter.h>
#include <Math/RaisedCosineFilter.h>
#include <Math/RangeSplitter.h>
#include <Math/RotationMatrix2D.h>
#include <Math/RotationMatrix3D.h>
#include <Math/RotationMatrix.h>
#include <Math/Shapelets1D.h>
#include <Math/Shapelets2D.h>
#include <Math/ShapeletsModel1D.h>
#include <Math/ShapeletsModel2D.h>
#include <Math/Statistics.h>
#include <Math/StatisticsFilter.h>
#include <Math/ThreeVector.h>
#include <Math/VectorConversion.h>
#include <Math/VectorNorms.h>

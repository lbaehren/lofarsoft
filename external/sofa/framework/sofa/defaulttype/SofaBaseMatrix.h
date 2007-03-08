/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef SOFA_DEFAULTTYPE_SOFABASEMATRIX_H
#define SOFA_DEFAULTTYPE_SOFABASEMATRIX_H

#include <sofa/defaulttype/SofaBaseVector.h>


namespace sofa
{

namespace defaulttype
{

class SofaBaseMatrix
{
public:
	virtual ~SofaBaseMatrix(){};

	virtual void resize(int nbRow, int nbCol) = 0;
	virtual double &element(int i, int j) = 0;
	virtual int rowDim(void) = 0;
	virtual int colDim(void) = 0;

	virtual void solveInResVect(void) = 0;
	virtual void solve(void) = 0;

	virtual SofaBaseVector *getOpVect(void) = 0;
	virtual SofaBaseVector *getResVect(void) = 0;

protected:
	SofaBaseVector *opVect, *resVect;
};


} // nampespace defaulttype

} // nampespace sofa

#endif

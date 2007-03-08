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
#ifndef SOFA_DEFAULTTYPE_NEWMATSOFAMATRIX_H
#define SOFA_DEFAULTTYPE_NEWMATSOFAMATRIX_H

#include <sofa/defaulttype/SofaBaseMatrix.h>
#include "NewMatSofaVector.h"

#include "NewMAT/newmat.h"
#include <iostream>

namespace sofa
{

namespace defaulttype
{

class NewMatSofaMatrix : public SofaBaseMatrix
{
public:

	NewMatSofaMatrix()
	{
		impl = new NewMAT::Matrix;
		resVect = new NewMatSofaVector;
		opVect = new NewMatSofaVector;
	}

	virtual ~NewMatSofaMatrix()
	{
		delete impl;
		delete resVect;
		delete opVect;
	}

	virtual void resize(int nbRow, int nbCol)
	{
		impl->ReSize(nbRow, nbCol);
		(*impl) = 0.0;

		resVect->resize(nbRow);
		opVect->resize(nbRow);
	};

	virtual int rowDim(void)
	{
		return impl->Nrows();
	};

	virtual int colDim(void)
	{
		return impl->Ncols();
	};

	virtual double &element(int i, int j)
	{
		return impl->element(i,j);
	};

	virtual void solveInResVect(void)
	{
		if ((opVect != NULL) && (resVect != NULL))
		{
			std::cout << "Entering in solveInResVect\n";
			NewMatSofaVector *rV = static_cast<NewMatSofaVector *>(resVect);
			NewMatSofaVector *oV = static_cast<NewMatSofaVector *>(opVect);

			*(rV->impl) = impl->i() * (*(oV->impl));
			std::cout << "Exiting solveInResVect\n";
		}
	};

	virtual void solve(void)
	{
		if (opVect != NULL)
		{

		}
	};

	virtual SofaBaseVector *getOpVect()
	{
			return opVect;
	};

	virtual SofaBaseVector *getResVect()
	{
			return resVect;
	};

private:
	NewMAT::Matrix *impl;
};


} // namespace defaulttype

} // namespace sofa
#endif

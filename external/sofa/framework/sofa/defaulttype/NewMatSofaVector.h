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
#ifndef SOFA_DEFAULTTYPE_NEWMATSOFAVECTOR_H
#define SOFA_DEFAULTTYPE_NEWMATSOFAVECTOR_H

#include <sofa/defaulttype/SofaBaseMatrix.h>

#include "NewMAT/newmat.h"

namespace sofa
{

namespace defaulttype
{

class NewMatSofaVector : public SofaBaseVector
{
	friend class NewMatSofaMatrix;

public:

	NewMatSofaVector()
	{
		impl = new NewMAT::ColumnVector();
	}

	virtual ~NewMatSofaVector()
	{
		delete impl;
	}

	virtual void resize(int dim)
	{
		impl->ReSize(dim);
		(*impl) = 0.0;
	};

	virtual double &element(int i)
	{
		return impl->element(i);
	};

	virtual int size(void)
	{
		return impl->Nrows();
	};

private:
	NewMAT::ColumnVector *impl;
};

} // namespace defaulttype

} // namespace sofa

#endif

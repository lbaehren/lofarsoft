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
// Author: Pierre-Jean Bensoussan, INRIA-LIFL, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
#include <sofa/component/odesolver/MatrixStaticSolver.h>
//#include "Sofa/Core/IntegrationGroup.h"
#include <sofa/core/ObjectFactory.h>
#include <sofa/defaulttype/NewMatSofaMatrix.h>
#include <sofa/defaulttype/NewMatSofaVector.h>
#include <math.h>
#include <iostream>
#include <stdio.h>


using std::cerr;
using std::endl;

namespace sofa
{

namespace component
{

namespace odesolver
{

using namespace sofa::defaulttype;
using namespace core::componentmodel::behavior;

MatrixStaticSolver::MatrixStaticSolver()
{
    mat = new NewMatSofaMatrix();
    updateMatrix = true;

    nbRow = 0;
    nbCol = 0;
}

void MatrixStaticSolver::solve(double dt)
{
    //objectmodel::BaseContext* group = getContext();
    OdeSolver* group = this;
    MultiVector f(group, VecId::force());
    MultiVector pos(group, VecId::position());
    MultiVector vel(group, VecId::velocity());
    bool printLog = f_printLog.getValue();

    if( printLog )
    {
        cerr<<"MatrixStaticSolver, dt = "<< dt <<endl;
        cerr<<"MatrixStaticSolver, initial x = "<< pos <<endl;
        cerr<<"MatrixStaticSolver, initial v = "<< vel <<endl;
    }

    if (this->updateMatrix)
    {
        group->computeForce(f);
        group->getMatrixDimension(&nbRow, &nbCol);
        mat->resize(nbRow, nbCol);

        group->computeMatrix(mat);
        group->computeOpVector(mat->getOpVect());
        mat->solveInResVect();

        group->matResUpdatePosition(mat->getResVect());

        updateMatrix = false;

        FILE *file = fopen("matrix.txt", "w");

        for (unsigned int i=0; i<nbRow; i++)
        {
            for (unsigned int j=0; j<nbCol; j++)
                fprintf(file, "%.1f ",mat->element(i, j));
            fprintf(file, "\n");
        }

        fclose(file);

        file = fopen("vector.txt", "w");

        for (unsigned int j=0; j<nbCol; j++)
            fprintf(file, "%.1f ", mat->getOpVect()->element(j));

        fclose(file);
    }



    /*
    	for (unsigned int j=0; j<nbCol; j++)
    		std::cout << mat->getOpVect()->element(j) << " ";
    */
}

SOFA_DECL_CLASS(MatrixStatic)

    int MatrixStaticSolverClass = core::RegisterObject("Solver using a dense matrix")
    .add< MatrixStaticSolver >();

} // namespace odesolver

} // namespace component

} // namespace sofa


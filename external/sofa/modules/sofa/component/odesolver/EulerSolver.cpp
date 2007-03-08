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
#include <sofa/component/odesolver/EulerSolver.h>
#include <sofa/core/ObjectFactory.h>
#include <math.h>
#include <iostream>

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

void EulerSolver::solve(double dt)
{
    //objectmodel::BaseContext* group = getContext();
    OdeSolver* group = this;
    MultiVector pos(group, VecId::position());
    MultiVector vel(group, VecId::velocity());
    MultiVector acc(group, VecId::dx());
    bool printLog = f_printLog.getValue();

    if( printLog )
    {
        cerr<<"EulerSolver, dt = "<< dt <<endl;
        cerr<<"EulerSolver, initial x = "<< pos <<endl;
        cerr<<"EulerSolver, initial v = "<< vel <<endl;
    }

    computeAcc ( getTime(), acc, pos, vel);
    vel.peq(acc,dt);
    pos.peq(vel,dt);

    if( printLog )
    {
        cerr<<"EulerSolver, acceleration = "<< acc <<endl;
        cerr<<"EulerSolver, final x = "<< pos <<endl;
        cerr<<"EulerSolver, final v = "<< vel <<endl;
    }
}

int EulerSolverClass = core::RegisterObject("A simple time integrator")
    .add< EulerSolver >();

SOFA_DECL_CLASS(Euler)

// helper::Creator<simulation::tree::xml::ObjectFactory, EulerSolver> EulerSolverClass("EulerSolver");
// helper::Creator<simulation::tree::xml::ObjectFactory, EulerSolver> EulerSolverClass2("Euler"); // Previous name for compatibility with existing scenes

} // namespace odesolver

} // namespace component

} // namespace sofa


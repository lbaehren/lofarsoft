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
#ifndef SOFA_SIMULATION_AUTOMATESCHEDULER_EDGE_H
#define SOFA_SIMULATION_AUTOMATESCHEDULER_EDGE_H

#include <sofa/simulation/automatescheduler/EdgeGFX.h>

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

class Node;

class Cond;

//////////
// Edge //
//////////

class Edge : public EdgeGFX 
{
	public:
		Edge(){};
		Edge(Cond *c, simulation::automatescheduler::Node *p, simulation::automatescheduler::Node *n);

		bool evalCond();
		simulation::automatescheduler::Node* getNext();

	private:
		simulation::automatescheduler::Node *prev;
		simulation::automatescheduler::Node *next;
		Cond *cond;
};

//////////
// Cond //
//////////

class Cond
{
	public:
		Cond();
	  
		virtual bool eval();
                virtual ~Cond() {};
};

//////////////
// condExec //
//////////////

class CondExec: public Cond
{

	bool (*myCondPtr)(void);

	public:

		CondExec(){};

		void setAction( bool (*func)(void) ) 
		{
			myCondPtr = func;
		}

		bool eval(){
			return myCondPtr();
		}
};

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa

#endif

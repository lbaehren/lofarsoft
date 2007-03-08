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
#ifndef SOFA_SIMULATION_AUTOMATESCHEDULER_THREADSIMULATION_H
#define SOFA_SIMULATION_AUTOMATESCHEDULER_THREADSIMULATION_H

#include <iostream>
#include <vector>

#include <sofa/core/VisualModel.h>
//#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/simulation/tree/GNode.h>

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

class CPU;

class ThreadSimulation : public virtual core::objectmodel::BaseObject
{
  private:

	static ThreadSimulation *threadSimulation;
	static bool instanceInitialized;

	int numCPU;
	char automateType[100];
	static bool periphInSimulation;
	
  public:
	 ThreadSimulation(void);
	~ThreadSimulation(void){};

	static ThreadSimulation* getInstance();
	static bool initialized() { return instanceInitialized; }
	void initInstance(const char *_automateType, int _numCPU);
	void addPeriph();
    void computeVModelsList(simulation::tree::GNode *root);

	std::vector< CPU* > tabCPU;
	std::vector<core::VisualModel*> vmodels;

	void start();
	void stop();
	void addThread();

	int getNumCPU();

	simulation::tree::GNode *root;
};

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa

#endif

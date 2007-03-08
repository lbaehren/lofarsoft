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
#include <sofa/simulation/automatescheduler/ThreadSimulation.h>
#include <sofa/simulation/automatescheduler/AutomateUtils.h>
#include <sofa/simulation/automatescheduler/StateMachine.h>
#include <sofa/simulation/automatescheduler/ExecBus.h>
#include <sofa/simulation/tree/Simulation.h>
#include <sofa/simulation/automatescheduler/Automate.h>
#include <sofa/simulation/automatescheduler/CPU.h>
#include <sofa/simulation/tree/xml/ObjectFactory.h>

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

ThreadSimulation *ThreadSimulation::threadSimulation = NULL;
bool ThreadSimulation::instanceInitialized  = false;
bool ThreadSimulation::periphInSimulation = false;

ThreadSimulation::ThreadSimulation()
: core::objectmodel::BaseObject()
{

}

// initInstance()

void ThreadSimulation::initInstance(const char *_automateType, int _numCPU)
{
	strcpy(automateType, _automateType);
	numCPU = _numCPU;

	instanceInitialized = true;
}

void ThreadSimulation::addPeriph()
{
	periphInSimulation = true;
}

void ThreadSimulation::computeVModelsList(simulation::tree::GNode *root)
{
	root->getTreeObjects<core::VisualModel>(&vmodels);
}


// getInstance()

ThreadSimulation* ThreadSimulation::getInstance()
{
	if (threadSimulation == NULL)
	{
		threadSimulation = new ThreadSimulation();
		return threadSimulation;
	}
	else if (!instanceInitialized)
	{
		std::cerr << "Trying to get ThreadSimulation Instance before initializing\n";
		return NULL;
	}
	else
	{
		return threadSimulation;
	}
}


void ThreadSimulation::start()
{
	printf("Start simulation with %d threads\n", numCPU);

	ExecBus::getInstance()->initInstance(StateMachine::getInstance());

    Automate::getInstance()->load(automateType, periphInSimulation);

	for(int i = 0; i < numCPU; i++)
	{
        tabCPU.push_back( new CPU(ExecBus::getInstance() ) );
	}

	if (periphInSimulation)
	{
        tabCPU.push_back(new CPU(ExecBus::getInstance(), PERIPH));
		numCPU++;
	}

	std::cout << "Computing ......\n";

// 	if (GUI!=NULL)
// 	{
// 		// Update GUI
// 		char buf[50];
// 		sprintf(buf, "%d Threads", numCPU + 1);
// 		GUI->threadLabel->value(buf);
// 	}

	for(int i = 0; i < numCPU; i++)
	{
		tabCPU[i]->start();
	}
}


void ThreadSimulation::stop()
{
	for(int i = 0; i < (int) tabCPU.size(); i++)
	{
		tabCPU[i]->finalize();
	}
}


void ThreadSimulation::addThread()
{
	tabCPU.push_back( new CPU( ExecBus::getInstance() ) );
	numCPU++;
	tabCPU[numCPU - 1]->start();
}


int ThreadSimulation::getNumCPU()
{
	return numCPU;
}

using namespace sofa::core::objectmodel;
using namespace sofa::defaulttype;

void create(ThreadSimulation*& obj, simulation::tree::xml::ObjectDescription* arg)
{
	if (!ThreadSimulation::initialized())
		ThreadSimulation::getInstance()->initInstance(arg->getAttribute("autoType","simpleMonoFrequency"),atoi(arg->getAttribute("nbThreads", "2")) - 1);

	obj = ThreadSimulation::getInstance();
}

SOFA_DECL_CLASS(ThreadSimulation)

Creator<simulation::tree::xml::ObjectFactory, ThreadSimulation> ThreadSimulationClass("ThreadSimulation");

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa

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
#ifndef SOFA_SIMULATION_AUTOMATESCHEDULER_CPU_H
#define SOFA_SIMULATION_AUTOMATESCHEDULER_CPU_H

#ifndef WIN32	
# include <pthread.h>
#endif

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

class ExecBus;

enum CPU_STATUS
{
	CPU_BLOCKED = 1,
	CPU_EXEC    = 2,
	CPU_NOEXEC  = 3,
};

enum { PU_NAME_LENGTH = 32 };

enum CPU_TYPE
{
	STANDARD,
	PERIPH
};

//class ExecBus;


/// A processing unit is a Thread.
class PU
{
public:
	int status;

protected:

#ifndef WIN32
	pthread_t threadID;
#else
	uintptr_t threadID;
#endif

	static int sid;
	int id;
	char name[PU_NAME_LENGTH];

public:
	PU();
	virtual ~PU() {}
	virtual void start() = 0;
	virtual void init() = 0;
	virtual void finalize() = 0;
};


class CPU : public PU
{
	ExecBus *xbus;

public:
	CPU(ExecBus *xb, CPU_TYPE _type=STANDARD);

#ifdef WIN32
	static void loop(void *sarg);
#else 
	static void* loop (void *sarg);
#endif

	void start();
	void finalize();
	void init();

	CPU_TYPE type;
};

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa

#endif

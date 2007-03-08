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
#ifndef SOFA_SIMULATION_AUTOMATESCHEDULER_AUTOMATE_H
#define SOFA_SIMULATION_AUTOMATESCHEDULER_AUTOMATE_H

#include <vector>
#include <sofa/simulation/tree/GNode.h>

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

class Node;


class Automate
{
  public:
    class DrawCB
    {
    public:
        virtual ~DrawCB() {}
        virtual void drawFromAutomate() = 0;
    };

	static Automate *automate;

    static DrawCB* drawCB;

//    void drawCode(void);

	Node *exitCompteur(void);
	
  public:
	 Automate(void);
	~Automate(void){};

	static Automate* getInstance();

	static DrawCB* getDrawCB() { return drawCB; }
	static void setDrawCB(DrawCB* fn) { drawCB = fn; }
	void execDrawCB(void);

	void load(char *_automateType, bool _periphInSimulation);

	std::vector<Node*> tabNodes;
};

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa

#endif

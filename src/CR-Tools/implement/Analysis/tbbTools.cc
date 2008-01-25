/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <Analysis/tbbTools.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  tbbTools::tbbTools ()
  {;}
    
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  tbbTools::~tbbTools ()
  {
    destroy();
  }
  
  void tbbTools::destroy ()
  {;}  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool tbbTools::meanFPGAtrigger(Vector<Double>, int level, int start, int stop, int window
				 Vector<Int> &index, Vector<Int> &sum, Vector<Int> &width, 
				 Vector<Int> &peak){
    
  };
  

} // Namespace CR -- end

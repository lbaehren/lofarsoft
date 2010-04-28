/***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sven Duscha (sduscha@mpa-garching.mpg.de)                             *
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

#ifndef RM_PARALLEL_H
#define RM_PARALLEL_H

#include <iostream>
#include <sys/types.h>
#include <sys/sysctl.h>

namespace RM {
  
  /*!
    \class parallel
    
    \ingroup RM
    
    \brief Parallelization functions for RM-Synthesis
    
    \author Sven Duscha
    \author Lars B&auml;hren
    
    \date 03.09.2009.

    <h3>Prerequisite</h3>

    - http://www.gsp.com/cgi-bin/man.cgi?section=3&topic=sysctl

    The string and integer information available for the CTL_HW level is detailed
    below. The changeable column shows whether a process with appropriate
    privilege may change the value.
    
    <table border=0>
      <tr>
        <td>Second level name</td>
        <td>Type</td>
        <td>Changeable</td>
      </tr>
      <tr>
        <td>HW_MACHINE</td>
        <td>string</td>
        <td>no</td>
      </tr>
      <tr>
        <td>HW_MODEL</td>
        <td>string</td>
        <td>no</td>
      </tr>
      <tr>
        <td>HW_NCPU</td>
        <td>integer</td>
        <td>no</td>
      </tr>
      <tr>
        <td>HW_BYTEORDER</td>
        <td>integer</td>
        <td>no</td>
      </tr>
      <tr>
        <td>HW_PHYSMEM</td>
        <td>integer</td>
        <td>no</td>
      </tr>
      <tr>
        <td>HW_USERMEM</td>
        <td>integer</td>
        <td>no</td>
      </tr>
      <tr>
        <td>HW_PAGESIZE</td>
        <td>integer</td>
        <td>no</td>
      </tr>
      <tr>
        <td>HW_FLOATINGPOINT</td>
        <td>integer</td>
        <td>no</td>
      </tr>
      <tr>
        <td>HW_MACHINE_ARCH</td>
        <td>string</td>
        <td>no</td>
      </tr>
    </table>

    <h3>Synopsis</h3>
    
    This class contains high-level RM-Synthesis functions that parallelize the
    computation in the data domain (trivial parallelization), using OpenMP
    functions.
  */
  class parallel
  {
  private:
    
    //! Number of cpus found on system
    unsigned int numcpus;
    //! Number of cores found on system (if using OpenMP)
    unsigned int nofCores_p;
    //! Available (physical) memory on the machine
    unsigned int availmem;
    //! Number of threads to parallelize computiation into
    unsigned int nofThreads_p;
    
  public:

    // === Construction =========================================================

    //! Default constructor
    parallel ();
	  
	 //! Destructor
	 ~parallel ();

    // === Parameter access =====================================================

    //! Get the number of CPU core available
    inline unsigned int nofCores () const {
      return nofCores_p;
    }

    //! Get number of threads the computation is working on
    inline unsigned int nofThreads () const {
      return nofThreads_p;
    }

    // === Methods ==============================================================
    
    //! Get the number of available CPUs
    unsigned int getAvailCPUs();
    //! Determine the amount of physical memory
    unsigned long getPhysmem();

    static int nofAllowedProcesses ();
    
    //! Provide a summary of the internal status
    inline void summary () {
      summary (std::cout);
    }
    
    //! Provide a summary of the internal status
    void summary (std::ostream &os);

  private:
    
    //! Determine number of CPU cores available
    unsigned int getNofCores ();
    //! Get number of threads the computation is working on
    unsigned int getNofThreads ();
    
  };  //  END -- class parallel
  
}  // END -- namespace RM

#endif

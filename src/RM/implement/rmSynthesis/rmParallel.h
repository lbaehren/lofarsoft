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

#include <sys/types.h>
#include <sys/sysctl.h>

namespace RM {
  
  /*!
    \class parallel
    
    \ingroup RM
    
    \brief Parallelization functions for RM-Synthesis
    
    \author Sven (sduscha@mpa-garching.mpg.de)
    
    \date 03.09.2009.
    
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
    unsigned int numcores;
    //! Available (physical) memory on the machine
    unsigned int availmem;
    //! Number of threads to parallelize computiation into
    unsigned int numthreads;
    
  public:
    
    //! Get the number of available CPUs
    unsigned int getAvailCPUs();
    //! Determine number of CPU cores available
    unsigned int getNumCores();
    //! Determine the amount of physical memory
    unsigned long getPhysmem();
    //! Get number of threads the computation is working on
    unsigned int getNumThreads();

  };  //  END -- class parallel
  
}  // END -- namespace RM

#endif

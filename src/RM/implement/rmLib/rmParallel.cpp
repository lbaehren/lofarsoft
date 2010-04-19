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

#include <rmParallel.h>

namespace RM {

  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                     parallel
  
  parallel::parallel ()
  {
    numcpus      = 0;
    nofCores_p   = getNofCores();
    availmem     = 0;
    nofThreads_p = 0;
  }

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                 getAvailCPUs
  
  /*!
    \brief Get the number of available CPUs in the system
    
    \return numcpus - number of CPUs available in the system
  */
  unsigned int parallel::getAvailCPUs()
  {
    int numCPU=0;
    int mib[4];
    size_t len; 

#ifdef HAVE_SYS_SYSCTL_H    
    // set the mib for hw.ncpu 
    mib[0] = CTL_HW;
    mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;
    
    // get the number of CPUs from the system 
    sysctl(mib, 2, &numCPU, &len, NULL, 0);
    
    if( numCPU < 1 ) {
      mib[1] = HW_NCPU;
      sysctl( mib, 2, &numCPU, &len, NULL, 0 );
      
      if( numCPU < 1 ) {
	numCPU = 1;
      }
    }	
    //cout << "Number of processors (OpenMP) = " omp_get_num_procs() << endl;
    // debug: openMP functions does not work?
#endif
    
    return numCPU;	
  }
  
  //_____________________________________________________________________________
  //                                                                  getNofCores
  
  /*!
    \brief Determine the number of cores in the system
    
    \return numcores - number of CPU cores present in the system
  */
  unsigned int parallel::getNofCores ()
  {
    int mib[2];
    size_t len;
    unsigned int numcores (1);
    
#ifdef HAVE_SYS_SYSCTL_H
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    len    = sizeof(numcores);
    
    if((sysctl(mib, 2, &numcores, &len, NULL, NULL))==-1) {
      throw "parallel::getNumberCores sysctl failed";
    }
#endif
    
    return numcores;
  }
  
  //_____________________________________________________________________________
  //                                                                getNofThreads
  
  unsigned int parallel::getNofThreads ()
  {
    unsigned int nofThreads (0);

    return nofThreads;
  }

  //_____________________________________________________________________________
  //                                                                   getPhysmem
  
  /*!
    \brief Determine the amount of physical memory in the system
    
    \return physmem - size of physical memory in bytes
  */
  unsigned long parallel::getPhysmem()
  {
    unsigned int physmem = 0;

#ifdef HAVE_SYS_SYSCTL_H
    size_t len           = sizeof(physmem);
    static int mib[2]    = {CTL_HW, HW_PHYSMEM};
    
    if((sysctl(mib, 2, &physmem, &len, NULL, 0)) == -1) {
      throw "availmem sysctl failed";
    }
#endif
    
    // Return amount of physical memory
    return physmem;
  }
    
  //_____________________________________________________________________________
  //                                                                      summary
  
  /*
    \param os -- Output stream to which the summary is written.
  */
  void parallel::summary (std::ostream &os)
  {
    os << "[parallel] Summary of internal parameters" << std::endl;
    
    os << "-- nof. CPUs       = " << numcpus      << std::endl;
    os << "-- nof. cores      = " << nofCores_p   << std::endl;
    os << "-- Physical memory = " << availmem     << std::endl;
    os << "-- nof. threads    = " << nofThreads_p << std::endl;

  }
  
  
}  // END -- namespace RM

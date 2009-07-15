
## ==============================================================================
##
##  Some generic system inspection
##
## ==============================================================================

## Mute the output of the find scripts
set (FFTW3_FIND_QUIETLY     YES )
set (GFORTRAN_FIND_QUIETLY  YES )
set (GSL_FIND_QUIETLY       YES )
set (HDF5_FIND_QUIETLY      YES )

## Load CMake modules
include (FindFFTW3)
include (FindGFortran)
include (FindGSL)
include (FindHDF5)

##____________________________________________________________________
## Executables

find_program (fink_executable   fink    PATHS ${bin_locations})
find_program (aptget_executable apt-get PATHS ${bin_locations})
find_program (yast_executable   yast    PATHS ${bin_locations})

##____________________________________________________________________
## Determine platform details


## ==============================================================================
##
##  Package lists
##
## ==============================================================================

if (UNIX)
  
  if (APPLE)
    
    if (fink_executable)
      
      ##______________________________________________________________
      ## Set up the list of Fink packages
      
      set (fink_packages
	atlas
	bison
	flex
	fftw3
	fftw3-shlibs
	gfortran-shlibs
	numpy-py26
	qt4-mac
	sip-py26
	pyqt4-py26
	)
      
      if (NOT doxygen_executable)
	list (APPEND fink_packages doxygen)
      endif (NOT doxygen_executable)
      
      if (NOT HAVE_GSL)
	list (APPEND fink_packages gsl)
	list (APPEND fink_packages gsl-shlibs)
      endif (NOT HAVE_GSL)
      
      if (NOT SWIG_FOUND)
	list (APPEND fink_packages swig)
      endif (NOT SWIG_FOUND)
      
      if (NOT wget_executable)
	list (APPEND fink_packages wget)
      endif (NOT wget_executable)
      
      ##______________________________________________________________
      ## Define installation targets for the packages
      
      foreach (fink_package ${fink_packages})
	## define target for individual Fink package
	add_custom_target (fink-${fink_package}
	  COMMAND ${fink_executable} install ${fink_package}
	  WORKING_DIRECTORY ${USG_BUILD}
	  COMMENT "Installating Fink package ${fink_package} ..."
	  )
	## target dependencies
	list (APPEND fink-packages-depends fink-${fink_package})
      endforeach (fink_package)
      
      ## Master target to install all of the above Fink packages
      add_custom_target (fink-packages
	COMMAND ${fink_executable} remove hdf5 hdf5-shlibs
	WORKING_DIRECTORY ${USG_BUILD}
	COMMENT "Installating Fink packages ..."
	)
      add_dependencies (fink-packages ${fink-packages-depends})
      
    else (fink_executable)
      message (STATUS "Unable to install Fink packages!")
    endif (fink_executable)
    
  else (APPLE)

    ## Set up the list of Debian packages to install through apt-get

    set (debian_packages
      libfreetype6-dev
      libpng12-dev
    )

  endif (APPLE)

endif (UNIX)
  
## ==============================================================================
##
##  Wrapper for system-dependent package managers
##
## ==============================================================================

##
## This is quick-and dirty wrapper for the wcslib.
## At a later stage we would probably want to use
## system-wide/USG version of the wcslib and a more
## robust wrapper (e.g. boost::python instead of swig)
##

include(FindPythonLibs)
include(FindSWIG)
include(UseSWIG)

file(GLOB libwcs_sources wcs/*.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/wcs
	            ${PYTHON_INCLUDE_PATH})
set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_SOURCE_DIR})

swig_add_module(wcslib python wcslib.i ${libwcs_sources})

  ## create symlink for the .so par of the module
get_target_property(so_path _wcslib LOCATION)
add_custom_command(TARGET _wcslib POST_BUILD
		   COMMAND ln -sf ${so_path} ${CMAKE_SWIG_OUTDIR})

  ## and remove it while cleaning
get_directory_property(clean_files ADDITIONAL_MAKE_CLEAN_FILES)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
       "${clean_files};${CMAKE_SWIG_OUTDIR}/wcslib.py;${CMAKE_SWIG_OUTDIR}/_wcslib.so")

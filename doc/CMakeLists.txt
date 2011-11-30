
## ==============================================================================
##
##  Variable definitions/settings
##
## ==============================================================================

set ( DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/share/lus/doc" )

## ==============================================================================
##
##  Documentation for build tools
##
## ==============================================================================

## Generate CMake master index matching installed version

execute_process (
  COMMAND ${CMAKE_COMMAND} --help-html
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  OUTPUT_FILE CMake.html
  )

## Generate CTest master index matching installed version

execute_process (
  COMMAND ${CMAKE_CTEST_COMMAND} --help-html
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  OUTPUT_FILE CTest.html
  )

## ==============================================================================
##
##  Documentation generation
##
## ==============================================================================

if (DOXYGEN_EXECUTABLE)

  if (DOXYGEN_GENERATE_LATEX)
    ## Locate required executables
    find_program (latex_executable      latex      PATHS ${bin_locations})
    find_program (makeindex_executable  makeindex  PATHS ${bin_locations})
    find_program (pdflatex_executable   pdflatex   PATHS ${bin_locations})
    ## Is LaTeX available?
    if (latex_executable AND makeindex_executable)
      set (DOXYGEN_GENERATE_LATEX YES)
    else (latex_executable AND makeindex_executable)
      set (DOXYGEN_GENERATE_LATEX NO)
    endif (latex_executable AND makeindex_executable)
    ## Is PDFLaTeX available?
    if (pdflatex_executable)
      set (DOXYGEN_USE_PDFLATEX YES)
    endif (pdflatex_executable)
  else (DOXYGEN_GENERATE_LATEX)
    set (DOXYGEN_GENERATE_LATEX NO)
  endif (DOXYGEN_GENERATE_LATEX)

  ## Create configuration file for Doxygen
  configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    )

  ## Check if output directory exists
  
  if (IS_DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY})
    if (LUS_VERBOSE_CONFIGURE)
      message (STATUS "[LUS] Found output directory for Doxygen documentation")
    endif (LUS_VERBOSE_CONFIGURE)
  else (IS_DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY})
    execute_process (
      COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIRECTORY}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      )
  endif (IS_DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY})
  
  ## Create documentation
  
  add_custom_target (doc
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

endif (DOXYGEN_EXECUTABLE)

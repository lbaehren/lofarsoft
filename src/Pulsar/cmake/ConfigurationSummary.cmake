
## ==============================================================================
##
##                                                    Print configuration summary
##
##  Instead of simply putting this overview into one of the CMakeLists.txt files,
##  by making it a CMake module, this overview can be called from more put a
##  single location within the project, while maintaining a consistent output
##  format. Especially in cases, where variable values might get overwritten
##  by project-internal instructions, having the means to track such changes
##  helps in debugging.
##
## ==============================================================================

message (STATUS "=============================================================="    )
message (STATUS " [Pulsar-Tools] Summary of configuration settings             "    )
message (STATUS "+------------------------------------------------------------+"    )
message (STATUS " System configuration:"                                            )
message (STATUS " .. CMAKE_COMMAND                = ${CMAKE_COMMAND}"               )
message (STATUS " .. CMAKE_SYSTEM                 = ${CMAKE_SYSTEM}"                )
message (STATUS " .. CMAKE_SYSTEM_PROCESSOR       = ${CMAKE_SYSTEM_PROCESSOR}"      )
message (STATUS " Package configuration:"                                           )
message (STATUS " .. PULSAR_SOURCE_DIR            = ${PULSAR_SOURCE_DIR}"           )
message (STATUS " .. PULSAR_BINARY_DIR            = ${PULSAR_BINARY_DIR}"           )
message (STATUS " .. CMAKE_INSTALL_PREFIX         = ${CMAKE_INSTALL_PREFIX}"        )
message (STATUS " .. Enable test programs         = ${PULSAR_ENABLE_TESTING}"       )
message (STATUS " .. Enable compiler warnings     = ${PULSAR_COMPILER_WARNINGS}"    )
message (STATUS " .. Print debugging messages     = ${PULSAR_DEBUGGING_MESSAGES}"   )
message (STATUS " .. Enable verbose configure     = ${PULSAR_VERBOSE_CONFIGURE}"    )
message (STATUS " .. Enable Python bindings       = ${PULSAR_PYTHON_BINDINGS}"      )
message (STATUS "    .. PYTHONPATH                = ${PULSAR_PYTHONPATH}"           )
message (STATUS "    .. LD_LIBRARY_PATH           = ${PULSAR_LD_LIBRARY_PATH}"      )
message (STATUS "    .. ENV:LD_LIBRARY_PATH       = $ENV{LD_LIBRARY_PATH}"          )
message (STATUS " External dependencies:"                                           )
message (STATUS " .. Have CFITSIO                 = ${CFITSIO_FOUND}"               )
message (STATUS "    .. CFITSIO version           = ${CFITSIO_VERSION}"             )
message (STATUS "    .. CFITSIO library           = ${CFITSIO_LIBRARIES}"           )
message (STATUS "    .. CFITSIO library path      = ${CFITSIO_LIBRARY_DIR}"         )
message (STATUS "    .. CFITSIO include path      = ${CFITSIO_INCLUDES}"            )
message (STATUS " .. Have FFTW3                   = ${FFTW3_FOUND}"                 )
message (STATUS "    .. libfftw3                  = ${FFTW3_FFTW3_LIBRARY}"         )
message (STATUS "    .. libfftw3 (static)         = ${FFTW3_FFTW3_STATIC_LIBRARY}"  )
message (STATUS "    .. libfftw3f                 = ${FFTW3_FFTW3F_LIBRARY}"        )
message (STATUS "    .. libfftw3f (static)        = ${FFTW3_FFTW3F_STATIC_LIBRARY}" )
message (STATUS " .. Have MPI                     = ${MPI_FOUND}"                   )
message (STATUS "    .. MPI compiler              = ${MPI_COMPILER}"                )
message (STATUS "    .. MPI library               = ${MPI_LIBRARY}"                 )
message (STATUS "    .. MPI include path          = ${MPI_INCLUDE_PATH}"            )
message (STATUS " .. Have PGPlot                  = ${HAVE_PGPLOT}"                 )
message (STATUS "    .. PGPlot include path       = ${PGPLOT_INCLUDES}"             )
message (STATUS "    .. libpgplot                 = ${PGPLOT_PGPLOT_LIBRARY}"       )
message (STATUS "    .. libcpgplot                = ${PGPLOT_CPGPLOT_LIBRARY}"      )
message (STATUS " .. Have Python                  = ${HAVE_PYTHON}"                 )
message (STATUS "    .. Python version            = ${PYTHON_VERSION}"              )
message (STATUS "    .. Python API version        = ${PYTHON_API_VERSION}"          )
message (STATUS "    .. Python interpreter        = ${PYTHON_EXECUTABLE}"           )
message (STATUS "    .. Include directory         = ${PYTHON_INCLUDES}"             )
message (STATUS "    .. Python library            = ${PYTHON_LIBRARIES}"            )
message (STATUS "    .. NumPy include directory   = ${NUMPY_INCLUDES}"              )
message (STATUS "+------------------------------------------------------------+"    )

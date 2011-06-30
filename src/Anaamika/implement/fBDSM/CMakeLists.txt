# $Id$

add_subdirectory(gsm)
add_subdirectory(io)
add_subdirectory(my_nr)
add_subdirectory(wcs+fits)

if(HAVE_PGPLOT)
  add_subdirectory(with_pgplot)
else(HAVE_PGPLOT)
  add_subdirectory(wout_pgplot)
endif(HAVE_PGPLOT)

if(BUILD_TESTING)
#  add_subdirectory(test)
endif(BUILD_TESTING)

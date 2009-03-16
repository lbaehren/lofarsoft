
## -----------------------------------------------------------------------------
## Boost libraries 

if (BOOST_LIB_DATE_TIME)
  add_subdirectory (date_time)
endif (BOOST_LIB_DATE_TIME)

if (BOOST_LIB_FILESYSTEM)
  add_subdirectory (filesystem)
endif (BOOST_LIB_FILESYSTEM)

if (BOOST_LIB_IO)
  add_subdirectory (io)
endif (BOOST_LIB_IO)

if (BOOST_LIB_IOSTREAMS)
  add_subdirectory (iostreams)
endif (BOOST_LIB_IOSTREAMS)

if (BOOST_LIB_LOGIC)
  add_subdirectory (logic)
endif (BOOST_LIB_LOGIC)

if (BOOST_LIB_MATH)
  add_subdirectory (math)
endif (BOOST_LIB_MATH)

if (BOOST_LIB_MPL)
  add_subdirectory (mpl)
endif (BOOST_LIB_MPL)

if (BOOST_LIB_PROGRAM_OPTIONS)
  add_subdirectory (program_options)
endif (BOOST_LIB_PROGRAM_OPTIONS)

if (BOOST_LIB_PYTHON)
  add_subdirectory (python)
endif (BOOST_LIB_PYTHON)

if (BOOST_LIB_RANGE)
  add_subdirectory (range)
endif (BOOST_LIB_RANGE)

if (BOOST_LIB_REGEX)
  add_subdirectory (regex)
endif (BOOST_LIB_REGEX)

if (BOOST_LIB_SERIALIZATION)
  add_subdirectory (serialization)
endif (BOOST_LIB_SERIALIZATION)

if (BOOST_LIB_SIGNALS)
  add_subdirectory (signals)
endif (BOOST_LIB_SIGNALS)

if (BOOST_LIB_SYSTEM)
  add_subdirectory (system)
endif (BOOST_LIB_SYSTEM)

if (BOOST_LIB_TEST)
  add_subdirectory (test)
endif (BOOST_LIB_TEST)

if (BOOST_LIB_THREAD)
  add_subdirectory (thread)
endif (BOOST_LIB_THREAD)

if (BOOST_LIB_WAVE)
  add_subdirectory (wave)
endif (BOOST_LIB_WAVE)

## -----------------------------------------------------------------------------
## Tests

add_subdirectory (preprocessor)

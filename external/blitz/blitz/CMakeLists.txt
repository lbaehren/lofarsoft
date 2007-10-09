
## blitz/blitz

add_subdirectory (array)
add_subdirectory (generate)
add_subdirectory (gnu)
add_subdirectory (meta)

## source files entering the library

file (GLOB blitz_sources *.cc)


add_library (blitz ${blitz_sources})

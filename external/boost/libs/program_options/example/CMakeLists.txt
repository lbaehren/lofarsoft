
## -----------------------------------------------------------------------------
## Source files to build the library

set (libboost_program_options_example
  custom_syntax
  first
  multiple_sources
  option_groups
  options_description
  real
  regex
  response_file
)

## -----------------------------------------------------------------------------
## Compiler instructions

foreach (example ${libboost_program_options_example})
  ## compiler instructions
  add_executable (${example} ${example}.cpp)
  ## linker instructions
  target_link_libraries (${example} boost_program_options)
endforeach (example)

## -----------------------------------------------------------------------------
## Installation

install (TARGETS ${libboost_program_options_example}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

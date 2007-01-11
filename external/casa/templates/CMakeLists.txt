## Template instantiation for the module libraries of the CASA system

project (templates)

MACRO(GLOB_FILES VAR)
  FOREACH(file ${ARGN})
    LIST(APPEND ${VAR} ${file})
  ENDFOREACH(file)
ENDMACRO(GLOB_FILES)

subdirs (
  casa
  scimath
  tables
  measures
)

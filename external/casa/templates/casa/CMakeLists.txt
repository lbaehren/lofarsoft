set (casaTemplates
  Arrays.cc
  BasicMath.cc
  Containers.cc
  Exceptions.cc
  Quanta.cc
  System.cc
  Utilities.cc
  )

set (casaTemplates "")

foreach (file Arrays.cc BasicMath.cc Containers.cc Exceptions.cc Quanta.cc System.cc Utilities.cc)
  list (APPEND casaTemplates " ${templates_SOURCE_DIR}/casa/${file}")
endforeach(file)

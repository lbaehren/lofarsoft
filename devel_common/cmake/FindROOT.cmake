# - Check for the presence of ROOT
#
# The following variables are set when ROOT is found:
#  HAVE_ROOT       = Set to true, if all components of ROOT have been found.
#  ROOT_INCLUDES   = Include path for the header files of ROOT
#  ROOT_LIBRARIES  = Link these to use ROOT
#  ROOT_LFGLAS     = Linker flags (optional)

## -----------------------------------------------------------------------------
## Check for the header files

find_path (ROOT_INCLUDES tCanvas.h TCint.h TObject.h
  PATHS
  /usr/include
  /usr/local/include
  /opt
  /sw/include
  /sw/share
  PATH_SUFFIXES
  root
  root/include
  NO_DEFAULT_PATH
  )

## -----------------------------------------------------------------------------
## Check for the libraries

set (libs
  Cint
  Cintex
  Core
  EG
  FitPanel
  Foam
  FFTW
  Ged
  Geom
  GeomBuilder
  GeomPainter
  Gpad
  Graf
  Graf3d
  Gui
  GX11
  Hbook
  Hist
  HistPainter
  MathCore
  Matrix
  Minuit
  Net
  Physics
  Postscript
#  Proof
#  ProofGui
#  ProofPlayer
  PyROOT
  RIO
  Rint
  thread
  Tree
  TreePlayer
  TreeViewer
  )

foreach (lib ${libs})
  ## try to locate the library
  find_library (root${lib} ${lib}
    PATHS
    /usr/lib
    /usr/local/lib
    /opt
    /sw/lib
    /sw/share
    PATH_SUFFIXES
    root
    root/lib
    NO_DEFAULT_PATH
    )
  ## if the library could be located, it is added to the list
  if (root${lib})
    list (APPEND ROOT_LIBRARIES ${root${lib}})
  endif (root${lib})
endforeach (lib)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

if (ROOT_INCLUDES AND ROOT_LIBRARIES)
  set (HAVE_ROOT TRUE)
else (ROOT_INCLUDES AND ROOT_LIBRARIES)
  if (NOT ROOT_FIND_QUIETLY)
    if (NOT ROOT_INCLUDES)
      message (STATUS "Unable to find ROOT header files!")
    endif (NOT ROOT_INCLUDES)
    if (NOT ROOT_LIBRARIES)
      message (STATUS "Unable to find ROOT library files!")
    endif (NOT ROOT_LIBRARIES)
  endif (NOT ROOT_FIND_QUIETLY)
endif (ROOT_INCLUDES AND ROOT_LIBRARIES)

if (HAVE_ROOT)
  if (NOT ROOT_FIND_QUIETLY)
    message (STATUS "Found components for ROOT")
    message (STATUS "ROOT_INCLUDES  = ${ROOT_INCLUDES}")
    message (STATUS "ROOT_LIBRARIES = ${ROOT_LIBRARIES}")
  endif (NOT ROOT_FIND_QUIETLY)
else (HAVE_ROOT)
  if (ROOT_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find ROOT!")
  endif (ROOT_FIND_REQUIRED)
endif (HAVE_ROOT)

## ------------------------------------------------------------------------------
## Mark as advanced ...

mark_as_advanced (
  rootCint
  rootCintex
  rootCore
  rootEG
  rootFitPanel
  rootFoam
  rootFFTW
  rootGed
  rootGeom
  rootGeomBuilder
  rootGeomPainter
  rootGpad
  rootGraf
  rootGraf3d
  rootGui
  rootGX11
  rootHbook
  rootHist
  rootHistPainter
  rootMathCore
  rootMatrix
  rootMinuit
  rootNet
  rootPhysics
  rootPostscript
  rootProof
  rootProofGui
  rootProofPlayer
  rootPyROOT
  rootRIO
  rootRint
  rootthread
  rootTree
  rootTreePlayer
  rootTreeViewer
  )

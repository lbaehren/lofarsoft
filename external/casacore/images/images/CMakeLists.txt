
## -----------------------------------------------------------------------------
## libimages

SET (images_modules
  Images
  Wnbt
  )

## collect the source files in the modules

foreach (images_module ${images_modules})
  FILE (GLOB images_${images_module} ${images_module}/*.cc ${images_module}/*.tcc)
  LIST(APPEND images_sources ${images_${images_module}})
endforeach (images_module)

## build the library

add_library (images
  ${images_sources}
)
target_link_libraries (images components coordinates fits mir)

## -----------------------------------------------------------------------------
## Files processed by Bison & Flex
##
## FILES=`ls *.y | sed s/"\.y"//`
## for FILE in $FILES ; { bison -p $FILE -o $FILE.ycc $FILE.y ; }
##
##  FILES=`ls *.l | sed s/"\.l"//`
##  for FILE in $FILES ; { flex -t -P$FILE $FILE.l > $FILE.lcc ; }

set (parser_inputs
  ImageExprGram
  )

if (bison_bin)
  message (STATUS "Processing bison input files...")
  foreach (src ${parser_inputs})
    execute_process (
      WORKING_DIRECTORY ${casacore_SOURCE_DIR}/images/images/Images
      COMMAND bison -p ${src} -o ${src}.ycc ${src}.yy
      TIMEOUT 20
      ERROR_VARIABLE bison_error
      OUTPUT_QUIET
      )
#    if (bison_error)
#      message (SEND_ERROR "There was an error processing ${src}.yy!")
#      message (SEND_ERROR ${bison_error})
#    endif (bison_error)
  endforeach (src)
else (bison_bin)
  message (SEND_ERROR "Unable to process bison input files!")
endif (bison_bin)

if (flex_bin)
  message (STATUS "Processing flex input files...")
  foreach (src ${parser_inputs})
    execute_process (
      WORKING_DIRECTORY ${casacore_SOURCE_DIR}/images/images/Images
      COMMAND flex -t -P${src} ${src}.ll
      TIMEOUT 20
      OUTPUT_FILE ${src}.lcc
      ERROR_VARIABLE flex_error
      OUTPUT_QUIET
      )
    if (flex_error)
      message (SEND_ERROR "There was an error processing ${src}.ll!")
      message (SEND_ERROR ${flex_error})
    endif (flex_error)
  endforeach (src)
else (flex_bin)
  message (SEND_ERROR "Unable to process flex input files!")
endif (flex_bin)

## -----------------------------------------------------------------------------
## Directories containing the test programs

if (casacore_tests)
  ## build up the list of directories ...
  foreach (images_module ${images_modules})
    LIST (APPEND tests "${images_module}/test")
  endforeach (images_module)
  ## ... and register them
  SUBDIRS (${tests})
endif (casacore_tests)

## -----------------------------------------------------------------------------
## Installation targets

install (TARGETS images
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## -----------------------------------------------------------------------------
## Debugging information
##
## ComponentImager
## |-- <images/Images/ComponentImager.h>
## |   `-- <casa/aips.h>
## |-- <images/Images/ImageInterface.h>
## |   |-- <casa/aips.h>
## |   |-- <images/Images/RegionHandler.h>
## |   |   `-- <casa/aips.h>
## |   |-- <images/Images/MaskSpecifier.h>
## |   |   `-- <casa/aips.h>
## |   `-- <images/Images/ImageInfo.h>
## |       |-- <casa/aips.h>
## |       `-- <casa/iosfwd.h>
## `-- <images/Images/ImageInfo.h>
## |   |-- <casa/aips.h>
## |   `-- <casa/iosfwd.h>
##

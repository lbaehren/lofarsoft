
## Libraries

## libsigproc

include_directories (${CMAKE_INSTALL_PREFIX}/include ${PULSAR_BINARY_DIR}/apps/sigproc)

set (sigproc_sources add_channels.c
add_samples.c
aliases.c
angle_split.c
aoftm.c
bandfactors.c
bit.c
bpp2fb.c
cel2gal.c
chan_freqs.c
close_log.c  
decimate_data.c
decimate_header.c
dedisperse_header.c
deg2dms.c
dmdelay.c
dmshift.c
error_message.c
fetch_hdrval.c
ffreq.c
file_exists.c
filterbank_header.c
fold_header.c
fold_data.c
gmrt2fb.c
help.c
ignored_channels.c
inv_cerf.c
indexx.c
lookup.c
machine2prf.c
mjd.c
norm_prof.c
np2.c
nrselect.c
nrutil.c
nrsort.c
nsamples.c
ooty2fb.c
open_file.c
open_log.c  
pack_unpack.c
phcalc.c
process.c
print_version.c
prof_adds.c
prof_ddis.c
prof_sbas.c
prof_sumc.c
prof_sumifs.c
pspm_decode.c
pspm_prof.c
pspm_tstart.c
pspm2fb.c
put.c
pulse.c
random.c
read_aoscan.c
read_block.c
read_header.c
read_polyco.c
recipes.c
scaledata.c
scale_prof.c
scamp2fb.c
send_stuff.c
shift_prof.c
fshift_prof.c
cprofc.c
sizeof_file.c
slalib.c
ssm.c
submean.c
submedian.c
swap_bytes.c
strings_equal.c
typeof_inputdata.c
update_log.c  
uttime.c  
vanvleck.c
vmin.c
vmax.c
wappcorrect.c
wapp_prof.c
write_profiles.c  
write_epn.c
${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c
readtim.c
chkio.c   
ioinf.c 
readPtim.c
smooth.c  
single_ch.c 
whiten.c   
select.c   
open_files.c 
write_pulses.c
thresh_1d.c
head_parse.c 
${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c
wapp2fb.c 
alfa_position.c 
dedisperse_data.c
)

set (sigproc_binaries
    filterbank fake bandpass reader decimate
    sigproc_fold polyco2period sigproc_profile flux splice barycentre
    depolyco sigproc_downsample splitter snrdm blanker 
    header flatten dedisperse clip sigproc_tree extract
) 

set (sigproc_fc_binaries
   pgplotter
)

set (sigproc_scripts
  sigproc_quicklook
  foldsignals
  hunt
  csearch
  accn
  polyco
  makedmlist
  monitor
  makedummy
  )

##__________________________________________________________
## libsigproc

add_library (sigproc STATIC ${sigproc_sources})

target_link_libraries (sigproc
  ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a
  ${FFTW3_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  ${PGPLOT_LIBRARIES}
  ${X11_LIBRARIES}
  )

if (UNIX)
 if (APPLE)
   set_target_properties (sigproc
     PROPERTIES
     SUFFIX .a
     COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -Ddarwin"
     LINK_FLAGS "-static"
     CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
     )
 else (APPLE)
   set_target_properties (sigproc
     PROPERTIES
     SUFFIX .a
     COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -Dlinux"
     LINK_FLAGS "-static"
     CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
     )
 endif (APPLE)
endif (UNIX)

add_dependencies (sigproc  
  fftw_install
  sigproc_fort
  presto_includes
  sigproc_quicklook
  foldsignals
  hunt
  csearch
  accn
  polyco
  makedmlist
  monitor
  makedummy
  )

##__________________________________________________________
## lib fortran

add_subdirectory (fortran)

## ==============================================================================
##
##  Executables
##
## ==============================================================================

##__________________________________________________________
## Build instructions for executables

add_executable (filterbank
  filterbank.c head_parse.c wapp2fb.c
  )

add_executable (extract
  extract.c
  )

add_executable (fake
  fake.c
  )

add_executable (header
  header.c
  )

add_executable (bandpass
  bandpass.c
  )

add_executable (reader
  reader.c
  )

add_executable (decimate
  decimate.c
  )

add_executable (dedisperse
  dedisperse.c
  )

add_executable (sigproc_fold
  fold.c
  )

add_executable (polyco2period
  polyco2period.c
  )

add_executable (sigproc_profile
  profile.c
  )

add_executable (flux
  flux.c
  )

add_executable (splice
  splice.c
  )

add_executable (barycentre
  barycentre.c
  )

add_executable (depolyco
  depolyco.c
  )

add_executable (flatten
  flatten.c
  )

add_executable (clip
  clip.c
  )

add_executable (sigproc_downsample
  downsample.c
  )

add_executable (splitter
  splitter.c
  )

add_executable (snrdm
  snrdm.c
  )

add_executable (blanker
  blanker.c
  )

add_executable (sigproc_tree
  tree.c
  )

add_executable (pgplotter
  pgplotter.c
  )

## linker instructions

foreach (src ${sigproc_binaries})
if (UNIX)
 if (APPLE)
  set_target_properties (${src}
    PROPERTIES
    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -Ddarwin"
    CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
    CMAKE_SHARED_LINKER_FLAGS ""
  )
 else (APPLE)
  set_target_properties (${src}
    PROPERTIES
    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64  -Dlinux"
    CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
    CMAKE_SHARED_LINKER_FLAGS ""
  )
 endif (APPLE)
endif (UNIX)
  target_link_libraries (${src} sigproc sigproc_fort ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a ${CFITSIO_LIBRARIES} ${libm})
endforeach (src)


foreach (src ${sigproc_fc_binaries})
  set_target_properties (${src}
    PROPERTIES
    LINKER_LANGUAGE Fortran
    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
    CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
    CMAKE_SHARED_LINKER_FLAGS ""
  )
  target_link_libraries (${src} sigproc sigproc_fort ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a ${CFITSIO_LIBRARIES} ${libm} ${PGPLOT_CPGPLOT_LIBRARY})
endforeach (src)

set (CMAKE_Fortran_COMPILER g77)

IF(NOT CMAKE_Fortran_LINK_EXECUTABLE)
  SET(CMAKE_Fortran_LINK_EXECUTABLE
    "<CMAKE_Fortran_COMPILER> <CMAKE_Fortran_LINK_FLAGS> <LINK_FLAGS> <FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")
ENDIF(NOT CMAKE_Fortran_LINK_EXECUTABLE)

if (IS_DIRECTORY "${PULSAR_BINARY_DIR}/apps/sigproc/scripts")
  message (STATUS "[PULSAR] Sigproc scripts location ${PULSAR_BINARY_DIR}/apps/sigproc/scripts exists!")
else (IS_DIRECTORY "${PULSAR_BINARY_DIR}/apps/sigproc/scripts")
  message (STATUS "[PULSAR] Sigproc scripts location ${PULSAR_BINARY_DIR}/apps/sigproc/scripts does not exist!")
  message (STATUS "[PULSAR] Creating Sigproc scripts location ${PULSAR_BINARY_DIR}/apps/sigproc/scripts")
  EXECUTE_PROCESS( COMMAND mkdir -p ${PULSAR_BINARY_DIR}/apps/sigproc/scripts)
endif (IS_DIRECTORY "${PULSAR_BINARY_DIR}/apps/sigproc/scripts")

add_custom_target (sigproc_quicklook
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/sigproc_quicklook ${PULSAR_BINARY_DIR}/apps/sigproc/scripts/sigproc_quicklook
  COMMAND echo "\\#!"`which csh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/sigproc_quicklook
  COMMAND cat quicklook.csh >>  ${CMAKE_INSTALL_PREFIX}/bin/sigproc_quicklook
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/sigproc_quicklook
  COMMAND cp  ${CMAKE_INSTALL_PREFIX}/bin/sigproc_quicklook ${PULSAR_BINARY_DIR}/apps/sigproc/scripts/sigproc_quicklook
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/sigproc_quicklook"
 )

add_custom_target (foldsignals
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/foldsignals
  COMMAND echo "\\#!"`which csh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/foldsignals
#  COMMAND echo "set bin = \\(${CMAKE_INSTALL_PREFIX}\\)" >>  ${CMAKE_INSTALL_PREFIX}/bin/foldsignals
  COMMAND echo "set bin = ${CMAKE_INSTALL_PREFIX}" >>  ${CMAKE_INSTALL_PREFIX}/bin/foldsignals
  COMMAND cat foldsignals.csh >>  ${CMAKE_INSTALL_PREFIX}/bin/foldsignals
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/foldsignals
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/foldsignals"
 )

add_custom_target (hunt
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/hunt
  COMMAND echo "\\#!"`which csh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/hunt
  COMMAND echo "set bin = ${CMAKE_INSTALL_PREFIX}" >>  ${CMAKE_INSTALL_PREFIX}/bin/hunt
  COMMAND cat hunt.csh >>  ${CMAKE_INSTALL_PREFIX}/bin/hunt
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/hunt
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/hunt"
 )

add_custom_target (csearch
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/csearch
  COMMAND echo "\\#!"`which csh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/csearch
  COMMAND echo "set bin = ${CMAKE_INSTALL_PREFIX}" >>  ${CMAKE_INSTALL_PREFIX}/bin/csearch
  COMMAND cat csearch.csh >>  ${CMAKE_INSTALL_PREFIX}/bin/csearch
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/csearch
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/csearch"
 )

add_custom_target (accn
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/accn
  COMMAND echo "\\#!"`which csh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/accn
  COMMAND echo "set bin = ${CMAKE_INSTALL_PREFIX}" >>  ${CMAKE_INSTALL_PREFIX}/bin/accn
  COMMAND cat accn.csh >>  ${CMAKE_INSTALL_PREFIX}/bin/accn
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/accn
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/accn"
 )

add_custom_target (polyco
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/polyco ${PULSAR_BINARY_DIR}/apps/sigproc/scripts/polyco
  COMMAND echo "\\#!"`which tclsh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/polyco
  COMMAND cat polyco.tcl >>  ${CMAKE_INSTALL_PREFIX}/bin/polyco
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/polyco
  COMMAND cp ${CMAKE_INSTALL_PREFIX}/bin/polyco ${PULSAR_BINARY_DIR}/apps/sigproc/scripts/polyco
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/polyco"
 )

add_custom_target (makedmlist
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/makedmlist
  COMMAND echo "\\#!"`which tclsh` | sed 's/ //g' >  ${CMAKE_INSTALL_PREFIX}/bin/makedmlist
  COMMAND cat makedmlist.tcl >>  ${CMAKE_INSTALL_PREFIX}/bin/makedmlist
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/makedmlist
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/makedmlist"
 )

add_custom_target (monitor
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/monitor
  COMMAND mkdir -p ${PULSAR_BINARY_DIR}/apps/sigproc/scripts
  COMMAND cp ${PULSAR_SOURCE_DIR}/apps/sigproc/mkmonitor.csh ${PULSAR_BINARY_DIR}/apps/sigproc/scripts
  COMMAND chmod +x ${PULSAR_BINARY_DIR}/apps/sigproc/scripts/mkmonitor.csh
  COMMAND cd ${PULSAR_BINARY_DIR}/apps/sigproc/scripts
  COMMAND ${PULSAR_BINARY_DIR}/apps/sigproc/scripts/mkmonitor.csh > ${CMAKE_INSTALL_PREFIX}/bin/monitor
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/monitor
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/monitor"
 )

add_custom_target (makedummy
  COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/bin/makedummy
  COMMAND cat ${PULSAR_SOURCE_DIR}/apps/sigproc/makedummy.csh > ${CMAKE_INSTALL_PREFIX}/bin/makedummy
  COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/makedummy
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Creating script ${CMAKE_INSTALL_PREFIX}/bin/makedummy"
 )
 
find_package(LATEX)

add_custom_target (sigproc_helpdocs
#  ADD_DEPENDENCIES ( sigproc_helpdocs
#  sigproc
#  filterbank
#  header
#  bandpass
#  reader decimate
#  splice
#  dedisperse
#  sigproc_fold
#  sigproc_profile
#  flux
#  barycentre
#  sigproc_quicklook
#  foldsignals
#  hunt
#  csearch
#  accn
#  polyco
#  makedmlist
#  monitor
#  makedummy
#  )
  COMMAND cp ${PULSAR_SOURCE_DIR}/apps/sigproc/help.csh ${PULSAR_BINARY_DIR}/apps/sigproc/help.csh
  COMMAND ./help.csh filterbank > /dev/null
  COMMAND ./help.csh fake > /dev/null
  COMMAND ./help.csh header > /dev/null
  COMMAND ./help.csh bandpass > /dev/null
  COMMAND ./help.csh reader > /dev/null
  COMMAND ./help.csh decimate > /dev/null
  COMMAND ./help.csh splice > /dev/null
  COMMAND ./help.csh dedisperse > /dev/null
  COMMAND ./help.csh sigproc_fold > /dev/null
  COMMAND ./help.csh sigproc_profile > /dev/null
  COMMAND ./help.csh flux > /dev/null
  COMMAND ./help.csh scripts/polyco > /dev/null
  COMMAND ./help.csh barycentre > /dev/null
  COMMAND ./help.csh scripts/sigproc_quicklook > /dev/null
  COMMAND cp ${PULSAR_SOURCE_DIR}/apps/sigproc/sigproc.tex ${PULSAR_BINARY_DIR}/apps/sigproc/sigproc.tex
  COMMAND cp ${PULSAR_SOURCE_DIR}/apps/sigproc/version.history ${PULSAR_BINARY_DIR}/apps/sigproc/version.history
#  COMMAND ${LATEX_COMPILER} sigproc
  COMMAND ${PDFLATEX_COMPILER} sigproc
  COMMAND cp sigproc.pdf ${CMAKE_INSTALL_PREFIX}/doc/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/sigproc
  COMMENT "Creating help files for sigproc, including ${CMAKE_INSTALL_PREFIX}/doc/sigproc.pdf"
 )

find_package(BISON)

if (EXISTS "${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c")
  message (STATUS "[PULSAR] Yacc results file ${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c exists!")
else (EXISTS "${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c")
  message (STATUS "[PULSAR] Creating yacc results ${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c")
  EXECUTE_PROCESS( COMMAND ${BISON_EXECUTABLE} -vdt -o ${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c ${PULSAR_SOURCE_DIR}/apps/sigproc/mkheader.y)
endif (EXISTS "${PULSAR_BINARY_DIR}/apps/sigproc/y.tab.c")

find_package(FLEX)

if (EXISTS "${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c")
  message (STATUS "[PULSAR] Yacc results file ${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c exists!")
else (EXISTS "${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c")
  message (STATUS "[PULSAR] Creating lex results ${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c")
  EXECUTE_PROCESS( COMMAND ${FLEX_EXECUTABLE} -o ${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c ${PULSAR_SOURCE_DIR}/apps/sigproc/mkheader.l)
endif (EXISTS "${PULSAR_BINARY_DIR}/apps/sigproc/mkheaderlex.c")


## ==============================================================================
##
##  Installation
## 
## ==============================================================================

install (
  TARGETS sigproc
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install (
  TARGETS ${sigproc_binaries} ${sigproc_fc_binaries} 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## ==============================================================================
##
##  Test programs
## 
## ==============================================================================

##add_executable (testbinresp
##  ${sigproc_SOURCE_DIR}/tests/testbinresp.c
##  ${PLOT2DOBJS}
##  )


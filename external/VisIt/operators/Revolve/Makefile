##
## Programs and options...
##
include /data_vobs/VisIt/include/make-variables
TOPDIR=/data_vobs/VisIt


##
## Libraries and includes...
##
VTK_INCLUDE= \
  -I/data_vobs/VisIt/include/vtk \
  -I/data_vobs/VisIt/include/vtk/Common \
  -I/data_vobs/VisIt/include/vtk/Filtering \
  -I/data_vobs/VisIt/include/vtk/Graphics \
  -I/data_vobs/VisIt/include/vtk/Hybrid \
  -I/data_vobs/VisIt/include/vtk/IO \
  -I/data_vobs/VisIt/include/vtk/Imaging \
  -I/data_vobs/VisIt/include/vtk/Rendering \
  -I/data_vobs/VisIt/include/vtk/Utilities
MOC=/data_vobs/VisIt/bin/moc
CXXFLAGS=$(CXXFLAGSORIG) $(QT_CXXFLAGS) $(PY_CXXFLAGS)
CPPFLAGS=$(CPPFLAGSORIG) $(VTK_INCLUDE) -I. -I/data_vobs/VisIt/include -I/data_vobs/VisIt/include/visit
LDFLAGS=$(LDFLAGSORIG) $(PY_LDFLAGS)  -L/data_vobs/VisIt/plugins/operators

##
## Files...
##
PLUGINDIR=operators
PLUGINNAME=RevolveOperator

WIDGETS=QvisRevolveWindow.h
ISRC=RevolvePluginInfo.C
COMMONSRC=RevolvePluginInfo.C RevolveCommonPluginInfo.C RevolveAttributes.C
GSRC=RevolveGUIPluginInfo.C QvisRevolveWindow.C
VSRC=RevolveViewerPluginInfo.C
ESRC=RevolveEnginePluginInfo.C avtRevolveFilter.C
SSRC=RevolveScriptingPluginInfo.C PyRevolveAttributes.C

SRC=$(ISRC) $(COMMONSRC) $(GSRC) $(VSRC) $(ESRC) $(SSRC)


JAVASRC=RevolveAttributes.java
JAVAOBJ=/data_vobs/VisIt/java/llnl/visit/operators/RevolveAttributes.class
JAVAPLUGINFLAGS=-d /data_vobs/VisIt/java -classpath /data_vobs/VisIt/java

SHLIB_FORCED=

ELIBS_FOR_MACOSX_PREBINDING=$(BZIP2_LIBS)
VLIBS_FOR_MACOSX_PREBINDING=$(BZIP2_LIBS)
ILIBS=
GLIBS=-lgui -lmdserverproxy -lviewerproxy -lproxybase -lmdserverrpc -lviewerrpc -lwinutil -ldbatts -lavtexceptions -lstate -lcomm -lmisc -lplugin -lexpr -lparser -lutility -lI$(PLUGINNAME) $(QT_LDFLAGS) $(QT_LIBS) $(QUI_LIBS) $(X_LIBS)
SLIBS=-lstate -lmisc -lcomm -lutility $(PY_LIB) -lI$(PLUGINNAME)
VLIBS=-lpipeline_ser -lplotter_ser -lavtfilters_ser -lavtmath_ser -lavtview -ldbatts -lavtexceptions -lstate -lmisc -lcomm -lexpr -lparser -lutility -lvisit_vtk -llightweight_visit_vtk -lparallel_visit_vtk_ser $(VLIBS_FOR_MACOSX_PREBINDING) -lI$(PLUGINNAME) $(VTK_LIBS)
ESERLIBS=-lpipeline_ser -lplotter_ser -lavtfilters_ser -lavtmath_ser -lavtview -ldbatts -lavtexceptions -lstate -lmisc -lcomm -lexpr -lparser -lutility -lvisit_vtk -llightweight_visit_vtk -lparallel_visit_vtk_ser -lI$(PLUGINNAME) $(ELIBS_FOR_MACOSX_PREBINDING) $(VTK_LIBS)
EPARLIBS=-lpipeline_par -lplotter_par -lavtfilters_par -lavtmath_par -lavtview -ldbatts -lavtexceptions -lstate -lmisc -lcomm -lexpr -lparser -lutility -lvisit_vtk -llightweight_visit_vtk -lparallel_visit_vtk_par -lI$(PLUGINNAME) $(ELIBS_FOR_MACOSX_PREBINDING) $(VTK_LIBS) $(SHLIB_MPI_LIBS)

IDSO=/data_vobs/VisIt/plugins/operators/libIRevolveOperator.so
GDSO=/data_vobs/VisIt/plugins/operators/libGRevolveOperator.so
SDSO=/data_vobs/VisIt/plugins/operators/libSRevolveOperator.so
VDSO=/data_vobs/VisIt/plugins/operators/libVRevolveOperator.so
ESERDSO=/data_vobs/VisIt/plugins/operators/libERevolveOperator_ser.so
EPARDSO=/data_vobs/VisIt/plugins/operators/libERevolveOperator_par.so


DISTRIB=

##
## Derived objects
##
IOBJ=$(ISRC:.C=.o)
GOBJ=$(COMMONSRC:.C=.o) $(GSRC:.C=.o)
SOBJ=$(COMMONSRC:.C=.o) $(SSRC:.C=.o)
VOBJ=$(COMMONSRC:.C=.o) $(VSRC:.C=.o)
MOBJ=$(COMMONSRC:.C=.o) $(MSRC:.C=.o) $(MSPECIFICSRC:.C=_mds.o)
ESEROBJ=$(COMMONSRC:.C=.o) $(ESRC:.C=.o)
EPAROBJ=$(COMMONSRC:.C=.o) $(ESRC:.C=_par.o)

MOCSRC = $(WIDGETS:.h=_moc.C)
MOCOBJ = $(MOCSRC:.C=.o)

##
## Standard targets...
##
all: message $(IDSO) $(GUILIB) $(VIEWERLIB) $(ENGINELIBSER) $(ENGINELIBPAR) $(SCRIPTINGLIB) $(JAVACLASS)

clean:
	$(RM) $(IOBJ) $(COMMONOBJ)
	$(RM) $(GOBJ) $(SOBJ) $(VOBJ) $(MOBJ) $(ESEROBJ) $(EPAROBJ)
	$(RM) $(MOCSRC) $(MOCOBJ)
	$(RM) $(IDSO) $(GDSO) $(SDSO) $(VDSO) $(MDSO) $(ESERDSO) $(EPARDSO)
	$(RM) $(JAVAOBJ)

##
## Other targets...
##
message:
	@echo
	@echo "****************************************************************************"
	@echo "*** Building Revolve Operator Plugin"
	@echo "****************************************************************************"

##
## moc
##
$(MOCSRC) or_no_widgets: $(WIDGETS)
	@rm -f $@
	$(MOC) $(@:_moc.C=.h) > $@


##
## Automatic dependency stuff
##
include /data_vobs/VisIt/include/make-targets

# Defs
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = $(HOME)/prog/bin
DOC_DIR = doc
LIB_DIR = lib
INC_DIR = include

#  ROOT flags
ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)

# STAR libs
L_RADIOLIB = -L$(LIB_DIR)/ -lSTARutil -lRecRadio
I_RADIOLIB = -I$(INC_DIR)/

# include module files
MODULE_STARUTIL = $(OBJ_DIR)/util.o $(OBJ_DIR)/trigger.o $(OBJ_DIR)/calibration.o $(OBJ_DIR)/reconstruction.o 
MODULE_RECRADIO = $(OBJ_DIR)/RecRadioBasic.o $(OBJ_DIR)/RecRadioCalib.o $(OBJ_DIR)/RecRadioTrigger.o $(OBJ_DIR)/RecRadioUtil.o

HEADER = $(SRC_DIR)/util.hh $(SRC_DIR)/trigger.hh $(SRC_DIR)/calibration.hh $(SRC_DIR)/reconstruction.hh 
HEADER_RECRADIO = $(SRC_DIR)/RecRadioBasic.hh $(SRC_DIR)/RecRadioCalib.hh $(SRC_DIR)/RecRadioTrigger.hh $(SRC_DIR)/RecRadioUtil.hh

# C++ flags
CXX           = g++
CXXFLAGS      = -ggdb3 -Wall -fPIC
LD            = g++
FFTW_LIB      = -lfftw3 -lm
LIBS	      = -lpq $(ROOTLIBS) $(ROOTGLIBS) $(FFTW_LIB) $(L_RADIOLIB)
LDFLAGS       = -ggdb3 $(LIBS)
POSTGRESQL	= -I/usr/include/postgresql -I/usr/include/pgsql
INCLUDES      = $(POSTGRESQL) $(I_RADIOLIB)



all: 	libs \
	$(BIN_DIR)/DynamicSpectra $(BIN_DIR)/EventViewer $(BIN_DIR)/SpectraViewer $(BIN_DIR)/RunFlagOut\
	$(BIN_DIR)/RunFlagInDB $(BIN_DIR)/RunFlagSelection $(BIN_DIR)/ChannelStatusControl \
	$(BIN_DIR)/RunFlagMergeGrande $(BIN_DIR)/Beamforming $(BIN_DIR)/RunFlagRecEvent \
	TA AS


########################################################
# Thomas Asch

TA:	$(BIN_DIR)/root2csv $(BIN_DIR)/db2csv $(BIN_DIR)/chplot4root \
	$(BIN_DIR)/timestamp_compare $(BIN_DIR)/MC_timestamp $(BIN_DIR)/TreeConvert\
	$(BIN_DIR)/starTrace $(BIN_DIR)/RunLoop $(BIN_DIR)/OptWindowSize $(BIN_DIR)/CalibrationD30\
	$(BIN_DIR)/ThresholdCalibration $(BIN_DIR)/FitPolarisation $(BIN_DIR)/FitLDF $(BIN_DIR)/FitParFieldStrength

$(OBJ_DIR)/root2csv.o: $(SRC_DIR)/root2csv.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/root2csv.cpp -o $(OBJ_DIR)/root2csv.o
$(BIN_DIR)/root2csv: $(OBJ_DIR)/root2csv.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/root2csv.o -o $(BIN_DIR)/root2csv

$(OBJ_DIR)/db2csv.o: $(SRC_DIR)/db2csv.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/db2csv.cpp -o $(OBJ_DIR)/db2csv.o
$(BIN_DIR)/db2csv: $(OBJ_DIR)/db2csv.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/db2csv.o -o $(BIN_DIR)/db2csv

$(OBJ_DIR)/timestamp_parse.o: $(SRC_DIR)/timestamp_parse.cpp  $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/timestamp_parse.cpp -o $(OBJ_DIR)/timestamp_parse.o
$(BIN_DIR)/timestamp_parse: $(OBJ_DIR)/timestamp_parse.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/timestamp_parse.o -o $(BIN_DIR)/timestamp_parse

$(OBJ_DIR)/chplot4root.o: $(SRC_DIR)/chplot4root.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/chplot4root.cpp -o $(OBJ_DIR)/chplot4root.o
$(BIN_DIR)/chplot4root: $(OBJ_DIR)/chplot4root.o
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/chplot4root.o -o $(BIN_DIR)/chplot4root

$(OBJ_DIR)/timestamp_compare.o: $(SRC_DIR)/timestamp_compare.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/timestamp_compare.cpp -o $(OBJ_DIR)/timestamp_compare.o
$(BIN_DIR)/timestamp_compare: $(OBJ_DIR)/timestamp_compare.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/timestamp_compare.o -o $(BIN_DIR)/timestamp_compare

$(OBJ_DIR)/MC_timestamp.o: $(SRC_DIR)/MC_timestamp.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/MC_timestamp.cpp -o $(OBJ_DIR)/MC_timestamp.o
$(BIN_DIR)/MC_timestamp: $(OBJ_DIR)/MC_timestamp.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/MC_timestamp.o -o $(BIN_DIR)/MC_timestamp

$(OBJ_DIR)/TreeConvert.o: $(SRC_DIR)/TreeConvert.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/TreeConvert.cpp -o $(OBJ_DIR)/TreeConvert.o
$(BIN_DIR)/TreeConvert: $(OBJ_DIR)/TreeConvert.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/TreeConvert.o -o $(BIN_DIR)/TreeConvert

$(OBJ_DIR)/starTrace.o: $(SRC_DIR)/starTrace.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/starTrace.cpp -o $(OBJ_DIR)/starTrace.o
$(BIN_DIR)/starTrace: $(OBJ_DIR)/starTrace.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/starTrace.o -o $(BIN_DIR)/starTrace

$(OBJ_DIR)/DynamicSpectraARG.o: $(SRC_DIR)/DynamicSpectraARG.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/DynamicSpectraARG.cpp -o $(OBJ_DIR)/DynamicSpectraARG.o
$(BIN_DIR)/DynamicSpectraARG: $(OBJ_DIR)/DynamicSpectraARG.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/DynamicSpectraARG.o -o $(BIN_DIR)/DynamicSpectraARG

$(OBJ_DIR)/RunLoop.o: $(SRC_DIR)/RunLoop.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/RunLoop.cpp -o $(OBJ_DIR)/RunLoop.o
$(BIN_DIR)/RunLoop: $(OBJ_DIR)/RunLoop.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/RunLoop.o -o $(BIN_DIR)/RunLoop

$(OBJ_DIR)/EventViewerARG.o: $(SRC_DIR)/EventViewerARG.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/EventViewerARG.cpp -o $(OBJ_DIR)/EventViewerARG.o
$(BIN_DIR)/EventViewerARG: $(OBJ_DIR)/EventViewerARG.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/EventViewerARG.o -o $(BIN_DIR)/EventViewerARG

$(OBJ_DIR)/OptWindowSize.o: $(SRC_DIR)/OptWindowSize.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/OptWindowSize.cpp -o $(OBJ_DIR)/OptWindowSize.o
$(BIN_DIR)/OptWindowSize: $(OBJ_DIR)/OptWindowSize.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/OptWindowSize.o -o $(BIN_DIR)/OptWindowSize

$(OBJ_DIR)/CalibrationD30.o: $(SRC_DIR)/CalibrationD30.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/CalibrationD30.cpp -o $(OBJ_DIR)/CalibrationD30.o
$(BIN_DIR)/CalibrationD30: $(OBJ_DIR)/CalibrationD30.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/CalibrationD30.o -o $(BIN_DIR)/CalibrationD30

$(OBJ_DIR)/ThresholdCalibration.o: $(SRC_DIR)/ThresholdCalibration.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/ThresholdCalibration.cpp -o $(OBJ_DIR)/ThresholdCalibration.o
$(BIN_DIR)/ThresholdCalibration: $(OBJ_DIR)/ThresholdCalibration.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/ThresholdCalibration.o -o $(BIN_DIR)/ThresholdCalibration

$(OBJ_DIR)/FitPolarisation.o: $(SRC_DIR)/FitPolarisation.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/FitPolarisation.cpp -o $(OBJ_DIR)/FitPolarisation.o
$(BIN_DIR)/FitPolarisation: $(OBJ_DIR)/FitPolarisation.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) -lMinuit $(OBJ_DIR)/FitPolarisation.o -o $(BIN_DIR)/FitPolarisation

$(OBJ_DIR)/FitLDF.o: $(SRC_DIR)/FitLDF.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/FitLDF.cpp -o $(OBJ_DIR)/FitLDF.o
$(BIN_DIR)/FitLDF: $(OBJ_DIR)/FitLDF.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) -lMinuit $(OBJ_DIR)/FitLDF.o -o $(BIN_DIR)/FitLDF

$(OBJ_DIR)/FitParFieldStrength.o: $(SRC_DIR)/FitParFieldStrength.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/FitParFieldStrength.cpp -o $(OBJ_DIR)/FitParFieldStrength.o
$(BIN_DIR)/FitParFieldStrength: $(OBJ_DIR)/FitParFieldStrength.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) -lMinuit $(OBJ_DIR)/FitParFieldStrength.o -o $(BIN_DIR)/FitParFieldStrength
########################################################

# Merlin Manewald
MM:
	@(echo "MM: not implemented")

########################################################

# Adrian Schmidt
AS:	$(BIN_DIR)/RunFlagOut_AS


$(OBJ_DIR)/RunFlagOut_AS.o: $(SRC_DIR)/RunFlagOut_AS.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/RunFlagOut_AS.cpp -o $(OBJ_DIR)/RunFlagOut_AS.o
$(BIN_DIR)/RunFlagOut_AS: $(OBJ_DIR)/RunFlagOut_AS.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/RunFlagOut_AS.o -o $(BIN_DIR)/RunFlagOut_AS

########################################################	

libs:	$(LIB_DIR)/libRecRadio.so $(LIB_DIR)/libSTARutil.so

########################################################	


# external lib support 


$(LIB_DIR)/libRecRadio.so: $(MODULE_RECRADIO)
	@echo "*****************************";
	@echo "* Generating libRecRadio.so *";
	@echo "*****************************";
	$(CXX) -shared -o $(LIB_DIR)/libRecRadio.so $(MODULE_RECRADIO) 
	@cp $(SRC_DIR)/RecRadio*.hh $(INC_DIR)/

$(OBJ_DIR)/RecRadioBasic.o: $(SRC_DIR)/RecRadioBasic.cpp $(HEADER_RECRADIO)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/RecRadioBasic.cpp -o $(OBJ_DIR)/RecRadioBasic.o
$(OBJ_DIR)/RecRadioCalib.o: $(SRC_DIR)/RecRadioCalib.cpp $(HEADER_RECRADIO)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/RecRadioCalib.cpp -o $(OBJ_DIR)/RecRadioCalib.o
$(OBJ_DIR)/RecRadioTrigger.o: $(SRC_DIR)/RecRadioTrigger.cpp $(HEADER_RECRADIO)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/RecRadioTrigger.cpp -o $(OBJ_DIR)/RecRadioTrigger.o
$(OBJ_DIR)/RecRadioUtil.o: $(SRC_DIR)/RecRadioUtil.cpp $(HEADER_RECRADIO)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/RecRadioUtil.cpp -o $(OBJ_DIR)/RecRadioUtil.o



########################################################
# general LOPES-STAR-Tools


$(LIB_DIR)/libSTARutil.so: $(MODULE_STARUTIL)
	@echo "*****************************";
	@echo "* Generating libSTARutil.so *";
	@echo "*****************************";
	$(CXX) -shared -o $(LIB_DIR)/libSTARutil.so $(MODULE_STARUTIL) 
	@cp $(SRC_DIR)/*.hh $(INC_DIR)/

$(OBJ_DIR)/util.o: $(SRC_DIR)/util.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/util.cpp -o $(OBJ_DIR)/util.o
$(OBJ_DIR)/trigger.o: $(SRC_DIR)/trigger.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/trigger.cpp -o $(OBJ_DIR)/trigger.o
$(OBJ_DIR)/calibration.o: $(SRC_DIR)/calibration.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/calibration.cpp -o $(OBJ_DIR)/calibration.o
$(OBJ_DIR)/reconstruction.o: $(SRC_DIR)/reconstruction.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/reconstruction.cpp -o $(OBJ_DIR)/reconstruction.o


$(OBJ_DIR)/DynamicSpectra.o: $(SRC_DIR)/DynamicSpectra.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/DynamicSpectra.cpp -o $(OBJ_DIR)/DynamicSpectra.o
$(BIN_DIR)/DynamicSpectra: $(OBJ_DIR)/DynamicSpectra.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/DynamicSpectra.o -o $(BIN_DIR)/DynamicSpectra

$(OBJ_DIR)/EventViewer.o: $(SRC_DIR)/EventViewer.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/EventViewer.cpp -o $(OBJ_DIR)/EventViewer.o
$(BIN_DIR)/EventViewer: $(OBJ_DIR)/EventViewer.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/EventViewer.o -o $(BIN_DIR)/EventViewer

$(OBJ_DIR)/SpectraViewer.o: $(SRC_DIR)/SpectraViewer.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/SpectraViewer.cpp -o $(OBJ_DIR)/SpectraViewer.o
$(BIN_DIR)/SpectraViewer: $(OBJ_DIR)/SpectraViewer.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/SpectraViewer.o -o $(BIN_DIR)/SpectraViewer

$(OBJ_DIR)/RunFlagOut.o: $(SRC_DIR)/RunFlagOut.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/RunFlagOut.cpp -o $(OBJ_DIR)/RunFlagOut.o
$(BIN_DIR)/RunFlagOut: $(OBJ_DIR)/RunFlagOut.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/RunFlagOut.o -o $(BIN_DIR)/RunFlagOut

$(OBJ_DIR)/RunFlagInDB.o: $(SRC_DIR)/RunFlagInDB.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/RunFlagInDB.cpp -o $(OBJ_DIR)/RunFlagInDB.o
$(BIN_DIR)/RunFlagInDB: $(OBJ_DIR)/RunFlagInDB.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/RunFlagInDB.o -o $(BIN_DIR)/RunFlagInDB

$(OBJ_DIR)/RunFlagSelection.o: $(SRC_DIR)/RunFlagSelection.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/RunFlagSelection.cpp -o $(OBJ_DIR)/RunFlagSelection.o
$(BIN_DIR)/RunFlagSelection: $(OBJ_DIR)/RunFlagSelection.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/RunFlagSelection.o -o $(BIN_DIR)/RunFlagSelection

$(OBJ_DIR)/RunFlagRecEvent.o: $(SRC_DIR)/RunFlagRecEvent.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/RunFlagRecEvent.cpp -o $(OBJ_DIR)/RunFlagRecEvent.o
$(BIN_DIR)/RunFlagRecEvent: $(OBJ_DIR)/RunFlagRecEvent.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/RunFlagRecEvent.o -o $(BIN_DIR)/RunFlagRecEvent

$(OBJ_DIR)/RunFlagMergeGrande.o: $(SRC_DIR)/RunFlagMergeGrande.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/RunFlagMergeGrande.cpp -o $(OBJ_DIR)/RunFlagMergeGrande.o
$(BIN_DIR)/RunFlagMergeGrande: $(OBJ_DIR)/RunFlagMergeGrande.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/RunFlagMergeGrande.o -o $(BIN_DIR)/RunFlagMergeGrande

$(OBJ_DIR)/ChannelStatusControl.o: $(SRC_DIR)/ChannelStatusControl.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/ChannelStatusControl.cpp -o $(OBJ_DIR)/ChannelStatusControl.o
$(BIN_DIR)/ChannelStatusControl: $(OBJ_DIR)/ChannelStatusControl.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/ChannelStatusControl.o -o $(BIN_DIR)/ChannelStatusControl
	
$(OBJ_DIR)/Beamforming.o: $(SRC_DIR)/Beamforming.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDES) -c $(SRC_DIR)/Beamforming.cpp -o $(OBJ_DIR)/Beamforming.o
$(BIN_DIR)/Beamforming: $(OBJ_DIR)/Beamforming.o 
	$(LD) $(LDFLAGS) $(ROOTCFLAGS) $(OBJ_DIR)/Beamforming.o -o $(BIN_DIR)/Beamforming
########################################################

clean: doc_clean
	@rm -f $(OBJ_DIR)/*.o $(SRC_DIR)/*.o 
	@rm -f $(LIB_DIR)/*.so $(INC_DIR)/*.hh
	@rm -f $(BIN_DIR)/root2csv $(BIN_DIR)/db2csv
	@rm -f $(BIN_DIR)/timestamp_parse
	@rm -f $(BIN_DIR)/chplot4root
	@rm -f $(BIN_DIR)/timestamp_compare $(BIN_DIR)/MC_timestamp
	@rm -f $(BIN_DIR)/TreeConvert
	@rm -f $(BIN_DIR)/starTrace
	@rm -f $(BIN_DIR)/DynamicSpectra
	@rm -f $(BIN_DIR)/RunLoop
	@rm -f $(BIN_DIR)/DynamicSpectraARG
	@rm -f $(BIN_DIR)/EventViewerARG
	@rm -f $(BIN_DIR)/EventViewer
	@rm -f $(BIN_DIR)/SpectraViewer
	@rm -f $(BIN_DIR)/RunFlagOut
	@rm -f $(BIN_DIR)/RunFlagOut_AS
	@rm -f $(BIN_DIR)/RunFlagInDB
	@rm -f $(BIN_DIR)/RunFlagSelection
	@rm -f $(BIN_DIR)/RunFlagRecEvent
	@rm -f $(BIN_DIR)/RunFlagMergeGrande
	@rm -f $(BIN_DIR)/OptWindowSize
	@rm -f $(BIN_DIR)/ChannelStatusControl
	@rm -f $(BIN_DIR)/Beamforming
	@rm -f $(BIN_DIR)/CalibrationD30
	@rm -f $(BIN_DIR)/ThresholdCalibration
	@rm -f $(BIN_DIR)/FitPolarisation
	@rm -f $(BIN_DIR)/FitLDF
	@rm -f $(BIN_DIR)/FitParFieldStrength
		
		
########################################################

doc: $(DOC_DIR)/html/index.html 
$(DOC_DIR)/html/index.html: $(SRC_DIR)/*.hh $(SRC_DIR)/*.cpp
	@mkdir -p $(DOC_DIR)/html
	@mkdir -p $(DOC_DIR)/latex
	doxygen $(DOC_DIR)/Doxyfile_star_tools.cfg
	
	
doc_clean: 
	@rm -rf $(DOC_DIR)/html
	@rm -rf $(DOC_DIR)/latex
	
pdf: doc
	@(cd $(DOC_DIR)/latex; make refman.pdf)

########################################################

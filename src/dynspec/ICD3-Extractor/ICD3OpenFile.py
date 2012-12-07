#!/usr/bin/python
# -*- coding: utf-8 -*-


#############################
# IMPORT COMMANDS
#############################

import sys, os, glob, commands
import time
import tkFileDialog

from pylab import *
import matplotlib

import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure

from numpy import *
import numpy as np
#import h5py

from Tkinter import *
from dal import *
from tkFileDialog import *

#############################
# OPEN READ AND WRITE METADATA
#############################


class ICD3OpenFile():

    def __init__(self, parent, h5FilePath):
      
        # Attributes of ICD6OpenFile Class
        self.grandparent 	= parent
        self.PATH0 		= h5FilePath
        
        self.SAPSelected	= ''
        self.indexSAPSelected	= ''
        self.observationID	= ''

	self.tStart			= ''
	self.tStop			= ''
	self.tIncrement			= ''	
	self.fMin			= ''
	self.fMax			= ''
	self.nofChannelsPerSubband	= ''        
	self.nofSubbands		= '' 	
	self.nofTimeSample		= ''	
	self.AXIS_VALUE_WORLD		= ''
	
	self.SAPRawFileSize		= ''	
	self.NOFSAP			= ''
	
        self.outputDir			= ''
	self.RAM 			= ''
	
	self.NEW_NOF_SUBBANDS		= ''
	self.NEW_CHANNELPERSUBBANDS	= ''
	self.NEW_FMIN			= ''
	self.NEW_FMAX			= ''
	self.NEWTIMEREBIN		= ''
	self.NEWTIMESTART		= ''
	self.NEWTIMESTOP		= ''        
        
        self.GOindicator 		= 'yes'

	self.RAMOptimumResumeAll	= ''  
	self.TimeEstimatedResumeAll	= ''        
        self.SizeEstimated		= ''
	
	self.StartTimeProcess		= ''

	
	self.NEW_CHANNELPERSUBBANDS2COMPUTE	= ''
	self.NEW_FMIN2COMPUTE			= ''
	self.NEW_FMAX2COMPUTE			= ''
	self.NEWTIMEREBIN2COMPUTE		= ''
	self.NEWTIMESTART2COMPUTE		= ''
	self.NEWTIMESTOP2COMPUTE		= ''	
	
	
	
	#print h5FilePath
	#self.PATH 	= askopenfiles(title = "Select Observation's  Beamed Formed Files", initialdir = h5FilePath, filetypes=[("All", "*.h5")])
	self.PATH 	= glob.glob("""%s/*.h5"""%(h5FilePath))
	self.PATH	= sort(self.PATH)
	
	self.FNAMES	= range(len(self.PATH))
	self.SAP	= range(len(self.PATH))
	self.B		= range(len(self.PATH))
	self.S		= range(len(self.PATH))
	self.P		= range(len(self.PATH))
	
	
	
	for i in range(len(self.PATH)):
 		#self.PATH[i] 	= self.PATH[i].name 		
 		## Determine the File name without PATH
 		FNAMESTEMP	= self.PATH[i].split(h5FilePath+'/')
 		self.FNAMES[i]	= FNAMESTEMP[1]
 		## Determine the SAP, BEAM, Stokes and P vectors for SAP, Beam, Stokes and P NoF
 		FNAMESTEMP	= self.FNAMES[i].split('_')
 		self.SAP[i]	= FNAMESTEMP[1]
 		self.B[i]	= FNAMESTEMP[2]
 		self.S[i]	= FNAMESTEMP[3]
 		self.P[i]	= FNAMESTEMP[4]
 		
	self.SAP 	= sort(self.SAP)
	self.B		= sort(self.B)
	self.S		= sort(self.S)
	self.P		= sort(self.P)
	
	
 	## Determine Nof SAP, Beam, Stockes and P(FrequencyPart)
 	
 	if len(self.PATH)-1 > 0:
	      temp = 0
	      self.nofSAP = 1
	      for i in range(len(self.PATH)-1):
		  temp = self.SAP[i]
		  if temp != self.SAP[i+1]:
		      self.nofSAP = self.nofSAP+1 	      
	      temp = 0
	      self.nofB = 1
	      for i in range(len(self.PATH)-1):
		  temp = self.B[i]
		  if temp != self.B[i+1]:
		      self.nofB = self.nofB+1 	      
	      temp = 0      
	      self.nofS = 1
	      for i in range(len(self.PATH)-1):
		  temp = self.S[i]
		  if temp != self.S[i+1]:
		      self.nofS = self.nofS+1		      
	      temp = 0
	      self.nofP = 1
	      for i in range(len(self.PATH)-1):
		  temp = self.P[i]		  
		  if temp != self.P[i+1]:
		      self.nofP = self.nofP+1
		      	      
	else:
	    self.nofSAP = 1
	    self.nofB = 1
	    self.nofS = 1
	    self.nofP = 1
	    
	  	    

        #### Define Left and Right Parents for organising windows 		
	################################################
        self.leftparent = Frame(self.grandparent, width=672)
        self.leftparent.pack(side=LEFT, fill=Y)
         

        
        ### Frames for Left Parent
        
	# FRAME N°0 (Frame for loading Dynspec => Listbox)
        self.lframe0 = LabelFrame(self.leftparent, text="SAP Beamed Formed Data",width=672, height=15)
        self.lframe0.pack(anchor=NW)
	### Listbox for dynspec
	yscrollbar0 = Scrollbar(self.lframe0,orient=VERTICAL )
	yscrollbar0.pack(side=RIGHT, fill= Y)		
	self.beamFormedTable = Listbox(self.lframe0,width=75, height=15,yscrollcommand=yscrollbar0.set) 
	
	
	for i in range(len(self.FNAMES)):	
	    self.beamFormedTable.insert(i, self.FNAMES[i])

	self.beamFormedTable.pack()
	yscrollbar0.config(command=self.beamFormedTable.yview)		
	
	
	
	### Frame N°1 (Frame for loading Metadata => TextBox)
	self.lframe1 = LabelFrame(self.leftparent, text="Metadata",width=672, height=15)
        self.lframe1.pack(anchor=W)
	#### Message box for showing selected metadata
	xscrollbar1 = Scrollbar(self.lframe1, orient=HORIZONTAL)
	xscrollbar1.pack(side=BOTTOM, fill=X)
	yscrollbar1 = Scrollbar(self.lframe1, orient=VERTICAL)
	yscrollbar1.pack(side=RIGHT, fill=Y)
	self.beamFormedMetaData = Text(self.lframe1,width=83, height=15, wrap=NONE,xscrollcommand=xscrollbar1.set,yscrollcommand=yscrollbar1.set)
	self.beamFormedMetaData.pack()
	xscrollbar1.config(command=self.beamFormedMetaData.xview)
	yscrollbar1.config(command=self.beamFormedMetaData.yview)	
			
	
	### Frame N°2b (Selection Resume)
	self.lframe2 = LabelFrame(self.leftparent, text="Resume SAP Selection:",width=672, height=2)
        self.lframe2.pack(anchor=W)
	### Listbox for select Stokes parameter
	self.resumeSelection = Listbox(self.lframe2,width=79, height=1)
	self.resumeSelection.pack()	
	
	
	
	####FRAME N°3 (Action button)
	self.lframe3 = LabelFrame(self.leftparent, text="Actions", width=672, height=2)
        self.lframe3.pack(anchor=SW)      
	
	#### Extract Button for plotting selected dynspec
	self.extractAllSAPBbutton = Button(self.lframe3, text="Convert All SAP Without Changes",width=73, height=2, padx=10, pady=3)
	self.extractAllSAPBbutton.config(state=DISABLED)
	self.extractAllSAPBbutton.pack()	

	#### Extract Button for plotting selected dynspec
	self.extractOneSAPBbutton = Button(self.lframe3, text="Convert Selected SAP Without Changes",width=73, height=2, padx=10, pady=3)
	self.extractOneSAPBbutton.config(state=DISABLED)
	self.extractOneSAPBbutton.pack()	

	#### Extract Button for plotting selected dynspec
	self.extractAllSAPRebinBbutton = Button(self.lframe3, text="Convert All SAP With Rebin and Selection",width=73, height=2, padx=10, pady=3)
	self.extractAllSAPRebinBbutton.config(state=DISABLED)
	self.extractAllSAPRebinBbutton.pack()	

	#### Extract Button for plotting selected dynspec
	self.extractOneSAPRebinBbutton = Button(self.lframe3, text="Convert Selected SAP With Rebin and Selection",width=73, height=2, padx=10, pady=3)
	self.extractOneSAPRebinBbutton.config(state=DISABLED)
	self.extractOneSAPRebinBbutton.pack()		
	
	
	#### Erase button for erase all and restart with another file
	self.eraseBbutton = Button(self.lframe3, text="Clear",width=73, height=2, padx=10, pady=3)
	self.eraseBbutton.pack()	
	
        
        
	#### Actions for All Buttons and Listbox etc ... 
	###############################################	    
	    
	
        #### Active buttons for Frame N°0 and N°1 
	self.beamFormedTable.bind("<Double-Button-1>", lambda event, beamFormedTable = self.beamFormedTable, beamFormedMetaData=self.beamFormedMetaData, nofSAP=self.nofSAP , nofB=self.nofB ,nofS=self.nofS ,nofP=self.nofP, resumeSelection = self.resumeSelection : self.selectionMetaData(self,beamFormedTable,beamFormedMetaData,nofSAP,nofB,nofS,nofP,resumeSelection))
	#self.StokesTable.bind("<Button-1>", lambda event, StokesTable = self.StokesTable, resumeSelection = self.resumeSelection : self.selectionStokes(self,StokesTable,resumeSelection))
        
        #### Active buttons for Frame N°3
        self.extractAllSAPBbutton.bind("<Button-1>", lambda event : self.extractAllSAP(self))
        self.extractOneSAPBbutton.bind("<Button-1>", lambda event : self.extractOneSAP(self))
        self.extractAllSAPRebinBbutton.bind("<Button-1>", lambda event : self.extractAllSAPRebin(self))
        self.extractOneSAPRebinBbutton.bind("<Button-1>", lambda event : self.extractOneSAPRebin(self))
        
        
        self.eraseBbutton.bind("<Button-1>", lambda event, lframe0 = self.lframe0, lframe1 = self.lframe1, lframe2 = self.lframe2,lframe3 = self.lframe3, leftparent = self.leftparent : self.selectionErase(self,lframe0,lframe1,lframe2,lframe3,leftparent))
 
        
        
    ################################################"    
    ## Erase Function     
    ################################################"    
    def selectionErase(self,event,lframe0,lframe1,lframe2,lframe3,leftparent):   # lframe0,lframe1,lframe2,lframe2b,lframe3,lframe4,leftparent,rightparent):	
	lframe0.destroy()
	lframe1.destroy()
	lframe2.destroy()
	#lframe2b.destroy()
	lframe3.destroy()
	#lframe4.destroy()
	leftparent.destroy()
	#rightparent.destroy()

	
	
    def eraseObject(self):	
	self.lframe0.destroy()
	self.lframe1.destroy()	
	self.lframe2.destroy()
	self.lframe3.destroy()
	self.leftparent.destroy()
	
	
    ################################################"    
    ## Selection Dynspec => plot metadata  Function     
    ################################################" 	
    def selectionMetaData(self,event,beamFormedTable,beamFormedMetaData,nofSAP,nofB,nofS,nofP,resumeSelection):
	
	beamFormedMetaData.config(state=NORMAL)
	beamFormedMetaData.delete(1.0, END)

	#StokesTable.config(state=NORMAL)
	#StokesTable.delete(0, END)
	
	resumeSelection.config(state=NORMAL)
	resumeSelection.delete(0, END)	
	
	nofSelectedBeamFormed = int(beamFormedTable.curselection()[0])
	
	k_nofSAP = range(nofSAP)
	k_nofB	 = range(nofB)
	k_nofS	 = range(nofS)
	k_nofP	 =  range(nofP)
	
	self.SAPSelected 	= 0
	BSelected	= 0
	SSelected	= 0
	pSelected	= 0
		
	index = -1	    
	for i in k_nofSAP:
	    for j in k_nofB:
		for k in  k_nofS:
		    for l in k_nofP:
			index = l+k*nofP+j*nofS*nofP+i*nofB*nofS*nofP
			if index == nofSelectedBeamFormed:
				self.SAPSelected 	= i
				BSelected	= j
				SSelected	= k
				pSelected	= l	

	if self.SAPSelected < 10:
	    self.indexSAPSelected = "SUB_ARRAY_POINTING_00%s"%(self.SAPSelected)
        if self.SAPSelected > 9 and nofSelectedDynspec<100:
	    self.indexSAPSelected = "SUB_ARRAY_POINTING_0%s"%(self.SAPSelected)
        if self.SAPSelected > 99 and nofSelectedDynspec<1000:   
            self.indexSAPSelected = "SUB_ARRAY_POINTING_%s"%(self.SAPSelected)
        

        
        
	if BSelected < 10:
	    indexBSelected = "BEAM_00%s"%(BSelected)
        if BSelected > 9 and nofSelectedDynspec<100:
	    indexBSelected = "BEAM_0%s"%(BSelected)
        if BSelected > 99 and nofSelectedDynspec<1000:   
            indexBSelected = "BEAM_%s"%(BSelected)        
        
	indexSSelected = "STOKES_%s"%(SSelected)
        
        #############################################################"
        # Load Data for the  Selected SAP
        
        h5File = File(self.PATH[nofSelectedBeamFormed], File.READ)
        obsID = AttributeString(h5File, "OBSERVATION_ID")
        #print obsID.value
        
        selectedSAPGroup	= Group(h5File, self.indexSAPSelected)
        selectedBEAMGroup	= Group(selectedSAPGroup, indexBSelected)
             
        selectStokesGroup	= DatasetFloat(selectedBEAMGroup, indexSSelected)    
        selectCoordinatesGroup = Group(selectedBEAMGroup,"COORDINATES")
        
        selectTimeGroup 	= Group(selectCoordinatesGroup,"COORDINATE_0")
        selectSpectralGroup 	= Group(selectCoordinatesGroup,"COORDINATE_1")
            
        
        #Load Meta Data for the  Selected SAP
        obsID 			= AttributeString(h5File, "OBSERVATION_ID")
        obsTarget		= AttributeVString(selectedBEAMGroup, "TARGETS")
        obsRA 			= AttributeFloat(selectedBEAMGroup, "POINT_RA")
        obsDEC 			= AttributeFloat(selectedBEAMGroup, "POINT_DEC")
        obsPositionUnit		= AttributeString(selectedBEAMGroup,"POINT_RA_UNIT")
        obsStationList 		= AttributeVString(selectedBEAMGroup, "STATIONS_LIST")
        nofobsStationList	= len(obsStationList.value)
        empty			= "\n"
        
        obsNuMin 		= AttributeFloat(h5File,"OBSERVATION_FREQUENCY_MIN")
        obsNuMax 		= AttributeFloat(h5File, "OBSERVATION_FREQUENCY_MAX") 
        obsNuCenter		= AttributeFloat(selectedBEAMGroup, "BEAM_FREQUENCY_CENTER")
	obsNuCenterUnit		= AttributeString(selectedBEAMGroup, "BEAM_FREQUENCY_CENTER_UNIT")	
        nofSubbands		= AttributeInt(selectStokesGroup, "NOF_SUBBANDS")
        channelsPerSubband	= AttributeInt(selectedBEAMGroup, "CHANNELS_PER_SUBBAND")
        nofChannels		= nofSubbands.value*channelsPerSubband.value
        SubbandsWidth		= AttributeFloat(selectedBEAMGroup, "SUBBAND_WIDTH")	
        channelWidth		= SubbandsWidth.value/1E6/channelsPerSubband.value
        totalBandWidth		= nofChannels*channelWidth
        bandWithUnit		= "Mhz"
        continousSubband	= 'no'
        if int(obsNuMax.value-obsNuMin.value) == int(totalBandWidth):
	    continousSubband = 'yes'
	stokesParameter		= AttributeString(selectStokesGroup, "STOKES_COMPONENT")
        
        obsTstart 		= AttributeString(h5File, "OBSERVATION_START_UTC")
        obsTstop 		= AttributeString(h5File, "OBSERVATION_END_UTC")
        obsDuration 		= AttributeFloat(h5File, "TOTAL_INTEGRATION_TIME")
        nofTimeSample		= AttributeInt(selectStokesGroup, "NOF_SAMPLES")
        obsTimeIncrement 	= obsDuration.value/nofTimeSample.value
        
        
        RawFileTemp	= self.PATH[nofSelectedBeamFormed].split('.')
        RawFile		= RawFileTemp[0]+".raw"
        
        RawFileSize 	= os.path.getsize(RawFile)/1E9
        oneSAPRawFileSize = RawFileSize*nofB*nofS*nofP
        allSAPRawFileSize = oneSAPRawFileSize*nofSAP


	beamFormedMetaData.update()

 	beamFormedMetaData.insert(1.0, """Observation ID: %s\n"""%(obsID.value))
 	beamFormedMetaData.insert(2.0, """Target: %s\n"""%(obsTarget.value))
 	beamFormedMetaData.insert(3.0, """Ra: %s\n"""%(obsRA.value)) 	
 	beamFormedMetaData.insert(4.0, """Dec: %s\n"""%(obsDEC.value))
 	beamFormedMetaData.insert(5.0, """Ra/Dec Unit: %s\n"""%(obsPositionUnit.value))
 	beamFormedMetaData.insert(6.0, """Nof Stations: %s\n"""%(nofobsStationList))
 	beamFormedMetaData.insert(7.0, """Station List: %s\n"""%(obsStationList.value))
 	
 	beamFormedMetaData.insert(8.0, empty)
 	
	beamFormedMetaData.insert(9.0, """Minimum Frequency: %s\n"""%(obsNuMin.value))
	beamFormedMetaData.insert(10.0, """Maximum Frequency: %s\n"""%(obsNuMax.value))
	beamFormedMetaData.insert(11.0, """Center Frequency: %s\n"""%(obsNuCenter.value))
	beamFormedMetaData.insert(12.0, """Frequency Unit: %s\n"""%(obsNuCenterUnit.value))
	beamFormedMetaData.insert(13.0, """Nof Subbands: %s\n"""%(nofSubbands.value))
	beamFormedMetaData.insert(14.0, """NoF Channels Per Subband: %s\n"""%(channelsPerSubband.value))
	beamFormedMetaData.insert(15.0, """Nof Frequency Channels: %s\n"""%(nofChannels))
	beamFormedMetaData.insert(16.0, """Subbands Width: %s\n"""%(SubbandsWidth.value/1E6))
	beamFormedMetaData.insert(17.0, """Channel Width: %s\n"""%(channelWidth))
	beamFormedMetaData.insert(18.0, """Total Band Width: %s\n"""%(totalBandWidth))
	beamFormedMetaData.insert(19.0, """Band Width Unit: %s\n"""%(bandWithUnit))
	beamFormedMetaData.insert(20.0, """Continous Subband: %s\n"""%(continousSubband))
 	beamFormedMetaData.insert(21.0, """Stokes Parameter: %s\n"""%(stokesParameter.value))
 	
 	beamFormedMetaData.insert(22.0, empty)
 		
	beamFormedMetaData.insert(23.0, """Start UTC: %s\n"""%(obsTstart.value))
	beamFormedMetaData.insert(24.0, """End UTC: %s\n"""%(obsTstop.value))
	beamFormedMetaData.insert(25.0, """Nof Time Samples: %s\n"""%(nofTimeSample.value))
	beamFormedMetaData.insert(26.0, """Time Increment: %s\n"""%(obsTimeIncrement))
	beamFormedMetaData.insert(27.0, """Total Integration Time: %s\n"""%(obsDuration.value))

 	beamFormedMetaData.insert(28.0, empty)
 	
 	beamFormedMetaData.insert(29.0,"""Selected File Size (Go): %s\n"""%(RawFileSize))	
 	beamFormedMetaData.insert(30.0,"""1 SAP File Size (Go): %s\n"""%(oneSAPRawFileSize))
 	beamFormedMetaData.insert(31.0,"""All SAP File Size (Go:) %s\n"""%(allSAPRawFileSize))
 	
	beamFormedMetaData.update()
		
	## Catch environment variable
	
	resumeSelection.insert(0, "Selected SAP : %s"%(self.indexSAPSelected))
	
	if self.indexSAPSelected != '' :
	    self.extractAllSAPBbutton.config(state=NORMAL)
	    self.extractOneSAPBbutton.config(state=NORMAL)
	    self.extractAllSAPRebinBbutton.config(state=NORMAL) 
	    self.extractOneSAPRebinBbutton.config(state=NORMAL)   
	       
	self.observationID	= obsID.value
	
	self.tStart			= 0
	self.tStop			= obsDuration.value
	self.tIncrement			= obsTimeIncrement
	self.nofTimeSample		= nofTimeSample.value
	
	
	self.fMin			= obsNuMin.value
	self.fMax			= obsNuMax.value
	
	AXIS_VALUE_WORLD_TEMP		= ((AttributeVFloat(selectSpectralGroup, "AXIS_VALUES_WORLD")).value)
	k				= range(len(AXIS_VALUE_WORLD_TEMP))
	self.AXIS_VALUE_WORLD		= range(len(AXIS_VALUE_WORLD_TEMP))
	for i in k:
	    self.AXIS_VALUE_WORLD[i]	= AXIS_VALUE_WORLD_TEMP[i]/1E6
	
	
	self.nofChannelsPerSubband	= channelsPerSubband.value
	self.nofSubbands		= nofSubbands.value
	
	
	self.SAPRawFileSize		= RawFileSize*nofB*nofS*nofP
	self.NOFSAP			= nofSAP
	
	

    
    
    ################################################"    
    ## EXTRACT ALL SAP WITHOUT CHANGES
    ################################################" 	
    def extractAllSAP(self,event):
        
	self.SizeEstimated 	=  self.SAPRawFileSize*self.NOFSAP
	    
	RAMOptimumTemp 	= 1/(1.277E8/(self.nofTimeSample*self.nofSubbands*self.nofChannelsPerSubband))
	self.RAMOptimum	= round(RAMOptimumTemp,3)+0.05 #
	    
	self.TimeEstimated	= self.SizeEstimated*1.34/0.2256*self.NOFSAP
        
        
	self.extractAllSAPBbutton.config(state=DISABLED)
	self.extractOneSAPBbutton.config(state=DISABLED)
	self.extractAllSAPRebinBbutton.config(state=DISABLED)
	self.extractOneSAPRebinBbutton.config(state=DISABLED)        
        
        
	self.outputDir = tkFileDialog.askdirectory(title='Choose Output Directory')
	self.RamSelect	= Tk()
	self.RamSelect.title("RAM Selection")	
	self.RAMText = Label(self.RamSelect, text="""How much RAM do you want to use ? [Default:1Go], OPTIMAL: %s Go """%(self.RAMOptimum))
	self.RAMText.pack( side = LEFT)	
	self.RAMValue = Entry(self.RamSelect, bd =5)
	self.RAMValue.pack(side = RIGHT)	
	self.RAMButton	= Button(self.RamSelect,text="GO",width=9, height=1)
        self.RAMButton.pack(side = BOTTOM)       
	self.RAMButton.bind("<Button-1>", lambda event, RAMValue = self.RAMValue, RamSelect = self.RamSelect : self.extractAllSAPGO(self,RAMValue,RamSelect))                        

	self.ProcessInfoAllSAP()
	
	
    def extractAllSAPGO(self,event,RAMValue,RamSelect):
	
      if RAMValue.get() == '':
	  self.RAM = 1
      else:
	  self.RAM = float(RAMValue.get())
	  if self.RAM <= 0:
	      self.RAM = 1

      self.RamSelect .destroy()	      
	
      
      current_dir = os.getcwd()
	
      cmd = """(cd %s/ LD_LIBRARY_PATH=/usr/local/lib/; %s/../src/ICD3-ICD6-Complete/DynspecAll %s/ L%s  %s/ %s)"""%(self.PATH0,current_dir,self.PATH0,self.observationID,self.outputDir,self.RAM)                               
      print commands.getoutput(cmd)
      self.grandparent.quit()
    
    
    
    
    
    
    
    
    
    
    
    
    ################################################"    
    ## EXTRACT SELECTED SAP WITHOUT CHANGES
    ################################################" 	  
      
    def extractOneSAP(self,event):
      
      
	    
	self.SizeEstimated 	=  self.SAPRawFileSize*self.NOFSAP
	    
	RAMOptimumTemp 	= 1/(1.277E8/(self.nofTimeSample*self.nofSubbands*self.nofChannelsPerSubband))
	self.RAMOptimum	= round(RAMOptimumTemp,3)+0.05 #
	 	 	 	 
	self.TimeEstimated	= self.SizeEstimated*1.34/0.2256
	          
      
	self.extractAllSAPBbutton.config(state=DISABLED)
	self.extractOneSAPBbutton.config(state=DISABLED)
	self.extractAllSAPRebinBbutton.config(state=DISABLED)
	self.extractOneSAPRebinBbutton.config(state=DISABLED)      
      
      
	self.outputDir = tkFileDialog.askdirectory(title='Choose Output Directory')	
	self.RamSelect	= Tk()
	self.RamSelect.title("RAM Selection")	
	self.RAMText = Label(self.RamSelect, text="""How much RAM do you want to use ? [Default:1Go], OPTIMAL: %s Go """%(self.RAMOptimum))
	self.RAMText.pack( side = LEFT)	
	self.RAMValue = Entry(self.RamSelect, bd =5)
	self.RAMValue.pack(side = RIGHT)	
	self.RAMButton	= Button(self.RamSelect,text="GO",width=9, height=1)
        self.RAMButton.pack(side = BOTTOM)        
	self.RAMButton.bind("<Button-1>", lambda event, RAMValue = self.RAMValue, RamSelect = self.RamSelect : self.extractOneSAPGO(self,RAMValue,RamSelect))                        
    
	self.ProcessInfoOneSAP()
    
    
    def extractOneSAPGO(self,event,RAMValue,RamSelect):
	
      if RAMValue.get() == '':
	  self.RAM = 1
      else:
	  self.RAM = float(RAMValue.get())
	  if self.RAM <= 0:
	      self.RAM = 1
	    
      self.RamSelect .destroy()
  
      current_dir = os.getcwd()
      
      cmd = """(cd %s/ LD_LIBRARY_PATH=/usr/local/lib/; %s/../src/ICD3-ICD6-Rebin/DynspecPart %s/ L%s  %s/  %s %s %s %s %s %s no %s %s)"""%(self.PATH0,current_dir,self.PATH0,self.observationID,self.outputDir,self.tStart,self.tStop,self.tIncrement,self.fMin,self.fMax,self.nofChannelsPerSubband,self.RAM,self.SAPSelected)                          
      
      print commands.getoutput(cmd)
      self.grandparent.quit()      
      
      
      
      
      
      
      
      
      
      
      
      
      
    ################################################"    
    ## EXTRACT ALL SAP WITH Rebin ... 
    ################################################" 	    
      
    def extractAllSAPRebin(self,event):

	self.extractAllSAPBbutton.config(state=DISABLED)
	self.extractOneSAPBbutton.config(state=DISABLED)
	self.extractAllSAPRebinBbutton.config(state=DISABLED)
	self.extractOneSAPRebinBbutton.config(state=DISABLED)    
    
    
    
	self.SelectionRebin = Tk()
	self.SelectionRebin.geometry("915x340+0+0")
	self.SelectionRebin.title("Time and Frequency selection and rebinning")
	
	
	self.parent1 = Frame(self.SelectionRebin)
        self.parent1.pack() 
	
	self.parent2 = Frame(self.SelectionRebin)
        self.parent2.pack()
        
	self.parent3 = Frame(self.SelectionRebin)
        self.parent3.pack()       
        
	self.parent4 = Frame(self.SelectionRebin)
        self.parent4.pack()  
        
	self.parent5 = Frame(self.SelectionRebin)
        self.parent5.pack()      
 
	self.parent6 = Frame(self.SelectionRebin)
        self.parent6.pack()
        
    
	self.TminSelection = Label(self.parent1,             text="""Time Start (in s, must be greater or equal to %s)                                                                                           """%(self.tStart))
	self.TmaxSelection = Label(self.parent2,             text="""Time Stop (in s, must be lower or equal to %s)                                                                              """%(self.tStop))
	self.FminSelection = Label(self.parent3,             text="""Frequency Start (in MHz, must be greater or equal to %s)                                                                 """%(self.fMin))
	self.FmaxSelection = Label(self.parent4,             text="""Frequency Stop (in MHz, must be lower or equal to %s)                                                                    """%(self.fMax))
	self.TincSelection		= Label(self.parent5, text="""New Time increment (in s, must be greater or equal to %s)                                                             """%(self.tIncrement))
	self.FNofChaSubSelection	= Label(self.parent6, text="""New Number of Channels per subbands (must be =1 or =< to %s and multiple of 2^(n) => 2,4,8,16,32 ... 2^(13))"""%(self.nofChannelsPerSubband))
      	
	self.TminValue 		= Entry(self.parent1, bd =15)
	self.TmaxValue 		= Entry(self.parent2, bd =15)
	self.FminValue 		= Entry(self.parent3, bd =15)
	self.FmaxValue 		= Entry(self.parent4, bd =15)
	self.TincValue 		= Entry(self.parent5, bd =15)
	self.FNofChaSubValue 	= Entry(self.parent6, bd =15)

 	self.TminSelection.pack( side = LEFT)
	self.TminValue.pack( side =  RIGHT)
	
	self.TmaxSelection.pack( side = LEFT)
	self.TmaxValue.pack( side =  RIGHT)
	
	self.FminSelection.pack( side = LEFT)
	self.FminValue.pack( side =  RIGHT)
	
	self.FmaxSelection.pack( side = LEFT)
	self.FmaxValue .pack( side =  RIGHT)
	
	self.TincSelection.pack( side = LEFT)
	self.TincValue.pack( side =  RIGHT)
	
	self.FNofChaSubSelection.pack( side = LEFT)	
	self.FNofChaSubValue.pack( side =  RIGHT)
   
	self.SelectionRebinButton= Button(self.SelectionRebin,text="GO",width=9, height=1)
        self.SelectionRebinButton.pack(side = BOTTOM) 
   
   
	self.SelectionRebinButton.bind("<Button-1>", lambda event, TminValue = self.TminValue, TmaxValue = self.TmaxValue, FminValue = self.FminValue, FmaxValue= self.FmaxValue, TincValue= self.TincValue, FNofChaSubValue = self.FNofChaSubValue,SelectionRebin = self.SelectionRebin : self.extractAllSAPRebinGO(self,TminValue, TmaxValue, FminValue, FmaxValue, TincValue, FNofChaSubValue,SelectionRebin))      

	
    def extractAllSAPRebinGO(self,event,TminValue, TmaxValue, FminValue, FmaxValue, TincValue, FNofChaSubValue,SelectionRebin):   
      
	self.GOindicator 		= 'yes'
	
	if TminValue.get() == '' or float(TminValue.get()) < self.tStart:	  
	    self.SelectionRebinButton.config(state=DISABLED)
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Time Start (in s) is <%s or empty"""%(self.tStart))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	    
	    
	    
	if TmaxValue.get() == ''or float(TmaxValue.get()) > self.tStop:  
	    self.SelectionRebinButton.config(state=DISABLED)	  
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Time Stop (in s) is >%s or empty"""%(self.tStop))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    

	    
	if float(TmaxValue.get()) < float(TminValue.get()):	
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Time Stop (in s) is <Time Start (in s)""")  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    

	    
	if FminValue.get() == '' or float(FminValue.get()) > self.fMax:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Minimum Frequency is empty or > Fmax (%s MHz)"""%(self.fMax))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	    
	    
	if FmaxValue.get() == '' or float(FmaxValue.get()) < self.fMin:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Maximum Frequency is empty or < Fmin (%s MHz)"""%(self.fMin))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	    
	if float(FmaxValue.get()) < float(FminValue.get()):
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Maximum Frequency < Minimum Frequency""")  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	    
	    
	if TincValue.get() == '' or float(TincValue.get()) < self.tIncrement:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""New time Increment < %s"""%(self.tIncrement))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	
	
	FNofChaSubValue_temp	= int(round(float(FNofChaSubValue.get())))
	if FNofChaSubValue.get() == '': 
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""New Number of Channels per Subband is empty""")  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	
	if FNofChaSubValue_temp > self.nofChannelsPerSubband:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""New Number of Channels per Subband is > %s"""%(self.nofChannelsPerSubband))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	
	if FNofChaSubValue_temp !=1:
	    if FNofChaSubValue_temp&(FNofChaSubValue_temp-1) !=0 or FNofChaSubValue_temp<2:
		self.SelectionRebinButton.config(state=DISABLED)	
		self.GOindicator = 'no'
		self.badParameterBox = Tk()
		self.badParameterBox.geometry("500x40+0+0")
		self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
		self.badParameter = Label(self.badParameterBox,text="""New Number of Channels per Subband is not a multiple of 2^(n) or !=1 """)  
		self.badParameter.pack()	    
		self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
		self.badParameterButton.pack(side = BOTTOM) 	    
		self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
		    
	
	#################################################
	# C++ Computation
	
	if self.GOindicator == 'yes':
	
	    index_fmin	= 0
	    start_min		= self.AXIS_VALUE_WORLD[0]   	
	    while start_min < float(FminValue.get()):
		    index_fmin 	= index_fmin+1
		    start_min 	= self.AXIS_VALUE_WORLD[index_fmin*self.nofChannelsPerSubband]
	
	
	    index_fmax	= self.nofSubbands-1 
	    start_max	= self.AXIS_VALUE_WORLD[len(self.AXIS_VALUE_WORLD)-1]
	    
	    while start_max > float(FmaxValue.get()):
		  index_fmax 	= index_fmax-1
		  start_max 	= self.AXIS_VALUE_WORLD[index_fmax*self.nofChannelsPerSubband+self.nofChannelsPerSubband-1] 
		    
	    timeIcrementReal 	= 1/(self.nofTimeSample/self.tStop)	
	    timeIndexIncrementRebin	= round(float(TincValue.get())/timeIcrementReal)
	    timeRebin 		= timeIndexIncrementRebin*(1/(self.nofTimeSample/self.tStop))

	    timeIncrement 		= timeRebin
	  
	    timeIndexStart   	= round(float(TminValue.get())/timeIcrementReal)
	    timeIndexStart 	= (timeIndexStart/timeIndexIncrementRebin)*timeIndexIncrementRebin
	    timeMinSelect  	= timeIndexStart*timeIcrementReal
	    
	    timeIndexStop	= round(float(TmaxValue.get())/timeIcrementReal)
	    timeIndexStop 	= timeIndexStop/timeIndexIncrementRebin*timeIndexIncrementRebin
	    timeMaxSelect 	= timeIndexStop*timeIcrementReal
	      
	    self.NEW_NOF_SUBBANDS		= index_fmax-index_fmin+1
	    self.NEW_CHANNELPERSUBBANDS	= int(round(float(FNofChaSubValue.get())))
	    self.NEW_FMIN			= start_min
	    self.NEW_FMAX			= start_max
	    self.NEWTIMEREBIN		= timeIncrement
	    self.NEWTIMESTART		= timeMinSelect
	    self.NEWTIMESTOP		= timeMaxSelect	
	    
	    
	    self.SizeEstimated 	=   (((self.NEWTIMESTOP-self.NEWTIMESTART)/self.NEWTIMEREBIN)*(float(FNofChaSubValue.get())*self.NEW_NOF_SUBBANDS))*(self.SAPRawFileSize*self.NOFSAP)/(self.nofTimeSample*self.nofSubbands*self.nofChannelsPerSubband)
	    
	    RAMOptimumTemp 	= 1/(1.277E8/((self.NEWTIMESTOP-self.NEWTIMESTART)/self.NEWTIMEREBIN*self.NEW_NOF_SUBBANDS*self.NEW_CHANNELPERSUBBANDS))
	    self.RAMOptimum	= round(RAMOptimumTemp,3)+0.05 
	    
	    self.TimeEstimated	= self.SizeEstimated*1.59/0.2256*self.NOFSAP
	    	 

	    ######################################
	    
	    
	    #self.SelectionRebin.destroy()
	    
	    self.SelectionRebinButton.config(state=DISABLED)	
	    
	    self.ResumeAll	= Tk()
	    self.ResumeAll.title("Overview Selection")	
	    self.ResumeAll.geometry("400x220+0+0")
	    
	    self.parent7 = Frame(self.ResumeAll)
	    self.parent7.pack() 
	    
	    self.parent8 = Frame(self.ResumeAll)
	    self.parent8.pack()
	    
	    self.parent9 = Frame(self.ResumeAll)
	    self.parent9.pack()       
	    
	    self.parent10 = Frame(self.ResumeAll)
	    self.parent10.pack()  
	    
	    self.parent11 = Frame(self.ResumeAll)
	    self.parent11.pack()      
    
	    self.parent12 = Frame(self.ResumeAll)
	    self.parent12.pack()
	    
	    self.parent13 = Frame(self.ResumeAll)
	    self.parent13.pack()        

	    self.parent14 = Frame(self.ResumeAll)
	    self.parent14.pack()    
	    
	    self.parent15 = Frame(self.ResumeAll)
	    self.parent15.pack()    	    

	    self.parent16 = Frame(self.ResumeAll)
	    self.parent16.pack() 	    
	    
	    self.TminResumeAll = Label(self.parent7,text="""Time Start (in s): %s) """%(self.NEWTIMESTART))
	    self.TmaxResumeAll = Label(self.parent8,text="""Time Stop (in s): %s)"""%(self.NEWTIMESTOP))
	    self.FminResumeAll = Label(self.parent9,text="""Frequency Start (in MHz): %s)"""%(self.NEW_FMIN))
	    self.FmaxResumeAll = Label(self.parent10,text="""Frequency Stop (in MHz):  %s) """%(self.NEW_FMAX))
	    self.TincResumeAll		= Label(self.parent11, text="""New Time increment %s s"""%(self.NEWTIMEREBIN))
	    self.FNofChaSubResumeAll	= Label(self.parent12, text="""New Number of Channels per subbands %s"""%(self.NEW_CHANNELPERSUBBANDS))
	    self.FILESIZE		= Label(self.parent13, text="""TOTAL of Files in Go %s"""%(self.SizeEstimated))
	    self.NoFSAPResumeAll	= Label(self.parent14, text="""Number of SAP to process: %s"""%(self.NOFSAP))
	    self.RAMOptimumResumeAll	= Label(self.parent15, text="""Optimal RAM Quantity (in Go) for processing: %s"""%(self.RAMOptimum))	    
	    self.TimeEstimatedResumeAll= Label(self.parent16, text="""Optimal Processing Time Estimation (in s): %s"""%(self.TimeEstimated)) 
	    
	    
	    self.TminResumeAll.pack( side = LEFT)
	    self.TmaxResumeAll.pack( side = LEFT)
	    self.FminResumeAll.pack( side = LEFT)
	    self.FmaxResumeAll.pack( side = LEFT)
	    self.TincResumeAll.pack( side = LEFT)
	    self.FNofChaSubResumeAll.pack( side = LEFT)
	    self.FILESIZE.pack( side = LEFT)
	    self.NoFSAPResumeAll.pack( side = LEFT)
	    self.RAMOptimumResumeAll.pack( side = LEFT)
	    self.TimeEstimatedResumeAll.pack( side = LEFT)	    
	    
	    
	    self.ResumeAllButtonYes	= Button(self.ResumeAll,text="GO",width=9, height=1)
	    self.ResumeAllButtonYes.pack(side = LEFT) 
      
	    self.ResumeAllButtonNo	= Button(self.ResumeAll,text="STOP",width=9, height=1)
	    self.ResumeAllButtonNo.pack(side = RIGHT) 
	    
	    self.ResumeAllButtonYes.bind("<Button-1>", lambda event  : self.ResumeAllYes_GO(self))      
	    self.ResumeAllButtonNo.bind("<Button-1>", lambda event,  ResumeAll = self.ResumeAll, SelectionRebin = self.SelectionRebin: self.ResumeAllButtonNo_Go(self, ResumeAll,SelectionRebin))      

        
	
    def ResumeAllYes_GO(self,event):
	
	self.ResumeAllButtonYes.config(state=DISABLED)
	self.ResumeAllButtonNo.config(state=DISABLED)
	
	
	self.outputDir = tkFileDialog.askdirectory(title='Choose Output Directory')	
	self.RamSelect	= Tk()
	self.RamSelect.title("RAM Selection")	
	self.RAMText = Label(self.RamSelect, text="""How much RAM do you want to use ? [Default:1Go], OPTIMAL: %s Go """%(self.RAMOptimum))
	self.RAMText.pack( side = LEFT)	
	self.RAMValue = Entry(self.RamSelect, bd =5)
	self.RAMValue.pack(side = RIGHT)	
	self.RAMButton	= Button(self.RamSelect,text="GO",width=9, height=1)
        self.RAMButton.pack(side = BOTTOM)        
	self.RAMButton.bind("<Button-1>", lambda event, RAMValue = self.RAMValue, RamSelect = self.RamSelect : self.extractALLSAP_REBIN_GO(self,RAMValue,RamSelect))   	
	
	self.ProcessInfoAllSAP()
	
	
    def extractALLSAP_REBIN_GO(self,event,RAMValue,RamSelect):
	
      if RAMValue.get() == '':
	  self.RAM = 1
      else:
	  self.RAM = float(RAMValue.get())
	  if self.RAM <= 0:
	      self.RAM = 1
	    
      self.RamSelect.destroy()
      self.ResumeAll.destroy()
      self.SelectionRebin.destroy()  
      
      current_dir = os.getcwd()
      
      cmd = """(cd %s/ LD_LIBRARY_PATH=/usr/local/lib/; %s/../src/ICD3-ICD6-Rebin/DynspecPart %s/ L%s  %s/  %s %s %s %s %s %s yes %s 0)"""%(self.PATH0,current_dir,self.PATH0,self.observationID,self.outputDir,self.NEWTIMESTART,self.NEWTIMESTOP,self.NEWTIMEREBIN,self.NEW_FMIN,self.NEW_FMAX,self.NEW_CHANNELPERSUBBANDS,self.RAM)                          
      
      print cmd

      #print commands.getoutput(cmd)
      
      os.system(cmd) 
      self.grandparent.quit()  	

	
    def ResumeAllButtonNo_Go(self,event,ResumeAll,SelectionRebin):
	    ResumeAll.destroy()
	    self.SelectionRebinButton.config(state=NORMAL)	    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    ################################################"    
    ## EXTRACT One SAP WITH Rebin ... 
    ################################################" 	    
    
    
    def extractOneSAPRebin(self,event):
      

	self.extractAllSAPBbutton.config(state=DISABLED)
	self.extractOneSAPBbutton.config(state=DISABLED)
	self.extractAllSAPRebinBbutton.config(state=DISABLED)
	self.extractOneSAPRebinBbutton.config(state=DISABLED)      
      
      
	self.SelectionRebin = Tk()
	self.SelectionRebin.geometry("915x340+0+0")
	self.SelectionRebin.title("Time and Frequency selection and rebinning")
	
	
	self.parent1 = Frame(self.SelectionRebin)
        self.parent1.pack() 
	
	self.parent2 = Frame(self.SelectionRebin)
        self.parent2.pack()
        
	self.parent3 = Frame(self.SelectionRebin)
        self.parent3.pack()       
        
	self.parent4 = Frame(self.SelectionRebin)
        self.parent4.pack()  
        
	self.parent5 = Frame(self.SelectionRebin)
        self.parent5.pack()      
 
	self.parent6 = Frame(self.SelectionRebin)
        self.parent6.pack()
        
    
	self.TminSelection = Label(self.parent1,             text="""Time Start (in s, must be greater or equal to %s)                                                                                           """%(self.tStart))
	self.TmaxSelection = Label(self.parent2,             text="""Time Stop (in s, must be lower or equal to %s)                                                                              """%(self.tStop))
	self.FminSelection = Label(self.parent3,             text="""Frequency Start (in MHz, must be greater or equal to %s)                                                                 """%(self.fMin))
	self.FmaxSelection = Label(self.parent4,             text="""Frequency Stop (in MHz, must be lower or equal to %s)                                                                    """%(self.fMax))
	self.TincSelection		= Label(self.parent5, text="""New Time increment (in s, must be greater or equal to %s)                                                             """%(self.tIncrement))
	self.FNofChaSubSelection	= Label(self.parent6, text="""New Number of Channels per subbands (must be =1 or =< to %s and multiple of 2^(n) => 2,4,8,16,32 ... 2^(13))"""%(self.nofChannelsPerSubband))
      	
	self.TminValue 		= Entry(self.parent1, bd =15)
	self.TmaxValue 		= Entry(self.parent2, bd =15)
	self.FminValue 		= Entry(self.parent3, bd =15)
	self.FmaxValue 		= Entry(self.parent4, bd =15)
	self.TincValue 		= Entry(self.parent5, bd =15)
	self.FNofChaSubValue 	= Entry(self.parent6, bd =15)

 	self.TminSelection.pack( side = LEFT)
	self.TminValue.pack( side =  RIGHT)
	
	self.TmaxSelection.pack( side = LEFT)
	self.TmaxValue.pack( side =  RIGHT)
	
	self.FminSelection.pack( side = LEFT)
	self.FminValue.pack( side =  RIGHT)
	
	self.FmaxSelection.pack( side = LEFT)
	self.FmaxValue .pack( side =  RIGHT)
	
	self.TincSelection.pack( side = LEFT)
	self.TincValue.pack( side =  RIGHT)
	
	self.FNofChaSubSelection.pack( side = LEFT)	
	self.FNofChaSubValue.pack( side =  RIGHT)
   
	self.SelectionRebinButton= Button(self.SelectionRebin,text="GO",width=9, height=1)
        self.SelectionRebinButton.pack(side = BOTTOM) 
   
   
	self.SelectionRebinButton.bind("<Button-1>", lambda event, TminValue = self.TminValue, TmaxValue = self.TmaxValue, FminValue = self.FminValue, FmaxValue= self.FmaxValue, TincValue= self.TincValue, FNofChaSubValue = self.FNofChaSubValue,SelectionRebin = self.SelectionRebin : self.extractOneSAPRebinGO(self,TminValue, TmaxValue, FminValue, FmaxValue, TincValue, FNofChaSubValue,SelectionRebin))      

	
    def badParameterButtonOK(self,event,badParameterBox,SelectionRebin):
	
	badParameterBox.destroy()
	self.SelectionRebinButton.config(state=NORMAL)
	
	
    def extractOneSAPRebinGO(self,event,TminValue, TmaxValue, FminValue, FmaxValue, TincValue, FNofChaSubValue,SelectionRebin):   
      
	self.GOindicator 		= 'yes'
	
	if TminValue.get() == '' or float(TminValue.get()) < self.tStart:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Time Start (in s) is <%s or empty"""%(self.tStart))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	    
	    
	    
	if TmaxValue.get() == ''or float(TmaxValue.get()) > self.tStop:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Time Stop (in s) is >%s or empty"""%(self.tStop))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    

	    
	if float(TmaxValue.get()) < float(TminValue.get()):	
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Time Stop (in s) is <Time Start (in s)""")  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    

	    
	if FminValue.get() == '' or float(FminValue.get()) > self.fMax:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Minimum Frequency is empty or > Fmax (%s MHz)"""%(self.fMax))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	    
	    
	if FmaxValue.get() == '' or float(FmaxValue.get()) < self.fMin:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Maximum Frequency is empty or < Fmin (%s MHz)"""%(self.fMin))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    

	    
	if float(FmaxValue.get()) <= float(FminValue.get()) < 0:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""Maximum Frequency < Minimum Frequency""")  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	    	    
	    
	if TincValue.get() == '' or float(TincValue.get()) < self.tIncrement:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""New time Increment < %s"""%(self.tIncrement))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	
	
	FNofChaSubValue_temp	= int(round(float(FNofChaSubValue.get())))
	if FNofChaSubValue.get() == '': 
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""New Number of Channels per Subband is empty""")  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	
	if FNofChaSubValue_temp > self.nofChannelsPerSubband:
	    self.SelectionRebinButton.config(state=DISABLED)	
	    self.GOindicator = 'no'
	    self.badParameterBox = Tk()
	    self.badParameterBox.geometry("500x40+0+0")
	    self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
	    self.badParameter = Label(self.badParameterBox,text="""New Number of Channels per Subband is > %s"""%(self.nofChannelsPerSubband))  
	    self.badParameter.pack()	    
	    self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
	    self.badParameterButton.pack(side = BOTTOM) 	    
	    self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
	
	if FNofChaSubValue_temp !=1:
	    if FNofChaSubValue_temp&(FNofChaSubValue_temp-1) !=0 or FNofChaSubValue_temp<2:
		self.SelectionRebinButton.config(state=DISABLED)	
		self.GOindicator = 'no'
		self.badParameterBox = Tk()
		self.badParameterBox.geometry("500x40+0+0")
		self.badParameterBox.title("Bad Selection or Rebin Parameter")		    
		self.badParameter = Label(self.badParameterBox,text="""New Number of Channels per Subband is not a multiple of 2^(n) or !=1 """)  
		self.badParameter.pack()	    
		self.badParameterButton= Button(self.badParameterBox,text="OK",width=9, height=1)
		self.badParameterButton.pack(side = BOTTOM) 	    
		self.badParameterButton.bind("<Button-1>", lambda event, badParameterBox = self.badParameterBox, SelectionRebin = self.SelectionRebin  : self.badParameterButtonOK(self,badParameterBox,SelectionRebin))      	    
			
	
	#################################################
	# C++ Computation
	
	if self.GOindicator == 'yes':
	  
	    
	    
	    #################################################

	    self.NEW_CHANNELPERSUBBANDS2COMPUTE	= FNofChaSubValue_temp
	    self.NEW_FMIN2COMPUTE			= float(FminValue.get())
	    self.NEW_FMAX2COMPUTE			= float(FmaxValue.get())
	    self.NEWTIMEREBIN2COMPUTE			= float(TincValue.get())
	    self.NEWTIMESTART2COMPUTE			= float(TminValue.get())
	    self.NEWTIMESTOP2COMPUTE			= float(TmaxValue.get())		  
	  	  
	    #################################################	  
	
	    index_fmin	= 0
	    start_min		= self.AXIS_VALUE_WORLD[0]   	
	    while start_min < float(FminValue.get()):
		    index_fmin 	= index_fmin+1
		    start_min 	= self.AXIS_VALUE_WORLD[index_fmin*self.nofChannelsPerSubband]
	
	
	    index_fmax	= self.nofSubbands-1 
	    start_max	= self.AXIS_VALUE_WORLD[len(self.AXIS_VALUE_WORLD)-1]
	    
	    while start_max > float(FmaxValue.get()):
		  index_fmax 	= index_fmax-1
		  start_max 	= self.AXIS_VALUE_WORLD[index_fmax*self.nofChannelsPerSubband+self.nofChannelsPerSubband-1] 
		    
	    timeIcrementReal 	= 1/(self.nofTimeSample/self.tStop)	
	    timeIndexIncrementRebin	= round(float(TincValue.get())/timeIcrementReal)
	    timeRebin 		= timeIndexIncrementRebin*(1/(self.nofTimeSample/self.tStop))

	    timeIncrement 		= timeRebin
	  
	    timeIndexStart   	= round(float(TminValue.get())/timeIcrementReal)
	    timeIndexStart 	= (timeIndexStart/timeIndexIncrementRebin)*timeIndexIncrementRebin
	    timeMinSelect  	= timeIndexStart*timeIcrementReal
	    
	    timeIndexStop	= round(float(TmaxValue.get())/timeIcrementReal)
	    timeIndexStop 	= timeIndexStop/timeIndexIncrementRebin*timeIndexIncrementRebin
	    timeMaxSelect 	= timeIndexStop*timeIcrementReal
	      
	    self.NEW_NOF_SUBBANDS		= index_fmax-index_fmin+1
	    self.NEW_CHANNELPERSUBBANDS	= int(round(float(FNofChaSubValue.get())))
	    self.NEW_FMIN			= start_min
	    self.NEW_FMAX			= start_max
	    self.NEWTIMEREBIN		= timeIncrement
	    self.NEWTIMESTART		= timeMinSelect
	    self.NEWTIMESTOP		= timeMaxSelect	
	    
	    
	    self.SizeEstimated 	=   (((self.NEWTIMESTOP-self.NEWTIMESTART)/self.NEWTIMEREBIN)*(float(FNofChaSubValue.get())*self.NEW_NOF_SUBBANDS))*(self.SAPRawFileSize*self.NOFSAP)/(self.nofTimeSample*self.nofSubbands*self.nofChannelsPerSubband)
	    
	    RAMOptimumTemp 	= 1/(1.277E8/((self.NEWTIMESTOP-self.NEWTIMESTART)/self.NEWTIMEREBIN*self.NEW_NOF_SUBBANDS*self.NEW_CHANNELPERSUBBANDS))
	    self.RAMOptimum	= round(RAMOptimumTemp,3)+0.05 
	    
	    self.TimeEstimated	= self.SizeEstimated*1.59/0.2256
	    
	    
	    
	    ######################################
	    
	    
	    #self.SelectionRebin.destroy()
	    
	    self.SelectionRebinButton.config(state=DISABLED)	
	    
	    self.ResumeOneSAP	= Tk()
	    self.ResumeOneSAP.title("Overview Selection")	
	    self.ResumeOneSAP.geometry("400x220+0+0")
	    
	    self.parent7 = Frame(self.ResumeOneSAP)
	    self.parent7.pack() 
	    
	    self.parent8 = Frame(self.ResumeOneSAP)
	    self.parent8.pack()
	    
	    self.parent9 = Frame(self.ResumeOneSAP)
	    self.parent9.pack()       
	    
	    self.parent10 = Frame(self.ResumeOneSAP)
	    self.parent10.pack()  
	    
	    self.parent11 = Frame(self.ResumeOneSAP)
	    self.parent11.pack()      
    
	    self.parent12 = Frame(self.ResumeOneSAP)
	    self.parent12.pack()
	    
	    self.parent13 = Frame(self.ResumeOneSAP)
	    self.parent13.pack()        

	    self.parent14 = Frame(self.ResumeOneSAP)
	    self.parent14.pack()    
	    
	    self.parent15 = Frame(self.ResumeOneSAP)
	    self.parent15.pack()    	    

	    self.parent16 = Frame(self.ResumeOneSAP)
	    self.parent16.pack() 	    
	    	    
	    
	    self.TminResumeAll = Label(self.parent7,text="""Time Start (in s): %s) """%(self.NEWTIMESTART))
	    self.TmaxResumeAll = Label(self.parent8,text="""Time Stop (in s): %s)"""%(self.NEWTIMESTOP))
	    self.FminResumeAll = Label(self.parent9,text="""Frequency Start (in MHz): %s)"""%(self.NEW_FMIN))
	    self.FmaxResumeAll = Label(self.parent10,text="""Frequency Stop (in MHz):  %s) """%(self.NEW_FMAX))
	    self.TincResumeAll		= Label(self.parent11, text="""New Time increment %s s"""%(self.NEWTIMEREBIN))
	    self.FNofChaSubResumeAll	= Label(self.parent12, text="""New Number of Channels per subbands %s"""%(self.NEW_CHANNELPERSUBBANDS))
	    self.FILESIZE		= Label(self.parent13, text="""TOTAL of Files in Go %s"""%(self.SizeEstimated))
	    self.NoFSAPResumeAll	= Label(self.parent14, text="""Number of SAP to process: 1""")
	    self.RAMOptimumResumeAll	= Label(self.parent15, text="""Optimal RAM Quantity (in Go) for processing: %s"""%(self.RAMOptimum))	    
	    self.TimeEstimatedResumeAll= Label(self.parent16, text="""Optimal Processing Time Estimation (in s): %s"""%(self.TimeEstimated)) 
	    
	    
	    self.TminResumeAll.pack( side = LEFT)
	    self.TmaxResumeAll.pack( side = LEFT)
	    self.FminResumeAll.pack( side = LEFT)
	    self.FmaxResumeAll.pack( side = LEFT)
	    self.TincResumeAll.pack( side = LEFT)
	    self.FNofChaSubResumeAll.pack( side = LEFT)
	    self.FILESIZE.pack( side = LEFT)
	    self.NoFSAPResumeAll.pack( side = LEFT)
	    self.RAMOptimumResumeAll.pack( side = LEFT)
	    self.TimeEstimatedResumeAll.pack( side = LEFT)	    
	    
	    
	    
	    self.ResumeOneSAPButtonYes	= Button(self.ResumeOneSAP,text="GO",width=9, height=1)
	    self.ResumeOneSAPButtonYes.pack(side = LEFT) 
      
	    self.ResumeOneSAPButtonNo	= Button(self.ResumeOneSAP,text="STOP",width=9, height=1)
	    self.ResumeOneSAPButtonNo.pack(side = RIGHT) 
	    
	    self.ResumeOneSAPButtonYes.bind("<Button-1>", lambda event  : self.ResumeOneSAPYes_GO(self))      
	    self.ResumeOneSAPButtonNo.bind("<Button-1>", lambda event,  ResumeOneSAP = self.ResumeOneSAP, SelectionRebin = self.SelectionRebin: self.ResumeOneSAPButtonNo_Go(self, ResumeOneSAP,SelectionRebin))      

        
	
    def ResumeOneSAPYes_GO(self,event):
	
	self.ResumeOneSAPButtonYes.config(state=DISABLED)
	self.ResumeOneSAPButtonNo.config(state=DISABLED)
	
	self.outputDir = tkFileDialog.askdirectory(title='Choose Output Directory')	
	self.RamSelect	= Tk()
	self.RamSelect.title("RAM Selection")	
	self.RAMText = Label(self.RamSelect, text="""How much RAM do you want to use ? [Default:1Go], OPTIMAL: %s Go"""%(self.RAMOptimum))
	self.RAMText.pack( side = LEFT)	
	self.RAMValue = Entry(self.RamSelect, bd =5)
	self.RAMValue.pack(side = RIGHT)	
	self.RAMButton	= Button(self.RamSelect,text="GO",width=9, height=1)
        self.RAMButton.pack(side = BOTTOM)        
	self.RAMButton.bind("<Button-1>", lambda event, RAMValue = self.RAMValue, RamSelect = self.RamSelect : self.extractOneSAP_REBIN_GO(self,RAMValue,RamSelect))   	
	
	self.ProcessInfoOneSAP()
	
	
	
    def extractOneSAP_REBIN_GO(self,event,RAMValue,RamSelect):
	
      if RAMValue.get() == '':
	  self.RAM = 1
      else:
	  self.RAM = float(RAMValue.get())
	  if self.RAM <= 0:
	      self.RAM = 1
	    
      self.RamSelect .destroy()     
      self.SelectionRebin.destroy() 
      self.ResumeOneSAP.destroy()
 
      current_dir = os.getcwd()
      
      cmd = """(cd %s/ LD_LIBRARY_PATH=/usr/local/lib/; %s/../src/ICD3-ICD6-Rebin/DynspecPart %s/ L%s  %s/  %s %s %s %s %s %s no %s %s)"""%(self.PATH0,current_dir,self.PATH0,self.observationID,self.outputDir,self.NEWTIMESTART,self.NEWTIMESTOP,self.NEWTIMEREBIN,self.NEW_FMIN,self.NEW_FMAX,self.NEW_CHANNELPERSUBBANDS,self.RAM,self.SAPSelected)
      print commands.getoutput(cmd)
      self.grandparent.quit()  	
	
	
    def ResumeOneSAPButtonNo_Go(self,event,ResumeOneSAP,SelectionRebin):
	    ResumeOneSAP.destroy()
	    self.SelectionRebinButton.config(state=NORMAL)	
      
      
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    ###########################################################################      
    #Process Info
    
    def ProcessInfoAllSAP(self):
      
	self.ProcessWindow      = Tk()
	self.ProcessWindow.title("Processing Information")      
	self.ProcessWindow.geometry("400x140+0+0")      
	self.parent17 = Frame(self.ProcessWindow)
	self.parent17.pack()       
	self.parent18 = Frame(self.ProcessWindow)
	self.parent18.pack()      
	self.parent19 = Frame(self.ProcessWindow)
	self.parent19.pack()             
	self.parent20 = Frame(self.ProcessWindow)
	self.parent20.pack()        
	self.parent21 = Frame(self.ProcessWindow)
	self.parent21.pack()        
	self.parent22 = Frame(self.ProcessWindow)
	self.parent22.pack()  

	self.StartTimeProcess	= time.asctime()
	self.StartInfo       	= Label(self.parent17,text=""" Start Processing at: %s"""%(self.StartTimeProcess))
	self.DataInfo      	= Label(self.parent18,text="""Data Flow to Process (Go): %s"""%(self.SizeEstimated))
	self.SAPInfo      	= Label(self.parent19,text="""Number of SAP to process: %s)"""%(self.NOFSAP))
	self.RAMOpInfo       	= Label(self.parent20,text="""Optimal RAM to process (Go): %s)"""%(self.RAMOptimum))
	self.RAMInfo       	= Label(self.parent21,text="""Selected RAM Quantity (Go): %s)"""%(self.RAM))
	self.TimeInfo      	= Label(self.parent22,text="""Optimal Processing Time Estimation (in s):  %s) """%(self.TimeEstimated))
	
	self.StartInfo.pack( side = LEFT)
	self.DataInfo.pack( side = LEFT)
	self.SAPInfo.pack( side = LEFT)
	self.RAMOpInfo.pack( side = LEFT)
	self.RAMInfo.pack( side = LEFT)
	self.TimeInfo.pack( side = LEFT)

	
    def ProcessInfoOneSAP(self):
      
	self.ProcessWindow      = Tk()
	self.ProcessWindow.title("Processing Information")      
	self.ProcessWindow.geometry("400x140+0+0")      
	self.parent17 = Frame(self.ProcessWindow)
	self.parent17.pack()       
	self.parent18 = Frame(self.ProcessWindow)
	self.parent18.pack()      
	self.parent19 = Frame(self.ProcessWindow)
	self.parent19.pack()             
	self.parent20 = Frame(self.ProcessWindow)
	self.parent20.pack()        
	self.parent21 = Frame(self.ProcessWindow)
	self.parent21.pack()        
	self.parent22 = Frame(self.ProcessWindow)
	self.parent22.pack()  

	self.StartTimeProcess	= time.asctime()
	self.StartInfo       	= Label(self.parent17,text=""" Start Processing at: %s"""%(self.StartTimeProcess))
	self.DataInfo      	= Label(self.parent18,text="""Data Flow to Process (Go): %s"""%(self.SizeEstimated))
	self.SAPInfo      	= Label(self.parent19,text="""Number of SAP to process: 1""")
	self.RAMOpInfo       	= Label(self.parent20,text="""Optimal RAM to process (Go): %s)"""%(self.RAMOptimum))
	self.RAMInfo       	= Label(self.parent21,text="""Selected RAM Quantity (Go): %s)"""%(self.RAM))
	self.TimeInfo      	= Label(self.parent22,text="""Optimal Processing Time Estimation (in s):  %s) """%(self.TimeEstimated))
	
	self.StartInfo.pack( side = LEFT)
	self.DataInfo.pack( side = LEFT)
	self.SAPInfo.pack( side = LEFT)
	self.RAMOpInfo.pack( side = LEFT)
	self.RAMInfo.pack( side = LEFT)
	self.TimeInfo.pack( side = LEFT)	
	

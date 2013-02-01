#!/usr/bin/python
# -*- coding: utf-8 -*-


#############################
# IMPORT COMMANDS
#############################

import sys, os, glob, commands
import tkFileDialog

from progress import PB
from time import sleep

from pylab import *
import matplotlib

import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure

from numpy import *
import numpy as np
import h5py

from Tkinter import *
from dal import *

#############################
# OPEN READ AND WRITE METADATA
#############################


class ICD6OpenFile():

    def __init__(self, parent, h5FilePath):
      
        # Attributes of ICD6OpenFile Class
        self.grandparent 	= parent
        self.PATH0 		= h5FilePath
        self.dynspec		= ""
        self.stokesArray	= ""
        self.selectedStokes	= ""
        self.dataSet		= ""
        self.decile		= ""
        
        self.dataSetRebin	= 0
	self.minFrequency	= 0
	self.maxFrequency	= 0
	self.spectralBinningMini = 0
        self.totalTime		= 0
	self.plotLevel		= 2.13
	self.timeIncrement	= 0
	self.dims		= 0
	self.timePixel0		= 0
	self.spectralPixel0	= 0	

	self.NofSubband			= ''
	self.NofChannelPerSubband	= ''
	self.ChannelWidth		= ''
	self.FrequencyTotalBanWidth	= ''
	self.AXIS_VALUE_WORLD		= ''
	
	
	
        ### Define Left and Right Parents for organising windows 		
	###############################################
        self.leftparent = Frame(self.grandparent, width=224)
        self.leftparent.pack(side=LEFT, fill=Y)
        
        
        self.rightparent = Frame(self.grandparent)
        self.rightparent.pack(side=RIGHT, fill=BOTH)       		
        
        self.rightparentup = Frame(self.rightparent, height = 100)
        self.rightparentup.pack(side=TOP, fill=X)  

        self.rightparentdown = Frame(self.rightparent)
        self.rightparentdown.pack(side=TOP, fill=BOTH)         
        
        
        self.rightparentupleft = Frame(self.rightparentup, width= 300, height = 100)
        self.rightparentupleft.pack(side=LEFT, fill=Y)        
        self.rightparentupright = Frame(self.rightparentup, width= 300, height = 100)
        self.rightparentupright.pack(anchor=NE)        
        

        
        ### Frames for Left Parent
        
	# FRAME N°0 (Frame for loading Dynspec => Listbox)
        self.lframe0 = LabelFrame(self.leftparent, text="Dynamic Spectra",width=224, height=15)
        self.lframe0.pack(anchor=NW)
	### Listbox for dynspec
	yscrollbar0 = Scrollbar(self.lframe0,orient=VERTICAL )
	yscrollbar0.pack(side=RIGHT, fill= Y)		
	self.dynspecTable = Listbox(self.lframe0,width=30, height=15,yscrollcommand=yscrollbar0.set) 
	### Load data file
	h5File = File(h5FilePath, File.READ)
	nofDynspec = AttributeInt(h5File, "NOF_DYNSPEC")
	k=range(nofDynspec.value)
	for i in k:
	    if i < 10:
		self.dynspecTable.insert(i,"DYNSPEC_00%s"%(i))
	    if i > 9 and k<100:
		self.dynspecTable.insert(i,"DYNSPEC_0%s"%(i))
	    if i > 99 and k<1000:
		self.dynspecTable.insert(i,"DYNSPEC_%s"%(i))
	### Listbox generation
	self.dynspecTable.pack()
	yscrollbar0.config(command=self.dynspecTable.yview)		
	
	
	
	### Frame N°1 (Frame for loading Metadata => TextBox)
	self.lframe1 = LabelFrame(self.leftparent, text="Metadata",width=224, height=15)
        self.lframe1.pack(anchor=W)
	### Message box for showing selected metadata
	xscrollbar1 = Scrollbar(self.lframe1, orient=HORIZONTAL)
	xscrollbar1.pack(side=BOTTOM, fill=X)
	yscrollbar1 = Scrollbar(self.lframe1, orient=VERTICAL)
	yscrollbar1.pack(side=RIGHT, fill=Y)
	self.dynspecMetaData = Text(self.lframe1,width=34, height=15, wrap=NONE,xscrollcommand=xscrollbar1.set,yscrollcommand=yscrollbar1.set)
	self.dynspecMetaData.pack()
	xscrollbar1.config(command=self.dynspecMetaData.xview)
	yscrollbar1.config(command=self.dynspecMetaData.yview)	
	
	
	### Frame N°2 (Stockes Parameter Selection)
	self.lframe2 = LabelFrame(self.leftparent, text="Click to select Stokes Parameter",width=224, height=4)
        self.lframe2.pack(anchor=W)
	### Listbox for select Stokes parameter
	self.StokesTable = Listbox(self.lframe2,width=32, height=4)
	self.StokesTable.pack()
	
	
	
	### Frame N°2b (Selection Resume)
	self.lframe2b = LabelFrame(self.leftparent, text="Resume Selection:",width=224, height=2)
        self.lframe2b.pack(anchor=W)
	### Listbox for select Stokes parameter
	self.resumeSelection = Listbox(self.lframe2b,width=32, height=2)
	self.resumeSelection.pack()	
	
	
	
	###FRAME N°3 (Action button)
	self.lframe3 = LabelFrame(self.leftparent, text="Actions", width=224, height=2)
        self.lframe3.pack(anchor=SW)      
	### Plot Button for plotting selected dynspec
	self.dplotBbutton = Button(self.lframe3, text="Plot (Non-Continoous) Dyn-Spec",width=29, height=2, padx=10, pady=3)
	self.dplotBbutton.config(state=DISABLED)
	self.dplotBbutton.pack()
	### Plot Button for plotting selected dynspec
	self.dplotBbutton2 = Button(self.lframe3, text="Plot (Continoous) Dyn-Spec",width=29, height=2, padx=10, pady=3)
	self.dplotBbutton2.config(state=DISABLED)
	self.dplotBbutton2.pack()	
	
	### Erase button for erase all and restart with another file
	self.eraseBbutton = Button(self.lframe3, text="Clear",width=29, height=2, padx=10, pady=3)
	self.eraseBbutton.pack()	
	
	
	
        
        ### Frame for Right Parent
        
        ### FRAME N°4a (Radio Button for contrast)
        self.lframe4a = LabelFrame(self.rightparentupleft, width=400, height=100, text="Contrast Button")
        self.lframe4a.pack(anchor=NW)
        self.contrastScale = Scale(self.lframe4a, variable = self.plotLevel,orient=HORIZONTAL, resolution = 0.1, from_=0.8, to = 10.0)
        self.contrastScale.config(state=DISABLED)
        self.contrastScale.pack(anchor=CENTER)
        
        self.contrastButton = Button(self.lframe4a, text="Change The Plot Maximum Value", command=self.updateGraph)
	self.contrastButton.pack(anchor=CENTER)
        self.contrastButton.config(state=DISABLED)
        self.contrastButton.pack(anchor=CENTER)
        
        
        ### FRAME N°4b (Resume the rebin parameters)
        self.lframe4b = LabelFrame(self.rightparentupright, width=400, height=100, text="Resume Rebin Parameters")
        self.resumeRebin = Listbox(self.lframe4b,width=60, height=4) 
        self.lframe4b.pack(anchor=NE)
        
        
        ### FRAME N°4 (Caneva, plot the data)
        self.lframe4 = LabelFrame(self.rightparentdown, width=800, height=650, text="Plotting The LOFAR Dynamic Spectrum Data")
        self.lframe4.pack(anchor=N)
        
        ### Data contained in Figure 
        self.figure = Figure(figsize=(8,6.5), dpi=100)
        
        ### Canvas which contains the figure and the toolbar
        self.canvas = FigureCanvasTkAgg(self.figure, master=self.lframe4)
        self.canvas.get_tk_widget().pack(anchor=NE)
        
        toolbar = NavigationToolbar2TkAgg(self.canvas, self.lframe4)
        toolbar.update()
        self.canvas._tkcanvas.pack(anchor=NE)
        self.canvas.show()      
        
        
	### Actions for All Buttons and Listbox etc ... 
	##############################################
	
        ### Active buttons for Frame N°0 and N°1 
	self.dynspecTable.bind("<Double-Button-1>", lambda event, dynspecTable = self.dynspecTable, dynspecMetaData=self.dynspecMetaData, StokesTable = self.StokesTable, resumeSelection = self.resumeSelection : self.selectionMetaData(self,dynspecTable,dynspecMetaData,StokesTable,resumeSelection))
	self.StokesTable.bind("<Double-Button-1>", lambda event, StokesTable = self.StokesTable, resumeSelection = self.resumeSelection : self.selectionStokes(self,StokesTable,resumeSelection))
        ### Active buttons for Frame N°3
        self.dplotBbutton.bind("<Double-Button-1>", lambda event, dynspecTable = self.dynspecTable, dynspecMetaData=self.dynspecMetaData, figure = self.figure, canvas = self.canvas, lframe4 = self.lframe4, plotLevel = self.plotLevel : self.selectionPlot(self,dynspecTable,dynspecMetaData, figure, canvas, lframe4, plotLevel))
        self.dplotBbutton2.bind("<Double-Button-1>", lambda event, dynspecTable = self.dynspecTable, dynspecMetaData=self.dynspecMetaData, figure = self.figure, canvas = self.canvas, lframe4 = self.lframe4, plotLevel = self.plotLevel : self.selectionPlot2(self,dynspecTable,dynspecMetaData, figure, canvas, lframe4, plotLevel))
        self.eraseBbutton.bind("<Double-Button-1>", lambda event, lframe0 = self.lframe0, lframe1 = self.lframe1, lframe2 = self.lframe2, lframe2b = self.lframe2b, lframe3 = self.lframe3, lframe4 = self.lframe4, leftparent = self.leftparent, rightparent = self.rightparent : self.selectionErase(self,lframe0,lframe1,lframe2,lframe2b,lframe3,lframe4,leftparent,rightparent))
                
        
        
    ###############################################"    
    # Erase Function     
    ###############################################"    
    def selectionErase(self,event,lframe0,lframe1,lframe2,lframe2b,lframe3,lframe4,leftparent,rightparent):	
	lframe0.destroy()
	lframe1.destroy()
	lframe2.destroy()
	lframe2b.destroy()
	lframe3.destroy()
	lframe4.destroy()
	leftparent.destroy()
	rightparent.destroy()
	
	
    def eraseObject(self):	
	self.lframe0.destroy()
	self.lframe1.destroy()	
	self.lframe2.destroy()
	self.lframe2b.destroy()
	self.lframe3.destroy()
	self.lframe4.destroy()
	self.leftparent.destroy()
	self.rightparent.destroy()
	
	
	
    ###############################################"    
    # Selection Dynspec => plot metadata  Function     
    ###############################################" 	
    def selectionMetaData(self,event,dynspecTable,dynspecMetaData,StokesTable,resumeSelection):
	
	dynspecMetaData.config(state=NORMAL)
	dynspecMetaData.delete(1.0, END)

	StokesTable.config(state=NORMAL)
	StokesTable.delete(0, END)
	
	resumeSelection.config(state=NORMAL)
	resumeSelection.delete(0, END)	
	
	nofSelectedDynspec = int(dynspecTable.curselection()[0])
	
	if nofSelectedDynspec < 10:
	    pathDynspec = "DYNSPEC_00%s"%(nofSelectedDynspec)
        if nofSelectedDynspec > 9 and nofSelectedDynspec<100:
	    pathDynspec = "DYNSPEC_0%s"%(nofSelectedDynspec)
        if nofSelectedDynspec > 99 and nofSelectedDynspec<1000:   
            pathDynspec = "DYNSPEC_%s"%(nofSelectedDynspec)
        
        self.dynspec = pathDynspec
        
        h5File = File(self.PATH0, File.READ)
        
        selectedDynspec= Group(h5File, pathDynspec)
        selectCoordinates = Group(selectedDynspec,"COORDINATES")
        selectTime = Group(selectCoordinates,"TIME")
        selectSpectral = Group(selectCoordinates,"SPECTRAL")
        selectPolarization=Group(selectCoordinates,"POLARIZATION")        
        
        selectedDynspec= Group(h5File, pathDynspec)

        
        
        obsID = AttributeString(h5File, "OBSERVATION_ID")
        obsTarget=AttributeString(h5File, "TARGET")
        obsRA = AttributeFloat(selectedDynspec, "POINT_RA")
        obsDEC = AttributeFloat(selectedDynspec, "POINT_DEC")
        obsStationList = AttributeVString(selectedDynspec, "BEAM_STATIONS_LIST")
        obsNuMin = AttributeFloat(selectedDynspec,"BEAM_FREQUENCY_MIN")
        obsNuMax = AttributeFloat(selectedDynspec, "BEAM_FREQUENCY_MAX") 
        obsNuCenter= AttributeFloat(selectedDynspec, "BEAM_FREQUENCY_CENTER")
        obsNuStep = AttributeFloat(selectedDynspec, "DYNSPEC_BANDWIDTH")
        obsTimeIncrement = AttributeVFloat(selectTime, "INCREMENT")
        obsDuration = AttributeFloat(h5File, "TOTAL_INTEGRATION_TIME")
        obsTstart = AttributeString(h5File, "OBSERVATION_START_UTC")
        obsTstop = AttributeString(h5File, "OBSERVATION_END_UTC")
        obsStokes = AttributeVString(selectPolarization,"AXIS_NAMES") 

	obsNofFrequencyChannel		= AttributeVFloat(selectSpectral,"AXIS_VALUE_WORLD")
	obsNofChannelPerSubband		= AttributeFloat(h5File,"CHANNELS_PER_SUBANDS")        
        obsChannelWidth			= AttributeFloat(h5File,"CHANNEL_WIDTH")
        
	dynspecMetaData.update()

 	dynspecMetaData.insert(1.0, """Observation ID: %s\n"""%(obsID.value))
 	dynspecMetaData.insert(2.0, """Target: %s\n"""%(obsTarget.value))
 	dynspecMetaData.insert(3.0, """Ra: %s\n"""%(obsRA.value)) 	
 	dynspecMetaData.insert(4.0, """Dec: %s\n"""%(obsDEC.value))
	dynspecMetaData.insert(5.0, """Station List: %s\n"""%(obsStationList.value))
	
	dynspecMetaData.insert(6.0, "\n")	

	dynspecMetaData.insert(7.0,  """Minimum Frequency : %s\n"""%(obsNuMin.value))
	dynspecMetaData.insert(8.0,  """Maximum Frequency : %s\n"""%(obsNuMax.value))
	dynspecMetaData.insert(9.0,  """Center Frequency : %s\n"""%(obsNuCenter.value))
	dynspecMetaData.insert(10.0,  """Frequency Band Width : %s\n"""%(obsNuStep.value))
	dynspecMetaData.insert(11.0, """Frequency Unit : MHz\n""")
	dynspecMetaData.insert(12.0, """Stokes Parameter : %s\n"""%(obsStokes.value))
	
	dynspecMetaData.insert(13.0, "\n")	
	
	dynspecMetaData.insert(14.0, """Time Increment : %s\n"""%(obsTimeIncrement.value))
	dynspecMetaData.insert(15.0, """Total Duration : %s\n"""%(obsDuration.value))
	dynspecMetaData.insert(16.0, """UTC Start Time : %s\n"""%(obsTstart.value))
	dynspecMetaData.insert(17.0, """UTC Stop Time : %s\n"""%(obsTstop.value))

	
	dynspecMetaData.update()
	
	self.stokesArray = obsStokes.value
	
	
	### Determine Stockes Selection for ploting
	########################
	
	k = range(len(self.stokesArray))
	for i in k:
	    StokesTable.insert(i, "Stokes Parameter : %s "%(self.stokesArray[i]))

	########################
	
	resumeSelection.insert(0, "Selected Dynspec : %s"%(self.dynspec))
	if self.selectedStokes != "":
	    indexStockesArray = self.selectedStokes
	    resumeSelection.insert(1, "Selected Stokes  : %s"%(self.stokesArray[indexStockesArray]))
	else:
	    resumeSelection.insert(1, "Selected Stokes  : ")

	
	if self.dynspec != "" and self.selectedStokes != "":
	    self.dplotBbutton.config(state=NORMAL)
	
	
	### Fix time and frequency bins 
	########################

	self.minFrequency	= obsNuMin.value
	self.maxFrequency	= obsNuMax.value	
	self.totalTime		= obsDuration.value

	self.NofChannelPerSubband	= obsNofChannelPerSubband.value
	self.NofSubband			= len(obsNofFrequencyChannel.value)/self.NofChannelPerSubband
	self.ChannelWidth		= obsChannelWidth.value
	self.AXIS_VALUE_WORLD		= obsNofFrequencyChannel.value
	
	self.FrequencyTotalBanWidth	= obsNuStep.value
	
    ###############################################"    
    # Select Sotckes parameter    
    ###############################################" 	
    def selectionStokes(self,event,StokesTable,resumeSelection):

	resumeSelection.config(state=NORMAL)
	resumeSelection.delete(0, END)	    
    
	self.selectedStokes = int(StokesTable.curselection()[0])

	resumeSelection.insert(0, "Selected Dynspec : %s"%(self.dynspec))
	if self.selectedStokes != "":
	    indexStockesArray = self.selectedStokes
	    resumeSelection.insert(1, "Selected Stokes  : %s"%(self.stokesArray[indexStockesArray]))
	else:
	    resumeSelection.insert(1, "Selected Stokes  : ")

	if self.dynspec != "" and self.selectedStokes != "":
	    self.dplotBbutton.config(state=NORMAL)	    
	    self.dplotBbutton2.config(state=NORMAL)	 
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    ###############################################"    
    # Plot  dynspec     
    ###############################################" 	
    def selectionPlot(self,event,dynspecTable,dynspecMetaData,figure,canvas,lframe4,plotLevel):
        

	## Open the hdf5 file
	pathDynspec = self.dynspec
	h5File = h5py.File(self.PATH0,'r')
	selectedDynspec = h5File[pathDynspec]
	self.dataSet = selectedDynspec['DATA'].value
	h5File.close()
	
	
	## Determine the dataset dimension
	self.dims = self.dataSet.shape	      
	      
	NofSpectral	= int((self.maxFrequency-self.minFrequency)/self.ChannelWidth)
	self.timePixel0		= self.dims[0]
	self.spectralPixel0	= NofSpectral	      
	      
	      
	if self.timePixel0*self.spectralPixel0	 > 200E6:       
	      
	      self.MsgBox	= Tk()
	      self.MsgBox.title("Processing Information")      
	      self.MsgBox.geometry("400x130+0+0")
	      
	      self.parentInfo = Frame(self.MsgBox)
	      self.parentInfo.pack(anchor=CENTER) 
	
	      self.Info  = Label(self.parentInfo,text=""" The generated matrix for plotting this dataset with gap \n between subbands is too heavy for visualization.\n\n You have to extract another lighter dataset ! \n\nLimit is fixed at 200E6 elements to plot,\n This dataset need to plot: %s elements"""%(self.timePixel0*self.spectralPixel0))
	      self.Info.pack( anchor = CENTER)
	      
	      self.InfoButton	= Button(self.parentInfo,text="OK",width=9, height=1)
	      self.InfoButton.pack( anchor = CENTER)
	    
	      self.InfoButton.bind("<Button-1>", lambda event, MsgBox = self.MsgBox  : self.InfoOk(self,MsgBox))  	      
	     
	     
	else:      
	      
        
        
	      ## Clean the figure
	      self.figure.clear()
	      
	      ## Open the Progress bar
	      bar = PB(300, 25)
	      bar.open()
	      bar.update(0)
	      
	      
	      ## Progress bar 10%
	      bar.update(0.1)
	    
	      ##### Decile (10% median histogramme) correction
	      kdecile= range(self.dims[1])
	      self.decile = np.zeros(self.dims[1])
	      
	      if sum(self.dataSet[:,:,0]) !=0:
	      
		    for i in kdecile:
		      
			if i == int(self.dims[1]/2):
			    bar.update(0.3)
			
			self.dataSetDecile = list(np.sort(self.dataSet[:,i,self.selectedStokes]))	    
			while self.dataSetDecile[0] == 0:
			    self.dataSetDecile.remove(0)
			    
			limit = int(len(self.dataSetDecile)/10)
			self.decile[i] = self.dataSetDecile[limit]
			if self.decile[i] >0:
				  self.dataSet[:,i,self.selectedStokes] = self.dataSet[:,i,self.selectedStokes]/self.decile[i]
			else:
				  self.dataSet[:,i,self.selectedStokes]  = 0
			
	      
	      # Progress bar 30%
	      bar.update(0.3)
		      
	     
	      

		

	      self.dataSetRebin	= np.zeros(shape=(self.dims[0],NofSpectral))
	      kTimeRebin0 = range(self.dims[0])
	      kSpecRebin  = range(self.dims[1])
	      kSpecRebin0 = range(NofSpectral)
	      
	      for j in kSpecRebin:
		  if j == round(self.dims[1]/3):
		      bar.update(0.5)
		  if j == round(self.dims[1]/3*2):
		      bar.update(0.7)		      
	        
	          for k in kSpecRebin0:
		      freqTemp = self.minFrequency+k*self.ChannelWidth

		      if round(self.AXIS_VALUE_WORLD[j]/1E6,2) == round(freqTemp,2):
		
			      self.dataSetRebin[:,k] = self.dataSet[:,j,self.selectedStokes]
			      
	      bar.update(0.9)
	      ## Bonne fenetre de plot 
		
	      kTimeRebin0 = range(self.timePixel0)
	      kSpecRebin0 = range(self.spectralPixel0)	
		
	      kTimeRebin0 = range(self.dims[0])
	      kSpecRebin0 = range(self.dims[1])
	      
	      timeComp2Sum0 = int(self.dims[0]/(self.timePixel0))
	      specComp2Sum0 = int(self.dims[1]/(self.spectralPixel0))
	      kTimeComp2Sum0 = range(timeComp2Sum0)
        	 	
	
	      # Progress bar 100% => Complete	      
	      bar.update(1)
	      sleep(1)
	      bar.close()
	          
	      ## Define X-Y Axis
	      binTime = float(self.dims[0])
	      self.totalTime = float(self.totalTime)
	      self.timeIncrement= float(self.totalTime/binTime)
	      self.spectralBinningMini = float((self.maxFrequency-self.minFrequency)/self.dims[1]*specComp2Sum0)  
	        
	      #### Resume Rebinning Window
	      self.resumeRebin.insert(0,"Number of Time Pixel: %s"%(self.timePixel0))
	      self.resumeRebin.insert(1,"Time Scale (s): %s"%(self.timeIncrement*timeComp2Sum0))
	      self.resumeRebin.insert(2,"Number of Spectral Pixel: %s"%(self.spectralPixel0))	
	      self.resumeRebin.insert(3,"Spectral Scale (MHz): %s"%((self.maxFrequency-self.minFrequency)/self.dims[1]*specComp2Sum0))
	      self.resumeRebin.pack()
	          
	      
	      
	      ## Real Data
	      flatdata=copy(self.dataSetRebin)
	      
	      ## Plot Rebin0
	      subPlot = self.figure.add_subplot(111)
	      plotBox = subPlot.imshow(flatdata, vmin=0.1, vmax=self.plotLevel, aspect='auto', extent=[self.minFrequency,self.maxFrequency,self.timePixel0*self.timeIncrement*timeComp2Sum0,0], interpolation='nearest') 
	      subPlot.set_xlabel('Frequency (MHz)')
	      subPlot.set_ylabel('Time (s)')	
	      colBox = self.figure.colorbar(plotBox)
	      colBox.set_label('Intensity Ratio')
	      self.canvas.show()
	      
	      self.contrastScale.config(state=NORMAL)
	      self.contrastButton.config(state=NORMAL)
	      	


	      	
	      	
	      	
	      	
	      	
	      	
    ###############################################"    
    # Plot  dynspec  Method 2   
    ###############################################" 	
    def selectionPlot2(self,event,dynspecTable,dynspecMetaData,figure,canvas,lframe4,plotLevel):
        

	## Open the hdf5 file
	pathDynspec = self.dynspec
	h5File = h5py.File(self.PATH0,'r')
	selectedDynspec = h5File[pathDynspec]
	self.dataSet = selectedDynspec['DATA'].value
	h5File.close()
	
	
	## Determine the dataset dimension
	self.dims = self.dataSet.shape	      
	      
	self.timePixel0		= self.dims[0]
	self.spectralPixel0	= int(self.dims[1])#+self.NofSubband*5)   
		      
	
	
	if self.timePixel0*self.spectralPixel0	 > 200E6:       
	      
	      self.MsgBox	= Tk()
	      self.MsgBox.title("Processing Information")      
	      self.MsgBox.geometry("400x130+0+0")
	      
	      self.parentInfo = Frame(self.MsgBox)
	      self.parentInfo.pack(anchor=CENTER) 
	
	      self.Info  = Label(self.parentInfo,text=""" The generated matrix for plotting this dataset with gap \n between subbands is too heavy for visualization.\n\n You have to extract another lighter dataset ! \n\nLimit is fixed at 200E6 elements to plot,\n This dataset need to plot: %s elements"""%(self.timePixel0*self.spectralPixel0))
	      self.Info.pack( anchor = CENTER)
	      
	      self.InfoButton	= Button(self.parentInfo,text="OK",width=9, height=1)
	      self.InfoButton.pack( anchor = CENTER)
	    
	      self.InfoButton.bind("<Button-1>", lambda event, MsgBox = self.MsgBox  : self.InfoOk(self,MsgBox))  	      
	     
	     
	else:      
	      
        
        
	      ## Clean the figure
	      self.figure.clear()
	      
	      ## Open the Progress bar
	      bar = PB(300, 25)
	      bar.open()
	      bar.update(0)
	      
	      
	      ## Progress bar 10%
	      bar.update(0.1)
	    
	      if sum(self.dataSet[:,:,0]) !=0:
	    
		    ##### Decile (10% median histogramme) correction
		    kdecile= range(self.dims[1])
		    self.decile = np.zeros(self.dims[1])
		    for i in kdecile:		
			if i == int(self.dims[1]/2):
			    bar.update(0.3)		  
			self.dataSetDecile = list(np.sort(self.dataSet[:,i,self.selectedStokes]))	    
			while self.dataSetDecile[0] == 0:
			    self.dataSetDecile.remove(0)		      
			limit = int(len(self.dataSetDecile)/10)
			self.decile[i] = self.dataSetDecile[limit]
			if self.decile[i] >0:
				  self.dataSet[:,i,self.selectedStokes] = self.dataSet[:,i,self.selectedStokes]/self.decile[i]
			else:
				  self.dataSet[:,i,self.selectedStokes]  = 0		  	      
		    # Progress bar 30%
	      bar.update(0.3)
		      
	      
	      
	      ###### Generate Data Matrix (included non continous band)  
	      
	      
	      self.dataSetRebin	= np.zeros(shape=(self.dims[0],self.dims[1]))


	      #kTimeRebin0 = range(self.timePixel0)
	      #kSpecRebin0 = range(self.spectralPixel0)	
	      
	      kTimeRebin0 = range(self.dims[0])
	      kSpecRebin0 = range(self.dims[1])
	      
	      timeComp2Sum0 = int(self.dims[0]/(self.timePixel0))
	      specComp2Sum0 = int(self.dims[1]/(self.spectralPixel0))
	      kTimeComp2Sum0 = range(timeComp2Sum0)
		      
	      for i in kTimeRebin0:
		
		  #if i == int(self.timePixel0/4):
		      ## Progress bar 40%
		      #bar.update(0.4)
		  #if i == int(self.timePixel0/2):
		      ## Progress bar 70%
		      #bar.update(0.7)
		  #if i == int(self.timePixel0/4*3):
		      ## Progress bar 90%
		      #bar.update(0.9)

		  for j in kSpecRebin0:	  		    
		      #for k in kTimeComp2Sum0:
			      #dataSetRebin0[i,j] = sum(self.dataSet[i*timeComp2Sum0+k,j*specComp2Sum0:(j+1)*specComp2Sum0-1,self.selectedStokes])+dataSetRebin0[i,j]		  
		      #dataSetRebin0[i,j] 	= dataSetRebin0[i,j]/(timeComp2Sum0*specComp2Sum0)
		      #self.dataSetRebin[i,j]	= dataSetRebin0[i,j]
		      self.dataSetRebin[i,j] = self.dataSet[i,j,self.selectedStokes]	      
	      
	      
	      
	      
	      
	      
	      
	      
	      
	      
	      
	      
	      
		
	      #self.timePixel0		= self.dims[0]
	      #self.spectralPixel0	= self.dims[1]
		
	      #self.dataSetRebin	= np.zeros(shape=(self.dims[0],self.dims[1]))
	      #kTimeRebin0 = range(self.dims[0])
	      #kSpecRebin0 = range(self.dims[1])
	      #for i in kTimeRebin0:	 
		    #if i == int(self.timePixel0/2):
			  ## Progress bar 70%
			  #bar.update(0.7)
			  #for j in kSpecRebin0:	  		    
			      #self.dataSetRebin[i,j] = self.dataSet[i,j,self.selectedStokes]	
			      #print self.dataSetRebin[i,j]

		

	      #self.dataSetRebin	= np.zeros(shape=(self.dims[0],NofSpectral))
	      #kTimeRebin0 = range(self.dims[0])
	      #kSpecRebin  = range(self.dims[1])
	      #kSpecRebin0 = range(NofSpectral)
	      
	      #for j in kSpecRebin:
		  #if j == round(self.dims[1]/3):
		      #bar.update(0.5)
		  #if j == round(self.dims[1]/3*2):
		      #bar.update(0.7)		      
	        
	          #for k in kSpecRebin0:
		      #freqTemp = self.minFrequency+k*self.ChannelWidth

		      #if round(self.AXIS_VALUE_WORLD[j]/1E6,3) == round(freqTemp,3):
		
			      #self.dataSetRebin[:,k] = self.dataSet[:,j,self.selectedStokes]
		
		
		
		
		
		
		
		
		
		
		
		
		
		
	      bar.update(0.9)
	      ## Bonne fenetre de plot 
		
	      kTimeRebin0 = range(self.timePixel0)
	      kSpecRebin0 = range(self.spectralPixel0)	
		
	      kTimeRebin0 = range(self.dims[0])
	      kSpecRebin0 = range(self.dims[1])
	      
	      timeComp2Sum0 = int(self.dims[0]/(self.timePixel0))
	      specComp2Sum0 = int(self.dims[1]/(self.spectralPixel0))
	      kTimeComp2Sum0 = range(timeComp2Sum0)
        	 	
	
	      # Progress bar 100% => Complete	      
	      bar.update(1)
	      sleep(1)
	      bar.close()
	          
	      ## Define X-Y Axis
	      binTime = float(self.dims[0])
	      self.totalTime = float(self.totalTime)
	      self.timeIncrement= float(self.totalTime/binTime)
	      self.spectralBinningMini = float((self.maxFrequency-self.minFrequency)/self.dims[1]*specComp2Sum0)  
	        
	      #### Resume Rebinning Window
	      self.resumeRebin.insert(0,"Number of Time Pixel: %s"%(self.timePixel0))
	      self.resumeRebin.insert(1,"Time Scale (s): %s"%(self.timeIncrement*timeComp2Sum0))
	      self.resumeRebin.insert(2,"Number of Spectral Pixel: %s"%(self.spectralPixel0))	
	      self.resumeRebin.insert(3,"Spectral Scale (MHz): %s"%((self.maxFrequency-self.minFrequency)/self.dims[1]*specComp2Sum0))
	      self.resumeRebin.pack()
	          
	      
	      
	      ## Real Data
	      flatdata=copy(self.dataSetRebin)
	      
	      ## Plot Rebin0
	      subPlot = self.figure.add_subplot(111)
	      plotBox = subPlot.imshow(flatdata, vmin=0.1, vmax=self.plotLevel, aspect='auto', extent=[self.minFrequency,self.maxFrequency,self.timePixel0*self.timeIncrement*timeComp2Sum0,0]) 
	      subPlot.set_xlabel('Frequency (MHz)')
	      subPlot.set_ylabel('Time (s)')	
	      colBox = self.figure.colorbar(plotBox)
	      colBox.set_label('Intensity Ratio')
	      self.canvas.show()
	      
	      self.contrastScale.config(state=NORMAL)
	      self.contrastButton.config(state=NORMAL)	      	
	      	
	      	
	      	
	      	
	      	
	      	
	      	
	      	
	      	
	      	
	      	
	      	
	      	
	      	
	      	
	
    def	updateGraph(self):
	
	self.plotLevel = self.contrastScale.get()
	#self.selectionPlot(self,self.dynspecTable,self.dynspecMetaData, self.figure, self.canvas, self.lframe4, self.plotLevel)
	
	
	## Clean the figure
        self.figure.clear()
	 
	    
	kTimeRebin0 = range(self.timePixel0)
	kSpecRebin0 = range(self.spectralPixel0)
        
	timeComp2Sum0 = int(self.dims[0]/(self.timePixel0))
	specComp2Sum0 = int(self.dims[1]/(self.spectralPixel0))
	    		  
	## Define X-Y Axis
	binTime = float(self.dims[0])
	totalTime = float(self.totalTime)
	timeIncrement= float(totalTime/binTime)
        
        ### Plotting Process
        
        flatdata=copy(self.dataSetRebin)
        
        ## Plot Rebin0
        subPlot = self.figure.add_subplot(111)
        plotBox = subPlot.imshow(flatdata,vmin=0.1, vmax=self.plotLevel, aspect='auto', extent=[self.minFrequency,self.maxFrequency,self.timePixel0*timeIncrement*timeComp2Sum0,0]) 
        subPlot.set_xlabel('Frequency (MHz)')
	subPlot.set_ylabel('Time (s)')	
        colBox = self.figure.colorbar(plotBox)
        colBox.set_label('Intensity Ratio')
	self.canvas.show()
	



    def InfoOk(self,event,MsgBox):
	    MsgBox.destroy()
	    



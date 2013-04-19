#!/usr/bin/python
# -*- coding: utf-8 -*-


#############################
# Quick look tool for ICD3 data visualization
# 25/01/2013
# © Nicolas Vilchez for ASTRON
# Nicolas.vilchez@cnrs-orleans.fr
#############################




#############################
# IMPORT COMMANDS
#############################

import sys, os, glob, commands, time
from pylab import *

import getopt
import matplotlib.pyplot as plt
from matplotlib.figure import Figure

import numpy as np
import  h5py
from h5py import *
from dal import *




#############################
# check the Launch
#############################

def main(initparameterlist):
	try:
	      opts, args = getopt.getopt(sys.argv[1:], "hiooppt:", ["help","id=","obsDIR=","outputDIR=","percentTimeData=","percentSpectralData=","transpose="])
	      
      
	except getopt.GetoptError as err:
	      print ""
	      print "Usage: ./ICD3-QuickLook.py --id=Lxxx --obsDIR= --outputDIR= --percentTimeData=[0->1] --percentSpectralData=[0->1] --transpose=[yes or no]"
	      print ""
	      sys.exit(2)
	      
	
	for par1, par2 in opts:
		
		if par1 in ("--help"):
			print ""
			print "Usage: ./ICD3-QuickLook.py --id=Lxxx --obsDIR= --outputDIR= --percentTimeData=[0->1] --percentSpectralData=[0->1] --transpose=[yes or no]"
			print ""
        		sys.exit(2)
        	
		elif par1 in ("--id"):
			initparameterlist[0]=par2
		elif par1 in ("--obsDIR"):
			initparameterlist[1]=par2
		elif par1 in ("--outputDIR"):
			initparameterlist[2]=par2
		elif par1 in ("--percentTimeData"):
			initparameterlist[3]=par2
		elif par1 in ("--percentSpectralData"):
			initparameterlist[4]=par2
		elif par1 in ("--transpose"):
			initparameterlist[5]=par2			
		else:
        		print("Option {} inconnue".format(par1))
        		sys.exit(2)


        		
	if initparameterlist[0] == "none" or initparameterlist[1] == "none" or initparameterlist[2] == "none" or initparameterlist[3] == "none" or initparameterlist[4] == "none" or initparameterlist[5] == "none":
		print ""
		print "MISSING Parameters"	
		print "Usage: ./ICD3-QuickLook.py --id=Lxxx --obsDIR= --outputDIR= --percentTimeData=[0->1] --percentSpectralData=[0->1] --transpose=[yes or no]"
		print ""
        	sys.exit(2)
		
							
			
	return initparameterlist 


#############################
# ICD3 List Etablishment
#############################

def findData(obsDir,obsID,outputList):
    	  
    
      cmd = """find %s -name *%s"""%(obsDir,obsID)+"*bf.h5  > "+"""%s"""%(outputList)
      print commands.getoutput(cmd)
      
      ICD3Inlist	= open(outputList,'r').readlines()
      NbICD3File	= len(ICD3Inlist)
      krange		= range(NbICD3File)
      
      pathFile		= range(NbICD3File)
      pathFiletemp	= range(NbICD3File)
      File		= range(NbICD3File)
      Filetemp		= range(NbICD3File)
      
      i = 0
      for line in ICD3Inlist:
	  splinter		= "L_S"
	  linesplit		= line.split(splinter)
	  pathFiletemp[i]	= linesplit[0]
	  File[i]		= linesplit[1]
	  Filetemp[i]		= linesplit[1]
	  i = i+1
      


      File.sort()
      for i in krange:
	  for j in krange:
	      if File[i] == Filetemp[j]:
		  pathFile[i] = pathFiletemp[j]		  
      
      listTxt	= open(outputList,'w')
      for i in krange:
	  cmd ="""%sL_S%s"""%(pathFile[i],File[i])
	  listTxt.write(cmd)
      listTxt.close()
      
      return outputList

      
#############################
# Matrix for Quicklook
#############################

def matrixGeneration(outputList,dataTimePercent,dataSpectralPercent,outputListDir,obsID,paramList,obsDir): 

      ICD3Inlist	= open(outputList,'r').readlines()
      NbICD3File	= len(ICD3Inlist)
      krange		= range(NbICD3File)

      
      # Test if list is empty
      if not ICD3Inlist:
	  print "generated list of ICD3 files is empty:"
	  print """%s observation must not be in %s"""%(obsID,obsDir)
	  print ""
	  exit()      
      
      i = 0
      for line in ICD3Inlist:
	  linesplit	= line.split()
	  h5File 		= File(linesplit[0], File.READWRITE)
	  telescope		= AttributeString(h5File, "TELESCOPE")
	  telescope.value	= "LOFAR" 	  
	  if i == 0:
		Path0 = linesplit[0]
	  i=i+1

	  
      h5File 	= File(Path0, File.READ)     
      SAP	= Group(h5File, "SUB_ARRAY_POINTING_000")
      BEAM	= Group(SAP,"BEAM_000")
      
      DATASET   = DatasetFloat(BEAM, "STOKES_0")
      
      selectCoordinates	= Group(BEAM,"COORDINATES")
      selectSpectral 	= Group(selectCoordinates,"COORDINATE_1")      


      samplingTimeObj	= AttributeFloat(BEAM, "SAMPLING_TIME")
      nofSampleObj	= AttributeInt(DATASET, "NOF_SAMPLES")
      freqCenterObj	= AttributeFloat(BEAM, "BEAM_FREQUENCY_CENTER")
      bandWidthObj	= AttributeFloat(h5File, "BANDWIDTH")      
      chanPerSubbandObj	= AttributeInt(BEAM, "CHANNELS_PER_SUBBAND")
      nofSubbandsObj	= AttributeInt(DATASET, "NOF_SUBBANDS")
      
      nbSAP	= 1
      nbBEAM	= 1
      nbSTOKES 	= 4
      
      nofSubbands	= nofSubbandsObj.value
      duration		= (samplingTimeObj.value)*(nofSampleObj.value)
      freqMin		= (freqCenterObj.value)-((bandWidthObj.value)/2.0)
      freqMax		= (freqCenterObj.value)+((bandWidthObj.value)/2.0)     
      ChannelPerSub	= chanPerSubbandObj.value 
      ChannelWidth	= bandWidthObj.value/(nofSubbands*ChannelPerSub)
      
      #List-of-Files Output-dir ObsName tmax fmin fmax NofSAP nofBEAM nofStokes
      
      current_dir = os.getcwd()
      
      cmd = """%s/src/ICD3-ICD6-Quicklook/DynspecQuick_Standalone %s %s  %s %s %s %s %s %s %s %s %s"""%(current_dir,outputList,outputListDir,obsID,duration,freqMin,freqMax,nbSAP,nbBEAM,nbSTOKES,dataTimePercent,dataSpectralPercent)
      print commands.getoutput(cmd)
      
      
      paramList[0]	= nbSAP
      paramList[1]	= nbBEAM
      paramList[2]	= duration
      paramList[3]	= freqMin     
      paramList[4]	= freqMax
      paramList[5]	= ChannelWidth
      paramList[6]	= ChannelPerSub
      
      return paramList
      

###################################
# Plot generation for the Quicklook
################################### 

def quickLookVisu(obsID,paramList,outputListDir,transpose):
 
 
      nbSAP  			= paramList[0]
      nbBEAM			= paramList[1]
      duration			= paramList[2]
      freqMin			= paramList[3]   
      freqMax			= paramList[4]     
      ChannelWidth		= paramList[5]  
      ChannelPerSub		= paramList[6]
     
      kSAP	= range(nbSAP)
      kBEAM	= range(nbBEAM)
      
      for i in kSAP:
	  if i<10:
	      indexSAP = """00%s"""%(i)
	  if i>=10 and i<100 :
	      indexSAP = """0%s"""%(i)	  
	  if i>=100:
	      indexSAP = i	  
	      
	  pathSAP = outputListDir+"Dynspec_Standalone_Quicklook_"+obsID+".h5"
	  

	  ###############################"
	  ## Open the hdf5 file
	  ###############################"
	  h5File = h5py.File(pathSAP,'r')

	  for j in kBEAM:
	    
	      if j<10:
		  indexBEAM = """00%s"""%(j)
	      if j>=10 and i<100 :
		  indexBEAM = """0%s"""%(j)	  
	      if j>=100:
		  indexBEAM = j
	
	      ###############################"
	      ## Continous Plot 
	      ###############################"
	      
	      pathDynspec = """DYNSPEC_%s"""%(indexBEAM)
	      
	      h5File 		= h5py.File(pathSAP,'r')
	      selectedDynspec 	= h5File[pathDynspec]
	      dataSet		= selectedDynspec['DATA'].value
	      
	      Coord			= selectedDynspec['COORDINATES']
	      spectroGroup		= Coord['SPECTRAL']
	      AXIS_VALUE_WORLD_TEMPO	= spectroGroup.attrs.items()      
	      AXIS_VALUE_WORLD_TEMP 	= list(AXIS_VALUE_WORLD_TEMPO[6])
	      AXIS_VALUE_WORLD		= list(AXIS_VALUE_WORLD_TEMP[1])
	      
	      freqMin	= AXIS_VALUE_WORLD[0]/1E6
	      freqMax	= AXIS_VALUE_WORLD[-1]/1E6	      
		          
	      
	      dims 	= dataSet.shape	
	      
	      ##### Decile (10% median histogramme) correction
	      kdecile= range(dims[1])
	      decile = np.zeros(dims[1])
	      
	      if sum(dataSet[:,:,0]) !=0:
	      
	      
		    for k in kdecile:
			
			dataSetDecile = list(np.sort(dataSet[:,k,0]))	    
			while dataSetDecile[0] == 0:
			      dataSetDecile.remove(0)
			    
			limit = int(len(dataSetDecile)/10)
			decile[k] = dataSetDecile[limit]
			if decile[k] >0:
			    dataSet[:,k,0] = dataSet[:,k,0]/decile[k]
			else:
			    dataSet[:,k,0] = 0
		      
		      
		    dataSetRebin	= np.zeros(shape=(dims[0],dims[1]))
		    
		    kTimeRebin0 = range(dims[0])
		    kSpecRebin0 = range(dims[1])
		    
		    for l in kTimeRebin0:
			for m in kSpecRebin0:	  		    
			    dataSetRebin[l,m] = dataSet[l,m,0]
			
			
		    plotMin	=  np.min(dataSetRebin[:,:])
		    plotMax	=  2.13
		    
		    
		    if transpose == "no":
		    
			  ##########################
			  #Plot and save the figure
			  pathFigure = outputListDir+"Dynspec_Standalone_Quicklook_"+obsID+"_SAP"+indexSAP+"_"+indexBEAM+".jpg"
			    
			  figure = plt.figure(figsize=(8,6.5), dpi=100)
			  
			  flatdata=copy(dataSetRebin)
			  
			  subPlot = figure.add_subplot(111)
			  plotBox = subPlot.imshow(flatdata, vmin=plotMin, vmax=plotMax, aspect='auto', extent=[freqMin,freqMax,duration,0], interpolation='nearest') 
			  subPlot.set_xlabel('Frequency (MHz)')
			  subPlot.set_ylabel('Time (s)')	
			  colBox = figure.colorbar(plotBox)
			  colBox.set_label('Intensity Ratio')
			  #plt.show()
			  figure.savefig(pathFigure, dpi=100)
			  figure.clear
	      
	      
		    if transpose == "yes":
		    
			  ##########################
			  #Plot and save the figure
			  pathFigure = outputListDir+"Dynspec_Standalone_Quicklook_"+obsID+"_SAP"+indexSAP+"_"+indexBEAM+".jpg"
			    
			  figure = plt.figure(figsize=(8,6.5), dpi=100)
			  
			  flatdata=copy(dataSetRebin.transpose())
			  
			  subPlot = figure.add_subplot(111)
			  plotBox = subPlot.imshow(flatdata, vmin=plotMin, vmax=plotMax, aspect='auto', extent=[0,duration,freqMax,freqMin], interpolation='nearest') 
			  subPlot.set_xlabel('Time (s)')
			  subPlot.set_ylabel('Frequency (MHz)')  
			  colBox = figure.colorbar(plotBox)
			  colBox.set_label('Intensity Ratio')
			  #plt.show()
			  figure.savefig(pathFigure, dpi=100)
			  figure.clear	      
	      
	      
	      
		    #############################################"
		    ## Non-Continous Plot 	      
		    ##############################################
		    
		    ## Determine the dataset dimension
		    

		    NofSpectral	= int((freqMax-freqMin)/(ChannelWidth*ChannelPerSub))
     
		    timePixel0		= dims[0]
		    spectralPixel0	= NofSpectral
		    		    
		    if spectralPixel0*timePixel0 < 1E9:  
		    
			  # Do the rebin with non-continous bands
			  dataSetRebin	= np.zeros(shape=(dims[0],NofSpectral))
			  kTimeRebin0 	= range(dims[0])
			  kSpecRebin  	= range(dims[1])
			  kSpecRebin0 	= range(NofSpectral)
			  
			  for l in kSpecRebin:
				for m in kSpecRebin0: 
				      freqTemp = (AXIS_VALUE_WORLD[0]/1E6)+m*(ChannelWidth*ChannelPerSub)
				      if round(AXIS_VALUE_WORLD[l]/1E6,3) == round(freqTemp,3):
					    dataSetRebin[:,m] = dataSet[:,l,0]
					    

			  kTimeRebin0 = range(timePixel0)
			  kSpecRebin0 = range(spectralPixel0)	
			  
			  kTimeRebin0 = range(dims[0])
			  kSpecRebin0 = range(dims[1])
			  
			  timeComp2Sum0 = int(dims[0]/(timePixel0))
			  specComp2Sum0 = int(dims[1]/(spectralPixel0))
			  kTimeComp2Sum0 = range(timeComp2Sum0)
			  

			  if transpose == "no":			  
				
				##########################
				#Plot and save the figure
				pathFigure = outputListDir+"Dynspec_Standalone_Quicklook_Non_Continous_"+obsID+"_SAP"+indexSAP+"_"+indexBEAM+".jpg"
				
				figure = plt.figure(figsize=(8,6.5), dpi=100)
				
				flatdata=copy(dataSetRebin)
				
				
				subPlot = figure.add_subplot(111)
				plotBox = subPlot.imshow(flatdata, vmin=plotMin, vmax=plotMax, aspect='auto', extent=[freqMin,freqMax,duration,0], interpolation='nearest') 
				subPlot.set_xlabel('Frequency (MHz)')
				subPlot.set_ylabel('Time (s)')	
				colBox = figure.colorbar(plotBox)
				colBox.set_label('Intensity Ratio')
				#plt.show()
				figure.savefig(pathFigure, dpi=100) 
				figure.clear
				
				
			  if transpose == "yes":			  
				
				##########################
				#Plot and save the figure
				pathFigure = outputListDir+"Dynspec_Standalone_Quicklook_Non_Continous_"+obsID+"_SAP"+indexSAP+"_"+indexBEAM+".jpg"
				
				figure = plt.figure(figsize=(8,6.5), dpi=100)
				
				flatdata=copy(dataSetRebin.transpose())
				
				subPlot = figure.add_subplot(111)
				plotBox = subPlot.imshow(flatdata, vmin=plotMin, vmax=plotMax, aspect='auto', extent=[0,duration,freqMax,freqMin], interpolation='nearest') 
				subPlot.set_xlabel('Time (s)')
				subPlot.set_ylabel('Frequency (MHz)')  
				colBox = figure.colorbar(plotBox)
				colBox.set_label('Intensity Ratio')
				#plt.show()
				figure.savefig(pathFigure, dpi=100)
				figure.clear	   				
				
				
				
				
		    else:
			print ""
			print "QuickLook for non-continous data is too voluminuous"
	      else:
		  print "" 
		  print "Data in the quicklook.h5 are empty, no plot generated"
		
	  ## Close the SAP file    
	  h5File.close()  
	          
   
 
 
 
 

 
    
      
      
#############################
#EXECUTION
#############################
   
if __name__ == '__main__':

  
  
    initparameterlist=range(7)
    
    initparameterlist[0]	= "none"	# Observation ID
    initparameterlist[1]	= "none"	# Directory where to find ICD3 Data
    initparameterlist[2]	= "none"	# Output for quicklook
    initparameterlist[3]	= "none"	# % of time data to use for quicklook
    initparameterlist[4]	= "none"	# % of spectral data to use for quicklook
    initparameterlist[5]	= "none"	# transpose matrix or not
  

    # Read and check parameters	
    initparameterlist = main(initparameterlist);

    ##Parameters
    obsID		= initparameterlist[0]
    obsDir		= initparameterlist[1]
    outputListDir	= initparameterlist[2]
    dataTimePercent	= float(initparameterlist[3])
    dataSpectralPercent	= float(initparameterlist[4])
    transpose		= initparameterlist[5]
    
    
    #Check if output directory exists!
 
    if os.path.isdir(outputListDir) != True:
	cmd = """mkdir %s"""%(outputListDir)
	print commands.getoutput(cmd)   
    
    ### WARNINGS !!
    if outputListDir[-1] != '/':
	outputListDir = outputListDir+'/'
	
    if obsDir[-1] != '/':
	obsDir = obsDir+'/'
	
    if dataTimePercent >1:
	print "dataTimePercent must be positive and lower or equal to 1! => new value : 1"
	dataTimePercent =1
	
    if dataTimePercent <0:
	print "dataTimePercent must be positive and  lower or equal to 1!"
	exit()	
	
    if dataSpectralPercent >1:
	print "dataSpectralPercent must be positive and lower or equal to 1! => new value: 1"
	dataSpectralPercent =1
	
    if dataSpectralPercent <0:
	print "dataSpectralPercent must be positive and  lower or equal to 1!"
	exit()	
	
    if transpose != "yes" and transpose != "no":
	print "transpose must be equal to yes or no"
	exit()
		
	
    ##################""
    ## Processing starts
	
    ##Internal parameter
    outputList		= outputListDir+"ICD3-list.txt"	
	
    start_step_global = time.clock()

    print "" 
    print "Standalone Quicklook Started"
    print "-----------------------------------"
    #find ICD3 files 
    start_step1 = time.clock()
    outputList = findData(obsDir,obsID,outputList)
    elapsedstep1 = (time.clock() - start_step1)
    print """ICD3 files list etablished and take: %s s"""%(elapsedstep1)
    print ""
    
    
    #generate the matrix for quicklook
    paramList	= range(7)
    paramList	= matrixGeneration(outputList,dataTimePercent,dataSpectralPercent,outputListDir,obsID,paramList,obsDir)
    
    #generate plots for quicklook
    start_step2 = time.clock()
    print "plot generation started"
    quickLookVisu(obsID,paramList,outputListDir,transpose)
    elapsedstep2 = (time.clock() - start_step2)
    print """Quicklook generation finished and take: %s s"""%(elapsedstep2)
    print "Finished"
    print "-----------------------------------"
	  

    elapsed_step_global = (time.clock() - start_step_global)  
    print ""
    print """Quicklook data generation is finished for All Part and takes %s s"""%(elapsed_step_global)
    print ""
    
    
    
    
    
    
    
    
    
    
    
    
    
    

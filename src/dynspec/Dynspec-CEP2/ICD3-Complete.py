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
	      opts, args = getopt.getopt(sys.argv[1:], "hiooRNRN:", ["help", "id=", "obsDIR=", "outputDIR=", "RAM=","Npart=", "RebinAll=", "NSAP="])
	      
      
	except getopt.GetoptError as err:
	      print "Usage: ./ICD3-Rebin.py --id=Lxxx  --obsDIR= --outputDIR=  --RAM=(x in Gb) --Npart= --RebinAll=(yes or no) [if Rebinall=yes default => 0: else --NSAP=]"
	      print ""
	      sys.exit(2)
	      
	
	for par1, par2 in opts:
		
		if par1 in ("--help"):
			print ""
        		print "Usage: ./ICD3-Rebin.py --id=Lxxx  --obsDIR= --outputDIR= --RAM=(x in Gb) --Npart= --RebinAll=(yes or no) [if Rebinall=yes default => 0: else --NSAP=]"
			print ""
        		sys.exit(2)
        	
		elif par1 in ("--id"):
			initparameterlist[0]=par2
		elif par1 in ("--obsDIR"):
			initparameterlist[1]=par2
		elif par1 in ("--outputDIR"):
			initparameterlist[2]=par2		
		elif par1 in ("--RAM"):
			initparameterlist[3]=par2
		elif par1 in ("--Npart"):
			initparameterlist[4]=par2
		elif par1 in ("--RebinAll"):
			initparameterlist[5]=par2			
		elif par1 in ("--NSAP"):
			initparameterlist[6]=par2										
		else:
        		print("Option {} inconnue".format(par1))
        		sys.exit(2)


        		
        # Check parameters		
 	if initparameterlist[0] == "none" or initparameterlist[1] == "none" or initparameterlist[2] == "none" or initparameterlist[3] == "none" or initparameterlist[4] == "none" or initparameterlist[5] == "none":
		print ""
		print "MISSING Parameters"	
        	print "Usage: ./ICD3-Rebin.py --id=Lxxx  --obsDIR= --outputDIR= --RAM=(x in Gb) --Npart= --RebinAll=(yes or no) [if Rebinall=yes default => 0: else --NSAP=]"
		print ""
        	sys.exit(2)       	
 

 
        # Only one or several SAP rebinning ? 		
	if initparameterlist[5] == "no":
	    try: 
		float(initparameterlist[6])
	    except:
		print "you must specify the number of SAP to  process"
        	sys.exit(2)
  	
	if initparameterlist[5] == "yes":
	      initparameterlist[6]=0	
        
        	#check parameters
	if initparameterlist[5] == "no" and initparameterlist[6] == 'none':
		print ""
		print "MISSING Parameters"	
        	print "You have to specify which SAP you want rebin giving the number to  --NSAP="
		print ""
        	sys.exit(2)
        	
	if initparameterlist[5] != "yes": 
		if initparameterlist[5] != "no":
		    print "for --RebinAll=  yous must specify yes or no"
		    sys.exit(2)	           	
        	        			
			
	return initparameterlist 


#############################
# ICD3 List Etablishment
#############################

def findData(obsDir,obsID,outputList):
    
      cmd = """find %s -name %s"""%(obsDir,obsID)+"_SAP*bf.h5  > "+"""%s"""%(outputList)
      print cmd
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
	  splinter		= obsID+"_SAP"
	  linesplit		= line.split(splinter)
	  pathFiletemp[i]	= linesplit[0]
	  File[i]		= obsID+"_SAP"+linesplit[1]
	  Filetemp[i]		= obsID+"_SAP"+linesplit[1]
	  i = i+1
      


      File.sort()
      for i in krange:
	  for j in krange:
	      if File[i] == Filetemp[j]:
		  pathFile[i] = pathFiletemp[j]		  
      
      listTxt	= open(outputList,'w')
      for i in krange:
	  cmd ="""%s%s"""%(pathFile[i],File[i])
	  listTxt.write(cmd)
      listTxt.close()
      
      return outputList

      
#############################
# Matrix for Quicklook
#############################

def matrixGeneration(outputList,outputListDir,obsID,RAM,RebinAll,NSAP,Npart): 

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
	  if i == 0:
		Path0 = linesplit[0]
	  i=i+1

      h5File 	= File(Path0, File.READ)     
      SAP	= Group(h5File, "SUB_ARRAY_POINTING_000")
      BEAM	= Group(SAP,"BEAM_000")
      
      selectCoordinates	= Group(BEAM,"COORDINATES")
      selectSpectral 	= Group(selectCoordinates,"COORDINATE_1")      

      
      nbSAPObj		= AttributeUInt(h5File, "OBSERVATION_NOF_SUB_ARRAY_POINTINGS")
      nbBEAMObj		= AttributeUInt(SAP,"OBSERVATION_NOF_BEAMS")
      nbSTOKESObj	= AttributeInt(BEAM,"OBSERVATION_NOF_STOKES")
      
      
      
      nbSAP	= nbSAPObj.value
      nbBEAM	= nbBEAMObj.value
      nbSTOKES 	= nbSTOKESObj.value
      
      

      #List-of-Files Output-dir ObsName tmax fmin fmax NofSAP nofBEAM nofStokes
      
      current_dir = os.getcwd()
      
      cmd = """%s/src/ICD3-ICD6-Complete/DynspecAll %s %s %s %s %s %s %s %s %s %s"""%(current_dir,outputList,outputListDir,obsID,RAM,RebinAll,NSAP,nbSAP,nbBEAM,nbSTOKES,Npart)
      print cmd
      print commands.getoutput(cmd)
      
      


 
    
      
      
#############################
#EXECUTION
#############################
   
if __name__ == '__main__':

  
  
    initparameterlist=range(7)
    
    initparameterlist[0]	= "none"	# Observation ID
    initparameterlist[1]	= "none"	# Directory where to find ICD3 Data
    initparameterlist[2]	= "none"	# Output for quicklook
    initparameterlist[3]	= "none"
    initparameterlist[4]	= "none" 
    initparameterlist[5]	= "none" 
    initparameterlist[6]	= "none" 


    # Read and check parameters	
    initparameterlist = main(initparameterlist);

    ##Parameters
    obsID		= initparameterlist[0]
    obsDir		= initparameterlist[1]
    outputListDir	= initparameterlist[2]
    RAM			= float(initparameterlist[3])
    Npart		= initparameterlist[4]
    RebinAll		= initparameterlist[5]
    NSAP		= initparameterlist[6]
    
    #Check if output directory exists!
 
    if os.path.isdir(outputListDir) != True:
	cmd = """mkdir %s"""%(outputListDir)
	print commands.getoutput(cmd)   
    
    ### WARNINGS !!
    if outputListDir[-1] != '/':
	outputListDir = outputListDir+'/'
	
    if obsDir[-1] != '/':
	obsDir = obsDir+'/'
		

    ##################""
    ## Processing starts
	
    ##Internal parameter
    outputList		= outputListDir+"ICD3-Complete-list.txt"	
	
	
    #find ICD3 files 
    start_step1 = time.clock()
    outputList = findData(obsDir,obsID,outputList)
    elapsedstep1 = (time.clock() - start_step1)
    print """ICD3 files list etablished and take: %s s"""%(elapsedstep1)
    print ""
    
    
    #generate rebin data
    matrixGeneration(outputList,outputListDir,obsID,RAM,RebinAll,NSAP,Npart)
    print ""
    

      

    
    
    
    
    
    
    
    
    
    
    
    
    
    

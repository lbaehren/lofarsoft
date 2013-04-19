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
	      opts, args = getopt.getopt(sys.argv[1:], "hiooRNRN:", ["help", "id=", "obsDIR=", "outputDIR=", "RAM="])
	      
      
	except getopt.GetoptError as err:
	      print "Usage: ./ICD3-Complete-Standalone.py --id=--id=Dxxxxxxxxxxxxxxxx.0Zxx...xxx  --obsDIR= --outputDIR=  --RAM=(x in Gb)"
	      print ""
	      sys.exit(2)
	      
	
	for par1, par2 in opts:
		
		if par1 in ("--help"):
			print ""
        		print "Usage: ./ICD3-Complete-Standalone.py --id=--id=Dxxxxxxxxxxxxxxxx.0Zxx...xxx --obsDIR= --outputDIR=  --RAM=(x in Gb)"
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
		else:
        		print("Unknown Option {}".format(par1))
        		sys.exit(2)


        		
        # Check parameters		
 	if initparameterlist[0] == "none" or initparameterlist[1] == "none" or initparameterlist[2] == "none" or initparameterlist[3] == "none":
		print ""
		print "MISSING Parameters"	
        	print "Usage: ./ICD3-Complete-Standalone.py --id=Dxxxxxxxxxxxxxxxx.0Zxx...xxx  --obsDIR= --outputDIR=  --RAM=(x in Gb)"
		print ""
        	sys.exit(2)       	
    	        			
			
	return initparameterlist 


#############################
# ICD3 List Etablishment
#############################

def findData(obsDir,obsID,outputList):
    
      cmd = """find %s -name *%s*"""%(obsDir,obsID)+".h5  > "+"""%s"""%(outputList)
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

def matrixGeneration(outputList,outputListDir,obsID,RAM): 

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
	  linesplit		= line.split()
	  h5File 		= File(linesplit[0], File.READWRITE)
	  telescope		= AttributeString(h5File, "TELESCOPE")
	  telescope.value	= "LOFAR" 
	  i=i+1

      
      nbSAP	= 1
      nbBEAM	= 1
      nbSTOKES 	= 4
      

      #List-of-Files Output-dir ObsName tmax fmin fmax NofSAP nofBEAM nofStokes
      
      current_dir = os.getcwd()
      
      cmd = """%s/src/ICD3-ICD6-Complete/DynspecAll_Standalone %s %s %s %s %s %s %s"""%(current_dir,outputList,outputListDir,obsID,RAM,nbSAP,nbBEAM,nbSTOKES)
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



    # Read and check parameters	
    initparameterlist = main(initparameterlist);

    ##Parameters
    obsID		= initparameterlist[0]
    obsDir		= initparameterlist[1]
    outputListDir	= initparameterlist[2]
    RAM			= float(initparameterlist[3])

    
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
    
    
    ##generate rebin data
    matrixGeneration(outputList,outputListDir,obsID,RAM)
    print ""
    

      

    
    
    
    
    
    
    
    
    
    
    
    
    
    

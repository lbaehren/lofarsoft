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
	      opts, args = getopt.getopt(sys.argv[1:], "hoifdifdkR:", ["help", "outDir=","id1=", "file1=", "dynspec1=","id2=", "file2=", "dynspec2=", "k=", "RAM="])
	      
      
	except getopt.GetoptError as err:	  
	      print ""	      
	      print "Usage: ./ICD6-Substraction.py --outDir= --id1=[what you want:Determine the output filename => id1-kxid2 or id1_dynspec1-kxdynspec2; depends if file1 & file2 are the same] --file1=[Name of the ICD6 file (*.h5)]  --dynspec1=[Name of the dynspec Group ex: DYNSPEC_000] --id2= --file2= --dynspec2= --k=[scaling factor: dynspec1-(k*dynspec2)] --RAM="
	      print ""
	      sys.exit(2)
	      
	
	for par1, par2 in opts:
		
		if par1 in ("--help"):
			print ""
			print "Usage: ./ICD6-Substraction.py --outDir= --id1=[what you want:Determine the output filename => id1-kxid2 or id1_dynspec1-kxdynspec2; depends if file1 & file2 are the same] --file1=[Name of the ICD6 file (*.h5)]  --dynspec1=[Name of the dynspec Group ex: DYNSPEC_000] --id2= --file2= --dynspec2= --k=[scaling factor: dynspec1-(k*dynspec2)] --RAM="
			print ""
        		sys.exit(2)
        		
		elif par1 in ("--outDir"):
			initparameterlist[0]=par2
		elif par1 in ("--id1"):
			initparameterlist[1]=par2			
		elif par1 in ("--file1"):
			initparameterlist[2]=par2
		elif par1 in ("--dynspec1"):
			initparameterlist[3]=par2
		elif par1 in ("--id2"):
			initparameterlist[4]=par2			
		elif par1 in ("--file2"):
			initparameterlist[5]=par2		
		elif par1 in ("--dynspec2"):
			initparameterlist[6]=par2
		elif par1 in ("--k"):
			initparameterlist[7]=par2
		elif par1 in ("--RAM"):
			initparameterlist[8]=par2				
		else:
        		print("Option {} inconnue".format(par1))
        		sys.exit(2)


        		
        # Check parameters		
 	if initparameterlist[0] == "none" or initparameterlist[1] == "none" or initparameterlist[2] == "none" or initparameterlist[3] == "none" or initparameterlist[4] == "none" or initparameterlist[5] == "none" or initparameterlist[6] == "none" or initparameterlist[7] == "none" or initparameterlist[8] == "none":
		print ""
		print "MISSING Parameters"	
		print "Usage: ./ICD6-Substraction.py --outDir= --id1=[what you want:Determine the output filename => id1-kxid2 or id1_dynspec1-kxdynspec2; depends if file1 & file2 are the same] --file1=[Name of the ICD6 file (*.h5)]  --dynspec1=[Name of the dynspec Group ex: DYNSPEC_000] --id2= --file2= --dynspec2= --k=[scaling factor: dynspec1-(k*dynspec2)] --RAM="
		print ""
        	sys.exit(2)       	
 

 
	return initparameterlist 



      
##############################
## Matrix for Quicklook
##############################

def matrixGeneration(outDir,id1,file1,dynspec1,id2,file2,dynspec2,k,RAM): 


      #################################################"
      ## Parameters for dynspec One
      h5FileOne 	= File(file1, File.READ)     
      dynspecOne	= Group(h5FileOne, dynspec1)
      coords		= Group(dynspecOne, "COORDINATES") 		
      spectral		= Group(coords,  "SPECTRAL")
    
      
      nbSTOKESDynspecOneObj	= AttributeVString(dynspecOne,"STOCKES_COMPONENT")
      NOF_SAMPLESObj		= AttributeInt(h5FileOne, "NOF_SAMPLES")
      FREQ_CHANNELObj		= AttributeVFloat(spectral, "AXIS_VALUE_WORLD")
      
      
      nbSTOKESOne 	= len(nbSTOKESDynspecOneObj.value)
      nbSamplesOne	= NOF_SAMPLESObj.value
      AxisValueOne	= FREQ_CHANNELObj.value
      nbFreqOne		= len(FREQ_CHANNELObj.value)
      

      
      
      #################################################"
      ## Parameters for dynspec Two
      h5FileTwo 	= File(file1, File.READ)     
      dynspecTwo	= Group(h5FileTwo, dynspec1)
      coords		= Group(dynspecTwo, "COORDINATES") 		
      spectral		= Group(coords,  "SPECTRAL")
    
      
      nbSTOKESDynspecTwoObj	= AttributeVString(dynspecTwo,"STOCKES_COMPONENT")
      NOF_SAMPLESObj		= AttributeInt(h5FileTwo, "NOF_SAMPLES")
      FREQ_CHANNELObj		= AttributeVFloat(spectral, "AXIS_VALUE_WORLD")
      
      
      nbSTOKESTwo 	= len(nbSTOKESDynspecTwoObj.value)
      nbSamplesTwo	= NOF_SAMPLESObj.value
      AxisValueTwo	= FREQ_CHANNELObj.value
      nbFreqTwo		= len(FREQ_CHANNELObj.value)
            
      
      
      # Bins check
      if nbSamplesOne != nbSamplesTwo:
	  print ""
	  print "Number of time bins from file1/dynspec1 and file2/dynspec2 are not the same!"
	  print nbSamplesOne+" time bins for file1 and  "+nbSamplesTwo+" time bins for file2"
	  print""
	  sys.exit(2)
	  
	  
      if nbFreqOne != nbFreqTwo:
	  print ""
	  print "Number of frequency bins from file1/dynspec1 and file2/dynspec2 are not the same!"
	  print nbFreqOne+" frequency bins for file1 and  "+nbFreqTwo+" frequency bins for file2"
	  print""
	  sys.exit(2)
	  
      if round(AxisValueOne[0],2) != round(AxisValueTwo[0],2) or  round(AxisValueOne[-1],2) != round(AxisValueTwo[-1],2):
	  print ""
	  print "file1/dynspec1 and file2/dynspec2 don't start or stop at the same frequency,"
	  print AxisValueOne[0]+" to " +AxisValueOne[-1] +" MHz for file1 and,"
	  print AxisValueTwo[0]+" to " +AxisValueTwo[-1] +" MHz for file1 and,"
	  print""
	  sys.exit(2)	
	  
      if nbSTOKESOne != nbSTOKESTwo:
	  print ""
	  print "file1/dynspec1 and file2/dynspec2 have not the same number of Stokes parameter,"
	  print nbSTOKESOne+" Stokes for file1/dynspec1 and  " +nbSTOKESTwo +" for file2/dynspec2"
	  print""
	  sys.exit(2)	
	  
      if file1 == file2 and dynspec1 == dynspec2:
	  print ""
	  print "WARNING !!!  file1/dynspec1 and file2/dynspec2 are the same dynamic spectrum !!"
	  print""
	  	
	       

      
	  
      nbSTOkES 	= nbSTOKESOne
      ID	= id1+"-"+k+"x"+id2


	
	
      
      current_dir = os.getcwd()
      
      cmd = """%s/src/ICD6-Substraction/Dynspec_Substraction %s %s %s %s %s %s %s %s %s"""%(current_dir,outDir,ID,file1,dynspec1,file2,dynspec2,k,RAM,nbSTOkES)
      print commands.getoutput(cmd)
      
      
#############################
#EXECUTION
#############################
   
if __name__ == '__main__':

  
  
    initparameterlist=range(9)
    
    initparameterlist[0]	= "none"	# Output directory  
    initparameterlist[1]	= "none"	# ID 1    
    initparameterlist[2]	= "none"	# file 1
    initparameterlist[3]	= "none"	# dynspec1
    initparameterlist[4]	= "none"	# ID 2    
    initparameterlist[5]	= "none"	# file 2
    initparameterlist[6]	= "none"	# dynspec 2
    initparameterlist[5]	= "none" 	# Scaling factor (dynspec1-k*dynspec2)
    initparameterlist[7]	= "none" 	# RAM quantity for avoiding swaping



    # Read and check parameters	
    initparameterlist = main(initparameterlist);

    ##Parameters
    outDir		= initparameterlist[0]
    id1			= initparameterlist[1]
    file1		= initparameterlist[2]
    dynspec1		= initparameterlist[3]
    id2			= initparameterlist[4]
    file2		= initparameterlist[5]
    dynspec2		= initparameterlist[6]
    k			= initparameterlist[7]
    RAM			= initparameterlist[8]
    
    #Check if output directory exists!
 
    if os.path.isdir(outDir) != True:
	cmd = """mkdir %s"""%(outDir)
	print commands.getoutput(cmd)   
    
    
    ##################  start of checks	    
    ### WARNINGS !!
    # Check end of path
    if outDir[-1] != '/':
	outDir = outDir+'/'
	
	
    # Check if file1 exists
    if os.path.isfile(file1) != True:
	print ""
	print "file1 don't exists! Check the name or the location of file1!"
	print ""
	sys.exit(2)
	
    # Check if file2 exists
    if os.path.isfile(file2) != True:
	print ""
	print "file2 don't exists! Check the name or the location of file2!"
	print ""
	sys.exit(2)
		
	
	
    # Check if dynspec1  exists
    h5FileOne 	= File(file1, File.READ)     
    dynspecOne	= Group(h5FileOne, dynspec1)
  
    try:
	
	dynspecOneTest	= AttributeFloat(dynspecOne,"POINT_RA")
	toto		= dynspecOneTest.value
    
    except:
	print "" 
	print "Unable to open dynspec1="+dynspec1+" Group,\nit doesn't exist in file: "+file1
	print ""
	sys.exit(2)
	
  
  
    # Check if dynspec2  exists    
    h5FileTwo 	= File(file2, File.READ)     
    dynspecTwo	= Group(h5FileTwo, dynspec2)	
    
    try:
	
	dynspecTwoTest	= AttributeFloat(dynspecTwo,"POINT_RA")
	toto		= dynspecTwoTest.value
    
    except:
	print "" 
	print "Unable to open dynspec2="+dynspec2+" Group,\nit doesn't exist in file: "+file2
	print ""
	sys.exit(2)
		
	
    ##################  end of checks	
		
		
    ###################""
    ### Processing starts
	
    #generate rebin data
    matrixGeneration(outDir,id1,file1,dynspec1,id2,file2,dynspec2,k,RAM)
    print ""
    

		     

    
    
    
    
    
    
    
    
    
    
    
    
    
    

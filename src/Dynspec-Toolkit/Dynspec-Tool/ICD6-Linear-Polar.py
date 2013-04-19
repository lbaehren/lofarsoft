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
	      opts, args = getopt.getopt(sys.argv[1:], "hoIfR:", ["help", "outDir=","ID=", "filename=","RAM="])
	      

	except getopt.GetoptError as err:	  
	      print ""	      
	      print "Usage: ./ICD6-Substraction.py --outDir= --ID=[what you want:Determine the output filename] --filename=[Name of the ICD6 file (*.h5)] --RAM="
	      print ""
	      sys.exit(2)
	      
	
	for par1, par2 in opts:
		
		if par1 in ("--help"):
			print ""
			print "Usage: ./ICD6-Substraction.py --outDir= --ID=[what you want:Determine the output filename] --filename=[Name of the ICD6 file (*.h5)] --RAM="
			print ""
        		sys.exit(2)
        		
		elif par1 in ("--outDir"):
			initparameterlist[0]=par2
		elif par1 in ("--ID"):
			initparameterlist[1]=par2			
		elif par1 in ("--filename"):
			initparameterlist[2]=par2
		elif par1 in ("--RAM"):
			initparameterlist[3]=par2				
		else:
        		print("Option {} inconnue".format(par1))
        		sys.exit(2)


        		
        # Check parameters		
 	if initparameterlist[0] == "none" or initparameterlist[1] == "none" or initparameterlist[2] == "none" or initparameterlist[3] == "none":
		print ""
		print "MISSING Parameters"	
		print "Usage: ./ICD6-Substraction.py --outDir= --ID=[what you want:Determine the output filename] --filename=[Name of the ICD6 file (*.h5)] --RAM="
		print ""
        	sys.exit(2)       	
 

 
	return initparameterlist 



      
##############################
## Matrix for Quicklook
##############################

def matrixGeneration(outDir,ID,filename,RAM): 

      
      #################################################"
      ## Parameters for dynspec One
      h5FileOne 	= File(filename, File.READ)     
      dynspecOne	= Group(h5FileOne, "DYNSPEC_000")
   
      
      nbBEAMObj	= AttributeInt(h5FileOne,"NOF_DYNSPEC")    
      nbBEAM 	= nbBEAMObj.value     
      
      nbSTOKESDynspecObj	= AttributeVString(dynspecOne,"STOCKES_COMPONENT")    
      nbSTOkES 			= len(nbSTOKESDynspecObj.value)
      
  
      if nbSTOkES != 4:
	  print ""
	  print "To Obtain the Linear Polarization and the Position Angle,\nwe need 4 Stokes parameters in the dynamic spectrum,\nand we have only "+str(nbSTOKES)
	  print""
	  sys.exit(2)	
	  
      
      current_dir = os.getcwd()
      
      cmd = """%s/src/ICD6-Linear-Polar/Dynspec_Linear_Polar %s %s %s %s %s %s"""%(current_dir,outDir,ID,filename,RAM,nbBEAM,nbSTOkES)
      print commands.getoutput(cmd)
      
      
#############################
#EXECUTION
#############################
   
if __name__ == '__main__':

  
  
    initparameterlist=range(4)
    
    initparameterlist[0]	= "none"	# Output directory  
    initparameterlist[1]	= "none"	# ID    
    initparameterlist[2]	= "none"	# file
    initparameterlist[3]	= "none" 	# RAM quantity for avoiding swaping



    # Read and check parameters	
    initparameterlist = main(initparameterlist);

    ##Parameters
    outDir		= initparameterlist[0]
    ID			= initparameterlist[1]
    filename		= initparameterlist[2]
    RAM			= initparameterlist[3]
    
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
    if os.path.isfile(filename) != True:
	print ""
	print "file1 don't exists! Check the name or the location of file1!"
	print ""
	sys.exit(2)
	

	
    # Check if dynspec1  exists
    h5FileOne	= File(filename, File.READ)     
    dynspecOne	= Group(h5FileOne, "DYNSPEC_000")
  
    try:
	
	dynspecTest	= AttributeFloat(dynspecOne,"POINT_RA")
	toto		= dynspecTest.value
    
    except:
	print "" 
	print "Unable to open dynspec= DYNSPEC_000 Group,\n"+filename+" must not exist"
	print ""
	sys.exit(2)
			
	
    ##################  end of checks	
		
		
    ###################""
    ### Processing starts
	
    #generate rebin data
    matrixGeneration(outDir,ID,filename,RAM)
    print ""
    

		     

    
    
    
    
    
    
    
    
    
    
    
    
    
    

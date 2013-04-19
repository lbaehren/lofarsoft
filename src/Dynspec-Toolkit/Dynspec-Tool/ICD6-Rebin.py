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
	      opts, args = getopt.getopt(sys.argv[1:], "hPootttffcR:", ["help", "Pathfile=", "obsname=", "outputFile=", "tmin=", "tmax=", "tscale=", "fmin=", "fmax=", "chanPerSubband=", "RAM="])
	      
      
	except getopt.GetoptError as err:
	      print "Usage: ./ICD6-Rebin.py --Pathfile=  --obsname=Lxxx[for metadata] --outputFile=[choose the complete output name]  --tmin= --tmax=  --tscale= --fmin= --fmax= --chanPerSubband= --RAM=(x in Gb)]"
	      print ""
	      sys.exit(2)
	      
	
	for par1, par2 in opts:
		
		if par1 in ("--help"):
			print ""
			print "Usage: ./ICD6-Rebin.py --Pathfile=  --obsname=Lxxx[for metadata] --outputFile=[choose the complete output name]  --tmin= --tmax=  --tscale= --fmin= --fmax= --chanPerSubband= --RAM=(x in Gb)]"
			print ""
        		sys.exit(2)
        	
		elif par1 in ("--Pathfile"):
			initparameterlist[0]=par2
		elif par1 in ("--obsname"):
			initparameterlist[1]=par2
		elif par1 in ("--outputFile"):
			initparameterlist[2]=par2
		elif par1 in ("--tmin"):
			initparameterlist[3]=par2
		elif par1 in ("--tmax"):
			initparameterlist[4]=par2
		elif par1 in ("--tscale"):
			initparameterlist[5]=par2
		elif par1 in ("--fmin"):
			initparameterlist[6]=par2
		elif par1 in ("--fmax"):
			initparameterlist[7]=par2
		elif par1 in ("--chanPerSubband"):
			initparameterlist[8]=par2			
		elif par1 in ("--RAM"):
			initparameterlist[9]=par2									
		else:
        		print("Option {} inconnue".format(par1))
        		sys.exit(2)


        		
        # Check parameters		
 	if initparameterlist[0] == "none" or initparameterlist[1] == "none" or initparameterlist[2] == "none" or initparameterlist[3] == "none" or initparameterlist[4] == "none" or initparameterlist[5] == "none" or initparameterlist[6] == "none" or initparameterlist[7] == "none" or initparameterlist[8] == "none" or initparameterlist[9] == "none":
		print ""
		print "MISSING Parameters"	
		print "Usage: ./ICD6-Rebin.py --Pathfile=  --obsname=Lxxx[for metadata] --outputFile=[choose the complete output name]  --tmin= --tmax=  --tscale= --fmin= --fmax= --chanPerSubband= --RAM=(x in Gb)]"
		print ""
        	sys.exit(2)       	
 
				
	return initparameterlist 



      
#############################
# Matrix for Quicklook
#############################

def matrixGeneration(Pathfile,obsname,outputFile,tmin,tmax,tscale,fmin,fmax,chanPerSubband,RAM): 

      #################################################"
      ## Parameters for dynspec One
      h5FileOne 	= File(Pathfile, File.READ)           
   
      durationObj	= AttributeFloat(h5FileOne, "TOTAL_INTEGRATION_TIME")
      freqMinObj	= AttributeFloat(h5FileOne, "OBSERVATION_FREQUENCY_MIN") 
      freqMaxObj	= AttributeFloat(h5FileOne, "OBSERVATION_FREQUENCY_MAX")  
      ChannelPerSubObj	= AttributeUInt(h5FileOne, "CHANNELS_PER_SUBANDS")    
      

      duration		= durationObj.value
      freqMin		= freqMinObj.value
      freqMax		= freqMaxObj.value      
      ChannelPerSub	= ChannelPerSubObj.value     
      
      #warning

      if fmin >freqMax:
	  print ""
	  print """The selected frequency minimum is higher than the maximum frequency of the observation: %s MHz"""%(freqMax)
	  print ""
	  print exit()

	  
      if fmax < freqMin:
	  print ""	
	  print """The selected frequency maximum is lower than the minimum frequency of the observation: %s Mhz"""%(freqMin)
	  print ""
	  print exit()
	  
	  
      if fmin > fmax:
	  print ""	
	  print "fmin is grater than fmax. Check these parameters"
	  print ""
	  sys.exit(2)   
	  
	  
      if tmin < 0:
	  print ""	
	  print "tmin must be equal to 0 or positive! You are crazy ^^ => New value: 0"
	  print ""
	  tmin = 0    
      
      
      if tmax > duration:
	  print ""	
	  print """tmax must be lower than total integration time => New value is: %s s"""%(round(duration,0)-1)
	  print ""
	  tmax = round(duration,2)-1
	  
	  
      if tmin > tmax:
	  print ""	
	  print "tmin is grater than tmax. Check these parameters"
	  print ""
	  sys.exit(2)   	  
	  
	
      if chanPerSubband!= 1:
	   if chanPerSubband&(chanPerSubband-1) !=0 or chanPerSubband<2:
	      print "Channel per subband must be multiple of 2^n or equal to 1"
	      print ""
	      exit()  
	
	
      if chanPerSubband > ChannelPerSub:
	   print """Channel per subband must be lower-equal: New value =>  %s"""%(ChannelPerSub)
	   print ""
	   chanPerSubband = ChannelPerSub   
	
	
      #List-of-Files Output-dir ObsName tmax fmin fmax NofSAP nofBEAM nofStokes
      
      current_dir = os.getcwd()
      
      cmd = """%s/src/ICD6-Rebin/Dyn2Dyn %s %s %s %s %s %s %s %s %s %s"""%(current_dir,Pathfile,obsname,outputFile,tmin,tmax,tscale,fmin,fmax,chanPerSubband,RAM)
      print commands.getoutput(cmd)
      
      


 
    
      
      
#############################
#EXECUTION
#############################
   
if __name__ == '__main__':
  
    initparameterlist=range(10)
    
    initparameterlist[0]	= "none"	# Pathfile
    initparameterlist[1]	= "none"	# obsname ; ID
    initparameterlist[2]	= "none"	# Output file
    initparameterlist[3]	= "none"	# timeMinSelect
    initparameterlist[4]	= "none" 	# timeMaxSelect
    initparameterlist[5]	= "none"	# timeRebin
    initparameterlist[6]	= "none"	# frequencyMin
    initparameterlist[7]	= "none"	# frequencyMax
    initparameterlist[8]	= "none"	# frequencyRebin  
    initparameterlist[9]	= "none"	# memoryRAM


    # Read and check parameters	
    initparameterlist = main(initparameterlist);

    ##Parameters
    Pathfile		= initparameterlist[0]
    obsname		= initparameterlist[1]
    outputFile		= initparameterlist[2]
    tmin		= float(initparameterlist[3])
    tmax		= float(initparameterlist[4])
    tscale		= float(initparameterlist[5])
    fmin		= float(initparameterlist[6]) 
    fmax		= float(initparameterlist[7]) 
    chanPerSubband	= int(initparameterlist[8]) 
    RAM			= float(initparameterlist[9])
   

    # Check if file1 exists
    if os.path.isfile(Pathfile) != True:
	print ""
	print "Pathfile don't exists! Check the name or the location"
	print ""
	sys.exit(2)   
   
    # Check if dynspec1  exists
    h5FileOne	= File(Pathfile, File.READ)     
    dynspecOne	= Group(h5FileOne, "DYNSPEC_000")
  
    try:
	
	dynspecTest	= AttributeFloat(dynspecOne,"POINT_RA")
	toto		= dynspecTest.value
    
    except:
	print "" 
	print "Unable to open dynspec= DYNSPEC_000 Group,\n"+Pathfile+" must not exist"
	print ""
	sys.exit(2)   
   
   
   
    ###################""
    ### Processing starts
		     
    #generate rebin data
    matrixGeneration(Pathfile,obsname,outputFile,tmin,tmax,tscale,fmin,fmax,chanPerSubband,RAM)
    print ""
    

      

    
    
    
    
    
    
    
    
    
    
    
    
    
    

#!/usr/bin/env python
#
# This is a script for importing ICD3 Beam formed data from 
# CEP 2 nodes and convert them in one ICD6 hdf5 file (convert ICD3->ICD6)
#
# July 2012 by N.Vilchez
#
###################################################################

import os, sys, glob, time
import commands
import getopt
import numpy as np
import time


###############################################################################

def main(parameter_list):
	try:
        	opts, args = getopt.getopt(sys.argv[1:], "hioR:", ["help","id=","output=","RAM="])
		
    	except getopt.GetoptError as err:
		print ""
        	print "Usage: ./run.py --id=Lxxxxx --output=/PATH [optionnal: --RAM=x(in Go, default 1Go))]"
		print ""
        	sys.exit(2)

	for par1, par2 in opts:
		if par1 in ("--help"):
			print ""
        		print "Usage: ./run.py --id=Lxxxxx --output=/PATH [optionnal: --RAM=x(in Go, default 1Go)]"
			print ""
        		sys.exit(2)
		elif par1 in ("--id"):
			parameter_list[0]=par2
		elif par1 in ("--output"):
			parameter_list[1]=par2
		elif par1 in ("--RAM"):
			parameter_list[2]=par2
		else:
        		print("Option {} inconnue".format(par1))
        		sys.exit(2)

	if parameter_list[0] == "none" or parameter_list[1] == "none":
		print ""
		print "MISSING Observation ID or Output directory"	
        	print "Usage: ./run.py --id=Lxxxxx --output=/PATH [optionnal: --RAM=x(in Go, default 1Go)]"
		print ""
        	sys.exit(2)
		
		
	try:
		RAM=float(parameter_list[2])
	except:
		print ""
		print "RAM entry is not a number, Maybe mistake with: --RAM=x"
		print ""
        	sys.exit(2)					
			
	return parameter_list


###############################################################################

def dataImport(parameter_list):

	status = os.path.isfile(parameter_list[1]+"""/"""+parameter_list[0]+"""_SAP000_B000_S0_P000_bf.h5""")
	if status == True:
		print ""
		print """Data extraction for %s observation has already been extracted"""%(parameter_list[0])
		print ""
	else:
		status = os.path.isdir(parameter_list[1]) 
		if status == True:
			cmd="""cexec locus: 'scp  /data/%s/*' %s"""%(parameter_list[0],parameter_list[1])
			print cmd
			print commands.getoutput(cmd)
		else:
			try:
				os.mkdir(parameter_list[1])
				cmd="""cexec locus: 'scp  /data/%s/*' %s"""%(parameter_list[0],parameter_list[1])
				print cmd
				print commands.getoutput(cmd)
			
			except:
				print ""
				print "Data output directory do not exist! and can not be created."
				print "Maybe you do not have the write access to this position"
				print ""
				sys.exit(2)
			
		status = os.path.isfile(parameter_list[1]+"""/"""+parameter_list[0]+"""_SAP000_B000_S0_P000_bf.h5""")
		if  status == True:
			print ""
			print """copy of %s data from CEP2 locus nodes are successed"""%(parameter_list[0])
			print ""
		else:
			print ""
			print "copy of %s data from CEP2 locus nodes are FAILED - script finished"
			print ""	
			sys.exit(2)

###############################################################################


def data_convert(parameter_list):

	print ""
	print " ICD3 -> ICD6 data conversion begin"
	print ""

	cmd="""(cd %s;LD_LIBRARY_PATH=/opt/cep/dal/current/lib /home/vilchez/ICD6-pipeline/dynspec_src/Dynspec %s"""%(parameter_list[1],parameter_list[1])+"/"+""" %s %s %s)"""%(parameter_list[0],parameter_list[1]+"/"+parameter_list[0]+".h5",parameter_list[2])
	print commands.getoutput(cmd)
	
	print ""
	print "Conversion finished -> Start cleaning output directoy"
	print ""
	
	cmd="""rm -rf %s"""%(parameter_list[1])+"""/*bf*"""
	print commands.getoutput(cmd)
	
	print ""
	print "Cleaning output directoy finished"
	
###############################################################################	print ""



###  M A I N ###
if __name__ == "__main__":

	parameter_list=range(3)
	
	parameter_list[0]="none"	# Observation ID
	parameter_list[1]="none"	# ICD3 Data output directory
	parameter_list[2]=1		# RAM used for ICD3 to ICD6 convertion
	
	# Read and check parameters	
	parameter_list = main(parameter_list);

	# Import data from locus
	dataImport(parameter_list)
	
	# Convert data to ICD6 format
	data_convert(parameter_list)
	
	
	
	












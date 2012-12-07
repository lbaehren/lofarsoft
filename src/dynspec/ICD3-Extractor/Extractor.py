#!/usr/bin/python
# -*- coding: utf-8 -*-


#############################
# IMPORT COMMANDS
#############################

import sys, os, glob, commands
import tkFileDialog

from ICD3OpenFile import *


from Tkinter import *
from dal import *



#############################
# CLASS MAIN WINDOW
#############################

class mainWindow():
  
    """Master window for the GUI"""

    def __init__(self, master):
      
	"""GUI Initialization
	:param master: main window
	:type master: Tkinter.Widget"""       
        
        self.parent 	= master
        self.hdf5Object	= ""

        
        ### Main Window variable
        ### Title 
	self.parent.title("Convert [and Extract] Beam Formed Data to Dynamic spectrum data")
	
	### Menu bar
        menubar = Menu(self.parent)
        self.parent.config(menu=menubar)        
        self.fileMenu = Menu(menubar)       
        quitMenu = Menu(menubar) 
        
        
	### File Menu	
        self.fileMenu.add_command(label="Load Beam Formed Data", underline=0, command=self.icd3OpenFile)
        submenu = Menu(self.fileMenu)
        
        ### Menu bar
        menubar.add_cascade(label="File", underline=0, menu=self.fileMenu)        
	menubar.add_command(label="Quit", underline=0, command=self.onExit)     

	
    def onExit(self):
        self.parent.quit()
    

        
    def icd3OpenFile(self):
      
	if self.hdf5Object != "":
	    self.hdf5Object.eraseObject()
	    
        h5FilePath = tkFileDialog.askdirectory(title='Choose Observation Directory')
	if h5FilePath != '':	    
	    self.hdf5Object = ICD3OpenFile(self.parent,h5FilePath)

	
	
#############################
#EXECUTION
#############################
   
if __name__ == '__main__':
    
    # Object creation => it will contains the main window
    mainObject = Tk()
    mainObject.geometry("600x800+0+0")
    
    # Main window is generated in the main object
    app = mainWindow(mainObject)
    mainObject.mainloop()  

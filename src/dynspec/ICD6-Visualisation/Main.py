#!/usr/bin/python
# -*- coding: utf-8 -*-


#############################
# IMPORT COMMANDS
#############################

import sys, os, glob, commands
import tkFileDialog

from ICD6OpenFile import *


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
        
        self.parent = master
        self.hdf5visu= ""
        
        
        ### Main Window variable
        ### Title 
	self.parent.title("Plot and Extract Dynamic spectrum data")
	
	### Menu bar
        menubar = Menu(self.parent)
        self.parent.config(menu=menubar)        
        self.fileMenu = Menu(menubar)       
        self.editMenu = Menu(menubar) 	
        quitMenu = Menu(menubar) 
        
        
	### File Menu	
        self.fileMenu.add_command(label="Load Dynamic Spectrum", underline=0, command=self.icd6OpenFile)
        submenu = Menu(self.fileMenu)
       
        
        ### Menu bar
        menubar.add_cascade(label="File", underline=0, menu=self.fileMenu)        
	menubar.add_command(label="Quit", underline=0, command=self.onExit)     

	
    def onExit(self):
        self.parent.quit()
        
        
    def icd6OpenFile(self):
      
	if self.hdf5visu != "":
	    self.hdf5visu.eraseObject()     
      
	ftypes = [('Dynamic Spectrum Files', '*.h5'), ('All files', '*')]
        dlg = tkFileDialog.Open(self.parent, filetypes = ftypes)
        h5FilePath = dlg.show()  
	if h5FilePath != '':	    
	    self.hdf5visu = ICD6OpenFile(self.parent,h5FilePath)

	    		
	
	
#############################
#EXECUTION
#############################
   
if __name__ == '__main__':
    
    # Object creation => it will contains the main window
    mainObject = Tk()
    mainObject.geometry("1024x810+0+0")
    
    # Main window is generated in the main object
    app = mainWindow(mainObject)
    mainObject.mainloop()  

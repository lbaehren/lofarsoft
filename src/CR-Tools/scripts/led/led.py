#! /usr/bin/env python

############################### BSD License ################################
# Copyright (c) 2010, Pim Schellart, Arthur Corstanje                      #
# All rights reserved.                                                     #
#                                                                          #
# Redistribution and use in source and binary forms, with or without       #
# modification, are permitted provided that the following conditions are   #
# met:                                                                     #
#                                                                          #
#     * Redistributions of source code must retain the above copyright     #
#       notice, this list of conditions and the following disclaimer.      #
#     * Redistributions in binary form must reproduce the above copyright  #
#       notice, this list of conditions and the following disclaimer in    #
#       the documentation and/or other materials provided with the         #
#       distribution.                                                      #
#     * Neither the name of the Radboud University Nijmegen nor the        #
#       names of its contributors may be used to endorse or promote        #
#       products derived from this software without specific prior written #
#       permission.                                                        #
#                                                                          #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS      #
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT        #
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A  #
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Pim Schellart BE    #
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR      #
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF     #
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS #
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN  #
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)  #
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF   #
# THE POSSIBILITY OF SUCH DAMAGE.                                          #
############################################################################

# General imports
import sys
import time
import numpy as np
import random
import datetime
from optparse import OptionParser

# Gui library imports
from PyQt4 import QtGui, QtCore
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg
from matplotlib.figure import Figure
  
# keys for accessing variables in splitted input lines
lineDescriptorKey = 0
timeKey = 1
thetaKey = 2
phiKey = 3
varianceKey = 4
distanceKey = 5

randomizationInDegrees = 1.0

class MplCanvas(FigureCanvasQTAgg):
    """Generic matplotlib figure canvas for embedding in a Qt 4 GUI."""

    def __init__(self, parent=None):
        """Create empty canvas and add plot."""

        # Add matplotlib figure to canvas
        self.fig = Figure()

        # Initialize base class
        FigureCanvasQTAgg.__init__(self, self.fig)

        # Bind to parent
        self.setParent(parent)

        # Set resize policy
        FigureCanvasQTAgg.setSizePolicy(self,
                                        QtGui.QSizePolicy.Expanding,
                                        QtGui.QSizePolicy.Expanding)

        # Set geometry
        FigureCanvasQTAgg.updateGeometry(self)

class ApplicationWindow(QtGui.QMainWindow):
    """Main application window."""
    
    def __init__(self, station, buffersize, refresh, fade, rmin, rmax, username, antennaMode, nofChannels, distancePlot, inputFile, liveMode):
        """Create a LOFAR Event Display window.
        
        It takes the following parameters: 
        *station* the station number passed on as argument to tails
        *buffersize* the size of the buffer (e.g. maximum number of 
        displayed points)
        *refresh* the refresh frequency in ms
        *fade* time for brightness to fade from 1 to 0 in ms
        (*rmin*,*rmax*) distance range for the plot.
        """

        # Store input parameters
        self.station=station
        self.buffersize=buffersize
        self.refresh=refresh
        self.fade=fade
        self.rlim=(rmin, rmax)
        self.username = username
        self.antennaMode = antennaMode
        self.nofChannels = nofChannels
        self.distancePlot = distancePlot
        self.inputFile = inputFile
        self.liveMode = liveMode

        # This is ugly! constructor should always work!
        # commandline parameter checking should be done in option parser.
        
        # OK, I agree. And of course this code doesn't even work. For now kick it out
        # and don't care about monkey users.
        
#        if inputFile and liveMode:
#            print 'Either run --live or --file=..., not both.'
#            exit(1)
            
        # Setup widget
        QtGui.QMainWindow.__init__(self)

        # Set main widget
        self.main = QtGui.QWidget(self)

        # Set window title
        self.setWindowTitle("LOFAR Event Display (station: {0})".format(self.station))

        # Add matplotlib canvas
        self.display=MplCanvas()

        # Add navigation toolbar
        self.toolbar=NavigationToolbar2QTAgg(self.display,self)

        # Add buttons
        self.startButton = QtGui.QPushButton('Start')
        self.stopButton = QtGui.QPushButton('Stop')
        self.stopButton.setEnabled(False)

        # Layout the widgets
        h = QtGui.QHBoxLayout()
        h.addWidget(self.startButton)
        h.addWidget(self.stopButton)

        v = QtGui.QVBoxLayout()
        v.addWidget(self.display)
        v.addWidget(self.toolbar)
        v.addLayout(h)

        # Set layout for main widget
        self.main.setLayout(v)

        # Set focus to main widget
        self.main.setFocus()

        # Set the main widget as the central widget
        self.setCentralWidget(self.main)

        # Destroy widget on close
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)

        # Fork process for listening to incoming data
        self.process = QtCore.QProcess()

        # Create timer for plot updates
        self.timer = QtCore.QTimer()

        # Connect signals and slots
        self.connect(self.timer, QtCore.SIGNAL("timeout()"),
            self.updatePlot)
        self.connect(self.timer, QtCore.SIGNAL("timeout()"),
            self.updateTime)
        self.connect(self.process, QtCore.SIGNAL("readyRead()"),
            self.readOutput)
        self.connect(self.process, QtCore.SIGNAL("processExited()"),
            self.resetButtons)
        self.connect(self.startButton, QtCore.SIGNAL("clicked()"),
            self.startCommand)
        self.connect(self.stopButton, QtCore.SIGNAL("clicked()"),
            self.stopCommand)

        # Set/reset data buffer
        self.resetBuffers()

        # Setup plot axes
        self.display.ax = self.display.fig.add_subplot(111, projection='polar')
        if self.distancePlot:
            self.display.ax.set_rscale('log')
        
        self.display.ax.set_ylim(self.rlim)

    def resetBuffers(self):
        """Set or reset data buffers"""

        # Index of buffer element to be read in next
        self.i = 0

        # Data buffers
        self.time = np.zeros(self.buffersize)
        self.phi = np.zeros(self.buffersize)
        self.theta = np.zeros(self.buffersize)

        # Color specifications for each data point in buffer
        # each point has a (r,g,b,alpha) color entry which is created
        # once and subsequently reused
        self.c = np.zeros((self.buffersize, 4))
        self.c[:,0:3] = (1, 0, 0)
        self.c[:,3] = np.zeros(self.buffersize)

    def updateTime(self):
        """Updates the reference time in ms by adding the refresh time"""

        self.ctime += self.refresh

    def startCommand(self):
        """Starts listening for incoming data"""

        # Set input widgets to correct state
        self.startButton.setEnabled(False)
        self.stopButton.setEnabled(True)
        self.toolbar.setEnabled(False)

        # Forks off a process listening for new data coming in, subsequently
        # each time new data is available a readyRead() signal is emitted
#        self.process.start('./tails',[str(self.station)])
        typeOfFit = 1 + (self.distancePlot == True)
        if self.liveMode:
            thisDate = datetime.datetime.utcnow()
            thisDateString = str(thisDate.year) + '-' + '%02d' % (thisDate.month) + '-' + '%02d' % (thisDate.day)
            self.process.start('./test/liveFeedFromLofarStation.sh', [self.station, thisDateString, self.username, self.antennaMode, str(self.nofChannels), str(typeOfFit)])
        else:
            self.process.start('./test/feedToVHECRtest.sh',[self.inputFile, 
                               self.antennaMode, str(self.nofChannels), str(typeOfFit), self.station])
    def stopCommand(self):
        """Stop listening for incoming data"""

        # Stop listening for incoming data
        self.process.terminate()

        # Stop updating plot
        self.timer.stop()

        # Empty data buffers (while keeping the plot)
        self.resetBuffers()

        # Set input widgets to correct state
        self.resetButtons()
        self.toolbar.setEnabled(True)

    def readOutput(self):
        """Called when new data is available from forked process, reads data
        into buffer.

        Also starts timer and sets reference time if this is the first event
        received since clicking "Start".
        """

        # Read data from stdout of forked process into Python list
        thisOutput = self.process.readAllStandardOutput()
        theseLines = str(thisOutput).splitlines()
        print thisOutput
        for line in theseLines:
            values = str(line).split()
#            print values
            # Check if list contains the expected number of values
            # this might not be the case if the input is not line buffered
            if values[lineDescriptorKey] == 'FitResult:':
                if len(values) < 5:
                    print 'Unexpected number of parameters: ', values
                elif float(values[varianceKey]) < 50.0:
                    # Check if this is the first event received and if so starts
                    # the timer controlling updates to the display, also sets the
                    # reference time
                    if not self.timer.isActive():
                        print 'Start the timer'
                        self.ctime = float(values[timeKey])*5e-6 # All times are in ms
                        self.timer.start(self.refresh)

                    # Store data (converting time to ms)
                    #self.time[self.i] = float(values[timeKey])*5e-6 BUGGY for non-matching time flow
                    self.time[self.i] = self.ctime
                    #print 'time is: ' + str(self.time[self.i])
                    self.phi[self.i] = float(values[phiKey]) + random.gauss(0.0, randomizationInDegrees)
                    #print self.phi[self.i]
                    if not self.distancePlot:
                        self.theta[self.i] = float(values[thetaKey]) + random.gauss(0.0, randomizationInDegrees)
                    else:
                        self.theta[self.i] = float(values[distanceKey])
                    #print self.theta[self.i]
                    # Reuse data buffer by cycling through it's indices
                    if self.i>=self.buffersize-1:
                        self.i=0
                    else:
                        self.i+=1
                else:
                    print 'Point discarded, variance too high: ' + str(float(values[varianceKey]))

    def resetButtons(self):
        """Reset buttons"""

        self.startButton.setEnabled(True)
        self.stopButton.setEnabled(False)

    def updatePlot(self):
        """Update plot with new data points.

        Data points fade away, disappearing completely when the difference
        between their time stamp and the reference time becomes larger than
        the requested *fade* time.
        """

        # Calculate the brightness of each point by comparing their
        # timestamps with the reference time
        self.c[:,3] = 1-(self.ctime-self.time.clip(self.ctime - self.fade))/self.fade
        #print self.c
#        self.c[:, 3] = self.c[:,3].clip(0) # equally expensive :(
#        self.c[self.c[:,3] < 0, 3] = 0 # if it's negative make it zero so we keep plotting it. <- Expensive operation!
        #print 'After'
        #print self.c
        self.c[:,0] = 0.8 + 0.2 * self.c[:, 3]
        #print self.c[0:100, 0]
        self.c[:,1] = 0.8 * (1 - self.c[:,3])
        self.c[:,2] = 0.8 * (1 - self.c[:,3])
        #print self.c[0:100, 2]
        #print self.c[0:100,3]
        # Find indices of all points that should be visible
        ind = np.argwhere((self.c[:,3]>=0)&(self.c[:,3]<=1)).ravel()

        # Ensure newer points get plotted last
        ind = ind[np.argsort(self.time[ind])]

        # Clear plot window
        self.display.ax.clear()

        # Add selected data points to plot (if any)
        if len(ind)>0:
            if not self.distancePlot:
                self.display.ax.scatter(self.phi[ind] * 2*np.pi / 360.0, 90.0 - self.theta[ind], c=self.c[ind], lw=0, s=80)
            else:
                self.display.ax.scatter(self.phi[ind] * 2*np.pi / 360.0, self.theta[ind], c=self.c[ind], lw=0, s=80)

            #print 'Plotting theta = ' + str(self.theta[ind]) + ' phi = ' + str(self.phi[ind])
        else:
            print 'No points to plot'
        if self.distancePlot:
            self.display.ax.set_rscale('log')
        self.display.ax.set_ylim(self.rlim)
  
#        self.display.ax.set_xlim(360.0)
        # Display time
#        for the time being hacked out 'UTC'
        #self.display.ax.set_title(time.ctime(self.ctime*1e-3)+" UTC")
        self.display.ax.set_title(time.ctime(self.ctime*1e-3) )

        # Draw plot on display
        self.display.draw()

# Parse command line options
parser=OptionParser(usage="usage: %prog [options]", version="%prog 1.0")
parser.add_option("-s", "--station",
                  type="string", dest="station", default='',
                  help="station name")
parser.add_option("-b", "--buffer-size",
                  type="int", dest="buffersize", default=1000,
                  help="buffer size for storing events")
parser.add_option("-r", "--refresh",
                  type="int", dest="refresh", default=1000,
                  help="refresh time in ms")
parser.add_option("-f", "--fade",
                  type="int", dest="fade", default=60000,
                  help="fade away time in ms")
parser.add_option("--rmin", default=0,
                  type="int", dest="rmin",
                  help="minimum radial distance")
parser.add_option("--rmax", default=90,
                  type="int", dest="rmax",
                  help="maximum radial distance")
parser.add_option("--user", default='corstanje',
                  type="string", dest="username",
                  help="user name for ssh-ing to the LOFAR portal")
parser.add_option("--antennas", default='LBA_OUTER',
                  type="string", dest="antennaMode", 
                  help="antenna mode with which the trigger file is (being) produced")
parser.add_option("--channels", default=48,
                  type="int", dest="nofChannels", 
                  help="number of coincident channels required")                

parser.add_option("--distance", action="store_true", dest="distancePlot", default=False, 
                  help="setting this option gives distance plot instead of direction plot")
parser.add_option("--file", default='/Users/acorstanje/triggering/electricfence/fenceoff/2010-04-13_TRIGGER_mode2_restarted.dat',
                  type="string", dest="inputFile", help="input trigger file to feed through. Mutually exclusive with --live")
parser.add_option("--live", action="store_true", dest="liveMode", default=False,
                  help="set flag to run in Live mode from given station")
                  
#parser.add_option("--distance", default=False,
#                   type="bool", dest="distancePlot",
#                   help="setting this option gives distance plot instead of direction plot")
                  
(options, args)=parser.parse_args()
if options.station == '':
    print 'Station name is required, use --station=...'
    exit(1)
    
# Create GUI
App = QtGui.QApplication(sys.argv)
aw = ApplicationWindow(options.station, options.buffersize, options.refresh, options.fade, options.rmin, options.rmax, options.username, options.antennaMode, options.nofChannels, options.distancePlot, options.inputFile, options.liveMode)
aw.show()

# Start application event loop
exit=App.exec_()


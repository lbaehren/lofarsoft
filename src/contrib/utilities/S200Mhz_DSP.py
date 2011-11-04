from HBA_I200Mhz import *
from S200Mhz_Interpreter import *
from DynamicSpectrumPlotter import *
from numpy import *
import BandpassFitters 
import GalacticForegroundFitters

class S200Mhz_DSP(DynamicSpectrumPlotter):
   def __init__(self, dataset):
      "Initialize with a S200Mhz_1min1Khz object"
      DynamicSpectrumPlotter.__init__(self)
      self.dataset = dataset
      self.subbandList = self.dataset.listSubbands()
      self.currentSubband = self.subbandList[0]
      self.currentPolarization = 0
      self.fitBandpass = False
      self.bandpassFilter = BandpassFitters.defaultFilter
      self.galacticForegroundFilter = GalacticForegroundFitters.defaultFilter
      self.logFilter      = lambda data: log(data) / log(10)
      self.filters = []
      
   def initSpecials(self, step):
      "Initializes extra buttons and checkboxes"
      # add buttons and checkboxes
      if step == 1:
         self.prevButtonAxis  = self.imageFigure.add_axes([0.87, 0.90, 0.04, 0.03])
         self.nextButtonAxis  = self.imageFigure.add_axes([0.92, 0.90, 0.04, 0.03])
         self.checkButtonAxis0= self.imageFigure.add_axes([0.87, 0.85, 0.09, 0.03])
         self.checkButtonAxis1= self.imageFigure.add_axes([0.87, 0.81, 0.09, 0.03])
         self.checkButtonAxis2= self.imageFigure.add_axes([0.87, 0.77, 0.09, 0.03])
         self.checkButtonAxis3= self.imageFigure.add_axes([0.87, 0.73, 0.09, 0.03])
      # add prev/next buttons
      elif step == 2:
         self.prevButton = Button(self.prevButtonAxis, '<')
         self.nextButton = Button(self.nextButtonAxis, '>')
         self.prevButton.on_clicked(self.__prev)
         self.nextButton.on_clicked(self.__next)
      # add checkboxes for bandpass / gal background correction
         self.checkButton0 = CheckButtons(self.checkButtonAxis0, ('fix axes',), (False,))
         self.checkButton1 = CheckButtons(self.checkButtonAxis1, ('log',), (False,))
         self.checkButton2 = CheckButtons(self.checkButtonAxis2, ('bandpass',), (False,))
         self.checkButton3 = CheckButtons(self.checkButtonAxis3, ('galac',), (False,))
         self.checkButton0.on_clicked(self.__handleCheckButtons)
         self.checkButton1.on_clicked(self.__handleCheckButtons)
         self.checkButton2.on_clicked(self.__handleCheckButtons)
         self.checkButton3.on_clicked(self.__handleCheckButtons)
      

   def __prev(self, event):
      "Handler for the prev button event"
      i = self.subbandList.index(self.currentSubband)
      if i > 0: self.plot(subband=self.subbandList[i-1])
      
      
   def __next(self, event):
      "Handler for the next button event"
      i = self.subbandList.index(self.currentSubband)
      if i+1 < len(self.subbandList): self.plot(subband=self.subbandList[i+1])
         
   def __toggleFilter(self, fil):
      "if the filter is present in self.filters, it is removed; otherwise it is added"
      if fil in self.filters:
         self.delFilter(fil)
      else:
         self.addFilter(fil)
      
   def __handleCheckButtons(self, label):
      "handler for all of the checkbuttons"
      if label == 'bandpass':
         self.__toggleFilter(self.bandpassFilter)
      if label == 'log':
         self.__toggleFilter(self.logFilter)
      if label == 'galac':
         self.__toggleFilter(self.galacticForegroundFilter)
         
      self.plot()
#       elif label == 'galac':
#          self.galbackCorr = not self.galbackCorr
#          self.replot()
#       elif label == 'fix axes':
#          self.fixAxes = not self.fixAxes
#          if (not self.fixAxes):
#             self.replot()

   def addFilter(self, fil):
      self.filters.append(fil)
   
   def delFilter(self, fil):
      self.filters.remove(fil)

   def plot(self, **args):
      """Plot a subband, takes the following keywords:
      subband     : a number
      polarization: 0 or 3
      fitbandpass : True or False"""
      for k in args.keys():
         if   k == 'subband'     : self.currentSubband = args['subband']
         elif k == 'polarization': self.currentPolarization = args['polarization']
         elif k == 'fitbandpass' : self.fitBandpass = args['fitbandpass']
      
      
      sdata = self.dataset.getAbsSet(self.currentSubband).transpose()
      
      # clip the data, the first channel has no signal :(
      sdata = sdata[1:]
      
      # apply filters
      for filt in self.filters:
         sdata = filt(sdata)
      
      # determine depth axes
      vmin = sdata.min()
      vmax = sdata.mean() + (sdata.mean() - sdata.min())
      
      DynamicSpectrumPlotter.plot(self, sdata, vmin=vmin, vmax=vmax)
      self.__writeTitle()

         
   def plotFrequency(self, freq):
      "Plots the subband containing specified frequency"
      (subb, chnl) = getSubChnlByFreq(freq)
      self.plot(subband=subb)
      
      
   def __writeTitle(self):
      "Writes window titles based on subband"
      outstr = ''
      frstr = "(%.3f - %.3f Mhz) " % (getFreq(self.currentSubband)/10**6.0, getFreq(self.currentSubband, 255)/10**6.0)
      outstr += "Subband %d-%d %s" % (self.currentSubband, self.currentPolarization, frstr)

#       if self.polarDiff != 0:
#          outstr += "Subband %d - polarization difference %s" % (self.currSubband, frstr)
#       else:
#          outstr += "Subband %d-%d %s" % (self.currSubband, self.currPolar, frstr)
#          if (self.bandpassCorr):
#             outstr += ' - bp'
#          if (self.galbackCorr):
#             outstr += ' - gal'
      self.imageWindow.wm_title(outstr)
      (ms, ant, sub) = getMSBySubband(self.currentSubband)
      self.imagePlot.set_title('%s A%d SB%d' % (ms, ant, sub))   
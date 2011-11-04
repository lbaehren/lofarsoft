from numpy import *
from matplotlib.widgets import Widget, CheckButtons
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
import Tkinter as Tk


# This is a MatPlotLib widget which draws a horizontal and vertical line on the coordinates of the mouseclick
class StationaryCursor:
    def __init__(self, ax, useblit=False, **lineprops):
        self.ax = ax
        self.canvas = ax.figure.canvas

        self.canvas.mpl_connect('button_press_event', self.onmove)
        self.canvas.mpl_connect('draw_event', self.clear)

        self.visible = True
        self.horizOn = True
        self.vertOn = True
        self.useblit = useblit

        self.lineh = ax.axhline(0, visible=False, **lineprops)
        self.linev = ax.axvline(0, visible=False, **lineprops)

        self.background = None
        self.needclear = False
        

    def clear(self, event):
        'clear the cursor'
        if self.useblit:
            self.background = self.canvas.copy_from_bbox(self.ax.bbox)
        self.linev.set_visible(False)
        self.lineh.set_visible(False)

    def onmove(self, event):
        'on mouse click draw the cursor if visible'
        if event.inaxes != self.ax:
            self.linev.set_visible(False)
            self.lineh.set_visible(False)

            if self.needclear:
                self.canvas.draw()
                self.needclear = False
            return
        self.needclear = True
        if not self.visible: return
        self.linev.set_xdata((event.xdata, event.xdata))

        self.lineh.set_ydata((event.ydata, event.ydata))
        self.linev.set_visible(self.visible and self.vertOn)
        self.lineh.set_visible(self.visible and self.horizOn)

        self._update()


    def _update(self):

        if self.useblit:
            if self.background is not None:
                self.canvas.restore_region(self.background)
            self.ax.draw_artist(self.linev)
            self.ax.draw_artist(self.lineh)
            self.canvas.blit(self.ax.bbox)
        else:

            self.canvas.draw_idle()

        return False
   


# Another MatPlotLib widget which makes a button. modified to remove the onmove event
class Button(Widget):
    def __init__(self, ax, label, image=None,
                 color='0.85', hovercolor='0.95'):
        if image is not None:
            ax.imshow(image)
        self.label = ax.text(0.5, 0.5, label,
                             verticalalignment='center',
                             horizontalalignment='center',
                             transform=ax.transAxes)

        self.cnt = 0
        self.observers = {}
        self.ax = ax
        ax.figure.canvas.mpl_connect('button_press_event', self._click)
        ax.set_navigate(False)
        ax.set_axis_bgcolor(color)
        ax.set_xticks([])
        ax.set_yticks([])
        self.color = color
        self.hovercolor = hovercolor

        self._lastcolor = color

    def _click(self, event):
        if event.inaxes != self.ax: return
        if not self.eventson: return
        for cid, func in self.observers.items():
            func(event)

    def _motion(self, event):
        if event.inaxes==self.ax:
            c = self.hovercolor
        else:
            c = self.color
        if c != self._lastcolor:
            self.ax.set_axis_bgcolor(c)
            self._lastcolor = c
            if self.drawon: self.ax.figure.canvas.draw()

    def on_clicked(self, func):
        cid = self.cnt
        self.observers[cid] = func
        self.cnt += 1
        return cid

    def disconnect(self, cid):
        'remove the observer with connection id cid'
        try: del self.observers[cid]
        except KeyError: pass



       
class DynamicSpectrumPlotter:
   """
      This class creates an interactive plot interface to display dynamic spectra.
   """
   def __init__(self):
      self.data = None
      self.initialized = False
      
   def initGUI(self):
      self.initialized = True
      self.initImageWindow()
      self.initGraphWindow()
      
   def initImageWindow(self):
      self.imageWindow = Tk.Tk()
      self.imageWindow.wm_title("Dynamic spectrum")
      # resize windows to split left-right
      (maxx, maxy) = self.imageWindow.maxsize()
      self.imageWindow.wm_geometry('%dx%d+0+0' % (maxx-1, int(0.6*maxy)))                 # 60% real estate
      # create the plot windows
      self.imageFigure = Figure()
      try:
         self.initSpecials(1)
      except:
         pass
      self.imagePlot       = self.imageFigure.add_axes([0.05, 0.07, 0.90, 0.88])
      # draw the plot windows
      self.imageCanvas = FigureCanvasTkAgg(self.imageFigure, master=self.imageWindow)
      try:
         self.initSpecials(2)
      except:
         pass
      self.imageCanvas.mpl_connect('button_press_event', self.updateIntersections)
      self.imageCursor = StationaryCursor(self.imagePlot, useblit=True, color='black', linewidth=1 )
      self.imageCanvas.show()
      self.imageCanvas.get_tk_widget().pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)      
      self.imageToolbar = NavigationToolbar2TkAgg( self.imageCanvas, self.imageWindow )
      self.imageToolbar.update()
      self.imageCanvas._tkcanvas.pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)
      self.image = None
      self.colorbar = None
   
   def initGraphWindow(self):
      self.graphWindow = Tk.Tk()
      self.graphWindow.wm_title("Intersections")
      # resize windows to split left-right
      (maxx, maxy) = self.imageWindow.maxsize()
      self.graphWindow.wm_geometry('%dx%d+0+%d' % (maxx-1, int(0.3*maxy), int(0.7*maxy))) # 40% real estate
      # create the plot windows
      self.graphFigure = Figure()
      self.timePlot    = self.graphFigure.add_axes([ 0.05, 0.075, 0.425, 0.90])
      self.freqPlot    = self.graphFigure.add_axes([0.525, 0.075, 0.425, 0.90])
      self.timeLine = None
      self.freqLine = None
      # draw the plot windows
      self.graphCanvas = FigureCanvasTkAgg(self.graphFigure, master=self.graphWindow)
      self.graphCanvas.show()
      self.graphCanvas.get_tk_widget().pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)      
      self.graphToolbar = NavigationToolbar2TkAgg( self.graphCanvas, self.graphWindow )
      self.graphToolbar.update()
      self.graphCanvas._tkcanvas.pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)
      
   def initSpecials(self, step):
      """
      Subclasses should override this to add checkbutton or regular buttons. It calls at two steps: step 1 being the creation of the axes. Step 2 for the instantiation of the button class and the linking to events
      """
      pass
      
   def updateIntersections(self, event):
      if event.inaxes == self.imagePlot:
         self.checkForReInit()
         xpos, ypos = round(event.xdata), round(event.ydata)
         if self.timeLine == None:
            self.timeLine,   = self.timePlot.plot(self.data[:,0])
            self.freqLine,   = self.freqPlot.plot(self.data[0,:])
         else:
            self.timeLine.set_ydata(self.data[:,xpos])
            self.freqLine.set_ydata(self.data[ypos,:])
            
         self.graphFigure.draw_artist(self.timeLine)
         self.graphFigure.draw_artist(self.freqLine)
         self.graphCanvas.draw()

   def replot(self):
      self.plot()
            
   def checkForReInit(self):
      try:
         self.imageWindow.winfo_name()
      except:
         self.initImageWindow()
      try:
         self.graphWindow.winfo_name()
      except:
         self.initGraphWindow()
      
         
   def plot(self, aData=None, vmin=None, vmax=None, interpolation='nearest'):
      if aData != None: self.data = aData
      self.data = squeeze(self.data)
      if (self.data.dtype.name[:7] == 'complex'): self.data = abs(self.data)
        
      fdata = self.data.flatten()
      fdata = fdata.compress(logical_not(isnan(fdata)))
      if vmin == None: vmin = min(fdata) # numpy's .max() tends to bug out when
      if vmax == None: vmax = max(fdata) #   NaN values are present
      
      self.checkForReInit()
      
      self.imagePlot.cla()
      self.image   = self.imagePlot.imshow(self.data, origin='lower', aspect='auto', interpolation=interpolation, vmin=vmin, vmax=vmax)
      if (self.colorbar == None):
         self.colorbar = self.imageFigure.colorbar(self.image)
      else:
         self.colorbar = self.imageFigure.colorbar(self.image, self.colorbar.ax)
         

#         self.image.set_clim(axes[0], axes[1])
      self.timePlot.cla()
      self.freqPlot.cla()
       
      self.timeLine,    = self.timePlot.plot(self.data[:,0])
      self.freqLine,    = self.freqPlot.plot(self.data[0,:])
      
      self.timePlot.set_ylim(vmin, vmax)
      self.freqPlot.set_ylim(vmin, vmax)
      self.timePlot.set_xlim(0, len(self.data))
      self.freqPlot.set_xlim(0, len(self.data[0]))
         
      self.draw()
      
   
   def draw(self):
         self.imageCanvas.draw()
         self.graphCanvas.draw()
   

         
class BufferedDSPlotter(DynamicSpectrumPlotter):
   """The buffered plotter adds a buffer for previously plotted data, which one can scroll through using the arrow buttons"""
   
   def __init__(self, buffersize=200):
      "Buffersize is in MB"
      DynamicSpectrumPlotter.__init__(self)
      self.buffersize = buffersize * 2**20
      self.bufferlist = []
      self.bindex = 0
      self.prevmin, self.prevmax = None, None

   def initSpecials(self, step):
      "Initializes extra buttons and checkboxes"
      if step == 1:
         self.prevButtonAxis  = self.imageFigure.add_axes([0.87, 0.90, 0.04, 0.03])
         self.nextButtonAxis  = self.imageFigure.add_axes([0.92, 0.90, 0.04, 0.03])
         self.bufferLabel     = self.imageFigure.text(0.88, 0.82, '')
      # add prev/next buttons
      elif step == 2:
         self.prevButton = Button(self.prevButtonAxis, '<')
         self.nextButton = Button(self.nextButtonAxis, '>')
         self.prevButton.on_clicked(self.__prev)
         self.nextButton.on_clicked(self.__next)
         
   def fixAxes(self):
      if self.prevmin:
         self.prevmin, self.prevmax = None, None
      else:
         self.prevmin, self.prevmax = self.image.get_clim()
                 
   def __prev(self, event):
      if self.bindex > 0: self.bindex -= 1
      self.__updateBufferLabel()
      DynamicSpectrumPlotter.plot(self, self.bufferlist[self.bindex], vmin=self.prevmin, vmax=self.prevmax)
   
   def __next(self, event):
      if self.bindex+1 < len(self.bufferlist): self.bindex += 1
      self.__updateBufferLabel()
      DynamicSpectrumPlotter.plot(self, self.bufferlist[self.bindex], vmin=self.prevmin, vmax=self.prevmax)
      
   def __updateBufferLabel(self):
      isize = (self.bufferlist[self.bindex].size * self.bufferlist[self.bindex].itemsize) / float(2**20)
      bsize = self.getBufferSize() / float(2**20)
      self.bufferLabel.set_text('%d / %d \n(%.2f / %.2f MB)' % (self.bindex+1, len(self.bufferlist), isize, bsize))
      
   def getBufferSize(self):
      return sum([d.size*d.itemsize for d in self.bufferlist])
   
   def clearBuffer(self):
      del self.bufferlist[:]
      self.bindex = 0
   
   def plotBuffered(self, index):
      self.bindex = index-1
      self.__updateBufferLabel()
      DynamicSpectrumPlotter.plot(self, self.bufferlist[self.bindex])
            
   def plot(self, aData=None, *args, **kwargs):
      if aData != None:
         self.data = aData
         dsize = self.data.size * self.data.itemsize
         if dsize < self.buffersize:
            while self.getBufferSize()+dsize > self.buffersize: del self.bufferlist[0]
            self.bufferlist.append(self.data)
            self.bindex = len(self.bufferlist)-1
      
      kwargs['vmin'] = self.prevmin
      kwargs['vmax'] = self.prevmax
      
      self.checkForReInit()   
      self.__updateBufferLabel()
      DynamicSpectrumPlotter.plot(self, *args, **kwargs)
         
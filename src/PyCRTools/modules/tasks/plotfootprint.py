"""
A tool to plot the footprint of a cosmic ray event with the superterp as background.

"""

import pycrtools as cr
import pycrtools.tasks as tasks
import pytmf
from pycrtools.tasks.shortcuts import *
from math import *
import pycrtools.rftools as rf

deg=pi/180.
pi2=pi/2.


class plotfootprint(tasks.Task):
    """
    **Description:**

    Plot the layout of the current dataset on the ground.

    **Usage:**

    **See also:**
    :class:`DirectionFitTriangles`

    **Example:**

    ::
        file=open("$LOFARSOFT/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
        file["ANTENNA_SET"]="LBA_OUTER"
        file["SELECTED_DIPOLES"]="odd"
        positions=file["ANTENNA_POSITIONS"]
        layout=trun("plotfootprint",positions=positions,sizes=range(48),names=range(48))
   """
    parameters=dict(
        positions={doc:"hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions (x0,y0,z0,...)",unit:"m"},
        size={default:300,doc:"Size of largest point."},
        sizes_min={default:None,doc:"If set, then use this as the minimum scale for the sizes, when normalizing and plotting."},
        sizes_max={default:None,doc:"If set, then use this as the maximum scale for the sizes, when normalizing."},
        normalize_sizes={default:True,doc:"Normalize the sizes to run from 0-1."},
        normalize_colors={default:False,doc:"Normalize the colors to run from 0-1."},
        power={default:20,doc:"hArray of dimension [NAnt] with the values for the power of each lofar antenna. This gives the sizes of the plot"},
        arrivaltime={default:'b',doc:"hArray of dimension [NAnt] with the values of relative peak arrival time. This gives the colors of the plot"},
        names={default:False,doc:"hArray of dimension [NAnt] with the names or IDs of the antennas"},
        title={default:False,doc:"Title for the plot (e.g., event or filename)"},
        newfigure=p_(True,"Create a new figure for plotting for each new instance of the task."),
        plot_finish={default: lambda self:cr.plotfinish(doplot=False,plotpause=False),doc:"Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"},
        plot_name={default:"",doc:"Extra name to be added to plot filename."},
        plotlegend={default:False,doc:"Plot a legend"},
        positionsT=p_(lambda self:cr.hArray_transpose(self.positions),"hArray with transposed Cartesian coordinates of the antenna positions (x0,x1,...,y0,y1...,z0,z1,....)",unit="m",workarray=True),
        NAnt=p_(lambda self: self.positions.shape()[-2],"Number of antennas.",output=True),
        figure={default:None,doc:"No startplot"},
        
        plotlora={default:True,doc:"Plot the LORA data when positions are present?"},
        loracore={default:None,doc:"Shower core position in hArray(float(X,Y,0))",unit:"m"},
        loradirection={default:None,doc:"Shower direction hArray(float,(Azimuth,Elevation)). Azimuth defined from North Eastwards. Elevation defined from horizon up",unit:"degrees"},
        lorapositions={default:None,doc:"3-dim hArray with transposed Cartesian coordinates of the antenna positions (x0,x1,...,y0,y1...,z0,z1,....)",unit:"m"},
        lorapower={default:20,doc:"hArray with power of the LORA events"},
        loraarrivaltimes={default:None,doc:"hArray with arrival time of LORA events"},
        lorashape={default:"p",doc:"Shape of LORA detectors. e.g. 's' for square, 'p' for pentagram, 'h' for hexagon"},
        lofarshape={default:"o",doc:"Shape of LOFAR antennas. e.g. 'o' for circle, '^' for triangle"},
        loracolor={default:"#BE311A",doc:"Color used for LORA plots. If set to 'time' uses the arrival time" },
        plotlayout={default:True,doc:"Plot the LOFAR layout of the stations as the background"}
        
        )

    def call(self):
        pass

    def run(self):

        #Calculate scaled sizes
        #import pdb; pdb.set_trace()

        if isinstance(self.power,(list)):
            self.sizes=cr.hArray(self.power)
        if isinstance(self.power, (int, long, float)):
            self.ssizes=self.power
        elif isinstance(self.power,tuple(cr.hRealContainerTypes)):
            self.ssizes=cr.hArray(copy=self.power)
            if "Int" in repr(type(self.ssizes)):
                self.ssizes=cr.hArray(float,dimensions=self.ssizes,fill=self.ssizes)
            if self.normalize_sizes:
                self.ssizes /= self.ssizes.max().val() if self.sizes_max==None else self.sizes_max
                self.ssizes *= self.size
                if self.sizes_min:
                    minindex=cr.hArray(int,dimensions=self.ssizes)
                    nrless=cr.hFindLessThan(minindex,self.ssizes,self.sizes_min).val()
                    cr.hCopy(self.ssizes,cr.hArray(float,dimensions=[nrless],fill=self.sizes_min),minindex,nrless)
        else:
            raise TypeError("PlotAntennaLayout: parameter 'sizes' needs to be a number or an hArray of numbers.")

        if isinstance(self.arrivaltime, (str, int, long, float)):
            self.scolors=self.arrivaltime
        elif isinstance(self.arrivaltime,tuple(cr.hAllContainerTypes)):
            self.scolors=cr.hArray(copy=self.arrivaltime)
            if self.normalize_colors:
                self.scolors -= self.scolors.min().val()
                self.scolors /= self.scolors.max().val()
        else:
            raise TypeError("PlotAntennaLayout: parameter 'colors' needs to be string or an hArray thereof.")

        if self.loracolor is "time": 
            if self.loraarrivaltimes:
                self.loracolor=self.loraarrivaltimes
            else:
                self.loracolor="#BE311A"

        if self.newfigure and not self.figure:
            self.figure=cr.plt.figure()
        cr.plt.clf()
        if self.plotlayout:
            from os import environ
            from os.path import isfile
            if "LOFARSOFT" in environ.keys():
                bgimname=environ["LOFARSOFT"]+"/src/PyCRTools/extras/LORA_layout_background.png"
                if isfile(bgimname):
                    bgim=cr.plt.imread(bgimname)
                    cr.plt.imshow(bgim,origin='upper',extent=[-375/2,375/2,-375/2-6*120/227,375/2-6*120/227],alpha=0.2)
                else:
                    print "WARNING Cannot plot layout. Image file not found. Run an svn update?"
            else:
                print "WARNING Cannot plot layout. Environment variable LOFARSOFT not found."
        if self.title:
            cr.plt.title(self.title)
        cr.plt.scatter(self.positionsT[0].vec(),self.positionsT[1].vec(),s=self.ssizes,c=self.colors,marker=self.lofarshape)
        cbar=cr.plt.colorbar()
        if self.plotlora:
            if isinstance(self.lorapower,(list)):
                self.lorapower=cr.hArray(self.lorapower)
            if isinstance(self.lorapower, (int, long, float)):
                self.lsizes=self.lorapower
                if self.lsizes < self.sizes_min :
                    self.lsizes= self.sizes_min
            elif isinstance(self.lorapower,(list)):
                self.lorapower=cr.hArray(self.lorapower)
            elif isinstance(self.lorapower,tuple(cr.hRealContainerTypes)):
                self.lsizes=cr.hArray(copy=self.lorapower)
                if "Int" in repr(type(self.lsizes)):
                    self.lsizes=cr.hArray(float,dimensions=self.lsizes,fill=self.lsizes)
                if self.normalize_sizes:
                    self.lsizes /= self.lsizes.max().val() if self.sizes_max==None else self.sizes_max
                    self.lsizes *= self.size
                    if self.sizes_min:
                        minindex=cr.hArray(int,dimensions=self.lsizes)
                        nrless=cr.hFindLessThan(minindex,self.lsizes,self.sizes_min).val()
                        cr.hCopy(self.lsizes,cr.hArray(float,dimensions=[nrless],fill=self.sizes_min),minindex,nrless)
            else:
                raise TypeError("PlotAntennaLayout: parameter 'sizes' needs to be a number or an hArray of numbers.")
            if self.lorapositions:
                cr.plt.scatter(self.lorapositions[0].vec(),self.lorapositions[1].vec(),s=self.lsizes,c=self.loracolor,marker=self.lorashape,)
            if self.loracore:
                cr.plt.scatter(self.loracore[0],self.loracore[1],marker='x',s=200,color='red',linewidth=3)
                if self.loradirection:
                    dcos=cr.cos(cr.radians(self.loradirection[0]))
                    dsin=cr.cos(cr.radians(self.loradirection[0]))
                    elev=self.loradirection[1]
                    cr.plt.arrow(self.loracore[0]+elev*dsin,self.loracore[1]+elev*dcos,-elev*dsin,-elev*dcos,lw=3,color='red',ls='dashed',hatch='\\')
        if self.names:
            for label,x,y in zip(self.names,self.positionsT[0].vec(),self.positionsT[1].vec()):
                cr.plt.annotate(str(label),xy=(x,y), xytext=(-3,3),textcoords='offset points', ha='right', va='bottom')
        if self.plotlegend:
            cr.plt.colorbar()
        cr.plt.xlabel("meters East")
        cr.plot.ylabel("meters North")
        plt.text(100,-220,"Size denotes signal power")
        self.plot_finish(name=self.__taskname__+self.plot_name)

"""Provides an n-dimensional array class.
"""
#import pdb
#pdb.set_trace()

import numpy as np
import matplotlib
import matplotlib.pyplot as plt

from hftools import *
from htypes import *
from vector import *
from harray import *

#======================================================================
#  Define Plotting functions for vectors and arrays
#======================================================================
plot_pause=True
doplot=True

class plotfinish:
    """
    **Usage:**
    
    ``plot_finish_instance=plotfinish(name="",plotpause=True,doplot=True,refresh=True,filename="",filetype="png",counter=0,plotfiles=[])``

    ``plot_finish_instance(txt="",name="",filename="",filetype="",setcounter=0,counter=None,savefig=False,same_row=False)`

    **Description**
    Class creating a function to be called after a plotting
    command. If ``plotpause.plotpause = True`` it will pause and ask
    for user input whether and how to continue calculation and
    plotting. If a filename is provided the figure will be saved to
    disk.


    The following parameters can be specified during initialization of
    the class or during a call to the instance of the plotfinish
    class.

    =========== ========  =====================================================
    *plotpause* = True    Pause for manual input after each plot.

    *doplot*    = True    Plot at all.

    *refresh*   = True    Refresh (redraw) plot window after each plotting command
                          also in batch mode.

    *filename*  = ""      If set, save figure to this file (w/o extension)

    *filetype*  = "png"   Extension of the figure file to also determine format.

    *txt*       = ""      Text to print after plotting and before user input
                          (parameter not available at initialization)

    *plotfiles* = []      Provide a list, which contains the plotfiles generated so far.
                          (parameter not available at initialization)
                          
    *same_row*            If true, then put the next plotfile in the
                          same 'row' as the previous, i.e. create a sub list. Can be used
                          for output formatting.
                          (parameter only available during call)

    *setcounter*          Reset the counter for the plotfiles (added to names)
                          to this value (parameter only available during call)

    *counter*             Temporarily set the counter for the plotfiles (added to names)
                          to this value (parameter only available during call)

    name        = ""      Name identifying current plot. Will also be appended
                          to filename. (parameter not available at initialization)
    =========== ========  =====================================================

    May modify ``plotpause`` and ``doplot``, depending on user input.

    Filename of the image file will be filename+("-"+name+)"."+filetype

    All filenames that were written to disk can be retrieved as a list
    from the attribute ``self.files``.

    Example:
        ::
            pp=plotfinish(filename="test")
            pp("Test","a")
            -> (a) Test
            -> Saved plot in --> Test-a.png
            -> Press [return] to continue. Press [q]+[return] to proceed without pausing, [n]+[return] to continue without plotting...q
            -> Continue without pausing from now on.

            pp.plotpause -> False
            pp("Try again")
            -> Nothing happens (i.e. no pause)....
    """
    def __init__(self,name="",plotpause=True,doplot=True,refresh=True,filename="",filetype="png",counter=0,plotfiles=[]):
        self.plot_pause=plotpause
        self.doplot=doplot
        self.refresh=refresh
        self.filename=filename
        self.filetype=filetype
        self.plotfiles=plotfiles
        self.counter=counter
        
    def __call__(self,txt="",name="",filename="",filetype="",setcounter=0,counter=None,savefig=False,same_row=False):
        """
        Usage:

        plot_finish_instance(txt="",name="",filename="",filetype="",setcounter=0,counter=None,savefig=False,same_row=False)

        Description:

        Call this after ever plot to save output and ask for user input.
        """
        if self.doplot:
            if not filename: filename=self.filename
            if not filetype: filetype=self.filetype
            (fdir,fname)=os.path.split(filename)
            if setcounter>0:
                self.counter=setcounter
            else:
                self.counter+=1
            if filename:
                if name:
                    f="{0:s}-{1:04d}-{2:s}-{3:s}.{4:s}".format(os.path.join(fdir,"pycrfig"),counter if not counter==None else self.counter,fname,name,filetype)
                else:
                    f="{0:s}-{1:04d}-{2:s}.{3:s}".format(os.path.join(fdir,"pycrfig"),counter if not counter==None else self.counter,fname,filetype)
                if same_row and len(self.plotfiles)>0:
                    if isinstance(self.plotfiles[-1],list):
                        self.plotfiles[-1].append(f)
                    else:
                        self.plotfiles[-1]=[self.plotfiles[-1],f]
                else:
                    self.plotfiles.append(f)
                fig=plt.gcf()
#                dpi=fig.get_dpi()
#                fig.set_dpi(200)
#                fig.set_size_inches(10,10)

                fig.savefig(f,dpi=200)
#               fig.set_dpi(dpi)
                print "Saved plot in -->",f
            if txt:
                print (("("+name+") ") if name else "")+txt
            if self.refresh:
                plt.draw();
            if self.plot_pause:
                k=raw_input("Press key+[return]: [any]=continue, [d]=draw on screen without pause, [q]=quiet plotting, don't pause, [n]=no plots at all. ")
                if k=="q":
                    self.plot_pause=False
                    self.refresh=False
                    print "Continue without pausing from now on."
                elif k=="d":
                    self.plot_pause=False
                    self.refresh=True
                elif k=="n":
                    self.doplot=False
                    self.plot_pause=False
                    print "Continue without plotting and without pausing."
                else:
                    print "Continue."



def hPlot_plot(self,xvalues=None,xlabel=None,ylabel=None,title=None,clf=True,logplot=None,xlim=None,ylim=None,legend=None,highlight=None,nhighlight=None,highlightcolor=None,**plotargs):
    """
    Method of arrays. Plots the current slice. If the array is in
    looping mode, multiple curves are plotted in one windows.

    Usage::

      >>> array[0].plot(self,xvalues=vec,xlabel='x',ylabel='y',title='Title',clf=True,logplot='xy')
      -> plot the array (loglog)

    You can set the plotting parameters also as attributes to the
    ``par`` class of the array, e.g., ``array.par.xvalues=x_vector;
    array.plot()``

    Parameters:

    ================ ======================================================================
    Parameter        Description
    ================ ======================================================================
    *xvalues*        an array with corresponding *x* values, if ``None``
                     numbers from 0 to length of the array are used.
    *xlabel*         the *x*-axis label, if not specified, use the ``name``
                     keyword of the ``xvalues`` array - units will be added
                     automatically.
    *ylabel*         the *y*-axis label, if not specified, use the ``name``
                     keyword of the array - units will be added automatically.
    *xlim*           tuple with minimum and maximum limits for the *x*-axis.
    *ylim*           tuple with minimum and maximum limits for the *y*-axis.
    *legend*         plots a legend, taking a tuple of strings for each
                     plotted line as input, e.g. ``legend=('A','B',...)``.
    *title*          the title for the plot
    *clf*            if ``True`` (default) clear the screen beforehand
                     (use ``False`` to compose plots with multiple lines
                     from different arrays.
    *logplot*        can be used to make loglog or semilog plots:
                       False -> linear plot
                       None  -> take defaults from array if present
                       'x'   -> semilog in x
                       'y'   -> semilog in y
                       'xy'  -> loglog plot

    *highlightcolor* color (e.g. 'red'), used to highlight sections of the plot

    *nhighlight*     determines how many (if any) sections of the data to highlight.

    *highlight*      ylist, vector, or array of tuples with start and end indices of
                     sections to highlight

    *plotarg1*       =..., plotarg2=...: you can add any plotting parameter
                     that is understood by ``.plot`` of scipy, e.g. color = 'green'
                     (see following parameters)

    *color*          = scipy name of plotting color (e.g. 'green')
    *linestyle*      = scipy style of line plotting (e.g. 'dashed', or '' for none)
    *marker*         = scipy style for data points (e.g. 'x' or 'o')
    ================ ======================================================================
    """
#    if EDP64bug==None and hasattr(self.plt,"EDP64bug"):
#        EDP64bug=self.plt.EDP64bug
    if (xvalues==None):
        if hasattr(self.par,"xvalues"):
            if hasattr(self,"__slice__"):
                dim_difference=self.getNumberOfDimensions() - self.par.xvalues.getNumberOfDimensions()
                if dim_difference==0:
                    xvalues=self.par.xvalues.getSlicedArray(self.__slice__)
                elif dim_difference>0:
                    ellipsiscount=self.__slice__.count(Ellipsis) # check if an ellipsis is present that occupies and extra slot in the index list
                    if ellipsiscount==1:
                        ellipsislocation=self.__slice__.index(Ellipsis)
                        if ellipsislocation==0 or type(self.__slice__[ellipsislocation-1]) in [int,long]:
                            ellipsiscount=0
                    dim_difference+=ellipsiscount
                    if dim_difference==len(self.__slice__):
                        xvalues=self.par.xvalues
                    elif dim_difference<len(self.__slice__):
                        xvalues=self.par.xvalues.getSlicedArray(self.__slice__[dim_difference:])
                    else:
                        raise IndexError("Slicing made y-value array dimenisons too small for x-value array in plot.")
                else:
                    raise IndexError("x-value array has higher dimensions than y-value array in plot.")
            else:
                xvalues=self.par.xvalues
        else:
            xvalues=hArray(range(len(self.vec())))
    xunit=xvalues.getUnit().replace("\\mu","$\\mu$")
    if not xunit=="": xunit=" ["+xunit+"]"
    yunit=self.getUnit().replace("\\mu","$\\mu$")
    if not yunit=="": yunit=" ["+yunit+"]"
    if xlabel==None: xlabel=xvalues.getKey("name")
    if ylabel==None: ylabel=self.getKey("name")
    var="clf"; dflt=True; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec(var+"="+str(dflt.__repr__()))
    var="xlabel"; dflt=""; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec(var+"="+str(dflt.__repr__()))
    var="ylabel"; dflt=""; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec(var+"="+str(dflt.__repr__()))
    var="xlim"; dflt=None; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec(var+"="+str(dflt.__repr__()))
    var="ylim"; dflt=None; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec(var+"="+str(dflt.__repr__()))
    var="legend"; dflt=None; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec(var+"="+str(dflt.__repr__()))
    var="title"; dflt=""; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        elif hasattr(self.par,"filename"): exec(var+"=self.par.filename")
        else: exec(var+"="+str(dflt.__repr__()))
    var="logplot"; dflt=""; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec(var+"="+str(dflt.__repr__()))
    var="highlightcolor"; dflt=("red"); val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec(var+"="+str(dflt.__repr__()))
    var="nhighlight"; dflt=[1]; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec(var+"="+str(dflt.__repr__()))
    var="highlight"; dflt=None; val=eval(var);
    if val==None:
        if hasattr(self.par,var): exec(var+"=self.par."+var)
        else: exec(var+"="+str(dflt.__repr__()))
    if type(val)==str: val=(str)
    if clf: self.plt.clf()
#    if logplot=="x": _plot=hSemiLogX if EDP64bug else self.plt.semilogx
#    elif logplot=="y": _plot=hSemiLogY if EDP64bug else self.plt.semilogy
#    elif (logplot=="xy") | (logplot=="yx"): _plot=hSemiLogXY if EDP64bug else self.plt.loglog
    if logplot=="x": _plot=self.plt.semilogx
    elif logplot=="y": _plot=self.plt.semilogy
    elif (logplot=="xy") | (logplot=="yx"): _plot=self.plt.loglog
    else: _plot=self.plt.plot
    iterate=True
    loop=0
    ylen=len(self.vec())
    xlen=len(xvalues.vec())
    while (iterate):
        if ylen<xlen:
            _plot(xvalues.vec()[:ylen],self.vec(),**plotargs)
        else:
            _plot(xvalues.vec(),self.vec(),**plotargs)
        if not highlight==None:
            if type(nhighlight) in [int,long]: nhighlight=[nhighlight]
            hv=ashArray(highlight).vec()
            ha=hArray(hv,[len(hv)/2,2])
            for n in range(nhighlight[loop]): #how many sections are to be highlighted?
                slc=slice(ha[n,0],ha[n,1]+1)
                _plot(xvalues.vec()[slc],self.vec()[slc],color=highlightcolor)
        xvalues.next()
        iterate=self.next().doLoopAgain()
        loop+=1;
    self.plt.ylabel(ylabel+yunit)
    self.plt.xlabel(xlabel+xunit)
    if not xlim==None: self.plt.xlim(*xlim)
    if not ylim==None: self.plt.ylim(*ylim)
    if not legend==None: self.plt.legend(legend)
    if not ((title=="") | (title==None)): self.plt.title(title)

for v in hAllArrayTypes:
    setattr(v,"plt",plt)
    setattr(v,"plot",hPlot_plot)


def hSemiLogY(x,y,**args):
    """Total frustration avoid EDP64 crash on new Mac function"""
    xvec=x
    yvec=Vector(float,len(y)); yvec.copy(y); yvec.log10()
    plt.plot(xvec,yvec,**args)

def hSemiLogX(x,y,**args):
    """Total frustration avoid EDP64 crash on new Mac function"""
    yvec=y
    xvec=Vector(float,len(x)); xvec.copy(x); xvec.log10()
    plt.plot(xvec,yvec,**args)

def hSemiLogXY(x,y,**args):
    """Total frustration avoid EDP64 crash on new Mac function"""
    yvec=Vector(float,len(y)); yvec.copy(y); yvec.log10()
    xvec=Vector(float,len(x)); xvec.copy(x); xvec.log10()
    plt.plot(xvec,yvec,**args)

def plotconst(xvalues,y):
    """
    Plot a constant line.

    Usage::

      >>> plotconst(xvalues,y).plot()
      -> will plot a constant line with y-value 'y' for the xvalues provided.

    """
    return hArray([y,y],xvalues=hArray([xvalues.vec()[0],xvalues.vec()[-1]]))

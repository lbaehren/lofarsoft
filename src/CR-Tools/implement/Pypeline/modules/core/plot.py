"""Provides an n-dimensional array class.
"""
#import pdb
#pdb.set_trace()

import numpy as np
import matplotlib.pyplot as plt

from hftools import *
from htypes import *
from vector import *
from harray import *

#======================================================================
#  Define Plotting functions for vectors and arrays
#======================================================================

def hPlot_plot(self,xvalues=None,xlabel=None,ylabel=None,title=None,clf=True,logplot=None,xlim=None,ylim=None,legend=None,highlight=None,nhighlight=None,highlightcolor=None,EDP64bug=None,**plotargs):
    """
    Method of arrays. Plots the current slice. If the array is in
    looping mode, multiple curves are plotted in one windows.

    If your name is Heino and you have a MacBook more recent then
    March 2011 with OS10.6.7 and you use the Enthought 64 bit (EDP64)
    Python version 6.3 then set plt.EDP64bug=True to avoid problems
    with semilog axes.Semilog will then work only with reduced
    functionality.

    Usage::

      >>> array[0].plot(self,xvalues=vec,xlabel='x',ylabel='y',title='Title',clf=True,logplot='xy')
      -> plot the array (loglog)

    You can set the plotting parameters also as attributes to the
    ``par`` class of the array, e.g., ``array.par.xvalues=x_vector;
    array.plot()``

    Parameters:

    ============ ============================================================
    Parameter    Description
    ============ ============================================================
    *xvalues*    an array with corresponding *x* values, if ``None``
                 numbers from 0 to length of the array are used.
    *xlabel*     the *x*-axis label, if not specified, use the ``name``
                 keyword of the ``xvalues`` array - units will be added
                 automatically.
    *ylabel*     the *y*-axis label, if not specified, use the ``name``
                 keyword of the array - units will be added automatically.
    *xlim*       tuple with minimum and maximum limits for the *x*-axis.
    *ylim*       tuple with minimum and maximum limits for the *y*-axis.
    *legend*     plots a legend, taking a tuple of strings for each
                 plotted line as input, e.g. ``legend=('A','B',...)``.
    *title*      the title for the plot
    *clf*        if ``True`` (default) clear the screen beforehand
                 (use ``False`` to compose plots with multiple lines
                 from different arrays.
    *logplot*    can be used to make loglog or semilog plots:
                   False -> linear plot                                                     
                   None  -> take defaults from array if present
                   'x'   -> semilog in x
                   'y'   -> semilog in y
                   'xy'  -> loglog plot
    *plotarg1*   =..., plotarg2=...: you can add any plotting parameter
                 that is understood by ``.plot`` of scipy, e.g.
                 ``color='green``, ``linestyle='dashed``.
    ============ ============================================================
    """
    if EDP64bug==None and hasattr(self.plt,"EDP64bug"):
        EDP64bug=self.plt.EDP64bug
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
    if logplot=="x": _plot=hSemiLogX if EDP64bug else self.plt.semilogx 
    elif logplot=="y": _plot=hSemiLogY if EDP64bug else self.plt.semilogy
    elif (logplot=="xy") | (logplot=="yx"): _plot=hSemiLogXY if EDP64bug else self.plt.loglog
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
            hv=highlight.vec()
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

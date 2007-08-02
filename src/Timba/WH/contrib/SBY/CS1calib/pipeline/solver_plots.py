#!/usr/bin/env python
# print png to standard out
# usage: python solver_plots.py Logfile.log



#% $Id$ 

#
# Copyright (C) 2006
# ASTRON (Netherlands Foundation for Research in Astronomy)
# and The MeqTree Foundation
# P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

def create_residual_plots(debug_file=None,sig_zeromean=True,sig_rescale=True,sig_logplot=True,sig_percent=0.7,npages=4):
      """produce printable plots from solver log files
         debug_file: solver log file
         sig_zeromean: remove mean values befor plotting
         sig_rescale: rescale by dividing by max abs value to fit paper
         sig_percent: percentage of rescale
         sig_logplot: plot in log scale
         npages: number of pages to produce
      """
      from pylab import figure, show, setp, connect, draw, savefig, rc
      from matplotlib.numerix import sin, cos, exp, pi, arange, log10
      from matplotlib.numerix.mlab import mean
      from matplotlib.transforms import Bbox, Value, Point, \
         get_bbox_transform, unit_bbox
      # for saving
      import matplotlib
      import sys
      import os
      import time
      import numpy
      matplotlib.use('Agg')



      if debug_file==None: return;
      # check for non existant files
      try:
         ff=open(debug_file,'r');
      except IOError:
         print "No file %s!"%debug_file
         return;

     
      (signals0,names)=read_array(ff)
      ff.close()
      # calculate dummy t
      nx=len(signals0[0])
      #print nx
      t=arange(0.0,nx,1)

      # plotting limits
      num_start=1
      num_end=len(names)
      lines_per_plot=(num_end-num_start+1)/npages
 
      signals=[]
      # remember means and scalings
      sig_means=[]
      sig_scales=[]
      
      for sig in signals0:
        sig_means+=[mean(sig)]
        # calculate true scale
        s_min=min(sig)
        s_max=max(sig)
        s_scale=max(abs(s_min),abs(s_max))
        sig_scales+=[s_scale]
        if sig_logplot:
           # find zeros, replace with min value
           if s_min==0:
             sig[numpy.where(sig==0)]=1e-9
           else:
             sig[numpy.where(sig==0)]=s_min
           sig=log10(sig)
        if len(sig)<nx:
          # extend length
          sig=N.concatenate((sig,N.zeros(nx-len(sig))))
        if sig_zeromean:
           sig = sig-mean(sig)
        if sig_rescale:
           s_min=min(sig)
           s_max=max(sig)
           s_scale=max(abs(s_min),abs(s_max))*sig_percent
           if s_scale!=0:
            sig=sig/s_scale
        signals+=[sig]

      signals_orig=signals;
      
      pagenum=1;
      num_low=num_start;
      num_high=num_low+lines_per_plot-1;
      while pagenum<=npages:
        #print (pagenum,npages,num_low,num_high)
        signals=[]
        for i,sig in enumerate(signals_orig):
          if i<=num_high and i>=num_low:
           signals+=[sig]
       
      
        ##### default figure properties
        rc('lines', lw=0.4)
        rc('font', size=10)
        rc('axes', titlesize=10)
        rc('axes', labelsize=7)
        rc('axes', labelcolor='black')
        rc('xtick', labelsize=7)
        rc('ytick', labelsize=7)
      
        lineprops = dict(linewidth=0.4, color='black', linestyle='-')
        fig = figure()
        ax = fig.add_axes([0.1, 0.1, 0.8, 0.8])
      
        # The normal matplotlib transformation is the view lim bounding box
        # (ax.viewLim) to the axes bounding box (ax.bbox).  Where are going to
        # define a new transform by defining a new input bounding box. See the
        # matplotlib.transforms module helkp for more information on
        # transforms
      
        # This bounding reuses the x data of the viewLim for the xscale -10 to
        # 10 on the y scale.  -10 to 10 means that a signal with a min/max
        # amplitude of 10 will span the entire vertical extent of the axes
        scale = 10
        boxin = Bbox(
          Point(ax.viewLim.ll().x(), Value(-scale)),
          Point(ax.viewLim.ur().x(), Value(scale)))
      
      
        # height is a lazy value
        height = ax.bbox.ur().y() - ax.bbox.ll().y()
      
        boxout = Bbox(
            Point(ax.bbox.ll().x(), Value(-0.5) * height),
            Point(ax.bbox.ur().x(), Value( 0.5) * height))
      
      
        # matplotlib transforms can accepts an offset, which is defined as a
        # point and another transform to map that point to display.  This
        # transform maps x as identity and maps the 0-1 y interval to the
        # vertical extent of the yaxis.  This will be used to offset the lines
        # and ticks vertically
        transOffset = get_bbox_transform(
            unit_bbox(),
            Bbox( Point( Value(0), ax.bbox.ll().y()),
                  Point( Value(1), ax.bbox.ur().y())
                  ))
      
        # now add the signals, set the transform, and set the offset of each
        # line
        ticklocs = []
        for i, s in enumerate(signals):
            trans = get_bbox_transform(boxin, boxout)
            offset = (i+1.)/(len(signals)+1.)
            trans.set_offset( (0, offset), transOffset)
        
            if len(t)==len(s):
             ax.plot(t, s, transform=trans, **lineprops)
            ticklocs.append(offset)
      
      
        ax.set_yticks(ticklocs)
        ax.set_yticklabels(['%s     %2.1e   %2.1e'%(names[num_low+i],sig_means[num_low+i],sig_scales[num_low+i]) for i in range(len(signals))])
        #ax.set_yticklabels(['%s:'%(names[num_low+i]) for i in range(len(signals))])
      
        # place all the y tick attributes in axes coords  
        all = []
        labels = []
        ax.set_yticks(ticklocs)
        for tick in ax.yaxis.get_major_ticks():
            all.extend(( tick.label1, tick.label2, tick.tick1line,
                         tick.tick2line, tick.gridline))
            labels.append(tick.label1)
        
        setp(all, transform=ax.transAxes)
        #setp(labels, x=0.8)
        setp(labels, x=0.14)
      
        ax.set_xlabel('Page ('+str(pagenum)+'/'+str(npages)+') date='+time.asctime()+' (Zero Mean:'+str(sig_zeromean)+' Rescale:'+str(sig_rescale)+' Log:'+str(sig_logplot)+')')
      
        set_ygain.scale = scale
#       connect('key_press_event', keypress)
        #ax.set_title('Zero Mean:'+str(sig_zeromean)+' Rescale:'+str(sig_rescale)+' Log:'+str(sig_logplot))
        ax.set_title(debug_file)
      
        savefig(debug_file+'_'+str(pagenum)+'.png',dpi=300,papertype='a4',format='png')
        os.spawnvp(os.P_NOWAIT,'a2ps',['a2ps','-1','-o',debug_file+'_'+str(pagenum)+'.ps',debug_file+'_'+str(pagenum)+'.png'])

        ## go to the next page
        num_low=num_low+lines_per_plot
        num_high=num_high+lines_per_plot
        pagenum=pagenum+1
      

# Because we have hacked the transforms, you need a special method to
# set the voltage gain; this is a naive implementation of how you
# might want to do this in real life (eg make the scale changes
# exponential rather than linear) but it gives you the idea
def set_ygain(direction):
    set_ygain.scale += direction
    if set_ygain.scale <=0:
        set_ygain.scale -= direction
        return

    for line in ax.lines:
        trans = line.get_transform()
        box1 =  trans.get_bbox1()
        box1.intervaly().set_bounds(-set_ygain.scale, set_ygain.scale)
    draw()

def keypress(event):
    if event.key in ('+', '='): set_ygain(-1)
    elif event.key in ('-', '_'): set_ygain(1)


def read_array(infile, dtype='float32', separator=None):
    """ Read a file with an arbitrary number of columns.
        The type of data in each column is arbitrary
        It will be cast to the given dtype at runtime
        use None as separator to handle spaces correctly
    """
    # load the data
    import numpy as N


    cast = N.cast
    # open file and get columns
    # remove names
    line=infile.next()
    names= line.strip().split(separator)
    ncols=len(names)
    #print ncols
    data = [[] for dummy in xrange(ncols)]
    for line in infile:
        fields = line.strip().split(separator)
        for i, number in enumerate(fields):
            data[i].append(number)
    for i in xrange(ncols):
        data[i] = cast[dtype](data[i])
    infile.close()
    return (data,names)



######## test
if __name__ == '__main__':
  import sys
  argc=len(sys.argv)
  if argc>1:
   create_residual_plots(sys.argv[1],npages=4)


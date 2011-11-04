from numpy import *
from MeasurementSet import MeasurementSet

class HistogramGenerator:
   "Generates histograms from a measurement set, which is useful for detection of RFI"
   
   def __init__(self, ms):
      "ms is a measurement set object which should contain the getBaselineData() method."
      self.ms = ms
      
   def generateHistogramFromData(cls, data, xsteps=None, rcoeff=1.0):
      """
      This is a class method. It generates a histogram from an array which is assumed to be measurement data. The expected histogram shape is a gaussian shape with distortions caused by RFI to the right of it. The boundaries and size of deltaX are determined with this shape in mind. Any RFI signal that is off the horizontal scale of the histogram will be appended to the highest value within the histogram, such that the surface of the histogram corrensponds to the sample count.
      input:
      data   - one dimensional array of singular values
      xsteps - provide amount of steps in the histogram. Default is auto
      rcoeff - a higher value increases the max value of the histogram
      """
      if xsteps == None: xsteps = round(sqrt(len(data))*3) * (0.5*rcoeff + 0.5)
      data = log(data)  # obtain a more convenient scale - RFI is up to 10^7 stronger than the gaussian
   # to determine the histogram boundaries, we start at the minimum value of the data range. The maximum values is the distance from the median to the min() added to the median. RFI will distort the median to the right such that there is enough space for its representation to the right   
      start, end = min(data), median(data) + rcoeff*(median(data)-min(data))
      bins = arange(start, end, (end-start)/xsteps)
      N, bins = histogram(data.clip(start,end), bins)
      return N, bins
#    peak = bins[N.tolist().index(N.max())]
#    cap = peak + (peak-min(ch1fl))
   generateHistogramFromData = classmethod(generateHistogramFromData)
   
      
      
      
      
      
      
"""
A histogram can be made over two variables:
      - over a specified time    (for long term autocorrelations, needs gal foreground fit)
      - over a range of channels (requires bandpass-correction)
      
The scaling for a histogram can be difficult since RFI can be up to ten orders of magnitude higher than the expected gaussian shape. It is probably best to use a logarithmic scale, and cut off at a certain max X value. Any value that would be off the scale will be appended to the highest value in the histogram.
"""
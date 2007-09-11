from numpy import *        # numpy is god

class PercentileGenerator:
   "Generates percentile lines from 2-dimensional arrays and measurement sets"
   
   def __init__(self, ms):
      self.ms = ms

   def generatePercentileFromData(cls, data, ps=[0, 25, 50, 75, 100],axis=1):
      """
      Generates a list of 1-dim arrays which describe a contour under which P percent of the data lies.
      input:
      data - a two dimensional array
      ps   - a list of percentages, one for each contourline
      axis - the axis across which the data is collapsed
      """
      odata = data.copy()
      odata.sort(axis=axis)
      return array([odata[:, floor((len(odata[0])-1)* p/100.0)] for p in ps])
   generatePercentileFromData = classmethod(generatePercentileFromData)
   

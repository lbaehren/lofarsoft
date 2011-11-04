from HBA_I200Mhz import *
from S200Mhz_Interpreter import *
from Tools import generateHistogram, BANDPASS

### from ipython Environment.py
# st.cd('S200Mhz')
# S200Mhz_Analysis.generateAllHistograms(ms, st['/NP/binscale'], st)

def generateAllHistograms(msi, bins, stor, offs=0):
   """msi should be a S200Mhz_MS instance, or one with an identical interface
   bins is the array marking the limits of each histogram bar. A sample bins on log scale is in analysis.h5:/NP/binscale.
   stor is a Storage object where the histograms will be stored in Histogram/* from its relative path"""
   sblist = msi.listAvailableSubbands()
   sblist = [i for i in sblist if i >= offs]
   for sb in sblist:
      #print sb
      #(ms, a) = msi.getMSFileBySubband(sb)
      #bns, dta = generateHistogram(ms, (an+1, an+1), 0, 0, 24, bins=bins)
      #stor.save('Histogram/sb%03d' % sb, dta, compress=True)
      
      # going to cheat a little since pycasa doesn't close tables after Python dereferences them, therefore
      # it can only open ~100 tables before it reaches the table-lock limit. When that is fixed, the above
      # three lines of code will suffice. But for now:
      (msName, antenna, relsubband) = getMSBySubband(sb)
      if antenna == 0:
         print sb, ': '
         (ms, cr) = msi.getMSFileBySubband(sb)
         for an in range(4):
            sbn = getAbsoluteSubband(msName, relsubband, an)
            print sbn
            bns, dta = generateHistogram(ms, (an+1, an+1), 0, 0, 24, bins=bins)
            stor.save('Histogram/sb%03d' % sbn, dta, compress=True)
         del ms
   
   
def flagAllSubbands(msi, flagger, stor, offs=0):
   """msi should be a S200Mhz_MS instance, or one with an identical interface
   flagger should be a flagging function like the ones in Flaggers.py
   stor is a Storage object where the flagdata will be stored in Flags/* from its relative path"""
   sblist = msi.listAvailableSubbands()
   sblist = [i for i in sblist if i >= offs]
   for sb in sblist:
      #print sb
      #(ms, a) = msi.getMSFileBySubband(sb)
      #flagarray = flagger(ms, (a+1, a+1), 0)
      #stor.save('Flags/sb%03d' % sb, flagarray, compress=True)
      
      # going to cheat a little since pycasa doesn't close tables after Python dereferences them, therefore
      # it can only open ~100 tables before it reaches the table-lock limit. When that is fixed, the above
      # three lines of code will suffice. But for now:
      (msName, antenna, relsubband) = getMSBySubband(sb)
      if antenna == 0:
         (ms, cr) = msi.getMSFileBySubband(sb)
         for an in range(4):
            sbn = getAbsoluteSubband(msName, relsubband, an)
            print sbn
            flagarray = flagger(ms, (an+1, an+1), 0)
            stor.save('Flags/sb%03d' % sbn, flagarray, compress=True)
         del ms
   

def getFlaggedSubband(msi, st, subband, integrate=1):
   """use this after all Flags are generated with flagAllSubbands()
   msi - S200Mhz_MS instance
   st  - Storage object containing the flags in subdir 'Flags'
   subband - subband number
   integrate - amount of steps to integrate into one
   """
   (ms, an) = msi.getMSFileBySubband(subband)
   flagarray = st['Flags/sb%03d' % subband]
   return ms.getBaselineData((an+1, an+1), 0, polars=0, time=(0, -1, 1, integrate), flags=flagarray)
   
   
def getPercentileFromHistogram():
   pass
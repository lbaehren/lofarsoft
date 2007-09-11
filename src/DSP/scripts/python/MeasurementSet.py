import pycasa
import time
import string
from numpy import *


# time in the MS is based on MJD in seconds... too much of a hassle write a converter, 
# so I just use an offset. It should be accurate for the near future
# 2007-06-29 15:18:17
MVTIME_OFFSET = 4689847097 - 1183123097

def MVtoUnix(mvtime):
   return mvtime - MVTIME_OFFSET
#[0.47917056206591, 
BANDPASS = [0.51450263071820, 0.55013957156187, 0.58584868428573, 0.62138865122171, 0.65651292433973, 0.69097334380148, 0.72452392243909, 0.75692472409382, 0.78794575881674, 0.81737081468304, 0.84500114455045, 0.87065892660331, 0.89419042002261, 0.91546874161156, 0.93439619563701, 0.95090609741619, 0.96496404112995, 0.97656857377370, 0.98575124981431, 0.99257605471542, 0.99713819970126, 0.99956230460231, 1.00000000000000, 0.99862699379184, 0.99563966036629, 0.99125122245431, 0.98568760608377, 0.97918305760825, 0.97197561825935, 0.96430255588093, 0.95639585529698, 0.94847786806089, 0.94075721911367, 0.93342506219264, 0.92665176779672, 0.92058411731185, 0.91534306477258, 0.91102211398211, 0.90768634367826, 0.90537209750188, 0.90408733910995, 0.90381265630883, 0.90450288199946, 0.90608928445621, 0.90848226541450, 0.91157449200084, 0.91524437804093, 0.91935982201606, 0.92378210313208, 0.92836983378122, 0.93298286620382, 0.93748605340990, 0.94175276933707, 0.94566810066759, 0.94913163249466, 0.95205976184504, 0.95438748660080, 0.95606963223775, 0.95708149459414, 0.95741889315667, 0.95709764564320, 0.95615249051559, 0.95463549902499, 0.95261403205704, 0.95016830902312, 0.94738866600390, 0.94437258801850, 0.94122160546060, 0.93803814727657, 0.93492244330520, 0.93196956537344, 0.92926669134954, 0.92689066856720, 0.92490594309794, 0.92336290956911, 0.92229672296394, 0.92172659949879, 0.92165561868575, 0.92207102350523, 0.92294500068830, 0.92423590888613, 0.92588990940517, 0.92784294259746, 0.93002298325291, 0.93235250072533, 0.93475104425126, 0.93713787113164, 0.93943453520778, 0.94156735536265, 0.94346968852602, 0.94508393869381, 0.94636324255392, 0.94727278314801, 0.94779069524276, 0.94790853934551, 0.94763133515851, 0.94697715928785, 0.94597632576900, 0.94467018101252, 0.94310955670785, 0.94135293468239, 0.93946438637821, 0.93751135621784, 0.93556236248781, 0.93368469134975, 0.93194215914449, 0.93039301531247, 0.92908805311787, 0.92806898810783, 0.92736715510567, 0.92700256382656, 0.92698334126653, 0.92730557623643, 0.92795356820590, 0.92890046941120, 0.93010929639068, 0.93153427515032, 0.93312247341126, 0.93481566419362, 0.93655235763669, 0.93826993267811, 0.93990679717679, 0.94140450436077, 0.94270975512823, 0.94377621967076, 0.94456611799130, 0.94505150695426, 0.94521523126914, 0.94505150695426, 0.94456611799130, 0.94377621967076, 0.94270975512823, 0.94140450436077, 0.93990679717679, 0.93826993267811, 0.93655235763669, 0.93481566419362, 0.93312247341126, 0.93153427515032, 0.93010929639068, 0.92890046941120, 0.92795356820590, 0.92730557623643, 0.92698334126653, 0.92700256382656, 0.92736715510567, 0.92806898810783, 0.92908805311787, 0.93039301531247, 0.93194215914449, 0.93368469134975, 0.93556236248781, 0.93751135621784, 0.93946438637821, 0.94135293468239, 0.94310955670785, 0.94467018101252, 0.94597632576900, 0.94697715928785, 0.94763133515851, 0.94790853934551, 0.94779069524276, 0.94727278314801, 0.94636324255392, 0.94508393869381, 0.94346968852602, 0.94156735536265, 0.93943453520778, 0.93713787113164, 0.93475104425126, 0.93235250072533, 0.93002298325291, 0.92784294259746, 0.92588990940517, 0.92423590888613, 0.92294500068830, 0.92207102350523, 0.92165561868575, 0.92172659949879, 0.92229672296394, 0.92336290956911, 0.92490594309794, 0.92689066856720, 0.92926669134954, 0.93196956537344, 0.93492244330520, 0.93803814727657, 0.94122160546060, 0.94437258801850, 0.94738866600390, 0.95016830902312, 0.95261403205704, 0.95463549902499, 0.95615249051559, 0.95709764564320, 0.95741889315667, 0.95708149459414, 0.95606963223775, 0.95438748660080, 0.95205976184504, 0.94913163249466, 0.94566810066759, 0.94175276933707, 0.93748605340990, 0.93298286620382, 0.92836983378122, 0.92378210313208, 0.91935982201606, 0.91524437804093, 0.91157449200084, 0.90848226541450, 0.90608928445621, 0.90450288199946, 0.90381265630883, 0.90408733910995, 0.90537209750188, 0.90768634367826, 0.91102211398211, 0.91534306477258, 0.92058411731185, 0.92665176779672, 0.93342506219264, 0.94075721911367, 0.94847786806089, 0.95639585529698, 0.96430255588093, 0.97197561825935, 0.97918305760825, 0.98568760608377, 0.99125122245431, 0.99563966036629, 0.99862699379184, 1.00000000000000, 0.99956230460231, 0.99713819970126, 0.99257605471542, 0.98575124981431, 0.97656857377370, 0.96496404112995, 0.95090609741619, 0.93439619563701, 0.91546874161156, 0.89419042002261, 0.87065892660331, 0.84500114455045, 0.81737081468304, 0.78794575881674, 0.75692472409382, 0.72452392243909, 0.69097334380148, 0.65651292433973, 0.62138865122171, 0.58584868428573, 0.55013957156187, 0.51450263071820]



class MeasurementSet:
   
   def __init__(self, path):
      """
         open the measurement set located at path
      """
      self.path = path
      self.name = self.path.split('/')[-1]
      self.MSTable = pycasa.table(path)
      self.__readObservationTable()
      self.__readSpectralWindowTable()
      self.__readDataDescriptionTable()
      self.__readAntennaTable()
      self.__readDSTimeRange()
      self.baselineTimesteps = len(self.MSTable) / len(self.baselines) / len(self.dataDescriptions)
      self.integrationTime = self.MSTable[0]['INTERVAL']
      self.dsliceShape = self.MSTable[0]['DATA'].shape
      if len(self.dsliceShape) == 2:
         self.polarCount = len(self.MSTable[0]['DATA'])
         self.channelCount = len(self.MSTable[0]['DATA'][0])
      else:
         self.channelCount = len(self.MSTable[0]['DATA'])
      # diff(self.timeRange) / float(self.baselineTimesteps)
      
      
   def __readObservationTable(self):
      """
         reads observation metadata from the OBSERVATION subtable. 1 row, contains columns:
         TIME_RANGE, LOG, SCHEDULE, FLAG_ROW, OBSERVER, PROJECT, RELEASE_DATE, SCHEDULE_TYPE, TELESCOPE_NAME
      """
      self.ObsTable = None
      try:
         self.ObsTable  = self.MSTable.keywordSet()['OBSERVATION'][0]
         self.timeRange = map(MVtoUnix, self.ObsTable['TIME_RANGE'])
         self.releaseDate = MVtoUnix(self.ObsTable['RELEASE_DATE'])
         self.telescopeName = self.ObsTable['TELESCOPE_NAME']
      except:
         print 'could not read observation metadata'

            
   def __readSpectralWindowTable(self):
      """
         reads spectral window metadata from the SPECTRAL_WINDOW subtable. 1 row per window
         MEAS_FREQ_REF, CHAN_FREQ, REF_FREQUENCY, CHAN_WIDTH, EFFECTIVE_BW,
         RESOLUTION, FLAG_ROW, FREQ_GROUP, FREQ_GROUP_NAME, IF_CONV_CHAIN,
         NAME, NET_SIDEBAND, NUM_CHAN, TOTAL_BANDWIDTH
      """
      self.SWTable = None
      self.spectralWindows = []
      try:
         self.SWTable = self.MSTable.keywordSet()['SPECTRAL_WINDOW']
         for sw in range(len(self.SWTable)):
            self.spectralWindows.append((self.SWTable[sw]['NAME'], self.SWTable[sw]['CHAN_FREQ'][0], self.SWTable[sw]['CHAN_FREQ'][-1]))
      except:
         print 'could not read spectral window metadata'
   
   
   def __readDataDescriptionTable(self):
      """
         reads the data_description subtable which references DATA_DESC_ID to SPECTRAL_WINDOW_ID
      """
      self.DDTable = None
      self.dataDescriptions = []
      try:
         self.DDTable = self.MSTable.keywordSet()['DATA_DESCRIPTION']
         for dd in range(len(self.DDTable)):
            self.dataDescriptions.append(self.spectralWindows[self.DDTable[dd]['SPECTRAL_WINDOW_ID']])
      except:
         self.dataDescriptions = [None]
         print 'could not read data description metadata'
   
   
   def __readAntennaTable(self):
      """
         reads antenna info - number of antennas available, number of baselines available
      """
      # there is no subtable listing the available baselines, so we're going to figure it out ourselves
      # the maximum amount of baselines (including autocorrelations) for N antennas is 1/2 ((N+1) N)
      self.baselines = []
      self.antennaCount = 0
      try:
         self.antennaCount = len(self.MSTable.keywordSet()['ANTENNA'])
         maxBL = (self.antennaCount**2+self.antennaCount) / 2
         for i in range(maxBL):
            a1, a2 = self.MSTable[i]['ANTENNA1']+1, self.MSTable[i]['ANTENNA2']+1
            if not (a1, a2) in self.baselines:
               self.baselines.append((a1,a2))
         self.baselines.sort()
      except:
         self.baselines = [(0,0)]
         print 'could not read antenna metadata'
            
           
   def __readDSTimeRange(self):
      """
      reads the timerange contained in the maintable of the dataset - this can be different from the observation itself when the observation MS is split in time resolution as well
      """
      itime = self.MSTable[0]['INTERVAL']
      stime = self.MSTable[0]['TIME_CENTROID']-(itime/2)
      etime = self.MSTable[len(self.MSTable)-1]['TIME_CENTROID']+(itime/2)
      self.dsTimeRange = map(MVtoUnix, (stime, etime))
      
            
   def __len__(self):
      """
         returns the number of records in the main table of the measurement set
      """
      return len(self.MSTable)
   
   
   def __getitem__(self, key):
      """
         returns row number (key) from the main table 
      """
      return self.MSTable[key]
   
   def selectBaseline(self, bt, sw=0):
      "Returns a pycasa table object with baseline data"
      if type(bt) == str: bt = (int(bt[:2]), int(bt[2:])) # convert the baseline to a tuple:
      # if the given spectral window is a name, search for it in the datadescription refs
      if type(sw) == str:
         match = [i for i,s in enumerate(self.dataDescriptions) if s[0] == sw]
         if len(match) > 0: ddid = match[0]
      else: ddid = sw
      
      # select the data
      subQry = self.MSTable.query('ANTENNA1==%d && ANTENNA2==%d && DATA_DESC_ID==%d' % (bt[0]-1, bt[1]-1, ddid))
      return subQry

   
   def getBaselineDataSummary(self, bt, sw, recordcount=1500, polar=0):
      """
      obtain an easily plottable summary of the entire baseline of a spectral window. Recordcount is the amount of time records in the returned set. The baseline data will be squeezed in the specified size
      """
      integsize = int(self.baselineTimesteps / recordcount)
      return self.getBaselineData(bt, sw=sw, time=(0,-1,1,integsize), polars=[polar])
   
   def getBaselineData(self, bt, sw=0, time=(), channels=slice(None, None, None), polars=[0], flags=None):
      """
      obtain the data from a given baseline bt, and spectral window sw.
      bt      : a tuple of the format (antenna1, antenna2) or a string of the format 'XXYY'.  First antenna index begins at 1.
      sw      : a name or an index.
      time    : a tuple of the form (start, end, step, integrate). If none provided, it selects all time elements. Integrate is the amount of items to collapse into one item. Use end=-1 to read until the end of the dataset is reached. If the values are floats, they will be interpreted as relative positions ranging from 0..1
      channels: a tuple of the form (start, end, step) or an array containing the channel numbers. If none provided, it selects all channels
      polars  : an array listing the polarizations to return. If none provided, it selects the first one
      flags   : a FlagArray containing flagging information. If flags is not None, all flagged data will be changed to (nan)
      
      examples:
      getBaselineData('0101', 0, time=(0,-1, 1, 60)) - returns a 60 times smaller dataset than the source
      getBaselineData('0101', 0, time=(0,-1, 2, 60)) - 120 times smaller, skips every other time element
      getBaselineData('0101', 0, time=10000, channels=[0,124]) - get the first 10000 records of chnl 0 and 124
      """
      channelwidth = 256
      
      col = self.selectBaseline(bt,sw).getcol('DATA')
      
      # convert time to a fixed size tuple
      if isinstance(time, int): time = (0, time)
      time = time + (0, len(col), 1, 1)[len(time):]
      time = list(time)
      if time[1] == -1: time[1] = len(col)
      if isinstance(time[0], float): time[0] = int(time[0] * len(col))
      if isinstance(time[1], float): time[1] = int(time[1] * len(col))
      # end-point should be multiple of integrate*step
      time[1] = ((time[1] / (time[3]*time[2]))) * time[3] * time[2]
      # an 'int' index should be converted to a list or it messes up the axes
      if isinstance(channels, int): channels = [channels]
      # convert polars to an array of numbers
      if isinstance(polars, int): polars = [polars]
      
      # now that the input is standardized, we now have to figure out fastest way to extract data.
      if time == [0, len(col), 1, 1] and channels == slice(None, None, None):
         if flags is not None:
            return col.get()[polars]*logical_not(flags)
         else:
            return col.get()[polars] # select everything
      else:
         if channels == slice(None, None, None):  # not having to subindex for the channels is much faster
            subs = lambda be, en, st: array([col[i] for i in xrange(be, en, st)])
         else:
            subs = lambda be, en, st: array([col[i][:, channels] for i in xrange(be, en, st)])
            
         if time[3] > 1: # if integration is enabled, do it the hard way
            if flags is not None:
               # flagging adds 60% to calculation time
               el = []
               for be in xrange(time[0], time[1], time[2]*time[3]):
                  en = be+(time[2]*time[3])
                  fs = logical_not(flags[channels, be:en:time[2]])
                  el.append((abs(subs(be, en, time[2])).transpose(1,2,0)*fs).sum(2) / fs.sum(1))
               return array(el).transpose(1,2,0)[polars]
            else:
               return array([subs(be, be+(time[2]*time[3]), time[2]).mean(0) for be in xrange(time[0], time[1], time[2]*time[3])]).transpose(1,2,0)[polars]
         else:
            ta = apply(subs, time[:3]).transpose(1,2,0)[polars]
            if flags is not None:
               return ta * logical_not(flags[channels, time[0]:time[1]:time[2]])
            else:
               return ta
      

                           
              
   def getTimeRange(self, format='%Y-%m-%d %H:%M:%S'):
      """
         returns the timerange of the measurement set in the given format
      """
      tups = map(time.localtime, self.timeRange)
      return (time.strftime(format, tups[0]), time.strftime(format, tups[1]))

   def getDSTimeRange(self, format='%Y-%m-%d %H:%M:%S'):
      """
         returns the timerange of the measurement set in the given format
      """
      tups = map(time.localtime, self.dsTimeRange)
      return (time.strftime(format, tups[0]), time.strftime(format, tups[1]))
            
      
   def info(self):
      """
         returns a string containing general information about this measurement set
      """
      (tst, ten) = self.getTimeRange()
      (dtst, dten) = self.getDSTimeRange()
      s = """Measurement set at %(path)s
observation time: %(tstart)s - %(tend)s UT
dataset time    : %(dtstart)s - %(dtend)s UT
integration time: %(intertime).2f s
antennas        : %(antennacount)d
total record cnt: %(tablelen)d
cnt per baseline: %(blts)d
channels        : %(channelcount)d
""" % {'path':       self.path,              'tstart': tst,                     'tend': ten,
       'intertime':  self.integrationTime,   'antennacount': self.antennaCount, 'tablelen': len(self.MSTable),
       'blts':       self.baselineTimesteps, 'channelcount': self.channelCount, 'dtstart': dtst,
       'dtend':      dten}
      # gather baseline info
      blstr = string.join(map(lambda x: '%02d%02d'%(x[0],x[1]), self.baselines), ', ')
      blinf = "baselines       : %s \n" % blstr 
      # gather spectral window info
      swinf = "spectral windows: \n"
      for sw in self.spectralWindows:
         swinf += "%s: %3.2f - %3.2f Mhz \n" % (sw[0], sw[1]/1E6, sw[2]/1E6)
      return s + blinf + swinf

      
      
      
      
# k = [abs(z[:,i:i+60]).sum(1) / (60-fs[1:,i:i+60].sum(1)) for i in xrange(0, z.shape[1]-60, 60)]       
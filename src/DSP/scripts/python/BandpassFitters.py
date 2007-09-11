"""
The HBA bandpass varies slightly but significantly between measurements. This
module contains several strategies for obtaining the bandpass from a dataset. 
There are two categories of functions: those which extract the cleanest possible
bandpass from a complete dataset (*Extractor), and those which alter a bandpass 
to make it smoother (*Fitter). You can chain multiple fits on top of an extract 
with generateFitter().

a few default strategies are defined in the public scope:
staticFitter, simpleFitter, chebyshevFitter, smoothFitter
"""

from numpy import *
from numpy.linalg import lstsq
from numpy.random import normal


# a clean HBA bandpass found on a subband near 150Mhz. 
BANDPASS_150MHZ = [0.0,0.0016561470693,0.00168621307239,0.00173494277988,0.00180323701352,0.00189050473273,0.00198796857148,0.00209594820626,0.00221698614769,0.00234371447004,0.00247401907109,0.00260670273565,0.00273739197291,0.0028648476582,0.00298394146375,0.0030934761744,0.00319240824319,0.0032774368301,0.00335080199875,0.00340963946655,0.00345394876786,0.00348417088389,0.00350068393163,0.0035015931353,0.00349273206666,0.00347172864713,0.00344162317924,0.00340449437499,0.00336100487038,0.00331126153469,0.00326154008508,0.00320944213308,0.00315623008646,0.00310663157143,0.0030601369217,0.00301658688113,0.00297973561101,0.00294701219536,0.00291978637688,0.00289883906953,0.0028836124111,0.00287558091804,0.0028746263124,0.00287948641926,0.00288905319758,0.00290334690362,0.0029232443776,0.00294631207362,0.00297205499373,0.00299974926747,0.0030304598622,0.00305795529857,0.00308720953763,0.0031165194232,0.00314070726745,0.0031630853191,0.0031830505468,0.00319857173599,0.00321032712236,0.00321742356755,0.00322002032772,0.00322154047899,0.0032124731224,0.00320158293471,0.0031892391853,0.00317201460712,0.00315399188548,0.0031350122299,0.00311469682492,0.00309435091913,0.00307374377735,0.00305484118871,0.00303777889349,0.0030233708676,0.00300945597701,0.00300250342116,0.00299386191182,0.00299076759256,0.00299167190678,0.00299162045121,0.00299792853184,0.00300769903697,0.00301884347573,0.00302943331189,0.003044645302,0.00305958651006,0.00307614775375,0.00309098255821,0.0031056269072,0.00311987148598,0.00313087529503,0.00314355804585,0.00315111875534,0.00315708410926,0.00315912533551,0.00316116842441,0.00315909879282,0.0031550663989,0.00314873130992,0.00314024998806,0.00313211930916,0.00312138767913,0.00310784648173,0.00309445452876,0.00308255827986,0.0030690014828,0.00305884820409,0.00304905767553,0.00304009974934,0.00303312344477,0.00302960700355,0.00302758044563,0.00302610127255,0.00302841048688,0.00303279561922,0.00303921592422,0.00304682180285,0.00305605819449,0.00306744244881,0.00307771982625,0.00308919302188,0.00310024130158,0.00311172753572,0.00312118488364,0.00313034583814,0.00313706952147,0.00314129260369,0.00314553873613,0.00314561300911,0.00314502627589,0.00314174080268,0.00313626858406,0.00312885083258,0.00312143890187,0.0031124711968,0.00310187507421,0.0030898754485,0.00308105302975,0.00306950625964,0.00306392554194,0.00305214105174,0.0030423917342,0.00303674116731,0.00303270365112,0.00303012831137,0.00303042726591,0.00303192064166,0.00303670647554,0.00304415938444,0.00305168377236,0.00306106964126,0.0030736848712,0.00308466586284,0.00309789553285,0.00311084557325,0.00312204123475,0.00313500431366,0.00314414338209,0.00315345986746,0.00316025945358,0.0031638876535,0.00316554424353,0.00316554005258,0.00316191813909,0.00315514905378,0.00314848124981,0.00313930097036,0.00312612578273,0.00311150285415,0.0030967395287,0.00308112124912,0.00306464312598,0.00305227027275,0.00303707597777,0.00302429543808,0.00301464647055,0.00300614675507,0.00300216069445,0.00299846427515,0.00299891200848,0.00300338421948,0.00301048532128,0.00302136247046,0.00303302775137,0.00304768420756,0.00306512624957,0.00308330985717,0.00310363504104,0.00312422541901,0.00314378365874,0.00316362711601,0.0031842100434,0.00320072425529,0.00321319838986,0.00322176422924,0.00322759943083,0.00323074823245,0.00322729465552,0.00322253000923,0.00321078649722,0.00319737242535,0.00317543558776,0.00315508642234,0.00312802009284,0.00310076912865,0.00307182478718,0.00304095097817,0.00301283155568,0.00298465020023,0.00295751751401,0.00293450406753,0.00291514093988,0.00289997621439,0.00289095565677,0.00288570742123,0.00288882968016,0.00289665185846,0.00291174626909,0.00293276645243,0.00296086585149,0.00299254409038,0.00303208013065,0.00307436170988,0.00312294787727,0.00317318923771,0.00322607904673,0.00327779212967,0.00332914385945,0.00337799615227,0.00342160509899,0.00345948338509,0.00348982540891,0.00351001811214,0.00352067872882,0.00351818487979,0.00350209069438,0.00347306556068,0.00342882238328,0.00336940097623,0.00329567259178,0.00320905609988,0.00311022461392,0.00299957953393,0.00287797860801,0.00275238999166,0.00262001040392,0.00248625245877,0.00235382956453,0.00222608470358,0.00210411171429,0.0019929385744,0.00189387437422,0.00180730118882,0.00173923652619,0.00168900191784,0.00165780948009 ]

class BandpassExtractor:
   "base class, implements the __call__ function, such that Extractors can be considered a function"
   def __call__(self, data):
      return self.extract(data)

class StaticExtractor(BandpassExtractor):
   "Doesn't actually do a fit, but returns a static bandpass which is generated from a ~10hr clean subband"
   def extract(self, data):
      "No arguments need to be passed."
      return BANDPASS_150MHZ[-len(data):]
   
class MeanExtractor(BandpassExtractor):
   "A fit based on the mean bandpass of the dataset. Works well for large and reasonably clean datasets"
   def extract(self, data):
      "required input: data - a 2-dim array (channel, time) of floats"
      return data.mean(1)

      
class SmartMeanExtractor(BandpassExtractor):
   """
   A more clever version of the MeanExtractor. It attempts to collect a clean subset of the array and generate a bandpass based on that subset.
   """
   def __init__(self, samplesize=0.1, spread=False, bleed=False):
      """
      samplesize: the relative size of the clean subset; 0 < samplesize <= 1
      spread NYI: if spread is True then SmartMeanExtractor will divide the data into
                  smaller sections and obtain the cleanest record from each section.
                  This is useful for when there is a largescale trend in the dataset,
                  e.g. galactic day/night on autocorrelation measurements
      bleed     : data which is close to RFI (in time) is less desirable. If bleed
                  is set true, it is less likely to select records which are near RFI
      """
      self.samplesize = samplesize
      self.spread     = spread
      self.bleed      = bleed
      
   def extract(self, data):
      "required input: data - a 2-dim array (channel, time) of floats"
      tdata = data.transpose()
      datam = tdata.mean(1)
      datac = datam.copy()
      if self.bleed:
         # data which is close to RFI (in time), is less desirable, so we bleed RFI to neighbouring channels
         for i in range(1,len(datam)):
            if (datac[i-1] > datac[i]): datac[i]+= 0.8*(datac[i-1]-datac[i])
         for i in range(len(datam)-2,-1,-1):
            if (datac[i+1] > datac[i]): datac[i]+= 0.8*(datac[i+1]-datac[i])
      # now sort the list
      slist = datac.argsort()
      # trim zero mean values
      slist = [s for s in slist if tdata[s].mean() > 0]
      takeam = int(self.samplesize * len(tdata))
      datset = tdata[slist[:takeam]].mean(0)
      return datset
      
      
class BandpassFitter:
   "base class, implements the __call__ function, such that Fitters can be chained"
   def __call__(self, bandpass):
      return self.fit(bandpass)

                  
class ChebyshevFitter(BandpassFitter):
   """
   The bandpass shape can be analytically expressed as a Chebyshev polynomial. This
   fitter will apply a Chebyshev fit on a dataset
   """
   def __init__(self, polycount=33):
      """
      polycount: the amount of Chebyshev polynomials to use in a fit.
      """
      self.polycount = polycount
         
   def fit(self, bandpass):
      return ChebyShevFitter.chebyshevFit(bandpass, self.polycount)
   
   def chebyshevFit(cls, bandpass, polycount):
      "Class method. Performs a Chebyshev Polynomial fit on a subband."
      x = arange(0, 1, 1.0/len(bandpass))
      A = zeros((len(bandpass), polycount), float32)
      A[:,0] = 1
      A[:,1] = 2*x
      for n in range(2, polycount):
         A[:,n] = 2*x*A[:,n-1] - A[:,n-2]
      
      (p, residuals, rank, s) = lstsq(A, bandpass)
      return (p*A).sum(axis=1)
   chebyshevFit = classmethod(chebyshevFit)
    
   
   
class SmoothFitter(BandpassFitter):
   """Based on a reference bandpass it will try to smoothen up the input bandpass."""
   def __init__(self, factor=3, bandpass=None):
      """
      factor is the tolerance value for variations of the source bandpass from the reference
      bandpass is the reference bandpass, it uses a default if not provided
      """
      self.factor = float(factor)
      self.bandpass = bandpass
      if self.bandpass == None:
         self.bandpass = BANDPASS_150MHZ
      
         
   def fit(self, bandpass):
      diffs = diff(self.bandpass)
      cp = bandpass.copy()
      # get the amplitude coeff
      amp = median(cp) / median(self.bandpass)
      cnter = 1.0; cnter2 = 1.0
      intv = 0
      for i in range(len(bandpass)-1):
         db = cp[i+1] - cp[i]
         if (db > 0):
            if (diffs[i] < 0):
               #print 'subt at %d for %f * %f = %f' % (i, diffs[i], amp, diffs[i]*amp)
               intv += 1
               cp[i+1] = cp[i]  + diffs[i]*amp
            elif db/(diffs[i]*amp) > ((self.factor-1)/cnter)+1:
               intv += 1
               cp[i+1] = cp[i] + self.factor*diffs[i]*amp
               cnter += 1.0
            else:
               cnter = 1.0
         elif (db < 0):
            if (diffs[i] < 0) and (diffs[i]*amp)/db > ((self.factor-1)/cnter2)+1 and i < 245:
               intv += 1
               cp[i+1] = cp[i] + diffs[i]*amp
               cnter2 += 1.0
            else:
               cnter2 = 1.0
      return cp
            


def generateFilter(fitter = None):
   """Creates a lambda function which filters the bandpass from a dataset. If no fitter (using generateFitter) is specified, it will use the default"""
   if fitter == None: fitter = defaultFitter
   return lambda data: (data.transpose() / fitter(data)).transpose()

   
   
def generateFitter(*args):
   """Generates a chain of fitters on top of an extractor. Each argument should be a Fitter instance and only the first one can be an Extractor. For example:
   fitter = generateFitter(SmartMeanExtractor(bleed=True), SmoothFitter()) """
   a = lambda data: reduce(lambda a,b: b(a), [data] + list(args))
   return a
   
   

# some default strategies:
staticFitter    = StaticExtractor()
simpleFitter    = SmartMeanExtractor()
chebyshevFitter = generateFitter(SmartMeanExtractor(), ChebyshevFitter())
smoothFitter    = generateFitter(SmartMeanExtractor(), SmoothFitter())

defaultFitter   = staticFitter
defaultFilter   = generateFilter(defaultFitter)

from MeasurementSet import *
from FlagIO import FlagArray
from numpy import *

# BAAAAAAAAAAAAAAAAAAAAAAAAD BANDPASS
#BANDPASS2 = [0.51450263071820, 0.55013957156187, 0.58584868428573, 0.62138865122171, 0.65651292433973, 0.69097334380148, 0.72452392243909, 0.75692472409382, 0.78794575881674, 0.81737081468304, 0.84500114455045, 0.87065892660331, 0.89419042002261, 0.91546874161156, 0.93439619563701, 0.95090609741619, 0.96496404112995, 0.97656857377370, 0.98575124981431, 0.99257605471542, 0.99713819970126, 0.99956230460231, 1.00000000000000, 0.99862699379184, 0.99563966036629, 0.99125122245431, 0.98568760608377, 0.97918305760825, 0.97197561825935, 0.96430255588093, 0.95639585529698, 0.94847786806089, 0.94075721911367, 0.93342506219264, 0.92665176779672, 0.92058411731185, 0.91534306477258, 0.91102211398211, 0.90768634367826, 0.90537209750188, 0.90408733910995, 0.90381265630883, 0.90450288199946, 0.90608928445621, 0.90848226541450, 0.91157449200084, 0.91524437804093, 0.91935982201606, 0.92378210313208, 0.92836983378122, 0.93298286620382, 0.93748605340990, 0.94175276933707, 0.94566810066759, 0.94913163249466, 0.95205976184504, 0.95438748660080, 0.95606963223775, 0.95708149459414, 0.95741889315667, 0.95709764564320, 0.95615249051559, 0.95463549902499, 0.95261403205704, 0.95016830902312, 0.94738866600390, 0.94437258801850, 0.94122160546060, 0.93803814727657, 0.93492244330520, 0.93196956537344, 0.92926669134954, 0.92689066856720, 0.92490594309794, 0.92336290956911, 0.92229672296394, 0.92172659949879, 0.92165561868575, 0.92207102350523, 0.92294500068830, 0.92423590888613, 0.92588990940517, 0.92784294259746, 0.93002298325291, 0.93235250072533, 0.93475104425126, 0.93713787113164, 0.93943453520778, 0.94156735536265, 0.94346968852602, 0.94508393869381, 0.94636324255392, 0.94727278314801, 0.94779069524276, 0.94790853934551, 0.94763133515851, 0.94697715928785, 0.94597632576900, 0.94467018101252, 0.94310955670785, 0.94135293468239, 0.93946438637821, 0.93751135621784, 0.93556236248781, 0.93368469134975, 0.93194215914449, 0.93039301531247, 0.92908805311787, 0.92806898810783, 0.92736715510567, 0.92700256382656, 0.92698334126653, 0.92730557623643, 0.92795356820590, 0.92890046941120, 0.93010929639068, 0.93153427515032, 0.93312247341126, 0.93481566419362, 0.93655235763669, 0.93826993267811, 0.93990679717679, 0.94140450436077, 0.94270975512823, 0.94377621967076, 0.94456611799130, 0.94505150695426, 0.94521523126914, 0.94505150695426, 0.94456611799130, 0.94377621967076, 0.94270975512823, 0.94140450436077, 0.93990679717679, 0.93826993267811, 0.93655235763669, 0.93481566419362, 0.93312247341126, 0.93153427515032, 0.93010929639068, 0.92890046941120, 0.92795356820590, 0.92730557623643, 0.92698334126653, 0.92700256382656, 0.92736715510567, 0.92806898810783, 0.92908805311787, 0.93039301531247, 0.93194215914449, 0.93368469134975, 0.93556236248781, 0.93751135621784, 0.93946438637821, 0.94135293468239, 0.94310955670785, 0.94467018101252, 0.94597632576900, 0.94697715928785, 0.94763133515851, 0.94790853934551, 0.94779069524276, 0.94727278314801, 0.94636324255392, 0.94508393869381, 0.94346968852602, 0.94156735536265, 0.93943453520778, 0.93713787113164, 0.93475104425126, 0.93235250072533, 0.93002298325291, 0.92784294259746, 0.92588990940517, 0.92423590888613, 0.92294500068830, 0.92207102350523, 0.92165561868575, 0.92172659949879, 0.92229672296394, 0.92336290956911, 0.92490594309794, 0.92689066856720, 0.92926669134954, 0.93196956537344, 0.93492244330520, 0.93803814727657, 0.94122160546060, 0.94437258801850, 0.94738866600390, 0.95016830902312, 0.95261403205704, 0.95463549902499, 0.95615249051559, 0.95709764564320, 0.95741889315667, 0.95708149459414, 0.95606963223775, 0.95438748660080, 0.95205976184504, 0.94913163249466, 0.94566810066759, 0.94175276933707, 0.93748605340990, 0.93298286620382, 0.92836983378122, 0.92378210313208, 0.91935982201606, 0.91524437804093, 0.91157449200084, 0.90848226541450, 0.90608928445621, 0.90450288199946, 0.90381265630883, 0.90408733910995, 0.90537209750188, 0.90768634367826, 0.91102211398211, 0.91534306477258, 0.92058411731185, 0.92665176779672, 0.93342506219264, 0.94075721911367, 0.94847786806089, 0.95639585529698, 0.96430255588093, 0.97197561825935, 0.97918305760825, 0.98568760608377, 0.99125122245431, 0.99563966036629, 0.99862699379184, 1.00000000000000, 0.99956230460231, 0.99713819970126, 0.99257605471542, 0.98575124981431, 0.97656857377370, 0.96496404112995, 0.95090609741619, 0.93439619563701, 0.91546874161156, 0.89419042002261, 0.87065892660331, 0.84500114455045, 0.81737081468304, 0.78794575881674, 0.75692472409382, 0.72452392243909, 0.69097334380148, 0.65651292433973, 0.62138865122171, 0.58584868428573, 0.55013957156187, 0.51450263071820]

BANDPASS = [0.0,0.0016561470693,0.00168621307239,0.00173494277988,0.00180323701352,0.00189050473273,0.00198796857148,0.00209594820626,0.00221698614769,0.00234371447004,0.00247401907109,0.00260670273565,0.00273739197291,0.0028648476582,0.00298394146375,0.0030934761744,0.00319240824319,0.0032774368301,0.00335080199875,0.00340963946655,0.00345394876786,0.00348417088389,0.00350068393163,0.0035015931353,0.00349273206666,0.00347172864713,0.00344162317924,0.00340449437499,0.00336100487038,0.00331126153469,0.00326154008508,0.00320944213308,0.00315623008646,0.00310663157143,0.0030601369217,0.00301658688113,0.00297973561101,0.00294701219536,0.00291978637688,0.00289883906953,0.0028836124111,0.00287558091804,0.0028746263124,0.00287948641926,0.00288905319758,0.00290334690362,0.0029232443776,0.00294631207362,0.00297205499373,0.00299974926747,0.0030304598622,0.00305795529857,0.00308720953763,0.0031165194232,0.00314070726745,0.0031630853191,0.0031830505468,0.00319857173599,0.00321032712236,0.00321742356755,0.00322002032772,0.00322154047899,0.0032124731224,0.00320158293471,0.0031892391853,0.00317201460712,0.00315399188548,0.0031350122299,0.00311469682492,0.00309435091913,0.00307374377735,0.00305484118871,0.00303777889349,0.0030233708676,0.00300945597701,0.00300250342116,0.00299386191182,0.00299076759256,0.00299167190678,0.00299162045121,0.00299792853184,0.00300769903697,0.00301884347573,0.00302943331189,0.003044645302,0.00305958651006,0.00307614775375,0.00309098255821,0.0031056269072,0.00311987148598,0.00313087529503,0.00314355804585,0.00315111875534,0.00315708410926,0.00315912533551,0.00316116842441,0.00315909879282,0.0031550663989,0.00314873130992,0.00314024998806,0.00313211930916,0.00312138767913,0.00310784648173,0.00309445452876,0.00308255827986,0.0030690014828,0.00305884820409,0.00304905767553,0.00304009974934,0.00303312344477,0.00302960700355,0.00302758044563,0.00302610127255,0.00302841048688,0.00303279561922,0.00303921592422,0.00304682180285,0.00305605819449,0.00306744244881,0.00307771982625,0.00308919302188,0.00310024130158,0.00311172753572,0.00312118488364,0.00313034583814,0.00313706952147,0.00314129260369,0.00314553873613,0.00314561300911,0.00314502627589,0.00314174080268,0.00313626858406,0.00312885083258,0.00312143890187,0.0031124711968,0.00310187507421,0.0030898754485,0.00308105302975,0.00306950625964,0.00306392554194,0.00305214105174,0.0030423917342,0.00303674116731,0.00303270365112,0.00303012831137,0.00303042726591,0.00303192064166,0.00303670647554,0.00304415938444,0.00305168377236,0.00306106964126,0.0030736848712,0.00308466586284,0.00309789553285,0.00311084557325,0.00312204123475,0.00313500431366,0.00314414338209,0.00315345986746,0.00316025945358,0.0031638876535,0.00316554424353,0.00316554005258,0.00316191813909,0.00315514905378,0.00314848124981,0.00313930097036,0.00312612578273,0.00311150285415,0.0030967395287,0.00308112124912,0.00306464312598,0.00305227027275,0.00303707597777,0.00302429543808,0.00301464647055,0.00300614675507,0.00300216069445,0.00299846427515,0.00299891200848,0.00300338421948,0.00301048532128,0.00302136247046,0.00303302775137,0.00304768420756,0.00306512624957,0.00308330985717,0.00310363504104,0.00312422541901,0.00314378365874,0.00316362711601,0.0031842100434,0.00320072425529,0.00321319838986,0.00322176422924,0.00322759943083,0.00323074823245,0.00322729465552,0.00322253000923,0.00321078649722,0.00319737242535,0.00317543558776,0.00315508642234,0.00312802009284,0.00310076912865,0.00307182478718,0.00304095097817,0.00301283155568,0.00298465020023,0.00295751751401,0.00293450406753,0.00291514093988,0.00289997621439,0.00289095565677,0.00288570742123,0.00288882968016,0.00289665185846,0.00291174626909,0.00293276645243,0.00296086585149,0.00299254409038,0.00303208013065,0.00307436170988,0.00312294787727,0.00317318923771,0.00322607904673,0.00327779212967,0.00332914385945,0.00337799615227,0.00342160509899,0.00345948338509,0.00348982540891,0.00351001811214,0.00352067872882,0.00351818487979,0.00350209069438,0.00347306556068,0.00342882238328,0.00336940097623,0.00329567259178,0.00320905609988,0.00311022461392,0.00299957953393,0.00287797860801,0.00275238999166,0.00262001040392,0.00248625245877,0.00235382956453,0.00222608470358,0.00210411171429,0.0019929385744,0.00189387437422,0.00180730118882,0.00173923652619,0.00168900191784,0.00165780948009 ]


def superFlagger():
   ms = MeasurementSet('/dop45_1/koning/data/L2007_02797/SB23.MS')
   
   far = []
   cnt = 0
   
   print 'READING'
   channels = ms.getBaselineData('0101', 0, polars=0)
   channels = channels[0]
   farr = FlagArray(shape=channels.shape)
   
   for cnumber, ch1 in enumerate(channels):
      if cnumber == 0: 
         farr[0] = ones(len(ch1), uint8)
      else:
         print 'flagging channel %d' % cnumber
         # strategy:
         #   - clip at maximum values. A clean signal never rises above median * 0.25 * 10E1
         # ch1 = fromfile('channel1.bin', complex64)
         lch1 = log(abs(ch1))/log(10)
         lch1c = lch1.clip(-10, median(lch1)+0.25)
         #   - polyfit(10) the clipped signal and subtract it from the original
         xax   = arange(len(ch1))
         ch1fl = lch1 - poly1d(polyfit(xax, lch1c, 10), r=0)(xax)
         #   - a histogram of the clean signal should have a gaussianesque shape
         #   RFI will cause a second peak. So we find the gaussian shape and clip 
         #   everything that is not part of it
         start, end = min(ch1fl), median(ch1fl)+(1/2)*abs(min(ch1fl))
         bins = arange(start, end, (end-start)/sqrt(len(ch1)))
         N, bins = histogram(ch1fl, bins)
         peak = bins[N.tolist().index(N.max())]
         cap = peak + (peak-min(ch1fl))
         clch1 = where(ch1fl > cap, None, ch1fl)
         clch1 = clch1.astype(float)
      
         # store flags in a binary array..... pycasa won't let me write them back into the MS
         # resize to a multiple of 8
         print 'saving channel %d: ' % cnumber
         farr[cnumber] = ch1fl > cap
      
   return farr
   #   tarr = ch1fl > cap
   #   tarr.resize(int(ceil(len(tarr)/8.0))*8)
   #   far.append(array([sum(tarr[i:i+8] * (128, 64, 32, 16, 8, 4, 2, 1)) for i in range(0, len(tarr), 8)], ubyte))   
   
def streamedFlagger1(m, bl, sw = 0, bsize=100):
   """m: MeasurementSet, bl: baseline, sw: spectral window
   Simple flagger working on the highest resolution. Running window of bsize. Keeps an average min() and average median () and clips everything that is above median+(median-min)
   """
   BUFFERSIZE = bsize
   minvalues    = []
   medianvalues = []
   bandpass = BANDPASS
   
   col = m.selectBaseline(bl, sw).getcol('DATA')
   ret = zeros((256, len(col)), uint8)
   ret[0] = 1
   # fill the first window
   minvalues    = [(abs(col[rn][0,1:]) / bandpass).min() for rn in xrange(BUFFERSIZE)]
   medianvalues = [median(abs(col[rn][0,1:]) / bandpass) for rn in xrange(BUFFERSIZE)]
   meanmin, meanmed = mean(minvalues), mean(medianvalues)   
   
   for rn in xrange(len(col)):
      if rn % 1000 == 0: print rn
      row = abs(col[rn][0,1:]) / bandpass
      
      meanmin -= minvalues[0] / BUFFERSIZE
      del minvalues[0]
      minvalues.append(row.min())
      meanmin += minvalues[-1] / BUFFERSIZE
      
      meanmed -= medianvalues[0] / BUFFERSIZE
      del medianvalues[0]
      medianvalues.append(median(row))
      meanmed += medianvalues[-1] / BUFFERSIZE
      
      ret[1:, rn] = row > (meanmed + (meanmed-meanmin))
      
   return ret
      
      

      
def streamedFlagger2(m, bl, sw = 0, bsize=100):
   """m: MeasurementSet, bl: baseline, sw: spectral window
   Simple flagger working on the highest resolution. Running window of bsize. Uses the min() and median() of the buffer and clips everything above median+(median-min)
   """
   BUFFERSIZE = bsize
   bandpass = BANDPASS
   
   col = m.selectBaseline(bl, sw).getcol('DATA')
   ret = zeros((256, len(col)), uint8)
   ret[0] = 1
   buff = []
   # fill the first window
   for rn in xrange(BUFFERSIZE):
      buff.extend(abs(col[rn][0,1:]) / bandpass)
   buff = array(buff)
   
   for rn in xrange(len(col)):
      if rn % 1000 == 0: print rn
      
      row = abs(col[rn][0,1:]) / bandpass
#       print 'BEFORE:'
#       print buff
      buff[:-255] = buff[255:]
      buff[-255:] = row
#       print 'AFTER:'
#       print buff
      minvalue = buff.min()
      medvalue = median(buff)
#       print 'MIN: %f' % minvalue
#       print 'MED: %s' % str(medvalue)
      ret[1:, rn] = row > (medvalue + (medvalue-minvalue))
      
   return ret
      
      
      
def streamedFlagger3(m, bl, sw = 0, bsize=200):
   """m: MeasurementSet, bl: baseline, sw: spectral window
   Simple flagger working on the highest resolution. Multiple running windows of bsize, with integration times of 1 and 60. Uses the min() and median() of the buffer and clips everything above median+(median-min). Flagged data will also be 'flagged' in the buffer
   """
   BUFFERSIZE = bsize
   bandpass = BANDPASS
   
   col = m.selectBaseline(bl, sw).getcol('DATA')
   ret = zeros((256, len(col)), uint8)
   ret[0] = 1
   buff = []
   # fill the first window
   for rn in xrange(BUFFERSIZE):
      buff.extend(abs(col[rn][0,1:]) / bandpass)
   buff = array(buff)
   minvalue = buff.min()
   
   for rn in xrange(len(col)):
      if rn % 1000 == 0: print rn
      
      row = abs(col[rn][0,1:]) / bandpass
      spot = (rn % bsize)*255
      buff[spot:spot+255] = row
      
      # single element clipping
      thec = buff.compress(isfinite(buff))
      minvalue = thec.min()
      medvalue = median(thec)
      buff[spot:spot+255] = where(row  > (medvalue + (medvalue-minvalue)), nan, row)
      ret[1:, rn] = row > (medvalue + (medvalue-minvalue))
      
   return ret
      
      
      
def streamedFlagger4(m, bl, sw = 0, bsize=100, jump=20):
   """m: MeasurementSet, bl: baseline, sw: spectral window
   Simple flagger working on the highest resolution. Multiple running windows of bsize, with integration times of 1 and 60. Uses a histogram to find the gaussian of the clean part of the signal and filters everything that falls outside that shape. It creates a histogram and clips every 'jump' records.
   """
   bandpass = BANDPASS
   
   col = m.selectBaseline(bl, sw).getcol('DATA')
   ret = zeros((256, len(col)), uint8)
   ret[0] = 1
   buff = []
   # fill the first window
   for rn in xrange(bsize):
      buff.extend(abs(col[rn][0,1:]) / bandpass)
   buff = array(buff)
   minvalue = buff.min()
   
   for rn in xrange(0, len(col), jump):
      if rn % 1000 == 0: print rn
      
      for ele in xrange(rn, rn+jump):
         row = abs(col[ele][0,1:]) / bandpass
         spot = (ele % bsize)*255
         buff[spot:spot+255] = row
      
      # single element clipping
      thec = buff.compress(isfinite(buff))
      
      start, end = thec.min(), thec.mean()*1.05
      bins = arange(start, end, (end-start)/sqrt(len(thec)))
      N, bins = histogram(thec, bins)
      peak = bins[N.tolist().index(N.max())]
      cap = peak + (peak-min(thec))
      toflg = (row > cap)
      
      for ele in xrange(rn, rn+jump):
         spot = (ele % bsize)*255
         toflg = (buff[spot:spot+255] > cap)
         buff[spot:spot+255] = where(toflg, nan, buff[spot:spot+255])
         ret[1:, ele] = toflg
      
   return ret      

      

      
class Buffer:
   """Makes item assignment and retrieval as if it was one array containing the entire dataset"""
   def __init__(self, blength):
      self.blength = blength
      self.buf = zeros((blength, 255), float)
      
   def __getitem__(self, key):
      if isinstance(key, int):
         return self.buf[key % self.blength]
      if isinstance(key, slice):
         tstart, tstop = key.start % self.blength, key.stop % self.blength
         if tstop > tstart:
            return self.buf[tstart:tstop]
         else:
            return concatenate((self.buf[tstart:], self.buf[:tstop]))
         
   def __setitem__(self, key, value):
      if isinstance(key, int):
         self.buf[key % self.blength] = value
      if isinstance(key, slice):
         tstart, tstop = key.start % self.blength, key.stop % self.blength
         if tstop > tstart:
            self.buf[tstart:tstop] = value
         else:
            vlen = self.blength - tstart
            self.buf[tstart:] = value[:vlen]
            self.buf[:tstop]  = value[vlen:]
   
   def __repr__(self):
      return self.buf.__repr__()
   
   def getIntegrated(self, isize):
      "isize should be a factor in blength"
      if isize > 1:
         retarr = zeros_like(self.buf)
         for i in xrange(0, self.blength, isize):
            msk = logical_not(isnan(self.buf[i:i+isize]))
            smsk = msk.sum(0)
            smsk = where(smsk == 0, nan, smsk)
            retarr[i:i+isize] = nan_to_num(self.buf[i:i+isize]).sum(0) / smsk
         return retarr
      else:
         return self.buf

   
   
   
         
            
   
         
def streamedFlagger5(m, bl, sw = 0, bsize=300, jump=30):
   """Tests at 1 second resolution, evaluates every 30 records, but evaluates the entire buffer.
   Best and fastest flagger of 5"""
   bandpass = BANDPASS
   col = m.selectBaseline(bl, sw).getcol('DATA')
   buf = Buffer(bsize)
   ret = zeros((len(col), 256), uint8)
   ret[:,0] = 1   # first channel is messed up, always
   
   # running window, bsize records long, re-evaluating every jump records
   # first, fill running window with first set of data:
   for i in xrange(bsize): buf[i] = abs(col[i][0,1:]) / bandpass
   
   # loop over the dataset, evaluating every jump
   for rn in xrange(bsize-jump, len(col), jump):
      if rn + jump >= len(col): jump = len(col)-rn
      if rn != 0:       # window is already available at rn == 0
         for i in xrange(rn, rn+jump): buf[i] = abs(col[i][0,1:]) / bandpass
         
      if rn % 1000 < jump: print rn
   
      # get rid of NaNs
      fbuf = buf.buf.flatten()
      thec = fbuf.compress(isfinite(fbuf))
      # find the median and min values
      medv = median(thec)
      minv = thec.min()
      cap  = medv + (medv-minv)
      
      # flag in the current buffer and the final return array
      buf.buf = where(buf.buf > cap, nan, buf.buf)
      ret[rn-bsize+jump:rn+jump, 1:] = isnan(buf[rn-bsize+jump:rn+jump])
      
   return ret.transpose()    
               
      
      
      

def streamedFlagger6(m, bl, sw = 0, bsize=300, jump=30, isizes=(1,10), preflag=None):
   """Tests at 1 and isize second resolution, evaluates every 30 records, but evaluates the entire buffer.
   Best and fastest flagger of 5. Channel agnostic"""
   bandpass = BANDPASS[1:]
   col = m.selectBaseline(bl, sw).getcol('DATA')
   buf = Buffer(bsize)
   ret = zeros((len(col), 256), uint8)
   ret[:,0] = 1   # first channel is messed up, always
   rrow = lambda n: abs(col[n][0,1:]) / bandpass
   if preflag != None:
      rrow_pre = rrow
      def rrow(n):
         prearr = rrow_pre(n) * logical_not(preflag[1:, n])
         return where(prearr == 0, nan, prearr)
   
   # running window, bsize records long, re-evaluating every jump records
   # first, fill running window with first set of data:
   for i in xrange(bsize): buf[i] = rrow(i)
   
   # loop over the dataset, evaluating every jump
   for rn in xrange(bsize-jump, len(col), jump):
      if rn + jump >= len(col): jump = len(col)-rn
      if rn != 0:       # window is already available at rn == 0
         for i in xrange(rn, rn+jump): 
            buf[i] = rrow(i)
         
      if rn % 1000 < jump: print rn

      for isize in isizes:
         # get rid of NaNs
         ibuf = buf.getIntegrated(isize)
         fbuf = ibuf.flatten()
         thec = fbuf.compress(isfinite(fbuf))
         # find the median and min values
         medv = median(thec)
         minv = thec.min()
         cap  = medv + (medv-minv)
         # flag in the current buffer
         buf.buf = where(ibuf > cap, nan, buf.buf)
      
      # update flags in the return array      
      ret[rn-bsize+jump:rn+jump, 1:] = isnan(buf[rn-bsize+jump:rn+jump])
      
      
   return ret.transpose()    
            
      
   
      
# buffer size is too big for small isize, too small for big isize. an alternative is to have jump vary for each isize (be a factor of isize), such that each integrated buffer has its own running window rather than piggybacking on the main window.
def streamedFlagger7(m, bl, sw = 0, bsize=300, jump=30, isizes=(1,10)):
   """Tests at 1 and isize second resolution, evaluates every 30 records, but evaluates the entire buffer.
   Best and fastest flagger of 5. Channel agnostic"""
   bandpass = BANDPASS[1:]
   col = m.selectBaseline(bl, sw).getcol('DATA')
   buf = Buffer(bsize)
   ret = zeros((len(col), 256), uint8)
   ret[:,0] = 1   # first channel is messed up, always
   
   # running window, bsize records long, re-evaluating every jump records
   # first, fill running window with first set of data:
   for i in xrange(bsize): buf[i] = abs(col[i][0,1:]) / bandpass
   
   # loop over the dataset, evaluating every jump
   for rn in xrange(bsize-jump, len(col), jump):
      if rn + jump >= len(col): jump = len(col)-rn
      if rn != 0:       # window is already available at rn == 0
         for i in xrange(rn, rn+jump): buf[i] = abs(col[i][0,1:]) / bandpass
         
      if rn % 1000 < jump: print rn

      for isize in isizes:
         # get rid of NaNs
         ibuf = buf.getIntegrated(isize)
         fbuf = ibuf.flatten()
         thec = fbuf.compress(isfinite(fbuf))
         # find the median and min values
         medv = median(thec)
         minv = thec.min()
         cap  = medv + (medv-minv)
         # flag in the current buffer
         buf.buf = where(ibuf > cap, nan, buf.buf)
      
      # update flags in the return array      
      ret[rn-bsize+jump:rn+jump, 1:] = isnan(buf[rn-bsize+jump:rn+jump])
      
   return ret.transpose()    
                     
      
"""
for mname in lst:
   m = MeasurementSet('/dop45_1/koning/data/L2007_03017/'+mname)
   for a in range(4):
       myf = Flaggers.streamedFlagger2(m, (a+1, a+1))
       imgd = m.getBaselineData((a+1, a+1), 0, time=(0, 1.0, 1, 60), flags=myf)
       st.save('FLAGGED/L03017_SB'+mname[2]+'A'+str(a+1), imgd)      
       
"""
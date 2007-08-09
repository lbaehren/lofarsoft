Parameterset := 1;

########################################
# Selection 1 from thesis
if (Parameterset == 1) {
########################################
#Maximum separation between shower core and center of LOPESs antennas.
  rlimit:=132;   #132;
#Minimum log number of muons
  lmulimit:=5   #5
#Minimum log energy (formula by R.Glasstaetter)
  lenlimit:=1
#Minimum angle between shower axis and magnetic field
  ang2Blimit:=0*degree
#Minimum number of antennas
  minants:=6         #6
#Minimum value of quality flag to call an event a detection. 
#Right now quality3 is used which is done by eye!
  quallimit:=0.2
#Lower limit on Zenith angle
  zelowlimit:=0*degree
#Upper limit on Zenith angle
  zeuplimit:=50*degree #50*degree

#Use the rms as error for single events
  doerrors := T
#Default relative gain error in flux for every event
  relgainerror:=0.05
#  relgainerror:=0.2

#Number of bins for the pulse-energy profile
  enprofbins := 8;

########################################
# Selection 2 from thesis
} else if (Parameterset == 2) {
########################################
#Maximum separation between shower core and center of LOPESs antennas.
  rlimit:=132;
#Minimum log number of muons
  lmulimit:=1
#Minimum log energy (formula by R.Glasstaetter)
  lenlimit:=1
#Minimum angle between shower axis and magnetic field
  sfun := function(x){return 3.58-0.6*x}
  ang2Blimit:=acos(1-sfun(log(evtab.LMUO)))
  ang2Blimit[!(ang2Blimit>=0)] := 0.   #the "!" is important to catch NANs
#Minimum number of antennas
  minants:=6
#Minimum value of quality flag to call an event a detection. 
#Right now quality3 is used which is done by eye!
  quallimit:=0.2
#Lower limit on Zenith angle
  zelowlimit:=0*degree
#Upper limit on Zenith angle
  zeuplimit:=50*degree

#Use the rms as error for single events
  doerrors := T
#Default relative gain error in flux for every event
  relgainerror:=0.05

#Number of bins for the pulse-energy profile
  enprofbins := 8;

########################################
# Selection 3 from thesis
} else if (Parameterset == 3) {
########################################
#Maximum separation between shower core and center of LOPESs antennas.
  rlimit:=132;
#Minimum log number of muons
  lmulimit:=5.396
#Minimum log energy (formula by R.Glasstaetter)
  lenlimit:=1
#Minimum angle between shower axis and magnetic field
  ang2Blimit:=0.69455
#Minimum number of antennas
  minants:=6
#Minimum value of quality flag to call an event a detection. 
#Right now quality3 is used which is done by eye!
  quallimit:=0.2
#Lower limit on Zenith angle
  zelowlimit:=0*degree
#Upper limit on Zenith angle
  zeuplimit:=50*degree

#Use the rms as error for single events
  doerrors := T
#Default relative gain error in flux for every event
  relgainerror:=0.05

#Number of bins for the pulse-energy profile
  enprofbins := 8;

########################################
# Selection 4 from thesis
} else if (Parameterset == 4) {
########################################
#Maximum separation between shower core and center of LOPESs antennas.
  rlimit:=132;
#Minimum log number of muons
  lmulimit:=1
#Minimum log energy (formula by R.Glasstaetter)
  lenlimit:=1
#Minimum angle between shower axis and magnetic field
  sfun := function(x){return 4.32-.71*x}
  ang2Blimit:=acos(1-sfun(log(evtab.LMUO)))
  ang2Blimit[!(ang2Blimit>=0)] := 0.
#Minimum number of antennas
  minants:=6
#Minimum value of quality flag to call an event a detection. 
#Right now quality3 is used which is done by eye!
  quallimit:=0.2
#Lower limit on Zenith angle
  zelowlimit:=0*degree
#Upper limit on Zenith angle
  zeuplimit:=50*degree

#Use the rms as error for single events
  doerrors := T
#Default relative gain error in flux for every event
  relgainerror:=0.05

#Number of bins for the pulse-energy profile
  enprofbins := 8;

########################################
} else if (Parameterset == 5) {
########################################
#Maximum separation between shower core and center of LOPESs antennas.
  rlimit:=70;
#Minimum log number of muons
  lmulimit:=5.341    #5.3411
#Minimum log energy (formula by R.Glasstaetter)
  lenlimit:=1
#Minimum angle between shower axis and magnetic field
  ang2Blimit:=0.589       #0.56136
#Minimum number of antennas
  minants:=6
#Minimum value of quality flag to call an event a detection. 
#Right now quality3 is used which is done by eye!
  quallimit:=0.2
#Lower limit on Zenith angle
  zelowlimit:=0*degree
#Upper limit on Zenith angle
  zeuplimit:=50*degree

#Use the rms as error for single events
  doerrors := T
#Default relative gain error in flux for every event
  relgainerror:=0.05

#Number of bins for the pulse-energy profile
  enprofbins := 8;

########################################
} else if (Parameterset == 6) {
########################################
#Maximum separation between shower core and center of LOPESs antennas.
  rlimit:=132;
#Minimum log number of muons
  lmulimit:=5.4723
#Minimum log energy (formula by R.Glasstaetter)
  lenlimit:=1
#Minimum angle between shower axis and magnetic field
  ang2Blimit:=0.632
#Minimum number of antennas
  minants:=6
#Minimum value of quality flag to call an event a detection. 
#Right now quality3 is used which is done by eye!
  quallimit:=0.2
#Lower limit on Zenith angle
  zelowlimit:=0*degree
#Upper limit on Zenith angle
  zeuplimit:=50*degree

#Use the rms as error for single events
  doerrors := F
#Default relative gain error in flux for every event
  relgainerror:=0.05

#Number of bins for the pulse-energy profile
  enprofbins := 8;

########################################
} else if (Parameterset == 7) {
########################################
#Maximum separation between shower core and center of LOPESs antennas.
  rlimit:=132;
#Minimum log number of muons
  lmulimit:=5.4
#Minimum log energy (formula by R.Glasstaetter)
  lenlimit:=1
#Minimum angle between shower axis and magnetic field
  ang2Blimit:=0.0
#Minimum number of antennas
  minants:=6
#Minimum value of quality flag to call an event a detection. 
#Right now quality3 is used which is done by eye!
  quallimit:=0.2
#Lower limit on Zenith angle
  zelowlimit:=0*degree
#Upper limit on Zenith angle
  zeuplimit:=50*degree

#Use the rms as error for single events
  doerrors := F
#Default relative gain error in flux for every event
  relgainerror:=0.05

#Number of bins for the pulse-energy profile
  enprofbins := 8;


########################################
} else if (Parameterset == 8) {
########################################
#Maximum separation between shower core and center of LOPESs antennas.
  rlimit:=1300;
#Minimum log number of muons
  lmulimit:=1
#Minimum log energy (formula by R.Glasstaetter)
  lenlimit:=1
#Minimum angle between shower axis and magnetic field
  ang2Blimit:=0
#Minimum number of antennas
  minants:=4
#Minimum value of quality flag to call an event a detection. 
#Right now quality3 is used which is done by eye!
  quallimit:=0.2
#Lower limit on Zenith angle
  zelowlimit:=0*degree
#Upper limit on Zenith angle
  zeuplimit:=100*degree

#Use the rms as error for single events
  doerrors := F
#Default relative gain error in flux for every event
  relgainerror:=0.05

#Number of bins for the pulse-energy profile
  enprofbins := 8;


} else {
  
};

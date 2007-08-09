#
#This file contains observatory specific calibration information for 
#the lopes tools

#-----------------------------------------------------------------------
#ITS coordinates by Jaap Bregman
#WGS84 coordinates observed 2003-11-12 08:47 MET
#52.908815 N
#06.869115 E
#height 2 m (at ground level)

#include 'measuresdata.g'
#addobservatory('LOFAR-ITS','wgs84','06.869115deg','52.908815deg','2m','provided by Jaap Bregman, observed 2003-11-12 08:47 MET','could be off by 100m')

#addobservatory('LOPES','wgs84','8.43796386deg','49.1006884deg','124.597m','from LOPES GPS-System','precision 5-10m')


#-----------------------------------------------------------------------

#Inititalize the record
obspar:=[=]
obspar.obs:=[=]
obspar.observatories:=["LOFAR-ITS","LOPES","LORUN","NDA"]
for (o in obspar.observatories) {obspar.obs[o]:=[=]};

obspar.init:=function(f,v){
  global obspar; 
  for (o in obspar.observatories) {
    obspar.obs[o][f] := [=]; 
    obspar.obs[o][f]['2000/01/01/12:00:00.0']:=v
  };
};
obspar.initary:=function(f,v,...){
  global obspar; 
  for (o in obspar.observatories) {
    obspar.obs[o][f] := [=]; 
    obspar.obs[o][f]['2000/01/01/12:00:00.0']:=array(v,...,obspar.obs[o].maxants['2000/01/01/12:00:00.0'])
  };
};
obspar.toglobalpar_alt:=function(observatory){
  global obspar; global globalpar;
  #print spaste('Setting Observatory to "',observatory,'" for observatory specific calibration variables.' ) 
  for (f in field_names(obspar.obs[observatory])) globalpar[f]:=obspar.obs[observatory][f]
};
obspar.toglobalpar:=function(observatory,date='2000/01/01/12:00:00.0'){
  d2sec := function(indate) { return  dq.convert(dq.quantity(indate),'s')['value'] }
  global obspar; global globalpar;
  #print spaste('Setting Observatory to "',observatory,'" for observatory specific calibration variables.' )
  datesec := d2sec(date);
  datadate := '2000/01/01/12:00:00.0';
  origdatasec := d2sec('2000/01/01/12:00:00.0');
  for (f in field_names(obspar.obs[observatory])) {
    datasec := origdatasec;
    for (dstring in field_names(obspar.obs[observatory][f]) ){
      dsec := d2sec(dstring);
      if ( (datesec >= dsec) & (dsec >= datasec) ) {
        datadate := dstring;
        datasec := dsec;
      };
    };
    globalpar[f]:=obspar.obs[observatory][f][datadate]
  };
};


#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].CalTablePath
#-----------------------------------------------------------------------
#Type: String
#Doc: path to the CalTabel with the calibration data 
#Ref: 
#-----------------------------------------------------------------------
obspar.obs["LOPES"].CalTablePath['2000/01/01/12:00:00.0']:=
                 concat_dir(globalconst.LOPESCASA,'data/LOPES/LOPES-CalTable');
obspar.obs["LOFAR-ITS"].CalTablePath['2000/01/01/12:00:00.0']:=
                 concat_dir(globalconst.LOPESCASA,'data/ITS/ITS-CalTable');
obspar.obs["NDA"].CalTablePath['2000/01/01/12:00:00.0']:=
                 concat_dir(globalconst.LOPESCASA,'data/NDA/NDA-CalTable');

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].AntIDdefs
#-----------------------------------------------------------------------
#Type: String
#Doc: Antenna IDs in the calibration table
#Ref: 
#-----------------------------------------------------------------------
obspar.init("AntIDdefs",[]);
obspar.obs["LOPES"].AntIDdefs['2000/01/01/12:00:00.0']:= 
  [010101, 010102, 010201, 010202, 020101, 020102, 020201, 020202, 030101, 030102,
   040101, 040102, 040201, 040202, 050101, 050102, 050201, 050202, 060101, 060102,
   070101, 070102, 070201, 070202, 080101, 080102, 080201, 080202, 090101, 090102]

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].maxants
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Maximum number of antennas
#Ref: plotpanel.g, globalpar.relgains
#-----------------------------------------------------------------------
obspar.init("maxants",100);
obspar.obs["LOPES"].maxants['2000/01/01/12:00:00.0']:=10;
obspar.obs["LOPES"].maxants['2005/01/01/12:00:00.0']:=30;
obspar.obs["LOFAR-ITS"].maxants['2000/01/01/12:00:00.0']:=64;

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].badantennas
#-----------------------------------------------------------------------
#Type: array
#Doc:  List of antennas that will be de-selected by default during
#      loading of the data.
#Ref: readfilesH, input_event
#-----------------------------------------------------------------------
obspar.init("badantennas",[]);

## ITS
obspar.obs["LOFAR-ITS"].badantennas['2000/01/01/12:00:00.0']:=[6,30,32] #2004.06 ant32?
obspar.obs["LOFAR-ITS"].badantennas['2005/06/01/00:00:00.0']:=[16]
obspar.obs["LOFAR-ITS"].badantennas['2005/11/01/00:00:00.0']:=[6,16] #2005.11
obspar.obs["LOFAR-ITS"].badantennas['2005/11/30/00:00:00.0']:=[6,16,49,50] #jupX9

## LOPES (dead time ~1.5 seconds)

obspar.obs["LOPES"].badantennas['2005/11/01/00:00:00.0']:=[24,27:30] #LOPES30
obspar.obs["LOPES"].badantennas['2005/12/05/06:46:13.0']:=[2,7,8,12,14,18,20,22,24,26:30] #81?&83?
obspar.obs["LOPES"].badantennas['2005/12/14/01:54:48.0']:=[2,7,12,14,18,24,27:30] #86
obspar.obs["LOPES"].badantennas['2005/12/30/02:09:57.0']:=[2,7,8,12,14,18,24,27:30] #90
obspar.obs["LOPES"].badantennas['2006/01/18/13:46:06.0']:=[8,12,18,20,22,24,26:30] #95
obspar.obs["LOPES"].badantennas['2006/01/24/13:23:57.0']:=[8,12,18,20,22,24,26:30] #97fails?

obspar.obs["LOPES"].badantennas['2006/02/07/09:40:20.0']:=[2,7,24,27:30] #106
obspar.obs["LOPES"].badantennas['2006/02/07/09:40:21.0']:=[24,27:30] #107
obspar.obs["LOPES"].badantennas['2006/02/11/19:01:17.0']:=[2,16:18,24:30] #108fails #110
obspar.obs["LOPES"].badantennas['2006/02/14/02:09:18.0']:=[24,27:30] #111..114
obspar.obs["LOPES"].badantennas['2006/02/23/04:02:50.0']:=[1:10,12,14,18,20:22,24,26:30] #115fails?
obspar.obs["LOPES"].badantennas['2006/02/23/04:02:51.0']:=[24,27:30] #116..121&1
obspar.obs["LOPES"].badantennas['2006/03/14/08:51:14.0']:=[2,7,8,12,14,18,20,22,24,26:30] #122fails
obspar.obs["LOPES"].badantennas['2006/03/14/08:51:15.0']:=[24,27:30] #123

obspar.obs["LOPES"].badantennas['2006/03/14/13:30:27.0']:=[2,24,27:30] #2&3&124
obspar.obs["LOPES"].badantennas['2006/04/05/05:39:13.0']:=[13,24,27:30] #4
obspar.obs["LOPES"].badantennas['2006/04/05/20:42:56.0']:=[13,24,27:30] #5
obspar.obs["LOPES"].badantennas['2006/04/04/12:21:02.0']:=[13,24,27:30] #last event with Ant.13
obspar.obs["LOPES"].badantennas['2006/04/21/16:30:02.0']:=[24,27,28,29,30] #last event without Ant.13

obspar.obs["LOPES"].badantennas['2006/05/01/10:31:40.0']:=[24,27:30] #6
obspar.obs["LOPES"].badantennas['2006/05/03/10:10:09.0']:=[2,11:20,24,27:30] # 9902 power problem #7&8
obspar.obs["LOPES"].badantennas['2006/06/14/11:03:15.0']:=[2,11:20,24,27:30] # 9902 power problem #9
obspar.obs["LOPES"].badantennas['2006/06/19/14:27:15.0']:=[2,10,16:18,24,27:30] # 9902 power problem #10&11
obspar.obs["LOPES"].badantennas['2006/06/29/16:32:06.0']:=[1,2,10,11,13:19,22:30] # 9902 power problem #12&13fail
obspar.obs["LOPES"].badantennas['2006/06/29/23:15:04.0']:=[1,2,10,11,15:20,22:30] # 9902 power problem #14fail
obspar.obs["LOPES"].badantennas['2006/07/18/06:44:07.0']:=[2,11:13,15,17,21,22,24,27:30] # pol. prep. #15
obspar.obs["LOPES"].badantennas['2006/07/28/14:27:54.0']:=[14,17:20] #,21:30] # lopes20

obspar.obs["LOPES"].badantennas['2006/08/02/20:30:46.0']:=[1,2,10:30] #16fails?
obspar.obs["LOPES"].badantennas['2006/08/02/20:30:47.0']:=[14,17:20] #,21:30] # lopes20 #17...

obspar.obs["LOPES"].badantennas['2006/08/10/14:02:39.0']:=[11:30] #2?,14?] # polarization (9902->9906)

obspar.obs["LOPES"].badantennas['2006/09/12/07:35:26.0']:=[11:30] #,14,15,16,29,30] # newconfig ???
obspar.obs["LOPES"].badantennas['2006/09/22/17:58:45.0']:=[7,11:30] #,14,15,16,29,30] #24
obspar.obs["LOPES"].badantennas['2006/09/22/17:58:46.0']:=[11:30] #,14,15,16,29,30] # newconfig ???
obspar.obs["LOPES"].badantennas['2006/10/06/13:14:12.0']:=[6,11:30] #,14,15,16,29,30] #27
obspar.obs["LOPES"].badantennas['2006/10/06/13:14:13.0']:=[11:30] #,14,15,16,29,30] # newconfig ???

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].centeroffset
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Offset of phase center relative to the origin of the antenna
#      array based coordinate system.<br>
#      Important notes:
#      <ul>
#        <li>X and Y coordinates are flipped, i.e. Y is first!
#        <li>As center offset we <b>define</b> the position of the 
#            Kascade central detector w.r.t. to the first antenna of the
#            LOPES antenna array.
#      </ul>
#-----------------------------------------------------------------------
obspar.init("centeroffset",[0,0,0]);
#angle:=-15.; dist:=13.
#offset:=mx.rotate([-97.50+0.5*dist,dist],angle,T)
#newoffset:=mx.rotate([-5*dist,5*dist],angle,T)
#offset:=-[offset[2],offset[1],0]

obspar.obs["LOPES"].centeroffset['2000/01/01/12:00:00.0']:=[-36.1095688,84.5346026,0];
obspar.obs["LOPES"].centeroffset['2004/10/05/01:00:00.0']:=[-79.61,45.96,0]

#obspar.obs["LOPES"].centeroffset['2000/01/01/12:00:00.0']:=[-22.5365688,34.3396029,124.597];

#-----------------------------------------------------------------------
#Name: obspar.obs["LOFAR-ITS"].centeroffset
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Offset of phase center relative to origin of antenna coordindate system. 
#-----------------------------------------------------------------------

#obspar.obs["LOFAR-ITS"].centeroffset:=[-22.1434,7.5431,0];
obspar.obs["LOFAR-ITS"].centeroffset['2000/01/01/12:00:00.0']:=[-18.7287, 14.0166, 0]

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].relgains
#-----------------------------------------------------------------------
#Type: String
#Doc:  A vector which contains relative gain corrections for each antenna.
#Ref: calcrelgain, readTIM40relgains
#-----------------------------------------------------------------------
obspar.initary("relgains",1)
obspar.obs["LOPES"].relgains['2000/01/01/12:00:00.0'][1:8] := 1/[1.18881671 ,1.15406021, 1.38183639, 1.01188692, 1.22998435, 1.23940966, 1.19296059, 1.647263]

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].delays
#-----------------------------------------------------------------------
#Type: String
#Doc:  A vector which contains relative delays for each antenna.
#      In sample times
#Ref: calcrelgain, readTIM40relgains
#-----------------------------------------------------------------------
obspar.initary("delays",0)

#after averaging over 6 solar burst events from 031028 (Antenna 1-8)
#after averaging over 4 solar burst events from 040330 (Antenna 9,10):
obspar.obs["LOPES"].delays['2000/01/01/12:00:00.0'][1:10] := [0, 0.15+0.041, -0.18+0.071, -0.42+0.1886, -2.03+0.08, -2.05+0.04668, 1.10+0.0398, 0.73+0.0137, 1.02, -0.99];

##averaged over two solar burst events from 2005.07.14
#obspar.obs["LOPES"].delays['2005/01/01/12:00:00.0'][1:30] := 
#[0, 0.21, -33.45, -33.17, -36.63, -36.71, -33.47, -33.44, -34.25, -34.44,
#-99.11, -97.72, -97.3, -98.68, -98.02, -98.47, -65.56, -65.59, -98.83, -99.17,
#-79.62, -79.59, -80.36, -80.11, -45.88, -46.51, -47.16, -47.05, -46.73, -48.45]

#adjusted values for event 2005.12.14.01:54:48.344 (1134525288)
obspar.obs["LOPES"].delays['2005/01/01/12:00:00.0'][1:30] := 
[0, 0.21, -33.45, -33.17, -36.63, -36.71, -33.47, -33.44, -34.25, -34.44,
-99.11, -97.72, -97.3, -98.68, -98.02, -98.47, -65.56, -65.59, -98.83, -99.17,
-78.62, -78.59, -79.36, -79.11, -44.88, -45.51, -46.16, -46.05, -45.73, -47.45]


 #manipulated delays for the january event
 #obspar.obs["LOPES"].delays[1:8] := [0, 0.15-2, -0.2, -0.4,   -2, -2, 1.15, 0.75-2];


#ITS delays from Transmitter at 15.16MHz using Falcke03
#obspar.obs["LOFAR-ITS"].delays[1:60] := -[0,-0.34,-0.17,-0.295,-0.003,-0.894,-0.212,-0.371,0.742,0.898,-0.113,-0.388,-0.086,0.268,0.459,-0.366,-0.242,-0.458,-0.355,-0.263,-0.311,-0.108,0.537,-1.12,0.271,0.042,0.446,0.091,0.027,-1.029,-0.42,2.41,-0.071,-0.442,-0.569,-0.677,-0.573,0.081,-0.661,0.394,0.3,-0.308,-0.635,-0.356,0.096,-0.306,0.372,0.422,0.582,-0.979,-0.564,-0.794,-0.067,-0.656,0.298,-1.197,-0.069,-0.029,0.246,-0.17]
#ITS delays from Transmitter at 15.16MHz and 12.01MHz using Falcke03
#obspar.obs["LOFAR-ITS"].delays[1:60] :=[0,0.08,-0.055,0.1225,-0.0435,0.267,-0.174,0.2805,-0.226,-0.504,-0.1135,0.079,0.088,-0.359,-0.1095,0.068,-0.019,0.289,0.2525,0.1865,0.3105,0.269,-0.1235,0.72,-0.2555,0.014,-0.128,0.0445,0.0865,0.6195,0.325,-2.465,0.2405,0.621,0.6745,0.6985,0.3215,0.0895,0.2405,0.038,-0.03,0.194,0.4275,0.093,-0.073,0.043,-0.216,-0.151,-0.176,0.4445,0.142,0.077,-0.0215,0.318,-0.059,0.4585,-0.0805,-0.2205,-0.253,-0.3] 


#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].tvphases
#-----------------------------------------------------------------------
#Type: String
#Doc:  A vector which contains relative phases for each antenna
#      for the picture carrier (ca. 62.2MHz) and the two sound carriers 
#      (ca. 67.7MHz and 67.94MHz) of the TV-station
#-----------------------------------------------------------------------
#The following values are from 031028
# The corrdelays were fixed at:
# obspar.obs["LOPES"].delays[1:8] := [0, 0.15, -0.18, -0.42, -2.03, -2.05, 1.1, 0.73];
# using the sun as calibrator source. So, corrdelays were not set to zero
# in order to get the tvphasedelays below!
# The last two delays (#9&10) are fantasy.
obspar.init("tvday",'010101');
obspar.obs["LOPES"].tvday['2000/01/01/12:00:00.0'] := '031028';
obspar.initary("tvphases",0,3);

obspar.obs["LOPES"].tvphases['2000/01/01/12:00:00.0'][1,]:=[0., 118.,  4.8, 99.6, 172.4,  -66., -113.6,  49., -138.1,  5.1];
obspar.obs["LOPES"].tvphases['2000/01/01/12:00:00.0'][2,]:=[0., 112.,-143., -73.,  -14., -136.,  -151., -25.,  111.4, -1.2];
obspar.obs["LOPES"].tvphases['2000/01/01/12:00:00.0'][3,]:=[0., 122., -97., -42.,  18.7, -113.,  -140., -14.,  140.6, 29.9];

obspar.obs["LOPES"].tvphases['2004/10/07/12:00:00.000'] := array(0.,3,30)
obspar.obs["LOPES"].tvphases['2004/10/07/12:00:00.000'][1,]:= [0., 180.0, -131.5, -0.6, -141.0, -41.9, 14.8, 162.5,   154.2,  -17.1, -50.9, 96.3, 94.7,  -136.9, -103.5, 41.7, 39.6, -158.6, 50.5, -132.9, -85.8, 52.2, -127.8, 57.3, 5.3, -160.4, -160.5, -30.7, 50.9,  -150.0];
obspar.obs["LOPES"].tvphases['2004/10/07/12:00:00.000'][2,] := [0., -42.2, 19.0,  141.5,   -135.2, -55.7, -4.8, -107.1, -92.5, 70.1,  -94.6, -1.1, -1.8, -124.8, -73.7, 53.9, 50.3, -37.4, 80.2, -2.3, -75.7, 80.8, 18.1,  160.1, 93.1, 50.9,  107.1,  -78.3, 165.6,  35.7];
obspar.obs["LOPES"].tvphases['2004/10/07/12:00:00.000'][3,] := [0., -36.0, 21.3,  155.2,   -83.7, -19.6, 34.3,  -85.7, -74.1, 82.3,  -50.6, 50.1,  46.1,  -64.5, -10.8, 115.5, 122.4, 35.5,  105.7, 35.8,  -40.9,  111.4, 70.9,  -151.0, 156.2, 95.6, 160.0, -43.5, 180.0, 39.8];

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].DefPolarization
#-----------------------------------------------------------------------
#Type: string
#Doc:  The default polarization of the  antennas. Used if not set in
#      header. Suitable values: 'WE' and 'NS'
#Ref: TIM40convFFT2AntennaGain
#-----------------------------------------------------------------------
obspar.init('DefPolarization','WE')
obspar.obs["LOPES"].DefPolarization['2000/01/01/12:00:00.0'] := 'WE'
obspar.obs["LOFAR-ITS"].DefPolarization['2000/01/01/12:00:00.0'] := 'WE' #????? 


#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].AntennaGainRec
#-----------------------------------------------------------------------
#Type: record
#Doc:  Record with the directivity values of the antennas
#Ref: TIM40convFFT2AntennaGain
#-----------------------------------------------------------------------
tempagain:=[=];
tempagain.azinp:=[1e999,1];tempagain.elinp:=[1e999,1];tempagain.freqinp:=[1e999,1];
tempagain.dir:=array(1,2,1,1)
obspar.init('AntennaGainRec',tempagain)
obspar.obs["LOPES"].AntennaGainRec['2000/01/01/12:00:00.0'] := read_value(add_dir('lopesbeam.sos'));

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].has_gainvalues
#-----------------------------------------------------------------------
#Type: Boolean
#Doc:  There are decent (antenna)gain values for this observatory
#
#Ref: TIM40convFFT2AntennaGain
#-----------------------------------------------------------------------
obspar.init('has_gainvalues',F);
obspar.obs["LOPES"].has_gainvalues['2000/01/01/12:00:00.0'] := T

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].antennagain
#-----------------------------------------------------------------------
#Type: real
#Doc:  The gain ("directivity") of the antenna compared to an isotropic 
#      radiator. (Like dBi, but without the "dB".)
#Ref: TIM40revconvVoltage2FieldStrength
#-----------------------------------------------------------------------
obspar.init('antennagain',1);
obspar.obs["LOPES"].antennagain['2000/01/01/12:00:00.0'] := 4.46
obspar.obs["LOFAR-ITS"].antennagain['2000/01/01/12:00:00.0'] := 4.46

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].GainFudgeFactor
#-----------------------------------------------------------------------
#Type: real
#Doc:  A fudge factor for the gain of our electronics. 
#Ref: TIM40revconvVoltage2FieldStrength
#-----------------------------------------------------------------------
obspar.init('GainFudgeFactor',1);

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].Bandpass
#-----------------------------------------------------------------------
#Type: real
#Doc: The start and stop frequency of the effective bandpass in Hz 
#Ref: autoflag,loadcr
#-----------------------------------------------------------------------
obspar.init('Bandpass',1);
obspar.obs["LOPES"].Bandpass['2000/01/01/12:00:00.0']:=[43e6,76e6];
obspar.obs["LOPES"].Bandpass['2004/10/07/12:00:00.0']:=[43e6,74e6];

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].FieldStrengthMultiplier
#-----------------------------------------------------------------------
#Type: real
#Doc:  A factor, used together with the Antenne Gain, to convert time 
#      series data into units of Volts/m/MHz.
#      It consists of a forefactor that includes all constants, the center 
#      Frequency, input impedance etc. and the effective bandpass in MHz. 
#Ref: TIM40revconvVoltage2FieldStrength,rec.setphase
#-----------------------------------------------------------------------
obspar.init('FieldStrengthMultiplier',1);
obspar.obs["LOPES"].FieldStrengthMultiplier['2000/01/01/12:00:00.0']:=1.947/
  ((obspar.obs["LOPES"].Bandpass['2000/01/01/12:00:00.0'][2]-
    obspar.obs["LOPES"].Bandpass['2000/01/01/12:00:00.0'][1])/1e6) #(76-43) #=33 #ANIGL
obspar.obs["LOPES"].FieldStrengthMultiplier['2004/10/07/12:00:00.0']:=1.947/
  ((obspar.obs["LOPES"].Bandpass['2004/10/07/12:00:00.0'][2]-
    obspar.obs["LOPES"].Bandpass['2004/10/07/12:00:00.0'][1])/1e6) #(74-43) #=31 #ANIGL

#-----------------------------------------------------------------------
#Name: obspar.obs["LOPES"].CRtriggeroffset
#-----------------------------------------------------------------------
#Type: real
#Doc:  
#Ref: TIM40revconvVoltage2FieldStrength,rec.setphase
#-----------------------------------------------------------------------
obspar.init('CRtriggeroffset',0.);
obspar.obs["LOPES"].CRtriggeroffset['2000/01/01/12:00:00.0']:=0.
obspar.obs["LOPES"].CRtriggeroffset['2004/07/23/00:00:00.0']:=-0.67e-6
obspar.obs["LOPES"].CRtriggeroffset['2005/08/18/09:00:00.0']:=0.054e-6



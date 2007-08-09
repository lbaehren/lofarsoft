# linclude('CalTableHW_Tools.g')
# print globalpar.CalTable_client["LOPES"]->openWrite();
# print globalpar.CalTable_client["LOPES"]->PrintSummary();
#
#


AddAllAntennas := function(){
  for (i in seq(data.len)) {
    rec := [=];
    rec.AntID := data.head('AntID',i);
    rec.AntName := data.head('ClusterPos',i);
    print globalpar.CalTable_client->addAntenna(rec)
  }
};

AddHardwareFields := function(obs='LOPES'){
  rec := [=];
  rec.FieldName := 'HWSetup';
  rec.FieldDesc := 'Changes to the hardware, that describe why a field in this table changed.'
  rec.DataType := 'String';
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  rec := [=];
  rec.FieldName := 'Position';
  rec.FieldDesc := 'Relative antenna Position <Northing, Easting, Height> (in m)  (Remark: This ordering is <Y, X, Z> )';
  rec.DataType := 'Array<Double>';
  rec.FriendField := 'HWSetup'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  rec := [=];
  rec.FieldName := 'FrequencyBand';
  rec.FieldDesc := 'Effective frequency band of this antenna <StartFreq, StopFreq> (in Hz)';
  rec.DataType := 'Array<Double>';
  rec.FriendField := 'HWSetup'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  rec := [=];
  rec.FieldName := 'Polarization';
  rec.FieldDesc := 'Polarization direction of this antenna/channel. ("EW" for East-West (standard) or "NS" nor North-South)';
  rec.DataType := 'String';
  rec.FriendField := 'HWSetup'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  return T;
}

AddHardwareDataLOPES10 := function(startdate=978350400,obs='LOPES'){ #'2001/01/01/12:00:00.000'
  if (data.len != 10) { print "Please load a LOPES10 event! (With 10 antennas)"; return F;}
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'HWSetup'
  rec.data := 'Initial LOPES10 setup';
  for (i in [1:10]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'Position'
  antpos := CalcAntPos(data,index=-1,refant=1,phasecenter=T);
  for (i in [1:10]){
    if (antpos[i].antenna != i) { print '(antpos[i].antenna != i)'; return F;};
    rec.data := antpos[i].position;
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'FrequencyBand'
  rec.data := [43e6,76e6]
  for (i in [1:10]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'Polarization';
  rec.data := 'EW';
  for (i in [1:10]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  return T;
}

AddHardwareDataLOPES30 := function(startdate=1104580800,obs='LOPES'){ #'2005/01/01/12:00:00.0'
  if (data.len != 30) { print "Please load a LOPES30 event! (With 30 antennas)"; return F;}
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'HWSetup'
  rec.data := 'First LOPES30 setup';
  for (i in [1:30]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'Position'
  antpos := CalcAntPos(data,index=-1,refant=1,phasecenter=T);
  for (i in [1:30]){
    if (antpos[i].antenna != i) { print '(antpos[i].antenna != i)'; return F;};
    rec.data := antpos[i].position;
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'FrequencyBand'
  rec.data := [43e6,74e6]
  for (i in [1:30]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'Polarization';
  rec.data := 'EW';
  for (i in [1:30]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  return T;
}

AddPhaseRefFields := function(obs='LOPES'){
  rec := [=];
  rec.FieldName := 'Delay';
  rec.FieldDesc := 'Relative delay of the antenna (in seconds).'
  rec.DataType := 'Double';
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  rec := [=];
  rec.FieldName := 'PhaseRefPhases';
  rec.FieldDesc := 'Phase calibration reference phases. E.g. for the calibration on the TV transmitter (in degrees)';
  rec.DataType := 'Array<Double>';
  rec.FriendField := 'Delay'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  rec := [=];
  rec.FieldName := 'PhaseRefFreqs';
  rec.FieldDesc := 'Freqency ranges for the phase calibration [ <StartFreq, StopFreq> , <Band Number>] (in Hz)';
  rec.DataType := 'Array<Double>';
  rec.FriendField := 'Delay'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  rec := [=];
  rec.FieldName := 'SampleJumps';
  rec.FieldDesc := 'Jumps by how many samples are to be tried in the phase calibration routine. (Only needed for the reference - currently the first (ID: 10101) - antenna.)';
  rec.DataType := 'Array<Double>';
  rec.FriendField := 'Delay'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  return T;
}

AddPhaseRefDataLOPES10 := function(startdate=978350400,obs='LOPES'){ #'2001/01/01/12:00:00.000'
  if (data.len != 10) { print "Please load a LOPES10 event!"; return F;}
  LOPES10delays := [0, 0.15+0.041, -0.18+0.071, -0.42+0.1886, -2.03+0.08, -2.05+0.04668, 1.10+0.0398, 0.73+0.0137, 1.02, -0.99];
  LOPES10delays := LOPES10delays /80e6;
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'Delay'
  for (i in [1:10]){
    rec.data := LOPES10delays[i];
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  LOPES10RefPhases := array(0.,3,10)
  LOPES10RefPhases[,2] := [  59.9,  48.8,  58.6]
  LOPES10RefPhases[,3] := [  25.4,-120.5, -74.4]
  LOPES10RefPhases[,4] := [ 164.1,  -2.8,  28.5]
  LOPES10RefPhases[,5] := [  -7.6,-146.3,-111.4]
  LOPES10RefPhases[,6] := [ 130.6, 109.8, 135.0]
  LOPES10RefPhases[,7] := [ -84.2,-150.8,-141.1]
  LOPES10RefPhases[,8] := [-175.7,  90.4, 100.5]
  LOPES10RefPhases[,9] := [ -66.3, 157.7,-174.2]
  LOPES10RefPhases[,10]:= [ -85.9, -68.4, -36.3]
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'PhaseRefPhases'
  for (i in [1:10]){
    rec.data := LOPES10RefPhases[,i];
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'PhaseRefFreqs';
  rec.data := array(0.,3,2);
  rec.data[1,] := [62165865.6, 62223239.5];
  rec.data[2,] := [67670000,   67712000];
  rec.data[3,] := [67915000,   67947000];
  for (i in [1:10]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }  
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'SampleJumps';
  rec.data := [-2.,2.,-16.,16.,-14.,14.,-18.,18.,-32.,32.]
  for (i in [1:10]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);  
  }
  return T;
}

AddPhaseRefDataLOPES30 := function(startdate=1104580800,obs='LOPES'){ #'2005/01/01/12:00:00.0'
  if (data.len != 30) { print "Please load a LOPES30 event!"; return F;}
  LOPES30delays := [0, 0.21, -33.45, -33.17, -36.63, -36.71, -33.47, -33.44, -34.25, -34.44,-99.11, -97.72, -97.3, -98.68, -98.02, -98.47, -65.56, -65.59, -98.83, -99.17,-78.62, -78.59, -79.36, -79.11, -44.88, -45.51, -46.16, -46.05, -45.73, -47.45]
  LOPES30delays := LOPES30delays /80e6;
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'Delay'
  for (i in [1:30]){
    rec.data := LOPES30delays[i];
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  LOPES30RefPhases := array(0.,3,30)
  LOPES30RefPhases[,2]  := [  92.3,-138.2,-132.4]
  LOPES30RefPhases[,3]  := [-159.6, 101.4, 140.9]
  LOPES30RefPhases[,4]  := [ -70.2,-179.3,-129.8]
  LOPES30RefPhases[,5]  := [  25.2,-135.9, -45.0]
  LOPES30RefPhases[,6]  := [ 162.9, -14.2,  62.5]
  LOPES30RefPhases[,7]  := [  29.2, 120.6,-163.8]
  LOPES30RefPhases[,8]  := [ 169.5,  12.2,  70.3]
  LOPES30RefPhases[,9]  := [  20.4, -92.7, -37.4]
  LOPES30RefPhases[,10] := [-107.7, 108.8, 157.8]
  LOPES30RefPhases[,11] := [ -41.5,-165.2, -14.1]
  LOPES30RefPhases[,12] := [  92.1, -91.5,  65.5]
  LOPES30RefPhases[,13] := [ -18.6, 152.8, -53.3]
  LOPES30RefPhases[,14] := [-106.7,-177.2, -10.8]
  LOPES30RefPhases[,15] := [ -19.8, -71.1,  98.6]
  LOPES30RefPhases[,16] := [-112.1, 161.3, -31.7]
  LOPES30RefPhases[,17] := [  33.7,-145.8,  -3.5]
  LOPES30RefPhases[,18] := [-158.9, 134.8, -82.5]
  LOPES30RefPhases[,19] := [  -7.8, -80.2,  50.3]
  LOPES30RefPhases[,20] := [-137.3, -90.8,  53.5]
  LOPES30RefPhases[,21] := [ -42.4,  95.1,-146.9]
  LOPES30RefPhases[,22] := [  70.3,-128.1, -13.6]
  LOPES30RefPhases[,23] := [ 110.2,  26.9, 164.0]
  LOPES30RefPhases[,24] := [-153.7,  90.0,-137.3]
  LOPES30RefPhases[,25] := [ -69.3,  34.4, 145.2]
  LOPES30RefPhases[,26] := [ -63.8, 171.2, -95.7]
  LOPES30RefPhases[,27] := [ 102.5,  53.5, 156.9]
  LOPES30RefPhases[,28] := [-121.7,-129.7, -46.0]
  LOPES30RefPhases[,29] := [-117.4,  42.5, 107.4]
  LOPES30RefPhases[,30] := [ 145.9,  56.0, 110.2]
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'PhaseRefPhases'
  for (i in [1:30]){
    rec.data := LOPES30RefPhases[,i];
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'PhaseRefFreqs';
  rec.data := array(0.,3,2);
  rec.data[1,] := [62165865.6, 62223239.5];
  rec.data[2,] := [67670000,   67712000];
  rec.data[3,] := [67915000,   67947000];
  for (i in [1:30]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }  
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'SampleJumps';
  rec.data := [-2.,2.,-1.,1.,-3.,3.];
  for (i in [1:30]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);  
  }
  return T;
}




AddAntennaGainFields := function(obs='LOPES'){
  rec := [=];
  rec.FieldName := 'AntennaGainFaktor';
  rec.FieldDesc := 'Array with the Antenna Gain Faktors i.e. the value: 1/sqrt(Gain) (with Gain==Directivity)  with axes: <Frequency, Azimuth, Elevation>'
  rec.DataType := 'Array<Double>';
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  rec := [=];
  rec.FieldName := 'AntennaGainFaktFreq';
  rec.FieldDesc := 'Frequency axis for AntennaGainFaktor (in Hz)';
  rec.DataType := 'Array<Double>';
  rec.FriendField := 'AntennaGainFaktor'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  rec := [=];
  rec.FieldName := 'AntennaGainFaktAz';
  rec.FieldDesc := 'Azimuth axis for AntennaGainFaktor 0==North, 90==East (in degrees)';
  rec.DataType := 'Array<Double>';
  rec.FriendField := 'AntennaGainFaktor'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  rec := [=];
  rec.FieldName := 'AntennaGainFaktEl';
  rec.FieldDesc := 'Elevation axis for AntennaGainFaktor (in degrees)';
  rec.DataType := 'Array<Double>';
  rec.FriendField := 'AntennaGainFaktor'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  return T;
}


AddAntennaGainData := function(startdate=978350400,obs='LOPES'){ #'2001/01/01/12:00:00.000'
  if (data.len != 30) { print "Please load a LOPES30 event!"; return F;}
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'AntennaGainFaktor';
  rec.data := 1/sqrt(globalpar.AntennaGainRec.dir[,,([1:19])]) 
  for (i in [1:30]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }  
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'AntennaGainFaktFreq';
  rec.data := globalpar.AntennaGainRec.freqs*1e6;
  for (i in [1:30]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }  
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'AntennaGainFaktAz';
  rec.data := globalpar.AntennaGainRec.azs
  for (i in [1:30]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }  
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'AntennaGainFaktEl';
  rec.data := globalpar.AntennaGainRec.els[1:19]
  for (i in [1:30]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }  
  return T;
};

AddAntennaPolarizationGainFields := function(obs='LOPES'){
  rec := [=];
  rec.FieldName := 'AntennaAziGain';
  rec.FieldDesc := 'The Antenna Gain Faktors for the azimuth polarization component. The value: 1/sqrt(Gain) (with Gain==Directivity)  with axes: <Frequency, Azimuth, Elevation>'
  rec.DataType := 'Array<Double>';
  rec.FriendField := 'AntennaGainFaktor'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  rec := [=];
  rec.FieldName := 'AntennaZeniGain';
  rec.FieldDesc := 'The Antenna Gain Faktors for the zenith polarization component. The value: 1/sqrt(Gain) (with Gain==Directivity)  with axes: <Frequency, Azimuth, Elevation>'
  rec.DataType := 'Array<Double>';
  rec.FriendField := 'AntennaGainFaktor'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  return T;
}

AddAntennaPolarizationGainData := function(startdate=978350400,obs='LOPES'){ #'2001/01/01/12:00:00.000'
  if (data.len != 30) { print "Please load a LOPES30 event!"; return F;}
  gainvals := read_value('lopespat_pedas2.sos');
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'AntennaAziGain';
  rec.data := 1/sqrt(gainvals.gain_azi) 
  for (i in [1:30]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }  
  rec := [=];
  rec.startdate := startdate;
  rec.field := 'AntennaZeniGain';
  rec.data := 1/sqrt(gainvals.gain_zeni) 
  for (i in [1:30]){
    rec.ant := data.head('AntID',i);
    print 'Adding data to ',rec.field,'AntID',rec.ant,':',globalpar.CalTable_client[obs]->addData(rec);
  }
  return T
};  










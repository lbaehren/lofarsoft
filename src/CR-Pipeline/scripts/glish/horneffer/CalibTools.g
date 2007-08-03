# linclude('CalibTools.g')
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

AddPhaseCalFields := function(){
  rec := [=];
  rec.FieldName := 'PhaseCal';
  rec.FieldDesc := 'Relative phase calibration values. Multiply to the frequency domian data to correct for phase differences of the filters etc.';
  rec.DataType := 'Array<DComplex>';
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client->addField(rec)
  rec := [=];
  rec.FieldName := 'PhaseCalFreq';
  rec.FieldDesc := 'Frequency axis for PhaseCal (in Hz)';
  rec.DataType := 'Array<Double>';
  rec.FriendField := 'PhaseCal'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client->addField(rec)
}

AddDummyPhaseCalData := function(startdate=978317568,obs='LOPES') { #2001/01/01.12:00:00.0
  if (data.len != 30) { print "Please load a LOPES30 event!"; return F;};
  for (i in [1:30]){
    rec := [=];
    rec.ant := data.head('AntID',i);
    rec.startdate := startdate;
    rec.field := 'PhaseCal';
    rec.data := [1.-0.i,1.-0.i];
    print 'Adding data to ',rec.field,':',globalpar.CalTable_client[obs]->addData(rec);
    rec := [=];
    rec.ant := data.head('AntID',i);
    rec.startdate := startdate;
    rec.field := 'PhaseCalFreq';
    rec.data := [0.,1.];
    print 'Adding data to ',rec.field,':',globalpar.CalTable_client[obs]->addData(rec);
  };
};

AddGainCalFields := function(obs='LOPES'){
  rec := [=];
  rec.FieldName := 'ElGainCal';
  rec.FieldDesc := 'Gain calibration values. Multiply to the frequency domian data to correct for the electronic gain. (I.e. the factor between FFT and CalFFT, so square this value befor applying to power data.)';
  rec.DataType := 'Array<Double>';
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
  rec := [=];
  rec.FieldName := 'ElGainCalFreq';
  rec.FieldDesc := 'Frequency axis for ElGainCal (in Hz)';
  rec.DataType := 'Array<Double>';
  rec.FriendField := 'ElGainCal'
  print 'Generating',rec.FieldName,':',globalpar.CalTable_client[obs]->addField(rec)
}

AddGainCalData := function(startdate=978317568,obs='LOPES',dofreq=T) {
  for (i in [1:(data.len)]){
    freqar := [40,41,42,42.99,43.01,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,75.99,76.01,77,78,79,80]*1e6;
    yarr := abs(data.get('CalFFT',i))/abs(data.get('FFT',i))
    interp.initialize(data.get('Frequency',i),yarr,'linear');
    gvals := interp.interpolate(freqar);
    rec := [=];
    rec.ant := data.head('AntID',i);
    rec.startdate := startdate;
    rec.field := 'ElGainCal';
    rec.data := gvals
    print 'Adding data to ',rec.field,':',globalpar.CalTable_client[obs]->addData(rec);
    if (dofreq) {
      rec := [=];
      rec.ant := data.head('AntID',i);
      rec.startdate := startdate;
      rec.field := 'ElGainCalFreq';
      rec.data := freqar;
      print 'Adding data to ',rec.field,':',globalpar.CalTable_client[obs]->addData(rec);
    }
  };
}

AddDummyGainCalData := function(startdate,AntID,obs='LOPES') {
  rec := [=];
  rec.ant := AntID;
  rec.startdate := startdate;
  rec.field := 'ElGainCal';
  rec.data := [0.0001,0.0001]
  print 'Adding data to ',rec.field,':',globalpar.CalTable_client[obs]->addData(rec);
  rec := [=];
  rec.ant := AntID;
  rec.startdate := startdate;
  rec.field := 'ElGainCalFreq';
  rec.data := [0.,1.];
  print 'Adding data to ',rec.field,':',globalpar.CalTable_client[obs]->addData(rec);
}

read_Hakenjos_Gain := function(file){  
  if (!fexist(file)){
    print 'Can\'t find file:',file,' Exiting!';
    return F;
  };
  tmp := shell(paste('wc',file));
  nopts := as_integer(split(tmp)[1]);
  erg := [=];
  erg.freq := array(0.,nopts);
  erg.gain := array(0.,nopts);
  erg.err := array(0.,nopts);
  i:=1;
  fd := open(paste('<',file));
  while (line := read(fd)){
    tmp := as_float(split(line));
    erg.freq[i] := tmp[1];
    erg.gain[i] := tmp[2];
    erg.err[i] := tmp[3];
    i+:=1;
    if(i>nopts) break;
  }
  return erg;
}

plot_Hakenjos := function(index=[1:30],factor=F){
    freqar1 := [40,41,42,42.99,43.01,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,75.99,76.01,77,78,79,80]*1e6;
    flag1 := [1:4,39:43]
    freqar2 := [40,41,42,42.99,43.01,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,73.99,74.01,75,76,77,78,79,80]*1e6;
    flag2 := [1:4,37:43]
  for (i in index){
    if (i<=10) { 
      freqar := freqar1; flag := flag1;
    } else {
      freqar := freqar2; flag := flag2; 
    }
    file := spaste('Amp_2006_03_14/Ant',as_string(i),'_Amp');
    gains := read_Hakenjos_Gain(file);
    gvals := 1/sqrt(gains.gain);
    minval := min(gvals[gvals>0])/1000.;
    gvals[!(gvals>0&gvals<=1)] := minval #negation to catch also NANs
    interpol := interpolate1d()
    interpol.initialize(gains.freq,gvals,'linear');
    intgvals := interpol.interpolate(freqar);
    interpol.done()

    intgvals[flag] := minval;
    
    if (factor) {
      pg.plotxy1(freqar,intgvals);
    } else {
      pg.plotxy1(gains.freq,sqrt(gains.gain));
    }
  };
};

avg_Hakenjos := function(index=[1:30]){
  erg.gain := array(0.,41)
  erg.freq := [40:80]*1e6;
  for (i in index){
    file := spaste('Amp_2006_03_14/Ant',as_string(i),'_Amp');
    gains := read_Hakenjos_Gain(file);
    interpol := interpolate1d()
    interpol.initialize(gains.freq,gains.gain,'linear');
    erg.gain := erg.gain + interpol.interpolate(erg.freq);
    interpol.done()
  };
  erg.gain := erg.gain/len(index);
  return erg;
}

Add_Hakenjos_GainData := function(startdate=1097150400,obs='LOPES',dofreq=T) {
    freqar1 := [40,41,42,42.99,43.01,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,75.99,76.01,77,78,79,80]*1e6;
    flag1 := [1:4,39:43]
    freqar2 := [40,41,42,42.99,43.01,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,73.99,74.01,75,76,77,78,79,80]*1e6;
    flag2 := [1:4,37:43]
  for (i in [1:(data.len)]){
#    if (i<=10) { 
#      freqar := freqar1; flag := flag1;
#    } else {
      freqar := freqar2; flag := flag2; 
#    }
    file := spaste('Amp_2006_03_14/Ant',as_string(i),'_Amp');
    gains := read_Hakenjos_Gain(file);
    gvals := 1/sqrt(gains.gain);
    minval := min(gvals[gvals>0])/10.;
    gvals[!(gvals>0&gvals<=1)] := minval #negation to catch also NANs
    interpol := interpolate1d()
    interpol.initialize(gains.freq,gvals,'linear');
    intgvals := interpol.interpolate(freqar);
    interpol.done()

    intgvals[flag] := minval;

    rec := [=];
    rec.ant := data.head('AntID',i);
    rec.startdate := startdate;
    rec.field := 'ElGainCal';
    rec.data := intgvals
    print 'Adding data to ',rec.field,':',globalpar.CalTable_client[obs]->addData(rec);
    if (dofreq) {
      rec := [=];
      rec.ant := data.head('AntID',i);
      rec.startdate := startdate;
      rec.field := 'ElGainCalFreq';
      rec.data := freqar;
      print 'Adding data to ',rec.field,':',globalpar.CalTable_client[obs]->addData(rec);
    }
  };
}


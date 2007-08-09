# include 'CalibFromBurst.g'
# pcal := read_value('phasecal_rec.sos')
# print globalpar.CalTable_client["LOPES"]->openWrite();
# print globalpar.CalTable_client["LOPES"]->PrintSummary();
#
#
# include 'CalibFromBurst.g';pcal := read_value('phasecal_rec.sos');print globalpar.CalTable_client->openWrite();;print globalpar.CalTable_client->PrintSummary();


phases_from_Burst := function(ant,MF_Strength=30,MaxPdiff=50){
  result := [=];
  result.freq_ar := [40:80];
  f_low := result.freq_ar-.5;
  f_high := result.freq_ar+.5;
  freq := data.get('Frequency',ant)/1e6;
  ary := data.get('PhaseDiff',ant);
  mf_ary1 := cMeanFilter(ary,MF_Strength);
  flag := abs(mf_ary1-ary)>MaxPdiff;
  ary2 := ary;
  ary2[flag] := mf_ary1[flag];
  mf_ary2 := cMeanFilter(ary2,MF_Strength);
  result.phases := array(0.,len(result.freq_ar));
  for (i in seq(len(result.freq_ar))){
    result.phases[i] := mean(mf_ary2[((freq>f_low[i])&(freq<f_high[i]))]);
  };
  return result;
}

plotall_Pcals := function(prec,ants=[11:30]){
  pcsum := prec[spaste('phasecal',as_string(ants[1]))];
  plot(prec.freq_ar,prec[spaste('phasecal',as_string(ants[1]))],lines=T,col=2,clear=T,xlabel='Frequency [MHz]',ylabel='phasecal [deg]',yrange=[-180,180]);
  for (i in [2:len(ants)]){
    plot(prec.freq_ar,prec[spaste('phasecal',as_string(ants[i]))],col=(1+i),lines=T)
   pcsum +:= prec[spaste('phasecal',as_string(ants[i]))]
  };
  return pcsum;
}



AddPhaseCalData := function(prec,startdate){
  if (data.len != (len(prec)-1)) {
    print 'Probably incosistent data, no LOPES 30 event loaded?'
    return F;
  };
  for (i in [1:30]){
    rec := [=];
    rec.ant := data.head('AntID',i);
    rec.startdate := startdate;
    rec.field := 'PhaseCal';
    degarr := prec[spaste('phasecal',as_string(i))];
    rec.data := exp(-1i*degarr*degree);
    print range(degarr), range(rec.data),len(rec.data);
    print 'Adding data to ',rec.field,':',globalpar.CalTable_client->addData(rec);
    rec := [=];
    rec.ant := data.head('AntID',i);
    rec.startdate := startdate;
    rec.field := 'PhaseCalFreq';
    rec.data := prec.freq_ar*1e6;
    print 'Adding data to ',rec.field,':',globalpar.CalTable_client->addData(rec);
  };
};




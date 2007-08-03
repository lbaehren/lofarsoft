#######################################################################
#
# functions to help me plan the antenna positions
#
########################################################################



plot2d := function(pg, posarray, newpage=T, marker=3){
	shap := shape(posarray);
	if ((shap[2] > 3) || (shap[2] < 2)) {
	  print 'Second dim of position-array has to be [SN,EW] or [SN,EW,"up-down"]';
	  return F;
	};
	if (newpage) {
	  xmin := xmax := ymin := ymax :=0;
	  for (i in [1:shap[1]]) {
	    if (posarray[i,1] > ymax)  ymax := posarray[i,1];
	    if (posarray[i,1] < ymin)  ymin := posarray[i,1];
	    if (posarray[i,2] > xmax)  xmax := posarray[i,2];
	    if (posarray[i,2] < xmin)  xmin := posarray[i,2];
	  };
	  ymax := as_integer(ymax)+5;
	  ymin := as_integer(ymin)-5;
	  xmax := as_integer(xmax)+5;
	  xmin := as_integer(xmin)-5;
	  pg.sci(1);
	  pg.env(xmin,xmax,ymin,ymax,1,0);
	};
	pg.sci(pg.qci()+1);
	pg.pt(posarray[,2],posarray[,1],marker);
};

plotBaselines := function(pg, posarray) {
	shap := shape(posarray);
	if ((shap[2] > 3) || (shap[2] < 2)) {
	  print 'Second dim of position-array has to be [SN,EW] or [SN,EW,"up-down"]';
	  return F;
	};
	i:=0;
	for (j in [1:(shap[1]-1)]) {
	  for (k in [(j+1):shap[1]]) {
		i := i+1;
		x[i] := posarray[j,2]-posarray[k,2];
		y[i] := posarray[j,1]-posarray[k,1];
		i := i+1;
		x[i] := - x[i-1];
		y[i] := - y[i-1];
	  };
	};
	print 'plotted ',i,' Points';
	newarr := cbind(y,x);

	plot2d(pg,newarr);
};

projekt1d := function(posarray, az=0, plot=F) {
	shap := shape(posarray);
	if ((shap[2] > 3) || (shap[2] < 2)) {
	  print 'Second dim of position-array has to be [SN,EW] or [SN,EW,"up-down"]';
	  return F;
	};
	i := 0; s:= array(0.,shap[1]);
	for (j in [1:shap[1]]) {
	  i:=i+1;
	  s[i] := sqrt(posarray[j,2]^2+posarray[j,1]^2)*
			cos(atan(posarray[j,2]/posarray[j,1])-(az*pi/180));
	  if (posarray[j,1]<0) s[i] :=-s[i];
	  if (is_nan(s[i])) s[i] :=0;
	};
	if (plot) {
	  hmin := hmax :=0
	  for (i in [1:shap[1]]) {
	    if (s[i] > hmax)  hmax := s[i];
	    if (s[i] < hmin)  hmin := s[i];
	  };
	  hmax := as_integer(hmax)+1.5;
	  hmin := as_integer(hmin)-1.5;
	  nobins := min(200, (hmax-hmin) );
	  pg.hist(s,hmin,hmax,nobins,0);
	};
	return s;
};



kreispos := function(step=20) {
	i:=0;
	maxze := as_integer(90./step);
	for (ize in [0:maxze]) {
	  maxaz := as_integer(360/step*sin(ize*step*pi/180));
	  for (iaz in [0:maxaz]) {
	    i:=i+1;
	    ze := (ize*step);
	    az := (360*iaz/(maxaz+1))+(step/maxze*ize);
	    x[i] := sin(az*pi/180.)*ze;
	    y[i] := cos(az*pi/180.)*ze;
	  };
	};
	return cbind(y,x);
};

kreisposneu := function(step=20) {
	i:=0;
	maxze := as_integer(90./step);
	for (ize in [0:maxze]) {
	  maxaz := as_integer(2*pi/asin(min(1,(1/ize))));
	  for (iaz in [0:maxaz]) {
	    i:=i+1;
	    ze := (ize*step);
	    az := (360*iaz/(maxaz+1))+(step/maxze*ize);
	    x[i] := sin(az*pi/180.)*ze;
	    y[i] := cos(az*pi/180.)*ze;
	  };
	};
	return cbind(y,x);
};

kreispos2 := function(ants,ofs=0,scale=10) {
	if (len(ants) != len(ofs)) ofs := array(0,len(ants));
	i:=1;
	maxze := len(ants);
	x[i] := y[i] := 0;
	for (ize in [1:maxze]) {
	  maxaz := ants[ize]
	  for (iaz in [0:maxaz]) {
	    i:=i+1;
	    ze := (ize*scale);
	    az := (360*iaz/(maxaz+1))+ofs[ize];
	    x[i] := sin(az*pi/180.)*ze;
	    y[i] := cos(az*pi/180.)*ze;
	  };
	};
	return cbind(y,x);
};
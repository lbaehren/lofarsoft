(* ::Package:: *)

  (* Reconstruct an Exponentiated Signal from a Poisson Data Model *)


(* Call me as *)
(*
SetDirectory["~/Text/IFT/math/"];
<<poisson.m;
*)


(* Initial Setup *)

 SeedRandom[42];
 maxerror = 1.0;      
 finalerror = 1 10^-4;


(* Bias *)
	b = 2.5;
	b = 0.5;

 NMAX = 5;    (* number of Datasets to be processed *)
 imax = 500; (* maximal number of itterations in solver*)


 verbose = True; 
 verbose = False;

 movie = False;
 Nmovie = 10000;

 saveMaps = False;
 saveMaps = True;

 graphics = True;
 saveStat = True;
 saveD    = False;

 outdirectory = "PlotResults0";

(* Number of pixels *)

 npix = 512; (* must be multiple of 32, zero pixel to be added*)
 L = 1; (* length of periodic box *)
 dL = L/npix;

(* inverse of the decay length *)
	m = 1/0.04; 
(*	m = 1/0.06; *)

(* window *)
	kappa0 = 100; (* galaxies/length *)
	kappa0 = 1000; (* galaxies/length *)

	(* Parameter for statisics run *)
	(* 
 SeedRandom[42];
 maxerror = 0.01;      
 NMAX = 1000;
 npix = 512; 
 m = 1/0.03; 
 b = 1;
 kappa0 = 1000; 
*)


kappa = kappa0 dL *
	Flatten[{Table[1, {n, 0, npix/8}], 
		 Table[0, {n, 1, 4 npix/32}], 
		 Table[0.025, {n, 1, 3 npix/32}], 
		 Table[0.1+0.9(4 n/npix)^4, {n, 1, npix/4}],   
		 Table[1, {n, 1, npix/4}],  
		 Table[0, {n, 1, npix/32}] , 
		 Table[1, {n, 1, npix/8}]  }];

kappa = kappa0 dL * Table[1, {n, 0, npix}];

(* clean figure (fig 1) *)
(*
SeedRandom[23];
npix = 1024;
kappa = kappa0 dL * Table[1, {n, 0, npix}];
kappa0 = 8000; (* galaxies/length *)
b = 0.75;
m = 1/0.05; 
NMAX=1;
*)



mask = kappa/(dL kappa0);

(* definitions *)

one  = IdentityMatrix[npix+1];
zero = Table[0,{n,0,npix}];

X[n_] := dL n;
n[x_] := x/dL;
xtab = Table[X[n], {n, 0, npix} ];

Psf[k_]:= 1/(k^2 + m^2);
Csf[x_]= PowerExpand[InverseFourierTransform[Psf[k], k, x]];

Cs = N[Table[Csf[X[n]-X[(npix)/2]], {n, 0, npix} ]];
kernel = Sqrt[Cs];
delta =  Flatten[{1,zero}];
delta = ListConvolve[kernel,delta,(npix)/2+1];
powerNorm = Sum[delta[[n+1]]^2, {n,0,npix}]  ;
Cs = Cs/powerNorm;
S = Table[Table[Cs[[1+Mod[i-n+npix/2, npix+1]]], {i,0,npix } ], {n,0,npix}];
S /= S[[1]][[1]]; (* ATTENTION: correct S-normalisation *)
SHat = Table[S[[n]][[n]],{n,1,npix+1}];
InvS = Inverse[S]; 


(* LaplaceOperator *)

laplace = Flatten[{1,-2,1,zero}];

Laplace = Table[Table[laplace[[1+Mod[i-n+1, npix+1]]], {i,0,npix } ], {n,0,npix}];



(* define routines *)

lp[plotdata_]:= ListPlot[Transpose[{xtab, plotdata}], PlotRange->All, PlotJoined -> True];


WriteArray[file_, array_] := Module[{i, j, stream}, 
   stream = OpenWrite[file]; For[i = 1, i < Length[array] + 1, i++, 
     For[j = 1, j < Length[array[[i]]] + 1, j++, WriteString[stream, 
        CForm[N[array[[i]][[j]]]], " "]]; WriteString[stream, "\n"]; ]; 
    Close[stream]; ];


WriteArray[file_, headstring_, array_] := Module[{i, j, stream}, 
   stream = OpenWrite[file]; 
     WriteString[stream, headstring, "\n"]; 
     For[i = 1, i < Length[array] + 1, i++, 
     For[j = 1, j < Length[array[[i]]] + 1, j++, 
        WriteString[stream, CForm[N[array[[i]][[j]]]], " "]]; WriteString[stream, "\n"]; ]; 
    Close[stream]; ];

WriteGnuMatrix[file_, headstring_, array_] := Module[{i, j, stream}, 
   stream = OpenWrite[file]; 
     WriteString[stream, headstring, "\n"]; 
     For[i = 1, i < Length[array] + 1, i++, 
     For[j = 1, j < Length[array[[i]]] + 1, j++, 
        (* WriteString[stream, CForm[N[X[i-1]]], " "]; 
           WriteString[stream, CForm[N[X[j-1]]], " "]; *)
        WriteString[stream, CForm[N[array[[i]][[j]]]], " "]; 
        (* WriteString[stream, "\n"];*)
         ];
       WriteString[stream, "\n"];]; 
    Close[stream]; ];

show[l__] := Show[l, Axes -> False, Frame -> True, 
   DisplayFunction -> $DisplayFunction];

myGauss[] := 
Module[{x1=1, x2=1, w, y1, y2},
	While[(w=x1 x1 + x2 x2) >= 1, 
	x1 = 2 Random[] - 1;
	x2 = 2 Random[] - 1;
	];
	w = Sqrt[ (-2 * Log[ w ] ) / w ];
	y1 = x1 w;
	y2 = x2 w];


myPoisson[lambda_] := 
Module[{accuracy = 0.1,Nmax, rest, count,i},
	Nmax = Round[lambda/accuracy -0.5];
	rest = lambda - accuracy Nmax;
	count = If[Random[]<= rest,1,0];
	For[i=1, i<=Nmax, i++, count += If[Random[]<= accuracy,1,0]];
	count];




















(*********** MakeMap *********)


MakeMap[T_, error_] := Module[{i, j, k, l},
 Print["MakeMap with T = ", T];

 (* INIT *)
 negmask = UnitStep[-kappa];
 mask =  1-negmask;

 lastmap = lastlastmap = map;
 lastM = lastlastM = M ; (* b^2 kappa Exp[b map];*)

 dt = 0.001; (* start speed *)
 laststep = lastlaststep = step = Flatten[{map, M}] + 10000;  
 laststep2 = lastlaststep2 = step2 = step . step;
 discrepancy = Sqrt[dL step2];
 lastdiscrepancy = lastlastdiscrepancy = discrepancy ;

 Macc = Exp[2 b];
 Macc = 2/m ;

 Macc =1;

(* loop *) 
If[verbose, Print["Now looping"],0];	

 justbreakedflag = True;

For[time=0; i=0, ((Max[Abs[direction]] > finalerror) || (i<3)) , i++,

  (*If[time+dt>1,dt=(time-1) 1.001,0];*)
  Print[" "];
  If[verbose, Print[i, ": time = ", time, " dt = ", dt],0];	

  lastlastmap = lastmap;
  lastlastM = lastM;
  lastmap = map;
  lastM = M;

  MHat = DiagonalMatrix[M];
  Dprop = Inverse[InvS + MHat];
  DpropHat = Table[Dprop[[n]][[n]],{n,1,npix+1}];        
  D2 = Dprop^2; (* squaring the individual components *)
  If[verbose, Print["Dprop is now prepared !"],0];

  eta = Exp[b map + (T b^2/2) DpropHat];
  etaHat = DiagonalMatrix[eta];

  kappaeta = DiagonalMatrix[kappa eta];
  Ske = S . kappaeta;

  Dmap  = Dprop . (b data - b kappa eta - InvS . map);

  DM  = Macc (b^2 kappa eta - M);
  If[verbose, Print["Dm & DM are now prepared !"],0];

     (* explicit solver *)

  Dpsi= Flatten[{Dmap, DM}]; (* psi = {m,M} *)

  dpsi = dt Dpsi; 
  dm = dt Dmap;
  dM = dt DM;

(*   
  dm = dpsi[[Table[l,{l,1,npix+1}]]];
  dM = dpsi[[Table[l,{l,npix+2, 2 npix + 2}]]];
*)

  If[verbose, Print["dm & dM are now prepared !"],0];


  If[Min[M]<0, stopflag=True;Print["M negative"],stopflag=False];
  Dpsi2 = Dpsi.Dpsi;

  dDHat = - D2 . dM;


  lastlaststep = laststep;
  lastlaststep2 = laststep2;
  laststep = step;
  laststep2 = step2;
  step = Flatten[{dm,-D2 . dM/Macc}]; 
  step2 = step . step;

  If[i>0,  
      cosAngle = step . laststep /Sqrt[step2 laststep2] (* curvature of path *),
      cosAngle = 1 (* inital step*)];


  If[verbose, Print["map and M are now prepared !"],0];
  direction = Flatten[{Dmap, -D2 . DM/Macc}]; 


  lastlastdiscrepancy = lastdiscrepancy;
  lastdiscrepancy = discrepancy;
  discrepancy = Sqrt[direction . direction dL];


  (* update timestep *)
  time += dt;

  map += dm;
  M += dM;

  Print[i, ": stepped at time = ", time, " with dt = ", dt];
  Print[i, ": distance        = ", Sqrt[step2 dL]];       
  Print[i, ": lastdiscrepancy = ", lastdiscrepancy];       
  Print[i, ": map discrepancy = ", discrepancy];       
  Print[i, ": cosAngle        = ", cosAngle];       

   timeerror =  error;

   If[cosAngle < 0.9,  stopflag = True,
    If[cosAngle < 0.95,  dt *= 0.5,
     If[cosAngle < 0.98,  dt *= 0.9, 
      dt*= 1.05;
     ]]];

  If[cosAngle > 0.99, dt *= 1.5, 0];



  If[((discrepancy > 1.1 lastdiscrepancy)||stopflag)&&(justbreakedflag == False)  , 
      dt *= 0.1; 
      map = lastmap;
      lastmap  = lastlastmap; 
      M = lastM;
      lastM = lastlastM; 
      discrepancy = lastdiscrepancy;
      lastdiscrepancy = lastlastdiscrepancy;
      step = laststep;
      laststep = lastlaststep;
      step2 = laststep2;
      laststep2 = lastlaststep2;
      Print["Emergency Brake !!"];
      justbreakedflag = True,
      justbreakedflag = False ];

(*   If[dt>1,dt=1,0];
  If[verbose, Print[i, ": next dt = ", dt],0]; 
*)

 If[movie && (Round[1.0 i/Nmovie] Nmovie == i),
    plotmap    = ListPlot[Transpose[{xtab, map}], 
                     PlotRange->All, PlotJoined -> True, 
                     DisplayFunction -> Identity, 
                     PlotStyle->RGBColor[1,0,0]];
    plotmappD    = ListPlot[Transpose[{xtab, map + DpropHat}], 
                     PlotRange->All, PlotJoined -> True, 
                     DisplayFunction -> Identity, 
                     PlotStyle->RGBColor[0,1,0]];
    plotmapmD    = ListPlot[Transpose[{xtab, map - DpropHat}], 
                     PlotRange->All, PlotJoined -> True, 
                     DisplayFunction -> Identity, 
                     PlotStyle->RGBColor[0,1,0]];
    plotDprop  = ListPlot[Transpose[{xtab, DpropHat}], PlotRange->All, 
                     PlotJoined -> True, DisplayFunction -> Identity, 
                     PlotStyle->RGBColor[0,0,0.5]];

    figuremap  = show[plotsignal, plotmap, plotmappD, plotmapmD, PlotRange->All];     
(* lp[dm]; lp[- D2. dM] *); 

    ];


   If[i>=imax, Print["Maximal number of itterations reached."]; direction *= 0,0];


   ];	(* end of For-loop *)  

 Print["finished MakeMap with T = ", T , " and map discrepancy = ", discrepancy];


 rest = signal-map;
 Dm    = Inverse[InvS +  b^2 DiagonalMatrix[kappa Exp[b map]] ];
 DmHat = Table[Dm[[n]][[n]],{n,1,npix+1}];

 If[graphics,
 plotmap    = ListPlot[Transpose[{xtab, map}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity, 
                       PlotStyle->RGBColor[1,0,0]];
 plotrest   = ListPlot[Transpose[{xtab, rest}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity];
 plotDm     = ListPlot[Transpose[{xtab, DmHat}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity, 
                       PlotStyle->RGBColor[0,1,0]];
 plotDprop  = ListPlot[Transpose[{xtab, DpropHat}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity, 
                       PlotStyle->RGBColor[1,0,0]];
 plotDmM    = ListPlot[Transpose[{xtab, -DmHat}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity, 
                       PlotStyle->RGBColor[0,1,0]];
 plotDpropM = ListPlot[Transpose[{xtab, -DpropHat}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity, 
                       PlotStyle->RGBColor[1,0,0]];

 figuremap  = show[plotsignal, plotmap, plotnaive, PlotRange->All];
 (*  figurerest = show[plotrest, plotDm, plotDprop, plotD0, plotDmM, plotDpropM, plotD0M];*)
 figurerest = show[plotrest, plotDprop, plotD0, plotDpropM, plotD0M];
 ,0];
]; (* end of MakeMap*)


MakeInfo := Module[{i,j},
 Print["MakeInfo - start"];
 Q = S . MHat;
 oneQ = one + Q;
 eigensystem = Eigensystem[oneQ];
 eVal = eigensystem[[1]];
 eVec = eigensystem[[2]];
 IeVec= Inverse[eVec];
(* Tests
  onetst = IeVec . eVec; 
 oneQtst = IeVec . DiagonalMatrix[eVal] . eVec;
  QHat = Table[oneQ[[n]][[n]],{n,1,npix+1}]; 
  QtHat = Table[oneQtst[[n]][[n]],{n,1,npix+1}]; 
  p1 = ListPlot[Transpose[{xtab, QHat}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity];
  p2 = ListPlot[Transpose[{xtab, QtHat}],PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity]; 
  show[p1,p2];
*)  
 logoneQ = IeVec . DiagonalMatrix[Log[eVal]] . eVec;
 info = 0.5 Table[logoneQ[[n]][[n]],{n,1,npix+1}]; 

 (* now pertubatively *)
 Q = S . DiagonalMatrix[b^2 kappa];
 oneQ = one + Q;
 eigensystem = Eigensystem[oneQ];
 eVal = eigensystem[[1]];
 eVec = eigensystem[[2]];
 IeVec= Inverse[eVec];
 logoneQ = IeVec . DiagonalMatrix[Log[eVal]] . eVec;
 info0 = 0.5 Table[logoneQ[[n]][[n]],{n,1,npix+1}]; (* expected gain before the date came *)
 info1 = info0 + 0.5 (b^3 kappa D0Hat) (mWien + 0.5 b (D0Hat + mWien^2)); (* pertubative correction *)

If[graphics,
 plotInfo = ListPlot[Transpose[{xtab, info}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity]; show[plotInfo];
,0];

 Print["MakeInfo - end"];
];


InitData := Module[{i,j},
 Print["InitData - start"];
 WhiteNoise = Table[myGauss[], {n, 0, npix} ];
 signal = ListConvolve[kernel,WhiteNoise,npix/2+1];
 signal = signal/Sqrt[powerNorm];
 mu = kappa Exp[b signal];
 data = Table[myPoisson[mu[[i]]], {i,1,npix+1}];
 jinfo = b (data - kappa);
 snaive = xnaive = {};
 mnaive = zero;
 For[i=1, i<=npix+1, i++, 
     If[kappa[[i]] >0,
        If[data[[i]] >0,     
          xnaive = Flatten[{xnaive, xtab[[i]]}]; 
          mnaive[[i]] = Log[data[[i]]/kappa[[i]]]/b;
          snaive = Flatten[{snaive,  mnaive[[i]]}]
          ,
          (* put in local classical solution for d=0 *)
          x=1;
          For[j=1,j<=10,j++,
	       x = S[[i]][[i]] kappa[[i]] b (1 + Exp[- b x])];
          mnaive[[i]] = -x]
        ,
        mnaive[[i]] = 0]
 ];
 D0 = Inverse[InvS +  b^2 DiagonalMatrix[kappa] ];
 D0Hat      = Table[D0[[n]][[n]],{n,1,npix+1}];
 mWien = D0 . jinfo;
 mWienPlus = D0 . (jinfo - 1/2 b^3 (kappa D0Hat + mWien^2 + b D0Hat mWien) );

 If[graphics,
    plotsignal = ListPlot[Transpose[{xtab, signal}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity];
    plotD0     = ListPlot[Transpose[{xtab, D0Hat}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity, 
                       PlotStyle->RGBColor[0.5,0.5,0]];
    plotD0M     = ListPlot[Transpose[{xtab, -D0Hat}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity, 
                       PlotStyle->RGBColor[0.5,0.5,0]];
    plotWien0   = ListPlot[Transpose[{xtab, mWien}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity, 
                  PlotStyle->RGBColor[1,0,1]];
    plotWienP   = ListPlot[Transpose[{xtab, mWienPlus}], PlotRange->{-2,2}, PlotJoined -> True, DisplayFunction -> Identity, 
                  PlotStyle->RGBColor[0,1,1]];
    plotnaive   = ListPlot[Transpose[{xtab, mnaive}], PlotRange->All, DisplayFunction -> Identity];
    figure0 = show[plotsignal, plotD0, plotWien0, plotnaive],0];


  If[saveD && counter == 1,
     Module[{filename = StringJoin[{outdirectory,"/D0.tab"}],
	      results, head},
        Print["writing D0 to file: ", filename];
        WriteGnuMatrix[filename, "#D0 - initial propagator", D0];
     ];];


  Print["InitData - end"];
];


(**************************************)
(************* CYCLE ******************)
(**************************************)


(* init statistics *)
(*
na : naive; 00 = cl = classic/T=0; 05 = hy = hybrid/T=0.5; 10 = fl = full/T=1 
*)

statna = zero;
stat00 = zero;
stat05 = zero;
stat10  = zero;
statDna = zero;
statD00  = zero;
statD05 = zero;
statD10  = zero;


For[counter=1, counter <NMAX+1, counter++,

 Print["cycle No. ",counter];
 InitData; 

 (* smeared out verion of naive map *)
 norm = Sum[S[[1]][[i]],{i,1,npix+1}];
 map = S . mnaive /norm; 

 M = b^2 kappa Exp[b map];
 MHat = DiagonalMatrix[M];
 Dprop = Inverse[InvS + MHat];
 DpropHat = Table[Dprop[[n]][[n]],{n,1,npix+1}];        

 mapna = map; DnaHat= DpropHat;
 rest = signal - mapna;
 statna = statna + rest^2;
 statDna= statDna + DnaHat;
 Mna = M;

 (* clasical map *) 

 MakeMap[0, maxerror]; 
 map00 = map; D00Hat= DpropHat; D00mHat = DmHat;
 stat00 = stat00 + rest^2;
 statD00= statD00 + D00Hat;
 M00 =M;

 (* interpolation between mapcl and mapit *) 

 MakeMap[0.5, maxerror]; 
 map05 = map; D05Hat= DpropHat; D05mHat = DmHat;
 stat05 = stat05 + rest^2;
 statD05= statD05 + D05Hat;
 M05 =M;

 (* interpolation between mapit *) 

 MakeMap[1, maxerror]; 
 map10 = map; D10Hat= DpropHat; D10mHat = DmHat;
 stat10 = stat10 + rest^2;
 statD10= statD10 + D10Hat;
 M10 =M;
 D10          = Dprop;

 If[graphics,
    plotcl  = ListPlot[Transpose[{xtab, map00}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity, 
                       PlotStyle->RGBColor[0,1,0]];
    plotit  = ListPlot[Transpose[{xtab, map10}], PlotRange->All, PlotJoined -> True, DisplayFunction -> Identity, 
                       PlotStyle->RGBColor[1,0,0]];
    figure1 = show[plotsignal,  plotcl, plotit]
    ,0];


  If[saveStat,
     Module[{filename = StringJoin[{outdirectory,"/stat.tab"}],
	      head, results, c = counter},
        Print["writing statistics to file: ", filename];
        results = Transpose[{xtab, mask, kappa, statna/c, stat00/c, stat05/c, stat10/c,
                            statDna/c, statD00/c, statD05/c, statD10/c}];
        head = "#  xtab-1  mask-2  kappa-3  statna-4  stat00-5  stat05-6  stat10-7  statDna-8 statD00-9  statD05-10  statD10-11 ";
        WriteArray[filename, head, results];
     ];
  ];

  If[saveMaps,
     Module[{filename = StringJoin[{outdirectory,"/data", ToString[counter], ".tab"}],
	      results, head},
        Print["writing results to file: ", filename];
        results = Transpose[{
           xtab, mask, kappa, signal, data, mu, jinfo,
           mnaive, mWien, mWienPlus, D0Hat,
           mapna, map00, map05, map10,
           Mna, M00, M05, M10,
           DnaHat, D00Hat, D05Hat, D10Hat, info, info0, info1}];
        head = "#  xtab-1  mask-2  kappa-3  signal-4  data-5  mu-6  jinfo-7  mnaive-8  mWien-9 mWienPlus-10  D0Hat-11  mapna-12  map00-13  map05-14  map10-15  Mna-16  M00-17 M05-18  M10-19  DclHat-20  D00Hat-21  D05Hat-22  D10Hat-23  info-24  info0-25  info1-26";
        WriteArray[filename, head, results];
     ];
  ];

  If[saveD,
     Module[{filename = StringJoin[{outdirectory,"/D10.", ToString[counter], ".tab"}],
	      results, head},
        Print["writing D10 to file: ", filename];
        WriteGnuMatrix[filename, "#Dprop T=1", D10];
     ];
  ];

];


Print["done"];

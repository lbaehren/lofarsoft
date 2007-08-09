########################################################################
########################################################################
#  Global parameter definitions for the cosmic ray events
#
#  The parameters in "globalconst" are defined in globals-const.g
#  The normal (i.e. not cosmic ray specific) parameters in "globalpar" 
#  are defined in globals-par.g
# 
########################################################################
########################################################################

########################################################################
#
#Section: Global Parameters
#
########################################################################


#-----------------------------------------------------------------------
#Name:   globalpar.event_shifts
#-----------------------------------------------------------------------
#Type: Float
#Doc:  Shift events by the time (in seconds). This allows for a manual correction
#      of the trigger arrival times. Negative delays shift the pulse to the right.
#      This is used for the GUI pipeline.
#Ref: guipipeline
#-----------------------------------------------------------------------
  globalpar.event_shifts:=[=];

#Manual shifts
  globalpar.event_shifts['1074151750']:=-0.08e-6
  globalpar.event_shifts['1074253027']:=0.06e-6
  globalpar.event_shifts['1074356321']:=-0.04e-6
  globalpar.event_shifts['1074495064']:=-0.02e-6
  globalpar.event_shifts['1076953836']:=0.125e-6
  globalpar.event_shifts['1077040605']:=-0.025e-6
  globalpar.event_shifts['1078184705']:=-0.085e-6
  globalpar.event_shifts['1081329309']:=0.08e-6-0.01776919e-6
  globalpar.event_shifts['1083612353']:=-0.15e-6
  globalpar.event_shifts['1086932036']:=0.06e-6
  globalpar.event_shifts['1080878978']:=-0.01e-6

#shifts from crresults.g print_shifts using the location of the 
#Gaussian fits to the CC-beam

globalpar.event_shifts['1073691822']:=-0.00522628e-6
globalpar.event_shifts['1073711595']:=0.01373365e-6
globalpar.event_shifts['1073731907']:=0.01141325e-6
globalpar.event_shifts['1073822733']:=0.0201702e-6
globalpar.event_shifts['1073867292']:=0.00643452e-6
globalpar.event_shifts['1073955167']:=0.02166551e-6
globalpar.event_shifts['1073964310']:=0.01161568e-6
globalpar.event_shifts['1074007766']:=-0.0176917e-6
globalpar.event_shifts['1074151750']:=-0.07404427e-6
globalpar.event_shifts['1074253027']:=0.06617833e-6
globalpar.event_shifts['1074356321']:=-0.04783987e-6
globalpar.event_shifts['1074364106']:=-0.02800748e-6
globalpar.event_shifts['1074438314']:=0.0461989e-6
globalpar.event_shifts['1074495063']:=0.00106103e-6
globalpar.event_shifts['1074701746']:=-0.01346758e-6
globalpar.event_shifts['1074765079']:=0.01039797e-6
globalpar.event_shifts['1074805669']:=-0.02869169e-6
globalpar.event_shifts['1074973132']:=-0.04418452e-6
globalpar.event_shifts['1075135889']:=0.01138049e-6
globalpar.event_shifts['1075138371']:=-0.02076031e-6
globalpar.event_shifts['1075220515']:=0.00961093e-6
globalpar.event_shifts['1075242794']:=-0.01738843e-6
globalpar.event_shifts['1075251666']:=0.02748919e-6
globalpar.event_shifts['1075272262']:=-0.01788547e-6
globalpar.event_shifts['1075559547']:=-0.00017921e-6
globalpar.event_shifts['1075796522']:=0.01126301e-6
globalpar.event_shifts['1075812862']:=0.03748171e-6
globalpar.event_shifts['1075826446']:=0.0245765e-6
globalpar.event_shifts['1075868236']:=-0.05813173e-6
globalpar.event_shifts['1076177163']:=0.00269933e-6
globalpar.event_shifts['1076377694']:=-0.02846502e-6
globalpar.event_shifts['1076532021']:=-0.00258526e-6
globalpar.event_shifts['1076576545']:=-0.00986519e-6
globalpar.event_shifts['1076603825']:=-0.02875897e-6
globalpar.event_shifts['1076661706']:=0.01422947e-6
globalpar.event_shifts['1076701523']:=-0.00237299e-6
globalpar.event_shifts['1076703471']:=0.02007674e-6
globalpar.event_shifts['1076710835']:=0.03901458e-6
globalpar.event_shifts['1076726671']:=-0.05748593e-6
globalpar.event_shifts['1076859240']:=-0.00592864e-6
globalpar.event_shifts['1076899794']:=-0.02537453e-6
globalpar.event_shifts['1076953836']:=0.13118757e-6
globalpar.event_shifts['1077040605']:=-0.01786213e-6
globalpar.event_shifts['1077105362']:=-0.01133231e-6
globalpar.event_shifts['1077119167']:=0.02175784e-6
globalpar.event_shifts['1077129087']:=0.05226291e-6
globalpar.event_shifts['1077380332']:=0.03166877e-6
globalpar.event_shifts['1077411001']:=-0.01069355e-6
globalpar.event_shifts['1077525880']:=0.01771477e-6
globalpar.event_shifts['1077695153']:=0.0034286e-6
globalpar.event_shifts['1077961965']:=0.0278767e-6
globalpar.event_shifts['1077997027']:=0.00352226e-6
globalpar.event_shifts['1078068270']:=-0.01824778e-6
globalpar.event_shifts['1078288506']:=-0.04612095e-6
globalpar.event_shifts['1078300431']:=-0.00645019e-6
globalpar.event_shifts['1078354639']:=-0.00261587e-6
globalpar.event_shifts['1078410450']:=-0.02376445e-6
globalpar.event_shifts['1078661204']:=-0.01107751e-6
globalpar.event_shifts['1078686484']:=0.02885758e-6
globalpar.event_shifts['1078712393']:=0.01556809e-6
globalpar.event_shifts['1078747283']:=-0.03485124e-6
globalpar.event_shifts['1078842386']:=0.04130225e-6
globalpar.event_shifts['1079109620']:=0.0195027e-6
globalpar.event_shifts['1079151013']:=0.02812221e-6
globalpar.event_shifts['1079197261']:=0.01967553e-6
globalpar.event_shifts['1079228956']:=0.01827001e-6
globalpar.event_shifts['1079243404']:=0.009799e-6
globalpar.event_shifts['1079304657']:=0.03338507e-6
globalpar.event_shifts['1079370859']:=-0.02977622e-6
globalpar.event_shifts['1079376907']:=0.00237882e-6
globalpar.event_shifts['1079421782']:=-0.0252348e-6+0.008e-6
globalpar.event_shifts['1079421783']:=-0.0252348e-6+0.040e-6+0.008e-6
globalpar.event_shifts['1079568867']:=-0.01499632e-6
globalpar.event_shifts['1079645833']:=0.02188247e-6
globalpar.event_shifts['1079649880']:=0.0510723e-6
globalpar.event_shifts['1079734470']:=-0.05236314e-6
globalpar.event_shifts['1079784257']:=-0.03163849e-6
globalpar.event_shifts['1079789580']:=0.00551864e-6
globalpar.event_shifts['1079797569']:=-0.04212083e-6
globalpar.event_shifts['1079840645']:=-0.07260705e-6
globalpar.event_shifts['1079956564']:=-0.03032099e-6
globalpar.event_shifts['1080051198']:=-0.02185661e-6
globalpar.event_shifts['1080328401']:=-0.03548774e-6
globalpar.event_shifts['1080334826']:=-0.021305e-6
globalpar.event_shifts['1080398526']:=0.03751865e-6
globalpar.event_shifts['1080415000']:=-0.0181451e-6
globalpar.event_shifts['1080501624']:=-0.02932123e-6
globalpar.event_shifts['1080531610']:=0.00571913e-6
globalpar.event_shifts['1080560055']:=-0.0194783e-6
globalpar.event_shifts['1080859053']:=-0.02248841e-6
globalpar.event_shifts['1080878979']:=-0.04820319e-6
globalpar.event_shifts['1080921150']:=0.03474097e-6
#globalpar.event_shifts['1081329310']:=-0.01776919e-6
globalpar.event_shifts['1081329865']:=-0.02144365e-6
globalpar.event_shifts['1081536609']:=0.01071813e-6
globalpar.event_shifts['1081599982']:=-0.00012892e-6
globalpar.event_shifts['1081766441']:=0.01186069e-6
globalpar.event_shifts['1081785683']:=0.03659531e-6
globalpar.event_shifts['1082005701']:=0.01429425e-6
globalpar.event_shifts['1082024164']:=-0.02666179e-6
globalpar.event_shifts['1082447199']:=0.03561617e-6
globalpar.event_shifts['1082655111']:=-0.03468152e-6
globalpar.event_shifts['1082857644']:=-0.00047699e-6
globalpar.event_shifts['1082859730']:=-0.00551731e-6
globalpar.event_shifts['1082884832']:=-0.00026086e-6
globalpar.event_shifts['1082993649']:=-0.02733243e-6
globalpar.event_shifts['1083073661']:=0.01975991e-6
globalpar.event_shifts['1083130550']:=-0.01384516e-6
globalpar.event_shifts['1083170765']:=-0.01598165e-6
globalpar.event_shifts['1083179672']:=0.02912273e-6
globalpar.event_shifts['1083612353']:=-0.14484135e-6
globalpar.event_shifts['1083619910']:=-0.02364611e-6
globalpar.event_shifts['1083707049']:=-0.02296169e-6
globalpar.event_shifts['1083762896']:=-0.03238066e-6
globalpar.event_shifts['1083853690']:=0.03357327e-6
globalpar.event_shifts['1084283789']:=-0.05016122e-6
globalpar.event_shifts['1084374200']:=0.04255754e-6
globalpar.event_shifts['1084410317']:=-0.0197765e-6
globalpar.event_shifts['1084430995']:=-0.03427513e-6
globalpar.event_shifts['1084439669']:=0.02543202e-6
globalpar.event_shifts['1084519426']:=-0.04780654e-6
globalpar.event_shifts['1084641951']:=0.04176211e-6
globalpar.event_shifts['1084945199']:=0.05235836e-6
globalpar.event_shifts['1084972686']:=0.0132034e-6
globalpar.event_shifts['1085141658']:=0.02200139e-6
globalpar.event_shifts['1085324447']:=0.01449505e-6
globalpar.event_shifts['1085365541']:=0.04703509e-6
globalpar.event_shifts['1085420319']:=0.0205497e-6
globalpar.event_shifts['1085444216']:=-0.03180684e-6
globalpar.event_shifts['1085604059']:=-0.05519281e-6
globalpar.event_shifts['1085605622']:=-0.07093647e-6
globalpar.event_shifts['1086082186']:=0.02839992e-6
globalpar.event_shifts['1086167597']:=0.02523822e-6
globalpar.event_shifts['1086187667']:=-0.00859669e-6
globalpar.event_shifts['1086248227']:=0.0109217e-6
globalpar.event_shifts['1086683074']:=0.04485785e-6
globalpar.event_shifts['1086932036']:=0.05760965e-6
globalpar.event_shifts['1086945674']:=0.0315576e-6
globalpar.event_shifts['1087097636']:=-0.00724222e-6
#LOPES30-events
globalpar.event_shifts['1130833235']:=-15.4409e-6 
globalpar.event_shifts['1131596398']:=-3.3717e-6
globalpar.event_shifts['1131849452']:=-0.07e-6 #?
#globalpar.event_shifts['1133645828']:=-0.0134e-6
globalpar.event_shifts['1134504350']:=0.0003e-6 #
globalpar.event_shifts['1134525288']:=0.0055e-6 #
globalpar.event_shifts['1135044024']:=0.0451e-6 #
globalpar.event_shifts['1137642119']:=0.0068e-6 #
#globalpar.event_shifts['1138109037']:=-0.0018e-6 #probably broken event
globalpar.event_shifts['1138229645']:=0.0383e-6 #
globalpar.event_shifts['1139237948']:=0.07099e-6#
globalpar.event_shifts['1139790566']:=0.03e-6 #large offset(2.114 deg)
globalpar.event_shifts['1140667370']:=0.0211e-6 #large offset(1.598)
globalpar.event_shifts['1141750547']:=0.0264e-6 #


#Weird shift by one in the CRID ....? 
for (f in field_names(globalpar.event_shifts)) {globalpar.event_shifts[as_string(as_integer(f)-1)]:=globalpar.event_shifts[f]}

#-----------------------------------------------------------------------
#Name:   globalpar.event_flag
#-----------------------------------------------------------------------
#Type: record of records
#Doc: Contains records listing the antennas to be manually flagged for each event.
#Ref: guipipeline
#-----------------------------------------------------------------------
  globalpar.event_flag:=[=];
  globalpar.event_flag['1074495064']:=[6]  	#Event 21
  globalpar.event_flag['1076349731']:=[3]  	#Event 54
  globalpar.event_flag['1077057092']:=[1,8,9]	#Event 72
  globalpar.event_flag['1077074831']:=[1,8,9]	#Event 73
  globalpar.event_flag['1077105362']:=[1,8,9]	#Event 74
  globalpar.event_flag['1077105362']:=[9,10]	#Event 74
  globalpar.event_flag['1077303085']:=[5,6]	#Event 79
  globalpar.event_flag['1077364522']:=[2,5,6]	#Event 81
  globalpar.event_flag['1078356492']:=[6]	#Event 100
  globalpar.event_flag['1079421782']:=[6,4]	#Event 125
  globalpar.event_flag['1079715494']:=[1,2]	#Event 132
  globalpar.event_flag['1080878978']:=[5]	#Event 163
  globalpar.event_flag['1081536608']:=[9]	#Event 169
  globalpar.event_flag['1082614878']:=[3,4]	#Event 181
  globalpar.event_flag['1082874928']:=[5,6]	#Event 187
  globalpar.event_flag['1082988469']:=[3]	#Event 189
  globalpar.event_flag['1087144039']:=[3,6]	#Event 247
  globalpar.event_flag['1089409015']:=[2,3,4]	#Event 263
  globalpar.event_flag['1089502431']:=[5,10]	#Event 265
  globalpar.event_flag['1089682265']:=[4,5,10]	#Event 268
  globalpar.event_flag['1091757921']:=[3,4]	#Event 299
  globalpar.event_flag['1091840877']:=[9,10]	#Event 303
  globalpar.event_flag['1092449007']:=[1,2]	#Event 318
  globalpar.event_flag['1092535416']:=[3,4]	#Event 321
  globalpar.event_flag['1092543789']:=[6,9]	#Event 322
  globalpar.event_flag['1092612658']:=[3,7]	#Event 325
  globalpar.event_flag['1092619925']:=[3,5]	#Event 326
  globalpar.event_flag['1093878969']:=[3,7]	#Event 352
  globalpar.event_flag['1093999706']:=[4,7]	#Event 360
  globalpar.event_flag['1094731725']:=[1,4]	#Event 375
  globalpar.event_flag['1094891225']:=[4]	#Event 377
  globalpar.event_flag['1094993176']:=[4]	#Event 379
  globalpar.event_flag['1095050271']:=[4]	#Event 380
  globalpar.event_flag['1095105081']:=[1]	#Event 383
  globalpar.event_flag['1095119141']:=[5,7]	#Event 384
  globalpar.event_flag['1095394094']:=[1,3]	#Event 387
  globalpar.event_flag['1095618411']:=[7,8]	#Event 392
#LOPES30-events
  globalpar.event_flag['1137642119']:=[7,18]
  globalpar.event_flag['1139790566']:=[11,18,19]
#  globalpar.event_flag['1140667370']:=[11:30]
#  globalpar.event_flag['1140667370']:=[1:10,21:30]
#  globalpar.event_flag['']:=[]

#Weird shift by one in the CRID ....? 
#Davon sehe ich nichts. (A.Horneffer)
#for (f in field_names(globalpar.event_flag)) {globalpar.event_flag[as_string(as_integer(f)-1)]:=globalpar.event_flag[f]}



#-----------------------------------------------------------------------
#Name:   globalpar.event_distance
#-----------------------------------------------------------------------
#Type: number
#Doc: Contains distances for each event.
#Ref: guipipeline, data.load
#-----------------------------------------------------------------------
globalpar.event_distance:=[=];
globalpar.event_distance['1073691822']:=5000
globalpar.event_distance['1073711595']:=3500
globalpar.event_distance['1073731907']:=4000
globalpar.event_distance['1073822733']:=2250
globalpar.event_distance['1073867292']:=1750
globalpar.event_distance['1073955167']:=3500
globalpar.event_distance['1073964310']:=2250
globalpar.event_distance['1074007766']:=2500
globalpar.event_distance['1074151750']:=2500
globalpar.event_distance['1074253027']:=1750
globalpar.event_distance['1074356321']:=4750
globalpar.event_distance['1074364106']:=3000
globalpar.event_distance['1074438314']:=1500
globalpar.event_distance['1074495063']:=2750
globalpar.event_distance['1074701746']:=2000
globalpar.event_distance['1074765079']:=3500
globalpar.event_distance['1074805669']:=4250
globalpar.event_distance['1074973132']:=3500
globalpar.event_distance['1075135889']:=3750
globalpar.event_distance['1075138371']:=2000
globalpar.event_distance['1075220515']:=3500
globalpar.event_distance['1075242794']:=3500
globalpar.event_distance['1075251666']:=2750
globalpar.event_distance['1075272262']:=2750
globalpar.event_distance['1075559547']:=2000
globalpar.event_distance['1075796522']:=3500
globalpar.event_distance['1075812862']:=2500
globalpar.event_distance['1075826446']:=2250
globalpar.event_distance['1075868236']:=1750
globalpar.event_distance['1076177163']:=1750
globalpar.event_distance['1076377694']:=1500
globalpar.event_distance['1076532021']:=3000
globalpar.event_distance['1076576545']:=3500
globalpar.event_distance['1076603825']:=1500
globalpar.event_distance['1076661706']:=1500
globalpar.event_distance['1076701523']:=2250
globalpar.event_distance['1076703471']:=2000
globalpar.event_distance['1076710835']:=3250
globalpar.event_distance['1076726671']:=3000
globalpar.event_distance['1076859240']:=3750
globalpar.event_distance['1076899794']:=2500
globalpar.event_distance['1076953836']:=2000
globalpar.event_distance['1077040605']:=2000
globalpar.event_distance['1077105362']:=4750
globalpar.event_distance['1077119167']:=2500
globalpar.event_distance['1077129087']:=3250
globalpar.event_distance['1077380332']:=1500
globalpar.event_distance['1077411001']:=3500
globalpar.event_distance['1077525880']:=2250
globalpar.event_distance['1077695153']:=2500
globalpar.event_distance['1077961965']:=8000
globalpar.event_distance['1077997027']:=3250
globalpar.event_distance['1078068270']:=1750
globalpar.event_distance['1078288506']:=4250
globalpar.event_distance['1078300431']:=2000
globalpar.event_distance['1078354639']:=2500
globalpar.event_distance['1078410450']:=2250
globalpar.event_distance['1078661204']:=4750
globalpar.event_distance['1078686484']:=2750
globalpar.event_distance['1078712393']:=3500
globalpar.event_distance['1078747283']:=2750
globalpar.event_distance['1078842386']:=5250
globalpar.event_distance['1079109620']:=2750
globalpar.event_distance['1079151013']:=2250
globalpar.event_distance['1079197261']:=2250
globalpar.event_distance['1079228956']:=3500
globalpar.event_distance['1079243404']:=2500
globalpar.event_distance['1079304657']:=8000
globalpar.event_distance['1079370859']:=2250
globalpar.event_distance['1079376907']:=1750
globalpar.event_distance['1079421783']:=3750
globalpar.event_distance['1079568867']:=2500
globalpar.event_distance['1079645833']:=2500
globalpar.event_distance['1079649880']:=3000
globalpar.event_distance['1079734470']:=2250
globalpar.event_distance['1079784257']:=7500
globalpar.event_distance['1079789580']:=2750
globalpar.event_distance['1079797569']:=2750
globalpar.event_distance['1079840645']:=8000
globalpar.event_distance['1079956564']:=3000
globalpar.event_distance['1080051198']:=1750
globalpar.event_distance['1080328401']:=7500
globalpar.event_distance['1080334826']:=2750
globalpar.event_distance['1080398526']:=4500
globalpar.event_distance['1080415000']:=2000
globalpar.event_distance['1080501624']:=3000
globalpar.event_distance['1080531610']:=2000
globalpar.event_distance['1080560055']:=5250
globalpar.event_distance['1080859053']:=1500
globalpar.event_distance['1080878979']:=2500
globalpar.event_distance['1080921150']:=8000
globalpar.event_distance['1081329310']:=4750
globalpar.event_distance['1081329865']:=3750
globalpar.event_distance['1081536609']:=3500
globalpar.event_distance['1081599982']:=8000
globalpar.event_distance['1081766441']:=2750
globalpar.event_distance['1081785683']:=5250
globalpar.event_distance['1082005701']:=2500
globalpar.event_distance['1082024164']:=1750
globalpar.event_distance['1082447199']:=2000
globalpar.event_distance['1082655111']:=2250
globalpar.event_distance['1082857644']:=2250
globalpar.event_distance['1082859730']:=1500
globalpar.event_distance['1082884831']:=2250
globalpar.event_distance['1082884832']:=2250
globalpar.event_distance['1082993649']:=2500
globalpar.event_distance['1083073661']:=2250
globalpar.event_distance['1083130550']:=1500
globalpar.event_distance['1083170765']:=3000
globalpar.event_distance['1083179672']:=3000
globalpar.event_distance['1083612353']:=6000
globalpar.event_distance['1083619910']:=3750
globalpar.event_distance['1083707049']:=2750
globalpar.event_distance['1083762896']:=2250
globalpar.event_distance['1083853690']:=2500
globalpar.event_distance['1084283789']:=8000
globalpar.event_distance['1084374200']:=2250
globalpar.event_distance['1084410317']:=1750
globalpar.event_distance['1084430995']:=1750
globalpar.event_distance['1084439669']:=1750
globalpar.event_distance['1084519426']:=2500
globalpar.event_distance['1084641951']:=2500
globalpar.event_distance['1084945199']:=1750
globalpar.event_distance['1084972686']:=2500
globalpar.event_distance['1085141658']:=2500
globalpar.event_distance['1085324447']:=3000
globalpar.event_distance['1085365541']:=2500
globalpar.event_distance['1085420319']:=4750
globalpar.event_distance['1085444216']:=2000
globalpar.event_distance['1085604059']:=4250
globalpar.event_distance['1085605622']:=2750
globalpar.event_distance['1086082186']:=3500
globalpar.event_distance['1086167597']:=2750
globalpar.event_distance['1086187667']:=4000
globalpar.event_distance['1086248227']:=3000
globalpar.event_distance['1086683074']:=3500
globalpar.event_distance['1086932036']:=2000
globalpar.event_distance['1086945674']:=3250
globalpar.event_distance['1087097636']:=3250

#Weird shift by one in the CRID ....? 
print 'There is a weird shift by one in the crid since some time ... - fixing globalpar.event_distance etc ... !!!'
for (f in field_names(globalpar.event_distance)) {globalpar.event_distance[as_string(as_integer(f)-1)]:=globalpar.event_distance[f]}

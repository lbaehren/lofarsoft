#ifndef Bin_Size_X
#define	Bin_Size_X	1	//Bin size for core distribution in meters
#define	Bin_Size_Y	1	//Bin size for ''	''	''	''
#define	NO_TRACES	10	//Max.no. of events for which the traces are to be plotted
#define	NO_LASAs	5
#define	NO_DETs		NO_LASAs*4
#define	N_Max		5000	//Max no. of events to be plotted in theta-phi polar plot
#define Det_Thres	20	//Analysis threshold for the detectors (ADC counts)

#define H_Landau_Min	0
#define H_Landau_Max	8000//1000//8000
#define H_Landau_No_Bin	600//100//300

#define BG_Min		0			//t_min (nsec) for background calculation
#define BG_Max		400//1500//400		//t_max (nsec) for background calculation
#define BG_No_Bin	(BG_Max-BG_Min)/2.5	//No. of bins for background calculation

#define	Window_Open	50	//START of the signal time window [: T_peak-Window_open] nsecs
#define	Window_Close	100	//END of the signal time window [: T_peak+Window_close] nsecs

#define H_PH_Min	0
#define H_PH_Max	500
#define H_PH_No_Bin	300

#define Theta_No_Bin	90	//No. of bins for theta distribution
#define Phi_No_Bin	360	//No. of bins for phi distribution
#define Lateral_No_Bin	300	//No. of R bins for lateral particle distribution
#define dt_Min		0.1	//Min value for dt distribution (in millisecs)
#define dt_Max		1000000	//Max value for dt distribution (in millisecs)
#define dt_No_Bin	100000	//No. of bins for dt distribution
#define	Hits_Min	0		//Min value for no. of hits
#define	Hits_Max	NO_DETs+1	//Max value for no. of hits
#define Hits_No_Bin	NO_DETs+1	//No. of bins for hits

#define H_PW_Min	0
#define H_PW_Max	50
#define H_PW_No_Bin	50
#define	vel_light	2.99792458e8	//velocity of light in m/sec.
#define	pi		3.14159265	//value of pi.

#define Muon_Count	400		//Single muon ADC count from the calibration
#define Det_Area	1		//Detector collection area in m^2
#define TRIGG_COND	5		//Minimum no. of detectors to be triggered to accept an event
#endif

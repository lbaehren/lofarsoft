#define On		1	//On switch
#define Off		0	//Off switch
#define Done		2	//Done switch

#define	Station_Trigger	2	//No. of stations to be fired for station trigger
#define NO_LASAs	5	//No. of stations
#define	mVolts2ADC	2	//voltage to ADC factor (1mV=2ADC)
#define HV_Upp_Limit	1700	//Upper limit of HV in volts
#define	NO_One_Sec	100	//No. of 1 sec messages to store in buffer
#define Store_One_Sec	5	//Storing one sec messages starts from the 5th sec
#define	NO_Event	100	//No. of data events to store in buffer
#define	One_Sec_Bytes	87	//Size of 1 sec messages in bytes
#define	Event_Bytes	12023	//Size of event data in bytes
#define	Control_Bytes	79	//Size of control messages (returned by HISPARC) in bytes
#define	Max_Event	1000	//Max. no. of event times to store together from all the LASAs
#define Spy_Event	4//20	//Spying every 'Spy_Event' total events from all LASAs
				//Make sure that spying is done before the event ring buffer is overwritten. For safety, put 'NO_Event'= at least 3*(Spy_Event/NO_LASAs)
#define Spy_Delay	0	//Spying 'Spy_Event' events from 'xxxth' event upto the last 'Spy_Delay'th event
#define Coin_Window	(long long unsigned)400	//Coincidence time window in nanosecs
#define	Monitor1	500//300	//Monitoring 1 (GUI) every n events/channel
#define	Monitor_rate	50//20	//Event interval to calculate event rate
#define	Hits_Display	3//5	//Time interval (min) for RECENT event hits display

//--------For background traces calculation----------
#define BG_Min		0			//t_min (nsec) for background calculation
#define BG_Max		400//1500			//t_max (nsec) for background calculation
#define BG_No_Bin	(BG_Max-BG_Min)/2.5	//No. of bins for background calculation
#define	Window_Open	50	//START of the signal time window [: T_peak-Window_open] nsecs
#define	Window_Close	100	//END of the signal time window [: T_peak+Window_close] nsecs

//-------For plotting--------------
#define Landau_Fit_Min	100	//Start point for the landau fitting of total count distri. 
#define Landau_Fit_Max	5000	//End point for the landau fitting of total count distri. 

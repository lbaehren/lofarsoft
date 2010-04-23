#define	Event_Bytes		12023		//Size of event data in bytes
#define	ADC_Full_Scale_Low	2030		//ADC Full Scale low
#define	ADC_Full_Scale_High	2060		//ADC Full Scale high
#define	ADC_Off_Low		2047		//ADC Offset low
#define	ADC_Off_High		2049		//ADC Offset high
#define	ADC_Common_Off_Low	149		//ADC Common offset low (use for gain calib)
#define	ADC_Common_Off_High	151		//ADC Common offset high (use for gain calib)
#define	ADC_Gain_Low		149		//ADC Gain low
#define	ADC_Gain_High		151		//ADC Gain high
//Final common offset value is set such that the average ADC sum on each channel is within this range
#define	Final_Common_Off_Low	12		//Final common offset value
#define	Final_Common_Off_High	13		//Use for setting final common offset value

char* Hostname="dwingeloo" ;
char* Master_Name="Master" ;
char* Master_Serial_No="FTQZTB7R" ;
int Master_Port_No=3301 ;

char* Slave_Name="Slave" ;
char* Slave_Serial_No="FTRCDY18" ;
int Slave_Port_No=3302 ;

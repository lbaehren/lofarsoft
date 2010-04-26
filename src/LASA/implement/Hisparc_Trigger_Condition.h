//This function returns the trigger condition for a Hisparc unit corresponding to the input trigger code "num". Ref: Lookup table provided in the HISPARC manual.
// 
char Trigger_Condition(unsigned short num)
{
	unsigned char trigger ;
	if(num==1) trigger=0x01 ; 
	if(num==2) trigger=0x02 ; 
	if(num==3) trigger=0x03 ; 
	if(num==4) trigger=0x04 ; 
	if(num==5) trigger=0x08 ; 
	if(num==6) trigger=0x09 ; 
	if(num==7) trigger=0x0A ; 
	if(num==8) trigger=0x0B ; 
	if(num==9) trigger=0x10 ; 
	if(num==10) trigger=0x11 ; 
	if(num==11) trigger=0x12 ; 
	if(num==12) trigger=0x18 ; 
	if(num==13) trigger=0x19 ; 
	if(num==14) trigger=0x20 ; 
	if(num==15) trigger=0x0C ; 
	if(num==16) trigger=0x0D ; 
	if(num==17) trigger=0x0E ; 
	if(num==18) trigger=0x0F ; 
	if(num==19) trigger=0x14 ; 
	if(num==20) trigger=0x15 ; 
	if(num==21) trigger=0x16 ; 
	if(num==22) trigger=0x17 ; 
	if(num==23) trigger=0x1C ; 
	if(num==24) trigger=0x1D ; 
	if(num==25) trigger=0x1E ; 
	if(num==26) trigger=0x1F ; 
	if(num==27) trigger=0x24 ; 
	if(num==28) trigger=0x25 ; 
	if(num==29) trigger=0x26 ; 
	if(num==30) trigger=0x27 ; 
	if(num==31) trigger=0x40 ;	//External trigger only 
	//if(num==32) trigger=0x48 ;	//External trigger with atleast one high signal
	if(num==32) trigger=0x41 ;	//External trigger with atleast one low signal
	if(num==33) trigger=0x80 ;	//Calibration mode
	
	if(num>33 || num<=0)
	{
		printf("Unknown Trigger Condition !!\n") ;
		exit(1) ;
	}
	return trigger ;
}

#######################################################################
#
# Functions and other stuff for the 40MHz bandwidth TIM-Board
#
########################################################################



#-----------------------------------------------------------------------
#Name: TIM40fgenEmptyRec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Creates the data field records and definiton for the different
#     fields for the TIM40f system.
#
#Ret: Record with fields for vital header information
#-----------------------------------------------------------------------

TIM40fgenEmptyRec := function() {
    rec:=[=];
    ci := TIM40coordinforec();

	ci['Dummy'].Stored:=F
	ci['f(x)'].Stored:=T

    rec:=TIM40finishcoordinforec(rec,ci)
	return rec;
}






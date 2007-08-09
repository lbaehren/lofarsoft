#This awk script converts the ITS data header to the LOPES tools header
#See the itstolopes script in this directory


BEGIN{
    months["Jan"]=1;
    months["Feb"]=2;
    months["Mar"]=3;
    months["Apr"]=4;
    months["May"]=5;
    months["Jun"]=6;
    months["Jul"]=7;
    months["Aug"]=8;
    months["Sep"]=9;
    months["Oct"]=10;
    months["Nov"]=11;
    months["Dec"]=12;
 
    nant=0;
    na=0;
    naf=0;
}


//{var=""}
/^#EOF/ {next}
/^[%]/ {nant=nant+1;  antposx[nant]=-$3; antposy[nant]=$2; antposz[nant]=$4}
/#[A-Z][A-Za-z_]*:/ { print $0 }
/^[a-z_A-Z]+=/ {sub("="," "); var=$1; $1=""; print "#ITS" var ": " $0}


#----------------
#Here start the more specific conversion lines
#----------------

#Extract Time and Date from a comment lines!!
#Note that this is not yet UTC!!!!
#This is for old data files prior to June 2004
/^# A.*quisition iteration/ {
print "#Time: " $9
print "#Timezone: +02"
printf "%s %4.4i%s%2.2i%s%2.2i%s%s\n","#Date:",$10,"/",months[$7],"/",$8,"/",$9 ".000"
}
/^triggertime.[0-9]+.=/ {sub("="," "); 
date=$2; 
gsub("-","/",date);
sub("T","/",date);
sub("Z","",date);
print "#Time: " substr(date,1,10); 
print "#Timezone: +00"
print "#Date:",date
}

#Create arrays with the antennas used: first field is empty, so start with $2
#and add "1" to the antenna number in order to start with antenna 1 and not 0
(var == "antennas") {for (i=1;na<=(NF-1);na+=1) {ant[na]=$(na+1)+1}}

#Extract filename and specify Antennaposition
(var == "file") {
sub(".+[/]",""); 
file=$0; 
naf=$0;
sub("^.*e0*","",naf); 
sub("\\..*dat *$","",naf); 
print "#AntPos:",antposy[naf] " , " antposx[naf] " , " antposz[naf];
print "#presync: ",0
print "#AntNum:",naf
print "#AntName:",naf
print "#Files: " file;
}



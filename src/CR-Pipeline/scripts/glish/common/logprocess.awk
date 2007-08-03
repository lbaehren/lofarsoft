#gawk -f ../glish/logprocess.awk ../Karlsruhe/scrt
BEGIN{FS="[ \t, =]+"}
/^ *[$][A-Za-z_]+ *=/ {
    sub(FS "$","");
    sub("[$]","",$1);
    var[$1,len]=NF-1; 
    for (i=2;i<=NF;i=i+1) {var[$1,i-1]=$i}
}
/#[A-Z][A-Za-z_]*:/ { 
    nvar=0; s=$0;
    while(match(s,"{*[$]([A-Za-z_]+)}*",a)) {
	nvar=nvar+1;
	svar[nvar]=a[1];
	cvar[nvar]=1;
	s=substr(s,RSTART+RLENGTH);
    }
    if (nvar==0) {print $0} 
    else {
	n=1;
	while(cvar[1]<=var[svar[1],len]) {
	    s=$0;
	    for (i=1;i<=nvar;i=i+1) {
		print "#" svar[i] ":",var[svar[i],cvar[i]]
		    gsub("{*[$]" svar[i] "}*",var[svar[i],cvar[i]],s)
		    }
	    print s;
	    cvar[nvar]+=1; j=nvar;
	    while (cvar[j]>var[svar[j],len] && j>1) {
		cvar[j]=1;
		j-=1;
		cvar[j]+=1}
	}
    }
}

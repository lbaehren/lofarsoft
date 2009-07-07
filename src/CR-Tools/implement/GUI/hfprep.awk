BEGIN {
    setdef=0; 
    setnew=0; 
    WORD="[A-Za-z][A-Za-z0-9]*"
    getline;
    print "//================================================================================"
    print "// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfprep.awk"
    print "//================================================================================"
    print "//     File was generated from " FILENAME " on " strftime() 
    print "//--------------------------------------------------------------------------------"
    print "//"
    print $0
} 

/^\$\$/ {next}

func readlines(ndel) {
    for (i=1; i<=ndel; i++) $i=""
    add=$0; sub("^ +","",add); sub("\\\\ *$","",add);
    ret=add;
    while ($0 ~ ".*\\\\ *$") {
	getline; print $0; add="\n" $0; sub("\\\\ *$","",add)
	ret=ret add;
    }
    return ret
}

{print}

func contains(A,elem) {
    for (i in A) if (A[i]==elem) return i
    return 0
}

func strcontains(str,elem) {
    split(str,elems,",")
    return contains(elems,elem)
}

func ary2str(A,sep) {
    ret=""
    for (i in A) ret=ret A[i] sep
    sub(sep "$","",ret)
    return ret
}

func delary(A) {
    for (i in A) delete A[i]
}

func addpar(par,val){
    NPAR[LASTREC,par]=NPAR[LASTREC,par]+1
    APARS[LASTREC]=APARS[LASTREC] par ","
    AVARS[LASTREC,par,NPAR[LASTREC,par]]=val
}

func process(inp) {
    out=inp
    plist=APARS[LASTREC]
    sub("[, ]+$","",plist)
    split(plist,LASTPARS,",")
    for (i in LASTPARS) {
	gsub("\\$" LASTPARS[i],AVARS[LASTREC,LASTPARS[i],1],out)
#	print i,LASTPARS[i],AVARS[LASTREC,LASTPARS[i],1],out
    }
# Now check for repetitions
    n=split(out,outlines,"\n")
    for (i=1;i<=n;i++) {
	line=outlines[i]
	m=match(line,"\\$\\*" WORD)
	if (m) {
	    par=substr(line,RSTART+2,RLENGTH-2)
	    nlines=NPAR[LASTREC,par]
	    for (j=1;j<=nlines;j++) {
		print substr(line,1,RSTART-1) AVARS[LASTREC,par,j] substr(line,RSTART+RLENGTH)  
	    }
	} else {
	    print line
	}
    }
}

func begin() {
    NRECS[LASTTYPE]=1+NRECS[LASTTYPE]
    ARECS[LASTTYPE,NRECS[LASTTYPE]]=LASTREC
    split(AINDEX[LASTTYPE],indxs,",")
    for (i in indxs) addpar(indxs[i],INDX[indxs[i]])
    process(ABEGIN[LASTTYPE])
}

/^ *\/\/ *\$DEFINE /{setdef=1;next}

# An ending comment line will terminate the definiton block and start the begin function 
/^ *--+\*\// && (setdef || setnew) {
    setdef=0;
    if (setnew) {
	setnew=0
	begin()
    }
}

/^ *\/\/ *\$NEW:/{setnew=1; 
    LASTTYPE=$2;
    delary(INDX)
    next}

/^ *\$INDEX:/ && (setdef) {
    NINDEX[$2]=NF-2; 
    for (i=3;i<=NF;i++) AINDEX[$2]=AINDEX[$2] $i "," ;
}

/^ *\$BEGIN:/ && (setdef) {type=$2; ABEGIN[type]=readlines(2)}
/^ *\$END:/ && (setdef) {type=$2;AEND[type]=readlines(2)}
/^ *\$PUBLISH:/ && (setdef) {type=$2;APUBLISH[type]=readlines(2)}


#/^ *\/\/ *\$BEGIN [a-zA-Z][a-zA-Z0-9]*/ && (!setnew) 

/^ *\/\/ *\$END [a-zA-Z][a-zA-Z0-9]*/ && (!setnew) {process(AEND[LASTTYPE])}
/^ *\/\/ *\$PUBLISH [a-zA-Z][a-zA-Z0-9]*/ && (!setnew) {
    LASTTYPE=$2
    for (ii=1; ii<=NRECS[LASTTYPE]; ii++) { 
	LASTREC=ARECS[LASTTYPE,ii]
	process(APUBLISH[LASTTYPE])
    }
}


(setnew) && /^ *[a-zA-Z][a-zA-Z0-9]*+:/ {
    sub(": *$","",$1);
    par=$1; val=readlines(1)
    indx=strcontains(AINDEX[LASTTYPE],par)
    if (indx) {
	INDX[par]=val
	LASTREC=LASTTYPE "," ary2str(INDX,",")
    } else {
	addpar(par,val)
    }
}


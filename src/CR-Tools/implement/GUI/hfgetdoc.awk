BEGIN{npar=0}

function blockset(s,indent,width){
    l=width
    prep=""
    while (length(s)>l) {
	m=match(substr(s,1,l)," [^ ]*$")
	if (m<1) {m=l}
	print prep substr(s,1,m)
	s=substr(s,m+1)
	prep = substr("                                                                                ",1,indent)
	l=width-indent
    }
    print prep s
}

/^\/\/\$SECTION:/ {
    gsub("\\/\\/\\$SECTION: *","")
    print "\nSECTION:", $0
    print "--------------------------------------------------"
}

/^[/ ]*#define +HFPP_FUNC_NAME +/ {
    funcname = $3
    npar=0;
    newfunc=1
}
/^#define +HFPP_FUNCDEF +/ { 
    s=$0; split(s,ary,"\"");
    funcdoc = ary[2]
}

/^#define +HFPP_PARDEF_[0-9]+ +/ { 
    s=$0; split(s,ary,"[()]"); 
    npar=npar+1;
    pars[npar,"Name"]=ary[4]
    pars[npar,"Type"]=ary[2]
    pars[npar,"Doc"]=ary[8]
} 

/^ *\/\*\!/ && (newfunc) {
    s=funcname "("
    if (npar>0) s=s pars[1,"Name"]
    for (i=2; i<=npar; i++) s= s ", " pars[i,"Name"]
    s=s ")"
    s=s substr("                     ",length(s)) " - " funcdoc
#    printf ("%s",s)
    blockset(s,25,78)
    print " "
    newfunc=0
}

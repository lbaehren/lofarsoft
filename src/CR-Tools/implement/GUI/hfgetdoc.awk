BEGIN{npar=0}

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
    s=s substr("                                  ",length(s)) " - " funcdoc
    print s
    newfunc=0
}

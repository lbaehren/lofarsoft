BEGIN{npar=0}

/^\/\/\$SECTION:/ {
    gsub("\\/\\/\\$SECTION: *","")
    section = $0
    print "\n#SECTION:", section
    print "#--------------------------------------------------\n"
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
    s=ary[8]
    sub("\"","",s)
    sub("\"$","",s)
    pars[npar,"Doc"]=s
} 

/ *\\brief / {next}
/ *\\param / {next}

/^ *\/\*\!/ && (newfunc) {
    adddoxygendoc=1
    doxydoc=""
    next
}

/^ *\*\// && (newfunc) {
    adddoxygendoc=0
    newfunc=0
    s="\nPYCRTOOLS - " section "\n"
    s=s "--------------------------------------------------\n"
    s=s funcdoc
    s=s "\n\nUsage as attribute: v." tolower(substr(funcname,2))  "("
    if (npar>1) s=s pars[2,"Name"]
    for (i=3; i<=npar; i++) s= s ", " pars[i,"Name"]
    s=s ")"
    s=s "\nUsage as function : " funcname  "("
    if (npar>0) s=s pars[1,"Name"]
    for (i=2; i<=npar; i++) s= s ", " pars[i,"Name"]
    s=s ")"
    for (i=1; i<=npar; i++) s= s "\n  - " pars[i,"Name"] ": " pars[i,"Doc"] 

    if (doxydoc != "") s=s "\n\n" doxydoc
    doxydoc=""
    print funcname ".__doc__=\"\"\"" s "\"\"\"\n" 
    next
}

(adddoxygendoc) {doxydoc = doxydoc $0}


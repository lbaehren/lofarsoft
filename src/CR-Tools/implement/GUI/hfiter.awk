BEGIN{true=1; false=0; iter=false}

/^\/\/\$DOCSTRING: / {docstring = $0; sub("\\/\\/\\$DOCSTRING: +","",docstring); print; next}

/^ *\$PARDOCSTRING/ && (funcdoc) {print commentdoc; commentdoc=""; funcdoc=0; next}

{ 
    gsub("\\$DOCSTRING",docstring)
}

(!iter) {print}

/^#define +HFPP_PARDEF_0 +/ {commentdoc=""}

/^#define +HFPP_PARDEF_[0-9]+ +/ { 
    s=$0; gsub("\"","",s); split(s,ary,"[()]"); 
    commentdoc=commentdoc "\n    \\param " ary[4] ": " ary[8] "\n"
    funcdoc=1
} 


/^\/\/\$ITERATE [a-zA-Z][a-zA-Z0-9]*/ {
    iter=true
    ivar=$2
    $1=""; $2=""
    sub(" +","")
    split($0,idxs,",")
    inp=""
    next
}

/^\/\/\$ENDITERATE/ {
    iter=false
    inp=substr(inp,1,length(inp)-1)
    for (i in idxs) {
	out = inp
	idxsicap=toupper(substr(idxs[i],1,1)) substr(idxs[i],2) 
	gsub("{?\\$" ivar "!CAPS}?",idxsicap,out)
	gsub("{?\\$" ivar "}?",idxs[i],out)
	print out
    }
    print $0
}

(iter==true) {
    inp = inp  $0 "\n"
}

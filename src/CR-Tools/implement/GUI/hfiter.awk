BEGIN{true=1; false=0; iter=false; inp=""}

/^\/\/\$DOCSTRING1: / {docstring1 = $0; sub("\\/\\/\\$DOCSTRING1: +","",docstring1); print; next}
/^\/\/\$DOCSTRING2: / {docstring2 = $0; sub("\\/\\/\\$DOCSTRING2: +","",docstring2); print; next}

{ 
    gsub("\\$DOCSTRING1",docstring1)
    gsub("\\$DOCSTRING2",docstring2)
}

(!iter) {print}



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
	gsub("\\$" ivar "!CAPS",idxsicap,out)
	gsub("\\$" ivar,idxs[i],out)
	print out
    }
    print $0
}

(iter==true) {
    inp = inp  $0 "\n"
}

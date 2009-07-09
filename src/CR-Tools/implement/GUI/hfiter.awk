BEGIN{true=1; false=0; iter=false; inp=""}

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
	gsub("\\$" ivar,idxs[i],out)
	print out
    }
    print $0
}

(iter==true) {
    inp = inp  $0 "\n"
}

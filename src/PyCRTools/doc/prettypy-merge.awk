BEGIN{tmpfile="prettypy.tmp"; OK=1}

/^%%MERGE: / {
    file="prettypy." $2
    while (getline line < file) {print line}
    next
}

/^%%LINE: [1-9]/ {
    while (!match(tmp,$0) && OK) { #skip forward to correct line
	OK=(getline tmp < tmpfile)
    }
    while ((getline tmp < tmpfile) && !match(tmp,"^%%LINE: [1-9]")) {
	print "  " tmp;
    }
    next
}

{print}

    
    
    
BEGIN{
    command=1; 
    begin=1; 
    LINE=0;
    SECTION1=0; TOC=""; 
    input = "prettypy.inp"
    process="/sw/bin/python2.6 -u -i $LOFARSOFT/src/CR-Tools/implement/Pypeline/modules/pycrtools.py > prettypy.tmp"
}


/^#%SKIP/ {skip=1} # Skipping producing output from the python command

/^#/ {next}

/"""/ {
    if (command && !begin) {
	if (skip) {skip=0}
	else {print "%%LINE:",LINE} #finish off previous command
    }
    if (command) {command=0}
    else {command=1}
    begin=0
    next 
}

(begin) {
    if (TOC==""){
	TOC="prettypy.toc"
	print "Table of Contents\n=================\n" > TOC
    }
    print $0 |& process
    next}

#Here come the actual commands ...
(command) && /^    / { # this is an indented command, so just pass it on without a line number
	    print "... " $0
	    print $0 >> input
	    print $0 |& process
	    next
	}

(command) {
    if (skip) {skip=0}
    else if (command>1) {print "%%LINE:",LINE} #finish off previous command
    command++
    LINE++
    print "\nprint '%%LINE:'," LINE  >> input
    print "\nprint '%%LINE:'," LINE  |& process
    print $0 >> input
    print $0 |& process
}

/^ *p_\(/{next} # take out the printing commands
/^ *savefigure\(/{next} # take out the savefigure commands
/^ *$/ && (command) {print; next}

(command) {print ">>> " $0; next}



##Now replace Section markers with actual Section numbers
/^ *\(\+\)/ {SECTION1++; sub(" *\\(\\+\\)",SECTION1 ". "); SECTION2=0; print >> TOC}
/^ *\(\+\+\)/ {SECTION2++; sub(" *\\(\\+\\+\\)",SECTION1 "." SECTION2 ". "); SECTION3=0; print >> TOC}
/^ *\(\+\+\+\)/ {SECTION3++; sub(" *\\(\\+\\+\\+\\)",SECTION1 "." SECTION2 "." SECTION3 ". " ); SECTION4=0; print >> TOC}

#print the input line to output
{print}

END{
    print "\n" >> TOC
    close(process)
}

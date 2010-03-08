BEGIN{
    comment=0; 
    begin=1; 
    SECTION1=0; TOC=""; 
    process="/sw/bin/python2.6 -u -i /Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/pycrtools.py > tmp.txt"
}

/"""/{
    comment = ! comment; 
    begin=0; 
    next 
}

(begin) {
    if (TOC==""){
	TOC=FILENAME ".toc"
	print "Table of Contents\n=================\n" > TOC
    }
    print $0 |& process
    next}

#Here come the actual commands ...
(!comment) { 
    print "$$$>>> " $0
    print $0 |& process
    LINE++
    print ">>> LINE:",LINE,$0 >> "tmp.txt" 
    next
}

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

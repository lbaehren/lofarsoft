import sys
import re

def table(filename, caption="", justification="l", line="|"):
    file = open(filename, "r")
    row1 = file.readline()
    columns = len(row1.split(" "))
    file = open(filename, "r")
    print "\\begin{table}[h]"
    print "\\footnotesize"
    print "\\centering"
    print "\\caption{%s}" % caption
    print "\\label{tab:%s}" % "-".join(caption.split())
    if line != None:
        spec = line +((justification+line)*columns)
    else:
        spec = (justification*columns)
    print "\\begin{tabular}{%s}" % spec
    if line != None:
        print "\\hline"
    for l in file.readlines():
        w = l.strip().split(" ")
        if len(w) != columns:
            raise "column count mismatch in line:",l
        print "&".join(w) + "\\\\"
        if line != None:
            print "\\hline"
    print "\\end{tabular}"
    print "\\end{table}"

if __name__ == "__main__":
    table(sys.argv[1], caption = sys.argv[2])

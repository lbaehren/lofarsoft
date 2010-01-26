#gawk -f hfsplit2h.awk hfpp-test.cc
BEGIN{FS="[ :=]+";WRITETOFILE=0}
/^ *[/]* *\$FILENAME: / {
    gsub("\\$SELF",FILENAME,$3); 
    FILENAMES[$2]=$3; 
    print "//$ --- Splitting off code to file " $2 "=\"" $3 "\""}
/^ *[/]* *\$COPY_TO +[A-Z]+ +START:*/ {
    COPYTOFILE=1; 
    FILE=FILENAMES[$2]; 
    print "//==================================================================================" > FILE
    print "// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk" >> FILE
    print "//==================================================================================" >> FILE
    print "//     File was generated from " FILENAME " on " strftime()  >> FILE
    print "//----------------------------------------------------------------------------------"  >> FILE
    print "//" >> FILE
    next
}
/^ *[/]* *\$COPY_TO +END/ {COPYTOFILE=0;next}
/^ *[/]* *\$COPY_TO +[A-Z]+:/ {print substr($0,match($0,": *")+1) >>FILENAMES[$2]}
(COPYTOFILE) {print $0 >> FILE}
{print}

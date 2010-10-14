#gawk -f hfsplit2h.awk hfpp-test.cc
#    print "//     File was generated from " FILENAME " on " strftime()  >> FILE
BEGIN{FS="[ :=]+";WRITETOFILE=0}
/^ *[/]* *\$FILENAME: / {
    gsub("\\$SELF",FILENAME,$3);
    FILENAMES[$2]=$3;
    print "//$ --- Splitting off code to file " $2 "=\"" $3 "\""}
/^ *[/]* *\$COPY_TO +[A-Z]+ +START:*/ {
    COPYTOFILE=1;
    FILE=FILENAMES[$2]".new";
    print "//==================================================================================" > FILE
    print "// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY hfsplit2h.awk" >> FILE
    print "//==================================================================================" >> FILE
    print "//     File was generated from " FILENAME >> FILE
    print "//----------------------------------------------------------------------------------"  >> FILE
    print "//" >> FILE
    next
}
/^ *[/]* *\$COPY_TO +END/ {COPYTOFILE=0;next}
/^ *[/]* *\$COPY_TO +[A-Z]+:/ {FILE0=FILENAMES[$2]; sub(" *[/]* *\\$COPY_TO +[A-Z]+: *",""); print >> FILE0".new"}
(COPYTOFILE) {print $0 >> FILE}
{print}

m#gawk -f ../makedoc.awk data-kern.g > scrt

BEGIN{
    n=0; 
    out=0; 
    newblock=0; 
    npar=0;
    dir="Doc/";
    ext="g";
    extp="." ext;
    filestart=1;
}

(NR==1) {
    file = FILENAME;
    sub("\\." "g$","",file);
    sfile0=file ".src.html";
    dfile0=file ".doc.html";
    ifile0=file ".idx.html";
    aifile0=file ".aidx.html";
    afile0="alphabeticindex.html";
    sfile=dir sfile0;
    dfile=dir dfile0;
    ifile=dir ifile0;
    aifile=dir aifile0;
    print "<html><head><title>" file extp " Documentation</title></head><body><h1><a name=top>Documentation of Functions and Variables</a> in <a href=../" file extp ">" file "</a></h1>" > dfile;
    print "<htm><head><title>" file extp " Source File</title></head><body>" > sfile;
    print "<htm><head><title>" file extp " Index File</title></head><body><h1><a name=top>Index of Functions and Variables</a> in <a href=../" file extp ">" file "</a></h1><ul>" > ifile;
#    print sfile, dfile, ifile
}

{s=$0; 
 gsub(" ","\\&nbsp;",s);
 printf ("%s%s%s%4.4i%s: ","<tt><a href=../",FILENAME,">",NR,"</a>") >> sfile;;
 if ($1=="#Name:") {
    print "<a name=" $2 ">#Name: </a><a href=" dfile0 "#" $2 ">" $2 "</a><br>" >> sfile;
    filler=substr("                              ",1,30-length($2));
    gsub(" ","\\&nbsp;",filler);
    print "<li><a name=" $2 "></a><tt><a href=" dfile0 "#" $2 ">" $2 "</a>" filler "</tt> (source code in file <i><a href=" sfile0 "#" $2 ">\"" file extp  "\"</a></i>)<br>" >> ifile;
 }
 else {print s "</tt><br>" >> sfile;};
}


/^[\t ]*$/{next}

/^\#Section:/{
  $1=""; 
  filestart=1; 
  print "<hr><hr><font color=\"#0000FF\"><h1>"o $0 "</h1></font>" >> dfile; 
  print "<hr><hr><font color=\"#0000FF\"><h1>"o $0 "</h1></font>" >> ifile; 
  next;
};


/^#[A-Z][A-Za-z]*[:]/ {
filestart=0;
sub("[#]","");
sub(":",""); 
par=$1; 
n=1; 
if (!newblock) {npar=0; delete pref; delete parlist; delete line};
npar=npar+1;
parlist[npar]=par;
pref[par]=npar
sub(par " *",""); 
line[npar]=$0;
parlist[npar]=par;
newblock=1;
next; 
}

/^[#][^-]+/ {
if (newblock) {sub ("^[#]+",""); n+=1;line[npar]=line[npar] "\n" $0; next};
}

/^ *[^#]/ {
if (newblock) {
print "<hr>" >> dfile;   
print "[<a href=index.html>Home</a>, <a href=" ifile0 ">Index of " FILENAME "</a>,  <a href=" aifile0 ">Alphabetic Index of " FILENAME "</a>, <a href=" afile0 ">Alphabetic index of all files</a>, <a href=" dfile0 ">Top</a>]" >> dfile;   

printf ("%s", "<h2><a name=" line[pref["Name"]] "></a><a href=" sfile0 "#" line[pref["Name"]] ">" line[pref["Name"]] "</a>") >> dfile;

if (pref["Type"]) {
    if (pref["Type"]) printf ("%s", " (" tolower(line[pref["Type"]]) ")") >> dfile;
  }
  print "</h2>" >> dfile;

  print "<i>" line[pref["Doc"]]  "</i>" >> dfile;

if (pref["Created"]) {
print "<p><b>Created:</b> " line[pref["Created"]] "<p>" >> dfile;
}

if (pref["Example"]) {
print "<p><b>Example:</b><br><tt>" line[pref["Example"]] "</tt><p>" >> dfile;
}

if (pref["Ret"]) {
print "<h3>Returns:</h3>" line[pref["Ret"]] >> dfile;
}

# List of function parameters

if (pref["Par"]) {
  print "<h3>Parameters:</h3><ul>" >> dfile
}
else {
  print "<br>" >> dfile
};

for (i=1; i<=npar; i+=1) {
  p=parlist[i];
  if (p=="Par") {
    match(line[i],"[ \t][-][ \t]"); 
    pnam1=substr(line[i],1,RSTART);
    pdoc=substr(line[i],RSTART+RLENGTH);
    nn=split(pnam1,pnam2," *= *");
    print "<li><b>" pnam2[1] "</b>" >> dfile; 
    if (nn>1) print " = " pnam2[2] >> dfile;
    print "<br>" pdoc >> dfile;
  }
}
print "</ul>" >> dfile;

# List of references to other functions

if (pref["Ref"]) {
  print "<p><font size=+1><b>References:</b> </font>" >> dfile;
  nrefs=split(line[pref["Ref"]],refs,"[ ,]+");
  for (i=1; i<=nrefs; i+=1) {   
    if (i<nrefs) {print "<a href=" afile0 "#" refs[i] ">" refs[i] "</a>" >> dfile; } 
    else {print "<a href=" afile0 "#" refs[i] ">" refs[i] "</a>"  >> dfile ;}; 
  };
  print "<p>" >> dfile;
};

# File/code status and Todo list

if (pref["Status"]) {
print "<p><b>Status:</b> " line[pref["Status"]] "<p>" >> dfile;
}

if (pref["Todo"]) {
print "<h3>Todo:</h3>" line[pref["Todo"]] "<p>" >> dfile;
}

print "<b><font size=-2>File:</b> <a href=" sfile0 "#" line[pref["Name"]] ">\"" file extp  "\"</a></font>" >> dfile;
  newblock=0;
} 
}

/^[\t ]*\#+/{
if (filestart) {
  sub("^[\t ]*#+","");
  sub("#+[\t ]*$","");
#  gsub(" ","\\&nbsp;");
  gsub("---+","");
  empty=match($0,"^[\t ]*$");
  if (empty && !parprinted) {
     parprinted=1; 
     print("<p>") >> dfile;
  } else {
       if (!empty) {
          parprinted=0; 
          print $0 >> dfile;
      }
  }
}
}



(nbrackets>0) {
split($0,a,"\\{");
nbrackets1=split(a[1],b,"\\(")-1+nbrackets1;
nbrackets2=split(a[1],b,"\\)")-1+nbrackets2;
printf("%s", a[1]) >> dfile;
if (nbrackets1>nbrackets2) {
  nbrackets=nbrackets1-nbrackets2;
} else {
  nbrackets=0;
  print "</tt><p>" >> dfile;};
}

/[A-Za-z_]+[^ ]* *:= *function/ {
filestart=0;
split($0,a,"[ \t]*:=[ \t]*");
name=a[1];
sub("^ *rec\\.","data.",name);
sub("[ \t]*function[ \t]*",name,a[2]);
split(a[2],b,"\\{");
print "<h3>Default Example:</h3>" >> dfile;
printf("%s", "<tt>" b[1] " ") >> dfile;
nbrackets1=split(b[1],c,"\\(")-1;
nbrackets2=split(b[1],c,"\\)")-1;
if (nbrackets1>nbrackets2) {
    nbrackets=nbrackets1-nbrackets2;
} else {
  print "</tt><p>" >> dfile;
};
}


 END{
    print "</body></html>" >> dfile;
    print "</body></html>" >> sfile;
    print "</ul></body></html>" >> ifile;
    close(dfile);
    close(sfile);
    close(ifile);
}

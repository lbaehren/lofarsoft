
#The script prints a list of bright radio sources and their positions
#above the LOFAR radio telescope in CASA!!
#------------------------------------------------------------------------
lofarpos=me.position(rf="WGS84",v0=qa.quantity("+06.52.08.18"),v1=qa.quantity("+52.54.31.55"),v2=qa.quantity("5m")) #Check position!!!
me.doframe(lofarpos)
me.framenow()  # select current date and time

#uncomment the following lines if you want to set a specific date
#date="2010/07/08"  
#utctime="15:08:00" 
#me.doframe(me.epoch("UTC",date+"/"+utctime))
#------------------------------------------------------------------------
#me.doframe(me.observatory('WSRT'))
#me.doframe(me.observatory('LOFAR'))
#------------------------------------------------------------------------

def meprettydms(q):
    """ Returns a quanta in a pretty dms form, i.e.: DDd MM' SS.dd"  "  """
    l=qa.formxxx(q,"dms").split(".")
    return l[0]+'d '+l[1]+"' "+l[2]+"."+l[3]+'"'


def meprettyhms(q):
    """ Returns a quanta in a pretty hms form, i.e.: HHh MM' SS.dd"  " """
    l=qa.formxxx(q,"hms").split(":")
    return l[0]+'h '+l[1]+"' "+l[2]+'"'
        

def meprettystring(mes):
    """Return a readable string of a direction or position measure.

    Example: meprettystring(me.source("0002-478"))
    """
    if mes["type"]=="position":
        if qa.convert(mes["m1"],"deg")["value"] > 0: NS=" N "
        else:  NS=" S "
        ew=qa.convert(mes["m0"],"deg")["value"]
        if (ew >= 0) & (ew<180): EW=" E "
        else:  EW=" W "
        s=" long = "+ meprettydms(mes["m0"])+EW+" lat = "+meprettydms(mes["m1"])+NS+" height = "+qa.tos(mes["m2"])+" ("+mes["refer"]+")"
    elif mes["type"]=="direction":
        if mes["refer"] in ['J2000', 'JMEAN', 'JTRUE', 'APP', 'B1950', 'B1950_VLA', 'BMEAN','BTRUE','ICRS']:
            s="RA = "+ meprettyhms(mes["m0"])+" DEC = "+meprettydms(mes["m1"])+" ("+mes["refer"]+")"
        elif mes["refer"] in ['GALACTIC']:
            s="l = "+ meprettydms(mes["m0"])+" b = "+meprettydms(mes["m1"])+" ("+mes["refer"]+") check order of l/b ...!!!"
        elif mes["refer"] in [ 'AZEL', 'AZELSW', 'AZELNE', 'AZELGEO','AZELSWGEO', 'AZELNEGEO']:
            s="AZ = "+ meprettyhms(mes["m0"])+" EL = "+meprettydms(mes["m1"])+" ("+mes["refer"]+")"
        else:
            s="ERROR: Code "+mes["refer"] + " unknown!"
    if mes["type"]=="epoch":
        s=qa.time(mes["m0"],form=["ymd","local"])+" (local time)"  # local time  
    return s


def meprint(mes):
    """Return a readable string of a direction or position measure.

    Example: meprettystring(me.source("0002-478"))
    """
    print meprettystring(mes)

def sourceinfostr(name,dir):
    azel=me.measure(dir,"AZEL")
    if azel["m1"]["value"]>0: up=" UP"
    else: up=""
    return (name+"            ")[0:12]+": "+meprettystring(me.measure(dir,"J2000"))+" | "+meprettystring(azel)+" | "+meriseset(dir)+up

def meriseset(dir):
    rs=me.riseset(dir)
    if rs["rise"]=="above": return "    circumpolar    "
    if rs["rise"]=="below": return "    not visible    "
    return qa.time(rs["rise"]["utc"]["m0"],form="local") + " - "+qa.time(rs["set"]["utc"]["m0"],form="local")

def listsources(planets=["Sun","Moon","Jupiter","Saturn"]):
    print "LOCATION, RISE & SET TIMES for current frame"
    print "----------------------------------"
    print me.showframe()
    print "\n Rise and set times in local time."
    for p in planets:
        print sourceinfostr(p,me.direction(p))
    l=hfradiosources.keys()
    l.sort()
    for s in l:
        print sourceinfostr(s,hfradiosources[s]["direction"])
        
sourceliststringall="""TYCHO SNR,                      00h25m14.0s +64d08m39s 
87GB[BWE91] 0022+6351,          00h25m21.0s +64d08m24s 
3C 58,                         02h05m35.2s +64d49m35s 
4C +61.05,                      02h25m44.1s +62d06m08s 
PERSEUS A,                      03h19m48.1s +41d30m42s 
MG2 J053411+2201,               05h34m11.6s +22d01m53s 
MG2 J053430+2204,               05h34m30.5s +22d04m22s 
MESSIER 1,                    05h34m32.0s +22d00m52s 
MG2 J053432+2157,               05h34m32.9s +21d57m55s 
MG2 J053439+2203,               05h34m39.5s +22d03m53s 
MG2 J053450+2200,               05h34m54.6s +22d00m17s 
NGC 2070,                 05h38m44.4s -69d04m39s 
3C 147.1,                       05h41m42.1s -01d53m55s 
PKS 0857-43,                    08h59m27.1s -43d45m09s 
ESO 212-EN 006,                 09h24m24.2s -51d59m29s 
PMN J1006-5712,                 10h06m39.0s -57d12m14s 
NGC 3581,                       11h12m01.9s -61d18m07s 
PMN J1209-6249,                 12h09m59.8s -62d49m38s 
3C 273,                         12h29m06.7s +02d03m09s 
MESSIER 87,                    12h30m49.4s +12d23m28s 
CEN A,                          13h25m27.6s -43d01m09s 
PMN J1325-4257,                 13h25m44.5s -42d57m39s 
PKS 1343-60,                    13h46m49.0s -60d24m29s 
PMN J1445-5949,                 14h45m16.2s -59d49m31s 
PMN J1553-5435,                 15h53m03.1s -54d35m18s 
PMN J1621-5026,                 16h21m31.6s -50d26m19s 
PKS 1618-49,                    16h22m12.0s -50d05m51s 
ESO 332-PN 023,                 17h09m34.3s -41d35m41s 
3C 353,                         17h20m28.1s -00d58m47s 
PMN J1809-2019,                 18h09m25.3s -20d19m08s 
PMN J1814-1755,                 18h14m15.1s -17d55m51s 
MRC 1828-021,                   18h31m25.8s -02d05m51s 
87GB 190753.4+090111,           19h10m14.7s +09d06m19s 
PKS 1921+14,                    19h23m42.9s +14d30m08s 
Cygnus A,                       19h59m28.3s +40d44m02s 
3C 454.3,                       22h53m57.7s +16d08m54s 
NGC 7538,                       23h13m38.6s +61d30m45s 
Cas A,                          23h23m27.9s +58d48m42s"""

sourceliststring="""TYCHO SNR,                      00h25m14.0s +64d08m39s 
87GB[BWE91] 0022+6351,          00h25m21.0s +64d08m24s 
3C 58,                          02h05m35.2s +64d49m35s 
4C +61.05,                      02h25m44.1s +62d06m08s 
PERSEUS A,                      03h19m48.1s +41d30m42s 
M1,                      05h34m32.0s +22d00m52s 
3C 147.1,                       05h41m42.1s -01d53m55s 
3C 273,                         12h29m06.7s +02d03m09s 
M87,                     12h30m49.4s +12d23m28s 
3C 353,                         17h20m28.1s -00d58m47s 
MRC 1828-021,                   18h31m25.8s -02d05m51s 
87GB 190753.4+090111,           19h10m14.7s +09d06m19s 
PKS 1921+14,                    19h23m42.9s +14d30m08s 
Cygnus A,                       19h59m28.3s +40d44m02s 
3C 454.3,                       22h53m57.7s +16d08m54s 
NGC 7538,                       23h13m38.6s +61d30m45s 
Cas A,                          23h23m27.9s +58d48m42s
Sgr A*,                         17h45m40.0409 -29d00m28.118s"""

hfradiosources={}
for s in sourceliststring.split("\n"):
    l=s.split(",")
    name=l[0]
    dirs=l[1].split();
    info={};
    info["direction"]=me.direction("J2000",dirs[0],dirs[1])
    hfradiosources[name]=info

print "Type listsources() to get a list of the brightest radio sources and planets."
listsources()

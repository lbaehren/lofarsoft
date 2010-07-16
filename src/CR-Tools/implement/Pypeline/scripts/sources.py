
#The script prints a list of bright radio sources and their positions
#above the LOFAR radio telescope in CASA!!
#------------------------------------------------------------------------
lofarpos=me.position(rf="WGS84",v0=qa.quantity("+06.52.11.415"),v1=qa.quantity("+52.54.54.44098"),v2=qa.quantity("49.344m"))
me.doframe(lofarpos)
me.framenow()  # select current date and time

#uncomment the following lines if you want to set a specific date
date="2012/09/26"  
utctime="12:00:00" 
#me.doframe(me.epoch("UTC",date+"/"+utctime))
#------------------------------------------------------------------------
#me.doframe(me.observatory('WSRT'))
#me.doframe(me.observatory('LOFAR'))
#------------------------------------------------------------------------
#You can run the following in order to add LOFAR to the data repository
#This needs to be done only once.
#You can then say
#
#me.doframe(me.observatory("LOFAR"))
#
execfile("/Applications/CASA.app/Contents/Resources/python/simutil.py")

def addobservatory(name,coordinate):
    """Add a new observatory to the aips data repository, or change the coordinates of an existing one
    Example:

    execfile("/Applications/CASA.app/Contents/Resources/python/simutil.py")
    lmtpos=me.position(rf="WGS84",v0=qa.quantity("-97.18.53"),v1=qa.quantity("+18.59.06"),v2=qa.quantity("4600m"))
    addobservatory("LMT",lmtpos)
    """
    obstable = os.getenv("CASAPATH").split(' ')[0]+"/data/geodetic/Observatories"
    tb.open(obstable,nomodify=False)
    pos=me.measure(coordinate,'WGS84')
    coor=simutil().long2xyz(pos["m0"]["value"],pos["m1"]["value"],pos["m2"]["value"],"WGS84")
    names=list(tb.getcol("Name"))
    if name in names:
        n=names.index(name)
    else:
        tb.addrows()
        n=tb.nrows()-1
        tb.putcell("Name",n,name)
    tb.putcell("MJD",n,int(qa.time(qa.quantity("today"),form=["MJD","no_time"])))
    tb.putcell("Type",n,"WGS84")
    tb.putcell("Long",n,qa.convert(pos["m0"],"deg")["value"])
    tb.putcell("Lat",n,qa.convert(pos["m1"],"deg")["value"])
    tb.putcell("Height",n,qa.convert(pos["m2"],"m")["value"])
    tb.putcell("X",n,coor[0])
    tb.putcell("Y",n,coor[1])
    tb.putcell("Z",n,coor[2])
    tb.putcell("Source",n,"Heino Falcke")
    tb.putcell("Comment",n,"automatically put in by script")
    tb.close()

#addobservatory("LMT",me.position(rf="WGS84",v0=qa.quantity("-97.18.53"),v1=qa.quantity("+18.59.06"),v2=qa.quantity("4600m")))
#addobservatory("LOFAR",me.position(rf="WGS84",v0=qa.quantity("+06.52.11.415"),v1=qa.quantity("+52.54.54.44098"),v2=qa.quantity("49.344m")))
#addobservatory("HESS",me.position(rf="WGS84",v0=qa.quantity("16.500270deg"),v1=qa.quantity("-23.271180deg"),v2=qa.quantity("1813m")))


#addobservatory("LLAMA1",me.position(rf="WGS84",v0=qa.quantity("-67.336290deg"),v1=qa.quantity("-24.657967deg"),v2=qa.quantity("3664m")))
#addobservatory("LLAMA2",me.position(rf="WGS84",v0=qa.quantity("-66.452405deg"),v1=qa.quantity("-24.222499deg"),v2=qa.quantity("4755m")))
#addobservatory("LLAMA3",me.position(rf="WGS84",v0=qa.quantity("-65.729295deg"),v1=qa.quantity("-23.205519deg"),v2=qa.quantity("4900m")))
#addobservatory("SOUTHPOLE",me.position(rf="WGS84",v0=qa.quantity("0deg"),v1=qa.quantity("-90deg"),v2=qa.quantity("2800m")))
#------------------------------------------------------------------------


def meframedate():
    """Return a quantity with the date of the current frame"""
    x=me.showframe()
    return qa.quantity(str(float(x[14:19])+(float(x[21:23])+float(x[24:26])/60+float(x[27:34])/3600)/24)+"d")

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
    print "OBSERVING DATE:",qa.time(meframedate(),form=["dmy","local"]),"\n"
    print me.showframe()
    print "\n Rise and set times in local time."
    for p in planets:
        print sourceinfostr(p,me.direction(p))
    l=hfradiosources.keys()
    l.sort()
    for s in l:
        print sourceinfostr(s,hfradiosources[s]["direction"])

class KML_File:
    """For creating KML files used for Google Earth.
    _author__ = "Jon Goodall <jon.goodall@gmail.com> - http://www.duke.edu/~jgl34 - modified by H.Falcke"
    __version__ = "0.0.2"
    __license__ = ""
    __copyright__ =""
    """
    def __init__(self, filepath,scale=1.0,iconcolor="ff00f00f",linecolor="73ffffff"):
        self.filepath = filepath
        "adds the kml header to a file (includes a default style)"
        file = open(filepath,"w")
        file.write(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"\
        "<kml xmlns=\"http://earth.google.com/kml/2.0\">\n"\
        "<Document>\n"\
        "<Style id='normalPlaceMarker'>\n"\
"""<IconStyle>
<color>"""+iconcolor+"""</color>
<scale>"""+str(scale)+"""</scale>
<Icon>
<href>icons/low.png</href>
</Icon>
</IconStyle>
<LineStyle>
<color>"""+linecolor+"""</color>
<width>3.5</width>
</LineStyle>
<LabelStyle>
<scale>"""+str(scale)+"""</scale>
</LabelStyle>
"""\
        "</Style>\n")
        file.close()

    def close(self):
        file = open(self.filepath,"a")
        file.write(
        "</Document>\n"\
        "</kml>")
        file.close()
        
    def open_folder(self, name):
        file = open(self.filepath,"a")
        file.write(
        "<Folder>\n"\
        "  <name>" + name + "</name>\n")
        file.close()

    def close_folder(self):
        file = open(self.filepath,"a")
        file.write(
        "</Folder>\n")
        file.close()
        
    def add_placemarker(self, latitude, longitude, altitude = 0.0, description = " ", name = " ", range = 6000, tilt = 45, heading = 0):
        "adds the point to a kml file"
        file = open(self.filepath,"a")
        file.write(
        "<Placemark>\n"\
        "  <description>" + description + "</description>\n"\
        "  <name>" + name + "</name>\n"\
        "  <styleUrl>#normalPlaceMarker</styleUrl>" + 
        "  <LookAt>\n"\
        "    <longitude>" + str(longitude) + "</longitude>\n"\
        "    <latitude>" + str(latitude) + "</latitude>\n"\
        "    <range>" + str(range) + "</range>\n"\
        "    <tilt>" + str(tilt) + "</tilt>\n"\
        "    <heading>" + str(heading) + "</heading>\n"\
        "  </LookAt>\n"\
        "  <visibility>0</visibility>\n"\
        "   <Point>\n"\
        "    <extrude>1</extrude>\n"\
        "    <altitudeMode>relativeToGround</altitudeMode>\n"\
        "    <coordinates>" + str(longitude) + "," + str(latitude) +", " +  str(altitude) + "</coordinates>\n"\
        "   </Point>\n"\
        " </Placemark>\n")
        file.close()


def makeobservatorylist(filename,observatories,size=1):
    """
    Creates a .cfg file that casa uses for simulated observations (simdata2).

    Example:
    observatories=[("ALMA", 12), ("CARMA",12), ("SMT",10), ("EHT1",12), ("VLT",12), ("NTT",12),  ("LLAMA1",12), ("LLAMA2",12), ("LLAMA3",12), ("LMT",12)]
    makeobservatorylist("eht.cfg",observatories,size=1)
    """
    f=open(filename,"w")
    f.write("# observatory=ALMA\n# coordsys=XYZ\n")
    util=simutil()
    for i in observatories:
        obs=me.measure(me.observatory(i[0]),'WGS84')
        coor=util.long2xyz(obs["m0"]["value"],obs["m1"]["value"],obs["m2"]["value"],"WGS84")
        f.write(str(coor[0]) +" "+str(coor[1])+" "+str(coor[2])+" "+str(i[1]*size)+" "+str(i[0])+"\n")
        print str(coor[0]) +" "+str(coor[1])+" "+str(coor[2])+" "+str(i[1]*size)+" "+str(i[0])
    f.close()
    print "Wrote observatories to ",filename


def megoogleearth(filename,position,scale=1.0,iconcolor="ff00f00f",linecolor="73ffffff"):
    """
    The scripts takes a list of positions (as measures) or observatory
    names defined in casa and writes them inot a .kml file that you
    can import into Google Earth and see the placemarks. A single
    position can also be given without a list.

    Named parameters:
    scale=1.0  - scale the size of text and icon
    iconcolor="ff00f00f", linecolor="73ffffff" - colors of icons and lines
    
    Example:
    
    megoogleearth("eht.kml",['ALMA', 'CARMA', 'SMT', 'JCMT', 'EHT1', 'LLAMA1', 'LLAMA3', 'LMT', 'SOUTHPOLE'],scale=3.5)
    """
    if not type(position) in [list,set,tuple]: position=[position]
    f=KML_File(filename,scale=scale,iconcolor=iconcolor,linecolor=linecolor)
    #f.open_folder("eht")
    for pos in position:
        if type(pos)==str: p=me.measure(me.observatory(pos),"WGS84")
        elif type(pos)==dict: p=me.measure(pos,"WGS84"); pos=meprettystring(pos)
        else: print "ERROR: unknown position type - either a measure (me.position) or observatory name."; return
        f.add_placemarker(qa.convert(p["m1"],"deg")["value"],qa.convert(p["m0"],"deg")["value"],altitude=1,name=pos)
    #f.close_folder()
    f.close()


def uptime(sources,observatory=None,utcdate=None,step=1,coords="J2000",verbose=True,plot=False):
    """
    Prints or plots the position (elevation) of a list of astronomical
    sources at various observatories.
    
    sources: list of sources or a single source, the source can be
    specified by its name (e.g."SUN", "MOON", or "Sgr A*", see the planetlist in CASA, "me.sourcelist()" or "hfradiosources") or simply by its
    J2000 coordinates in the format "23:30:25.0 +70.23.10" or "23H30M25.0S +70D23M10S" (some casa convention separated by space) - the coordinate system can
 be changed with the keyword 'coords'

    observatory: list or single observatory, the observatory can be
    specified by its name (e.g., observatory=["ALMA","LOFAR"]) or by a position measure, e.g. observatory=me.position(rf="WGS84",v0=qa.quantity("+06.52.11.415"),v1=qa.quantity("+52.54.54.44098"),v2=qa.quantity("49.344m"))
    if none is specified the current frame is used.
    
    utcdate: the date (UTC) for which to calculate the uptimes.

    step: the stepsize to calculate elevations values in hours

    coords: The coordinate system to be used for interpreting source
    coordinates (could be e.g., "B1950", "GALACTIC").
    
    """
    if not utcdate==None: me.doframe(me.epoch("UTC",utcdate))
    if not type(observatory)==list: observatory=[observatory]
    if not type(sources)==list: sources=[sources]
    result={}
    for obs in observatory:
        if type(obs)==str: me.doframe(me.observatory(obs)); obs+=" "
        elif type(obs)==dict: me.doframe(obs); obs=meprettystring(obs); obs+=" "
        else: obs=""
        startday=floor(meframedate()["value"])
        for source in sources:
            if source in list(me.listcodes(me.direction())["extra"]): direction=me.direction(source)
            elif source in hfradiosources.keys(): direction=hfradiosources[source]["direction"]
            elif me.sourcelist().find(source): direction=me.source(source)
            else:
                s=source.split()
                direction=me.direction(coords,s[0],s[1])
            el=[];az=[];time=[]
            for i in range(24/step):
                day=startday+i*step/24.
                time.append(i*step)
                me.doframe(me.epoch("UTC",qa.quantity(str(day)+"d")))
                el.append(qa.convert(me.measure(direction,"AZEL")["m1"],"deg")["value"])
                az.append(qa.convert(me.measure(direction,"AZEL")["m0"],"deg")["value"])
                if verbose: print obs,qa.time(qa.quantity(str(day)+"d"),form=["ymd","local"]),sourceinfostr(source,direction)
                if (len(sources)==1) and (len(observatory)>1): key=obs
                else: key=obs+source
                result[key]={"time":time,"az":az,"el":el}
    if plot:
        pl.clf()
        pl.title("Observation Start:"+qa.time(qa.quantity(str(startday)+"d"),form=["ymd"]))
        pl.xlabel("Time of day [h]")
        pl.ylabel("Elevation [degree]")
        for k in result.keys():
            pl.plot(result[k]["time"],result[k]["el"],label=k)
        pl.legend()
        pl.xlim(0,30)
        pl.ylim(0,90)
    return result
        
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
#listsources()

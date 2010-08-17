#!/usr/bin/python

# PuMa2 Control and Monitor Interface (puma2cmi) notes
# This code makes use of Twisted protocol to implement the client-server scheme.
# The application listens at port 56026 - either TMS or user. A typical observation
# at the WSRT happens as follows: specify observation details using TMS, and have the
# details of a particular measurement in a text file in PuMa2 - this is the Specification
# file.
# network interface stuff
from twisted.internet.protocol import Protocol, Factory
from twisted.internet import reactor

# system stuff
from socket import *
from string import *
import time
import datetime 
import os
import sys

from optparse import OptionParser

# global stuff

STATE=["none\n"];
nList=[];

class serv(Protocol):

    n_dict={ "nodes":["storage02","storage03","storage04","storage05",
                      "storage07","storage08","storage09","storage10"]
             }

    n_dict["nnodes"]=len(n_dict["nodes"])
    
    def connectionMade(self):
        s=time.gmtime(time.time());
        aTstamp=("%04d-%02d-%02d-%02d:%02d:%02d ") % (s[0],s[1],s[2],s[3],s[4],s[5])
        aHost=self.transport.getPeer().host;
        sys.stderr.write(aTstamp + " New connection from %s\n" % aHost);
        self.transport.write("Welcome to DADA\r\nDADA:> ") 
		
    def dataReceived(self, a_line):
        s=time.gmtime(time.time());
        aTstamp=("%04d-%02d-%02d-%02d:%02d:%02d ") % (s[0],s[1],s[2],s[3],s[4],s[5])
        sys.stderr.write(aTstamp + " got line:: " + a_line);
        
        (exitStat, msg) = mainLoop(a_line)

        if exitStat == 0:
            self.transport.write(msg)
            self.transport.write("> ok\r\n")

        elif exitStat == -1:
            self.transport.write(msg)
            self.transport.write("> fail\r\n")
            self.transport.write("DADA:> ")

        elif exitStat == -2:
            self.transport.write(msg);
            self.transport.loseConnection()
            
        else:
            self.transport.write("unknown error. exiting\n")
            self.transport.loseConnection()

################################################################
#    METHODS implementing various parts of the commands parser #
################################################################

def mainLoop(line):

    gReturn = -1;gMessage=[];
    
    aline = strip(line);
    msg='';
    
    if aline in ("exit","Exit","EXIT"):
        (err, msg) = (-2,"ok\nexiting\r\n");
    else:
        (gReturn, gList) = parseLine(aline);
        for aVal in gList: (err, msg) = (gReturn, aVal+msg)
            
    if gReturn == 0:
        (eReturn, eMessage) = executeCmd(gList);
        (err, msg) = (eReturn, eMessage)
    
    return (err, msg);

# method that distills commands from the input line
# and validates the command
# implement state related stuff here.
# Various commands are:
# INIT 0/1 (only level 0 implemented now)
# CONF FREQ=112,113,114,115,116,117,118,119; BW=20,20,20,20,20,20,20,20;SOURCE=B1937+21;OBS_ID=103405;SpecFile=/home/pulsar/SPEC/S0.txt
# CONF FREQ=112,113,114,115,116,117,118,119; BW=-2.5000,-2.5000,-2.500,-2,-20,-20,-20,-20;SOURCE=B1937+21;OBS_ID=103405;SpecFile=/home/pulsar/SPEC/S0.txt
# START
# STOP
# STATUS
# of these CONF has to be iteratively distilled to extract parameters
# first split into tokens, based on white space, comma and "=", then search
# for important parameters.

# aLine is the string received.
# retList is the return list
def parseLine(aLine):
    retList=[];
    tokens=split(aLine,";");
    for i in range(len(tokens) - 1): tokens[i+1]=replace(tokens[i+1]," ","");
    subToken=split(tokens[0]);
    if subToken: primeToken=subToken[0];
    else:
        retList.append("nop ... \r\n");
        return(-1,retList);
       
    if primeToken in ("INIT","CONF","START","STOP","HELP","STATUS","help"):
        pass
    else:
        retList.append("parseLine: invalid command\r\n");
        return(-1,retList);

    if primeToken == "INIT" and len(subToken) != 2:
        retList.append("parseLine: INIT takes exactly one parameter\n");
        return(-1,retList);
    elif primeToken == "INIT":
        if (subToken[1] == "0"):
            retList=["INIT"];
            return (0,retList)
        else:
            retList.append("parseLine: invalid INIT level\r\n");
            return(-1,retList);

    elif primeToken == "CONF":
        aList=[];bList=[];retList=[];
        if len(subToken) < 2:
            retList.append("parseLine: CONF needs parameters\r\n");
            return (-1, retList);

        for i in range(len(tokens)):
            if "=" in tokens[i]:
                for val in split(tokens[i],"="): aList.append(val)
            else:
                aList.append(tokens[i])

        for i in range(len(aList)):
            if " " in aList[i]:
                for val in split(aList[i]): bList.append(val)
            else:
                bList.append(aList[i])

        #print bList, tokens

        bLine=aLine.replace(" ","")
        
        if "FREQ=" not in bLine: retList.append("parseLine: FREQuency required\r\n");
        if "BW=" not in bLine: retList.append("parseLine: BandWidth required\r\n")
        if "SOURCE=" not in bLine: retList.append("parseLine: SOURCE required\r\n")
        if "OBS_ID=" not in bLine: retList.append("parseLine: OBS_ID required\r\n")
        if "SpecFile=" not in bLine: retList.append("parseLine: Specification File required\r\n")

        aLen=len(bLine);
        for param in ("FREQ=","BW="):
            i=find(bLine,param) + len(param);
            j=find(bLine,";",i,aLen);
            if len(split(bLine[i:j],",")) !=8:
                retList.append("parseLine: incorrect parameters to " + param[:-1] + "\r\n")
            
        for param in ("SOURCE=","OBS_ID=","SpecFile="):
            i=find(bLine,param) + len(param);
            j=find(bLine,";",i,aLen);
            if len(bLine[i:j]) < 1: retList.append("parseLine: invalid " + param[:-1] + "\r\n")

        if len(retList) > 0: return(-1, retList)

        return(0,bList);

    elif primeToken == "START":
        if len(subToken) > 1:
            retList=["parseLine: START accepts no parameters\r\n"];
            return(-1, retList);
        else:
            retList=["START"];
            return (0,retList);

    elif primeToken == "STOP":
        if len(subToken) > 1:
            retList=["parseLine: STOP accepts no parameters\r\n"];
            return(-1, retList);
        else:
            retList=["STOP"];
            return (0,retList);

    elif primeToken in ("help","HELP"):
        if len(subToken) > 1:
            retList=["parseLine: HELP accepts no parameters\r\n"];
            return(-1, retList);
        else:
            retList=["HELP"];
            return (0,retList);
       
    elif primeToken == "STATUS":
        return (0,["STATUS"])
    
    return(0,primeToken);

 
# implement all commands here
def executeCmd(aList):

    global STATE;
    global nList;
    
    BandNodeMap={"BAND0":"storage02",
                 "BAND1":"storage03",
                 "BAND2":"storage04",
                 "BAND3":"storage05",
                 "BAND4":"storage07",
                 "BAND5":"storage08",
                 "BAND6":"storage09",
                 "BAND7":"storage10"};
    useList=[];
    Bands=[];
    hList=[];

    BWList=[];
    FList=[];
    dirList=[];
    tmpList=[];
    

    s=time.gmtime(time.time());
    aTstamp=("%04d-%02d-%02d-%02d:%02d:%02d ") % (s[0],s[1],s[2],s[3],s[4],s[5])
    sys.stderr.write(aTstamp + " Command:: %s \n" % aList);
    
    # if HELP is requested.
    if aList[0] == "HELP":
        return(0,
        "Possible commands for DADA Primary Write Clients:\n \
        help      print this help\n \
        INIT      initialize PWC nodes\n \
        CONF      configure PWC nodes\n \
        START     start acquisition\n \
        STOP      stop a current acquisition\n \
        STATUS    return current state of system\n \
        EXIT      exit\r\n")

    # if STATUS is requested:
    if aList[0] == "STATUS":
        return(0,"");

    # implement CONF command
    # Check if valid SpecFile is given. If not find defaults.
    # in SpecFile, check IN_USE parameters, and choose only those nodes
    # Collect, BW and FREQ from CONF parameters.
    if aList[0] == "CONF":

        nList=[];
        # check state and return error if not INIT'ed
        
        # is the specification file present?
        f_index = aList.index("SpecFile") + 1;
        try: specFP = file(aList[f_index]);
        except IOError:
            msg=("executeCmd: cannot open specification file: %s\r\n")%(aList[f_index]);
            return(-1,msg);

        tmpList=[];
        for line in specFP:
            if "IN_USE" in line: tmpList.append(line);
        specFP.seek(0,0);

        
        # extract only those band numbers in use from spec file
        for item in tmpList:
            line=split(item);
            if line[1] == "1":
                line=split(line[0],"_");
                line=line[0];
                Bands.append(line[4:]);

        # All BW's specified in CONF
        i=aList.index("BW")+1;
        BWList=split(aList[i],",");
        i=aList.index("FREQ")+1;
        FList=split(aList[i],",");
        I=0;
        for aBand in Bands:
            bIndex=atoi(aBand);
            if atof(BWList[bIndex]):
                tmpList=[];
                for line in specFP:
                    aStr="BAND" + aBand;
                    if aStr in line:
                        line=split(line,"#");
                        line=rstrip(lstrip(line[0])) + "\\";
                        line=replace(line,aStr+"_","");
                        if "FREQ" in line: line="FREQ  "+ FList[bIndex]+"\\";
                        if "BW" in line: line="BW  "+ BWList[bIndex]+"\\";
                        i=aList.index("OBS_ID") + 1;
                        if "OBS_ID" in line: line="OBS_ID  "+ aList[i]+"\\";
                        i=aList.index("SOURCE") + 1;
                        if "SOURCE" in line: line="SOURCE  "+ aList[i]+"\\";
                        if "FILE_NAME" in line: dirList.append(split(line)[1]);
                        line=replace(line,"\\ ","\\");
                        tmpList.append(line);
                hList.append("");
                if BandNodeMap.has_key(aStr): nList.append(BandNodeMap[aStr]);
                for i in range(len(tmpList) - 1): hList[I] = hList[I] + tmpList[i];
                I=I+1;
            specFP.seek(0,0); # rewind file

        if (len(nList)) < 1: return(-1,"executeCmd: <CONF> no nodes in list\r\n");

        # connect to all nodes in list, and send parameters.
        pidlist=[];
        for i in range(len(nList)):
            dmsg="";
            aNode=nList[i];
            aDir=dirList[i];
            pid1 = os.fork();
            if pid1 == 0:
                s=time.gmtime(time.time());
                aTstamp=("%04d-%02d-%02d-%02d:%02d:%02d ") % (s[0],s[1],s[2],s[3],s[4],s[5]);
                
                cmd= "ssh -o ConnectTimeout=2 %s /bin/mkdir -p %s " % (aNode,aDir)
                os.system(cmd);
                time.sleep(0.1);
                dmsg=aTstamp+"\n"+ cmd;

                cmd= "ssh -o ConnectTimeout=2 %s dada_dbdisk -D %s -W -d" % (aNode,aDir)
                os.system(cmd);
                time.sleep(0.1);
                dmsg = dmsg +"\n"+ cmd;
                # replace current process with a ssh client.
                # this process will wait here indefinitely - after the following line
                # puma2_dmadb doesn't exit, and ssh will not exit. So wait till parent
                # generates a interrupt to kill the ssh client in master. puma2_dmadb
                # continues in background in the storage nodes
                cmd = ["ssh","-o ConnectTimeout=2",aNode,
                       "limit memorylocked 2000000;puma2_dmadb -m 2 >& /tmp/log&"];
                dmsg = dmsg +"\n"+ str(cmd);
                sys.stderr.write(dmsg+"\n"+"Going to replace this process\n");
                os.execvp("ssh",cmd);

            else:
                pidlist.append(pid1);
                continue

        time.sleep(3);
        for pid in pidlist:
            dmsg="pid: %d" % pid;
            try: os.kill(pid,9);
            except OSError:
                dmsg = dmsg + "executeCmd: <CONF> child had already exited\n";
                sys.stderr.write(dmsg);
            os.waitpid(pid,0);     

        # connect to all nodes again, and send respective header info
        pidlist=[];
        #rPipeList=[];
        #wPipeList=[];
        for i in range(len(nList)):        
            aNode=nList[i];
            aHeader="header " + hList[i] + "\n ";
            # open pipes to facilitate child-parent IPC
            #aPipe=os.pipe();
            #rPipeList.append(aPipe[0]);
            #wPipeList.append(aPipe[1]);
            # now go on to spawn child processes
            pid1 = os.fork();
            if pid1 == 0:
                # close read end of pipe open write end
                #os.close(rPipeList[i]);
                #w = os.fdopen(wPipeList[i],'w',0);
                #w.write("message from child no %02d\n" % i)
                #w.flush();
                #w.close();
                # print debug message, with time stamp.
                s=time.gmtime(time.time());
                aTstamp=("%04d-%02d-%02d-%02d:%02d:%02d ") % (s[0],s[1],s[2],s[3],s[4],s[5])
                dmsg=aTstamp;
                
                sock = socket(AF_INET,SOCK_STREAM);
                sock.settimeout(3); 
                try: sock.connect((aNode,56026));
                except error:
                    dmsg = dmsg + "executeCmd: <CONF-header> connect error: %s\r\n" % aNode;
                    sys.stderr.write(dmsg);# write out debug message
                    # return(-1,"CONF: socket error");
                except timeout:
                    dmsg = dmsg + "executeCmd: <CONF-header> timeout: %s\r\n" % aNode;
                    sys.stderr.write(dmsg);# write out debug message
                    # return(-1,"socket timeout");
                else:
                    time.sleep(0.2);
                    aCmd = "reset\n "
                    sock.send(aCmd);
                    time.sleep(0.1);
                    response = sock.recv(1024);
                    dmsg = dmsg + aCmd + response;
                    time.sleep(0.1);

                    aTstamp=("%04d-%02d-%02d-%02d:%02d:%02d ") % (s[0],s[1],s[2],s[3],s[4],s[5])
                    dmsg=aTstamp;
                    sock.send(aHeader);
                    time.sleep(0.1);
                    response = sock.recv(1024);
                    dmsg = dmsg + aHeader + response;
                    time.sleep(0.1);
                    sock.close();
                    dmsg = dmsg + "socket closed\n";
                    sys.stderr.write(dmsg);# write out debug message
                    
                time.sleep(10000); # sleep till parent kills
                
            else:
                pidlist.append(pid1);
                continue

        time.sleep(3);
        # the read end of pipes
        #for i in range(len(rPipeList)):
            #os.close(wPipeList[i]);
            #r = os.fdopen(rPipeList[i]);
            #aTxt = r.read();
            #sys.stderr.write("aTxt: %s"%aTxt); 
        # kill all child processes            
        for pid in pidlist:
            try: os.kill(pid,9);
            except OSError: sys.stderr.write("executeCmd: <CONF-header> child had already exited\n");
            os.waitpid(pid,0);

        # the read end of pipes
        #for i in range(len(rPipeList)):
            #os.close(wPipeList[i]);
            #r = os.fdopen(rPipeList[i]);
            #aTxt = r.read();
            #sys.stderr.write("aTxt: %s"%aTxt);     

        # return proper error message
        return(0,"");

    # The START command - start acquisition in all nodes
    if aList[0] == "START":

        pidlist=[];
        for aNode in nList:
            pid1 = os.fork();
            if pid1 == 0:   
                # print debug message, with time stamp.
                s=time.gmtime(time.time());
                aTstamp=("%04d-%02d-%02d-%02d:%02d:%02d ") % (s[0],s[1],s[2],s[3],s[4],s[5])
                dmsg=aTstamp;
                
                sock = socket(AF_INET,SOCK_STREAM);
                sock.settimeout(1); 
                try: sock.connect((aNode,56026));
                except error:
                    dmsg = dmsg + "executeCmd: <START> connect error: %s\r\n" % aNode;
                    sys.stderr.write(dmsg);# write out debug message
                except timeout:
                    dmsg = dmsg + "executeCmd: <START> timeout: %s\r\n" % aNode;
                    sys.stderr.write(dmsg);# write out debug message    
                else:
                    aCmd = "go\n ";
                    sock.send(aCmd);
                    time.sleep(0.1);
                    response = sock.recv(1024);
                    dmsg = dmsg + aCmd + response;
                    sock.close();
                    dmsg = dmsg + "socket closed\n";
                    sys.stderr.write(dmsg);# write out debug message
                
                time.sleep(10000);# wait for parent to kill
                
            else:
                pidlist.append(pid1);
                continue

        time.sleep(2);
        for pid in pidlist:
            try: os.kill(pid,9);
            except OSError: sys.stderr.write("parent: executeCmd: <START> child had already exited\n");
            os.waitpid(pid,0);
            
        return(0,"");

    # The STOP command
    if aList[0] == "STOP":
        pidlist=[];
        for aNode in nList:
            pid1 = os.fork();
            if pid1 == 0:   
                # print debug message, with time stamp.
                s=time.gmtime(time.time());
                aTstamp=("%04d-%02d-%02d-%02d:%02d:%02d ") % (s[0],s[1],s[2],s[3],s[4],s[5])
                dmsg=aTstamp;
                
                sock = socket(AF_INET,SOCK_STREAM);
                sock.settimeout(1); 
                try: sock.connect((aNode,56026));
                except error:
                    dmsg = dmsg + "executeCmd: <STOP> connect error: %s\r\n" % aNode;
                    sys.stderr.write(dmsg);# write out debug message
                except timeout:
                    dmsg = dmsg + "executeCmd: <STOP> timeout: %s\r\n" % aNode;
                    sys.stderr.write(dmsg);# write out debug message
                else:    
                    aCmd = "stop\n ";
                    sock.send(aCmd);
                    time.sleep(0.1);
                    response = sock.recv(1024);
                    dmsg = dmsg + aCmd + response;
                    sock.close();
                    dmsg = dmsg + "socket closed\n";
                    sys.stderr.write(dmsg);# write out debug message
                
                time.sleep(10000);# wait for parent to kill
                
            else:
                pidlist.append(pid1);
                continue

        time.sleep(2);
        for pid in pidlist:
            try: os.kill(pid,9);
            except OSError:  sys.stderr.write("parent:executeCmd: <STOP> child had already exited\n");
            os.waitpid(pid,0);
            
        return(0,"");
    
    # Cleanup in nodes, if INIT command

    if (aList[0] == "INIT"):
        STATE=["INIT\n"];
        pidlist=[];
        # print debug message, with time stamp.
        s=time.gmtime(time.time());
        aTstamp=("%04d-%02d-%02d-%02d:%02d:%02d ") % (s[0],s[1],s[2],s[3],s[4],s[5])
        dmsg=aTstamp + "sending cleanup/db commands\n";
        sys.stderr.write(dmsg);# write out debug message

        time.sleep(1); # sleep a second for things to settle down
        for nodenum in (2,3,4,5,7,8,9,10):
            aNode=("storage%02d")%nodenum
            pid1 = os.fork();
            if pid1 == 0: # child processes
                # print debug message, with time stamp.
                s=time.gmtime(time.time());
                aTstamp=("%04d-%02d-%02d-%02d:%02d:%02d ") % (s[0],s[1],s[2],s[3],s[4],s[5])
                dmsg=aTstamp;

                cmd= "ssh -o ConnectTimeout=2 %s /home/pulsar/bin/cleanup" % (aNode)
                oo,ii,ee=os.popen3(cmd);
                dmsg = dmsg + cmd + "\n" + ee.read();
                time.sleep(0.1);
                
                cmd= "ssh -o ConnectTimeout=2 %s dada_db -n 200 -b 4000000" % (aNode)
                oo,ii,ee=os.popen3(cmd);
                dmsg = dmsg + cmd + "\n" + ee.read(); 
                time.sleep(0.1);
                sys.stderr.write(dmsg);# write out debug message
                
                time.sleep(10000); #wait for parent to kill
                
            else:
                pidlist.append(pid1);
                continue

        #leave node loop - this is parent    
        time.sleep(6);

        # print debug message, with time stamp.
        s=time.gmtime(time.time());
        aTstamp=("%04d-%02d-%02d-%02d:%02d:%02d ") % (s[0],s[1],s[2],s[3],s[4],s[5])
        dmsg=aTstamp + "killing child processes\n";
        sys.stderr.write(dmsg);# write out debug message

        for pid in pidlist:
            os.kill(pid,9);
            os.waitpid(pid,0);

        return(0,"");

    msg=("cannot execute %s command\n")%(aList[0]); 
    return (-1,msg);
    
def cmdparse():

    usage = "usage: %prog [options] arg"
    parser = OptionParser(usage)

    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose")

    (options, args) = parser.parse_args()

    if len(args) != 0:
        print "No of args ",len(args);
        parser.error("incorrect number of arguments")


# This is the main function.

if __name__ == "__main__":

    # parse command line, and get parameters.
    # get node, end, begin time list
    cmdparse();

    # sanity checks - test if daemon is already running
    # examine /proc, based on pid file.

    # The first of double forks - so that we can exit peacefully.
    try:
        pid1 = os.fork()
        if pid1 > 0: sys.exit(0) # Exit first parent.
    except OSError, e:
        sys.stderr.write("fork #1 failed: (%d) %s\n" % (e.errno, e.strerror))
        sys.exit(1)

    # Decouple from parent environment.
    os.chdir("/")
    os.umask(0)
    os.setsid()

    # Do second fork.
    try:
        pid1 = os.fork()
        if pid1 > 0: sys.exit(0) # Exit second parent.
    except OSError, e:
        sys.stderr.write("fork #2 failed: (%d) %s\n" % (e.errno, e.strerror))
        sys.exit(1)


    #pid1 = os.fork();
    #if pid1 > 0:
        #sys.exit(0) # the first parent exits.
    #else:

    # redirect stdin/stdout stuff
    logfile = "/tmp/puma2cmi.log";
    pidfile = "/tmp/puma2cmi.pid";
    si = file("/dev/null", 'r')
    so = file(logfile, 'a+')
    se = file(logfile, 'a+', 0)
    
    pid = str(os.getpid());
    fpid = file(pidfile,'w+')
    fpid.write("%s\n" % (pid))
    fpid.flush();
    fpid.close();
    
    os.dup2(si.fileno(), sys.stdin.fileno())
    os.dup2(so.fileno(), sys.stdout.fileno())
    os.dup2(se.fileno(), sys.stderr.fileno())
    
    # start the server for TMS/user
    factory = Factory()
    factory.protocol = serv
    
    # 56026 is the port DADA is listening
    reactor.listenTCP(56026, factory)
    reactor.run()
    
    

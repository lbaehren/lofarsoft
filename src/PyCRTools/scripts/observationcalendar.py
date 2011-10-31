#! /usr/bin/env python
#
#  Make an observation calendar using a basic HTML table.
#  Created by Arthur Corstanje on Oct 24, 2011.
#  Copyright (c) 2011. All rights reserved.

import subprocess
import os
import time

def getFileList(criterion, dir):
# get all parset filenames which match 'criterion'
    proc = subprocess.Popen(['grep', '-lr', criterion, dir], stdout=subprocess.PIPE)
    output = proc.communicate()[0]
    print output
    filelist = output.splitlines()
    return filelist

def getStartStopTime(filename):
    infile = open(filename, 'r')
    startTime = -1
    stopTime = -1
    for line in infile:
        if 'Observation.startTime' in line:
            startTime = line.split('=')[1]
            print startTime
        if 'Observation.stopTime' in line:
            stopTime = line.split('=')[1]
            print stopTime
    infile.close()
    return (startTime, stopTime)

def hexstr(i):
    s = hex(i)[2:]
    if len(s) < 2:
        s = '0' + s
    return s

parsetDir = '/Users/acorstanje/triggering/CR/observations'
criterion = 'NoCoincChann=1000' # for including TBBs - without VHECR radio trigger (i.e. LORA)

print 'Getting the latest Observationxxxxx parset files...'
proc = subprocess.Popen(['rsync', '-avv', 'mcu001:/opt/lofar/share/Observation*', parsetDir])
proc.wait()

filelist = getFileList(criterion, parsetDir)
for file in filelist:
    (startTime, stopTime) = getStartStopTime(filelist[0])
    print time.strptime(startTime, "%Y-%b-%d %H:%M:%S\n")


htmlfilename = '/Users/acorstanje/usg/src/PyCRTools/scripts/htmltabletest.html'

title = 'Test observation calendar'

html = open(htmlfilename, 'w')

html.write("<html>\n<head>\n<title>" + title + "</title>\n<body>\n<table cellpadding=5>\n")

html.write("<tr>\n<th>October 2011</th>")
for i in range(24): # columns (header)
    html.write("<th>" +("0" + str(i) if i < 10 else str(i)) + "</th>")
html.write("</tr>\n")

for i in range(30): # rows
    bgcolor = 255 # 64 + ((255-64) * i) / 30
    colorstr = hexstr(bgcolor)

    html.write("<tr bgcolor=#FF"+colorstr+"FF>\n")
    html.write("<td>" + str(i) + "</td>") # day of month
    for j in range(24):
        if (j + i) % 5 != 0:
            html.write("<td>  </td>")
        else:
            html.write('<td bgcolor=#FF0000>  </td>')
    html.write("</tr>\n")
    
html.close()

#<div style="height: 10; width: 12px; overflow: hidden">
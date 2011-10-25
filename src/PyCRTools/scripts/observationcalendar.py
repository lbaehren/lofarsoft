#! /usr/bin/env python
#
#  Make an observation calendar using a basic HTML table.
#  Created by Arthur Corstanje on Oct 24, 2011.
#  Copyright (c) 2011. All rights reserved.

def hexstr(i):
    s = hex(i)[2:]
    if len(s) < 2:
        s = '0' + s
    return s

htmlfilename = '/Users/acorstanje/usg/src/PyCRTools/scripts/htmltabletest.html'

title = 'Test observation calendar'

html = open(htmlfilename, 'w')

html.write("<html>\n<head>\n<title>" + title + "</title>\n<body>\n<table cellpadding=5>\n")

html.write("<tr>\n<th>October 2011</th>")
for i in range(24): # columns (header)
    html.write("<th>" +("0" + str(i) if i < 10 else str(i)) + "</th>")
html.write("</tr>\n")

for i in range(30): # rows
    bgcolor = 64 + ((255-64) * i) / 30
    colorstr = hexstr(bgcolor)

    html.write("<tr bgcolor=#00"+colorstr+"00>\n")
    html.write("<td>" + str(i) + "</td>") # day of month
    for j in range(24):
        if (j + i) % 5 != 0:
            html.write("<td> X </td>")
        else:
            html.write('<td bgcolor=#FF0000>  </td>')
    html.write("</tr>\n")
    
html.close()

#<div style="height: 10; width: 12px; overflow: hidden">
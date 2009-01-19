#!/usr/bin/python

# points.py

import sys, random
from PyQt4 import QtGui, QtCore


class Points(QtGui.QWidget):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)

        self.setGeometry(300, 300, 250, 150)
        self.setWindowTitle('Points')

    def paintEvent(self, event):
        paint = QtGui.QPainter()
        paint.begin(self)
        paint.setPen(QtCore.Qt.red)
        size = self.size()
        for i in range(1000):
            x = random.randint(1, size.width()-1)
            y = random.randint(1, size.height()-1)
            paint.drawPoint(x, y)
        paint.end()

app = QtGui.QApplication(sys.argv)
dt = Points()
dt.show()
#app.exec_()

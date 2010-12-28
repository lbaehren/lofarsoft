import sys, time

##global Message Levels
ErrorLevel   = 70 ## Error
WarningLevel = 60 ## Warning
NotifyLevel  = 50 ## Always visible, stderr of external processes
VerboseLevel = 30 ## Visible on verbose, stdout of external processes
DebugLevel   = 10 ## Visible on debug

class WSRTmessages(list):
    """Class for handling message logging redirection and reporting tresholds."""
    def __init__(self):
        list.__init__(self)
        self.log   = {sys.stdout:NotifyLevel} ## NotifyLevel and above are always sent to stdout
        self._store = False

    def store(self):
        self._store = True
            
    def pause(self):
        self._store = False
                
    def clear(self):
        list.__init__(self)
        self._store = False

    def append(self, level, item):
        """level determines if the message gets reported, item should basically be a string"""
        t = time.gmtime()
        if self._store and level > DebugLevel:
            list.append(self, (t, level, item)) ## storing the item for parsing by the caller.
        for output in self.log.keys():
            if self.log[output] <= level:
                if level >= ErrorLevel:
                    e = ' Error   : '
                elif level >= WarningLevel:
                    e = ' Warning : '
                elif level >= NotifyLevel:
                    e = ' Notification: '
                elif level >= VerboseLevel:
                    e = '     Message : '
                elif level >= DebugLevel:
                    e = '        Debug: '
                output.write('%04d-%02d-%02d %02d:%02d:%02d' % (t[0], t[1], t[2], t[3], t[4], t[5])
                             + e + item.strip() + '\n')
                output.flush()

    def __repr__(self):
        text = ''
        for i in self:
            t     = i[0]
            level = i[1]
            item  = i[2]
            if level >= ErrorLevel:
                e = ' Error   : '
            elif level >= WarningLevel:
                e = ' Warning : '
            elif level >= NotifyLevel:
                e = ' Notification: '
            elif level >= VerboseLevel:
                e = '     Message : '
            elif level >= DebugLevel:
                e = '        Debug: '
            text += ('%04d-%02d-%02d %02d:%02d:%02d' % (t[0], t[1], t[2], t[3], t[4], t[5])
                         + e + item.strip() + '\n')
        return text

    def addlogger(self, level, logger):
        """The level should be one of the above 
        global levels and the logger should support
        a write(string) method."""
        self.log[logger] = level

    def setloglevel(self, level, logger):
        """Changes the level at which logging info is written to the logger."""
        for output in self.log.keys():
            if logger == output:
                self.log[logger] = level

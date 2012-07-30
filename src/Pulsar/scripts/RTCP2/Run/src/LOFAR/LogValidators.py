import re

class ValidationError(Exception):
  pass

class LogValidator:
  """ Validates a log file. """

  def __init__(self):
    self.ok = True

  def begin(self):
    pass

  def parse(self,line):
    parts = line.split(" ",4)
    if len(parts) != 5:
      self.parseLine(line)
    else:
      self.parseLogLine(*parts)

  def parseLogLine(self,proc,date,time,level,msg):
    pass

  def parseLine(self,line):
    pass

  def end(self):
    pass

  def valid(self):
    return self.ok

class AlwaysValid(LogValidator): 
  """ Considers a log to be always valid. """
  pass

class NoErrors(LogValidator):
  """ Considers a log valid if there are no errors or exceptions. """

  def parseLogLine(self,proc,date,time,level,msg):
    if level in ["FATAL","ERROR","EXCEPTION"]:
      raise ValidationError( "Encountered an %s" % (level,) )

class NoDrops(LogValidator):
  """ Considers a log valid if there is no dropped data reported. """

  def parseLogLine(self,proc,date,time,level,msg):
    if proc.startswith("IONProc") and "Dropping data" in msg:
      raise ValidationError( "Dropped data" )

    if proc.startswith("Storage") and "OutputThread dropped" in msg:
      raise ValidationError( "Dropped data" )

class RealTime(LogValidator):
  """ Considers a log valid if the pipeline seems to operate in real time. """

  def parseLogLine(self,proc,date,time,level,msg):
    if proc.startswith("IONProc") and re.search("late: +[0-9.]+ +s",msg):
      raise ValidationError( "Non-realtime behaviour" )


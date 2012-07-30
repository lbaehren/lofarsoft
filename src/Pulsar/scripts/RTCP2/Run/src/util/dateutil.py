#!/usr/bin/env python

import datetime
import time

__all__ = ["parse","parseDuration","format","timestamp"]

def parse( str ):
  """ Accept: a timestamp, HH:MM:SS, YYYY-MM-DD, YYYY-MM-DD HH:MM:SS, +HH:MM:SS or +seconds. Returns: a datetime object. """

  str = str.strip()

  if str[0] == "+":
    # +HH:MM:SS or +seconds, use it as offset from now()
    return datetime.datetime.now() + datetime.timedelta( 0, parseDuration( str[1:] ) )
  elif "-" in str:
    # assume YYYY-MM-DD HH:MM:SS, but time elements are optional
    try:
      date,time = str.split()
    except ValueError:
      date,time = str,"00:00:00"

    def conv_month(m):
      """ Convert month 'm', which is either a  string or a number. """
      months = ["Jan","Feb","Mar",
                "Apr","May","Jun",
                "Jul","Aug","Sep",
                "Oct","Nov","Dec"]

      try:
        return months.index(m) + 1
      except ValueError:
        return int(m)
	  
    y,m,d = date.split("-")
    date_elements = [int(y), conv_month(m), int(d)]

    h,m,s = time.split(":")

    try:
      s,ms = s.split(".")

      # convert to 6 digits
      digits = 6 # microsecond precision in datetime objects
      ms = ms[:digits]
      ms = ms + "0" * (digits-len(ms)) # left align the digits!
    except:
      s,ms = s,0

    time_elements = map( int, [h,m,s,ms] )

    return datetime.datetime( *(date_elements + time_elements) )
  elif ":" in str:
    # assume HH:MM:SS, and use today as the date
    now = datetime.datetime.now()
    time_elements = map( int, str.split(":") )

    return datetime.datetime( now.year, now.month, now.day, *(time_elements) )
  else:
    # a timestamp
    return datetime.datetime.fromtimestamp( int(str) )   

def parseDuration( str ):
  """ Accept: a number of seconds, HH:MM:SS. Returns: an integer. """
  str = str.strip()

  def toSeconds( h, m = 0, s = 0 ):
    return h * 3600 + m * 60 + s;

  if ":" in str:
    # assume HH:MM:SS
    time_elements = map( int, str.split(":") )
    return toSeconds( *(time_elements) )
  else:
    # a number of seconds
    return float( str )

def format( dt ):
  """ Convert either a datetime object or a timestamp to YYYY-MM-DD HH:MM:SS. """

  if isinstance( dt, int ) or isinstance( dt, float ):
    dt = datetime.datetime.fromtimestamp( dt )

  return dt.strftime("%F %T")

def timestamp( dt ):
  """ Returns the UTC timestamp corresponding to the provided datetime object. """

  return float(time.mktime( dt.utctimetuple() ))
  
if __name__ == "__main__":
  """ Convert whatever parse() accepts into UNIX timestamps and a proper date/time string. """

  import sys

  if len(sys.argv) == 1:
    print """usage: %s str [str] ...

Where "str" can be anything accepted by parse(), which is described by:

%s""" % (sys.argv[0],parse.__doc__)
    sys.exit(1)

  for s in sys.argv[1:]:
    parsed = parse(s)
    print "%s = %s / %s" % (s,timestamp(parsed),format(parsed))


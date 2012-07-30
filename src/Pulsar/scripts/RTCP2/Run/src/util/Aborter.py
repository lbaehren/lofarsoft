import signal

__all__ = ["runUntilSuccess","runFunc"]

class Aborter:
  """ Abort commands using a sequence of kill commands, which are kept in check with a timeout. """

  def installHandler( self ):
    """ Installs our custom alarm handler. """

    self.oldhandler = signal.getsignal( signal.SIGALRM )
    if self.oldhandler is None:
      self.oldhandler = signal.SIG_DFL

    signal.signal( signal.SIGALRM, self.sigHandler )

  def removeHandler( self ):
    """ Removes our custom alarm handler. """

    signal.signal( signal.SIGALRM, self.oldhandler )

  def sigHandler( self, signum, frame ):
    """ Our custom alarm handler. """

    raise KeyboardInterrupt

  def resetAlarm( self ):
    """ Reset a raised alarm, if any. """

    self.alarmraised = False

  def runFunc( self, func, timeout ):
    """ Run a function, which is aborted either through a timeout or a KeyboardInterrupt. """

    self.alarmraised = False
    self.installHandler()

    try:
      try:
        signal.alarm( timeout )

        func()
      except KeyboardInterrupt:
        self.alarmraised = True
    finally:
      signal.alarm( 0 )

      self.removeHandler()

    return not self.alarmraised 

  def runUntilSuccess( self, funclist, timeout ):
    """ Run functions from funclist sequentially until one does not time out. """

    for f in funclist:
      success = self.runFunc( f, timeout )

      if success:
        return True

    return False 

Aborter = Aborter()
runUntilSuccess = Aborter.runUntilSuccess
runFunc = Aborter.runFunc

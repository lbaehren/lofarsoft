#from message import ErrorLevel, NotifyLevel, VerboseLevel, DebugLevel
import time, os, select, pty, fcntl, sys, logging

class CookError(Exception):
    """Base class for all exceptions raised by this module."""
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return `self.value`

class WSRTCook(object):
    def __init__(self, task, inputs, outputs, logger):
        self.inputs   = inputs
        self.outputs  = outputs
        self.task     = task.strip()
        self.logger   = logger

class PipelineCook(WSRTCook):
    def __init__(self, task, inputs, outputs, logger):
        super(PipelineCook, self).__init__(task, inputs, outputs, logger)
        try:
            exec(eval("'from %s import %s' % (task, task)"))
            exec(eval("'self.recipe = %s()' % task"))
            self.recipe.logger = logging.getLogger("%s.%s" % (self.logger.name, task))
        except Exception, e:
            self.logger.exception("Exception caught: " + str(e))
            self.recipe = None
            raise CookError (task + ' can not be loaded')

    def try_running(self):
        """Run the recipe, inputs should already have been checked."""
        self.recipe.name     = self.task
        if not self.recipe.run(self.task):
            self.copy_outputs()
        else:
            raise CookError (self.task + ' failed')

    def copy_inputs(self):
        for k in self.inputs.keys():
            self.recipe.inputs[k] = self.inputs[k]
#            if self.recipe.optionparser.has_option('--' + k):
#                self.recipe.inputs[k] = self.inputs[k]
#            else:
#                self.logger.debug(self.task + ' has no argument ' + str(k))

    def copy_outputs(self):
        if self.recipe.outputs == None:
            raise CookError (self.task + ' has no outputs') ## should it have??
        else:
            for k in self.recipe.outputs.keys():
                self.outputs[k] = self.recipe.outputs[k]

    def spawn(self):
        self.copy_inputs()
        self.try_running()

class SystemCook(WSRTCook):
    """Based on Parseltongue cody by Mark Kettenis (JIVE)
    and subProcess from: Padraig Brady at www.pixelbeat.org
    and Pexpect from: Noah Spurrier on sourceforge"""
    def __init__(self, task, inputs, outputs, logger):
        super(SystemCook, self).__init__(task, inputs, outputs, logger)
        self._pid      = None ## spawned process ID
        self._child_fd = None ## child output file descriptor
        self._expect   = []
        self._fd_eof   = self._pipe_eof = 0
        ## We can only have a pipe for stderr as otherwise stdio changes it's
        ## buffering strategy
        (self._errorpipe_end, self._errorpipe_front) = os.pipe()
##        self.poll = select.poll()

    def StripNoPrint(self, S):
        from string import printable
        return "".join([ ch for ch in S if ch in printable ])

    def set_expect(self, expectlist):
        self._expect = expectlist

    def spawn(self, env=None):
        """Try to start the task."""
        try:
            (self._pid, self._child_fd) = pty.fork()
        except OSError, e:
            self.logger.error('Unable to fork:' + str(e))
            raise CookError ('fork failed')
        if self._pid == 0: ## the new client
            try:
                #fcntl.fcntl(self.errw, fcntl.F_SETFL, os.O_NONBLOCK)
                #os.dup2(self.outputpipe_front, 1) ## This makes stdio screw
                #up buffering because a pipe is a block device
                
                # we hardcoded assume stderr of the pty has fd 2
                os.dup2(self._errorpipe_front, 2)
                
                os.close(self._errorpipe_end)
                os.close(self._errorpipe_front) ## close what we don't need
                self.logger.info("starting " + " ".join(self.inputs))
                if env:
                    os.execvpe(self.task, self.inputs, env)
                else:
                    os.execvp(self.task, self.inputs)
            except:
                sys.stderr.write('Process could not be started: ' + self.task)
                os._exit(1)
        else: ## the parent
##            self.poll.register(self._child_fd)
##            self.poll.register(self._errorpipe_end)
            os.close(self._errorpipe_front) ## close what we don't need
            fcntl.fcntl(self._child_fd, fcntl.F_SETFL, os.O_NONBLOCK)

    def finished(self):
        """Check whether the task has finished."""
        return self._pid == 0

    def handle_messages(self):
        """Read messages."""
        tocheck=[]
        if not self._fd_eof:
            tocheck.append(self._child_fd)
        if not self._pipe_eof:
            tocheck.append(self._errorpipe_end)
        ready = select.select(tocheck, [], [], 0.25)
        for file in ready[0]:
            try:
                time.sleep(0.05)
                text = os.read(file, 32768)
            except: ## probalby Input/Output error because the child died
                text = ''
            if text:
                for x in self._expect:
                    if x[0] in text: ## we need to do something if we see this text
                        returntext = x[1](text)
                        if returntext:
                            os.write(file, returntext)
                text.replace('\r','\n') ## a pty returns '\r\n' even on Unix
                text.replace('\n\n','\n')
                for line in text.split('\n'): ## still have odd behaviour for gear output
                    if file == self._child_fd:
                        self.logger.info(self.StripNoPrint(line))
                    elif file == self._errorpipe_end:
                        self.logger.warn(self.StripNoPrint(line))
            else:
                if file == self._child_fd:
                    self._fd_eof   = 1
                elif file == self._errorpipe_end:
                    self._pipe_eof = 1
            return 1
##        if self._child_fd in ready[0]:
##            try:
##                text = os.read(self._child_fd, 1024)
##            except: ## probalby Input/Output error because the child died
##                text = ''
##            if text == '':
##                self._fd_eof   = 1
##            else: # should probably do some kind of line buffering
##                if text.find('(O)k/(C)ancel (Ok)') >= 0:
##                    os.write(self._child_fd, 'C\n')
##                else:
##                    self.messages.append(VerboseLevel, self.StripNoPrint(text))
##            return 1
##        if self._errorpipe_end in ready[0]:
##            try:
##                time.sleep(0.002) ## stderr isn't buffered
##                text = os.read(self._errorpipe_end, 1024)
##            except: ## probalby Input/Output error because the child died
##                text = ''
##            if text == '':
##                self._pipe_eof = 1
##            else: # should probably do some kind of line buffering
##                self.messages.append(NotifyLevel, self.StripNoPrint(text))
##            return 1
        if self._fd_eof and self._pipe_eof: # should be an and not an or, but python 2.3.5 doesn't like it
            return 0
        if len(ready[0]) == 0: ## no data in 0.25 second timeout
            return 1
        return 0
##        if self._fd_eof and self._pipe_eof:
##            return 0
##        ready = self.poll.poll(1000)
##        for x in ready:
##            text = ''
##            if (x[1] & select.POLLOUT) or (x[1] & select.POLLPRI):
##                try:
##                    text = os.read(x[0], 1024)
##                except:
##                    if x[0] == self._child_fd:
##                        self._fd_eof   = 1
##                    elif x[0] == self._errorpipe_end:
##                        self._pipe_eof = 1
##            if (x[1] & select.POLLNVAL) or (x[1] & select.POLLHUP) or (x[1] & select.POLLERR) or (text == ''):
##                if x[0] == self._child_fd:
##                    self._fd_eof   = 1
##                elif x[0] == self._errorpipe_end:
##                    self._pipe_eof = 1
##            elif text:
##                if x[0] == self._child_fd:
##                    self.messages.append(VerboseLevel, text)
##                elif x[0] == self._errorpipe_end:
##                    self.messages.append(NotifyLevel, text)
##        if self._fd_eof and self._pipe_eof:
##             return 0
##        return 1 ##else

    def wait(self):
        """Check if the task is finished and clean up."""
        ##self.__excpt = sys.exc_info() might want to check this in some way?
        try:
          (pid, status) = os.waitpid(self._pid, os.WNOHANG) ## clean up the zombie
          assert(pid == self._pid)
          if os.WIFEXITED(status) or os.WIFSIGNALED(status):
              self._pid       = 0
              self.exitstatus = status
          assert(self.finished())
          del self._pid
  ##        self.poll.unregister(self._child_fd)
  ##        self.poll.unregister(self._errorpipe_end)
          os.close(self._child_fd)
          os.close(self._errorpipe_end)
          ## Interpret the return value
          if (self.exitstatus == 0 or self.exitstatus > 255):
              if (self.exitstatus > 255):
                  self.exitstatus = self.exitstatus >> 8
              else: self.exitstatus = 0
              self.logger.info(self.task + ' has ended with exitstatus: ' + str(self.exitstatus))

          else:
              self.logger.warn(self.task + ' was aborted with exitstatus: ' + str(self.exitstatus))
        except Exception, e:
          self.logger.exception('Exception caught: ' + str(type(Exception)) + ' ' + str(e))
          raise CookError (self.task + ' critical error' + str(type(Exception)) + ' ' + str(e))

class MiriadCook(SystemCook):
    def __init__(self, task, inputs, outputs, logger):
        mirbin = os.environ['MIRBIN'] + '/' ## can raise an exception if it doesn't exist
        super(MiriadCook, self).__init__(mirbin + task, inputs, outputs, logger)
        self.logger.debug('Using ' + task + ' found in ' + mirbin)

class AIPSCook(SystemCook):
    def __init__(self, task, inputs, outputs, logger):
        # someting with the Parseltongue AIPSTask
        super(AIPSCook, self).__init__(task, inputs, outputs, logger)

class AIPS2Cook(SystemCook):
    def __init__(self, task, inputs, outputs, logger):
        # Don't know if we can do this right now, we might need a Python interface to AIPS++
        super(AIPS2Cook, self).__init__(task, inputs, outputs, logger)

class GlishCook(SystemCook):
    def __init__(self, task, inputs, outputs, logger):
        # Don't know if we can do this right now, we might need a Python interface to AIPS++
        aipspath = os.environ['AIPSPATH'] ## can raise an exception if it doesn't exist
        super(GlishCook, self).__init__(task, inputs, outputs, logger)
        self.logger.debug('Using ' + task + ' with AIPSPATH ' + aipspath)

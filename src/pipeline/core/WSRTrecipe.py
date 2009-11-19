#!/usr/bin/env python
import message, ingredient, cook

##global Message Levels, imported separately for easy use
from message import ErrorLevel
from message import NotifyLevel
from message import VerboseLevel
from message import DebugLevel

class RecipeError(cook.CookError):
    pass

class WSRTrecipe:
    """Base class for recipes, pipelines are created by calling the cook_* methods.
    Most subclasses should only need to reimplement go() and add inputs and outputs.
    Some might need to addlogger() to messages or override main_results."""
    def __init__(self):
        ## List of messages, use addlogger(level, logger) and append(level, string)
        self.messages = message.WSRTmessages()
        ## List of inputs, self.inputs[key] != True is considered valid input
        self.inputs   = ingredient.WSRTingredient()
        ## List of outputs, should only be filled on succesful execution
        self.outputs  = ingredient.WSRTingredient()
        ## All of these should do something sensible in their __str__ for simple print output
        
        ## Help text, should be indented 8 spaces, for pretty printing with help()
        self.helptext = """
        Standard recipe helptext. No useful help available."""

    def help(self):
        """Shows helptext and inputs and outputs of the recipe"""
        print self.helptext
        print '        Inputs:'
        for k in self.inputs.keys():
            if self.inputs[k]:
                if type(self.inputs[k]) == str:
                    print '            ' + k.ljust(20) + ': [Default: \'' + str(self.inputs[k]) + '\']'
                else:
                    print '            ' + k.ljust(20) + ': [Default: ' + str(self.inputs[k]) + ']'
            else:
                if type(self.inputs[k]) == list:
                    print '            ' + k.ljust(20) + ': [No Default: can be a list]'
                else:
                    if type(self.inputs[k]) == bool:
                        print '            ' + k.ljust(20) + ': [Default: ' + str(self.inputs[k]) + ']'
                    else:
                        print '            ' + k.ljust(20) + ': [No Default]'
        print '        Outputs:'
        for k in self.outputs.keys():
            print '            ' + k
        print """
        Use --help or -h for help, --verbose or -v for more verbose output
        Commandline inputs can either be written in full or
        abbreviated to one letter, example:
        Inputs:    number   [No Default]
        can be entered from the commandline as:
        \"--number=123456\" or \"-n123456\" 
        lists can be entered as comma separated values for example:
        \"--filenames=test1.txt,test2.txt,test3.txt\" """

    def main_init(self):
        """Main initialization for stand alone execution, reading input from the command line"""
        import getopt, sys
        short_ops = 'vhd'
        long_ops  = ['verbose', 'help', 'debug']
        for k in self.inputs.keys():
            short_ops += k[0] + ":"
            long_ops.append(k + "=")
        try:
            opts, args = getopt.getopt(sys.argv[1:], short_ops, long_ops)
            if not self.parse_commandline(opts):
                return 0
        except getopt.GetoptError:
            self.messages.append(ErrorLevel, 'Unable to parse commandline')
            return 0
        return self.check_inputs()

##   parser = OptionParser(usage='%prog [options] <name> <path>')
##   parser.add_option("-c", "--chmod", dest="chmod", default=False,
##                     help="chmod a file on the server")
##   parser.add_option("-u", "--update", dest="update",
##                     action="store_true", default=False,
##                     help="update a file, on the server or locally")
##   parser.add_option("-g", "--get", dest="get",
##                     action="store_true", default=False,
##                     help="download a file from the server")
##   parser.add_option("-d", "--delete", dest="delete",
##                     action="store_true", default=False,
##                     help="delete a file from the server")
##
##   options, args = parser.parse_args(argv)

    def parse_commandline(self, opts):
        """Parse the options retrieved with getopt"""
        import sys, string
        for o, a in opts:
            if o in ('-v', '--verbose'):
                self.messages.setloglevel(VerboseLevel, sys.stdout)
            if o in ('-d', '--debug'):
                self.messages.setloglevel(DebugLevel, sys.stdout)
            if o in ('-h', '--help'):
                return 0
##            if o in ('-l', '--log'):
##                f = open(a, 'a')
##                self.messages.addlogger(DebugLevel, f)
            for k in self.inputs.keys():
                if a and (o == ('--' + k) or o == ('-' + k[0])):
                    if type(self.inputs[k]) == list:
                        for l in string.split(a,','):
                            self.inputs[k].append(l)
                    else:
                        if type(self.inputs[k]) == bool:
                            if a == 'T':
                                self.inputs[k] = True
                            else:
                                self.inputs[k] = False
                        else:
                            self.inputs[k] = a
        return 1

    def check_inputs(self):
        """Check if there are no empty inputs w/o a default setting.
        We assume all inputs either need defaults or need to be initialized by the user
        this doesn't work for ints/floats with default value 0, instead of False use 'F'"""
        for k in self.inputs.keys():
            if not self.inputs[k]:
                if type(self.inputs[k]) == bool: continue
                self.print_debug(self.name + ' missing argument ' + k)
                return 0
        return 1

    def main(self):
        """This function is to be run in standalone mode."""
        import sys, os.path
        self.name = os.path.basename(sys.argv[0]).rstrip('.py')
        if self.main_init():
            self.run(self.name)
            self.main_result()
        else:
            self.help()

    def run(self, name):
        """This code will run if all inputs are valid, and wraps the actual functionality 
        in self.go() with some exception handling, might need another name, like try_execute,
        because it's to similar to go()."""
        self.name = name
        self.messages.append(NotifyLevel, 'recipe ' + name + ' started')
        try:
            self.go()
        except Exception, e:
            self.messages.append(ErrorLevel, 'Exception caught: ' + str(e))
            self.messages.append(ErrorLevel, 'recipe ' + name + ' failed')
            self.outputs = None ## We're not generating any results we have confidence in
            return 0
        else:
            self.messages.append(NotifyLevel, 'recipe ' + name + ' completed')
            return 1
    
    def get_run_info(self, filepath):
        import pickle
        try:
            fd = open(filepath + '/pipeline.pickle')
            results = pickle.load(fd)
        except:
            return None
        fd.close()
        if self.name in results.keys():
            return results[self.name]
        else:
            return None
    
    def set_run_info(self, filepath):
        import pickle
        try:
            fd = open(filepath + '/' + 'pipeline.pickle', 'w')
            try:
              results = pickle.load(fd)
            except:
              results = {}
            results[self.name] = {'inputs':self.inputs, 'outputs':self.outputs}
            pickle.dump(results, fd)
            fd.close()
        except:
            return None
    
    def rerun(self, name, directory):
        """Function that you can use to rerun a recipe from the point where it ended.
        Not working completely yet. [untested]"""
        self.name = name
        self.messages.append(NotifyLevel, 'recipe ' + name + ' started')
        try:
            results = self.get_run_info(directory)
            if not results: return
            if not results[self.name]: return
            self.inputs  = results[self.name]['inputs']
            self.outputs = results[self.name]['outputs']
            self.run()
        except Exception, e:
            self.messages.append(ErrorLevel, 'Exception caught: ' + str(e))
            self.messages.append(ErrorLevel, 'recipe ' + name + ' failed')
            self.outputs = None ## We're not generating any results we have confidence in
            return 0
        else:
            self.messages.append(NotifyLevel, 'recipe ' + name + ' completed')
            return 1

    def go(self):
        """Main functionality, this empty placeholder only shows help"""
        self.help()

    def main_result(self):
        """Main results display for stand alone execution, displaying results on stdout"""
        if self.outputs == None:
            print 'No results'
        else:
            print 'Results:'
            for o in self.outputs.keys():
                print str(o) + ' = ' + str(self.outputs[o])

    ## Maybe these cooks should go in some subclass?
    ## Problem is you might need all of them in a recipe describing a pipeline
    def cook_recipe(self, recipe, inputs, outputs, messages=None):
        """Execute another recipe/pipeline as part of this one"""
        if not messages == None:
            c = cook.PipelineCook(recipe, inputs, outputs, messages)
        else:
            c = cook.PipelineCook(recipe, inputs, outputs, self.messages)
        c.spawn()

    def cook_system(self, command, options):
        """Execute an arbitrairy system call, returns it's exitstatus"""
        l = [command]
        if type(options) == list:
            l.extend(options)
        else: ## we assume it's a string
            l.extend(options.split())
        self.print_debug('running ' + command + ' ' + str(options))
        c = cook.SystemCook(command, l, {}, self.messages)
        c.spawn()
        while c.handle_messages():
            pass ## we could have a timer here
        c.wait()
        return c.exitstatus ## just returning the exitstatus, the programmer must decide what to do

    def cook_interactive(self, command, options, expect):
        """Execute an arbitrairy system call, returns it's exitstatus, expect can define some functions
        to call if certain strings are written to the terminal stdout of the called program.
        Whatever such functions return is written to the stdin of the called program."""
        commandline = [command]
        if type(options) == list:
            commandline.extend(options)
        else: ## we assume it's a string
            commandline.extend(options.split())
        self.print_debug('running ' + command + ' ' + str(options))
        c = cook.SystemCook(command, commandline, {}, self.messages)
        c.set_expect(expect)
        c.spawn()
        while c.handle_messages():
            pass ## we could have a timer here
        c.wait()
        return c.exitstatus ## just returning the exitstatus, the programmer must decide what to do

    def cook_miriad(self, command, options):
        """Execute a Miriad task, uses MRIBIN, returns it's exitstatus"""
        l = [command]
        if type(options) == list:
            l.extend(options)
        else: ## we assume it's a string
            l.extend(options.split())
        self.print_debug('running ' + command + str(options))
        c = cook.MiriadCook(command, l, {}, self.messages)
        c.spawn()
        while c.handle_messages():
            pass ## we could have a timer here
        c.wait()
        # should probably parse the messages on '### Fatal Error'
        if c.exitstatus: raise RecipeError('%s failed with error: %s' % (command, c.exitstatus))
        return c.exitstatus ## just returning the exitstatus, the programmer must decide what to do

##    def cook_aips(self, command, options):
##        """Execute an AIPS task, returns it's exitstatus"""
##        l = [command]
##        if type(options) == list:
##            l.extend(options)
##        else: ## we assume it's a string
##            l.extend(options.split())
##        self.print_debug('running ' + command + str(options))
##        c = cook.AIPSCook(command, l, {}, self.messages)
##        c.spawn()
##        while c.handle_messages():
##            pass ## we could have a timer here
##        c.wait()
##        return c.exitstatus ## just returning the exitstatus, the programmer must decide what to do
##
##    def cook_aips2(self, command, options):
##        """Execute an AIPS++ tool, returns it's exitstatus""" #?
##        l = [command]
##        if type(options) == list:
##            l.extend(options)
##        else: ## we assume it's a string
##            l.extend(options.split())
##        self.print_debug('running ' + command + str(options))
##        c = cook.AIPS2Cook(command, l, {}, self.messages)
##        c.spawn()
##        while c.handle_messages():
##            pass ## we could have a timer here
##        c.wait()
##        return c.exitstatus ## just returning the exitstatus, the programmer must decide what to do

    def cook_glish(self, command, options):
        """Execute a Glish script, uses AIPSPATH, returns it's exitstatus"""
        l = ['glish', '-l', command + '.g']
        if type(options) == list:
            l.extend(options)
        else: ## we assume it's a string
            l.extend(options.split())
        self.print_debug('running ' + command + str(options))
        c = cook.GlishCook('glish', l, {}, self.messages)
        c.spawn()
        while c.handle_messages():
            pass ## we could have a timer here
        c.wait()
        return c.exitstatus ## just returning the exitstatus, the programmer must decide what to do

    def print_debug(self, text):
        """Add a message at the debug level"""
        self.messages.append(DebugLevel, text)
    
    def print_message(self, text):
        """Add a message at the verbose level"""
        self.messages.append(VerboseLevel, text)
    
    def print_notification(self, text):
        """Add a message at the notification level"""
        self.messages.append(NotifyLevel, text)
    
    def print_error(self, text):
        """Add a message at the error level, should raise an Exception after this."""
        self.messages.append(ErrorLevel, text)

# The feeling is this needs to be part of the ingredient, or separate module, not the recipe
    def zap(self, filepath):
        import os #, exception
    #    if filepath == '/' or filepath == '~/':
    #      raise Exception
    #    else:
    #      for root, dirs, files in os.walk(filepath, topdown=False):
    #        for name in files:
    #            os.remove(join(root, name))
    #        for name in dirs:
    #            os.rmdir(join(root, name))
        if os.path.isdir(filepath):
            self.cook_system('rm', ' -rf ' + filepath)
        elif os.path.isfile(filepath):
            self.cook_system('rm', ' -f ' + filepath)
        else:
            self.print_debug(filepath + ' doesn\'t seem to exist')

# Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    standalone = WSRTrecipe()
    standalone.main()

#!/usr/bin/env python
import ingredient, cook, parset
import sys

from optparse import OptionParser

####
# Use the standard Python logging module for flexibility.
# Standard error of external jobs goes to logging.WARN, standard output goes
# to logging.INFO.
import logging

from traceback import format_exc

class RecipeError(cook.CookError):
    pass

class WSRTrecipe(object):
    """Base class for recipes, pipelines are created by calling the cook_*
    methods.  Most subclasses should only need to reimplement go() and add
    inputs and outputs.  Some might need to addlogger() to messages or
    override main_results."""
    def __init__(self):
        ## List of inputs, self.inputs[key] != True is considered valid input
        self.inputs   = ingredient.WSRTingredient()
        ## List of outputs, should only be filled on succesful execution
        self.outputs  = ingredient.WSRTingredient()
        ## All of these should do something sensible in their __str__ for
        ## simple print output

        ## Try using the standard Python system for handling options
        self.optionparser = OptionParser(
            usage="usage: %prog [options]"
        )
        self.optionparser.remove_option('-h')
        self.optionparser.add_option(
            '-h', '--help', action="store_true"
        )
        self.optionparser.add_option(
            '-v', '--verbose',
            action="callback", callback=self.__setloglevel,
            help="verbose [default=%default]"
        )
        self.optionparser.add_option(
            '-d', '--debug',
            action="callback", callback=self.__setloglevel,
            help="debug [default=%default]"
        )

        self.helptext = """LOFAR/WSRT pipeline framework"""

    def help(self):
        """Shows helptext and inputs and outputs of the recipe"""
        print self.helptext
        self.optionparser.print_help()
        print '\nOutputs:'
        for k in self.outputs.keys():
            print '  ' + k

    def main_init(self):
        """Main initialization for stand alone execution, reading input from
        the command line"""
        logging.basicConfig(
            format="%(asctime)s - %(levelname)s - %(name)s:  %(message)s",
            datefmt="%Y-%m-%d %H:%M:%S"
        )
        self.logger = logging.getLogger(self.name)
        opts = sys.argv[1:]
        try:
            myParset = parset.Parset(self.name + ".parset")
            for p in myParset.keys():
                opts[0:0] = "--" + p, myParset.getString(p)
        except IOError:
            logging.debug("Unable to open parset")
        (options, args) = self.optionparser.parse_args(opts)
        self.inputs = vars(options)
        self.args = args
        if options.help:
            return 1
        else:
            return 0

    def main(self):
        """This function is to be run in standalone mode."""
        import os.path
        self.name = os.path.splitext(os.path.basename(sys.argv[0]))[0]
        status = self.main_init()
        if not status:
            status = self.run(self.name)
            self.main_result()
        else:
            self.help()
        return status

    def run(self, name):
        """This code will run if all inputs are valid, and wraps the actual
        functionality in self.go() with some exception handling, might need
        another name, like try_execute, because it's to similar to go()."""
        self.name = name
        self.logger.info('recipe ' + name + ' started')
        try:
            status = self.go()
        except Exception, e:
            self.logger.exception('Exception caught: ' + str(e))
            self.outputs = None ## We're not generating any results we have
                                ## confidence in
            return 1
        else:
            self.logger.info('recipe ' + name + ' completed')
            return status

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
        """Function that you can use to rerun a recipe from the point where it
        ended.  Not working completely yet. [untested]"""
        self.name = name
        self.logger.info('recipe ' + name + ' started')
        try:
            results = self.get_run_info(directory)
            if not results: return
            if not results[self.name]: return
            self.inputs  = results[self.name]['inputs']
            self.outputs = results[self.name]['outputs']
            self.run(name)
        except Exception, e:
            self.logger.exception('Exception caught: ' + str(e))
            self.outputs = None ## We're not generating any results we have
                                ## confidence in
            return 0
        else:
            self.logger.info('recipe ' + name + ' completed')
            return 1

    def go(self):
        """Main functionality, this empty placeholder only shows help"""
        self.help()

    def main_result(self):
        """Main results display for stand alone execution, displaying results
        on stdout"""
        if self.outputs == None:
            print 'No results'
        else:
            print 'Results:'
            for o in self.outputs.keys():
                print str(o) + ' = ' + str(self.outputs[o])

    ## Maybe these cooks should go in some subclass?
    ## Problem is you might need all of them in a recipe describing a pipeline
    def cook_recipe(self, recipe, inputs, outputs):
        """Execute another recipe/pipeline as part of this one"""
        c = cook.PipelineCook(recipe, inputs, outputs, self.logger)
        c.spawn()

    def cook_system(self, command, options):
        """Execute an arbitrairy system call, returns it's exitstatus"""
        l = [command]
        if type(options) == list:
            l.extend(options)
        else: ## we assume it's a string
            l.extend(options.split())
        self.print_debug('running ' + command + ' ' + str(options))
        c = cook.SystemCook(command, l, {})
        c.spawn()
        while c.handle_messages():
            pass ## we could have a timer here
        c.wait()
        return c.exitstatus ## just returning the exitstatus, the programmer
                            ## must decide what to do

    def cook_interactive(self, command, options, expect):
        """Execute an arbitrairy system call, returns it's exitstatus, expect
        can define some functions to call if certain strings are written to
        the terminal stdout of the called program.
        Whatever such functions return is written to the stdin of the called
        program."""
        commandline = [command]
        if type(options) == list:
            commandline.extend(options)
        else: ## we assume it's a string
            commandline.extend(options.split())
        self.print_debug('running ' + command + ' ' + str(options))
        c = cook.SystemCook(command, commandline, {})
        c.set_expect(expect)
        c.spawn()
        while c.handle_messages():
            pass ## we could have a timer here
        c.wait()
        return c.exitstatus ## just returning the exitstatus, the programmer
                            ## must decide what to do

    def cook_miriad(self, command, options):
        """Execute a Miriad task, uses MRIBIN, returns it's exitstatus"""
        l = [command]
        if type(options) == list:
            l.extend(options)
        else: ## we assume it's a string
            l.extend(options.split())
        self.print_debug('running ' + command + str(options))
        c = cook.MiriadCook(command, l, {})
        c.spawn()
        while c.handle_messages():
            pass ## we could have a timer here
        c.wait()
        # should probably parse the messages on '### Fatal Error'
        if c.exitstatus:
            raise RecipeError('%s failed with error: %s' %
                              (command, c.exitstatus))
        return c.exitstatus ## just returning the exitstatus, the programmer
                            ## must decide what to do

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
        c = cook.GlishCook('glish', l, {})
        c.spawn()
        while c.handle_messages():
            pass ## we could have a timer here
        c.wait()
        return c.exitstatus ## just returning the exitstatus, the programmer
                            ## must decide what to do

    def print_debug(self, text):
        """Add a message at the debug level"""
        self.logger.debug(text)

    def print_message(self, text):
        """Add a message at the verbose level"""
        self.logger.info(text)
    print_notification = print_message # backwards compatibility

    def print_warning(self, text):
        """Add a message at the warning level."""
        self.logger.warn(text)

    def print_error(self, text):
        """Add a message at the error level"""
        self.logger.error(text)

    def print_critical(self, text):
        """Add a message at the critical level"""
        self.logger.crit(text)


# The feeling is this needs to be part of the ingredient, or separate module,
# not the recipe
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

    def __setloglevel(self, option, opt_str, value, parser):
        """Callback for setting log level based on command line arguments"""
        if str(option) == '-v/--verbose':
            self.logger.setLevel(logging.INFO)
        elif str(option) == '-d/--debug':
            self.logger.setLevel(logging.DEBUG)


# Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    standalone = WSRTrecipe()
    sys.exit(standalone.main())

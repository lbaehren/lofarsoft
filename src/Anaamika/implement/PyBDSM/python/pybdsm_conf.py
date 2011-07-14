# IPython configuration file. This file initializes the IPython
# shell for everything needed by PyBDSM. It is called on the
# start-up of the IPython shell by pybdsm.py.
#
# Note that we adopt the convention for this UI of using
# lines of 72 characters max for doc strings and the
# start-up banner. However, the parameter list will fill
# the entire available terminal width to consume as few
# vertical lines as possible.
import ipy_defaults
import IPython.ipapi

ip = IPython.ipapi.get()
o = ip.options

# Enable autocalling, so that parentheses are not needed
# for commands (as in casapy).
o.autocall = 2

# Customize the prompt
o.prompt_in1 = 'BDSM [\#]: '

# Add custom autocomplete. 
def opts_completer(self, event):
    """ Returns a list of strings with possible completions."""
    import os
    import glob
    from bdsm.image import Image
    img = Image({'filename':''})
    opts = img.opts.get_names()

    # Split the command entered by user when TAB was pressed
    # and check for up to three components (from e.g. "par = val")
    cmd1 = (event.line).rsplit(None)[0]
    if len((event.line).rsplit(None)) > 1:
        cmd2 = (event.line).rsplit(None)[1]
    else:
        cmd2 = ''
    if len((event.line).rsplit(None)) > 2:
        cmd3 = (event.line).rsplit(None)[2]
    else:
        cmd3 = ''

    # First, check to see if user has entered a parameter name
    # and an equals sign. If so, check parameter type. If Enum
    # or Option, match only to the allowable values.
    # Allowable values are available from v._type.values if v is
    # type Enum (v has no attribute _type.values if not).
    if "=" in cmd1 or "=" in cmd2:
        par_vals = []
        if "=" in cmd1:
            cmd3 = cmd1.split('=')[1]
            cmd1 = cmd1.split('=')[0]
        if cmd1 in opts:
            from bdsm.tc import tcEnum, tcOption
            v = img.opts.__class__.__dict__[cmd1]
            partype = v._type
            if isinstance(partype, tcOption):
                par_vals = ['None']
            elif isinstance(partype, tcEnum):
                if ('"' in cmd2 or "'" in cmd2 or
                    '"' in cmd3 or "'" in cmd3):
                    par_vals = v._type.values
                    if not isinstance(par_vals, list):
                        par_vals = list(par_vals)
                    if None in par_vals:
                        # Remove None from list
                        pindx = par_vals.index(None)
                        par_vals.pop(pindx)
                else:
                    if None in v._type.values:
                        par_vals.append('None')
                    if True in v._type.values:
                        par_vals.append('True')
                    if False in v._type.values:
                        par_vals.append('False')
            elif v._default == True or v._default == False:
                par_vals = ['True', 'False']
        if cmd1 == 'filename' or cmd1 == 'outfile':
            if ('"' in cmd2 or "'" in cmd2 or
                    '"' in cmd3 or "'" in cmd3):
                # Also add files in current directory
                found = [f.replace('\\','/') for f in glob.glob('*')]
                if len(found) > 0:
                    for fnd in found:
                        par_vals.append(fnd)
        return par_vals
    elif cmd1 == 'inp' or cmd1 == 'go':
        # Match task names only
        cmds = ['process_image', 'write_gaul', 'export_image', 'show_fit']
        return cmds
    elif cmd1 == 'cd' or cmd1 == 'tput' or cmd1 == 'tget':
        # Match to files in current directory (force use of ' or " with
        # tput and tget, as filename must be a string).
        files = []
        found = [f.replace('\\','/') for f in glob.glob('*')]
        if len(found) > 0:
            for fnd in found:
                files.append(fnd)
        if cmd1 == 'tput' or cmd1 == 'tget' and not ('"' in cmd2 or
                                                     "'" in cmd2):
            # User has not (yet) started to enter a string, so don't
            # return filenames
            return []
        return files
    elif cmd1 == 'help':
        if '"' in cmd2 or "'" in cmd2:
            # User has started to enter a string:
            # Match to parameter names, as they must be strings
            par_vals = opts
            return par_vals
        else:
            # User has not started to enter a string:
            # Match to commands + tasks only
            cmds = ['process_image', 'write_gaul', 'export_image', 'show_fit',
                    'go', 'inp', 'tget', 'tput', 'default']
            return cmds
    else:
        # Match to parameter, task, and command names only
        # Add command names
        opts.append('inp')
        opts.append('go')
        opts.append('tget')
        opts.append('tput')
        opts.append('default')
        opts.append('help')

        # Add task names
        opts.append('process_image')
        opts.append('show_fit')
        opts.append('write_gaul')
        opts.append('export_image')
        return opts
ip.set_hook('complete_command', opts_completer, re_key = '.*')

# Finally, print a welcome banner
from bdsm import __version__, __revision__
print 'PyBDSM version ' + __version__ + ' (LUS revision ' + __revision__ + ')'
print '=' * 72
print 'PyBDSM commands'
print '  inp task ............ : Set current task and list parameters'
print "  par = val ........... : Set a parameter (par = '' sets it to default)"
print '                          Autocomplete (with TAB) works for par and val'
print '  go .................. : Run the current task'
print '  default ............. : Set current task parameters to default values'
print "  tput ................ : Save parameters values"
print "  tget ................ : Load parameters values"
print 'PyBDSM tasks'
print '  process_image ....... : Process an image: find sources, etc.'
print '  show_fit ............ : Show the results of a fit'
print '  write_gaul .......... : Write out list of sources to a file' 
print '  export_image ........ : Write residual/model/rms/mean image to a file'
print 'PyBDSM help'
print '  help command/task ... : Get help on a command or task'
print '                          (e.g., help process_image)'
print "  help 'par' .......... : Get help on a parameter (e.g., help 'rms_box')"
print '_' * 72

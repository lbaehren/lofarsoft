#! /usr/bin/python
# Custom IPython shell for interactive PyBDSM use.
from IPython.Shell import IPShellEmbed
import bdsm
from bdsm.image import Image
import pydoc
import sys
import inspect

###############################################################################
# Functions needed only in the custom IPython shell are defined here. Other
# functions used by both the custom shell and normal Python or IPython
# environments are defined in interface.py.
#
# Before starting the IPython shell, we need to define all the functions and
# variables that we want in the namespace. Note that we adopt the convention
# for this UI of using lines of 72 characters max for doc strings and the
# start-up banner. However, the parameter list will fill the entire available
# terminal width to consume as few vertical lines as possible.
global _img
_img = Image({'filename':'', 'output_all':False})
T = True
F = False
true = True
false = False

def inp(cur_cmd=None):
    """List inputs for current task.

    If a task is given as an argument, inp sets the current task
    to the given task. If no task is given, inp lists the parameters
    of the current task.
    """
    global _img
    success = _set_pars_from_prompt()
    if not success:
        return
    if cur_cmd != None:
        if not hasattr(cur_cmd, 'arg_list'):
            print '\033[31;1mERROR\033[0m: not a valid task'
            return
        _set_current_cmd(cur_cmd)
    bdsm.interface.list_pars(_img, opts_list=_img._current_cmd_arg_list,
                             banner=_img._current_cmd_desc,
                             use_groups=_img._current_cmd_use_groups)
    
    
def go(cur_cmd=None):
    """Executes the current task.

    If a task is given as an argument, go executes the given task,
    even if it is not the current task. The current task is not
    changed.
    """
    global _img
    success = _set_pars_from_prompt()
    if not success:
        return
    if cur_cmd == None:
        cur_cmd = _img._current_cmd
    cur_cmd()

        
def default(cur_cmd=None):
    """Resets all parameters for a task to their default values.

    If a task name is given (e.g., "default show_fit"), the
    parameters for that task are reset. If no task name is
    given, the parameters of the current task are reset.
    """
    global _img
    if cur_cmd == None:
        cur_cmd = _img._current_cmd

    if hasattr(cur_cmd, 'arg_list'):
        opts_list = cur_cmd.arg_list
    else:
        print '\033[31;1mERROR\033[0m: not a valid task'
        return
    _img.opts.set_default(opts_list)
    _replace_vals_in_namespace(opt_names=opts_list)

                
def tget(filename=None):
    """Load processing parameters from a parameter save file.

    A file name may be given (e.g., "tget 'savefile.sav'"), in which case the
    parameters are loaded from the file specified. If no file name is given,
    the parameters are loaded from the file 'pybdsm.last' if it exists.
    
    Normally, the save file is created by the tput command (try "help tput"
    for more info).
    
    The save file is a "pickled" python dictionary which can be loaded into
    python and edited by hand. See the pickle module for more information.
    Below is an example of how to edit a save file by hand:
    
      BDSM [1]: import pickle
      BDSM [2]: savefile = open('savefile.sav', 'w')
      BDSM [3]: pars = pickle.load(savefile)
      BDSM [4]: pars['rms_box'] = (80, 20)  --> change rms_box parameter
      BDSM [5]: pickle.dump(pars, savefile) --> save changes

    """
    try:
        import cPickle as pickle
    except ImportError:
        import pickle
    import os
    
    global _img
    if filename == None or filename == '':
        if os.path.isfile('pybdsm.last'):
            filename = 'pybdsm.last'
        else:
            print '\033[31;1mERROR\033[0m: No file name given and "pybdsm.last" not found.\n'\
                'Please specify a file to load.'
            return
        
    if os.path.isfile(filename):
        try:
            pkl_file = open(filename, 'rb')
            pars = pickle.load(pkl_file)
            pkl_file.close()
            _img.opts.set_opts(pars)
            _replace_vals_in_namespace()
            print "--> Loaded parameters from file '" + filename + "'."
        except:
            print "\033[31;1mERROR\033[0m: Could not read file '" + filename + "'."
    else:
        print "\033[31;1mERROR\033[0m: File '" + filename + "' not found."
        

def tput(filename=None, quiet=False):
    """Save processing parameters to a file.

    A file name may be given (e.g., "tput 'savefile.sav'"), in which case the
    parameters are saved to the file specified. If no file name is given, the
    parameters are saved to the file 'pybdsm.last'. The save parameter can be
    loaded using the tget command (try "help tget" for more info).
    
    The save file is a "pickled" python dictionary which can be loaded into
    python and edited by hand. See the pickle module for more information.
    Below is an example of how to edit a save file by hand:
    
      BDSM [1]: import pickle
      BDSM [2]: savefile = open('savefile.sav', 'w')
      BDSM [3]: pars = pickle.load(savefile)
      BDSM [4]: pars['rms_box'] = (80, 20)  --> change rms_box parameter
      BDSM [5]: pickle.dump(pars, savefile) --> save changes

    """
    try:
        import cPickle as pickle
    except ImportError:
        import pickle

    global _img
    success = _set_pars_from_prompt()
    if not success:
        return
    if filename == None or filename == '':
        filename = 'pybdsm.last'
        
    # convert opts to dictionary
    pars = _img.opts.to_dict()
    output = open(filename, 'wb')
    pickle.dump(pars, output)
    output.close()
    if not quiet:
        print "--> Saved parameters to file '" + filename + "'."

        
def _set_pars_from_prompt():
    """Gets parameters and value and stores them in _img.

    To do this, we extract all the valid parameter names
    and values from the f_locals directory. Then, use
    set_pars() to set them all.

    Returns True if successful, False if not.
    """
    global _img
    f = sys._getframe(len(inspect.stack())-1)
    f_dict = f.f_locals
    
    # Check through all possible options and
    # build options dictionary
    opts = _img.opts.to_dict()
    user_entered_opts = {}
    for k, v in opts.iteritems():
        if k in f_dict:
            if f_dict[k] == '':
                # Set option to default value in _img and namespace
                _img.opts.set_default(k)
                f_dict[k] = _img.opts.__getattribute__(k)
            user_entered_opts.update({k: f_dict[k]})

    # Finally, set the options
    try:
        _img.opts.set_opts(user_entered_opts)
        return True
    except RuntimeError, err:
        # If an opt fails to set, replace its value in the namespace
        # with its current value in _img. Then print error so user knows.
        err_msg = str(err)
        indx1 = err_msg.find('"') + 1
        indx2 = err_msg.find('"', indx1)
        k = err_msg[indx1:indx2]
        orig_opt_val = opts[k]
        f_dict[k] = orig_opt_val
        print '\033[31;1mERROR\033[0m: ' + str(err) + ' Resetting to previous value.'
        return False

    
def _replace_vals_in_namespace(opt_names=None):
    """Replaces opt values in the namespace with the ones in _img.

    opt_names - list of option names to replace (can be string if only one)
    """
    global _img
    f = sys._getframe(len(inspect.stack())-1)
    f_dict = f.f_locals
    if opt_names == None:
        opt_names = _img.opts.get_names()
    if isinstance(opt_names, str):
        opt_names = [opt_names]
    for opt_name in opt_names:
        if opt_name in f_dict:
            f_dict[opt_name] = _img.opts.__getattribute__(opt_name)


def _set_current_cmd(cmd):
    """Sets information about current command in img.

    This function is used to emulate a casapy interface.
    
    """
    global _img
    cmd_name = cmd.__name__
    doc = cmd.__doc__
    _img._current_cmd = cmd
    _img._current_cmd_name = cmd_name
    _img._current_cmd_desc = cmd_name.upper() + ': ' + doc.split('\n')[0] 
    _img._current_cmd_arg_list = cmd.arg_list
    _img._current_cmd_use_groups = cmd.use_groups

###############################################################################
# Next, we define the tasks such that they may be called directly by
# the user if so desired. These functions simply pass on the user-
# specified arguments to the appropriate Image method. Here we also
# define the detailed doc strings used by help, and, after each task
# definition, we define its list of arguments and whether it should
# use the opts 'group' attribute, both needed when inp is called. If
# a new parameter is added to a task, it needs to be added to opts.py
# and to the list of arguments for the task below (the "arg_list")
# attribute.
def process_image(**kwargs):
    """Find and measure sources in an image.

    There are many possible parameters and options for process_image. Use
    "inp process_image" to list them. To get more information about a
    parameter, use help. E.g.,
    
    > help 'rms_box'

    When process_image is executed, PyBDSM performs the following steps in
    order:

    1. Reads in the image.

    2. Calculates basic statistics of the image and stores them in the Image
    object. Calculates sensible values of processing parameters and stores
    them. First calculates mean and rms, with and without (3-sigma)
    clipping, min and max pixel and values, solid angle. Hereafter, rms
    indicates the 3-sigma clipped measure. Next, the number of beams per
    source is calculated (see help on algorithms for details), using a
    sensible estimate of boxsize and stepsize (which can be set using the
    rms_box parameter). Finally, the thresholds are set. They can either be
    hard-thresholded (by the user or set as 5-sigma for pixel threshold and
    3-sigma for island boundaries internally) or can be calculated using the
    False Detection Rate (FDR) method using an user defined value for
    alpha. If the user does not specify whether hard thresholding or FDR
    should be applied, one or the other is chosen internally based on the
    ratio of expected false pixels and true pixels (the choice is written
    out in the log file).

    3. Calculates rms image. 3-sigma clipped rms and mean are calculated
    inside boxes of size boxsize in steps of stepsize. Intermediate values
    are calculated using bilinear interpolation (it was seen that bicubic
    spline did not yield appreciably better results but is also
    available). Depending on the resulting statistics (see help on
    algorithms for details), we either adopt the rms image or a constant rms
    in the following analysis.

    4. Identifies islands of contiguous emission. First all pixels greater
    than the pixel threshold are identified (and sorted by descending flux
    order). Next, starting from each of these pixels, all contiguous pixels
    (defined by 8-connectivity, i.e., the surrounding eight pixels) higher
    than the island boundary threshold are identified as belonging to one
    island, accounting properly for overlaps of islands.

    5. Fit multiple gaussians and/or shapelets to each island. For each
    island, the subimages of emission and rms are cut out. The number of
    multiple gaussians to be fit can be determined by three different
    methods (see help on algorithms for details). With initial guesses
    corresponding to these peaks, gaussians are simultaneously fit to the
    island using the Levenberg-Marqhardt algorithm. Sensible criteria for bad
    solutions are defined. If multiple gaussians are fit and one of them is
    a bad solution then the number of gaussians is decreased by one and fit
    again, till all solutions in the island are good (or zero in number, in
    which case its flagged). After the final fit to the island, the
    deconvolved size is computed assuming the theoretical beam and the
    statistics in the source area and in the island are computed and
    stored. Errors on each of the fitted parameters are computed using the
    formulae in Condon (1997). Finally all good solutions are written into
    the gaussian catalog as an ascii and binary file. If shapelets are
    required, the program calculates optimal nmax, beta and the centre, and
    stores these and the shapelet coefficients in a file.
    
    """
    global _img
    success = _set_pars_from_prompt()
    if not success:
        return
    # Save current command, as it might be overwritten when process
    # is called by the user directly and is not the current command.
    cur_cmd = _img._current_cmd
        
    # Run process. Note that process automatically picks up options
    # from the Image object, so we don't need to get_task_kwargs as
    # we do for the other tasks.
    success = _img.process(**kwargs)

    # Now restore parameters and save to pybdsm.last
    if success:
        _set_current_cmd(cur_cmd)
        tput(quiet=True)
        
task_list = _img.opts.get_names()
process_image.arg_list = task_list
process_image.use_groups = True


def show_fit(**kwargs):
    """Show results of fit.

    Selected plots are displayed to give the user a quick overview of the
    results of the fit. The plots may be zoomed, saved to a file, etc. using
    the controls at the bottom of the plot window.

    In addition, the following commands are available:
      Press "i" ........ : Get integrated fluxes and mean rms values
                           for the visible portion of the image
      Press "m" ........ : Change min and max scaling values
      Press "n" ........ : Show / hide island IDs
      Press "0" ........ : Reset scaling to default
      Press "c" ........ : Change source for SED plot
      Click Gaussian ... : Print Gaussian and source IDs (zoom_rect mode, 
                           toggled with the "zoom" button and indicated in 
                           the lower right corner, must be off)
                           The SED plot will also show the chosen source.
                           
    Parameters: ch0_image, rms_image, mean_image, ch0_islands,
                gresid_image, sresid_image, gmodel_image,
                smodel_image, pyramid_srcs, source_seds,
                ch0_flagged

    For more information about a parameter, use help.  E.g.,
      > help 'ch0_image'
      
    """
    global _img
    success = _set_pars_from_prompt()
    if not success:
        return
    img_kwargs = _get_task_kwargs(show_fit)
    for k in kwargs:
        # If user enters an argument, use it instead of
        # that in _img
        img_kwargs[k] = kwargs[k]
    try:
        _img.show_fit(**img_kwargs)
        tput(quiet=True)
    except KeyboardInterrupt:
        print "\n\033[31;1mAborted\033[0m"
        
show_fit.arg_list = ['ch0_image', 'rms_image', 'mean_image', 'ch0_islands',
                     'gresid_image', 'sresid_image', 'gmodel_image',
                     'smodel_image', 'pyramid_srcs', 'source_seds',
                     'ch0_flagged']
show_fit.use_groups = False

    
def write_gaul(**kwargs):
    """Write the Gaussian list to a file.

    The Gaussian list can be written in a number of formats. The information
    included in the output file varies with the format used. Use
    "help 'format'" for more information.

    Parameters: outfile, format, srcroot, bbs_patches, incl_wavelet, clobber

    For more information about a parameter, use help.  E.g.,
      > help 'bbs_patches'

    The ASCII and FITS output files contain the following columns:
    
      1. Gaus_id - Gaussian id number : starting from zero.
      2. Isl_id - Island id number : the id of the island the Gaussian
         belongs to.
      3. Source_id - Source id number : the id of the source the Gaussian
         is grouped into. Note that if the Gaussians are not grouped into
         sources, they have source id = 0 and source code = 'X'.
      4. Wave_id - Wavelet id : For the original image, this is 0 and for
         the rest, is the wavelet order of the image.
      5. Total - Total flux, in Jy.
      6. E_Total - Error in total flux, in Jy.
      7. Peak - Peak flux density, in Jy/beam.
      8. E_Peak - Error in peak flux density, in Jy/beam.
      9. RA - R.A. of the centre of the Gaussian, in degrees.
     10. E_RA - Error in R.A. of the centre of the Gaussian, in degrees.
     11. Dec - Dec of the centre of the Gaussian, in degrees.
     12. E_Dec - Error in Dec of the centre of the Gaussian, in degrees.
     13. Xposn - x-position of the centre of the Gaussian, in pixels.
     14. E_Xposn - Error in x-position of the centre of the Gaussian,
         in pixels.
     15. Yposn - y-position of the centre of the Gaussian, in pixels.
     16. E_Yposn - Error in y-position of the centre of the Gaussian,
         in pixels.
     17. Bmaj - Major axis FWHM of fitted Gaussian, in degrees.
     18. E_Bmaj - Error in major axis FWHM of fitted Gaussian, in degrees.
     19. Bmin - Minor axis FWHM of fitted Gaussian, in degrees.
     20. E_Bmin - Error in minor axis FWHM of fitted Gaussian, in degrees.
     21. Bpa - Position angle of fitted Gaussian, in degrees.
     22. E_Bpa - Error in position angle of fitted Gaussian, in degrees.
     23. DC Bmaj - Deconvolved major axis FWHM of fitted Gaussian, in degrees.
         Zero if unresolved.
     24. E_DC_Bmaj - Error in deconvolved major axis FWHM of fitted Gaussian,
         in degrees.
     25. DC Bmin - Deconvolved minor axis FWHM of fitted Gaussian, in degrees.
         Zero if unresolved.
     26. E_DC_Bmin - Error in deconvolved minor axis FWHM of fitted Gaussian,
         in degrees.
     27. DC Bpa - Deconvolved position angle of fitted Gaussian, in degrees.
     28. E_DC_Bpa - Error in deconvolved position angle of fitted Gaussian,
         in degrees.
     29. I_rms - The rms for the island, taken from the background rms image,
         in Jy/beam.
     30. I_mean - The mean for the island, taken from the background rms
         image, in Jy/beam.
     31. I_R_rms - The residual rms in the island after Gaussian subtraction,
         in Jy/beam.
     32. I_R_mean - The residual mean in the island after Gaussian
         subtraction, in Jy/beam.
     33. S_code - The code ('S', 'C' or 'M') of the source to which the
         Gaussian belongs
     
    """
    global _img
    success = _set_pars_from_prompt()
    if not success:
        return
    img_kwargs = _get_task_kwargs(write_gaul)
    for k in kwargs:
        # If user enters an argument, use it instead of
        # that in _img
        img_kwargs[k] = kwargs[k]
    try:
        _img.write_gaul(**img_kwargs)
        tput(quiet=True)
    except KeyboardInterrupt:
        print "\n\033[31;1mAborted\033[0m"

write_gaul.arg_list = ['bbs_patches', 'format', 'outfile', 'srcroot',
                       'incl_wavelet', 'clobber']
write_gaul.use_groups = False


def export_image(**kwargs):
    """Write one or more images to a file.

    Parameters: filename, img_type, img_format, incl_wavelet, clobber

    For more information about a parameter, use help.  E.g.,
      > help 'img_type'

    """
    global _img
    success = _set_pars_from_prompt()
    if not success:
        return
    img_kwargs = _get_task_kwargs(export_image)
    for k in kwargs:
        # If user enters an argument, use it instead of
        # that in _img
        img_kwargs[k] = kwargs[k]
    try:
        _img.export_image(**img_kwargs)
        tput(quiet=True)
    except KeyboardInterrupt:
        print "\n\033[31;1mAborted\033[0m"
        
export_image.arg_list = ['outfile', 'img_type', 'img_format',
                         'incl_wavelet', 'clobber']
export_image.use_groups = False


def _get_task_kwargs(task):
    """Returns dictionary of keyword arguments from _img for the given task."""
    global _img
    arg_list = task.arg_list
    kwargs = {}
    for a in arg_list:
        kwargs.update({a: _img.opts.__getattribute__(a)})
    return kwargs


###############################################################################
# Customize the help system for PyBDSM
class bdsmDocHelper(pydoc.Helper):
    def help(self, request):
        global _img
        topbar = '_' * 72 + '\n' # 72-character divider
        if hasattr(request, '__name__'):
            pydoc.pager(topbar + 'Help on ' + pydoc.text.bold(request.__name__)
                        + ':\n\n' + pydoc.getdoc(request))
        else:
            opts = _img.opts.__class__.__dict__
            try:
                opt = opts[request]
                desc_list = str(opt.doc()).split('\n')
                desc = '\n\n'.join(desc_list)
                default_val = opt._default
                if isinstance(default_val, str):
                    valstr = "'" + default_val + "'"
                else:
                    valstr = str(default_val)
                default_val_text = 'Default value: ' + valstr
                if opt.group() != None and opt.group() != 'hidden':
                    group_text = '\nBelongs to group: ' + opt.group()
                else:
                    group_text = ''
                desc_text = bdsm.interface.wrap(desc, 72)
                desc_text = '\n'.join(desc_text)
                pydoc.pager(topbar + 'Help on the ' + pydoc.text.bold(request)
                            + ' parameter:\n\n' + default_val_text
                            + group_text
                            + '\n\n' + desc_text)
            except(KeyError):
                print "Parameter '" + request + "' not recognized."
pydoc.help = bdsmDocHelper(sys.stdin, sys.stdout)

    
# Now run the IPython shell with this namespace and the customisations
# in the pybdsm_conf.py file
_set_current_cmd(process_image)
ipshell = IPShellEmbed(['-prompt_in1','BDSM <\#>: ','-autocall','2'], user_ns=locals())
ipshell.IP.runlines('import pybdsm_conf')
ipshell()

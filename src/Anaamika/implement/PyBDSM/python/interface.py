"""Interface module.

The interface module handles all functions typically needed by the user in an
interactive environment such as IPython. Many are also used by the pybdsm.py
custom IPython shell. 

"""
import exceptions

def process(img, **kwargs):
    """Find and measure sources in an image.

    This function is used by process_image in __init__.py and by process_image
    in pybdsm.py. It is also used as a method of the Image object in image.py
    to allow reprocessing of existing Image objects with the command
    img.process().

    Any options given as keyword arguments will override existing ones.

    """
    from bdsm import default_chain, _run_op_list
    from image import Image
    import mylogger 

    # First, reset img to initial state (in case img is being reprocessed)
    if hasattr(img, 'use_io'): del img.use_io
    if hasattr(img, 'source'): del img.source
    if hasattr(img, 'gaussians'): del img.gaussians
    if hasattr(img, 'atrous_gaussians'): del img.atrous_gaussians
    if hasattr(img, 'islands'): del img.islands

    # Start up logger. We need to initialize it each time process() is
    # called, in case the quiet or debug options have changed
    log = img.opts.filename + '.pybdsm.log'
    img.log = ''
    mylogger.init_logger(log, quiet=img.opts.quiet,
                         debug=img.opts.debug)
    f = open(log, 'a')
    f.write('\n' + '='*72 + '\n')
    f.close()
    mylog = mylogger.logging.getLogger("PyBDSM.Process")
    mylog.info("Running PyBDSM on "+img.opts.filename)

    # Run all the op's
    try:
        # set options if given
        if len(kwargs) > 0:
            set_pars(img, **kwargs)
            
        # Run op's in chain
        op_chain = get_op_chain(img)
        _run_op_list(img, op_chain)   
        return True
    except RuntimeError, err:
        # Catch and log error
        mylog.error(str(err))
        return False
    except KeyboardInterrupt:
        mylogger.userinfo(mylog, "\n\033[31;1mAborted\033[0m")
        return False
        
def get_op_chain(img):
    """Determines the optimal Op chain for an Image object"""
    from bdsm import default_chain
    
    return default_chain
    # prev_opts = img._prev_opts
    # new_opts = img.opts.to_dict()
    
    # Find whether new opts differ from previous opts
    # If new_opts == prev_opts, don't do anything

    # If filename, beam, trim_box differ, start from readimage
    # Elif shapelet_do, etc. differ, start from there
    
    
def load_pars(filename):
    """Load parameters from a save file.

    The file must be a pickled opts dictionary.

    filename - name of options file to load.
    Returns None if no file can be loaded successfully.
    """
    from image import Image
    try:
        import cPickle as pickle
    except ImportError:
        import pickle

    try:
        pkl_file = open(filename, 'rb')
        pars = pickle.load(pkl_file)
        pkl_file.close()
        timg = Image(pars)
        print "--> Loaded parameters from file '" + filename + "'."
        return timg
    except:
        return None
        
def save_pars(img, savefile=None):
    """Save parameters to a file.
    
    The save file is a "pickled" opts dictionary.
    """
    try:
        import cPickle as pickle
    except ImportError:
        import pickle
    import tc
    import sys

    if savefile == None or savefile == '':
        savefile = img.opts.filename + '.pybdsm.sav'
        
    # convert opts to dictionary
    pars = img.opts.to_dict()
    output = open(savefile, 'wb')
    pickle.dump(pars, output)
    output.close()
    print "--> Saved parameters to file '" + savefile + "'."

def list_pars(img, opts_list=None, banner=None, use_groups=True):
    """Lists all parameters for the Image object.

    opts_list - a list of the parameter names to list;
                if None, all parameters are used.
    banner - banner text to place at top of listing.
    use_groups - whether to use the group information for each
                 parameter.
    """
    import tc
    import sys

    # Get all options as a list sorted by name
    opts = img.opts.to_list()

    # Filter list 
    if opts_list != None:
        opts_temp = []
        for o in opts:
            if o[0] in opts_list:
                opts_temp.append(o)
        opts = opts_temp
        
    # Move filename, infile, outfile to front of list
    for o in opts:
        if o[0] == 'filename' or o[0] == 'infile' or o[0] == 'outfile':
            opts.remove(o)
            opts.insert(0, o)

    # Now group options with the same "group" together.
    if use_groups:
        opts = group_opts(opts)
    
    # Finally, print options, values, and doc strings to screen
    print_opts(opts, img, banner=banner)

    
def set_pars(img, **kwargs):
    """Set parameters using arguments instead of using a dictionary.

    Allows partial names for parameters as long as they are unique. Parameters
    are set to default values if par = ''.
    """
    import re
    import sys
    from image import Image
    
    # Enumerate all options
    opts = img.opts.get_names()

    # Check that parameters are valid options and are unique
    full_key = []
    for i, key in enumerate(kwargs):
        chk_key = checkpars(opts, key)
        if chk_key == []:
            raise RuntimeError("Input parameter '" + key + "' not recognized.")
        if len(chk_key) > 1 and key not in opts:
            raise RuntimeError("Input parameter '" + key + "' matches to more than one"\
                         "possible parameter:\n " + "\n ".join(chk_key))
        if key in opts:
            full_key.append(key)
        else:
            full_key.append(chk_key[0])    
    
    # Build options dictionary
    pars = {}
    for i, key in enumerate(kwargs):
        if kwargs[key] == '':
            temp_img = Image({'filename':''})
            opt_names = temp_img.opts.get_names
            for k in opt_names:
                if key == k:
                    kwargs[key] = temp_img.opts.__getattribute__(k)
        pars.update({full_key[i]: kwargs[key]})

    # Finally, set the options
    img.opts.set_opts(pars)
    

def group_opts(opts):
    """Sorts options by group (as defined in opts.py).

    Returns a list of options, with suboptions arranged in a list inside the
    main list and directly following the main options. Options belonging to the
    "hidden" group are excluded from the returned list (as defined in opts.py).
    """
    groups = []
    gp = []
    for i in range(len(opts)):
        grp = opts[i][1].group()
        if grp != None and grp not in groups:
            groups.append(opts[i][1].group())

    groups.sort()

    # Now, make a list for each group with its options. Don't include
    # "hidden" options, as they should never by seen by the user.
    for g in groups:
        g_list = []
        for i in range(len(opts)):
            if isinstance(opts[i], tuple):
                if g == str(opts[i][1].group()):
                    g_list.append(opts[i])
        for gs in g_list:
            opts.remove(gs)

        for i in range(len(opts)):
            if g == str(opts[i][0]) and g != 'hidden':
                opts.insert(i+1, g_list)
                break
    return opts

                            
def print_opts(grouped_opts_list, img, banner=None):
    """Print options to screen.
    
    Options can be sorted by group (defined in opts.py) previously by
    group_opts. Output of grouped items is suppressed if parent option is
    False. The layout is as follows:
    
      [20 spaces par name with ...] = [at least 49 spaces for value]
                                      [at least 49 spaces for doc]
    
    When more than one line is required for the doc, the next line is:
    
      [25 blank spaces][at least 47 spaces for doc]

    As in casapy, print non-defaults in blue, options with suboptions in
    47m and suboptions in green. Option Values are printed in bold, to help
    to distinguish them from the descriptions. NOTE: in iTerm, one needs
    to set the bold color in the profiles to white, as it defaults to red,
    which is a bit hard on the eyes in this case.
    """
    from image import Image
    import os

    termx, termy = getTerminalSize()
    minwidth = 28 # minimum width for parameter names and values
    # Define colors for output
    dc = '\033[1;34m' # Blue: non-default option text color
    ec = '\033[0;47m' # expandable option text color
    sc = '\033[0;32m' # Green: suboption text color
    nc = '\033[0m'    # normal text color
    ncb = '\033[1m'    # normal text color bold

    if banner != None:
        print banner
    spcstr = ' ' * minwidth # spaces string for second or later lines
    infix = nc + ': ' + nc # infix character used to separate values from comments
    print '=' * termx # division string for top of parameter listing
    for indx, o in enumerate(grouped_opts_list):
        if isinstance(o, tuple):
            # Print main options, which are always tuples, before printing
            # suboptions (if any).
            k = o[0]
            v = o[1]
            val = img.opts.__getattribute__(k)
            v1 = v2 = ''
            if val == v._default:
                # value is default
                v1 = ncb
                v2 = nc
            else:
                # value is non-default
                v1 = dc
                v2 = nc
            if isinstance(val, str):
                valstr = v1 + repr(val) + v2
                if k == 'filename':
                    # Since we can check whether filename is valid,
                    # do so here and print in red if not.
                    if not os.path.exists(val):
                        valstr = '\033[31;1m' + repr(val) + nc
                width_par_val = max(minwidth, len(k) + len(str(val)) + 5)
            else:
                if isinstance(val, float):
                    val = round_float(val)
                if isinstance(val, tuple):
                    val = round_tuple(val)
                valstr = v1 + str(val) + v2
                width_par_val = max(minwidth, len(k) + len(str(val)) + 4)
            width_desc = max(termx - width_par_val - 3, 44)
            # Get the option description text from the doc string, which
            # is defined in opts.py. By convention, print_opts will only
            # show the short description; help('option_name') will
            # print both the short and long description. The versions
            # are separated in the doc string by '\n', which is split
            # on here:
            desc_text = wrap(str(v.doc()).split('\n')[0], width_desc)
            fmt = '%' + str(minwidth) + 's' + infix + '%44s'
                    
            # Now loop over lines of description
            if indx < len(grouped_opts_list)-1:
                # Here we check if next entry in options list is a tuple or a
                # list.  If it is a list, then the current option has
                # suboptions and should be in the ec color. Since we check the
                # next option, we can't do this if we let indx go to the end.
                if isinstance(grouped_opts_list[indx+1], tuple):
                    parvalstr = nc + k + nc + ' ..'
                else:
                    parvalstr = ec + k + nc + ' ..'
            else:
                # Since this is the last entry in the options list and is a
                # tuple, it cannot be an expandable option, so make it nc color
                 parvalstr = nc + k + nc + ' ..'
            if "'" in valstr:
                len_without_formatting = len(k) + len(str(val)) + 5
            else:
                len_without_formatting = len(k) + len(str(val)) + 4
            for i in range(len_without_formatting, minwidth):
                parvalstr += '.'
            parvalstr += ' ' + valstr
            if "'" not in valstr:
                parvalstr += ' '
            for dt_indx, dt in enumerate(desc_text):
                if dt_indx == 0:
                    print fmt % (parvalstr.ljust(minwidth), dt.ljust(44))
                else:
                    print nc + spcstr + '   %44s' % dt.ljust(44)
        else:
            # Print suboptions, indented 2 spaces from main options in sc color
            parent_opt = grouped_opts_list[indx-1]
            parent_val = img.opts.__getattribute__(parent_opt[0])
            if parent_val == True:
                for og in o:
                    k = og[0]
                    v = og[1]
                    val = img.opts.__getattribute__(k)
                    v1 = v2 = ''
                    if val == v._default:
                        # value is default
                        v1 = ncb
                        v2 = nc
                    else:
                        # value is non-default
                        v1 = dc
                        v2 = nc
                    if isinstance(val, str):
                        valstr = v1 + repr(val) + v2
                        width_par_val = max(minwidth, len(k) + len(str(val)) + 7)
                    else:
                        if isinstance(val, float):
                            val = round_float(val)
                        if k == 'beam_spectrum' and val != None:
                            val = round_list_of_tuples(val)
                        if k == 'frequency_sp' and val != None:
                            val = round_list(val)
                        valstr = v1 + str(val) + v2
                        width_par_val = max(minwidth, len(k) + len(str(val)) + 6)
                    width_desc = max(termx - width_par_val - 3, 44)
                    desc_text = wrap(str(v.doc()).split('\n')[0], width_desc)
                    fmt = '  ' + '%' + str(minwidth) + 's' + infix + '%44s'
                    parvalstr = sc + k + nc + ' ..'
                    if "'" in valstr:
                        len_without_formatting = len(k) + len(str(val)) + 7
                    else:
                        len_without_formatting = len(k) + len(str(val)) + 6
                    for i in range(len_without_formatting, minwidth):
                        parvalstr += '.'
                    parvalstr += ' ' + valstr
                    if "'" not in valstr:
                        parvalstr += ' '
                    for dt_indx, dt in enumerate(desc_text):
                        if dt_indx == 0:
                            print fmt % (parvalstr.ljust(minwidth-2), dt.ljust(44))
                        else:
                            print nc + spcstr + '   %44s' % dt.ljust(44)


def getTerminalSize():
    """Returns the x and y size of the terminal."""
    def ioctl_GWINSZ(fd):
        try:
            import fcntl, termios, struct, os
            cr = struct.unpack('hh', fcntl.ioctl(fd, termios.TIOCGWINSZ,
        '1234'))
        except:
            return None
        return cr
    cr = ioctl_GWINSZ(0) or ioctl_GWINSZ(1) or ioctl_GWINSZ(2)
    if not cr:
        try:
            fd = os.open(os.ctermid(), os.O_RDONLY)
            cr = ioctl_GWINSZ(fd)
            os.close(fd)
        except:
            pass
    if not cr:
        try:
            cr = (env['LINES'], env['COLUMNS'])
        except:
            cr = (25, 80)
    return int(cr[1]), int(cr[0])


def wrap(text, width=80):
    """Wraps text to given width and returns list of lines."""
    lines = []
    for paragraph in text.split('\n'):
        line = []
        len_line = 0
        for word in paragraph.split(' '):
            word.strip()
            len_word = len(word)
            if len_line + len_word <= width:
                line.append(word)
                len_line += len_word + 1
            else:
                lines.append(' '.join(line))
                line = [word]
                len_line = len_word + 1
        lines.append(' '.join(line))
    return lines


def checkpars(lines, regex):
    """Checks that parameters are unique"""
    import re
    result = []
    for l in lines:
        match = re.match(regex,l)
        if match:
            result += [l]
    return result


def in_ipython():
    """Checks if interpreter is IPython."""
    try:
        __IPYTHON__
    except NameError:
        return False
    else:
        return True

    
def raw_input_no_history(prompt):
    """Removes user input from readline history."""
    import readline
    input = raw_input(prompt)
    if input != '':
        readline.remove_history_item(readline.get_current_history_length()-1)
    return input


# The following functions just make the printing of
# parameters look better
def round_tuple(val):
    valstr_list = []
    for v in val:
        vstr = '%s' % (round(v, 5))
        if len(vstr) > 7:
            vstr = '%.5f' % (v,)
        valstr_list.append(vstr)
    valstr = '(' + ','.join(valstr_list) + ')'
    return valstr

def round_float(val):
    vstr = '%s' % (round(val, 5))
    if len(vstr) > 7 and val < 1e3:
        vstr = '%.5f' % (val,)
    elif len(vstr) > 7 and val >= 1e3:
        vstr = '%.2e' % (val,)
    return vstr

def round_list(val):
    valstr_list = []
    for v in val:
        valstr_list.append('%.2e' % (v,))
    valstr = '[' + ','.join(valstr_list) + ']'
    return valstr

def round_list_of_tuples(val):
    valstr_list = []
    valstr_list_tot = []
    for l in val:
        for v in l:
            vstr = '%s' % (round(v, 5))
            if len(vstr) > 7:
                vstr = '%.5f' % (v,)
            valstr_list.append(vstr)
        valstr = '(' + ','.join(valstr_list) + ')'
        valstr_list_tot.append(valstr)
    valstr = '[' + ','.join(valstr_list_tot) + ']'  
    return valstr 


###############################################################################
# Output functions for Image methods.
#
# These are more flexible/have more features than the functions in
# output.py and are designed specifically for interactive use.
###############################################################################
        
def write_bbs_gaul(img, filename=None, srcroot=None, patch=None,
                   incl_primary=True, incl_wavelet=True, sort_by='flux',
                   clobber=False):
    """Writes Gaussian list to a BBS sky model"""
    import numpy as N
    from const import fwsig
    import mylogger
    import os

    mylog = mylogger.logging.getLogger("PyBDSM.write_gaul")
    if int(img.equinox) != 2000 and int(img.equinox) != 1950:
        mylog.warning('Equinox of input image is not J2000 or B1950. '\
                          'Sky model may not be appropriate for BBS.')
    if int(img.equinox) == 1950:
        mylog.warning('Equinox of input image is B1950. Coordinates '\
                          'will be precessed to J2000.')

    outl, outn, patl = list_and_sort_gaussians(img, patch=patch,
                                               root=srcroot, sort_by=sort_by)
    if incl_wavelet and hasattr(img, 'atrous_gaussians'):
        wavoutl, wavoutn, wavpatl = list_and_sort_gaussians(img, patch=patch,
                                                            root=srcroot,
                                                            wavelet=True,
                                                            sort_by=sort_by)
    else:
        wavoutl = []
    if not incl_primary and not incl_wavelet:
        print '\033[31;1mERROR\033[0m: incl_primary and incl_wavelet cannot both be False.'
        return
    if incl_primary:
        if incl_wavelet and hasattr(img, 'atrous_gaussians'):
            outstr_list = make_bbs_str(img, outl+wavoutl, outn+wavoutn, patl+wavpatl)
        else:
            outstr_list = make_bbs_str(img, outl, outn, patl)
    else:
        if len(wavoutl) > 0:
            outstr_list = make_bbs_str(img, wavoutl, wavoutn, wavpatl)
        else:
            return
    if filename == None:    
        filename = img.imagename + '.sky_in'
    if os.path.exists(filename) and clobber == False:
        return None
    mylog.info('Writing ' + filename)
    f = open(filename, 'w')
    for s in outstr_list:
        f.write(s)
    f.close()
    return filename

def write_ds9_gaul(img, filename=None, srcroot=None, deconvolve=False,
                   incl_wavelet=True, clobber=False):
    """Writes Gaussian list to a ds9 region file"""
    import numpy as N
    from const import fwsig
    import mylogger
    import os

    mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Output")
    outl, outn, patl = list_and_sort_gaussians(img, patch=None)
    if incl_wavelet and hasattr(img, 'atrous_gaussians'):
        wavoutl, wavoutn, wavpatl = list_and_sort_gaussians(img, patch=None,
                                                            wavelet=True)
        outl += wavoutl
        outn += wavoutn
    outstr_list = make_ds9_str(img, outl, outn, deconvolve=deconvolve)
    if filename == None:
        filename = img.imagename + '.reg'
    if os.path.exists(filename) and clobber == False:
        return None
    mylog.info('Writing ' + filename)
    f = open(filename, "w")
    for s in outstr_list:
        f.write(s)
    f.close()
    return filename

        
def write_ascii_gaul(img, filename=None, incl_wavelet=True, sort_by='indx',
                     clobber=False):
    """Writes Gaussian list to an ASCII file"""
    import mylogger
    import os

    mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Output")
    outl, outn, patl = list_and_sort_gaussians(img, patch=None, sort_by=sort_by)
    if incl_wavelet and hasattr(img, 'atrous_gaussians'):
        wavoutl, wavoutn, wavpatl = list_and_sort_gaussians(img, patch=None,
                                                            wavelet=True,
                                                            sort_by=sort_by)
        outl += wavoutl
    outstr_list = make_ascii_str(img, outl)
    if filename == None:
        filename = img.imagename + '.gaul'
    if os.path.exists(filename) and clobber == False:
        return None
    mylog.info('Writing ' + filename)
    f = open(filename, "w")
    for s in outstr_list:
        f.write(s)
    f.close()
    return filename

  
def write_casa_gaul(img, filename=None, incl_wavelet=True, clobber=False):
    """Writes a clean box file for use in casapy"""
    import mylogger
    import os
  
    mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Output")
    outl, outn, patl = list_and_sort_gaussians(img, patch=None)
    if incl_wavelet and hasattr(img, 'atrous_gaussians'):
        wavoutl, wavoutn, wavpatl = list_and_sort_gaussians(img, patch=None,
                                                            wavelet=True)
        outl += wavoutl
    outstr_list = make_casa_str(img, outl)
    if filename == None:
        filename = img.imagename + '.box'
    if os.path.exists(filename) and clobber == False:
        return None
    mylog.info('Writing ' + filename)
    f = open(filename, "w")
    for s in outstr_list:
        f.write(s)
    f.close()
    return filename

def write_fits_gaul(img, filename=None, sort_by='indx',
                    incl_wavelet=True, clobber=False):
    """ Write as FITS binary table.

    incl_wavelet not yet implemented.
    """
    import mylogger
    import pyfits
    import os
    import numpy as N

    mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Output")
    outl, outn, patl = list_and_sort_gaussians(img, patch=None, sort_by=sort_by)
    if incl_wavelet and hasattr(img, 'atrous_gaussians'):
        wavoutl, wavoutn, wavpatl = list_and_sort_gaussians(img, patch=None,
                                                            wavelet=True,
                                                            sort_by=sort_by)
        outl += wavoutl
    cnames = N.array(['Gaus_id','Isl_id', 'Souce_id', 'Wave_id', 'Total', 
                  'E_Total','Peak','E_Peak','RA','E_RA', 
                  'DEC','E_DEC','Xposn','E_xposn','Yposn','E_yposn','Bmaj', 
                  'E_Bmaj','Bmin','E_Bmin','Bpa','E_Bpa', 
                  'DC_Bmaj','E_DC_Bmaj','DC_Bmin', 
                  'E_DC_Bmin','DC_Bpa','E_DC_Bpa','I_rms', 
                  'I_mean','I_R_rms','I_R_mean','S_code','Spec_indx',
                  'Err_spec_indx'])
    cformat = N.array(['1J','1J','1J','1J','1D',
                       '1D','1D','1D','1D','1D',
                       '1D','1D','1D','1D','1D','1D','1D',
                       '1D','1D','1D','1D','1D',
                       '1D','1D','1D',
                       '1D','1D','1D','1D',
                       '1D','1D','1D','1A','1D','1D'])
    cunit = N.array([' ',' ',' ',' ','Jy','Jy','Jy/beam','Jy/beam','deg','deg','deg', 
                     'deg','pix','pix','pix','pix','arcsec','arcsec','arcsec', 
                     'arcsec','deg','deg','arcsec','arcsec','arcsec','arcsec', 
                     'deg','deg','Jy/beam','Jy/beam','Jy/beam','Jy/beam',' ',
                     ' ',' '])
    out_list = make_fits_list(img, outl)
    col_list = []
    for ind, col in enumerate(out_list):
      list1 = pyfits.Column(name=cnames[ind], format=cformat[ind],
                            unit=cunit[ind], array=out_list[ind])
      col_list.append(list1)
    tbhdu = pyfits.new_table(col_list)
    if filename == None:
        filename = img.imagename + '.fits'
    if os.path.exists(filename) and clobber == False:
        return None
    mylog.info('Writing ' + filename)
    tbhdu.writeto(filename, clobber=True)
    return filename

def write_kvis_ann(img, filename=None, sort_by='indx', incl_wavelet=True,
                   clobber=False):
    import mylogger
    import os

    mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Output")
    if filename == None:
        filename = img.imagename + '.kvis.ann'
    if os.path.exists(filename) and clobber == False:
        return None
    f = open(filename, 'w')
    mylog.info('Writing '+filename)
    f.write("### KVis annotation file\n\n")
    f.write("color green\n\n")

    outl, outn, patl = list_and_sort_gaussians(img, patch=None, sort_by=sort_by)
    if incl_wavelet and hasattr(img, 'atrous_gaussians'):
        wavoutl, wavoutn, wavpatl = list_and_sort_gaussians(img, patch=None,
                                                            wavelet=True,
                                                            sort_by=sort_by)
        outl += wavoutl

    for g in outl[0]:
        iidx = g.island_id
        ra, dec = g.centre_sky
        shape = g.size_sky

        str = 'text   %10.5f %10.5f   %d\n' % \
            (ra, dec, iidx)
        f.write(str)
        str = 'ellipse %10.5f %10.5f   %10.7f %10.7f %10.4f\n' % \
            (ra, dec, shape[0], shape[1], shape[2])
        f.write(str)
    f.close()
    return filename

def write_star(img, filename=None, sort_by='indx', incl_wavelet=False,
               clobber=False):
    from output import ra2hhmmss, dec2ddmmss
    import mylogger
    import os

    mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Output")
    if filename == None:
        filename = img.imagename + '.star'
    if os.path.exists(filename) and clobber == False:
        return None
    f = open(filename, 'w')
    mylog.info('Writing '+filename)

    outl, outn, patl = list_and_sort_gaussians(img, patch=None, sort_by=sort_by)
    if incl_wavelet and hasattr(img, 'atrous_gaussians'):
        wavoutl, wavoutn, wavpatl = list_and_sort_gaussians(img, patch=None,
                                                            wavelet=True,
                                                            sort_by=sort_by)
        outl += wavoutl

    for g in outl[0]:
        A = g.peak_flux
        ra, dec = g.centre_sky
        shape = g.size_sky
        ### convert to canonical representation
        ra = ra2hhmmss(ra)
        dec= dec2ddmmss(dec)
        decsign = ('-' if dec[3] < 0 else '+')

        str = '%2i %2i %6.3f ' \
              '%c%2i %2i %6.3f ' \
              '%9.4f %9.4f %7.2f ' \
              '%2i %13.7f %10s\n' % \
              (ra[0], ra[1], ra[2], 
               decsign, dec[0], dec[1], dec[2],
               shape[0]*3600, shape[1]*3600, shape[2],
               4, A, '')

        f.write(str)
    f.close()
    return filename

def list_and_sort_gaussians(img, patch=None, root=None, wavelet=False,
                            sort_by='index'):
    """Returns sorted lists of Gaussians and their names and patch names.

    wavelet - if True, use only wavelet Gaussians; if False, use only
              primary Gaussians
    
    Returns (outlist, outnames, patchnames)
    outlist is [[g1, g2, g3], [g4], ...]
    outnames is [['root_i2_s1_g1', 'root_i2_s1_g2', 'root_i2_s1_g3'], ...]
    patchnames is ['root_patch_s1', 'root_patch_s2', ...]
         
    The names are root_iXX_sXX_gXX (or wXX_iXX_sXX_gXX for wavelet Gaussians)
    """
    import numpy as N

    # Define lists
    if root == None:
        root = img.parentname
    gauslist = []
    gausname = []
    outlist = []
    outnames = []
    patchnames = []
    patchnames_sorted = []
    gausflux = [] # fluxes of Gaussians
    gausindx = [] # indices of Gaussians
    patchflux = [] # total flux of each patch
    patchindx = [] # indices of sources
    if wavelet:
        if hasattr(img, 'atrous_gaussians'):
            src_list = []
            for sl in img.atrous_sources:
                src_list += sl
        else:
            return ([], [], [])
    else:
        src_list = img.source
    for src in src_list:
        for g in src.gaussians:
            gauslist.append(g)
            gausflux.append(g.total_flux)
            gausindx.append(g.gaus_num-1)
            if wavelet:
                jstr = '_w' + str(g.wavelet_j)
            else:
                jstr = ''
            gausname.append(root + jstr + '_i' + str(src.island_id) + '_s' +
                            str(src.source_id) + '_g' + str(g.gaus_num-1))
            if patch == 'gaussian':
                outlist.append(gauslist)
                outnames.append(gausname)
                patchnames.append(root + '_patch' + jstr + '_g' + str(g.gaus_num-1))
                patchflux.append(N.sum(gausflux))
                patchindx.append(g.gaus_num-1)
                gauslist = [] # reset for next Gaussian
                gausname = []
                gausflux = []
                gausindx = []
        if patch == 'source':
            sorted_gauslist = list(gauslist)
            sorted_gausname = list(gausname)
            if sort_by == 'flux':
                # Sort Gaussians by flux within each source
                indx = range(len(gausflux))
                indx.sort(lambda x,y: cmp(gausflux[x],gausflux[y]), reverse=True)
            elif sort_by == 'index':
                # Sort Gaussians by index within each source
                indx = range(len(gausindx))
                indx.sort(lambda x,y: cmp(gausindx[x],gausindx[y]), reverse=False)
            else:
                # Unrecognized property --> Don't sort
                indx = range(len(gausindx))
            for i, si in enumerate(indx):
                sorted_gauslist[i] = gauslist[si]
                sorted_gausname[i] = gausname[si]
                
            outlist.append(sorted_gauslist)
            outnames.append(sorted_gausname)
            patchnames.append(root + '_patch' + jstr + '_s' + str(src.source_id))
            patchflux.append(N.sum(gausflux))
            patchindx.append(src.source_id)
            gauslist = [] # reset for next source
            gausname = []
            gausflux = []

    # Sort
    if patch == 'single' or patch == None:
        outlist = [list(gauslist)]
        outlist_sorted = [list(gauslist)]
        outnames = [list(gausname)]
        outnames_sorted = [list(gausname)]
        if patch == 'single':
            patchnames = [root + '_patch']
        else:
            patchnames = [None]
        if sort_by == 'flux':
            # Sort by Gaussian flux
            indx = range(len(gauslist))
            indx.sort(lambda x,y: cmp(gausflux[x],gausflux[y]), reverse=True)
        elif sort_by == 'index':
            # Sort by Gaussian index
            indx = range(len(gausindx))
            indx.sort(lambda x,y: cmp(gausindx[x],gausindx[y]), reverse=False)
        else:
            # Unrecognized property --> Don't sort
            indx = range(len(gausindx))
        for i, si in enumerate(indx):
                outlist_sorted[0][i] = outlist[0][si]
                outnames_sorted[0][i] = outnames[0][si]
                patchnames_sorted = list(patchnames)
    else:
        outlist_sorted = list(outlist)
        outnames_sorted = list(outnames)
        patchnames_sorted = list(patchnames)
        if sort_by == 'flux':
            # Sort by patch flux
            indx = range(len(patchflux))
            indx.sort(lambda x,y: cmp(patchflux[x],patchflux[y]), reverse=True)
        elif sort_by == 'index':
            # Sort by source index
            indx = range(len(patchindx))
            indx.sort(lambda x,y: cmp(patchindx[x],patchindx[y]), reverse=False)
        else:
            # Unrecognized property --> Don't sort
            indx = range(len(gausindx))
           
        for i, si in enumerate(indx):
            outlist_sorted[i] = outlist[si]
            outnames_sorted[i] = outnames[si]
            patchnames_sorted[i] = patchnames[si]

    return (outlist_sorted, outnames_sorted, patchnames_sorted)


def make_bbs_str(img, glist, gnames, patchnames):
    """Makes a list of string entries for a BBS sky model."""
    from output import ra2hhmmss
    from output import dec2ddmmss
    from libs import B1950toJ2000

    outstr_list = []
    if img.opts.spectralindex_do: 
        freq = "%.5e" % img.freq0
    else:
        freq = "%.5e" % img.cfreq
    if patchnames[0] == None:
        outstr_list.append("format = Name, Type, Ra, Dec, I, Q, U, V, "\
                               "MajorAxis, MinorAxis, Orientation, "\
                               "ReferenceFrequency='"+freq+"', "\
                               "SpectralIndex='[]'\n\n")
    else:
        outstr_list.append("format = Name, Type, Patch, Ra, Dec, I, Q, U, V, "\
                               "MajorAxis, MinorAxis, Orientation, "\
                               "ReferenceFrequency='"+freq+"', "\
                               "SpectralIndex='[]'\n\n")
    patchname_last = ''
    for pindx, patch_name in enumerate(patchnames): # loop over patches
      if patch_name != None and patch_name != patchname_last:
          outstr_list.append(', , ' + patch_name + ', 00:00:00, +00.00.00\n')
          patchname_last = patch_name
      gaussians_in_patch = glist[pindx]
      names_in_patch = gnames[pindx]
      for gindx, g in enumerate(gaussians_in_patch):
          src_name = names_in_patch[gindx]
          ra, dec = g.centre_sky
          if img.equinox == 1950:
              ra, dec = B1950toJ2000([ra, dec])
          ra = ra2hhmmss(ra)
          sra = str(ra[0]).zfill(2)+':'+str(ra[1]).zfill(2)+':'+str("%.3f" % (ra[2])).zfill(6)
          dec = dec2ddmmss(dec)
          decsign = ('-' if dec[3] < 0 else '+')
          sdec = decsign+str(dec[0]).zfill(2)+'.'+str(dec[1]).zfill(2)+'.'+str("%.3f" % (dec[2])).zfill(6)
          total = str("%.3e" % (g.total_flux))
          deconv = g.deconv_size_sky
          if deconv[0] == 0.0  and deconv[1] == 0.0:
              stype = 'POINT'
              deconv[2] = 0.0
          else:
              stype = 'GAUSSIAN'
          deconv1 = str("%.5e" % (deconv[0]*3600.0)) 
          deconv2 = str("%.5e" % (deconv[1]*3600.0)) 
          deconv3 = str("%.5e" % (deconv[2])) 
          deconvstr = deconv1 + ', ' + deconv2 + ', ' + deconv3
          specin = '-0.8'
          if hasattr(g, 'spin1'):
              src = get_src(img.source, g.source_id)
              spin1 = src.spin1
              if spin1 != None:
                  specin = str("%.3e" % (spin1[1]))
          sep = ', '
          if img.opts.polarisation_do:
              src = get_src(img.source, g.source_id)
              Q_flux = str("%.3e" % (src.total_flux_Q))
              U_flux = str("%.3e" % (src.total_flux_U))
              V_flux = str("%.3e" % (src.total_flux_V))
          else:
              Q_flux = '0.0'
              U_flux = '0.0'
              V_flux = '0.0'
          if patch_name == None:
              outstr_list.append(src_name + sep + stype + sep + sra + sep +
                                 sdec + sep + total + sep + Q_flux + sep +
                                 U_flux + sep + V_flux + sep +
                                 deconvstr + sep + freq + sep +
                                 '[' + specin + ']\n')
          else:
              outstr_list.append(src_name + sep + stype + sep + patch_name +
                                 sep + sra + sep + sdec + sep + total + sep +
                                 Q_flux + sep + U_flux + sep + V_flux + sep +
                                 deconvstr + sep + freq + sep +
                                 '[' + specin + ']\n') 
    return outstr_list


def make_ds9_str(img, glist, gnames, deconvolve=False):
    """Makes a list of string entries for a ds9 region file."""
    outstr_list = []
    if img.opts.spectralindex_do: 
        freq = "%.5e" % img.freq0
    else:
        freq = "%.5e" % img.cfreq
    if img.equinox == None:
        equinox = 'fk5'
    else:
        if int(img.equinox) == 2000:
            equinox = 'fk5'
        elif int(img.equinox) == 1950:
            equinox = 'fk4'
        else:
            mylog.warning('Equinox of input image is not J2000 or B1950. '\
                                  'Regions may not be correct.')
            equinox = 'fk5'
                
    outstr_list.append('# Region file format: DS9 version 4.0\nglobal color=green '\
                           'font="helvetica 10 normal" select=1 highlite=1 edit=1 '\
                           'move=1 delete=1 include=1 fixed=0 source\n'+equinox+'\n')

    for gindx, g in enumerate(glist[0]):
        src_name = gnames[0][gindx]
        ra, dec = g.centre_sky
        if deconvolve:
            deconv = g.deconv_size_sky
        else:
            deconv = g.size_sky
        if deconv[0] == 0.0 and deconv[1] == 0.0:
            stype = 'POINT'
            deconv[2] = 0.0
            region = 'point(' + str(ra) + ',' + str(dec) + \
                ') # point=cross width=2 text={' + src_name + '}\n'
        else:
            # ds9 can't handle 1-D Gaussians, so make sure they are 2-D
            if deconv[0] < 1.0/3600.0: deconv[0] = 1.0/3600.0
            if deconv[1] < 1.0/3600.0: deconv[1] = 1.0/3600.0
            stype = 'GAUSSIAN'
            region = 'ellipse(' + str(ra) + ',' + str(dec) + ',' + \
                str(deconv[0]*3600.0) + '",' + str(deconv[1]*3600.0) + \
                '",' + str(deconv[2]+90.0) + ') # text={' + src_name + '}\n'
        outstr_list.append(region)
    return outstr_list


def make_ascii_str(img, glist):
    """Makes a list of string entries for an ascii region file."""
    outstr_list = []
    if img.opts.spectralindex_do: 
        freq = "%.5e" % img.freq0
    else:
        freq = "%.5e" % img.cfreq

    outstr_list.append('# PyBDSM Gaussian list for '+img.filename+'\n')
    outstr_list.append('# Reference frequency : %s Hz\n' % freq)
    outstr_list.append('# Equinox : %s \n\n' % img.equinox)
    if img.opts.spectralindex_do:
        outstr_list.append('#  Gaus_id  Isl_id  Source_id  Wave_id  Total  '\
                               'E_Total  Peak  E_Peak  '\
                               'RA  E_RA  DEC  E_DEC  Xposn  E_Xposn  '\
                               'Yposn  E_Yposn  Bmaj  '\
                               'E_Bmaj  Bmin  E_Bmin  Bpa  '\
                               'E_Bpa  DC_Bmaj E_DC_Bmaj  '\
                               'DC_Bmin  E_DC_Bmin  '\
                               'DC_Bpa  E_DC_Bpa  I_rms  I_mean  I_R_rms  '\
                               'I_R_mean  S_code  Spec_indx  Err_spec_indx\n')
    else:           
        outstr_list.append('#   Gaus_id  Isl_id  Source_id  Wave_id  Total  '\
                               'E_Total  Peak  E_Peak  '\
                               'RA  E_RA  DEC  E_DEC  Xposn  E_Xposn  '\
                               'Yposn  E_Yposn  Bmaj  '\
                               'E_Bmaj  Bmin  E_Bmin  Bpa  '\
                               'E_Bpa  DC_Bmaj E_DC_Bmaj  '\
                               'DC_Bmin  E_DC_Bmin  '\
                               'DC_Bpa  E_DC_Bpa  I_rms  I_mean  I_R_rms  '\
                               'I_R_mean  S_code\n')
    for g in glist[0]:
        gidx = g.gaus_num-1  # python numbering
        iidx = g.island_id  # python numbering
        widx = g.wavelet_j
        sidx = g.source_id
        F = g.flag
        A = g.peak_flux
        T = g.total_flux
        ra, dec = g.centre_sky
        x, y = g.centre_pix
        shape = g.size_sky
        shape_deconv = g.deconv_size_sky
        eA = g.peak_fluxE
        eT = g.total_fluxE
        era, edec = g.centre_skyE
        ex, ey = g.centre_pixE
        irms = g.rms
        imean = g.mean
        eshape = g.size_skyE
        eshape_deconv = g.deconv_size_skyE
        src = img.source[sidx]
        scode = src.code
        if img.opts.spectralindex_do:
            spin1 = g.spin1
            espin1 = g.espin1
            if spin1 == None:
                specin = 0.0
                especin = 0.0
            else:                       
                specin = spin1[1]
                especin = espin1[1]
            outstr_list.append("%4d  %4d  %4d   %4d   %10f  %10f  %10f  %10f  %10f  %10f " \
                                   "%10f  %10f  %10f  %10f  %10f  %10f  %10f  %10f  " \
                                   "%10f  %10f  %10f  %10f  %10f  %10f  " \
                                   "%10f  %10f  %10f  %10f  %10f  %10f  " \
                                   "%10f  %10f  %2s   %10f  %10f\n" % 
                               (gidx, iidx, sidx, widx, T, eT, A, eA,
                                ra, era, dec, edec, x, ex,  y, ey, 
                                shape[0], eshape[0], shape[1], eshape[1],  shape[2], eshape[2],
                                shape_deconv[0], eshape_deconv[0], shape_deconv[1],
                                eshape_deconv[1],  shape_deconv[2], eshape_deconv[2], irms,
                                imean, irms, imean, scode, specin, especin))
        else:                
            outstr_list.append("%4d  %4d  %4d   %4d   %10f  %10f  %10f  %10f  %10f  %10f " \
                                   "%10f  %10f  %10f  %10f  %10f  %10f  %10f  %10f  " \
                                   "%10f  %10f  %10f  %10f  %10f  %10f  " \
                                   "%10f  %10f  %10f  %10f  %10f  %10f  " \
                                   "%10f  %10f  %2s\n" % 
                               (gidx, iidx, sidx, widx, T, eT, A, eA,
                                ra, era, dec, edec, x, ex,  y, ey, 
                                shape[0], eshape[0], shape[1], eshape[1],  shape[2], eshape[2],
                                shape_deconv[0], eshape_deconv[0], shape_deconv[1],
                                eshape_deconv[1],  shape_deconv[2], eshape_deconv[2], irms,
                                imean, irms, imean, scode))
    return outstr_list
        
def make_fits_list(img, glist):
    import functions as func

    out_list = []
    for g in glist[0]:
        gidx = g.gaus_num-1
        iidx = g.island_id
        widx = g.wavelet_j
        A = g.peak_flux
        T = g.total_flux
        ra, dec = g.centre_sky
        x, y = g.centre_pix
        shape = g.size_sky
        deconv_shape = g.deconv_size_sky
        eA = g.peak_fluxE
        eT = g.total_fluxE
        era, edec = g.centre_skyE
        ex, ey = g.centre_pixE
        eshape = g.size_skyE
        deconv_eshape = g.deconv_size_skyE
        isl_idx = g.island_id
        isl = img.islands[isl_idx]
        isl_rms = isl.rms
        isl_av = isl.mean
        sidx = g.source_id
        src = img.source[sidx]
        scode = src.code
        src_rms = src.rms_isl
        src_av = isl.mean
        flag = g.flag
        grms = g.rms
        x, y = g.centre_pix
        xsize, ysize, ang = g.size_pix # FWHM
    
        specin = 0.0
        especin = 0.0
        if img.opts.spectralindex_do:
            spin1 = g.spin1
            espin1 = g.espin1
            if spin1 == None:
                specin = 0.0
                especin = 0.0
            else:                       
                specin = spin1[1]
                especin = espin1[1]

        list1 = [gidx, iidx, sidx, widx, T, eT, A, eA, ra, era, dec, edec, x, ex, y,
                 ey, shape[0], eshape[0], shape[1], eshape[1], shape[2],
                 eshape[2], deconv_shape[0], deconv_eshape[0],
                 deconv_shape[1], deconv_eshape[1], deconv_shape[2],
                 deconv_eshape[2], src_rms, src_av, isl_rms, isl_av, scode,
                 specin, especin]
        out_list.append(list1)
    out_list = func.trans_gaul(out_list)
    return out_list

def make_casa_str(img, glist):
    """Makes a list of string entries for a casa clean box file."""
    import functions as func
    outstr_list = []
    sep = ' '
    scale = 2.0
    for gindx, g in enumerate(glist[0]):
        x, y = g.centre_pix
        xsize, ysize, ang = g.size_pix # FWHM
        ellx, elly = func.drawellipse(g)
        blc = [int(min(ellx)), int(min(elly))]
        trc = [int(max(ellx)), int(max(elly))]

        blc[0] -= (x - blc[0]) * scale
        blc[1] -= (y - blc[1]) * scale
        trc[0] += (trc[0] - x) * scale
        trc[1] += (trc[1] - y) * scale
        # Format is: <id> <blcx> <blcy> <trcx> <trcy>
        # Note that we use gindx rather than g.gaus_num so that
        # all Gaussians will have a unique id, even if wavelet
        # Gaussians are included.
        outstr_list.append(str(gindx+1) + sep + str(blc[0]) + sep +
                           str(blc[1]) + sep + str(trc[0]) + sep +
                           str(trc[1]) +'\n')
    return outstr_list

def export_image(self, outfile=None, img_format='fits',
                 img_type='resid_gaus', incl_wavelet=True,
                 clobber=False):
    """Write an image to a file.

    outfile - name of resulting file; if None, file is
    named automatically.
    img_type - type of image to export; see below
    img_format - format of resulting file: 'fits' or 'casa'
    incl_wavelet - include wavelet Gaussians in model
                     and residual images?
    clobber - overwrite existing file?

    The following images may be exported:
        'ch0' - image used for source detection
        'rms' - rms map image
        'mean' - mean map image
        'resid_gaus' - Gaussian model residual image
        'model_gaus' - Gaussian model image
        'resid_shap' - Shapelet model residual image
        'model_shap' - Shapelet model image
    """
    import os
    import functions as func

    if hasattr(self, 'ngaus') == False and 'gaus' in img_type:
        print 'Gaussians have not been fit. Please run process_image first.'
        return False    
    elif self.opts.shapelet_do == False and 'shap' in img_type:
        print 'Shapelets have not been fit. Please run process_image first.'
        return False    
    elif hasattr(self, 'mean') == False:
        print 'Image has not been processed. Please run process_image first.'
        return False    
    format = img_format.lower()
    if (format in ['fits', 'casa']) == False:
        print '\n\033[91mERROR\033[0m: img_format must be "fits" or "casa"'
        return False 
    if format == 'casa':
        print "Sorry, only img_format = 'fits' is supported at the moment"
        return False 
    filename = outfile
    if filename == None or filename == '':
        filename = self.imagename + '_' + img_type + '.' + format
    if os.path.exists(filename) and clobber == False:
        print '\033[91mERROR\033[0m: File exists and clobber = False.'
        return False 
    if format == 'fits':
        use_io = 'fits'
    if format == 'casa':
        use_io = 'rap'
    bdir = ''
    try:
        if img_type == 'ch0':
            func.write_image_to_file(use_io, filename,
                                     self.ch0, self, bdir,
                                     clobber=clobber)
        elif img_type == 'rms':
            func.write_image_to_file(use_io, filename,
                                     self.rms, self, bdir,
                                     clobber=clobber)
        elif img_type == 'mean':
            func.write_image_to_file(use_io, filename,
                                     self.mean, self, bdir,
                                     clobber=clobber)
        elif img_type == 'gaus_resid':
            if hasattr(self, 'ngaus'):
                if incl_wavelet and hasattr(self, 'atrous_gaussians'):
                    im = self.resid_wavelets
                else:
                    im = self.resid_gaus
                func.write_image_to_file(use_io, filename,
                                         im, self, bdir,
                                         clobber=clobber)
            else:
                print 'Gaussians have not been fit. Please run '\
                    'process_image first.'
                return False
        elif img_type == 'gaus_model':
            if hasattr(self, 'ngaus'):
                if incl_wavelet and hasattr(self, 'atrous_gaussians'):
                    im = self.ch0 - self.resid_wavelets
                else:
                    im = self.model_gaus
                func.write_image_to_file(use_io, filename,
                                         im, self, bdir,
                                         clobber=clobber)
            else:
                print 'Gaussians have not been fit. Please run '\
                    'process_image first.'
                return False
        elif img_type == 'shap_resid':
            if hasattr(self, 'resid_shap') and self.opts.shapelet_do:
                func.write_image_to_file(use_io, filename,
                                         self.resid_shap, self, bdir,
                                         clobber=clobber)
            else:
                print 'Image has not been decomposed in shapelets. '\
                    'Please run process_image first.'
                return False
        elif img_type == 'shap_model':
            if hasattr(self, 'model_shap') and self.opts.shapelet_do:
                func.write_image_to_file(use_io, filename,
                                         self.model_shap, self, bdir,
                                         clobber=clobber)
            else:
                print 'Image has not been decomposed in shapelets. '\
                    'Please run process_image first.'
                return False
        else:
            print "\n\033[91mERROR\033[0m: img_type not recognized."
            return False
        print '--> Wrote file ' + repr(filename)
        return True
    except:
        print '\033[91mERROR\033[0m: File ' + filename + ' could not be written.'
        raise

def write_gaul(self, outfile=None, format='bbs', srcroot=None,
               bbs_patches=None, incl_wavelet=True, clobber=False):
    """Write the Gaussian list to a file.

    filename - name of resulting file; if None, file is
               named automatically.
    format - format of output list. Supported formats are:
        "fits"  - FITS binary table
        "ascii" - ASCII text file
        "bbs"   - BBS sky model
        "ds9"   - ds9 region file
        "star"  - AIPS STAR file
        "kvis"  - kvis file
    srcroot - root for source and patch names (BBS/ds9 only);
              if None, the srcroot is chosen automatically
    bbs_patches - type of patches to use:
        None - no patches
        "gaussian" - each Gaussian gets its own patch
        "single"   - all Gaussians are put into a single
                     patch
        "source"   - sources are group by source into patches
    incl_wavelet - Include Gaussians from wavelet decomposition?
    sort_by - Property to sort output list by:
        "flux" - sort by total integrated flux, largest first
        "indx" - sort by Gaussian and island index, smallest first
    clobber - Overwrite existing file?
    """
    # Eventually, it would be good to replace this function with
    # write_catalog(), that can also handle source lists and shapelets
    if hasattr(self, 'ngaus')==False:
        print 'Gaussians have not been fit. Please run process_image first.'
        return False      
    format = format.lower()
    patch = bbs_patches
    filename = outfile
    if isinstance(patch, str):
        patch = patch.lower()
    if (format in ['fits', 'ascii', 'bbs', 'ds9', 'star', 
                   'kvis']) == False:
        print '\033[91mERROR\033[0m: format must be "fits", '\
            '"ascii", "ds9", "star", "kvis",  or "bbs"'
        return False
    if (patch in [None, 'gaussian', 'single', 'source']) == False:
        print '\033[91mERROR\033[0m: patch must be None, '\
            '"gaussian", "source", or "single"'
        return False
    if self.ngaus == 0:
        print 'No Gaussians were fit to image. Output file not written.'
        return False 
    if filename == '': filename = None
    if format == 'fits':
        filename = write_fits_gaul(self, filename=filename,
                                             incl_wavelet=incl_wavelet,
                                             clobber=clobber)
        if filename == None:
            print '\033[91mERROR\033[0m: File exists and clobber = False.'
            return False
        else:
            print '--> Wrote FITS file ' + repr(filename)
            return True
    if format == 'ascii':
        filename = write_ascii_gaul(self, filename=filename,
                                              incl_wavelet=incl_wavelet,
                                              sort_by='index',
                                              clobber=clobber)
        if filename == None:
            print '\033[91mERROR\033[0m: File exists and clobber = False.'
            return False
        else:
            print '--> Wrote ascii file ' + repr(filename)
            return True
    if format == 'bbs':
        filename = write_bbs_gaul(self, filename=filename,
                                            srcroot=srcroot,
                                            patch=patch,
                                            incl_wavelet=incl_wavelet,
                                            sort_by='flux',
                                            clobber=clobber)
        if filename == None:
            print '\033[91mERROR\033[0m: File exists and clobber = False.'
            return False
        else:
            print '--> Wrote BBS sky model ' + repr(filename)
            return True
    if format == 'ds9':
        filename = write_ds9_gaul(self, filename=filename,
                                            srcroot=srcroot,
                                            incl_wavelet=incl_wavelet,
                                            clobber=clobber)
        if filename == None:
            print '\033[91mERROR\033[0m: File exists and clobber = False.'
            return False
        else:
            print '--> Wrote ds9 region file ' + repr(filename)
            return True
    if format == 'star':
        filename = write_star(self, filename=filename,
                                        incl_wavelet=incl_wavelet,
                                        clobber=clobber)
        if filename == None:
            print '\033[91mERROR\033[0m: File exists and clobber = False.'
            return False
        else:
            print '--> Wrote AIPS STAR file ' + repr(filename)
            return True
    if format == 'kvis':
        filename = write_kvis_ann(self, filename=filename,
                                            incl_wavelet=incl_wavelet,
                                            clobber=clobber)
        if filename == None:
            print '\033[91mERROR\033[0m: File exists and clobber=False.'
            return False
        else:
            print '--> Wrote kvis file ' + repr(filename)
            return True
    # if format == 'casabox':
    #     filename = interface.write_casa_gaul(self, filename=filename,
    #                                   incl_wavelet=incl_wavelet,
    #                                   clobber=clobber)
    #     if filename == None:
    #         print '\033[91mERROR\033[0m: File exists and clobber=False.'
    #     else:
    #         print '--> Wrote CASA clean box file ' + filename
        
def get_src(src_list, srcid):
    """Returns the source for srcid or None if not found"""
    for src in src_list:
        if src.source_id == srcid:
            return src
    return None

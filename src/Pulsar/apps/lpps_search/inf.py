'''
PRESTO style metadata (a.k.a. .inf files) reading/writing. Furthermore there are
some functions that act as sanity checks on the values of coordinates and the 
like. Except for the inf_reader and inf_writer functions the functions of this 
module are meant only for internal use in ssps.

The PRESTO metadata (.inf files) are converted on the fly to InfoDataFile 
instances and back using getattr and setattr.
'''
# TODO: move ra_ok and dec_ok out of this module

import os
import logging
import re

from xml.sax.saxutils import escape
from xml.sax.saxutils import quoteattr


class InfoDataFile:
    '''
    PRESTO style metadata class (represent a .inf file)
    
    NOTE: attributes are added to this class on the fly using getattr and 
    setattr.
    '''
    pass

def ra_ok(hour, minute, second):
    '''
    Sanity check Right Ascension values. Returns True if ok, False if not.
    '''
    if not 0 <= hour <= 23:
        return False
    if not 0 <= minute <= 59:
        return False
    if not 0 <= second < 60:
        return False
    return True

def dec_ok(sign, degree, minute, second):
    '''
    Sanity check Declination values. Returns True if ok, False if not.
    '''
    if not sign in (-1, 1):
        return False
    if not 0 <= degree <= 90:
        return False
    if not 0 <= minute <= 59:
        return False
    if not 0 <= second < 60:
        return False
    return True

def int2boolean(i):
    '''
    Convert integer to python Boolean (match PRESTO's behaviour).
    '''
    if i == 0 : return False
    elif i == 1 : return True
    else : raise ValueError

def boolean2int(b):
    '''
    Convert Boolean to integer.
    '''
    if b: return 1
    else: return 0

ASCII2DEC = re.compile(r'(?P<sign>(-|\+)*)(?P<degree>\d{1,2}):(?P<minute>\d{1,2}):(?P<second>((\d{1,2}\.\d+)|(\d{1,2})))')

def ascii2dec(s):
    '''
    Convert declination in ASCII representation to a 4-tuple of numbers.
    '''
    m = ASCII2DEC.match(s.strip())
    try:
        if m.group('sign') == '' or m.group('sign') == '+':
            sign = +1
        elif m.group('sign') == '-':
            sign = -1
        degree = int(m.group('degree'))
        minute = int(m.group('minute'))
        second = float(m.group('second'))
    except (ValueError, IndexError), e:
        raise ValueError
    else:
        if not dec_ok(sign, degree, minute, second):
            raise ValueError
    return sign, degree, minute, second

def dec2ascii(sign, degree, minute, second):
    '''
    Convert declination from a 4-tuple of numbers to ASCII representation.
    '''

    if sign == -1:
        sign_str = '-'
    elif sign == 1:
        sign_str = ''

    return '%(sign)s%(degree)02d:%(minute)02d:%(second)007.4f' % {
        'sign' : sign_str,
        'degree' : degree,
        'minute' : minute,
        'second' : second,
    }


ASCII2RA = re.compile(r'(?P<hour>\d{1,2}):(?P<minute>\d{1,2}):(?P<second>((\d{1,2}\.\d+))|(\d{1,2}))')

def ascii2ra(s):
    '''
    Convert Right Ascension in ASCII representation to a 3-tuple of numbers.
    '''
    m = ASCII2RA.match(s.strip())
    try:
        hour = int(m.group('hour'))
        minute = int(m.group('minute'))
        second = float(m.group('second'))
    except (ValueError, IndexError), e:
        raise ValueError
    else:
        if not ra_ok(hour, minute, second):
            raise ValueError
    return hour, minute, second

def ra2ascii(hour, minute, second):
    '''
    Convert Right Ascension as a 3-tuple of numbers to ASCII representation.
    '''
    return '%(hour)02d:%(minute)02d:%(second)007.4f' % {
        'hour' : hour,
        'minute' : minute,
        'second' : second,
    }

def special_float2str(f):
    '''
    Convert float to string, truncating decimals behind decimal point if zero.
    
    NOTE : function is used to more closely match PRESTO .inf files.
    '''
    # Truncates decimals behind decimal point if they are zero
    tmp = int(f)
    if tmp == f:
        return str(tmp)
    else:
        return str(f)

# To get the same output as PRESTO when writing a .inf file the attributes are
# ordered as specified in SINGLEPULSE_ORDERING
SINGLEPULSE_ORDERING = ['data_file', 'telescope', 'instrument', 'object', 
    'j2000_ra', 'j2000_dec', 'observer', 'epoch_mjd', 'bary', 'n_bins', 
    'bin_width', 'breaks', 'on_off_pairs', 'em_band', 'beam_diameter', 'dm', 
    'low_channel_central_freq', 'total_bandwidth', 'n_channels', 
    'channel_bandwidth', 'analyst'] 


# Specification of the file format of .inf files:
SPEC = [
    # This list contains elements with the following layout :
    # [<key in info file>, 
    #    <internal attribute name>, 
    #    <ascii to internal representation convertor>,
    #    <internal representation to ascii convertor>]
    [" Data file name without suffix          ",
        'data_file',
        lambda x : str(x).strip(),
        str],
    [" Telescope used                         ",
        'telescope',
        lambda x : str(x).strip(),
        str],
    [" Instrument used                        ",
        'instrument',
        lambda x : str(x).strip(),
        str],
    [" Object being observed                  ",
        'object',
        lambda x : str(x).strip(),
        str],
    [" J2000 Right Ascension (hh:mm:ss.ssss)  ",
        'j2000_ra',
        ascii2ra,
        lambda x : ra2ascii(x[0], x[1], x[2])],
    [" J2000 Declination     (dd:mm:ss.ssss)  ",
        'j2000_dec',
        ascii2dec,
        lambda x : dec2ascii(*x)],
    [" Data observed by                       ",
        'observer',
        lambda x : str(x).strip(),
        str],
    [" Epoch of observation (MJD)             ",
        'epoch_mjd',
        float,
        lambda x : '%.15f' % x], 
    [" Barycentered?           (1=yes, 0=no)  ",
        'bary',
        lambda x : int2boolean(int(x)),
        lambda x : str(boolean2int(x))],
    [" Number of bins in the time series      ",
        'n_bins',
        int,
        lambda x : str(x)],
    [" Width of each time series bin (sec)    ",
        'bin_width',
        float,
        str],
    [" Any breaks in the data? (1=yes, 0=no)  ",
        'breaks',
        lambda x : int2boolean(int(x)),
        lambda x : str(boolean2int(x))],
    [" Type of observation (EM band)          ",
        'em_band',
        lambda x : str(x).strip(),
        str],
    [" Beam diameter (arcsec)                 ", 
        'beam_diameter',
        float,
        special_float2str],
    [" Dispersion measure (cm-3 pc)           ",
        'dm',
        float,
        special_float2str],
    [" Central freq of low channel (Mhz)      ",
        'low_channel_central_freq',
        float,
        str],
    [" Total bandwidth (Mhz)                  ",
        'total_bandwidth',
        float,
        special_float2str],
    [" Number of channels                     ",
        'n_channels',
        int,
        str],
    [" Channel bandwidth (Mhz)                ",
        'channel_bandwidth',
        float,
        str],
    [" Data analyzed by                       ",
        'analyst',
        lambda x : str(x).strip(),
        str],
]

# This dictionary maps the keys in the info file to a attribute name of the
# InfoDataFile objects used internally:
mapping = dict([(x[0].upper(), x[1]) for x in SPEC])

# and for convenience its reverse:
reverse_mapping = dict([(x[1], x[0]) for x in SPEC])

# This dictionary maps attribute names to ascii reader functions:
readers = dict([(x[1], x[2]) for x in SPEC])
# This dictionary maps attribute names to ascii writer functions:
writers = dict([(x[1], x[3]) for x in SPEC])


# ------------------------------------------------------------------------------
# -- Special conversion functions for On/Off pairs -----------------------------


ON_OFF_PAIR_REGEXP = r'^ On/Off bin pair #\s*(?P<number>\d+)\s*$'
ON_OFF_PAIR_PATTERN = re.compile(ON_OFF_PAIR_REGEXP)

def ascii2on_off_pair(value):
    '''Convert On/Off pair in .inf representation to a 2 tuple of integers.'''
    tmp = value.split(',')
    tmp = tuple([int(x.strip()) for x in tmp])
    if len(tmp) != 2:
        raise Exception('Not a pair of samples.')
    return tuple(tmp)    

def on_off_pairs2ascii(on_off_pairs_dict):
    l = list(on_off_pairs_dict.keys())
    l.sort()
    tmp = []
    for k in l:
        s = ''' On/Off bin pair #%3d                   =  %-11d, %-11d\n''' % \
            (k, on_off_pairs_dict[k][0], on_off_pairs_dict[k][1])
        tmp.append(s)
    return ''.join(tmp)

# ------------------------------------------------------------------------------

def inf_reader(file, verbose = False):
    '''
    Read a PRESTO style metadata file.
    
    Note about this implementation. The idea is to treat a .inf file as a series
    of lines where each line is a key value pair. The possible keys and values 
    are listed in the SPEC list defined in this module. The keys from the .inf 
    file are used as keys to dictionaries derived from SPEC that contain the 
    conversion functions for the values from the .inf file. This fails for 2 
    cases. The additional notes free form field at the end of a .inf file and 
    the On/Off pair lines that have information even in the key in the .inf 
    file. These two are special cased in the conversion code --- see the 
    code comments for where and how.
    '''
    # TODO Find out what all the entries in inf files are. If there is stuff 
    #   that is optional think about how to deal with that case.
    logging.debug('Reading file %s' % file)
    info = InfoDataFile()
    info.notes = []
    try:
        fd = open(file, "r")
        try:    
            is_additional_notes = False
            for line_no, line in enumerate(fd):
                if is_additional_notes or 'Any additional notes:' in line:
                    # Deal with additional notes free form field (assumed to 
                    # always be at the end of a .inf file).
                    is_additional_notes = True
                    info.notes.append(line)
                else:
                    tmp = line.split('=')
                    
                    # Some keys have = signs in them, deal with that:
                    if len(tmp) > 2:
                        key = '='.join(tmp[:-1])
    #                    key = key.upper()
                    else:
                        key = tmp[0]
    #                    key = key.upper()

                    # Special case for On/Off pairs (which have the problem that 
                    # there is information in the .inf key 8/ )
                    try:
                        getattr(info, 'on_off_pairs')
                    except (AttributeError), e:
                        info.on_off_pairs = {}
                    # Find out whether we are dealing with a On/Off pair by
                    # trying to match the current .inf line to a regular 
                    # expression.
                    m = ON_OFF_PAIR_PATTERN.match(key)
                    if m:
                        # In fact we are dealing with an On/Off pair:
                        try:
                            t = ascii2on_off_pair(tmp[-1])
                            k = int(m.group('number'))
                        except:
                            logging.debug('Failed to convert line %d.' % line_no)
                        else:
                            info.on_off_pairs[k] = t
                        continue
                    # end of special case ...
                    key = key.upper()
                    try:
                        target_attribute = mapping[key]
                    except KeyError:
                        logging.debug('Unknown attribute in line %d.' % line_no)
                    except:
                        raise
                    else:
                        # TODO : raise appropriate errors if there are double 
                        # entries for some attribute in the .inf file.
                        convertor_function = readers[target_attribute]
                        try:
                            value = convertor_function(tmp[-1])
                        except ValueError:
                            logging.debug('Known attribute, wrong value in line %d.' % line_no)
                        except:
                            raise
                        else:
                            setattr(info, target_attribute, value)
        finally:
            fd.close()
    except IOError:
        # Log the fact that the file was not readable for some reason and 
        # propagate the exception so that calling code can decide what to do
        # about it.
        logging.debug('Failed reading file %s' % file)
        raise
    return info

def inf_writer(file, inf_object):
    '''
    Write a PRESTO style metadata file given a InfoDataFile instance.
    '''
    try:
        fd = open(file, 'w')
        logging.debug('Writing file %s' % file)
        try:
            for k in SINGLEPULSE_ORDERING:
                if k == 'on_off_pairs': # special case for on_off_pairs
                    s = on_off_pairs2ascii(inf_object.on_off_pairs)
                    fd.write(s)
                else: # the normal case:
                    fd.write(reverse_mapping[k] + '=  ' + writers[k](getattr(inf_object, k)) + '\n')
            for line in inf_object.notes:
                fd.write(line)
        finally:
            fd.close()
    except IOError:
        logging.debug('Failed reading file %s' % file)

def inf_htmlchunk(inf_object):
    '''
    Write a PRESTO metadata to a string for inclusion in an HTML file.
    
    Note: this is quite ad hoc and will be replaced with proper templating when
    the exact specs are more clear.
    '''
    html_chunks = ['''<div><h3>.inf file contents</h3><table>''']
    for k in SINGLEPULSE_ORDERING:
        if k == 'on_off_pairs': # special case for on_off_pairs
            on_off_pairs_dict = inf_object.on_off_pairs
            l = list(on_off_pairs_dict.keys())
            l.sort()
            tmp = []
            for k in l:
                s = '''<tr><td>On/Off bin pair #%3d</td><td>%-11d, %-11d</td></tr>\n''' % \
                    (k, on_off_pairs_dict[k][0], on_off_pairs_dict[k][1])
                html_chunks.append(s)
        else: # the normal case:
            s = '<td>' + escape(reverse_mapping[k]) + '</td><td>' + escape(writers[k](getattr(inf_object, k))) + '</td>'
            html_chunks.extend(['\n<tr>', s, '</tr>'])
    html_chunks.append('''</table>''')
    html_chunks.extend(['<h3>additional notes from .inf file</h3>\n<p>'])
    for line in inf_object.notes[1:]:
        html_chunks.append(escape(line))
    html_chunks.append('''</p></div>''')
    return ''.join(html_chunks)

def inf_subset_writer(file, inf_object):
    '''    
    Write a PRESTO metadata file with only a subset of the usual attributes.
    '''
    try:
        fd = open(file, 'w')
        logging.debug('Writing file %s' % file)
        try:
            for k in SINGLEPULSE_ORDERING:
                try:
                    fd.write(reverse_mapping[k] + '=  ' + writers[k](getattr(inf_object, k)) + '\n')
                except AttributeError:
                    pass
            for line in inf_object.notes:
                fd.write(line)
        finally:
            fd.close()
    except IOError:
        logging.debug('Failed reading file %s' % file)


if __name__ == "__main__":
    print __doc__

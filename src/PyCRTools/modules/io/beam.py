"""
This module implements the primary interface for reading complex beam data, which is the product of beamform.py

It contains one function `open` that is used to open a binary file containing complex beamformed data and returns a :class:`~BeamData` file object.

.. moduleauthor:: Emilio Enriquez <E.Enriquez@astro.ru.nl>

"""

import os
import numpy as np
import pycrtools as cr
from pycrtools import metadata as md

import pdb
# pdb.set_trace()

# This class implements the IO interface
from interfaces import IOInterface


class BeamData(IOInterface):
    """This class provides an interface to single file beamformed data.
    """

    def __init__(self, filename, chunk=0, block=0, dm=0):
        """Constructor.
        """
        # Useful to do unit conversion
        self.__conversiondict = {"": 1, "kHz": 1000, "MHz": 10 ** 6, "GHz": 10 ** 9, "THz": 10 ** 12}

        self.__filename = filename

        # Open files
        self.__files = [cr.hArrayRead(file, block=0, ext='beam') for file in filename]

        # How many beams are there
        self.__nofBeamDataSets = len(self.__files)

        # Current chunk number
        self.__chunk = chunk

        # Current block number
        self.__block = block

        # Residual delay from CLOCK_OFFSET calibration (between stations) from parsets.
        self.__cloffdelay = self.__clock_offsets()

        #Align the stations to the same block since second started.
        self.__block_alignment = max(self.__block_number()) - self.__block_number()

        # Total number of chunks
        self.__nchunks = self.getNchunks()

        # Current dispersion measure
        self.__dm = dm
        self.__dm_offset = None

        # Current delay calibration (between stations)
        self.__caldelay = cr.hArray(float, len(self.__files), fill=0)  # Need to maybe replace this with a function that reads metadata in the future...?

        # Create keyword dict for easy access
        self.__setKeywordDict()

        # Mark file as opened
        self.closed = False

    def __setKeywordDict(self):

        self.__keyworddict = {
            # NON-ICD KEYWORDS
            "CHUNK": lambda: self.__chunk,
            "BLOCK": lambda: self.__block,
            "DM": lambda: self.__dm,
            "BLOCK_NUMBER": self.__block_number(),
            "NCHUNKS": lambda: self.__nchunks,
            "NOF_BEAM_DATASETS": lambda: self.__nofBeamDataSets
            }

        for par2 in self.__files[0].par.hdr.keys():
            if par2 == 'BeamFormer':
                for par2 in self.__files[0].par.hdr['BeamFormer'].keys():
                    self.__keyworddict['BEAM_' + par2.upper()] = self.__files[0].par.hdr['BeamFormer'][par2]
            elif 'BEAM_' in par2:
                self.__keyworddict[par2.upper()] = self.__files[0].par.hdr[par2]
            else:
                self.__keyworddict['TBB_' + par2.upper()] = self.__files[0].par.hdr[par2]

        self.__keyworddict['FILENAMES'] = self.__filename
        self.__keyworddict['STATION_NAME'] = lambda: [self.__files[i].par.hdr['STATION_NAME'][0] for i in range(self.__nofBeamDataSets)]
        self.__keyworddict['ANTENNA_SET'] = lambda: [self.__files[i].par.hdr['BeamFormer']['antenna_set'] for i in range(self.__nofBeamDataSets)]
        # self.__keyworddict['BEAM_STATIONPOS'] =??
        self.__keyworddict['MAXIMUM_READ_LENGTH'] = self['NCHUNKS'] * self['BEAM_BLOCKLEN'] * self['BEAM_NBLOCKS']
        self.__keyworddict['BLOCKSIZE'] = self['BEAM_BLOCKLEN']
        self.__keyworddict['TIME'] = self.__init_Time()
        self.__keyworddict['FREQUENCY_INTERVAL'] = [self['TBB_FREQUENCY_INTERVAL']]
        self.__keyworddict['SAMPLE_INTERVAL'] = self['TBB_SAMPLE_INTERVAL']
        self.__keyworddict['CAL_DELAY'] = lambda: self.__caldelay
        self.__keyworddict['DM_OFFSET'] = lambda: self.__dm_offset

    setable_keywords = set(["CHUNK", "BLOCK", "DM", "NCHUNKS", "CAL_DELAY"])

    def __setitem__(self, key, value):
        """Set values to keywords if allowed.
        """
        if key not in self.setable_keywords:
            raise KeyError("Invalid keyword '" + str(key) + "' - vailable keywords: " + str(list(self.setable_keywords)))
        elif key is "CHUNK":
            self.__chunk = value
        elif key is "BLOCK":
            self.__block = value
        elif key is "DM":
            self.__dm = value
            self.__dm_offset = self.calcDedispersionIndex(self.__dm)
        elif key is "NCHUNKS":
            self.__nchunks = value
        elif key is "CAL_DELAY":
            self.__caldelay = cr.hArray(float, len(self.__files), fill=value)
            for i, dt in enumerate(self.__caldelay):
                if abs(dt) < 1e-15:  # Removing unfeasible time resolution in delays ()
                    self.__caldelay[i] = 0
        else:
            raise KeyError(str(key) + " cannot be set. Available keywords: " + str(list(self.setable_keywords)))

    def __getitem__(self, *keys):
        """Implements keyword access.
        """
        # If multiple keywords are provided, return a list of results
        if isinstance(keys[0], tuple):
            return [self[k] for k in keys[0]]
        else:
            key = keys[0]

        if key not in self.keys():
            raise KeyError("Invalid keyword: " + key)
        else:
            if hasattr(self.__keyworddict[key], "__call__"):
                return self.__keyworddict[key]()
            else:
                return self.__keyworddict[key]

    def __contains__(self, key):
        """Allows inquiry if key is implemented.
        """

        return key in self.keys()

    def __repr__(self):
        """Display summary when printed.
        """
        return self.info(False, False)

    def info(self, verbose=True, show=True):
        """Display some information about the file. Short and long versions (verbose=False/True)
        """
        # Selecting a subset of keys.
        if verbose:
            key = self.keys()
            key = sorted([k for k in key if 'EMPTY' not in k])
        else:
            key = ['FILENAMES', 'TBB_TIME_HR', 'DATA_LENGTH', 'DATA_LENGTH_TIME', 'STATION_NAME', 'NOF_BEAM_DATASETS', 'BEAM_POINTINGS']

        output = '[Beam Data] Summary of object properties'
        if show:
            print output.strip()

        # For the print out format.
        maxii = 0
        for i in range(len(key)):
            maxii = max([maxii, len(key[i])])
        stringlength = maxii + 5
        und = ''

        # Loop over the selected keys.
        for k in key:
            s = ""

            if k == 'DATA_LENGTH_TIME' and not(verbose):
                s = k + ' ' * (stringlength - len(k)) + ' : ' + str(self['BEAM_BLOCKLEN'] * self['NCHUNKS'] * self['BEAM_NBLOCKS'] * cr.asval(self["TBB_SAMPLE_INTERVAL"])) + ' s'
                if show:
                    print s
                output += '\n' + s
                continue

            if k == 'DATA_LENGTH' and not(verbose):
                s = k + ' ' * (stringlength - len(k)) + ' : ' + str(self['NCHUNKS'] * self['BEAM_NBLOCKS']) + ' Blocks ( ' + str(self['NCHUNKS']) + ' Chunks, each of ' + str(self['BEAM_NBLOCKS']) + ' Blocks, each of ' + str(self['BEAM_BLOCKLEN']) + ' Samples) '
                if show:
                    print s
                output += '\n' + s
                continue

            ss = k + ' ' * (stringlength - len(k)) + ' : '

            try:
                if isinstance(self[k], type([0, 0])) and len(self[k]) > 7:
                    if all(x == self[k][0] for x in self[k]):
                        if verbose:
                            s = ss + '[ ' + str(self[k][0]) + ', ...] x' + str(len(self[k])) + ' with ' + str(type(self[k][0]))
                        else:
                            s = ss + str(self[k][0])
                    else:
                        s = ss + str(self[k][:3] + ['...'] + self[k][-3:])
                else:
                    if isinstance(self[k], (cr.core.hftools._hftools.ComplexArray, cr.core.hftools._hftools.IntArray, cr.core.hftools._hftools.BoolArray, cr.core.hftools._hftools.FloatArray, cr.core.hftools._hftools.StringArray)):
                        s = ss + str(self[k].__repr__(maxlen=10))
                    else:
                        if self[k] == 'UNDEFINED':
                            und += k + ' , '
                            continue
                        else:
                            s = ss + str(self[k])
                if show:
                    print s
                output += '\n' + s

            except IOError:
                pass
        if len(und) > 0:
            s = 'These keywords are UNDEFINED : [' + str(und) + ']'
            if show:
                print s
            output += s

        if not show:
            return output.strip()

    def keys(self, excludedata=False):
        """Returns list of valid keywords.
        """
        return [k for k in self.__keyworddict.keys() if not k[-5:] == "_DATA"] if excludedata else self.__keyworddict.keys()

    def items(self, excludedata=False):
        """Return list of keyword/content tuples of all header variables
        """

        lst = []

        for k in self.keys(excludedata):
            try:
                lst.append((k, self.__keyworddict[k]() if hasattr(self.__keyworddict[k], "__call__") else self.__keyworddict[k]))
            except IOError:
                pass

        return lst

    def getHeader(self):
        """Return a dict with keyword/content pairs for all header variables."""
        return dict(self.items(excludedata=True))

    def getFrequencies(self):
        """Returns the frequencies that are applicable to the FFT data

        Output:
           This method returns a FloatVector with the selected frequencies in Hz.
        """

        return cr.hArray(self['BEAM_FREQUENCIES'])

    def getTimes(self):
        """Returns the time vector.

        Output:
           This method returns a FloatVector with the selected time in sec.
        """

        #Create a time vector
        times = cr.hArray(float,self['NCHUNKS']*self['BEAM_NBLOCKS'],name="Time",units=("","s"))
        times.fillrange(0,self['BLOCKSIZE']*self['SAMPLE_INTERVAL'][0])

        return times

    def __init_Time(self):
        ''' Finds the start time of the TBB observation in seconds for each station.
        '''

        time = [self.__files[i].par.hdr['TIME'][0] for i in range(self.__nofBeamDataSets)]

        return time

    def __block_number(self):
        '''Finds the block start number.
            Similar function to SAMPLE_NUMBER in tbb.
            WARNING: Assuming sample_numbers are from the same second.
        '''

        block_start = []
        if min(cr.hArray(self.__init_Time())) != max(cr.hArray(self.__init_Time())):
            raise NotImplementedError('Stations started observing at a different second. Fix not implemented yet.')

        for nbeam in range(self.__nofBeamDataSets):
            offset = max(self.__files[nbeam].par.hdr['SAMPLE_NUMBER']) + self.__cloffdelay[0][nbeam]
            block_start.append(int(np.ceil(offset/float(self.__files[0].par.hdr['BeamFormer']['blocklen']))))

        return cr.hArray(block_start)

    def __clock_offsets(self):
        '''Gathers the clock offsets of the stations from the parsets, and calculates in number of samples and the residual.
        '''

        BF_PARSETS=os.environ["BF_PARSETS"].rstrip('/')+'/'
        Obs_ID = self.__filename[0].split('/')[-1].split('_')[0]
        fullparsetname=BF_PARSETS+Obs_ID+'.parset'

        offsets = cr.hArray(float,self.__nofBeamDataSets)
        for i in range(self.__nofBeamDataSets):
            f_clock_offset = float(md.getClockCorrectionParset(fullparsetname,self.__files[i].par.hdr['STATION_NAME'][0], antennaset=self.__files[i].par.hdr['BeamFormer']['antenna_set']))
            offsets[i] = f_clock_offset/self.__files[i].par.hdr['SAMPLE_INTERVAL'][0]

        # Residual offsets to reference frequency
        residual = cr.hArray(float, len(offsets), fill=offsets)
        cr.hFmod(residual, 1)

        # Integer offsets to reference frequency
        offset = offsets - residual
        residual *= self.__files[0].par.hdr['SAMPLE_INTERVAL'][0]

        return [offset, residual]

    def getNchunks(self):
        """Find the maximum number of blocks beetwen the beams. Analogous to MAXIMUM_READ_LENGTH in tbb.py.
        WARNING: Note that NCHUNKS is also used as the modulus (for wrapping) when using DM>0.
        """

        all_nblocks = []

        for nbeam in range(self.__nofBeamDataSets):
            all_nblocks.append((self.__files[nbeam].par.nblocks*self.__files[0].par.hdr['BeamFormer']['nblocks'] - self.__block_alignment[nbeam])/self.__files[0].par.hdr['BeamFormer']['nblocks'])

        nblocks = min(all_nblocks)

        return nblocks

    def getITRFAntennaPositions(self):
        """Returns ITRF Station positions. The positions are of the center of the station (which depend on the ANTENNA_SET used).

        Output:
        a one dimensional array containing the Cartesian position of
        each antenna in meters in IRTF coordinates from a predefined
        center.
        So that if `a` is the returned array `a[i]` is an array of
        length 3 with positions (x,y,z) of antenna i.
        """

        pos = cr.hArray(float, [self['NOF_BEAM_DATASETS'], 3])

        for i, file in enumerate(self.__files):
            pos[i] = file.par.hdr['BeamFormer']['stationpos']

        return pos

    def getRelativeAntennaPositions(self):
        """Returns relative Station positions. The positions are of the center of the station (which depend on the ANTENNA_SET used).

        Output:
        a two dimensional array containing the Cartesian position of
        each antenna in meters in local coordinates from a predefined
        center.
        So that if `a` is the returned array `a[i]` is an array of
        length 3 with positions (x,y,z) of antenna i.
        """

        pos = cr.hArray(float, [self['NOF_BEAM_DATASETS'], 3])

        for i, file in enumerate(self.__files):
#            pos[i] = file.par.hdr['BeamFormer']['stationpos']
            pos[i] = md.getStationPositions(self['STATION_NAME'][i], self['ANTENNA_SET'][i], return_as_hArray=True,coordinatesystem='ITRF')

        pos = md.convertITRFToLocal(pos,reflonlat=None)

        return pos

    def getFFTData(self, data, block=-1):
        """Writes FFT data for selected stations to data array.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *data*        data array to write FFT data to.
        *block*       index of bock to return data from.
        ============= =================================================

        Output:
        a two dimensional array containing the FFT data of the
        specified block for each of the selected stations and
        for the selected frequencies (all freqs and stations for now).

        So that if `a` is the returned array `a[i]` is an array of
        length (number of frequencies) of antenna i.

        """

        block = cr.asval(block)

        if block < 0:
            block = self.__block
        else:
            self.__block = block

        spec_len = data.shape()[1]

        if not self['DM']:
            for i, file in enumerate(self.__filename):
                data[i].readfilebinary(os.path.join(file, "data.bin"), (block + self.__block_alignment[i])*spec_len)
        else:

            if len(self['DM_OFFSET'][0]) != spec_len:
                raise ValueError('Variable offset need correct lenght.')

            frequency_range = range(spec_len)
            modulus = self['NCHUNKS'] * self['BEAM_NBLOCKS']

            real_offset = cr.hArray(int, spec_len, self['DM_OFFSET'][0])

            cr.hAdd(real_offset, block)
            cr.hModulus(real_offset, modulus)
            cr.hAdd(real_offset, modulus)    # To remove negative indices.
            cr.hModulus(real_offset, modulus)
            cr.hMul(real_offset, spec_len)
            cr.hAdd(real_offset, cr.hArray(int, spec_len, frequency_range))

            # Note, this following loop could be slow (hOffsetReadFileBinary could maybe be optimized)
            for i, file in enumerate(self.__filename):
                cr.hOffsetReadFileBinary(data[i], os.path.join(file, "data.bin"), real_offset + self.__block_alignment[i]*spec_len)

            #RFI excision
            if self['BEAM_RFI_CHANNELS']:
                for i in range(len(self.__filename)):
                    data[i,self['BEAM_RFI_CHANNELS']] /= 10000

            pass
            # Addding phase correction to DM offsets. (coherent dedispersion).
            weights_dm = self.empty('FFT_DATA')
            phases_dm = cr.hArray(float, weights_dm.shape()[1], fill=0)
            cr.hDelayToPhase(phases_dm, self['BEAM_FREQUENCIES'], self['DM_OFFSET'][1])  # Using this form of delay2phase since have delays as func. of freq.
            weights_dm[0].phasetocomplex(phases_dm)
            weights_dm[1:] = weights_dm[0]
            #data[...].mul(weights_dm[...])

        # Adding calibration delay between stations, subsample residual from parset-CLOCK_OFFSET.
        weights_cloff = self.empty('FFT_DATA')
        phases_cloff = cr.hArray(float, weights_cloff, fill=0)
        phases_cloff.delaytophase(self['BEAM_FREQUENCIES'],self.__cloffdelay[1])
        weights_cloff.phasetocomplex(phases_cloff)
        data[...].mul(weights_cloff[...])

        # Adding extra calibration delay between stations.
        if np.any(self['CAL_DELAY']):
            weights = self.empty('FFT_DATA')
            phases = cr.hArray(float, weights, fill=0)
            phases.delaytophase(self['BEAM_FREQUENCIES'], self['CAL_DELAY'])
            weights.phasetocomplex(phases)
            data[...].mul(weights[...])

    def getTimeseriesData(self, data=None, chunk=-1):
        """Returns timeseries data for selected antennas.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *data*        data array to write timeseries data to.
        *chunk*       index of chunk to return data from.
        ============= =================================================

        Output:
        a two dimensional array containing the timeseries data of the
        specified chunk for each of the selected antennae.
        So that if `a` is the returned array `a[i]` is an array of
        length blocksize of antenna i.

        """

        raise NotImplementedError

        # NOTE: Need to add a dependency on the nyquist zone used:if nyquist=2 then need to use an array with twice the dimensions, notes will be available in Beam_tools.ccBeams
        chunk = cr.asval(chunk)

        if chunk < 0:
            chunk = self.__chunk
        else:
            self.__chunk = chunk

        data = self.getFFTData(chunk=chunk)

        time_series_list = []

        for i in range(self['NOF_BEAM_DATASETS']):
            time_series = cr.hArray(float, [self['BEAM_NBLOCKS'], self['BEAM_BLOCKLEN']])
            cr.hInvFFTw(time_series[...], data[i][...])
            time_series_list.append(time_series)

        return time_series_list

    def empty(self, key):
        """Return empty array for keyword data.
        Known keywords are: "TIMESERIES_DATA", "TIME_DATA", "FREQUENCY_DATA", "FFT_DATA".
        """

        if key == "TIMESERIES_DATA":
            return cr.hArray(float, dimensions=(self['NOF_BEAM_DATASETS'], self['BEAM_BLOCKLEN']), name="E-Field(t)", units=("", "Counts"))
        elif key == "TIME_DATA":
            return cr.hArray(float, self["BLOCKSIZE"], name="Time", units=("", "s"))
        elif key == "FREQUENCY_DATA":
            return cr.hArray(float, self['BEAM_SPECLEN'], name="Frequency", units=("", "Hz"))
        elif key == "FFT_DATA":
            return cr.hArray(complex, dimensions=(self['NOF_BEAM_DATASETS'], self['BEAM_SPECLEN']), name="fft(E-Field)", xvalues=self.getFrequencies(), logplot="y")
        else:
            raise KeyError("Unknown key: " + str(key))

    def calcDedispersionIndex(self, DM, Ref_Freq=None):
        ''' It calculates the integer indices (number of blocks) for a given DM.
            It also returns the time offset from the residual values (to be added as phases).
        '''

        DM = cr.asval(DM)
        dt = self['BEAM_BLOCKLEN'] * cr.asval(self["TBB_SAMPLE_INTERVAL"])
        frequencies = self.getFrequencies()
        if not Ref_Freq:
            Ref_Freq = frequencies[0]

#        hDedispersionShifts(shifts, frequencies, reference, dm, dt)

        # Calculate the relative shifts in samples per frequency channels
        # Constant value comes from "Handbook of Pulsar Astronomy - by Duncan Ross Lorimer , Section 4.1.1, pagina 86 (in google books)"
        shifts = (4.148808e-3 * DM / dt) * 1e9 ** 2 * (Ref_Freq ** -2 - frequencies ** -2)

        # Residual offsets to reference frequency
        residuals = cr.hArray(float, frequencies, fill=shifts)
        cr.hFmod(residuals, 1)

        # Integer offsets to reference frequency
        offsets = shifts - residuals
        offsets *= -1

        residuals *= -1 * dt    # Since use exclusively by getFFTData.

        return [offsets, residuals]

    def close(self):
        """Closes file and sets the data attribute `.closed` to
        True. A closed object can no longer be used for I/O operations.
        `close()` may be called multiple times without error.
        """

        # Call file object destructor which should close the file
        del self.__files

        self.closed = True


def open(filename, *args, **kwargs):
    """Open file(s) with beamformed data.
    """

    if not isinstance(filename, list):
        filename = [filename]

    for file in filename:
        if not os.path.isdir(file):
            raise IOError("No such directory: " + file)

    return BeamData(filename, *args, **kwargs)

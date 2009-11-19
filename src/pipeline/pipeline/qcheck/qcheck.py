from pyrap import tables as ptables
import numpy
import logging
import sys
import os.path
import pylab
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure


DEFAULTS = {}
DEFAULTS['fieldnames'] = ['logtable', 'coords', 'units']
DEFAULTS['colnames'] = ['map']
IMAGEDIMS = {'y': 3, 'x': 4, 'polarization': 2, 'channel': 1}

"""

TODO:

  - check for NaNs

"""



def check_basics(image, loggers):
    logger = loggers["main"]
    assert image.ndim == 5, "image does not have 5 dimensions"


def clip_image(image, niter=0, clip=(-3, 3)):
    if niter > 0:
        mean = image.mean()
        sigma = numpy.sqrt(image.var())
        return clip_image(image[(image > mean+clip[0]*sigma)&(image < mean+clip[1]*sigma)],
                   niter=niter-1, clip=clip)
    return image


def check_stats(image, filename, loggers, plot=False):
    BINCOUNTS = 1000
    MINNBINS = 20
    CLIPCOUNTS = 0.001
    NSIGMA = 0.1
    NMAXSIGMA = 10.0
    POLAXIS = ['I', 'Q', 'U', 'V']
    if plot:
        figure = Figure()
        canvas = FigureCanvas(figure)
    npols = image.shape[IMAGEDIMS['polarization']]
    nchannels = image.shape[IMAGEDIMS['channel']]
    nsubplotrows = nsubplotcols = int(numpy.sqrt(npols))
    if nsubplotcols * nsubplotrows < npols:
        nsubplotrows += 1
    for npol in range(npols):
        if plot:
            axes = figure.add_subplot(nsubplotrows, nsubplotcols, npol+1)
        for nchan in range(nchannels):
            twodimage = image[0, nchan, npol, :, :]
            flatimage = twodimage.flatten()
            mean = flatimage.mean()
            variance = flatimage.var()
            stddev = numpy.sqrt(variance)
            median = numpy.median(flatimage)
            imgmin, imgmax = min(flatimage), max(flatimage)
            loggers["main"].info("%d.%d.minimum = %.4e" % (npol+1, nchan+1,
                                                        imgmin))
            loggers["main"].info("%d.%d.maximum = %.4e" % (npol+1, nchan+1, 
                                                        imgmax))
            loggers["main"].info("%d.%d.mean = %.4e" % (npol+1, nchan+1, mean))
            loggers["main"].info("%d.%d.median = %.4e" % (
                    npol+1, nchan+1, median))
            loggers["main"].info("%d.%d.Standard deviation = %.4e" % (
                    npol+1, nchan+1,stddev))
            # Keep only the bins with a minimum number of counts,
            # so we can 'fit' a Gaussian distribution to calculate the mode
            nbins = (int(flatimage.size/BINCOUNTS)
                     if flatimage.size > 1e5 else MINNBINS)
            counts, bins = numpy.histogram(flatimage, nbins)
            clipped = {}
            clipped['indices'] = counts > max(counts)*CLIPCOUNTS
            clipped['counts'] = counts[clipped['indices']]
            clipped['bins'] = bins[clipped['indices']]
            if plot:
                axes.plot(bins[numpy.invert(clipped['indices'])],
                          counts[numpy.invert(clipped['indices'])], 'ob')
            clippedimage = flatimage[(flatimage >= min(clipped['bins'])) &
                                     (flatimage <= max(clipped['bins']))]
            nbins = (int(clippedimage.size/BINCOUNTS)
                     if clippedimage.size > 1e5 else MINNBINS)
            counts, bins = numpy.histogram(clippedimage, nbins)
            bins = (bins[0:-1] + bins[1:])/2.  # new behaviour in numpy 1.2
            mode = sum(bins * counts)/sum(counts)
            width = (numpy.sqrt(abs(sum( (bins - mode)**2 * counts) /
                                    sum(counts))))
            loggers["main"].info("%d.%d.mode = %.4e" % (npol+1, nchan+1, mode))
            # Also calculate the statistics for a clipped image, ie
            # only the background (no sources)
            clippedimage = clip_image(flatimage, niter=3, clip=(-1, 1))
            mean = clippedimage.mean()
            variance = clippedimage.var()
            stddev = numpy.sqrt(variance)
            median = numpy.median(clippedimage)
            imgmin, imgmax = min(clippedimage), max(clippedimage)
            loggers["main"].info("%d.%d.background-minimum = %.4e" % (
                    npol+1, nchan+1, imgmin))
            loggers["main"].info("%d.%d.background-maximum = %.4e" % (
                    npol+1, nchan+1, imgmax))
            loggers["main"].info("%d.%d.background-mean = %.4e" % (
                    npol+1, nchan+1, mean))
            loggers["main"].info("%d.%d.background-median = %.4e" % (
                    npol+1, nchan+1, median))
            loggers["main"].info("%d.%d.background-stddev = %.4e" % (
                    npol+1, nchan+1, stddev))
            # Verify that mode, background mean & background median are within
            # a few background sigma from each other:
            if abs(mean-median) > NSIGMA*stddev:
                loggers["warn"].warn(
                        " Background mean and background median are more "
                        "than %.1f standard deviations different" % NSIGMA)
            if abs(mean-mode) > NSIGMA*stddev:
                loggers["warn"].warn(
                    " Mode and background mean are more than %.1f "
                    "standard deviations different" % NSIGMA)
            if abs(mode-median) > NSIGMA*stddev:
                loggers["warn"].warn(
                    " Mode and background median are more than %.1f "
                    "standard deviations different" % NSIGMA)
            if imgmax < 0:
                loggers["warn"].warn(" Background maximum is negative")
            if imgmin > 0:
                loggers["warn"].warn(" Background minimum is positive")
            if imgmax > NMAXSIGMA*stddev:
                loggers["warn"].warn(
                    " Background maximum is more than %.1f the "
                    "standard deviation")
            if imgmin < -NMAXSIGMA*stddev:
                loggers["warn"].warn(
                    " Background minimum is less than %.1f the "
                    "standard deviation")
    
            if plot:
                axes.plot(bins, counts, 'ob')
                axes.plot(bins, max(counts) * numpy.exp(-(bins-mode)**2 /
                                                         (2 * width**2)), '-g')
    if plot:
        canvas.print_figure(plot)

    
def setup_logging(logfile):
    loggers = {'main': logging.getLogger('main'),
               'warn': logging.getLogger('warn')}
    handlers = {'main': logging.FileHandler(logfile, mode="w"),
                'warn': logging.StreamHandler()}
    formatters = {'main': logging.Formatter("%(message)s"),
                  'warn': logging.Formatter("%(levelname)s: %(message)s")}
    handlers['main'].setFormatter(formatters['main'])
    handlers['warn'].setFormatter(formatters['warn'])
    loggers['main'].addHandler(handlers['main'])
    loggers['warn'].addHandler(handlers['warn'])
    loggers['main'].setLevel(logging.INFO)
    loggers['warn'].setLevel(logging.WARNING) # warnings only
    return loggers

    
def run(filename, logfile=None, plot=False, outputdir=False, loggers=False):
    if not logfile:
        logfile = filename + "_stats.log"
    if not isinstance(plot, basestring):
        plot = filename + "_histo.pdf"
    if outputdir:
        plot = os.path.join(outputdir, os.path.basename(plot))
    if not loggers:
        loggers = setup_logging(logfile)
    try:
        table = ptables.table(filename, ack=False)
    except RuntimeError:  # pyrap is just a wrapper around C++, so no proper exceptions are thrown
        loggers['main'].error("Error: image %s not properly opened" % filename)
        return
    names = {}
    for part in ('col', 'field'):
        partname = part + 'names'
        names[part] = table.__getattribute__(partname)()
        for defaultname in DEFAULTS[partname]:
            if defaultname not in names[part]:
                # use 'warn' logger instead? 
                # But script can't continue with this fault,
                # so should quit
                raise KeyError("%s not in %snames" % (defaultname, part))
    imgcol = table.col('map')
    image = imgcol.getcol()
    check_basics(image, loggers)
    check_stats(image, filename, loggers, plot=plot)


if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args) != 1:
        sys.exit(1)
    run(args[0], plot=True)

#                                                       LOFAR PIPELINE FRAMEWORK
#
#                              Group data into appropriate chunks for processing
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from collections import defaultdict
import subprocess

from lofar.parameterset import parameterset

import lofarpipe.support.utilities as utilities
from lofarpipe.support.clusterdesc import get_compute_nodes
from lofarpipe.support.parset import Parset

def group_files(logger, clusterdesc, node_directory, group_size, filenames):
        """
        Group a list of files into blocks suitable for simultaneous
        processing, such that a limited number of processes run on any given
        host at a time.

        All node_directory on all compute nodes specified in clusterdesc is
        searched for any of the files listed in filenames. A generator is
        produced; on each call, no more than group_size files per node
        are returned.
        """
        # Given a limited number of processes per node, the first task is to
        # partition up the data for processing.
        logger.debug('Listing data on nodes')
        data = {}
        for node in get_compute_nodes(clusterdesc):
            logger.debug("Node: %s" % (node))
            exec_string = ["ssh", node, "--", "find",
                node_directory,
                "-maxdepth 1",
                "-print0"
                ]
            logger.debug("Executing: %s" % (" ".join(exec_string)))
            my_process = subprocess.Popen(exec_string, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            sout, serr = my_process.communicate()
            data[node] = sout.split('\x00')
            data[node] = utilities.group_iterable(
                [element for element in data[node] if element in filenames],
                group_size,
            )

        # Now produce an iterator which steps through the various chunks of
        # data to image, and image each chunk
        data_iterator = utilities.izip_longest(*list(data.values()))
        for data_chunk in data_iterator:
            to_process = []
            for node_data in data_chunk:
                if node_data: to_process.extend(node_data)
            yield to_process

def gvds_iterator(gvds_file, nproc=4):
    """
    Reads a GVDS file.

    Provides a generator, which successively returns the contents of the GVDS
    file in the form (host, filename), in chunks suitable for processing
    across the cluster. Ie, no more than nproc files per host at a time.
    """
    parset = Parset(gvds_file)

    data = defaultdict(list)
    for part in range(parset.getInt('NParts')):
        host = parset.getString("Part%d.FileSys" % part).split(":")[0]
        file = parset.getString("Part%d.FileName" % part)
        vds  = parset.getString("Part%d.Name" % part)
        data[host].append((file, vds))

    for host, values in data.iteritems():
        data[host] = utilities.group_iterable(values, nproc)

    while True:
        yieldable = []
        for host, values in data.iteritems():
            try:
                for filename, vds in values.next():
                    yieldable.append((host, filename, vds))
            except StopIteration:
                pass
        if len(yieldable) == 0:
            raise StopIteration
        else:
            yield yieldable

def load_data_map(filename):
    """
    Load a mapping of filename <-> compute node from a parset on disk.
    """
    datamap = Parset(filename)
    data = []
    for host in datamap:
        for filename in datamap.getStringVector(host):
            data.append((host, filename))
    return data

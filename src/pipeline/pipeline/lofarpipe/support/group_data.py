import subprocess
import lofarpipe.support.utilities as utilities
from lofarpipe.support.clusterdesc import get_compute_nodes

def group_files(logger, clusterdesc, node_directory, group_size, filenames):
        # Given a limited number of processes per node, the first task is to
        # partition up the data for processing.
        logger.debug('Listing data on nodes')
        data = {}
        for node in get_compute_nodes(clusterdesc):
            logger.debug("Node: %s" % (node))
#            try:
            exec_string = ["ssh", node, "--", "find",
                node_directory,
                "-maxdepth 1",
                "-print0"
                ]
            logger.debug("Executing: %s" % (" ".join(exec_string)))
            my_process = subprocess.Popen(exec_string, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            sout, serr = my_process.communicate()
            data[node] = sout.split('\x00')
#            except:
#                pass
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

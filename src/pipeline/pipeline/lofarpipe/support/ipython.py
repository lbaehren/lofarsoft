#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                   Extensions to IPython system
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------
from ConfigParser import NoSectionError
from IPython.kernel.task import StringTask
from IPython.kernel import client as IPclient
from lofarpipe.support.lofarexceptions import ClusterError

class LOFARTask(StringTask):
    """
    Extended version of IPython's StringTask, allowing external
    arguments for dependency resolution.
    """
    def __init__(self, expression, pull=None, push=None,
        clear_before=False, clear_after=False, retries=0,
        recovery_task=None, depend=None, dependargs=None
    ):
        self.dependargs = dependargs
        return super(LOFARTask, self).__init__(
            expression, pull, push, clear_before, clear_after,
            retries, recovery_task, depend
        )

    def check_depend(self, properties):
        """
        Calls self.depend(properties, self.dependargs)
        to see if a task should be run.
        """
        if self.depend is not None:
            return self.depend(properties, self.dependargs)
        else:
            return True

class IPythonRecipeMixIn(object):
    """
    Mix-in for recipes to provide access to an IPython cluster.
    """
    def _get_cluster(self):
        """
        Return task and multiengine clients connected to the running
        pipeline's IPython cluster.
        """
        self.logger.info("Connecting to IPython cluster")
        try:
            tc  = IPclient.TaskClient(self.config.get('cluster', 'task_furl'))
            mec = IPclient.MultiEngineClient(self.config.get('cluster', 'multiengine_furl'))
        except NoSectionError:
            self.logger.error("Cluster not definied in configuration")
            raise ClusterError
        except:
            self.logger.error("Unable to initialise cluster")
            raise ClusterError
        return tc, mec

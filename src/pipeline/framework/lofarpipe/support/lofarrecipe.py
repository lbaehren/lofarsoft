#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                           IPython and RemoteCommand all in one
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
from lofarpipe.support.ipython import IPythonRecipeMixIn
from lofarpipe.support.stateful import BaseRecipe

class LOFARrecipe(BaseRecipe, IPythonRecipeMixIn, RemoteCommandRecipeMixIn):
    pass

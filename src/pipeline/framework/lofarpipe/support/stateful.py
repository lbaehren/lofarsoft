#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                          Stateful LOFAR Recipe
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from functools import wraps

import os.path
import cPickle

from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.lofarexceptions import PipelineException

def stateful(run_task):
    @wraps(run_task)
    def wrapper(self, configblock, datafiles=[], **kwargs):
        try:
            my_state = self.completed.pop()
        except (AttributeError, IndexError):
            my_state = ('','')

        if configblock == my_state[0]:
            # We have already run this task and stored its state, or...
            self.logger.info("Task already exists in saved state; skipping")
            return my_state[1]
        elif my_state[0] != '':
            # There is a stored task, but it doesn't match this one, or...
            self.logger.error("Stored state does not match pipeline definition; bailing out")
            raise PipelineException("Stored state does not match pipeline definition")
        else:
            # We need to run this task now.
            outputs = run_task(self, configblock, datafiles, **kwargs)
            self.state.append((configblock, outputs))
            self._save_state()
            return outputs
    return wrapper

class StatefulRecipe(BaseRecipe):
    """
    Enables recipes to save and restore state.

    This is used exactly as :class:`~lofarpipe.support.baserecipe.BaseRecipe`,
    but will write a ``statefile`` in the job directory, recording the current
    state of the pipeline after each recipe completes. If the pipeline is
    interrupted, it can automatically resume where it left off.

    To reset the pipeline and start from the beginning again, just remove the
    ``statefile``.
    """
    inputs = {} # No non-default inputs
    def __init__(self):
        super(StatefulRecipe, self).__init__()
        self.state = []
        self.completed = []

    def _save_state(self):
        """
        Dump pipeline state to file.
        """
        statefile = open(
            os.path.join(
                self.config.get('layout', 'job_directory'),
                'statefile'
            ),
        'w')
        state = [self.inputs, self.state]
        cPickle.dump(state, statefile)

    def go(self):
        super(StatefulRecipe, self).go()
        statefile = os.path.join(
            self.config.get('layout', 'job_directory'),
            'statefile'
        )
        try:
            statefile = open(statefile, 'r')
            inputs, self.state = cPickle.load(statefile)
            statefile.close()

            # What's the correct thing to do if inputs differ from the saved
            # state? start_time will always change.
            for key, value in inputs.iteritems():
                if key != "start_time" and self.inputs[key] != value:
                    raise PipelineException(
                        "Input %s (%s) differs from saved state (%s)" %
                        (key, str(self.inputs[key]), inputs[key])
                    )

            self.completed = list(reversed(self.state))
        except (IOError, EOFError):
            # Couldn't load state
            self.completed = []

    run_task = stateful(BaseRecipe.run_task)

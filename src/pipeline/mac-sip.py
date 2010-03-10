"""
This is a model imaging pipeline definition.

Although it should be runnable as it stands, the user is encouraged to copy it
to a job directory and customise it as appropriate for the particular task at
hand.
"""
from __future__ import with_statement
import sys, os
from pipeline.master.control import control
from pipeline.support.utilities import log_time
import threading
import collections

from ep.control import ControllerPort_Interface
from ep.control import ControlStartedEvent
from ep.control import ControlConnectedEvent
from ep.control import ControlResyncedEvent
from ep.control import ControlClaimedEvent
from ep.control import ControlPreparedEvent
from ep.control import ControlResumedEvent
from ep.control import ControlSuspendedEvent
from ep.control import ControlReleasedEvent
from ep.control import ControlQuitedEvent

class sip(control):
    def pipeline_logic(self):
        pipeline_state = {
            'run':   threading.Event(),
            'quit':  threading.Event(),
            'pause': threading.Event()
        }

        control_thread  = threading.Thread(target=control_loop, args=(pipeine_state,))
        pipeline_thread = threading.Thread(target=pipeline_definition, args=(pipeline_state,))

        # The control_thread will finish when we're donel the pipeline_thread
        # is daemonic.
        pipeline_thread.daemon = True
        control_thread.start()
        pipeline_thread.start()
        control_thread.join()
        self.logger.info("Control loop finished; shutting down")

    def control_loop(self, state):
        my_interface = ControllerPort_Interface()
        my_interface.send_event(ControlStartedEvent)

        class ReceiverThread(threading.Thread):
            # The receiver thread buffers all events received from the
            # eventport interface.
            def __init__(self, interface):
                super(ReceiverThread, self).__init__()
                self.interface = interface
                self.event_queue = collections.deque()
            def run(self):
                while True:
                    self.event_queue.append(self.interface.receive_event())
                
        event_receiver = ReceiverThread()
        event_receiver.daemon = True
        event_receiver.start()
        
        while True:
            try:
                # Check for newly received events
                current_event = event_receiver.event_queue.popleft()
            except IndexError:
                # No new events in the queue
                current_event = None

            # Handle any new events
            if isinstance(currentEvent, ControlConnectEvent):
                pass
            elif isinstance(currentEvent, ControlClaimEvent):
                pass
            elif isinstance(currentEvent, ControlPrepareEvent):
                pass
            elif isinstance(currentEvent, ControlSuspendEvent):
                state['run'].clear()
                my_interface.send(ControlSuspendedEvent())
            elif isinstance(currentEvent, ControlResumeEvent):
                state['run'].set()
                my_interface.send(ControlResumedEvent())
            elif isinstance(currentEvent, ControlReleaseEvent):
                pass
            elif isinstance(currentEvent, ControlQuitEvent):
                state['quit'].set() # Signal pipeline to stop at next opportunity
                my_interface.send(ControlQuitedEvent())
            elif isinstance(currentEvent, ControlResyncEvent):
                pass
            elif isinstance(currentEvent, ControlScheduleEvent):
                pass

            if state['finished'].is_set():
                # The pipeline thread reports all done.
                # Break out of the control loop, and we're all done.
                # (Do we need to notify MAC?)
                break

            time.sleep(1)
            
                
    def pipeline_definition(self, state):
        class PipelineQuit(Exception):
            pass

        self.logger.info("Waiting for run state")
        state['run'].wait()
        self.logger.info("Run state set; starting pipeline run")
        with log_time(self.logger):
            try:
                datafiles = self.run_task("vdsreader")
                state['run'].wait()
                if state['quit'].is_set():
                    self.logger.info("Pipeline instructed to quit; bailing out")
                    raise PipelineQuit
                datafiles = self.run_task("dppp_pass1", datafiles)
                state['run'].wait()
                if state['quit'].is_set():
                    self.logger.info("Pipeline instructed to quit; bailing out")
                    raise PipelineQuit
            except PipelineQuit:
                # On quit, we jump to here, do nothing, and execute the
                # finally block to set the finished state.
                pass
            finally:
                self.logger.info("Entering finished state")
                state['finished'].set()
        
       
if __name__ == '__main__':
    sys.exit(eval(os.path.splitext(os.path.basename(sys.argv[0]))[0])().main())

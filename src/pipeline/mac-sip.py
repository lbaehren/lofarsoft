"""
This is a model imaging pipeline definition.

Although it should be runnable as it stands, the user is encouraged to copy it
to a job directory and customise it as appropriate for the particular task at
hand.
"""
from __future__ import with_statement
import sys, os
import threading
import collections

# Generic pipeline stuff
from pipeline.master.control import control
from pipeline.support.utilities import log_time
from pipeline.support.lofarexceptions import PipelineQuit

# MAC controller interface
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

        # The control_thread will finish when we're done; the pipeline_thread
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
                self.active = True
            def run(self):
                while self.active:
                    self.event_queue.append(self.interface.receive_event())
            def next_event(self):
                try:
                    return self.event_queue.popleft()
                except IndexError:
                    return None
                
        event_receiver = ReceiverThread()
        event_receiver.daemon = True
        event_receiver.start()
        
        while True:
            # Check for newly received events
            current_event = event_receiver.next_event()

            # Handle any new events
            # NB!!! We need to properly initialise the events we send back to
            # MAC!
            if isinstance(currentEvent, ControlConnectEvent):
                pass
            elif isinstance(currentEvent, ControlClaimEvent):
                pass
            elif isinstance(currentEvent, ControlPrepareEvent):
                pass
            elif isinstance(currentEvent, ControlSuspendEvent):
                self.logger.debug("Clearing run state; pipeline must pause")
                state['run'].clear()
                my_interface.send(ControlSuspendedEvent())
            elif isinstance(currentEvent, ControlResumeEvent):
                self.logger.debug("Setting run state: pipeline may run")
                state['run'].set()
                my_interface.send(ControlResumedEvent())
            elif isinstance(currentEvent, ControlReleaseEvent):
                pass
            elif isinstance(currentEvent, ControlQuitEvent):
                self.logger.debug("Setting quit state: pipeline must exit")
                state['quit'].set() # Signal pipeline to stop at next opportunity
                my_interface.send(ControlQuitedEvent())
            elif isinstance(currentEvent, ControlResyncEvent):
                pass
            elif isinstance(currentEvent, ControlScheduleEvent):
                pass

            # The pipeline thread reports all done.
            # Break out of the control loop & the pipeline is finished.
            # The daemonic event_receiver shouldn't be a problem, but
            # let's stop it just in case.
            # (Do we need to notify MAC?)
            if state['finished'].is_set():
                self.logger.debug("Got finished state: control loop exiting")
                event_receiver.active = False
                break

            time.sleep(1)
            
                
    def pipeline_definition(self, state):
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
                # NB, this finally block should always set the finished state
                # to MAC, even if we exit unsuccessfully. Maybe we should
                # communicate that back?
                self.logger.info("Entering finished state")
                state['finished'].set()
        
       
if __name__ == '__main__':
    sys.exit(eval(os.path.splitext(os.path.basename(sys.argv[0]))[0])().main())

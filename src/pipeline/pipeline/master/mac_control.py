import sys, datetime, logging, os, time
import threading, collections
from pipeline.support.lofaringredient import LOFARinput, LOFARoutput
from pipeline.support.lofarexceptions import PipelineException, PipelineQuit
from pipeline.master.control import control
import pipeline.support.utilities as utilities
from ep.control import *
from ep.control import OK as controlOK

class MAC_control(control):
    """
    This control script communicates with MAC.
    """
    def __init__(self):
        super(MAC_control, self).__init__()
        self.optionparser.add_option('--controllername')
        self.optionparser.add_option('--servicemask')
        self.optionparser.add_option('--targethost')
        self.optionparser.add_option('--treeid')

    def pipeline_logic(self):
        # Define pipeline logic here in subclasses
        raise NotImplementedError

    def run_task(self, configblock, datafiles=[]):
        self.logger.info( "Waiting for run state...")
        self.state['run'].wait()
        self.logger.info( "Running.")
        self.logger.debug("Quit is %s" % (str(self.state['quit'].isSet())))
        if self.state['quit'].isSet():
            self.logger.info("Pipeline instructed to quit; bailing out")
            raise PipelineQuit
        try:
            super(MAC_control, self).run_task(configblock, datafiles)
        except PipelineException, message:
            self.logger.error(message)
#            raise PipelineQuit

    def go(self):
        super(control, self).go()
        self._setup()

        self.logger.info(
            "Standard Imaging Pipeline (%s) starting." %
            (self.name,)
        )

        self.state = {
            'run':      threading.Event(),
            'quit':     threading.Event(),
            'pause':    threading.Event(),
            'finished': threading.Event()
        }

        control_thread = threading.Thread(target=self.control_loop)
        pipeline_thread = threading.Thread(target=self.pipeline_logic)

        pipeline_thread.setDaemon(True)
        control_thread.start()
        pipeline_thread.start()
        control_thread.join()
        logging.info("Control loop finished; shutting down")
        return 0

    def control_loop(self):
        try:
            my_interface = ControllerPort_Interface(self.inputs['servicemask'], self.inputs['targethost'])
        except:
            self.logger.info("Control interface not connected")
            self.state['quit'].set()
            self.state['run'].set()
            return
        my_interface.send_event(ControlConnectEvent(self.inputs['controllername']))
        
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
                    self.logger.debug("Got a new event")
            def next_event(self):
                try:
                    return self.event_queue.popleft()
                except IndexError:
                    return None
                    
        event_receiver = ReceiverThread(my_interface)
        event_receiver.setDaemon(True)
        event_receiver.start()
        
        while True:
            # Check for newly received events
            current_event = event_receiver.next_event()
            
            # Handle any new events
            if isinstance(current_event, ControlConnectedEvent):
                self.logger.debug("Received ConnectedEvent")
            elif isinstance(current_event, ControlClaimEvent):
                self.logger.debug("Received ClaimEvent")
                my_interface.send_event(ControlClaimedEvent(self.inputs['controllername'], controlOK))
            elif isinstance(current_event, ControlPrepareEvent):
                self.logger.debug("Received PrepareEvent")
                my_interface.send_event(ControlPreparedEvent(self.inputs['controllername'], controlOK))
            elif isinstance(current_event, ControlSuspendEvent):
                self.logger.debug("Received SuspendEvent")
                self.logger.debug("Clearing run state; pipeline must pause")
                self.state['run'].clear()
                my_interface.send_event(ControlSuspendedEvent(self.inputs['controllername'], controlOK))
            elif isinstance(current_event, ControlResumeEvent):
                self.logger.debug("Received ResumeEvent")
                self.logger.debug("Setting run state: pipeline may run")
                self.state['run'].set()
                my_interface.send_event(ControlResumedEvent(self.inputs['controllername'], controlOK))
            elif isinstance(current_event, ControlReleaseEvent):
                self.logger.debug("Received ReleaseEvent")
                my_interface.send_event(ControlReleasedEvent(self.inputs['controllername'], controlOK))
            elif isinstance(current_event, ControlQuitEvent):
                self.logger.debug("Received QuitEvent")
                self.logger.debug("Setting quit state: pipeline must exit")
                self.state['quit'].set() # Signal pipeline to stop at next opportunity
                self.state['run'].set()  # Pipeline needs to be able to run in order to quit
                my_interface.send_event(ControlQuitedEvent(self.inputs['controllername'], self.inputs['treeid'], controlOK, "no error"))
            elif isinstance(current_event, ControlResyncEvent):
                self.logger.debug("Received ResyncEvent")
                my_interface.send_event(ControlResyncedEvent(self.inputs['controllername'], controlOK))
            elif isinstance(current_event, ControlScheduleEvent):
                self.logger.debug("Received ScheduleEvent")
                my_interface.send_event(ControlScheduledEvent(self.inputs['controllername'], controlOK))

            # The pipeline thread reports all done.
            # Break out of the control loop & the pipeline is finished.
            # The daemonic event_receiver shouldn't be a problem, but
            # let's stop it just in case.
            if self.state['finished'].isSet():
                self.logger.debug("Got finished state: control loop exiting")
                my_interface.send_event(ControlQuitedEvent(self.inputs['controllername'], self.inputs['treeid'], controlOK, "pipeline finished"))
                event_receiver.active = False
                break

            self.logger.debug("Control looping...")
            time.sleep(1)

if __name__ == '__main__':
    sys.exit(control().main())

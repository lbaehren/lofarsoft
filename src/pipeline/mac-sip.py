#!/usr/bin/python
"""
This is a model imaging pipeline definition.

Although it should be runnable as it stands, the user is encouraged to copy it
to a job directory and customise it as appropriate for the particular task at
hand.
"""
import sys, os, time
import threading
import collections

# Generic pipeline stuff
from pipeline.support.lofarexceptions import PipelineQuit

# MAC controller interface
#from ep.control import ControllerPort_Interface
#from ep.control import ControlConnectEvent
#from ep.control import ControlResyncedEvent
#from ep.control import ControlClaimedEvent
#from ep.control import ControlPreparedEvent
#from ep.control import ControlResumedEvent
#from ep.control import ControlSuspendedEvent
#from ep.control import ControlReleasedEvent
#from ep.control import ControlQuitedEvent
from ep.control import *
from ep.control import OK as controlOK

class sip(object):
    def __init__(self, cntlrname, servicemask, hostname, treeID):
        self.cntlrname = cntlrname
        self.servicemask = servicemask
        self.hostname = hostname
        self.treeID = treeID

    def pipeline_logic(self):
        self.state = pipeline_state = {
            'run':      threading.Event(),
            'quit':     threading.Event(),
            'pause':    threading.Event(),
            'finished': threading.Event()
        }

        control_thread  = threading.Thread(target=self.control_loop)
        pipeline_thread = threading.Thread(target=self.pipeline_definition)

        # The control_thread will finish when we're done; the pipeline_thread
        # is daemonic.
#        pipeline_thread.daemon = True
        pipeline_thread.setDaemon(True)
        control_thread.start()
        pipeline_thread.start()
        control_thread.join()
        logging.info("Control loop finished; shutting down")

    def control_loop(self):
        try:
            my_interface = ControllerPort_Interface(self.servicemask, self.hostname)
        except:
            logging.info("Control interface not connected")
            self.state['quit'].set()
            self.state['run'].set() 
            return 
        my_interface.send_event(ControlConnectEvent(self.cntlrname))

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
                    logging.info("Got a new event")
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
                logging.info("Received ConnectedEvent")
            elif isinstance(current_event, ControlClaimEvent):
                logging.info("Received ClaimEvent")
                my_interface.send_event(ControlClaimedEvent(self.cntlrname, controlOK))
            elif isinstance(current_event, ControlPrepareEvent):
                logging.info("Received PrepareEvent")
                my_interface.send_event(ControlPreparedEvent(self.cntlrname, controlOK))
            elif isinstance(current_event, ControlSuspendEvent):
                logging.info("Received SuspendEvent")
                logging.info("Clearing run state; pipeline must pause")
                self.state['run'].clear()
                my_interface.send_event(ControlSuspendedEvent(self.cntlrname, controlOK))
            elif isinstance(current_event, ControlResumeEvent):
                logging.info("Received ResumeEvent")
                logging.info("Setting run state: pipeline may run")
                self.state['run'].set()
                my_interface.send_event(ControlResumedEvent(self.cntlrname, controlOK))
            elif isinstance(current_event, ControlReleaseEvent):
                logging.info("Received ReleaseEvent")
                my_interface.send_event(ControlReleasedEvent(self.cntlrname, controlOK))
            elif isinstance(current_event, ControlQuitEvent):
                logging.info("Received QuitEvent")
                logging.info("Setting quit state: pipeline must exit")
                self.state['quit'].set() # Signal pipeline to stop at next opportunity
                self.state['run'].set()  # Pipeline needs to be able to run in order to quit
                my_interface.send_event(ControlQuitedEvent(self.cntlrname, self.treeID, controlOK, "no error"))
            elif isinstance(current_event, ControlResyncEvent):
                logging.info("Received ResyncEvent")
                my_interface.send_event(ControlResyncedEvent(self.cntlrname, controlOK))
            elif isinstance(current_event, ControlScheduleEvent):
                logging.info("Received ScheduleEvent")
                my_interface.send_event(ControlScheduledEvent(self.cntlrname, controlOK))

            # The pipeline thread reports all done.
            # Break out of the control loop & the pipeline is finished.
            # The daemonic event_receiver shouldn't be a problem, but
            # let's stop it just in case.
            # (Do we need to notify MAC?)
            if self.state['finished'].isSet():
                logging.info("Got finished state: control loop exiting")
                my_interfae.send_event(ControlQuitedEvent(self.cntlrname, self.treeID, controlOK, "pipeline finished"))
                event_receiver.active = False
                break

            logging.info("Control looping...")
            time.sleep(1)

    def __checkpoint(self):
        logging.info( "Waiting for run state...")
        self.state['run'].wait()
        logging.info( "Running.")
        logging.info("Quit is %s" % (str(self.state['quit'].isSet())))
        if self.state['quit'].isSet():
            logging.info("Pipeline instructed to quit; bailing out")
            raise PipelineQuit

    def pipeline_definition(self):
        try:
            self.__checkpoint()
            logging.info( "RUN TASK 1")
            time.sleep(5)
            logging.info( "TASK 1 DONE")
            self.__checkpoint()
            logging.info( "RUN TASK 2")
            time.sleep(5)
            logging.info("TASK 2 DONE")
            self.__checkpoint()
            logging.info( "RUN TASK 3")
            time.sleep(5)
            logging.info("TASK 3 DONE")
            self.__checkpoint()
            logging.info( "RUN TASK 4")
            time.sleep(5)
            logging.info("TASK 4 DONE")
            self.__checkpoint()
            logging.info( "RUN TASK 5")
            time.sleep(5)
            logging.info("TASK 5 DONE")
            self.__checkpoint()
            logging.info( "RUN TASK 6")
            time.sleep(5)
            logging.info("TASK 6 DONE")
            self.__checkpoint()
            logging.info( "RUN TASK 7")
            time.sleep(5)
            self.__checkpoint()
            logging.info("TASK 7 DONE")
            self.__checkpoint()
            logging.info( "RUN TASK 8")
            time.sleep(5)
            logging.info("TASK 8 DONE")
        except PipelineQuit:
            # On quit, we jump to here, do nothing, and execute the
            # finally block to set the finished state.
            pass
        # NB, this finally block should always set the finished state
        # to MAC, even if we exit unsuccessfully. Maybe we should
        # communicate that back?
        logging.info( "Entering finished state")
        self.state['finished'].set()

if __name__ == '__main__':
#	1. name of the parset file (abs path)
#	2. name of machine to connect to
#    3. service name
#    4. our name
    import logging
    LOG_FILENAME = '/home/swinbank/log/mac-sip.log'
    logging.basicConfig(filename=LOG_FILENAME,level=logging.DEBUG)
    parset, target_host, service_name, cntlr_name = sys.argv[1:5]


    logging.info("would use parset: " + parset)
    logging.info(cntlr_name)
    logging.info(service_name)
    logging.info(target_host)
    pipeline = sip(cntlr_name, service_name, target_host, int(parset[-4:]))
    pipeline.pipeline_logic()

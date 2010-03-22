#!/usr/bin/env python
from WSRTrecipe import *
from job_parser import *
import os, os.path, time, threading, types, thread, sys

NewJob     = 1
UpdateJob  = 2
RestartCom = 3

##--------------------- Server Code, should maybe be in separate module? --------------------------
class server_wrapper(threading.Thread):
    """Base class to create a server to listen for new jobs in a separate thread.
    Mainly implements how to talk with the pipeline manager."""
    def __init__(self, server): ## might need to be overridden
        threading.Thread.__init__(self)
        self.server     = server
        self.stop       = False
        self.stop_lock  = threading.Lock()
        self.events     = [] ## list of received events. Main thread will remove them
        self.event_lock = threading.Lock()  ## lock to make sure only one is using this list
        self.event      = threading.Event() ## event to signal the main thread
        self.setDaemon(True) ## this kills the server when the main thread finishes
    
    def run(self):
        """Empty function override in a subclass"""
        pass ## override in sub-class
        
##    def new_job(self, fileName):
##        """Send an event to the pipeline manager that a new job is available."""
##        self.event_lock.acquire()
##        self.events.append((NewJob, fileName))
##        self.event.set() ## signal the main thread we've done something
##        self.event.clear()
##        self.event_lock.release()

    def new_job(self, fileName, fileContent):
        """Send an event to the pipeline manager that a new job is available."""
        self.event_lock.acquire()
        self.events.append((NewJob, fileName, fileContent))
        self.event.set() ## signal the main thread we've done something
        self.event.clear()
        self.event_lock.release()

    def update_job(self, exportID, status):
        """Send an event to the pipeline manager that a job needs to be updated."""
        self.event_lock.acquire()
        self.events.append((UpdateJob, exportID, status))
        self.event.set() ## signal the main thread we've done something
        self.event.clear()
        self.event_lock.release()

    def restart(self):
        """function that gets called when a restart of the communication
        of jobs statusses is needed."""
        self.event_lock.acquire()
        self.events.append((RestartCom))
        self.event.set() ## signal the main thread we've done something
        self.event.clear()
        self.event_lock.release()

    def serve_forever(self):
        """Accept requests until the pipeline manager signals to stop."""
        self.server.socket.settimeout(60)
        while 1:
            #? self.stop_lock.acquire() doesn't seem necessary
            if self.stop:
                #? self.stop_lock.release() doesn't seem necessary
                break
            #? self.stop_lock.release() doesn't seem necessary
            self.server.handle_request()
##--------------------- End Server Code --------------------------

class pipeline_manager(WSRTrecipe):
    def __init__(self):
        WSRTrecipe.__init__(self)
        ## inputs
        self.inputs['ConfigurationFile'] = 'pipeline_manager_config' ## without the .py
        self.inputs['NewJobsDirectory']  = ''
        self.inputs['JobsDirectory']     = ''
        self.inputs['LogDirectory']      = ''

        ##outputs
        self.outputs['failed_communication'] = []

        ## Help text
        self.helptext = """
        Script to run as a server for executing individial jobs
        ConfigurationFile should be given without the '.py' extention.
        NewJobs is where new jobs should be written, the JobsDirectory
        contains unfinished Jobs"""
        
        self.jobs                 = []
        self.parser               = job_parser()
        self.parser.messages      = self.messages
        self.running_job          = None
    
    ## Code to generate results ---------------------------------------------
    def startup(self):
        """Tell the user we stared, read the configuration and try to read unfinished jobs from JobDirectory"""
        print 'WSRT pipeline manager version 0.5'
        print 'Press Ctrl-C to abort'
        exec(eval("'from %s import *' % self.inputs['ConfigurationFile']"))
        self.log     = file(self.inputs['LogDirectory'] + '/pipeline_manager.log', 'a', 1)
        self.messages.addlogger(message.DebugLevel, self.log)
        self.print_message('----- Logging started -----')
        ExistingJobs = os.listdir(self.inputs['JobsDirectory'])
        self.server  = server
        self.client  = client
        self.restart_communication()
        for e in ExistingJobs:
            self.new_job(e, "")

    def communicate_job(self, job): 
        """function to write to log and communicate with GUI"""
        if   job['Status'] == JobError:     self.print_notification('Job:' + str(job['ExportID']) + ' Failed')
        elif job['Status'] == JobHold:      self.print_notification('Job:' + str(job['ExportID']) + ' is on Hold')
        elif job['Status'] == JobScheduled: self.print_notification('Job:' + str(job['ExportID']) + ' Scheduled')
        elif job['Status'] == JobProducing: self.print_notification('Job:' + str(job['ExportID']) + ' Started')
        elif job['Status'] == JobProduced:  self.print_notification('Job:' + str(job['ExportID']) + ' Produced')
        try:
            if not isinstance(self.client, types.NoneType):
                (status, message) = self.client.setStatus(str(job['ExportID']), str(job['Status']))
                if status: ## we retry, because the client does not do an internal retry, but only reports the problem
                    count = 1
                    while (status and (count < 10)):
                        self.print_notification("Got some error, retrying " + str(job['ExportID']) + ": " + message)
                        time.sleep(60)
                        (status, message) = self.client.setStatus(str(job['ExportID']), str(job['Status']))
                        count += 1
                if status:
                    self.print_error(message)
                else:
                    self.print_message(message)
        except:
            self.outputs['failed_communication'].append((job['ExportID'], job['Status']))
            self.set_run_info(self.inputs['LogDirectory'])
            self.print_error('Could not update job %s status to %s.' % (str(job['ExportID']), str(job['Status'])))

    def restart_communication(self):
        """Try to tell the client what we failed to tell earlier."""
        results = self.get_run_info(self.inputs['LogDirectory'])
        if not results: return
        if not results[self.name]: return
        for i in results[self.name]['outputs']['failed_communication']:
            try:
                if not isinstance(self.client, types.NoneType):
                    self.print_message(self.client.setStatus(i[0], i[1]))
            except:
                self.print_error('Could not update job %s status to %s.' % (str(job['ExportID']), str(job['Status'])))
                return
        self.outputs['failed_communication'] = []
        self.set_run_info(self.inputs['LogDirectory'])

    def new_job(self, filename, fileContent):
        """Read filename and add to the list of jobs if it is a valid file."""
        import shutil
        try:
            if fileContent:
                f = open(self.inputs['NewJobsDirectory'] + '/' + filename, 'w')
                f.write(fileContent)
                f.close()
            shutil.move(self.inputs['NewJobsDirectory'] + '/' + filename, self.inputs['JobsDirectory'] + '/' + filename)
        except:
            self.print_debug('file not found (existing job?): ' + self.inputs['NewJobsDirectory'] + '/' + filename)
        self.parser.inputs['Job'] = self.inputs['JobsDirectory'] + '/' + filename
        self.parser.outputs       = ingredient.WSRTingredient() ## empty ingredient to be able to run more than once
        self.parser.go()
        job             = self.parser.outputs.copy()
        job['filename'] = filename
        if job['Status'] == JobScheduled:
            self.jobs.append(job)
            self.communicate_job(job)
        else:
            self.print_notification('Parsing ' + self.inputs['JobsDirectory'] + '/' + filename + ' failed') ## Not implemented yet

    def find_job(self, exportID):
        for j in self.jobs:
            if j['ExportID'] == exportID:
                return j
        return None

    def update_job(self, exportID, status): 
        """for communicating job status with GUI, mainly to put on Hold."""
        j = self.find_job(exportID)
        if j:
            j['Status'] = status
            self.communicate_job(j)
        else:
            self.print_debug('Job ' + str(exportID) + ' not found, ignoring message.')

    def check_server(self):
        """Check if there are any new jobs communicated to the server."""
        self.server.event.wait(10)
        self.server.event_lock.acquire()
        while len(self.server.events) > 0:
            job = self.server.events.pop(0)
            if job[0] == NewJob:
                self.new_job(job[1], job[2])
            elif job[0] == UpdateJob:
                self.update_job(job[1], job[2])
            elif job[0] == RestartCom:
                self.restart_communication()
        self.server.event_lock.release()

    def next_job(self):
        """See if there is another job scheduled, then start it."""
        import shutil
        for j in self.jobs:
            if j['Status'] >= JobScheduled:
                if j['Status'] > JobScheduled:
                    self.print_notification('Restarting job: ' + j['ExportID'])
                j['Status'] = JobProducing
                self.running_job = j
                self.communicate_job(j)
                self.cook_job(j)
                self.communicate_job(j)
                shutil.move(self.inputs['JobsDirectory'] + '/' + j['filename'], 
                            self.inputs['LogDirectory'] + '/' + str(j['ExportID']) + '/' + j['filename'])
                self.jobs.remove(j) ## we can not use .pop() because the first job might be on hold
                self.running_job = None ## tell ourselves that we're doing nothing
                return ## we did a jobs.remove() so we don't what to stay in the for loop!
    
    def prepare_recipe_parameters(self, job):
        """Prepare ingedients and message handler for the cook to cook the recipe."""
        import sys
        logfile = file(self.inputs['LogDirectory'] + '/' + str(job['ExportID']) + '/pipeline_manager.log', 'a', 1)
        messages = message.WSRTmessages()
        results  = ingredient.WSRTingredient()
        if self.messages.log[sys.stdout] == message.DebugLevel:
            messages.addlogger(message.DebugLevel, logfile)
            messages.setloglevel(message.DebugLevel, sys.stdout)
        else:
            messages.addlogger(message.VerboseLevel, logfile)
            messages.setloglevel(message.NotifyLevel, sys.stdout)
        inputs = job.copy()
        del inputs['scriptname']
        del inputs['Status']
        del inputs['filename']
        return (logfile, inputs, results, messages)

    def cook_job(self, job):
        """This starts a recipe with the inputs as defined in the job file."""
        if not os.path.isdir(self.inputs['LogDirectory'] + '/' + str(job['ExportID'])):
            os.makedirs(self.inputs['LogDirectory'] + '/' + str(job['ExportID']))
        logfile, inputs, results, messages = self.prepare_recipe_parameters(job)
        try:
            self.cook_recipe(job['scriptname'], inputs, results, messages)
        except Exception, e:
            messages.append(message.ErrorLevel, str(e))
            job['Status'] = JobError
            results       = None
        if results:
            job['Status'] = JobProduced # something more elaborate?
            messages.append(message.VerboseLevel, 'Results:')
            for o in results.keys():
                messages.append(message.VerboseLevel, str(o) + ' = ' + str(results[o]))
        else: # should a recipe always have results?
            messages.append(message.VerboseLevel, 'No Results!')
            job['Status'] = JobError
        logfile.close()
        ## dump the logfile to the webdav as a dataproduct.
        if 'repository' in job.keys():
            try:
                temp = ingredient.WSRTingredient()
                temp['server']        = job['repository'][0]
                temp['resultdir']     = job['repository'][1]
                temp['filepath']      = self.inputs['LogDirectory'] + '/' + str(job['ExportID'])
                temp['filename']      = 'pipeline_manager.log'
                temp['resultfilename']= str(job['ExportID']) + '_pipeline.log'
                self.cook_recipe('put_pipeline_log', temp, temp)
            except:
                self.print_notification('failed writing pipeline log.')

    def print_time(self):
        t = time.gmtime()
        timestring = '\r%04d-%02d-%02d %02d:%02d:%02d' % (t[0], t[1], t[2], t[3], t[4], t[5])
        if self.running_job:
            timestring += ' Busy running job: ' + self.running_job['ExportID']
        else:
            if self.jobs:
                timestring += ' checking for new jobs.'
            else:
                ##timestring += ' No jobs available, waiting for next job.'
                timestring = '.'
        sys.stdout.write(timestring)
        sys.stdout.flush()

    def go(self):
        self.startup()
        try:
            while True: ##run forever
                try:
                    if not self.running_job:
                        thread.start_new_thread((self.next_job), ())
                    self.print_time()
                    if not isinstance(self.server, types.NoneType):
                        self.check_server()
                        time.sleep(1) # temp fix as apparantly check_server can return fast enough to re-enter
                                      # next_job before the previous one gets to self.running_job = j
                                      # should be something with a lock like self.stop = False  self.stop_lock = threading.Lock()
                    else:
                        if self.jobs:
                            time.sleep(10)
                        else:
                            raise Exception ("No more jobs and no Server, ending manager.")
                except KeyboardInterrupt:
                    self.print_notification('Pipeline Manager: Keyboard interupt detected, asking user...')
                    reply = raw_input('Do you want to end the pipeline manager (y/n)?')
                    if 'y' in reply:
                        raise KeyboardInterrupt ('Pipeline Manager: User wants to end program')
        except KeyboardInterrupt, k:
            self.print_notification(str(k))
        except Exception, inst:
            self.print_error('Pipeline Manager: Exception caught: ' + str(type(Exception)) + ' ' + str(inst))
            raise inst
        if not isinstance(self.server, types.NoneType): ## check if the server is alive
            self.server.stop_lock.acquire()
            self.server.stop = True ## tell the server to stop
            self.server.stop_lock.release()

## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    standalone = pipeline_manager()
    standalone.main()

#!/usr/bin/env python
from WSRTrecipe import *

JobError     = -1
JobHold      =  0
JobScheduled =  1
JobProducing =  2
JobProduced  =  3

class job_parser(WSRTrecipe):
    def __init__(self):
        WSRTrecipe.__init__(self)
        ##inputs
        self.inputs['Job'] = ''
        self.name = 'job parser'
        # We have no outputs
        ## Help text
        self.helptext = """
        Script to parse an XML job file for use by the pipeline_manager.
        See the exportjob.dtd for a definition of the format of a valid file."""
    
    ## Code to generate results ---------------------------------------------
    def go(self):
        try:
            from xml.dom import minidom, Node
            doc = minidom.parse(self.inputs['Job'])
            if doc.documentElement.nodeName == 'exportjob':
                self.outputs['ExportID'] = str(doc.documentElement.attributes.get('exportID').nodeValue)
                for node in doc.documentElement.childNodes:
                    if node.nodeName == 'scriptname':
                        value = node.childNodes[0].nodeValue
                        self.outputs['scriptname'] = value
                    elif node.nodeName == 'repository':
                        for itemnode in node.childNodes:
                            if itemnode.nodeName == 'server':
                                name = itemnode.childNodes[0].nodeValue
                            elif itemnode.nodeName == 'resultdir':
                                res  = itemnode.childNodes[0].nodeValue
                        if res and name: 
                            self.outputs['repository'] = (name, res)
                    elif node.nodeName == 'inputlist':
                        name  = "'" + node.attributes.get('name').nodeValue + "'"
                        exec(eval("'self.outputs[%s] = []' % (name)"))
                        for itemnode in node.childNodes:
                            if itemnode.nodeName == 'listitem':
                                value = itemnode.childNodes[0].nodeValue
                                exec(eval("'self.outputs[%s].append(%s)' % (name, value)"))
                    elif node.nodeName == 'input':
                        name  = "'" + node.attributes.get('name').nodeValue + "'"
                        value = node.childNodes[0].nodeValue
                        #try: # we should just interpret the value, and have the cook/script worry about if it's an int or string.
                        if value == 'True' or value == 'False':
                            exec(eval("'self.outputs[%s] = %s' % (name, value)"))
                        #except:
                        else:
                            value = "'''" + value + "'''" ## tripple quotes because a value could be "8 O'clock" for example
                            exec(eval("'self.outputs[%s] = %s' % (name, value)"))
            if self.outputs['ExportID']: ## we need an export ID to identify the job
                self.outputs['Status'] = JobScheduled
                return
        except Exception, inst:
            self.print_notification('Failed importing job: ' + self.inputs['Job'] + '; Error: ' + str(inst))
        self.outputs['Status'] = JobError

## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    standalone = job_parser()
    standalone.main()

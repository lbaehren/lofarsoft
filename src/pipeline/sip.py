import sys, os
from pipeline.master.control import control

class sip(control):
    def pipeline_logic(self):
        datafiles = self.run_task("vdsreader")
        datafiles = self.run_task("dppp", datafiles)
        datafiles = self.run_task("exclude_DE001LBA", datafiles)
        datafiles = self.run_task("trim_300", datafiles)
        datafiles = self.run_task("bbs", datafiles)
        datafiles = self.run_task("local_flag", datafiles)
        self.outputs['images'] = self.run_task("mwimager", datafiles)
        self.outputs['average'] = self.run_task("collector")
        self.run_task("qcheck", self.outputs['images'])
        self.run_task("sourcefinder", self.outputs['average'])
       
if __name__ == '__main__':
    sys.exit(eval(os.path.splitext(os.path.basename(sys.argv[0]))[0])().main())

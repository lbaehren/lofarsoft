from pipeline.master.pipeline import pipeline

class sip(pipeline):
    def pipeline_logic(self):
        datafiles = self.run_task("vdsreader")
        datafiles = self.run_task("dppp", datafiles)
        datafiles = self.run_task("exclude_DE001LBA", datafiles)
        datafiles = self.run_task("trim_300", datafiles)
        datafiles = self.run_task("bbs", datafiles)
        datafiles = self.run_task("local_flag", datafiles)
        self.outputs['images'] = self.run_task("mwimager", datafiles)
        self.outputs['average'] = self.run_task("collector")
        self.run_task("sourcefinder", self.outputs['average'])
       

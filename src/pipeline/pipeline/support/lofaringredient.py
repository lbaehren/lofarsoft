from cuisine.ingredient import WSRTingredient

class LOFARinput(WSRTingredient):
    """
    All LOFAR pipeline ingredients are required to provide a few basic
    parameters: 

    * job_name
    * runtime_directory
    * config
    * dry_run
    * start_time
    """
    def __init__(self, defaults):
        super(LOFARinput, self).__init__(self)
        for param in (
            "job_name", "runtime_directory", "config", "dry_run", "start_time"
        ):
            if defaults.has_key(param):
                self[param] = defaults[param]
            else:
                self[param] = None

class LOFARoutput(WSRTingredient):
    pass
        

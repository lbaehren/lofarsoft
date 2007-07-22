
class Image:
    """Image dataholder"""
    opts = None

    def __init__(self, opts):
        self.opts = Opts(opts)


class Op:
    """Common parent for all image operations"""
    def __call__(self, img):
        raise NotImplementedError("This method should be redifined")


class Opts:
    """Options storage"""
    def __init__(self, defaults):
        self.__dict__["_Opts__defaults"] = {}
        self.__dict__["_Opts__values"] = {}
        for k, v in defaults.iteritems():
            if isinstance(v, dict):
                self.__defaults[k] = Opts(v)
            else:
                self.__defaults[k] = v

    def __repr__(self):
        return "bdsm.Opts=" + repr(self.__dict__)
    
    def _has_key(self, name):
        if self.__defaults.has_key(name) or self.__values.has_key(name):
            return True
        else:
            return False
    
    def _apply(self, vals):
        for k, v in vals.iteritems():
            if isinstance(v, dict):
                if self._has_key(k) and isinstance(self[k], Opts):
                    self[k]._apply(v)
                else:
                    self._add_group(k, v)
            else:
                self[k] = v
    
    def _reset(self):
        self.__values = {}
        for k, v in self.__defaults.iteritems():
            if isinstance(v, Opts):
                v._reset()
        
    def _add_group(self, name, defaults):
        self.__values[name] = Opts(defaults)
        
        ### Options access functions
    def __getattr__(self, name):
        if self.__values.has_key(name):
            return self.__values[name]
        else:
            return self.__defaults[name]

    def __getitem__(self, name):
        return self.__getattr__(name)

    def __setattr__(self, name, value):
        if self.__dict__.has_key(name):
            self.__dict__[name] = value
            return
        self.__values[name] = value

    def __setitem__(self, name, value):
        self.__setattr__(name, value)
        
    def __delattr__(self, name):
        if self.__values.has_key(name):
            del self.__values[name]
        elif self.__defaults.has_key(name) and isinstance(self.__defaults[name], Opts):
            self.__defaults[name]._reset()
        else:
            raise KeyError(name)

    def __delitem__(self, name):
        self.__delattr__(name)

import numpy as N
import numpy.core.ma as M
from Image import Op

def intersect8(run, runs):
    """Check whether the run 'run' intersects with one of the
    runs from 'runs' according to 8-connectivity rules.
    Beware that it does not checks whether the runs are located
    on consequitive lines.
    """
    for t in runs:
        if min(run[2],t[2])-max(run[1],t[1]) >= 0 :
            return True
    return False

def split(arr, idx):
    """Split array arr into the set of contiguous runs
    Returns the list of 3-tuples, where first element is
    idx, other two are the 'begin' and 'end' indices of the run.
    The 'end' index is an 'last + 1' index as is used by slicing
    notation.
    """
        ### first trivial cases -- none and all
    if not arr.any():
        return []
    if arr.all():
        return [(idx, 0, len(arr))]
        ### now start splitting
    res = []; i = 0
    while i < len(arr):
        if not arr[i:i+10].any():
            i += 10; continue
        for j in xrange(i, i+10):	# find first active pixel
            if arr[j]:
                i = j; break
        j = i + 1			# now look for the end of run
        while j <= len(arr):
            if arr[i:j+5].all():
                j += 5; continue
            for j in xrange(j, j+5):
                if j == len(arr) or not arr[j]:
                    break
            break
        res.append((idx, i, min(j, len(arr))))
        i = j
    return res

class Op_islands(Op):
    class _isl:
        def __init__(self, run):
            self.new = []
            self.cur = [run]
            self.old = []
        
        def __repr__(self):
            return "ISL"+str((self.new, self.cur, self.old))

        __str__ = __repr__
         
        def empty(self): ### check if empty
            return (self.new, self.cur, self.old) == ([], [], [])
        
        def closed(self): ### check if closed
            return self.new == []
        
        def runs(self):
            return self.old + self.cur + self.new
        
        def add(self, run): ### add new run
            self.new.append(run)
        
        def merge(self, isl): ### merge with other island
            self.new.extend(isl.new); isl.new = []
            self.cur.extend(isl.cur); isl.cur = []
            self.old.extend(isl.old); isl.old = []
        
        def endline(self): ### rotate arrays
            self.old.extend(self.cur)
            self.cur = self.new
            self.new = []
            return self

    class _isl_list:
        def __init__(self):
            self.new = []
            self.cur = []
            self.old = []
        
        def __str__(self):
            return "ISL_LIST(%s,\n%s,\n%s)" % (
                str(self.new), str(self.cur), str(self.old))
        
        def flatten(self):
            res = []
            for i in (self.old + self.cur + self.new):
                res.append(i.runs())
            return res
        
        def add_line(self, runs, intersect = intersect8):
            for r in runs:
                found = None
                for i in self.cur:
                    if intersect(r, i.cur):
                        if found:
                            found.merge(i)
                        else:
                            i.add(r)
                            found = i
                if not found:
                    self.new.append(Op_islands._isl(r))
            self.endline()
        
        def endline(self): ### next line
            for i in reversed(xrange(len(self.cur))):
                if self.cur[i].empty():
                    del self.cur[i]
                    continue
                if self.cur[i].closed():
                    self.old.append(self.cur[i].endline())
                    del self.cur[i]
                    continue
                self.cur[i].endline()
            
            self.cur.extend(self.new)
            self.new = []

    def parser(self):    
        return Op_islands._isl_list()

    def isl_size(self, isl):
        size = 0
        for run in isl:
            size += run[2] - run[1]
        return size

    def __call__(self, img):
        data = img.img
        opts = img.opts

            ### islands detection
        islands = self.parser()
        for idx in xrange(data.shape[0]):
            mask = (data[idx] > opts.thresh_isl).filled(fill_value = False)
            runs = split(mask, idx)
            islands.add_line(runs)

            ### convert to formal islands and store
        isl = islands.flatten()
        res = []
        for i in isl:
            if self.isl_size(i) > opts.isl_min_size:
                t = island(img, i)
                if t.masked.max() > opts.thresh_isl2:
                    res.append(t)

        img.islands = res
        img.rislands = isl

        print "Total islands found: " + str(len(isl))
        print "Large islands found: " + str(len(res))

        return img

class island:
    """Island
    """
    def __init__(self, img, runs):
        self.runs = runs
        self.bbox = bb = self._bbox(runs, img.img.shape)
        self.slices = (slice(bb[0], bb[1]), slice(bb[2], bb[3]))
        self.img  = M.array(img.img[self.slices], copy = False)
        self.opts = img.opts
        self.shape = self.img.shape

          ## create masks:
          ## isl_mask for the island itself
          ## mask     for island and surrounding noise
	mask = self._noise_mask(img.opts.mean, img.opts.thresh_isl)
        isl_mask = N.ones(self.shape, dtype=bool)
        bb = self.bbox; x1o = bb[0]; x2o = bb[2]
        size = 0
        for r in runs:
            size += r[2]-r[1]
            mask[r[0]-x1o, (r[1]-x2o):(r[2]-x2o)] = False
            isl_mask[r[0]-x1o, (r[1]-x2o):(r[2]-x2o)] = False
        self.mask = mask;
        self.isl_mask = isl_mask
        self.size = size
        self.masked = M.array(self.img.data, mask = mask, copy = False)
    
    def _bbox(self, runs, shape):
        x1min = x1max = runs[0][0]
        x2min = x2max = runs[0][1]
        for r in runs:
            x1min = min(x1min, r[0])
            x1max = max(x1max, r[0])
            x2min = min(x2min, r[1])
            x2max = max(x2max, r[2])
          ## add one extra pixel on each side of the island
        return (max(0, x1min-1), min(shape[0], x1max+2),
                max(0, x2min-1), min(shape[1], x2max+1))

    def _noise_mask(self, mean, thresh):
        arr = self.img
        return (abs(arr - mean) > thresh).filled(fill_value = True)

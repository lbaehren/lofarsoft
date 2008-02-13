import numpy as N
from image import Op

try:
    import scipy.ndimage as nd
    _use_ndimage = True
except ImportError:
    _use_ndimage = False

class Op_islands(Op):
    """Detect islands of emission in the image

    Adds to the image attribute 'islands' with the list of detected islands
    """
    def __call__(self, img):
        data_mask = (None if img.img_mask is img.nomask else img.img_mask)
        parameters = (img.opts.mean, img.opts.thresh_isl, img.opts.thresh_isl2,
                      img.opts.isl_min_size)

        if _use_ndimage:
            islands = self.ndimage_alg(img, data_mask, *parameters)
        else:
            islands = self.pixel_runs_alg(img, data_mask, *parameters)

        img.islands = islands
        print "Islands found: " + str(len(islands))
        return img

    def ndimage_alg(self, img,  data_mask, mean, thresh1, thresh2, minsize):
        """Island detection algorithm using image manipulation routines 
        from scipy.ndimage.measurements package.
        """
        data = img.img

        ### islands detection
        mask = data - mean > thresh1
        if data_mask is not None:
            mask[data_mask] = False
        labels, count = nd.label(mask, nd.generate_binary_structure(2,2))
        slices = nd.find_objects(labels)

        ### filter out noise and convert to 'real' islands
        res = []
        for idx, s in enumerate(slices):
            idx += 1            # indices are counted from 1
            if (labels[s] == idx).sum() >= minsize and \
                    nd.maximum(data[s], labels[s], idx) - mean > thresh2:
                res.append(Island(img, labels=(labels, s, idx)))

        return res

    def pixel_runs_alg(self, img, data_mask, mean, thresh1, thresh2, minsize):
        """Island detection algorithm using pixel runs. It's somewhat slower
        that ndimage_alg, but is more memory efficient.
        """
        data = img.img

        ### islands detection
        islands = _isl_list()
        for idx in xrange(data.shape[0]):
            mask = data[idx] - mean > thresh1
            if data_mask is not None:
                mask[data_mask[idx]] = False
            runs = _split(mask, idx)
            islands.add_line(runs)

        ### filter out noise and convert to 'real' islands
        isl = islands.flatten()
        res = []
        for i in isl:
            if self._runs_size(i) >= minsize:
                t = Island(img, runs=i)
                if t.img[N.logical_not(t.isl_mask)].max() - mean > thresh2:
                    res.append(t)

        return res

    def _runs_size(self, runs):
        """Calculate number of pixels covered by runs"""
        size = 0
        for run in runs:
            size += run[2] - run[1]
        return size



class Island:
    """This class represents an island.

    Most usefull attributes:
      bbox      : bounding box of the island (in the image coordinates)
                  (slice(x0min, x0max), slice(x1min, x1max))
      origin    : coordinates of the lower-left corner of the island
      img       : image of the islands
      isl_mask  : mask, where all but 'active' pixels are flagged
      noisy_mask: mask including active pixels and surrounding noise.
                  i.e. accidental active pixel or neighbour islands are masked out
      shape     : shape of the island
      size      : number of active pixels in the island
      opts      : reference to image's options
      runs      : pixel runs comprising the island (present only when pixel-runs
                  algorithm is used for island detection)
    """
    def __init__(self, img, **args):
        if 'runs' in args:
            self._init_from_runs(img, args['runs'])
        elif 'labels' in args:
            self._init_from_labels(img, args['labels'])
        else:
            raise ValueError('Not enough data to initialize Island')
    
    def _init_from_labels(self, img, args):
        labels, bbox, idx = args

        bbox = self._expand_bbox(bbox, img.img.shape)
        origin = (bbox[0].start, bbox[1].start)
        data = img.img[bbox]

        ### create (inverted) masks
        isl_mask = (labels[bbox] == idx)
        noise_mask = (labels[bbox] == 0)
        N.logical_or(noise_mask, isl_mask, noise_mask)

        isl_size = isl_mask.sum()

        ### invert masks
        N.logical_not(isl_mask, isl_mask)
        N.logical_not(noise_mask, noise_mask)
        if img.img_mask is not img.nomask:
            noise_mask[img.img_mask[bbox]] = True

        self.bbox = bbox
        self.origin = origin
        self.img  = data
        self.isl_mask = isl_mask
        self.noisy_mask = noise_mask
        self.shape = data.shape
        self.size = isl_size
        self.opts = img.opts

    def _init_from_runs(self, img, runs):
        bbox = self._bbox_from_runs(runs, img.img.shape)
        origin = (bbox[0].start, bbox[1].start)
        data = img.img[bbox]

        ### create masks
        noise_mask = data - img.opts.mean > img.opts.thresh_isl
        isl_mask = N.ones(data.shape, dtype=bool)
        size = 0
        x1o, x2o = origin
        for r in runs:
            size += r[2]-r[1]
            noise_mask[r[0]-x1o, (r[1]-x2o):(r[2]-x2o)] = False
            isl_mask  [r[0]-x1o, (r[1]-x2o):(r[2]-x2o)] = False
        if img.img_mask is not img.nomask:
            noise_mask[img.img_mask[bbox]] = True

        self.bbox = bbox
        self.origin = origin
        self.img  = data
        self.isl_mask = isl_mask
        self.noisy_mask = noise_mask
        self.shape = data.shape
        self.size = size
        self.opts = img.opts
        self.runs = runs

    def _bbox_from_runs(self, runs, shape):
        x1min = x1max = runs[0][0]
        x2min = x2max = runs[0][1]
        for r in runs:
            x1min = min(x1min, r[0])
            x1max = max(x1max, r[0])
            x2min = min(x2min, r[1])
            x2max = max(x2max, r[2])
          ## add one extra pixel on each side of the island
        return (slice(max(0, x1min-1), min(shape[0], x1max+2)),
                slice(max(0, x2min-1), min(shape[1], x2max+1)))

    def _expand_bbox(self, bbox, shape):
        return (slice(max(0, bbox[0].start - 1), min(shape[0], bbox[0].stop + 1)),
                slice(max(0, bbox[1].start - 1), min(shape[1], bbox[1].stop + 1)))

class _isl:
    """Island class used by the island-detection algorithm.

    The island is represented as a number of 'runs', where each run 
    is a 3-tuple (row, begin, end).
    
    The tuples are sorted into 3 lists -- new, old, cur:
      new -- runs added during current iteration of the algorithm
      cur -- runs from the previous iteration
      old -- all other runs
    """
    def __init__(self, run):
        self.new = []
        self.cur = [run]
        self.old = []

    def __repr__(self):
        return "ISL"+str((self.new, self.cur, self.old))

    __str__ = __repr__

    def empty(self): 
        """Check whether island has no runs (i.e. was merged into other island)"""
        return (self.new, self.cur, self.old) == ([], [], [])

    def closed(self):
        """Check if island is closed -- no new runs were added in the latest iteration"""
        return self.new == []

    def runs(self):
        """Return all runs as a plain list (flatten island)"""
        return self.old + self.cur + self.new

    def add(self, run):
        """Add new run"""
        self.new.append(run)

    def merge(self, isl):
        """Merge other island into current"""
        self.new.extend(isl.new); isl.new = []
        self.cur.extend(isl.cur); isl.cur = []
        self.old.extend(isl.old); isl.old = []

    def endline(self):
        """Rotate arrays in the end of the iteration"""
        self.old.extend(self.cur)
        self.cur = self.new
        self.new = []
        return self

class _isl_list:
    """Island list class used in the island detection algorithm.

    Collects all islands into three groups:
      new -- new islands from the current iteration
      cur -- open islands from previous iteration
      old -- closed islands
    """
    def __init__(self):
        self.new = []
        self.cur = []
        self.old = []

    def __str__(self):
        return "ISL_LIST(%s,\n%s,\n%s)" % (
            str(self.new), str(self.cur), str(self.old))

    def flatten(self):
        """Flatten all islands and return them as a list"""
        res = []
        for i in (self.old + self.cur + self.new):
            res.append(i.runs())
        return res

    def add_line(self, runs, intersect = None):
        """Add a set of runs:
        Runs are added to 'cur' islands if they intersect with them
        or new islands are created.
        """
        if not intersect:
            intersect = _intersect8

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
                self.new.append(_isl(r))
        self.endline()

    def endline(self):
        """End of iteration:
        remove empy (merged) islands
        move closed islands out of 'cur' list
        move 'new' islands into 'cur'
        """
        for i in reversed(xrange(len(self.cur))):
            if self.cur[i].empty():
                del self.cur[i]
                continue
            if self.cur[i].closed():
                self.old.append(self.cur[i])
                del self.cur[i]
                continue
            self.cur[i].endline()

        self.cur.extend(self.new)
        self.new = []


def _intersect8(run, runs):
    """Check whether the run 'run' intersects with one of the
    runs from 'runs' according to 8-connectivity rules.
    Beware that it does not checks whether the runs are located
    on consequitive lines.
    """
    for t in runs:
        if min(run[2],t[2])-max(run[1],t[1]) >= 0 :
            return True
    return False

def _split(arr, idx):
    """Split array arr into the set of contiguous runs
    Returns the list of 3-tuples, where first element is
    idx, other two are the 'begin' and 'end' indices of the run.
    The 'end' index is an 'last + 1' index as is slicing notation.
    """
    ### first trivial cases -- none and all
    if not arr.any():
        return []
    if arr.all():
        return [(idx, 0, len(arr))]

    ### now start splitting
    res = []; i = 0; st = 200
    while i < len(arr):
        if not arr[i:i+st].any():
            i += st; continue
        for j in xrange(i, i+st):	# find first active pixel
            if arr[j]:
                i = j; break
        j = i + 1			# now look for the end of run
        for j in xrange(j, j+len(arr)):
            if j == len(arr) or not arr[j]:
                break
        res.append((idx, i, min(j, len(arr))))
        i = j

    return res

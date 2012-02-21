""" Tests various stuff to split an island properly. """

import functions as func
import scipy.ndimage as nd
import numpy as N
import matplotlib.pyplot as pl
#import pylab as pl

def test_split(isl):

  mask = N.array(isl.mask_active, int)
  connected, count = func.connect(mask)
  connectivity = nd.generate_binary_structure(2,2)

  ft3 = N.ones((3,3), int)
  open3 = nd.binary_opening(~isl.mask_active, ft3)
  open3 = check_1pixcontacts(open3)
  labels, n_subisl3 = nd.label(open3, connectivity)  # get label/rank image for open3. label = 0 for masked pixels
  labels3 = assign_leftovers(isl, open3, n_subisl3, labels)  # add the leftover pixels to some island

  ft5 = N.ones((5,5), int)
  open5 = nd.binary_opening(~isl.mask_active, ft5)
  open5 = check_1pixcontacts(open5)
  labels, n_subisl5 = nd.label(open5, connectivity)  # get label/rank image for open3. label = 0 for masked pixels
  labels5 = assign_leftovers(isl, open5, n_subisl5, labels)  # add the leftover pixels to some island

  pl.figure()
  pl.suptitle('Island '+str(isl.island_id))
  pl.subplot(4,2,1); pl.imshow(N.transpose(isl.image*~isl.mask_active), origin='lower', interpolation='nearest'); pl.title('Image')
  pl.subplot(4,2,2); pl.imshow(N.transpose(~mask), origin='lower', interpolation='nearest'); pl.title('mask')
  pl.subplot(4,2,3); pl.imshow(N.transpose(open3), origin='lower', interpolation='nearest'); pl.title('Open3 : '+str(n_subisl3))
  pl.subplot(4,2,4); pl.imshow(N.transpose(~mask-open3), origin='lower', interpolation='nearest'); pl.title('Leftout 3')
  pl.subplot(4,2,5); pl.imshow(N.transpose(labels3), origin='lower', interpolation='nearest'); pl.title('Labels 3 : '+str(n_subisl3))
  pl.subplot(4,2,6); pl.imshow(N.transpose(open5), origin='lower', interpolation='nearest'); pl.title('Open5 : '+str(n_subisl5))
  pl.subplot(4,2,7); pl.imshow(N.transpose(~mask-open5), origin='lower', interpolation='nearest'); pl.title('Leftout 5')
  pl.subplot(4,2,8); pl.imshow(N.transpose(labels5), origin='lower', interpolation='nearest'); pl.title('Labels 5 : '+str(n_subisl5))

  npix3 = [len(N.where(labels3==i)[0]) for i in range(1,n_subisl3+1)]
  npix5 = [len(N.where(labels5==i)[0]) for i in range(1,n_subisl5+1)]

  print 'Open 3 ',n_subisl3, N.array(npix3)/float(N.sum(npix3))
  print 'Open 5 ',n_subisl5, N.array(npix5)/float(N.sum(npix5))

def check_1pixcontacts(open):
    import scipy.ndimage as nd
    import numpy as N
    from copy import deepcopy as cp

    connectivity = nd.generate_binary_structure(2,2)
    ind = N.transpose(N.where(open[1:-1,1:-1] > 0)) + [1,1]   # exclude boundary to make it easier
    for pixel in ind:
      x, y = pixel
      grid = cp(open[x-1:x+2, y-1:y+2]); grid[1,1] = 0
      grid = N.where(grid == open[tuple(pixel)], 1, 0)
      ll, nn = nd.label(grid, connectivity)
      if nn > 1: 
        open[tuple(pixel)] = 0

    return open

def assign_leftovers(isl, open, nisl, labels):
    """ 
    Given isl and the image of the mask after opening (open) and the number of new independent islands n, connect up the
    left over pixels to the new islands if they connect to only one island and not more. Assign the remaining to an island.

    We need to assignthe leftout pixels of either of many sub islands. Define the border pixels with each sub island, and grow them
    into the leftover island, calculating median at every step. when median is a minimum, draw a border.
    
    """

    from copy import deepcopy as cp

    n, m = isl.image.shape
    leftout = ~isl.mask_active - open

    connectivity = nd.generate_binary_structure(2,2)
    mlabels, count = nd.label(leftout, connectivity)
    npix = [len(N.where(labels==b)[0]) for b in range(1,nisl+1)]

    pl.figure()
    pl.imshow(N.transpose(labels), origin='lower', interpolation='nearest')

    for i_subisl in range(count):
      c_list = []    # is list of all bordering pixels of the sub island
      ii = i_subisl+1
      coords = N.transpose(N.where(mlabels==ii))  # the coordinates of island i of left-out pixels
      for co in coords:
        co8 = [[x,y] for x in range(co[0]-1,co[0]+2) for y in range(co[1]-1,co[1]+2) if x >=0 and y >=0 and x <n and y<m]
        c_list.extend([tuple(cc) for cc in co8 if mlabels[tuple(cc)] == 0])
      c_list = list(set(c_list))     # to avoid duplicates
      vals = N.array([labels[c] for c in c_list])
      belongs = list(set(vals[N.nonzero(vals)]))
      if len(belongs) == 0: print 'something wrong in assign_leftovers'
      if len(belongs) == 1: 
        for cc in coords: 
          labels[tuple(cc)] = belongs[0]
      else:                             # get the border pixels of the islands
        nn = [npix[b-1] for b in belongs]
        addto = belongs[N.argmin(nn)]
        for cc in coords: 
          labels[tuple(cc)] = addto

    return labels






    



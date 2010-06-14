
import pylab as pl
import numpy as N
import functions as func

def test_spectralindex_chfit1(img, isl, data, rmask, x_ax, y_ax, p_ini, p_fit, nchan):

    p_mod = [p_fit[0], p_ini[1], p_ini[2], p_fit[3], p_fit[4], p_fit[5]]
    pl.figure()
    pl.suptitle('isl '+str(isl.island_id) + '  nchan '+str(nchan)+' SNRch0 '+str(p_ini[0]/isl.rms))
    pl.subplot(3,3,1)
    pl.imshow(N.transpose(data), interpolation='nearest', origin='lower'); 
    pl.colorbar(); pl.title('data')
    pl.subplot(3,3,2)
    pl.imshow(N.transpose(rmask), interpolation='nearest', origin='lower'); 
    pl.colorbar(); pl.title('rmask')
    pl.subplot(3,3,3)
    pl.imshow(N.transpose((data-img.ch0[isl.bbox])*~rmask), interpolation='nearest', origin='lower'); 
    pl.colorbar(); pl.title('data-ch0')
    pl.subplot(3,3,4)
    pl.imshow(N.transpose(data*~rmask), interpolation='nearest', origin='lower'); 
    pl.colorbar(); pl.title('masked data')
    pl.subplot(3,3,5)
    pl.imshow(N.transpose((data-func.gaus_2d(p_ini, x_ax, y_ax))*~rmask), interpolation='nearest', origin='lower'); 
    pl.colorbar(); pl.title('data - ch0 para')
    pl.subplot(3,3,6)
    pl.imshow(N.transpose((data-func.gaus_2d(p_fit, x_ax, y_ax))*~rmask), interpolation='nearest', origin='lower'); 
    pl.colorbar(); pl.title('data - fit para')
    pl.subplot(3,3,7)
    pl.imshow(N.transpose((func.gaus_2d(p_ini, x_ax, y_ax))*~rmask), interpolation='nearest', origin='lower'); 
    pl.colorbar(); pl.title('ch0 para')
    pl.subplot(3,3,8)
    pl.imshow(N.transpose((func.gaus_2d(p_fit, x_ax, y_ax))*~rmask), interpolation='nearest', origin='lower'); 
    pl.colorbar(); pl.title('fit para')
    pl.subplot(3,3,9)
    pl.imshow(N.transpose((data - func.gaus_2d(p_mod, x_ax, y_ax))*~rmask), interpolation='nearest', origin='lower'); 
    pl.colorbar(); pl.title('data - fit para w ini cen')


def test_spectralindex_chfit2(para, epara, nchan, p_ini, islid):
    para = N.array(para)
    epara = N.array(epara)
    pl.figure()
    pl.suptitle(str(islid))
    pl.subplot(2,3,1) 
    pl.errorbar(range(nchan), para[:,0], epara[:,0]); pl.title('flux');pl.plot(range(nchan), [p_ini[0]]*nchan)
    pl.subplot(2,3,2) 
    pl.errorbar(range(nchan), para[:,1], epara[:,1]); pl.title('cenx');pl.plot(range(nchan), [p_ini[1]]*nchan)
    pl.subplot(2,3,3) 
    pl.errorbar(range(nchan), para[:,2], epara[:,2]); pl.title('ceny');pl.plot(range(nchan), [p_ini[2]]*nchan)
    pl.subplot(2,3,4) 
    pl.errorbar(range(nchan), para[:,3], epara[:,3]); pl.title('bmaj');pl.plot(range(nchan), [p_ini[3]]*nchan)
    pl.subplot(2,3,5)
    pl.errorbar(range(nchan), para[:,4], epara[:,4]); pl.title('bmin');pl.plot(range(nchan), [p_ini[4]]*nchan)
    pl.subplot(2,3,6)
    pl.plot(range(nchan), para[:,0]*para[:,3]*para[:,4]); pl.title('total')


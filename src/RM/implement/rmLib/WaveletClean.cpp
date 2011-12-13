#include "wavelet.h"

/* ===========================================================================*/
/* =================== Class Wavelet cleaning  methods =======================*/
/* ===========================================================================*/

/* ======== constructor, automatically do wavelet transform ==================*/

//_______________________________________________________________________________
//                                                                   WaveletClean

WaveletClean::WaveletClean (vector<complex<double> > s,
			    vector<double> lam)
  : signal(s),
    RMSF(-10,15,1, -20, 150, 1),
    data(-10,15,1, -20, 150, 1),		// should be compatible
    WaveletSynthesis(-10,15,1, -20, 150, 1)
{
  WaveletSynthesis::lamsq = lam;
  res.reserve(data.bn);
  analys(s, data);
  wplus = data;
}

//_______________________________________________________________________________
//                                                                   WaveletClean

WaveletClean::WaveletClean (vector<complex<double> > s,
			    vector<double> lam,
			    double amin,
			    double amax,
			    double adelta, 
			    double bmin,
			    double bmax,
			    double bdelta)
  : signal(s),
    RMSF(amin,amax,adelta,bmin,bmax,bdelta),
    data(amin,amax,adelta,bmin,bmax,bdelta),		// should be compatible
    WaveletSynthesis(amin,amax,adelta,bmin,bmax,bdelta)
{
  /*	WaveletSynthesis::lamsq = lam;
	res.reserve(data.bn);
	for(unsigned int i=0; i<res.size(); i++) res[i] = 0;
	analys(s, data);
	wplus = data;*/
}

//_______________________________________________________________________________
//                                                                           init
	
void WaveletClean::init(vector<complex<double> > s, vector<double> lam, vector<double> fd, int numw)
{
  res.resize(fd.size());
  lamsq = lam;
  depths = fd;
  for(unsigned int i=0; i<res.size(); i++) res[i] = 0;
  data.useWavelet(numw);
  wminus.useWavelet(numw);
  wres.useWavelet(numw);
  RMSF.useWavelet(numw);
  analys(s, data);
  wplus = data;
}
/* ======== main intro point - calls "times" steps of cleaning ===============*/
void WaveletClean::perform(int times)
{
	unsigned int am, bm;
	getMaxPos(am, bm);
	wmaxinit = cabs(data.wc[am][bm]);
	for(int i=1; i<=times; i++)
		perform();
}
void WaveletClean::perform()
{
	complex<double> kf;
	vector<complex <double> > ft(lamsq.size());	// point signal (fourier)
	unsigned int i, k, am, bm;
//	doSymmetry (getMaxPos(wplus, am, bm), 20);
	getMaxPos(wplus, am, bm);
	wres = wplus;
    for(k = 0 ; k <ft.size() ; k++ )
	{
		ft[k] = exp(complex<double>(0.,2*depths[bm]*lamsq[k]));	// fourier image of point signal in b=bm
//		for(i = 0; i<t.size(); i++)
//			ft[k] += exp(-complex<double>(0.,1.)*depths[i]*lamsq[k]*2.)*t[i];
    }
	RMSF.ftransform(ft, lamsq);
	kf = wres.wc[am][bm] / RMSF.wc[am][bm];
	res[bm] = kf;
	for(i=0; i<wplus.an; i++)
		for(k=0; k<wplus.bn; k++)
			wplus.wc[i][k] = wres.wc[i][k] - RMSF.wc[i][k]*kf;
}

/* ============================= finds position of maximum ===================*/
unsigned int WaveletClean::getMaxPos(unsigned int &a, unsigned int &b)
{
	return getMaxPos(data, a, b);
}
unsigned int WaveletClean::getMaxPos(wavelet &w, unsigned int &a, unsigned int &b)
{
	unsigned mx=1, my=1;
	double max=0, abs;
	for(unsigned i=0; i<data.an; i++)
		for(unsigned k=0; k<w.bn; k++)
		{
			abs = cabs(w.wc[i][k]);
			if(abs > max) 
			{
				max = abs;
				mx = i;
				my = k;
			}
		}
	a = mx;
	b = my;
	return b;
}

/* ============= chi-square criterion (requires inverse transform!) ==================*/
double WaveletClean::chi2()
{
	return 0;  // not ready
}

/* ============= max sygnal/max residue criterion ====================================*/
double WaveletClean::ratio()
{
	unsigned int am, bm;
	getMaxPos(wplus, am, bm);
	return cabs(wplus.wc[am][bm])/wmaxinit;
}

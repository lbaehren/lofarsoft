#include "wavelet.h"
#define _USE_MATH_DEFINES       // for use of M_PI
#include <math.h>				// mathematics library

double cabs(complex<double> z)
{
	return sqrt(z.real()*z.real() + z.imag()*z.imag());
}
/* ===========================================================================*/
/* =================== Class Wavelet methods =================================*/
/* ===========================================================================*/

/* ====== simple constructor, needs initialisation of array to use ==========*/
/* ++++++ SHOULD NOT NORMALLY BE USED! ++++++++++++++++++++++++++++++++++++++*/
wavelet::wavelet()
{
	amin = -5;
	amax = 10;
	adelta = 1;
	an = (int)ceil((amax-amin)/adelta);
	bmin = 0;
	bmax = 0; // unknown length of sygnal yet
	bdelta = 0;
	bn = 1;
	useWavelet(0);
	log2=2;
	log2=log(log2);
// 	printf("used default constructor for wavelet...\n");
}

/*====== constructor with main parameters =================================*/
wavelet::wavelet(double a1, double a2, double ad, double b1, double b2, double bd)
{
	unsigned int i;
	log2=2;
	log2=log(log2);
	amin = a1;
	amax = a2;
	adelta = ad;
	an = (int)ceil((amax-amin)/adelta)+1;
	for(i=0; i<an; i++) as.push_back(exp(log2*(amin + i*adelta))); //logarithmic scales, base 2
	bmin = b1;
	bmax = b2;
	bdelta = bd;
	bn = (int)ceil((bmax-bmin)/bdelta)+1;
	for(i=0; i<bn; i++) bs.push_back(bmin + i*bdelta); // arithmetic shifts
	useWavelet(0);
	wc = (complex<double> **)calloc(an, sizeof(complex<double> *));
//	if(wc == NULL) {printf("\nAlloc_mem: insufficient memory!\n",-1,-1);exit(1);}

   for(unsigned i = 0; i < an; i++) {
         wc[i] = (complex<double> *)calloc(bn, sizeof(complex<double>));
//		 if(wc[i] == NULL) {printf("\nAlloc_mem: insufficient memory!\n",-1,-1);exit(1);
   }
// 	printf("used constructor wavelet (%lf, %lf, %lf, %lf, %lf, %lf)\n",a1, a2, ad, b1, b2, bd);
}

/* =================================== simple Haar wavelet ===============================*/
complex<double> wavelet::Haar(double x)
{
	if(x>1 || x<-1 || x==0) return 0;
	if(x>0) return 1;
	return -1;
}
complex<double> wavelet::Haar_f(double xi)
{
	if(fabs(xi)<1e-6) return 0.;
	return -( 2./M_PI*(1-cos(xi) ) )/xi;
}

/* =================================== Mexican Hat wavelet (real) ========================*/
complex<double> wavelet::MexicanHat(double x)
{
	x = 25*x*x; // sqaured to speed and multiplied by 25 to rescale into [-1,1]
	return exp(-x/2)*(1-x);
}
complex<double> wavelet::MexicanHat_f(double xi)
{
	xi = 25*xi*xi;
	return exp(-xi/2)*xi;
}

/* ===================================  Morlet wavelet ===================================*/
complex<double> wavelet::Morlet(double x)
{
	x = 5*x;
	return cos(M_PI*x*sqrt(2./log2))*exp(-x*x/2.);
}
complex<double> wavelet::Morlet_f(double xi)
{
	xi = 5*xi;
	return cosh(M_PI*xi*sqrt(2./log2))/exp((2*M_PI*M_PI + xi*xi*log2)/2/log2);
}

//	complex Morlet(complex x);		// complex wavelet - Morlet

/* ============= choose the number of using wavelet =======================================*/
double wavelet::useWavelet(int number)
{
/*	switch(number)
		{
		case 0: wvf = wavelet::Haar; break;
		case 1: wvf = &wavelet::MexicanHat; break;
		case 2: wvf = &wavelet::Morlet; break;
default: ;
		}*/
	wavelet::nw = number;
	Cpsi = 1;
// tabulation and interpolation needed
	return Cpsi;
}
/* ============ proxy function for all wavelets ========================================*/
complex<double> wavelet::wvf(double x)
{
	switch(wavelet::nw)
	{
	case 0: return wavelet::Haar(x);
	case 1: return wavelet::MexicanHat(x);
	case 2: return wavelet::Morlet(x);
	default: return 0;
	}
}
complex<double> wavelet::wvf_f(double x)
{
	switch(wavelet::nw)
	{
	case 0: return wavelet::Haar_f(x);
	case 1: return wavelet::MexicanHat_f(x);
	case 2: return wavelet::Morlet_f(x);
	default: return 0;
	}
}

/* ============= direct wavelet transform, result in the wc array ==========================*/
void wavelet::transform(vector<complex<double> > y)  // uniform grid
{
	vector<double> x(y.size());
	for(unsigned i=0; i<x.size(); i++) x[i] = i;
	wavelet::transform(y, x);
}

void wavelet::transform(vector<complex<double> > y, vector<double> x)  // any grid
{
	unsigned ai, bi, xi;
	complex<double> prev, next;
	for(ai=0; ai<an; ai++)
		for(bi=0; bi<bn; bi++)
		{
			wc[ai][bi] = 0;
			for(xi = 0; x[xi]<bs[bi]-2*as[ai] && xi<x.size()-1; xi++);   // dummy summation outside wavelet support
			prev = 0;
			for(; x[xi]<=2*as[ai]+bs[bi] && xi<x.size()-1; xi++)
			{
				next = y[xi]*conj(wvf( (x[xi]-bs[bi]) ) / as[ai] );
				wc[ai][bi] += (x[xi+1]-x[xi])*next;				// rectangular rule
//				wc[ai][bi] += (x[xi+1]-x[xi])*(prev + next)/2.;	// trapezoidal rule
				prev = next;
			}
			wc[ai][bi] /= fabs(as[ai]);
		}
}

void wavelet::ftransform(vector<complex<double> > y, vector<double> x)  // wavelet transorm of fourier image
{
	unsigned ai, bi, xi;
	complex<double> prev, next;
	for(ai=0; ai<an; ai++)
		for(bi=0; bi<bn; bi++)
		{
			wc[ai][bi] = 0;
			for(xi = 0; x[xi]<bs[bi]-2*as[ai] && xi<x.size()-1; xi++);   // dummy summation outside wavelet support
			prev = 0;
			for(xi=0; /*x[xi]<=200*as[ai]+bs[bi] &&*/ xi<x.size()-1; xi++)
			{
				next = y[xi]*conj(wvf_f( -2*as[ai]*x[xi] ))*exp(-2.*complex<double>(0.,1.)*bs[bi]*x[xi]);
//				wc[ai][bi] += (x[xi+1]-x[xi])*next;					// rectangular integration
				wc[ai][bi] += (x[xi+1]-x[xi])*(prev + next)/2.;		// trapezoidal intagration
				prev = next;
			}
			wc[ai][bi] /= M_PI;
		}
}

/* ============= inverse wavelet transform from the wc array ===============================*/
vector<complex<double> > wavelet::invTransform(vector<double> x)
{
	vector<complex<double> > res (x.size());
	unsigned ai, bi, xi;
	complex<double> tmp;
	tmp = 0;
	for(xi=0; xi<x.size(); xi++)
	{
		res[xi] = 0;
		for(ai=0;ai<an;ai++)
		{
			tmp = 0;
			for(bi=0;bi<bn;bi++)
				tmp += wvf( (x[xi]-bs[bi]) / as[ai] ) * wc[ai][bi];
			tmp *= bdelta*exp(log2*adelta)/as[ai];
			res[xi] += tmp;
		}
		res[xi] /= Cpsi;
	}
	return res;
}
/* ================= destructor: releases memory ===========================*/
wavelet::~wavelet()
{
if(an>0)
   for(unsigned i = 0; i < an; i++)
         free(wc[i]);

   if(wc) free(wc);
}

/* ===========================================================================*/
/* =================== Class Wavelet synthesis methods =======================*/
/* ===========================================================================*/

// Should not be used!
WaveletSynthesis::WaveletSynthesis()
{
}
/* ================= constructor with main parameters ===========================*/
WaveletSynthesis::WaveletSynthesis(double amin, double amax, double adelta, 
								  double bmin, double bmax, double bdelta) :
			wplus(amin, amax, adelta, bmin, bmax, bdelta),
			wminus(amin, amax, adelta, bmin, bmax, bdelta),
			wres(amin, amax, adelta, bmin, bmax, bdelta),
			data(0)
{
}
/* ================= reads data from FITS file (or call another utility) =====*/
void WaveletSynthesis::dataLoad(string fname)  // yet dummy
{
// should have dataReady==true and freqs.size()>0
	dataReady = 0;
	for(unsigned i=0; i<freqs.size(); i++)
		lamsq[i] = 9E16/(freqs[i]*freqs[i]);
}

/* ================= extract 1 line of sight =================================*/
vector<complex<double> > WaveletSynthesis::getLine(int x, int y)
{
	vector<complex<double> > tmp;
	if(!dataReady) return tmp;
//	vector<double> lams(freqs.size());
	for(unsigned i=0; i<freqs.size(); i++)
	{
		tmp.push_back(data[x][y][i]);
		lamsq[i] = 9E16/(freqs[i]*freqs[i]);
	}
	return tmp;
}

/* ================= apply symmetry argument: wplus->wminus->wres===============*/
void WaveletSynthesis::doSymmetry(int b0, int size)
{
	int i, k, kr;
	for(i=0; i<wplus.an; i++)
		for(k=0; k<wplus.bn; k++)
		{
			kr = 2*b0 - k;
			if(kr<1 || kr>wplus.bn || abs(kr-k)>size) wminus.wc[i][k] = 0;
				else wminus.wc[i][k] = wplus.wc[i][kr];
			wres.wc[i][k] = wplus.wc[i][k] + wminus.wc[i][k];
		}
}

/* ================= wavelet analysis stage ====================================*/
void WaveletSynthesis::analys(vector<complex<double> > line, wavelet &wres)
{
//	wres = new wavelet(-5, 10, 1, lamsq[0], lamsq[lamsq.size()-1], fabs(lamsq[lamsq.size()-1]-lamsq[0]) / lamsq.size() );
	wres.ftransform(line, lamsq);
}

/* ================= wavelet synthesis stage ====================================*/
vector<complex<double> > WaveletSynthesis::synth(wavelet &w, vector<double> fs)
{
	return w.invTransform(fs);
}
/* ================= destructor: releases memory ===========================*/
WaveletSynthesis::~WaveletSynthesis()
{

   if(data) free(data);
//    wplus.~wavelet();
//    wminus.~wavelet();
//    wres.~wavelet();
}

/* ===========================================================================*/
/* =================== Class Wavelet cleaning  methods =======================*/
/* ===========================================================================*/

/* ======== constructor, automatically do wavelet transform ==================*/
WaveletClean::WaveletClean(vector<complex<double> > s, vector<double> lam): signal(s),
	RMSF(-10,15,1, -20, 150, 1),
	data(-10,15,1, -20, 150, 1),		// should be compatible
	WaveletSynthesis(-10,15,1, -20, 150, 1)
{
	WaveletSynthesis::lamsq = lam;
	res.reserve(data.bn);
	analys(s, data);
	wplus = data;
}
WaveletClean::WaveletClean(vector<complex<double> > s, vector<double> lam,
					double amin, double amax, double adelta, 
					double bmin, double bmax, double bdelta): signal(s),
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

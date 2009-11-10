#ifndef HFANALYSIS_H
#define HFANALYSIS_H

#define VecTit typename vector<T>::iterator
#define VecIit typename vector<HInteger>::iterator
#define VecNit typename vector<HNumber>::iterator
#define VecCit typename vector<HComplex>::iterator

//! function value of a Gaussian distribution
inline HNumber funcGaussian(HNumber x,
			    HNumber sigma=1.0,
			    HNumber mu=0.0);

//! Calculate the running average of an array of Numbers.
template <class T> 
void hRunningAverageT (const vector<T>::iterator idata_start,
		       const vector<T>::iterator idata_end,
		       const vector<T>::iterator odata_start,
		       const vector<HNumber>::iterator weights_start,
		       const vector<HNumber>::iterator weights_end);

//! Calculate the running average of an array of Numbers.
template <class T>
void hRunningAverageN (T* start_data_in,
		       T* start_data_out,
		       address datalen,
		       HNumber * start_weights,
		       address weightslen);


enum hWEIGHTS {WEIGHTS_FLAT,WEIGHTS_LINEAR,WEIGHTS_GAUSSIAN};

vector<HNumber> hWeights(address wlen, hWEIGHTS wtype);
vector<HNumber> hFlatWeights(address wlen);
vector<HNumber> hLinearWeights(address wlen);
vector<HNumber> hGaussianWeights(address wlen);

template <class T>
void hRunningAverageVec(vector<T> &vec_in,vector<T> &vec_out, address len=5, hWEIGHTS wtype=WEIGHTS_LINEAR);
void hRunningAverageVec(vector<HString> &vec_in,vector<HString> &vec_out, address wlen, hWEIGHTS wtype);
void hRunningAverageVec(vector<HPointer> &vec_in,vector<HPointer> &vec_out, address wlen, hWEIGHTS wtype);


extern void (*hRunningAverageVec_N)(vector<HNumber > &vec_in,vector<HNumber > &vec_out, address len, hWEIGHTS wtype);
extern void (*hRunningAverageVec_I)(vector<HInteger> &vec_in,vector<HInteger> &vec_out, address len, hWEIGHTS wtype);
extern void (*hRunningAverageVec_C)(vector<HComplex> &vec_in,vector<HComplex> &vec_out, address len, hWEIGHTS wtype);

/*
extern void (*hRunningAverageVec_N_3)(vector<HNumber > &vec_in,vector<HNumber > &vec_out, address len);
extern void (*hRunningAverageVec_I_3)(vector<HInteger> &vec_in,vector<HInteger> &vec_out, address len);
extern void (*hRunningAverageVec_C_3)(vector<HComplex> &vec_in,vector<HComplex> &vec_out, address len);
extern void (*hRunningAverageVec_N_2)(vector<HNumber > &vec_in,vector<HNumber > &vec_out);
extern void (*hRunningAverageVec_I_2)(vector<HInteger> &vec_in,vector<HInteger> &vec_out);
extern void (*hRunningAverageVec_C_2)(vector<HComplex> &vec_in,vector<HComplex> &vec_out);
*/

#endif

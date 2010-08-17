#ifndef _OOC_MATHS_H
#define _OOC_MATHS_H

namespace oocuda {
	
	typedef unsigned long long ullong;
	typedef unsigned int uint;

	// Very simple static array wrapper for passing to CUDA kernels
	// TODO: This is no good for CUDA! It doesn't appear to know how to unroll it, and uses local mem!
	template<typename T, size_t T_ndims>
	class Array
	{
		T m_val[T_ndims];
	public:
		Array() {};
		Array(T val)
			{
				#pragma unroll 3
				for( size_t d=0; d<T_ndims; ++d ) {
					m_val[d] = val;
				}
			}
		
		// Construction from non-matching contained type
		template<typename T_other>
		Array(const Array<T_other, T_ndims>& other)
			{
				#pragma unroll 3
				for( size_t d=0; d<T_ndims; ++d ) {
					m_val[d] = other[d];
				}
			}
		// Conversion to non-matching contained type
		template<typename T_other>
		inline operator Array<T_other, T_ndims> ()
			{
				Array<T_other, T_ndims> other(*this);
				return other;
			}
		
		// TODO: Check that this works in CUDA!
		inline const T& operator [] (const size_t i) const { return m_val[i]; }
		inline T& operator [] (const size_t i) { return m_val[i]; }
		
		operator uint4 ()
			{
				uint4 val;
				val.x = m_val[0];
				if( T_ndims > 1 ) val.y = m_val[1];
				if( T_ndims > 2 ) val.z = m_val[2];
				if( T_ndims > 3 ) val.w = m_val[3];
				return val;
			}
	};

	// Very simple mathematical vector type
	template<typename T, size_t T_ndims>
	class Vector
	{
		T m_val[T_ndims];
	public:
		Vector() {};
		Vector(T val)
			{
				for( size_t d=0; d<T_ndims; ++d ) {
					m_val[d] = val;
				}
			}
		// TODO: Check that this works in CUDA!
		inline const T& operator [] (size_t i) const { return m_val[i]; }
		inline T& operator [] (size_t i) { return m_val[i]; }

		// TODO: Add other operators and methods!
	};
	
	// Min and max functions
	template<typename T>
	inline T min(T a, T b) { return a<b?a:b; }
	template<typename T>
	inline T max(T a, T b) { return a>b?a:b; }
	
    //Integer division rounded up/down
	inline int divCeil(int n, int d)        { return (n-1) / d + 1; }
	inline int divFloor(int n, int d)       { return n / d; }

    // Returns the floor form of binary logarithm for a 32 or 64-bit integer or -1 if n is 0
    // TODO: Test this more thoroughly!
	template<typename T>
	inline T log2(T n)
	{
		typedef unsigned long long ullong;
		switch( sizeof(T) ) {
			case 1: {
				int pos = 0;
				if (n >= 1<< 4) { n >>=  4; pos +=  4; }
				if (n >= 1<< 2) { n >>=  2; pos +=  2; }
				if (n >= 1<< 1) {           pos +=  1; }
				return ((n == 0) ? (-1) : pos);
			}
			case 2: {
				int pos = 0;
				if (n >= 1<< 8) { n >>=  8; pos +=  8; }
				if (n >= 1<< 4) { n >>=  4; pos +=  4; }
				if (n >= 1<< 2) { n >>=  2; pos +=  2; }
				if (n >= 1<< 1) {           pos +=  1; }
				return ((n == 0) ? (-1) : pos);
			}
			case 4: {
				int pos = 0;
				if (n >= 1<<16) { n >>= 16; pos += 16; }
				if (n >= 1<< 8) { n >>=  8; pos +=  8; }
				if (n >= 1<< 4) { n >>=  4; pos +=  4; }
				if (n >= 1<< 2) { n >>=  2; pos +=  2; }
				if (n >= 1<< 1) {           pos +=  1; }
				return ((n == 0) ? (-1) : pos);
			}
			case 8: {
				int pos = 0;
				ullong _n = (ullong)n;
				if (_n >= (ullong)1<<32) { _n >>= 32; pos += 32; }
				if (_n >= (ullong)1<<16) { _n >>= 16; pos += 16; }
				if (_n >= (ullong)1<< 8) { _n >>=  8; pos +=  8; }
				if (_n >= (ullong)1<< 4) { _n >>=  4; pos +=  4; }
				if (_n >= (ullong)1<< 2) { _n >>=  2; pos +=  2; }
				if (_n >= (ullong)1<< 1) {            pos +=  1; }
				return ((_n == (ullong)0) ? ((ullong)-1) : (ullong)pos);
			}
		}
		// Shouldn't be reached
		return 0;
	}

	template<typename T>
	inline bool isPow2(T x)
	{
		return (0==(x&(x-1)));
	}

	template<class T>
	inline T nextPow2(T k)
	{
		if( 0 == k ) {
			return 1;
		}
		--k;
		for (unsigned int i=1; i<sizeof(T)*8; i<<=1) {
			//k = k | k >> i;
			k |= k >> i;
		}
		return k+1;
	}

	// TODO: This function is not suitable for the GPU as it uses long double!
	__host__
	inline unsigned long long choose(unsigned n, unsigned k)
	{
		if (k > n)
			return 0;

		if (k > n/2)
			k = n-k; // Take advantage of symmetry

		long double accum = 1;
		for (unsigned i = 1; i <= k; i++)
			accum = accum * (n-k+i) / i;

		return (unsigned long long)(accum + 0.5); // avoid rounding error
	}

	template<typename Tx, typename Tpow>
	inline Tx int_pow(Tx x, const Tpow pow)
	{
		Tx a = 1;
		for( int i=0; i<pow; ++i )
			a *= x;
		return a;
	}

	template<class T>
	T sum(T *data, size_t size)
	{
		T sum = data[0];
		T c = (T)0.0;
		for (size_t i = 1; i < size; i++)
		{
			T y = data[i] - c;  
			T t = sum + y;      
			c = (t - sum) - y;  
			sum = t;            
		}
		return sum;
	}

	template<typename T>
	bool solveUpperDiag(int n, const T** U, T* x, const T* y)
	{
		// Solves "Ux = y" for x
		x[n-1] = y[n-1] / U[n-1][n-1];
		for( int m=n-2; m>=0; --m ) {
			T Ux = 0;
			for( int i=n-1; i>m; --i )
				Ux += U[m][i]*x[i];
			x[m] = (y[m] - Ux)  / U[m][m];
		}
		return true;
	}
	template<typename T>
	bool solveLowerDiag(int n, const T** L, T* x, const T* y)
	{
		// Solves "Lx = y" for x
		x[0] = y[0] / L[0][0];
		for( int m=1; m<n; ++m ) {
			T Lx = 0;
			for( int i=0; i<m; ++i )
				Lx += L[m][i]*x[i];
			x[m] = (y[m] - Lx)  / L[m][m];
		}
		return true;
	}

	// Factorial look-up table (0!-20!)
	static const ullong factorial_table[21] = {
		1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800,
		39916800, 479001600, 6227020800, 87178291200,
		1307674368000, 20922789888000, 355687428096000,
		6402373705728000, 121645100408832000, 2432902008176640000
	};

	template<typename T>
	inline T evalHermitePoly(size_t n, T x)
	{
		T h0 = (T)1.0;
		if( n == 0 )
			return h0;
		T h1 = 2*x;
		for( size_t i=2; i<=n; ++i ) {
			T h = 2*(x * h1 - (i-1) * h0);
			h0 = h1;
			h1 = h;
		}
		return h1;
	}
	template<typename T>
	inline float evalProbabilistsHermitePoly(size_t n, T x)
	{
		T h0 = 1.0;
		if( n == 0 )
			return h0;
		T h1 = x;
		for( size_t i=2; i<=n; ++i ) {
			T h = x * h1 - (i-1) * h0;
			h0 = h1;
			h1 = h;
		}
		return h1;
	}

}

#endif //_OOC_MATHS_H

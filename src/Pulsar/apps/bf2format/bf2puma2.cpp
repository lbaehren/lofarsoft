/*
 * bf2puma2wc.cpp
 *
 *  Created on: Dec 6, 2011
 *      Author: anoutsos
 */

#include<iostream>
#include<fstream>
#include <sstream>
#include<cstdlib>
#include<cmath>
#include<iomanip>
#include<vector>
#include<algorithm>
#include<map>
#include<complex>
#include<cstring>
#include<string>
#include <iterator>
// 2011 May 24  James M Anderson  --MPIfR  code modifications for speedup
// 2011 May 25  JMA  --convert more IO to C
// 2011 May 27  JMA  --restrict to common scaling for both real and imag parts
//                     and both X and Y polarizations (or R and L)
// 2011 Jun 03  JMA  --use same min/max range for all real/imaginary values,
//                     for both X and Y simulateously, but different for
//                     different channels
// 2011 Jun 03  JMA  --scale to a certain inclusion percentage for all channels
//                     for a specific time block.  Scale so that the outer
//                     fraction P is outside the range of int8_t and is written
//                     as flag value F (normally -128).
#include <stdint.h>

/* restrict
 This is a really useful modifier, but it is not supported by
 all compilers.  Furthermore, the different ways to specify it
 (double * restrict dp0, double dp1[restrict]) are not available
 in the same release of a compiler.  If you are still using an old
 compiler, your performace is going to suck anyway, so this code
 will only give you restrict when it is fully available.
 */
#ifdef __GNUC__
#  ifdef restrict
/*   Someone else has already defined it.  Hope they got it right. */
#  elif !defined(__GNUG__) && (__STDC_VERSION__ >= 199901L)
/*   Restrict already available */
#  elif !defined(__GNUG__) && (__GNUC__ > 2) || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#    define restrict __restrict
#  elif (__GNUC__ > 3) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#    define restrict __restrict
#  else
#    define restrict
#  endif
#else
#  ifndef restrict
#    define restrict
#  endif
#endif

#define MAXLONG 2147483647
#define NEED_TO_BYTESWAP 1
#define MAX_NFILES 10000

uint32_t CHANNELS = 16;
uint32_t SUBBANDS = 32;
uint32_t SAMPLES = 12208;
uint32_t NPOL = 2;
float CLOCKRES = 0.1953125;

int verb = 0; // generic verbosity flag
int realv = 0; // float-writer flag
const int_fast32_t INPUT_DATA_BLOCK_HEADER_SIZE = 512;


using namespace std;

streamsize getSize(const char*);
inline float FloatRead(int32_t a)
{
	union
	{
		float f;
		int32_t i;
	} dat;
#if(NEED_TO_BYTESWAP)
#  ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
	a = __builtin_bswap32(a);
#    else
	uint32_t u(a);
	a = int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#    endif
#  else
	uint32_t u(a);
	a = int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#  endif
#else
	/* do nothing */
#endif
	dat.i = a;
	return dat.f;
}
inline float int32_t_bits_to_float(int32_t a)
{
	union
	{
		float f;
		int32_t i;
	} dat;
	dat.i = a;
	return dat.f;
}
inline float uint32_t_bits_to_float(uint32_t a)
{
	union
	{
		float f;
		uint32_t i;
	} dat;
	dat.i = a;
	return dat.f;
}
inline int32_t byteswap_32_bit_int(int32_t &a)
{
#if(NEED_TO_BYTESWAP)
#  ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
	return __builtin_bswap32(a);
#    else
	uint32_t u(a);
	return int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#    endif
#  else
	uint32_t u(a);
	return int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#  endif
#else
	/* do nothing */
#endif
	return a;
}
inline uint32_t byteswap_32_bit_int(uint32_t &u)
{
#if(NEED_TO_BYTESWAP)
#  ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
	return uint32_t(__builtin_bswap32(int32_t(u)));
#    else
	return uint32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#    endif
#  else
	return uint32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#  endif
#else
	/* do nothing */
#endif
	return u;
}
void byteswap_32_bit_float_array(const size_t NUM, float* const restrict fp)
{
#if(NEED_TO_BYTESWAP)
	int32_t* const restrict ip(reinterpret_cast<int32_t* const restrict>(fp));
	for(size_t s=0; s < NUM; s++)
	{
#  ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
		ip[s] = __builtin_bswap32(ip[s]);
#    else
		uint32_t u(ip[s]);
		ip[s] = int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#    endif
#  else
		uint32_t u(ip[s]);
		ip[s] = int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#  endif
	}
#else
	/* do nothing */
#endif
	return;
}
float byteswap_32_bit_float_array_and_find_max(const size_t NUM,
		float* const restrict fp);

// histogram sutff
const int_fast32_t HIST_SIZE = 2048;
inline float histogram_center(void)
{
	static const float c = HIST_SIZE / 2 - 0.5;
	return c;
}
void flush_histogram(void);
void calculate_hist_constants(float min, float max);
void add_buffer_hist(const size_t NUM, const float* const restrict fp);
void find_hist_fraction_point_top(const float fraction,
		const int_fast64_t count_offset, int_fast32_t* index,
		int_fast64_t* count);
void find_hist_fraction_point_bot(const float fraction,
		const int_fast64_t count_offset, int_fast32_t* index,
		int_fast64_t* count);
float get_float_for_top_index_pos(const int_fast32_t index);
float get_float_for_bot_index_pos(const int_fast32_t index);

//complex<float> convert_complex(complex<float>);

struct samp_t
{
	int8_t Xr;
	int8_t Xi;
	int8_t Yr;
	int8_t Yi;
};

class writer
{
public:
	writer(const char* &base, char* &hdr, char* &pset, uint32_t &part) :
			sample_block_offset(0xFFFFFFFFU), blocksamples(0)
	{
		passFilenames(base, hdr, pset);
		passPart(part);
		readParset();
		filesInit();
	}
	~writer();
	void filesInit();
	void filesClose();
	void writePuMa2Block(const float global_minmax,
			const float * const restrict Xsamp,
			const float * const restrict Ysamp);
	void readParset();

	void passFilenames(const char* &, char* &, char* &);
	void passSub(uint32_t &);
	void passChan(uint32_t &);
	void passPart(uint32_t &);

	uint32_t NCHANNELS;
	uint32_t SUBBSTART;
	uint32_t SUBBEND;
	uint32_t NSUBBANDS;
	uint32_t PNSUB;
	uint32_t PNUM;

	int SUBBLIST[248];

	string DATE;
	string ANTENNA;

private:
	FILE* datafile[MAX_NFILES];
	FILE* realvolt[MAX_NFILES];
	void writeHeader(unsigned int &, unsigned int &);
	const char* _basename;
	char* _hdrFilename;
	char* _psetFilename;
	uint32_t _the_chan;
	uint32_t _the_sub;
	uint32_t _the_part;
	string getKeyVal(string &);
	double mjdcalc();
	size_t sample_block_offset;
	samp_t* restrict blocksamples;
};
writer::~writer()
{
	free(blocksamples);
}

int main(int argc, char** argv)
{

	if (argc == 1)
	{
		cout << endl << endl
				<< "Usage: <executable> -f <input file> -h <header file> "
				<< "-p <parset file>  [OPTIONS]" << endl << endl << endl;
		cout
				<< "OPTIONS:"
				<< endl
				<< endl
				<< " -v                       Verbose"
				<< endl
				<< endl
				<< " -t                       Write out ascii files (.rv) containing the complex values"
				<< endl
				<< endl
				<< " -b <nblocks>             Only read the first <nblocks> blocks"
				<< endl
				<< endl
				<< " -hist_cutoff <fraction>  Clip <fraction> off the edges of the samples histogram"
				<< endl
				<< "     >> note: eliminates spiky RFI -- but may also clip bright pulsar signals!"
				<< endl
				<< endl
				<< " -all_times               Normalise data based on the entire data set "
				<< endl
				<< "     >> note: if not used, the scaling is updated after every data block "
				<< endl << endl << endl << endl;
		return 0;
	}

	char* rawfname = 0;
	const char* basename = 0;
	char* hdrFilename = 0;
	char* psetFilename = 0;
	int scale_all_time_blocks_together = 0;
	float cutoff_fraction = 0.0f;

	// by default, read all blocks
	long block_limit = MAXLONG; // max long

	for (int ia = 0; ia < argc; ia++)
	{
		if (string(argv[ia]) == "-f")
		{
			if (ia + 1 < argc)
			{
				rawfname = argv[ia + 1];
			}
			else
			{
				cerr << "no argument following '-f'" << endl;
				exit(2);
			}
			continue;
		}
		else if (string(argv[ia]) == "-h")
		{
			if (ia + 1 < argc)
			{
				hdrFilename = argv[ia + 1];
			}
			else
			{
				cerr << "no argument following '-h'" << endl;
				exit(2);
			}
			continue;
		}
		else if (string(argv[ia]) == "-p")
		{
			if (ia + 1 < argc)
			{
				psetFilename = argv[ia + 1];
			}
			else
			{
				cerr << "no argument following '-p'" << endl;
				exit(2);
			}
			continue;
		}
		else if (string(argv[ia]) == "-v")
		{
			// Turn on verbosity
			verb = 1;
			continue;
		}
		else if (string(argv[ia]) == "-t")
		{
			// Write float complex samples
			realv = 1;
			continue;
		}
		else if (string(argv[ia]) == "-b")
		{
			// Number of blocks to read
			if (isdigit(argv[ia + 1][0]))
				block_limit = atoi(argv[ia + 1]);

			if (block_limit < 0 || block_limit > MAXLONG)
			{
				cerr << "Warning: invalid block limit, setting to all blocks"
						<< endl;
				block_limit = MAXLONG;
			}
			continue;
		}
		else if (string(argv[ia]) == "-all_times")
		{
			// Write float complex samples
			scale_all_time_blocks_together = 1;
			continue;
		}
		else if (string(argv[ia]) == "-hist_cutoff")
		{
			// Write float complex samples
			if (ia + 1 < argc)
			{
				cutoff_fraction = atof(argv[ia + 1]);
				if (cutoff_fraction < 0.0f)
				{
					cerr << "Warning: cutoff less than 0, setting to 0.0"
							<< endl;
					cutoff_fraction = 0.0f;
				}
				else if (cutoff_fraction >= 0.4999f)
				{
					cerr
							<< "Warning: cutoff greater than 0.4999, setting to 0.4999"
							<< endl;
					cutoff_fraction = 0.4999f;
				}
			}
			else
			{
				cerr << "no argument following '-hist_cutoff'" << endl;
				exit(2);
			}
			continue;
		}
	}

	if (verb)
	{
		cout << "input = " <<  rawfname << endl;
		cout << "header = " << hdrFilename << endl;
		cout << "parset = " << psetFilename << endl;
	}
        //L44652_SAP000_B000_S1_P001_bf.raw
        string prefix = string(rawfname).substr(0,18);
        basename = prefix.c_str();
	string suffix = string(rawfname).substr(21);
	

	string filenameX0_str = prefix + "_S0" + suffix;
	string filenameX1_str = prefix + "_S1" + suffix;
	string filenameY0_str = prefix + "_S2" + suffix;
	string filenameY1_str = prefix + "_S3" + suffix;

	const char* filenameX0 = filenameX0_str.c_str();
	const char* filenameX1 = filenameX1_str.c_str();
	const char* filenameY0 = filenameY0_str.c_str();
	const char* filenameY1 = filenameY1_str.c_str();

	FILE * pfileX0;
	FILE * pfileX1;
	FILE * pfileY0;
	FILE * pfileY1;

	pfileX0 = fopen(filenameX0, "rb");
	if (pfileX0 == NULL)
	{
		fputs("File error", stderr);
		exit(1);
	}
	pfileX1 = fopen(filenameX1, "rb");
	if (pfileX1 == NULL)
	{
		fputs("File error", stderr);
		exit(1);
	}

	pfileY0 = fopen(filenameY0, "rb");
	if (pfileY0 == NULL)
	{
		fputs("File error", stderr);
		exit(1);
	}
	pfileY1 = fopen(filenameY1, "rb");
	if (pfileY1 == NULL)
	{
		fputs("File error", stderr);
		exit(1);
	}

	uint32_t partno = atoi(string(rawfname).substr(23,3).c_str());

	writer puma2data(basename, hdrFilename, psetFilename, partno);


	if (verb)
		cout << "\n\nUsing Channels: " << CHANNELS << "\t and Subbands: "
				<< SUBBANDS << "\t and Samples: " << SAMPLES << endl << endl;
		cout << "\n\nFound Frequency Splits: " << puma2data.PNUM << "\t and Subbands per Part: "
				<< puma2data.PNSUB  <<"\t Current Split: " << partno <<endl << endl;

	size_t Jnumber_floats = CHANNELS * SUBBANDS * SAMPLES;
	size_t Jnumber_floats_read = CHANNELS * SUBBANDS * SAMPLES;
	size_t Jnumber_char = sizeof(float) * Jnumber_floats_read;

	// One-dimensional arrays for temporary storage
	float * restrict XJV;
	XJV = reinterpret_cast<float*restrict>(malloc(Jnumber_char));
	float * restrict YJV;
	YJV = reinterpret_cast<float*restrict>(malloc(Jnumber_char));

	if (XJV == 0 || YJV == 0)
	{
		fputs("Memory fail", stderr);
		exit(1);
	}

	if (verb)
	{
		cout << "SUBBANDS = " << puma2data.NSUBBANDS << endl;
		cout << "CHANNELS = " << puma2data.NCHANNELS << endl;
	}

	if (scale_all_time_blocks_together != 0)
	{
		if (verb)
			cout << "Reading through all blocks to find min/max" << endl;

		uint32_t iblock = 0;
		float global_minmax = 0.0f;
		int top_bottom_flag = -1;
		int_fast64_t top_count = 0;
		int_fast64_t bottom_count = 0;
		int_fast32_t top_index = 0;
		int_fast32_t bottom_index = 0;
		int_fast32_t histogram_loop = 0;
		float top;
		float bottom;

		long i0=0;
		unsigned zrx,zix,zry,ziy;

		while (1)
		{
			size_t num;
			float minmax;
			if (verb)
				cout << "Reading block: " << iblock << endl;

			zrx=0;
			zix=0;
			zry=0;
			ziy=0;

			if (iblock == block_limit)
				goto end_of_minmax_loop;

			// Read Re(X) data:

			num = fread(XJV, Jnumber_char, 1, pfileX0);
			if (num != 1)
				goto end_of_minmax_loop;

			minmax = byteswap_32_bit_float_array_and_find_max(Jnumber_floats,
					XJV);

			if(fabs(minmax)<1e-10) zrx=1;

			if (minmax > global_minmax)
				global_minmax = minmax;

			// Read Im(X) data:

			num = fread(XJV, Jnumber_char, 1, pfileX1);
			if (num != 1)
				goto end_of_minmax_loop;

			minmax = byteswap_32_bit_float_array_and_find_max(Jnumber_floats,
					XJV);

			if(fabs(minmax)<1e-10) zix=1;

			if (minmax > global_minmax)
				global_minmax = minmax;

			// Read Y data:

			num = fread(YJV, Jnumber_char, 1, pfileY0);
			if (num != 1)
				goto end_of_minmax_loop;

			minmax = byteswap_32_bit_float_array_and_find_max(Jnumber_floats,
					YJV);

			if(fabs(minmax)<1e-10) zry=1;

			if (minmax > global_minmax)
				global_minmax = minmax;

			// Read Y data:

			num = fread(YJV, Jnumber_char, 1, pfileY1);
			if (num != 1)
				goto end_of_minmax_loop;

			minmax = byteswap_32_bit_float_array_and_find_max(Jnumber_floats,
					YJV);

			if(fabs(minmax)<1e-10) ziy=1;

			if (minmax > global_minmax)
				global_minmax = minmax;

			if (verb)
				cout << setw(10) << "IBLOCK = " << setw(10) << iblock
						<< setw(10) << endl;

			if(zrx==1 && zix==1 && zry==1 && ziy==1) i0++;

			iblock++;
		} // End of while(1) loop
		end_of_minmax_loop: if (verb)
			cout << " *** Read " << iblock << " blocks (of which "<< i0 <<" were empty)." << endl;
		
		if (verb)
			cout << "Global minmax is " << global_minmax << endl;
		if (global_minmax <= 0.0f)
		{
			global_minmax = 1.0f;
			cerr << "block has only zeros!" << endl;
			goto start_data_output;
		}
		if (cutoff_fraction <= 0.0f)
		{
			goto start_data_output;
		}

		top = fabsf(global_minmax);
		bottom = -fabsf(global_minmax);
		while (histogram_loop < 20)
		{
			// now go back and build the histogram area
			rewind(pfileX0);
			rewind(pfileX1);
			rewind(pfileY0);
			rewind(pfileY1);
			iblock = 0;

			if (verb)
				cout << "starting histogram loop " << histogram_loop << endl;
			flush_histogram();
			calculate_hist_constants(bottom, top);
			while (1)
			{
				size_t num;
				if (verb)
					cout << "Reading block: " << iblock << endl;

				if (iblock == block_limit)
					goto end_of_histogram_N;

				// Read Re(X) data:
				num = fread(XJV, Jnumber_char, 1, pfileX0);
				if (num != 1)
					goto end_of_histogram_N;
#if(NEED_TO_BYTESWAP)
				byteswap_32_bit_float_array(Jnumber_floats, XJV);
#endif

				// Read Im(X) data:
				num = fread(XJV, Jnumber_char, 1, pfileX1);
				if (num != 1)
					goto end_of_histogram_N;
#if(NEED_TO_BYTESWAP)
				byteswap_32_bit_float_array(Jnumber_floats, XJV);
#endif

				// Read Re(Y) data:
				num = fread(YJV, Jnumber_char, 1, pfileY0);
				if (num != 1)
					goto end_of_histogram_N;
#if(NEED_TO_BYTESWAP)
				byteswap_32_bit_float_array(Jnumber_floats, YJV);
#endif

				// Read Im(Y) data:
				num = fread(YJV, Jnumber_char, 1, pfileY1);
				if (num != 1)
					goto end_of_histogram_N;
#if(NEED_TO_BYTESWAP)
				byteswap_32_bit_float_array(Jnumber_floats, YJV);
#endif

				if (verb)
					cout << setw(10) << "IBLOCK = " << setw(10) << iblock
							<< setw(10) << endl;

				add_buffer_hist(Jnumber_floats, XJV);
				add_buffer_hist(Jnumber_floats, YJV);
				iblock++;
			} // End of while(1) loop
			end_of_histogram_N: if (verb)
				cout << " *** Read " << iblock << " blocks." << endl;

			if (verb)
				cout << "Evaluating results for histogram " << histogram_loop
						<< endl;
			if (top_bottom_flag == 0)
			{
				// top already decided
				find_hist_fraction_point_top(cutoff_fraction, top_count,
						&top_index, &top_count);
				if (verb)
					cout << "top index " << top_index << endl;
				if (verb)
					cout << "top count " << top_count << endl;
				top = get_float_for_top_index_pos(top_index);
				bottom = get_float_for_bot_index_pos(top_index);
				global_minmax = fabsf(top);
				if (top_index > 10)
				{
					break;
				}
			}
			else if (top_bottom_flag == 1)
			{
				// bottom already decided
				find_hist_fraction_point_bot(cutoff_fraction, bottom_count,
						&bottom_index, &bottom_count);
				if (verb)
					cout << "bottom index " << bottom_index << endl;
				if (verb)
					cout << "bottom count " << bottom_count << endl;
				top = get_float_for_top_index_pos(bottom_index);
				bottom = get_float_for_bot_index_pos(bottom_index);
				global_minmax = fabsf(bottom);
				if (HIST_SIZE - bottom_index > 10)
				{
					break;
				}
			}
			else
			{
				// figure out whether top or bottom is farther out
				find_hist_fraction_point_top(cutoff_fraction, top_count,
						&top_index, &top_count);
				if (verb)
					cout << "top index " << top_index << endl;
				if (verb)
					cout << "top count " << top_count << endl;
				find_hist_fraction_point_bot(cutoff_fraction, bottom_count,
						&bottom_index, &bottom_count);
				if (verb)
					cout << "bottom index " << bottom_index << endl;
				if (verb)
					cout << "bottom count " << bottom_count << endl;
				float diff_top = top_index - histogram_center();
				float diff_bot = histogram_center() - bottom_index;
				if (diff_top > 0.0f)
				{
					if (diff_top > diff_bot)
					{
						top_bottom_flag = 0;
						top = get_float_for_top_index_pos(top_index);
						bottom = get_float_for_bot_index_pos(top_index);
						global_minmax = fabsf(bottom);
						if (diff_top > 10.0f)
						{
							break;
						}
					}
					else
					{
						top_bottom_flag = 1;
						top = get_float_for_top_index_pos(bottom_index);
						bottom = get_float_for_bot_index_pos(bottom_index);
						global_minmax = fabsf(bottom);
						if (diff_bot > 10.0f)
						{
							break;
						}
					}
				}
				else if (diff_bot > 0.0f)
				{
					top_bottom_flag = 1;
					top = get_float_for_top_index_pos(bottom_index);
					bottom = get_float_for_bot_index_pos(bottom_index);
					global_minmax = fabsf(bottom);
					if (diff_bot > 10.0f)
					{
						break;
					}
				}
				else
				{
					cerr << "programmer error! check!" << endl;
					global_minmax = 1.0f;
					break;
				}
			}
			if (verb)
				cout << "Top " << top_index << " bottom " << bottom_index
						<< endl;
			histogram_loop++;
		} // while(histogram_loop < 20) over histogram looping
		if (histogram_loop >= 20)
		{
			cerr
					<< "Warning: maximum histogram loops reached on your data.  Check for bad outliers in your original data."
					<< endl;
		}

		start_data_output: if (verb)
			cout << "Global minmax is " << global_minmax << endl;
		if (verb)
			cout << "Starting data output" << endl;
		rewind(pfileX0);
		rewind(pfileX1);
		rewind(pfileY0);
		rewind(pfileY1);
		iblock = 0;
		if (global_minmax <= 0.0f)
		{
			global_minmax = 1.0f;
		}

		while (1)
		{
			size_t num;
			if (verb)
				cout << "Reading block: " << iblock << endl;

			if (iblock == block_limit)
				goto end_of_fulldata;

			// Read Re(X) data:
			num = fread(XJV, Jnumber_char, 1, pfileX0);
			if (num != 1)
				goto end_of_fulldata;
#if(NEED_TO_BYTESWAP)
			byteswap_32_bit_float_array(Jnumber_floats, XJV);
#endif

			// Read Im(X) data:
			num = fread(XJV, Jnumber_char, 1, pfileX1);
			if (num != 1)
				goto end_of_fulldata;
#if(NEED_TO_BYTESWAP)
			byteswap_32_bit_float_array(Jnumber_floats, XJV);
#endif

			// Read Re(Y) data:
			num = fread(YJV, Jnumber_char, 1, pfileY0);
			if (num != 1)
				goto end_of_fulldata;
#if(NEED_TO_BYTESWAP)
			byteswap_32_bit_float_array(Jnumber_floats, YJV);
#endif

			// Read Im(Y) data:
			num = fread(YJV, Jnumber_char, 1, pfileY1);
			if (num != 1)
				goto end_of_fulldata;
#if(NEED_TO_BYTESWAP)
			byteswap_32_bit_float_array(Jnumber_floats, YJV);
#endif

			if (verb)
				cout << setw(10) << "IBLOCK = " << setw(10) << iblock
						<< setw(10) << endl;

			//START WRITING
			for (uint32_t isub = 0; isub < puma2data.NSUBBANDS; isub++)
			{
				if (verb)
					cout << "Starting subband: " << isub << endl;
				puma2data.passSub(isub);
				for (uint32_t ichan = 0; ichan < puma2data.NCHANNELS; ichan++)
				{
					puma2data.passChan(ichan);

					puma2data.writePuMa2Block(global_minmax, XJV, YJV);
				} // End of loop over channels

			} // End of loop over subbands
			iblock++;
		} // End of while(1) loop
		end_of_fulldata: if (verb)
			cout << " *** Read " << iblock << " blocks." << endl;
	}
	else
	{
		// doing histogram by each block

		uint32_t iblock = 0;

		float X0minmax = 0.0f;
		float X1minmax = 0.0f;
		float Y0minmax = 0.0f;
		float Y1minmax = 0.0f;

		float Xminmax = 0.0f;
		float Yminmax = 0.0f;

		while (1)
		{
			Xminmax = Yminmax = 0.0f;
			size_t num;
			int top_bottom_flag = -1;
			int_fast64_t top_count = 0;
			int_fast64_t bottom_count = 0;
			int_fast32_t top_index = 0;
			int_fast32_t bottom_index = 0;
			int_fast32_t histogram_loop = 0;
			float top;
			float bottom;
			if (verb)
				cout << "Reading block: " << iblock << endl;

			if (iblock == block_limit)
				goto end_of_data;

			// Read Re(X) data:
			num = fread(XJV, Jnumber_char, 1, pfileX0);
			if (num != 1)
				goto end_of_data;

			X0minmax = byteswap_32_bit_float_array_and_find_max(Jnumber_floats,
					XJV);

			// Read Im(X) data:
			num = fread(XJV, Jnumber_char, 1, pfileX1);
			if (num != 1)
				goto end_of_data;

			X1minmax = byteswap_32_bit_float_array_and_find_max(Jnumber_floats,
					XJV);

			// Read Re(Y) data:
			num = fread(YJV, Jnumber_char, 1, pfileY0);
			if (num != 1)
				goto end_of_data;

			Y0minmax = byteswap_32_bit_float_array_and_find_max(Jnumber_floats,
					YJV);

			// Read Im(Y) data:
			num = fread(YJV, Jnumber_char, 1, pfileY1);
			if (num != 1)
				goto end_of_data;

			Y1minmax = byteswap_32_bit_float_array_and_find_max(Jnumber_floats,
					YJV);

			if (verb)
				cout << setw(10) << "IBLOCK = " << setw(10) << iblock
						<< setw(10) << endl;

			float overall_max = X0minmax;
			if (X1minmax > overall_max)
				overall_max = X1minmax;
			if (Y0minmax > overall_max)
				overall_max = Y0minmax;
			if (Y1minmax > overall_max)
				overall_max = Y1minmax;

			Xminmax = overall_max;
			Yminmax = overall_max;

			if (Yminmax > Xminmax)
			{
				Xminmax = Yminmax;
			}
			if (Xminmax <= 0.0f)
			{
				Xminmax = 1.0f;
				cerr << "block has only zeros!" << endl;
				goto start_block_output;
			}
			if (cutoff_fraction <= 0.0f)
			{
				goto start_block_output;
			}

			top = fabsf(Xminmax);
			bottom = -fabsf(Xminmax);
			while (histogram_loop < 20)
			{
				// now go back and build the histogram area
				if (verb)
					cout << "starting histogram loop " << histogram_loop
							<< endl;
				if (verb)
					cout << "hist bottom top " << bottom << " " << top << endl;
				flush_histogram();
				calculate_hist_constants(bottom, top);
				add_buffer_hist(Jnumber_floats, XJV);
				add_buffer_hist(Jnumber_floats, YJV);

				if (verb)
					cout << "Evaluating results for histogram "
							<< histogram_loop << endl;
				if (top_bottom_flag == 0)
				{
					// top already decided
					find_hist_fraction_point_top(cutoff_fraction, top_count,
							&top_index, &top_count);
					if (verb)
						cout << "top index " << top_index << endl;
					if (verb)
						cout << "top count " << top_count << endl;
					top = get_float_for_top_index_pos(top_index);
					bottom = get_float_for_bot_index_pos(top_index);
					Xminmax = fabsf(top);
					if (top_index > 10)
					{
						break;
					}
				}
				else if (top_bottom_flag == 1)
				{
					// bottom already decided
					find_hist_fraction_point_bot(cutoff_fraction, bottom_count,
							&bottom_index, &bottom_count);
					if (verb)
						cout << "bottom index " << bottom_index << endl;
					if (verb)
						cout << "bottom count " << bottom_count << endl;
					top = get_float_for_top_index_pos(bottom_index);
					bottom = get_float_for_bot_index_pos(bottom_index);
					Xminmax = fabsf(bottom);
					if (HIST_SIZE - bottom_index > 10)
					{
						break;
					}
				}
				else
				{
					// figure out whether top or bottom is farther out
					find_hist_fraction_point_top(cutoff_fraction, top_count,
							&top_index, &top_count);
					if (verb)
						cout << "top index " << top_index << endl;
					if (verb)
						cout << "top count " << top_count << endl;
					find_hist_fraction_point_bot(cutoff_fraction, bottom_count,
							&bottom_index, &bottom_count);
					if (verb)
						cout << "bottom index " << bottom_index << endl;
					if (verb)
						cout << "bottom count " << bottom_count << endl;
					float diff_top = top_index - histogram_center();
					float diff_bot = histogram_center() - bottom_index;
					if (diff_top > 0.0f)
					{
						if (diff_top > diff_bot)
						{
							top_bottom_flag = 0;
							top = get_float_for_top_index_pos(top_index);
							bottom = get_float_for_bot_index_pos(top_index);
							Xminmax = fabsf(bottom);
							if (diff_top > 10.0f)
							{
								break;
							}
						}
						else
						{
							top_bottom_flag = 1;
							top = get_float_for_top_index_pos(bottom_index);
							bottom = get_float_for_bot_index_pos(bottom_index);
							Xminmax = fabsf(bottom);
							if (diff_bot > 10.0f)
							{
								break;
							}
						}
					}
					else if (diff_bot > 0.0f)
					{
						top_bottom_flag = 1;
						top = get_float_for_top_index_pos(bottom_index);
						bottom = get_float_for_bot_index_pos(bottom_index);
						Xminmax = fabsf(bottom);
						if (diff_bot > 10.0f)
						{
							break;
						}
					}
					else
					{
						cerr << "programmer error! check!" << endl;
						Xminmax = 1.0f;
						break;
					}
				}
				histogram_loop++;
			} // while(histogram_loop < 20) over histogram looping
			if (histogram_loop >= 20)
			{
				if (verb)
					cerr
							<< "Warning: maximum histogram loops reached on your data.  Check for bad outliers in your original data."
							<< endl;
			}

			start_block_output:

			//START WRITING
			for (uint32_t isub = 0; isub < puma2data.NSUBBANDS; isub++)
			{
				if (verb)
					cout << "Starting subband: " << isub << endl;
				puma2data.passSub(isub);
				for (uint32_t ichan = 0; ichan < puma2data.NCHANNELS; ichan++)
				{
					puma2data.passChan(ichan);

					puma2data.writePuMa2Block(Xminmax, XJV, YJV);
				} // End of loop over channels

			} // End of loop over subbands
			iblock++;
		} // End of while(1) loop
		end_of_data: if (verb)
			cout << " *** Read " << iblock << " blocks." << endl;
	}
	if (verb)
		cout << "Closing output files: " << endl;
	puma2data.filesClose();

	return 0;
}

void writer::passFilenames(const char* &base, char* &hdr, char* &pset)
{
	_basename = base;
	_hdrFilename = hdr;
	_psetFilename = pset;
}

void writer::passSub(unsigned int &sub)
{
	_the_sub = sub;
}

void writer::passChan(unsigned int &chan)
{
	_the_chan = chan;
}

void writer::passPart(unsigned int &part)
{
	_the_part = part;
}


void writer::readParset()
{

// Determine the type of antenna:
	vector<string> tmps;
	tmps.push_back("Observation.antennaArray");
	tmps.push_back("Observation.antennaSet");

	ANTENNA = "UNKNOWN";
	for (unsigned int it = 0; it < tmps.size(); it++)
	{
		string key = getKeyVal(tmps[it]).substr(0, 3);
		if (key == "HBA" || key == "LBA")
		{
			ANTENNA = key;
			break;
		}
	}
	if (verb)
		cout << " ANTENNA TYPE = " << ANTENNA << endl;

// Determine clock resolution:
	string tmp = "Observation.subbandWidth";
	CLOCKRES = atof(getKeyVal(tmp).c_str())/1000.0;
	if (verb)
		cout << " CLOCK RESOLUTION = " <<setprecision(7)<< CLOCKRES << endl;

// Determine number of channels:
	tmp = "Observation.channelsPerSubband";
	NCHANNELS = atoi(getKeyVal(tmp).c_str());
	CHANNELS = NCHANNELS;
	if (verb)
		cout << " CHANNELS PER SUBBAND = " << NCHANNELS << endl;

// Determine number of polarisations:
	tmp = "Observation.nrPolarisations";
	NPOL = atoi(getKeyVal(tmp).c_str());
	if (verb)
		cout << " Number of Polarisations = " << NPOL << endl;
	if ((NPOL < 1) || (NPOL > 4))
	{
		cerr << "NPOL has bad value " << NPOL << endl;
		exit(2);
	}

//  // Determine starting subband:
//  string mychar = "";
//  int ic = 1;
//  while(1){
//    tmp = "Observation.subbandList";
//    mychar = getKeyVal(tmp).substr(1,ic);
//
//    if(mychar.substr(ic-1,1)==".") break;
//    ic++;
//  }
//  SUBBSTART = atoi(mychar.c_str());
//  if(verb)cerr << "Got SUBBSTART = " << SUBBSTART <<endl;
//
//  // Determine ending subband:
//  mychar = "";
//  int nc = ic+2;
//  ic = 1;
//  while(1){
//    tmp = "Observation.subbandList";
//    mychar = getKeyVal(tmp).substr(nc,ic);
//
//    if(mychar.substr(ic-1,1)=="]") break;
//
//    ic++;
//  }
//  SUBBEND = atoi(mychar.c_str());
//
//  if(verb){
//    cerr << "Got SUBBEND = " << SUBBEND <<endl;
//    cout << " SUBBAND RANGE = [" << SUBBSTART << "--" << SUBBEND <<"]" <<endl;
//  }
//
//  // Determine number of subbands:
//  NSUBBANDS = SUBBEND - SUBBSTART + 1;
//  SUBBANDS = NSUBBANDS;

// Load list of subbands:

	tmp = "Observation.subbandList";
	string mystring = getKeyVal(tmp);

	int jj = 0;
	int nc = 1;
	while (1)
	{
		string mychar = "";
		string lastchar = "";

		int ic = 1;

		int subfirst = -1;
		int sublast = -1;

		while (1)
		{
			mychar = mystring.substr(nc, ic);
			lastchar = mychar.substr(mychar.size() - 1, 1);

			if (lastchar == ".") // parse first subband of subrange
			{
				subfirst = atoi(mychar.substr(0, mychar.size() - 1).c_str());
				nc += (ic + 1);
				ic = 1;
				continue;
			}
			if (lastchar == "," || lastchar == "]") // parse last subband of subrange
			{
				sublast = atoi(mychar.substr(0, mychar.size() - 1).c_str());
				if (subfirst == -1)
					subfirst = sublast;
				nc += ic;
				break;
			}
			ic++;
		}

		int sbb = subfirst;
		while (sbb <= sublast)
		{
			SUBBLIST[jj] = sbb++; // store all subbands
			jj++;
		}

		if (lastchar == "]")
			break;

	}

	NSUBBANDS = jj;
	SUBBANDS = jj;


	if (verb)
		cout << " SUBBAND RANGE = [" << SUBBLIST[0] << "--"
				<< SUBBLIST[NSUBBANDS - 1] << "]" << endl;


// Determine number of subbands per part:
	tmp = "OLAP.CNProc_CoherentStokes.subbandsPerFile";
	PNSUB = atoi(getKeyVal(tmp).c_str());
	if (verb)
		cout << " Number of Subbands per Part = " << PNSUB << endl;

// Determine number of parts:
	PNUM = int(ceil(NSUBBANDS/PNSUB));

	if(PNUM > 1) 
	{
		int nbb = 0;
		while (nbb<min(PNSUB,NSUBBANDS - _the_part*PNSUB + 1))
		{
			SUBBLIST[nbb] = SUBBLIST[_the_part*PNSUB+nbb];
			nbb++;
		}

		NSUBBANDS = nbb;
		SUBBANDS = nbb;
	}	



// Determine Number of samples:
	tmp = "OLAP.CNProc.integrationSteps";
	SAMPLES = atoi(getKeyVal(tmp).c_str());
	if (verb)
		cout << "Have SAMPLES: " << SAMPLES << endl;
	if (SAMPLES < 1)
	{
		cerr << "SAMPLES has bad value " << SAMPLES << endl;
		exit(2);
	}

	free(blocksamples);
	blocksamples = reinterpret_cast<samp_t*restrict>(malloc(sizeof(samp_t)*SAMPLES));

// Determine observing epoch:
	tmp = "Observation.startTime";
	DATE = getKeyVal(tmp).c_str();

	mjdcalc();
	cout << "DATE = " << DATE << endl;

// determine the sample block offset within the loops for
// writePuMa2Block.  This is the number of subbands times the
// number of channels per subband, times the number of words
// per complex voltage
	sample_block_offset = size_t(NSUBBANDS) * NCHANNELS;



}

void writer::filesInit()
{

	for (unsigned int isub = 0; isub < NSUBBANDS; isub++)
	{

		for (unsigned int ichan = 0; ichan < NCHANNELS; ichan++)
		{

			stringstream the_sub_sstrm;
			the_sub_sstrm << SUBBLIST[isub];

			stringstream the_chan_sstrm;
			the_chan_sstrm << ichan;

			string basename_str = _basename;
			string filename_str = basename_str + "_SB" + the_sub_sstrm.str()
					+ "_CH" + the_chan_sstrm.str();
			string filename2_str = filename_str + ".rv";

			datafile[isub * NCHANNELS + ichan] = fopen(filename_str.c_str(),
					"wb");
			if (datafile[isub * NCHANNELS + ichan] == NULL)
			{
				cerr << "Unable to open file '" << filename_str
						<< "' for writing" << endl;
				exit(2);
			}
			if (realv)
			{
				realvolt[isub * NCHANNELS + ichan] = fopen(
						filename2_str.c_str(), "wb");
				if (realvolt[isub * NCHANNELS + ichan] == NULL)
				{
					cerr << "Unable to open file '" << filename2_str
							<< "' for writing" << endl;
					exit(2);
				}
			}
			writeHeader(isub, ichan);
		}

	}

}

void writer::filesClose()
{
	for (unsigned int isub = 0; isub < NSUBBANDS; isub++)
		for (unsigned int ichan = 0; ichan < NCHANNELS; ichan++)
		{
			fclose(datafile[isub * NCHANNELS + ichan]);
			if (realv)
				fclose(realvolt[isub * NCHANNELS + ichan]);
		}
}

void writer::writePuMa2Block(const float global_minmax,
		const float * const restrict Xsamp, const float * const restrict Ysamp) restrict
{

// to convert from float value f to int8_t value i, we want to do
// i = int8_t( 127.5 * f / diff )
// where diff is the maximum (absolute) difference away from 0 found
// in the values.  Rewrite this as
// i = int8_t( lrintf( f * multiplier ) )
// where multiplier = 127.5 / diff
// and lrintf rounds to the nearest integer.  In order to prevent
// going out of range, instead of exactly 127.5 we use 127.495 to
// prevent rounding errors from exceeding the int8_t range.

	static const float int8_max_float = 127.495;
	static const int_fast32_t int8_offset = 127;
	static const int_fast32_t int8_out_of_range = ~(int_fast32_t(0xFF));

	int8_t flag_value(-128);

	const size_t start = (size_t(_the_sub) * NCHANNELS + _the_chan);

	int iv = 0;
	int navg = 100;
	float vsum = 0.0f;

	size_t offset = 0;
	const float multiplier = int8_max_float / global_minmax;

////////////////REPACKING AND SAVING//////////////////////

	for (uint_fast32_t isamp = 0; isamp < SAMPLES; isamp++, offset +=
			sample_block_offset)
	{
		float reX = Xsamp[start + offset];
		float imX = Xsamp[start + offset + 1];
		float reY = Ysamp[start + offset];
		float imY = Ysamp[start + offset + 1];

		int_fast32_t reXl = lrintf(reX * multiplier);
		int_fast32_t imXl = lrintf(imX * multiplier);
		int_fast32_t reYl = lrintf(reY * multiplier);
		int_fast32_t imYl = lrintf(imY * multiplier);

		blocksamples[isamp].Xr =
				(!((reXl + int8_offset) & int8_out_of_range)) ?
						int8_t(reXl) : flag_value;
		blocksamples[isamp].Xi =
				(!((imXl + int8_offset) & int8_out_of_range)) ?
						int8_t(imXl) : flag_value;
		blocksamples[isamp].Yr =
				(!((reYl + int8_offset) & int8_out_of_range)) ?
						int8_t(reYl) : flag_value;
		blocksamples[isamp].Yi =
				(!((imYl + int8_offset) & int8_out_of_range)) ?
						int8_t(imYl) : flag_value;

		if (realv)
		{
			iv++;
			vsum += reX * reX + imX * imX + reY * reY + imY * imY;

//			float phX = atan2(imX,reX);
//			float phY = atan2(imY,reY);
			
//			vsum = phY-phX;
//			while(vsum<-3.14) vsum += 6.28;
//			while(vsum>3.14) vsum -= 6.28;
			
//			fwrite(&phY, sizeof(vsum), 1,
//						realvolt[_the_sub * NCHANNELS + _the_chan]);
			
			
			if (iv == navg)
			{
				if (fwrite(&vsum, sizeof(vsum), 1,
						realvolt[_the_sub * NCHANNELS + _the_chan]) != 1)
				{
					cerr << "Error writing to realvolt" << endl;
				}
				iv = 0;
				vsum = 0.0f;
			}
		}
	}

	if (fwrite(blocksamples, sizeof(samp_t) * SAMPLES, 1,
			datafile[_the_sub * NCHANNELS + _the_chan]) != 1)
	{
		cerr << "Error writing to datafile for blocksamples" << endl;
		exit(2);
	}
}

void writer::writeHeader(unsigned int &isub, unsigned int &ichan)
{

	const int HEADER_SIZE = 4096;
	char buf[HEADER_SIZE];

	/* for header */
	memset(buf, '\0', sizeof(buf));

	string line;
	stringstream the_freq, the_mjd, the_bw, the_tsamp;
	ifstream headerfile;
	headerfile.open(_hdrFilename);


/*
   if (CHANNELS > 1) {
      subintdata.freq_cent = 100 + (subbandFirst - (0.5/CHANNELS) - 0.5) * CLOCKRES
   } else {  // bypass 2PPF, freq_cent calculation changes
      subintdata.freq_cent = 100 + (subbandFirst - (0.5/CHANNELS)) * CLOCKRES 
   }
*/



	while (1)
	{
		getline(headerfile, line, '\n');

		if (line.substr(0, 4) == "FREQ")
		{
			/* Only correct for 200MHz clock... Vlad, Jul 12, 2012 */
			float extra_half;
			if (NCHANNELS > 1) extra_half = 0.5; else extra_half = 0.0;
			if (ANTENNA == "HBA") the_freq << setprecision(20) 
				<< 100.0 + (SUBBLIST[isub] + float(ichan) / float(NCHANNELS) - 0.5/float(NCHANNELS) - extra_half) * CLOCKRES;
			else if (ANTENNA == "LBA") the_freq << setprecision(20)
					<< (SUBBLIST[isub] + float(ichan) / float(NCHANNELS) - 0.5/float(NCHANNELS) - extra_half) * CLOCKRES;
			else the_freq << setprecision(20)
					<< 100.0 + (SUBBLIST[isub] + float(ichan) / float(NCHANNELS) - 0.5/float(NCHANNELS) - extra_half) * CLOCKRES;

			line = "FREQ " + the_freq.str();
		}
		else if (line.substr(0, 9) == "MJD_START")
		{

			the_mjd << setprecision(20) << mjdcalc();
			line = "MJD_START " + the_mjd.str();
		}
		else if (line.substr(0, 9) == "UTC_START")
		{
			line = "UTC_START " + DATE.substr(1, 19);
		}
		else if (line.substr(0, 2) == "BW")
		{
			the_bw << setprecision(20) << CLOCKRES / NCHANNELS;
			line = "BW " + the_bw.str().substr();
		}
		else if (line.substr(0, 5) == "TSAMP")
		{
			the_tsamp << setprecision(8) << (1.0 / CLOCKRES) * NCHANNELS;
			line = "TSAMP " + the_tsamp.str().substr();
		}

		strncat(buf, (line + "\n").c_str(), HEADER_SIZE - strlen(buf) - 1);
		if (headerfile.eof())
			break;

	}

	if (fwrite(buf, HEADER_SIZE, 1, datafile[isub * NCHANNELS + ichan]) != 1)
	{
		cerr << "Error writing to datafile header for isub " << isub
				<< " ichan " << ichan << endl;
		exit(2);
	}
	headerfile.close();

}

// obtaining file size
streamsize getSize(const char* filename)
{
	streamsize begin, end;
	ifstream myfile(filename, ios::binary);
	begin = myfile.tellg();
	myfile.seekg(0, ios::end);
	end = myfile.tellg();
	myfile.close();

	return end - begin;
}

string writer::getKeyVal(string &key)
{

	string line;
//int start;
	string value;

	ifstream parsetfile;
	parsetfile.open(_psetFilename);

	while (1)
	{
		getline(parsetfile, line, '\n');

		if (parsetfile.eof())
			break;

		for (int i = line.find(key, 0); i != int(string::npos);
				i = line.find(key, i))
		{

			istringstream ostr(line);
			istream_iterator<string> it(ostr);
			istream_iterator<string> end;

			size_t nwords = 0;
			while (it++ != end)
				nwords++;

			string temp;
			istringstream words(line);
			vector<string> parsed;
			while (words)
			{
				words >> temp;
				if (parsed.size() >= nwords)
					break;
				parsed.push_back(temp);
			}

			if (nwords > 3)
			{
				value = parsed[nwords - 2] + "-" + parsed[nwords - 1];
			}
			else
			{
				value = parsed[nwords - 1];
			}
			i++;
		}
	}
	parsetfile.close();
	return value;
}

double writer::mjdcalc()
{

	int yy = atoi(DATE.substr(1, 4).c_str());
	int mm = atoi(DATE.substr(6, 2).c_str());
	int dd = atoi(DATE.substr(9, 2).c_str());

	int hh = atoi(DATE.substr(12, 2).c_str());
	int mi = atoi(DATE.substr(15, 2).c_str());
	int ss = atoi(DATE.substr(18, 2).c_str());

	int m, y, ia, ib, ic;
	double jd, mjd0;

	if (mm <= 2)
	{
		y = yy - 1;
		m = mm + 12;
	}
	else
	{
		y = yy;
		m = mm;
	}
	ia = y / 100;
	ib = 2 - ia + ia / 4;
	if (y + m / 100. + dd / 1e4 < 1582.1015)
		ib = 0;
	ia = (int) ((m + 1) * 30.6001);
	ic = (int) (y * 365.25);
	jd = dd + ia + ib + ic + 1720994.5;
	mjd0 = jd - 2400000.5;

	mjd0 += ((double) (hh) + (double) (mi) / 60.0 + (double) (ss) / 3600.0)
			/ 24.0;

	return mjd0;
}

float byteswap_32_bit_float_array_and_find_max(const size_t NUM,
		float* const restrict fp)
{
	static const size_t SIZE = 4;
	float max0, max1, max2, max3;
	float min0, min1, min2, min3;
#if(NEED_TO_BYTESWAP)
	int32_t* const restrict ip(reinterpret_cast<int32_t* const restrict>(fp));
	if (NUM >= SIZE)
	{
		{
#  ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
			int32_t a0 = __builtin_bswap32(ip[0]);
			int32_t a1 = __builtin_bswap32(ip[1]);
			int32_t a2 = __builtin_bswap32(ip[2]);
			int32_t a3 = __builtin_bswap32(ip[3]);
#    else
			uint32_t u0(ip[0]);
			uint32_t u1(ip[1]);
			uint32_t u2(ip[2]);
			uint32_t u3(ip[3]);
			int32_t a0 = int32_t((u0>>24)|((u0&0xFF0000)>>8)|((u0&0xFF00)<<8)|(u0<<24));
			int32_t a1 = int32_t((u1>>24)|((u1&0xFF0000)>>8)|((u1&0xFF00)<<8)|(u1<<24));
			int32_t a2 = int32_t((u2>>24)|((u2&0xFF0000)>>8)|((u2&0xFF00)<<8)|(u2<<24));
			int32_t a3 = int32_t((u3>>24)|((u3&0xFF0000)>>8)|((u3&0xFF00)<<8)|(u3<<24));
#    endif
#  else
			uint32_t u0(ip[0]);
			uint32_t u1(ip[1]);
			uint32_t u2(ip[2]);
			uint32_t u3(ip[3]);
			int32_t a0 = int32_t((u0>>24)|((u0&0xFF0000)>>8)|((u0&0xFF00)<<8)|(u0<<24));
			int32_t a1 = int32_t((u1>>24)|((u1&0xFF0000)>>8)|((u1&0xFF00)<<8)|(u1<<24));
			int32_t a2 = int32_t((u2>>24)|((u2&0xFF0000)>>8)|((u2&0xFF00)<<8)|(u2<<24));
			int32_t a3 = int32_t((u3>>24)|((u3&0xFF0000)>>8)|((u3&0xFF00)<<8)|(u3<<24));
#  endif
			float f0 = int32_t_bits_to_float(a0);
			float f1 = int32_t_bits_to_float(a1);
			float f2 = int32_t_bits_to_float(a2);
			float f3 = int32_t_bits_to_float(a3);
			ip[0] = a0;
			ip[1] = a1;
			ip[2] = a2;
			ip[3] = a3;
			max0 = min0 = f0;
			max1 = min1 = f1;
			max2 = min2 = f2;
			max3 = min3 = f3;
		}
		size_t s;
		for (s = SIZE; s < NUM; s += SIZE)
		{
#  ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
			int32_t a0 = __builtin_bswap32(ip[s + 0]);
			int32_t a1 = __builtin_bswap32(ip[s + 1]);
			int32_t a2 = __builtin_bswap32(ip[s + 2]);
			int32_t a3 = __builtin_bswap32(ip[s + 3]);
#    else
			uint32_t u0(ip[s+0]);
			uint32_t u1(ip[s+1]);
			uint32_t u2(ip[s+2]);
			uint32_t u3(ip[s+3]);
			int32_t a0 = int32_t((u0>>24)|((u0&0xFF0000)>>8)|((u0&0xFF00)<<8)|(u0<<24));
			int32_t a1 = int32_t((u1>>24)|((u1&0xFF0000)>>8)|((u1&0xFF00)<<8)|(u1<<24));
			int32_t a2 = int32_t((u2>>24)|((u2&0xFF0000)>>8)|((u2&0xFF00)<<8)|(u2<<24));
			int32_t a3 = int32_t((u3>>24)|((u3&0xFF0000)>>8)|((u3&0xFF00)<<8)|(u3<<24));
#    endif
#  else
			uint32_t u0(ip[s+0]);
			uint32_t u1(ip[s+1]);
			uint32_t u2(ip[s+2]);
			uint32_t u3(ip[s+3]);
			int32_t a0 = int32_t((u0>>24)|((u0&0xFF0000)>>8)|((u0&0xFF00)<<8)|(u0<<24));
			int32_t a1 = int32_t((u1>>24)|((u1&0xFF0000)>>8)|((u1&0xFF00)<<8)|(u1<<24));
			int32_t a2 = int32_t((u2>>24)|((u2&0xFF0000)>>8)|((u2&0xFF00)<<8)|(u2<<24));
			int32_t a3 = int32_t((u3>>24)|((u3&0xFF0000)>>8)|((u3&0xFF00)<<8)|(u3<<24));
#  endif
			float f0 = int32_t_bits_to_float(a0);
			float f1 = int32_t_bits_to_float(a1);
			float f2 = int32_t_bits_to_float(a2);
			float f3 = int32_t_bits_to_float(a3);
			ip[s + 0] = a0;
			ip[s + 1] = a1;
			ip[s + 2] = a2;
			ip[s + 3] = a3;
			if (f0 > max0)
			max0 = f0;
			else if (f0 < min0)
			min0 = f0;
			if (f1 > max1)
			max1 = f1;
			else if (f1 < min1)
			min1 = f1;
			if (f2 > max2)
			max2 = f2;
			else if (f2 < min2)
			min2 = f2;
			if (f3 > max3)
			max3 = f3;
			else if (f3 < min3)
			min3 = f3;
		}
		if (s != NUM)
		{
			s -= SIZE;
			for (; s < NUM; s++)
			{
#  ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
				int32_t a = __builtin_bswap32(ip[s]);
#    else
				uint32_t u(ip[s]);
				int32_t a = int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#    endif
#  else
				uint32_t u(ip[s]);
				int32_t a = int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#  endif
				float f = int32_t_bits_to_float(a);
				ip[s] = a;
				if (f > max0)
				max0 = f;
				else if (f < min0)
				min0 = f;
			}
		}
		max0 = fabsf(max0);
		max1 = fabsf(max1);
		max2 = fabsf(max2);
		max3 = fabsf(max3);
		min0 = fabsf(min0);
		min1 = fabsf(min1);
		min2 = fabsf(min2);
		min3 = fabsf(min3);
		float d0 = (max0 > min0) ? max0 : min0;
		float d1 = (max1 > min1) ? max1 : min1;
		float d2 = (max2 > min2) ? max2 : min2;
		float d3 = (max3 > min3) ? max3 : min3;
		float d01 = (d0 > d1) ? d0 : d1;
		float d23 = (d2 > d3) ? d2 : d3;
		float d = (d01 > d23) ? d01 : d23;
		return d;
	}
	else
	{ // NUM < SIZE
		max0 = min0 = 0.0f;
		for (size_t s = 0; s < NUM; s++)
		{
#  ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
			int32_t a = __builtin_bswap32(ip[s]);
#    else
			uint32_t u(ip[s]);
			int32_t a = int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#    endif
#  else
			uint32_t u(ip[s]);
			int32_t a = int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#  endif
			float f = int32_t_bits_to_float(a);
			ip[s] = a;
			if (f > max0)
			max0 = f;
			if (f < min0)
			min0 = f;
		}
		max0 = fabsf(max0);
		min0 = fabsf(min0);
		float d = (max0 > min0) ? max0 : min0;
		return d;
	}
#else
	if (NUM >= SIZE)
	{
		{
			float f0 = fp[0];
			float f1 = fp[1];
			float f2 = fp[2];
			float f3 = fp[3];
			max0 = min0 = f0;
			max1 = min1 = f1;
			max2 = min2 = f2;
			max3 = min3 = f3;
		}
		size_t s;
		for (s = SIZE; s < NUM; s += SIZE)
		{
			float f0 = fp[s + 0];
			float f1 = fp[s + 1];
			float f2 = fp[s + 2];
			float f3 = fp[s + 3];
			if (f0 > max0)
				max0 = f0;
			else if (f0 < min0)
				min0 = f0;
			if (f1 > max1)
				max1 = f1;
			else if (f1 < min1)
				min1 = f1;
			if (f2 > max2)
				max2 = f2;
			else if (f2 < min2)
				min2 = f2;
			if (f3 > max3)
				max3 = f3;
			else if (f3 < min3)
				min3 = f3;
		}
		if (s != NUM)
		{
			s -= SIZE;
			for (; s < NUM; s++)
			{
				float f = fp[s];
				if (f > max0)
					max0 = f;
				else if (f < min0)
					min0 = f;
			}
		}
		max0 = fabsf(max0);
		max1 = fabsf(max1);
		max2 = fabsf(max2);
		max3 = fabsf(max3);
		min0 = fabsf(min0);
		min1 = fabsf(min1);
		min2 = fabsf(min2);
		min3 = fabsf(min3);
		float d0 = (max0 > min0) ? max0 : min0;
		float d1 = (max1 > min1) ? max1 : min1;
		float d2 = (max2 > min2) ? max2 : min2;
		float d3 = (max3 > min3) ? max3 : min3;
		float d01 = (d0 > d1) ? d0 : d1;
		float d23 = (d2 > d3) ? d2 : d3;
		float d = (d01 > d23) ? d01 : d23;
		return d;
	}
	else
	{ // NUM < SIZE
		max0 = min0 = 0.0f;
		for (size_t s = 0; s < NUM; s++)
		{
			float f = fp[s];
			if (f > max0)
				max0 = f;
			if (f < min0)
				min0 = f;
		}
		max0 = fabsf(max0);
		min0 = fabsf(min0);
		float d = (max0 > min0) ? max0 : min0;
		return d;
	}
#endif
	return 0.0f;
}

int_fast64_t hist0[HIST_SIZE];
int_fast64_t hist_sum = 0;
float hist_s = 0.0f;
float hist_b = 0.0f;
float hist_min = 0.0f;
float hist_max = 0.0f;

void flush_histogram(void)
{
	memset(hist0, 0, sizeof(int_fast64_t) * HIST_SIZE);
	hist_sum = 0;
	return;
}

void calculate_hist_constants(float min, float max)
{
	if (min != max)
	{
		float b = (-0.49f + (0.51f - HIST_SIZE) * (min / max))
				/ (1.0f - min / max);
		float s;
		if (min != 0.0f)
		{
			s = (-0.49f - b) / min;
		}
		else
		{
			s = (HIST_SIZE - 0.51f - b) / max;
		}
		hist_b = b;
		hist_s = s;
		hist_min = min;
		hist_max = max;
	}
	else
	{
		hist_b = 0.0f;
		hist_s = 1.0f;
		hist_min = -1.0f;
		hist_max = +1.0f;
	}
//if(verb) cout << "hist constants " << hist_b << " " << hist_s << endl;
	return;
}

void add_buffer_hist(const size_t NUM, const float* const restrict fp)
{
	static const size_t SIZE = 4;
	const float b = hist_b;
	const float slope = hist_s;
	if (NUM >= SIZE)
	{
		size_t s;
		for (s = 0; s < NUM; s += SIZE)
		{
			float f0 = fp[s + 0];
			float f1 = fp[s + 1];
			float f2 = fp[s + 2];
			float f3 = fp[s + 3];
			int_fast32_t i0(lrintf(f0 * slope + b));
			int_fast32_t i1(lrintf(f1 * slope + b));
			int_fast32_t i2(lrintf(f2 * slope + b));
			int_fast32_t i3(lrintf(f3 * slope + b));
			if ((i0 >= 0) && (i0 < HIST_SIZE))
			{
				++hist0[i0];
			}
			if ((i1 >= 0) && (i1 < HIST_SIZE))
			{
				++hist0[i1];
			}
			if ((i2 >= 0) && (i2 < HIST_SIZE))
			{
				++hist0[i2];
			}
			if ((i3 >= 0) && (i3 < HIST_SIZE))
			{
				++hist0[i3];
			}
		}
		if (s != NUM)
		{
			s -= SIZE;
			for (; s < NUM; s++)
			{
				float f0 = fp[s];
				int_fast32_t i0(lrintf(f0 * slope + b));
				if ((i0 >= 0) && (i0 < HIST_SIZE))
				{
					++hist0[i0];
				}
			}
		}
	}
	else
	{ // NUM < SIZE
		for (size_t s = 0; s < NUM; s++)
		{
			float f0 = fp[s];
			int_fast32_t i0(lrintf(f0 * slope + b));
			if ((i0 >= 0) && (i0 < HIST_SIZE))
			{
				++hist0[i0];
			}
		}
	}
	hist_sum += NUM;
	return;
}

void find_hist_fraction_point_top(const float fraction,
		const int_fast64_t count_offset, int_fast32_t* index,
		int_fast64_t* count)
{
	if (fraction == 0.0f)
	{
		*index = HIST_SIZE - 1;
		*count = count_offset;
		return;
	}
	const int_fast64_t fraction_int(llrint(double(fraction) * hist_sum));
	int_fast64_t sum = count_offset;
	for (int_fast32_t i = HIST_SIZE - 1; i >= 0; i--)
	{
		sum += hist0[i];
		if (sum > fraction_int)
		{
			*index = i;
			*count = sum - hist0[i];
			return;
		}
	}
	*index = 0;
	*count = sum - hist0[0];
	return;
}

void find_hist_fraction_point_bot(const float fraction,
		const int_fast64_t count_offset, int_fast32_t* index,
		int_fast64_t* count)
{
//     if(verb) {
//         int_fast64_t sum0=0;
//         int_fast64_t sum1=hist_sum;
//         for(int_fast32_t i=0; i < HIST_SIZE; i++) {
//             sum0 += hist0[i];
//             sum1 -= hist0[i];
//             cout << i << " " << sum0/float(hist_sum) << " " << sum1/float(hist_sum) << " " << sum0 << " " << sum1 << endl;
//         }
//     }
	if (fraction == 0.0f)
	{
		*index = 0;
		*count = count_offset;
		return;
	}
	const int_fast64_t fraction_int(llrint(double(fraction) * hist_sum));
	int_fast64_t sum = count_offset;
	for (int_fast32_t i = 0; i < HIST_SIZE; i++)
	{
		sum += hist0[i];
		if (sum > fraction_int)
		{
			*index = i;
			*count = sum - hist0[i];
			return;
		}
	}
	*index = HIST_SIZE - 1;
	*count = sum - hist0[HIST_SIZE - 1];
	return;
}

float get_float_for_top_index_pos(const int_fast32_t index)
{
	return (index + 0.5f - hist_b) / (hist_s);
}

float get_float_for_bot_index_pos(const int_fast32_t index)
{
	return (index - 0.5f - hist_b) / (hist_s);
}


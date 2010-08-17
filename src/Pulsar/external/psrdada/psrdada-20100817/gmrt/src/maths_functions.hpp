
#pragma once

namespace math {

template<typename T> inline T min(const T& a, const T& b) {
	return a<b ? a : b;
}
template<typename T> inline T max(const T& a, const T& b) {
	return a<b ? a : b;
}
template<typename T> inline T div_round_up(const T& a, const T& b) {
	return (a-1)/b+1;
}

namespace detail
{

// Compile-time log2 implementation
template<unsigned int N, unsigned int CUR>
struct static_log2 {
    static const unsigned int value = static_log2<(N >> 1),CUR+1>::value;
};
template<unsigned int CUR>
struct static_log2<1, CUR> {
    static const unsigned int value = CUR;
};

typedef unsigned long ulong;

// Run-time log2 implementation	
template<unsigned Size>
inline ulong log2_impl(ulong n) { /* error_invalid_type */ return 0; }
template<> inline ulong log2_impl<1>(ulong n) {
	int pos = 0;
	if (n >= 1 << 4) { n >>=  4; pos +=  4; }
	if (n >= 1 << 2) { n >>=  2; pos +=  2; }
	if (n >= 1 << 1) {           pos +=  1; }
	return ((n == 0) ? (-1) : pos);
}
template<> inline ulong log2_impl<2>(ulong n) {
	int pos = 0;
	if (n >= 1 << 8) { n >>=  8; pos +=  8; }
	if (n >= 1 << 4) { n >>=  4; pos +=  4; }
	if (n >= 1 << 2) { n >>=  2; pos +=  2; }
	if (n >= 1 << 1) {           pos +=  1; }
	return ((n == 0) ? (-1) : pos);
}
template<> inline ulong log2_impl<4>(ulong n) {
	int pos = 0;
	if (n >= 1 <<16) { n >>= 16; pos += 16; }
	if (n >= 1 << 8) { n >>=  8; pos +=  8; }
	if (n >= 1 << 4) { n >>=  4; pos +=  4; }
	if (n >= 1 << 2) { n >>=  2; pos +=  2; }
	if (n >= 1 << 1) {           pos +=  1; }
	return ((n == 0) ? (-1) : pos);
}

} // namespace detail

// Compile-time integer log2
template<unsigned int N>
struct static_log2 {
    static const unsigned int value = detail::static_log2<N,0>::value;
};

// Run-time integer log2
template<typename T>
inline T log2(T n) {
	// Statically dispatch on sizeof(T)
	return detail::log2_impl<sizeof(T)>(n);
}

} // namespace math

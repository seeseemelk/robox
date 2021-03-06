#ifndef _FIXFFT_H_
#define _FIXFFT_H_

#include "defs.h"

#include <avr/pgmspace.h>
#include <stdbool.h>

/**
 * Perform forward/inverse fast Fourier transform.
 * fr[n],fi[n] are real and imaginary arrays, both INPUT AND
 * RESULT (in-place FFT), with 0 <= n < 2**m; set inverse to
 * 0 for forward transform (FFT), or 1 for iFFT.
 */
i16 fix_fft(i8 fr[], i8 fi[], i16 m, bool inverse);

/**
 * forward/inverse FFT on array of real numbers.
 * Real FFT/iFFT using half-size complex FFT by distributing
 * even/odd samples into real/imaginary arrays respectively.
 * In order to save data space (i.e. to avoid two arrays, one
 * for real, one for imaginary samples), we proceed in the
 * following two steps: a) samples are rearranged in the real
 * array so that all even samples are in places 0-(N/2-1) and
 * all imaginary samples in places (N/2)-(N-1), and b) fix_fft
 * is called with fr and fi pointing to index 0 and index N/2
 * respectively in the original array. The above guarantees
 * that fix_fft "sees" consecutive real samples as alternating
 * real and imaginary samples in the complex array.
 */
i16 fix_fftr(i8 f[], i16 m, bool inverse);

#endif

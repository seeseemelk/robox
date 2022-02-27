#include "defs.h"
#include "fix_fft.h"

/* fix_fft.c - Fixed-poi16 in-place Fast Fourier Transform  */
/*
  All data are fixed-poi16 short i16egers, in which -32768
  to +32768 represent -1.0 to +1.0 respectively. i16eger
  arithmetic is used for speed, instead of the more natural
  floating-poi16.

  For the forward FFT (time -> freq), fixed scaling is
  performed to prevent arithmetic overflow, and to map a 0dB
  sine/cosine wave (i.e. amplitude = 32767) to two -6dB freq
  coefficients. The return value is always 0.

  For the inverse FFT (freq -> time), fixed scaling cannot be
  done, as two 0dB coefficients would sum to a peak amplitude
  of 64K, overflowing the 32k range of the fixed-poi16 i16egers.
  Thus, the fix_fft() routine performs variable scaling, and
  returns a value which is the number of bits LEFT by which
  the output must be shifted to get the actual amplitude
  (i.e. if fix_fft() returns 3, each value of fr[] and fi[]
  must be multiplied by 8 (2**3) for proper scaling.
  Clearly, this cannot be done within fixed-poi16 short
  i16egers. In practice, if the result is to be used as a
  filter, the scale_shift can usually be ignored, as the
  result will be approximately correctly normalized as is.

  Written by:  Tom Roberts  11/8/89
  Made portable:  Malcolm Slaney 12/15/94 malcolm@i16erval.com
  Enhanced:  Dimitrios P. Bouras  14 Jun 2006 dbouras@ieee.org
  Modified for 8bit values David Keller  10.10.2010
*/


#define N_WAVE  256    /* full length of Sinewave[] */
#define LOG2_N_WAVE 8   /* log2(N_WAVE) */

/*
  Since we only use 3/4 of N_WAVE, we define only
  this many samples, in order to conserve data space.
*/

const i8 s_sinewave[N_WAVE-N_WAVE/4] PROGMEM =
{
	0, 3, 6, 9, 12, 15, 18, 21,
	24, 28, 31, 34, 37, 40, 43, 46,
	48, 51, 54, 57, 60, 63, 65, 68,
	71, 73, 76, 78, 81, 83, 85, 88,
	90, 92, 94, 96, 98, 100, 102, 104,
	106, 108, 109, 111, 112, 114, 115, 117,
	118, 119, 120, 121, 122, 123, 124, 124,
	125, 126, 126, 127, 127, 127, 127, 127,

	127, 127, 127, 127, 127, 127, 126, 126,
	125, 124, 124, 123, 122, 121, 120, 119,
	118, 117, 115, 114, 112, 111, 109, 108,
	106, 104, 102, 100, 98, 96, 94, 92,
	90, 88, 85, 83, 81, 78, 76, 73,
	71, 68, 65, 63, 60, 57, 54, 51,
	48, 46, 43, 40, 37, 34, 31, 28,
	24, 21, 18, 15, 12, 9, 6, 3,

	0, -3, -6, -9, -12, -15, -18, -21,
	-24, -28, -31, -34, -37, -40, -43, -46,
	-48, -51, -54, -57, -60, -63, -65, -68,
	-71, -73, -76, -78, -81, -83, -85, -88,
	-90, -92, -94, -96, -98, -100, -102, -104,
	-106, -108, -109, -111, -112, -114, -115, -117,
	-118, -119, -120, -121, -122, -123, -124, -124,
	-125, -126, -126, -127, -127, -127, -127, -127,
};

/**
 * fixed-poi16 multiplication & scaling.
 * Substitute inline assembly for hardware-specific
 * optimization suited to a particluar DSP processor.
 * Scaling ensures that result remains 16-bit.
 */
i8 fix_mpy(i8 a, i8 b)
{
	/* shift right one less bit (i.e. 15-1) */
	i16 c = ((i16)a * (i16)b) >> 6;
	/* last bit shifted out = rounding-bit */
	b = c & 0x01;
	/* last shift + rounding bit */
	a = (c >> 1) + b;

	return a;
}

i16 fix_fft(i8 fr[], i8 fi[], i16 m, bool inverse)
{
	i16 mr, nn, i, j, l, k, istep, n, scale, shift;
	i8 qr, qi, tr, ti, wr, wi;

	n = 1 << m;

	/* max FFT size = N_WAVE */
	if (n > N_WAVE)
		return -1;

	mr = 0;
	nn = n - 1;
	scale = 0;

	/* decimation in time - re-order data */
	for (m = 1; m <= nn; ++m)
	{
		l = n;
		do
		{
			l >>= 1;
		}
		while (mr+l > nn);
		mr = (mr & (l-1)) + l;

		if (mr <= m)
			continue;
		tr = fr[m];
		fr[m] = fr[mr];
		fr[mr] = tr;
		ti = fi[m];
		fi[m] = fi[mr];
		fi[mr] = ti;
	}

	l = 1;
	k = LOG2_N_WAVE-1;
	while (l < n)
	{
		if (inverse)
		{
			/* variable scaling, depending upon data */
			shift = 0;
			for (i=0; i<n; ++i)
			{
				j = fr[i];
				if (j < 0)
					j = -j;
				m = fi[i];
				if (m < 0)
					m = -m;
				if (j > 16383 || m > 16383)
				{
					shift = 1;
					break;
				}
			}
			if (shift)
			++scale;
		}
		else
		{
			/*
			fixed scaling, for proper normalization --
			there will be log2(n) passes, so this results
			in an overall factor of 1/n, distributed to
			maximize arithmetic accuracy.
			*/
			shift = 1;
		}
		/*
		it may not be obvious, but the shift will be
		performed on each data poi16 exactly once,
		during this pass.
		*/
		istep = l << 1;
		for (m=0; m<l; ++m)
		{
			j = m << k;
			/* 0 <= j < N_WAVE/2 */
			wr =  pgm_read_byte_near(s_sinewave + j+N_WAVE/4);

			wi = -pgm_read_byte_near(s_sinewave + j);
			if (inverse)
				wi = -wi;
			if (shift)
			{
				wr >>= 1;
				wi >>= 1;
			}
			for (i=m; i<n; i+=istep)
			{
				j = i + l;
				tr = fix_mpy(wr,fr[j]) - fix_mpy(wi,fi[j]);
				ti = fix_mpy(wr,fi[j]) + fix_mpy(wi,fr[j]);
				qr = fr[i];
				qi = fi[i];
				if (shift)
					{
					qr >>= 1;
					qi >>= 1;
				}
				fr[j] = qr - tr;
				fi[j] = qi - ti;
				fr[i] = qr + tr;
				fi[i] = qi + ti;
			}
		}
		--k;
		l = istep;
	}
	return scale;
}

i16 fix_fftr(i8 f[], i16 m, bool inverse)
{
	i16 i, N = 1<<(m-1), scale = 0;
	i8 tt, *fr=f, *fi=&f[N];

	if (inverse)
		scale = fix_fft(fi, fr, m-1, inverse);
	for (i=1; i<N; i+=2)
	{
		tt = f[N+i-1];
		f[N+i-1] = f[i];
		f[i] = tt;
	}
	if (!inverse)
		scale = fix_fft(fi, fr, m-1, inverse);
	return scale;
}


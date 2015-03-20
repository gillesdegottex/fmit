#include "LPC.h"

// http://kbs.cs.tu-berlin.de/~jutta/gsm/lpc.html

#define	P_MAX	8	/* order p of LPC analysis, typically 8..14 */

/* Invented by N. Levinson in 1947, modified by J. Durbin in 1959.
 */
double            	    /* returns minimum mean square error    */
			levinson_durbin(
			double const * ac,  /*  in: [0...p] autocorrelation values  */
	double 	     * ref, /* out: [0...p-1] reflection coef's	    */
	double	     * lpc) /*      [0...p-1] LPC coefficients      */
{
	int i, j;  double r, error = ac[0];

	if (ac[0] == 0) {
		for (i = 0; i < P_MAX; i++) ref[i] = 0; return 0; }

		for (i = 0; i < P_MAX; i++) {

		/* Sum up this iteration's reflection coefficient.
		*/
			r = -ac[i + 1];
			for (j = 0; j < i; j++) r -= lpc[j] * ac[i - j];
			ref[i] = r /= error;

		/*  Update LPC coefficients and total error.
		*/
			lpc[i] = r;
			for (j = 0; j < i/2; j++) {
				double tmp  = lpc[j];
				lpc[j]     += r * lpc[i-1-j];
				lpc[i-1-j] += r * tmp;
			}
			if (i % 2) lpc[j] += lpc[j] * r;

			error *= 1.0 - r * r;
		}
		return error;
}

/* I. Sch"ur's recursion from 1917 is related to the Levinson-Durbin
 * method, but faster on parallel architectures; where Levinson-Durbin
 * would take time proportional to p * log(p), Sch"ur only requires
 * time proportional to p.  The GSM coder uses an integer version of
 * the Sch"ur recursion.
 */
double			   /* returns the minimum mean square error */
			schur(  double const * ac,	/*  in: [0...p] autocorrelation c's */
					double       * ref)	/* out: [0...p-1] reflection c's    */
{
	int i, m;  double r, error = ac[0], G[2][P_MAX];

	if (ac[0] == 0.0) {
		for (i = 0; i < P_MAX; i++) ref[i] = 0;
		return 0;
	}

	/*  Initialize rows of the generator matrix G to ac[1...P]
	*/
	for (i = 0; i < P_MAX; i++) G[0][i] = G[1][i] = ac[i+1];

	for (i = 0;;) {

		/*  Calculate this iteration's reflection
		*  coefficient and error.
		*/
		ref[i] = r = -G[1][0] / error;
		error += G[1][0] * r;

		if (++i >= P_MAX) return error;

		/*  Update the generator matrix.
		*
		*  Unlike the Levinson-Durbin summing of reflection
		*  coefficients, this loop could be distributed to
		*  p processors who each take only constant time.
		*/
		for (m = 0; m < P_MAX - i; m++) {
			G[1][m] = G[1][m+1] + r * G[0][m];
			G[0][m] = G[1][m+1] * r + G[0][m];
		}
	}
}

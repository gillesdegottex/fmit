#ifndef _Autocorrelation_h_
#define _Autocorrelation_h_

/* Compute the autocorrelation
 *			,--,
 *  ac(i) = >  x(n) * x(n-i)  for all n
 *	  		`--'
 * for lags between 0 and lag-1, and x == 0 outside 0...n-1
 */
void autocorrelation(
		int   n, double const * x,   /*  in: [0...n-1] samples x   */
	int lag, double       * ac);  /* out: [0...lag-1] ac values */

#endif

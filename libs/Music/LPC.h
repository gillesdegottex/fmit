#ifndef _LPC_h_
#define _LPC_h_

// http://kbs.cs.tu-berlin.de/~jutta/gsm/lpc.html

/* LPC- and Reflection Coefficients
 *
 * The next two functions calculate linear prediction coefficients
 * and/or the related reflection coefficients from the first P_MAX+1
 * values of the autocorrelation function.
 */

/* Invented by N. Levinson in 1947, modified by J. Durbin in 1959.
 */
double            	    /* returns minimum mean square error    */
			levinson_durbin(
			double const * ac,  /*  in: [0...p] autocorrelation values  */
	double 	     * ref, /* out: [0...p-1] reflection coef's	    */
	double	     * lpc); /*      [0...p-1] LPC coefficients      */

/* I. Sch"ur's recursion from 1917 is related to the Levinson-Durbin
 * method, but faster on parallel architectures; where Levinson-Durbin
 * would take time proportional to p * log(p), Sch"ur only requires
 * time proportional to p.  The GSM coder uses an integer version of
 * the Sch"ur recursion.
 */
double			   /* returns the minimum mean square error */
			schur(  double const * ac,	/*  in: [0...p] autocorrelation c's */
					double       * ref);	/* out: [0...p-1] reflection c's    */

#endif

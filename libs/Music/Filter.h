// Copyright 2007 "Gilles Degottex"

// This file is part of "Music"

// "Music" is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "Music" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#ifndef _Filter_h_
#define _Filter_h_

#include <assert.h>
#include <vector>
#include <deque>

namespace Music
{
    /**************************************************************************
    * Parks-McClellan algorithm for FIR filter design (C version)
    *-------------------------------------------------
    *  Copyright (c) 1995,1998  Jake Janovetz (janovetz@uiuc.edu)
    *
    *  This library is free software; you can redistribute it and/or
    *  modify it under the terms of the GNU Library General Public
    *  License as published by the Free Software Foundation; either
    *  version 2 of the License, or (at your option) any later version.
    *
    *  This library is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    *  Library General Public License for more details.

    *  You should have received a copy of the GNU Library General Public
    *  License along with this library; if not, write to the Free
    *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    *
    *************************************************************************/

    #define BANDPASS       1
    #define DIFFERENTIATOR 2
    #define HILBERT        3

    #define NEGATIVE       0
    #define POSITIVE       1

//     #define Pi             3.1415926535897932
//     #define Pi2            6.2831853071795865

    #define GRIDDENSITY    16
    #define MAXITERATIONS  40

    /* Function prototype for remez() - the only function that should need be
    * called from external code
    */
    void remez(double h[], int numtaps,
            int numband, double bands[], double des[], double weight[],
            int type);

	std::vector<double> fir1_lowpass(int n, double cutoff);
	std::vector<double> fir1_highpass(int n, double cutoff);
	std::vector<double> fir1_bandpass(int n, double low_cutoff, double high_cutoff);

	/* Real-Time Filter
	*/
	class RTFilter
	{
	  public:
		virtual double operator()(double v)=0;

		virtual int getLength()=0;

		virtual ~RTFilter(){}
	};

	class FIRRTFilter : public RTFilter
	{
		std::vector<double> m_imp_res;
		std::deque<double> m_to_filter;

	  public:
		FIRRTFilter(){m_imp_res=std::vector<double>(1, 1.0);}
		FIRRTFilter(std::vector<double>& imp_res);
		void setImpulseResponse(std::vector<double> imp_res)	{m_imp_res=imp_res;}

		virtual int getLength()									{return m_imp_res.size();}

		virtual double operator()(double v);
	};

	class RectangularHighPassRTFilter : public RTFilter
	{
		int m_N;
		std::deque<double> m_summed_values;
		double m_sum;

	  public:
		RectangularHighPassRTFilter(int N=0);

		void reset(int N);
		virtual int getLength()									{return m_N;}

		virtual double operator()(double v);
	};
	class RectangularLowPassRTFilter : public RTFilter
	{
	  public:
		virtual double operator()(double v);
	};
	class RectangularBandPassRTFilter : public RTFilter
	{
	  public:
		virtual double operator()(double v);
	};

	class DummyRTFilter : public RTFilter
	{
	  public:
		DummyRTFilter()	{}

		virtual int getLength()									{return 1;}

		virtual double operator()(double v)	{return v;}
	};
}

#endif // _Filter_h_

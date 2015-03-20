// Copyright 2004 "Gilles Degottex"

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


#ifndef _Convolution_h_
#define _Convolution_h_

#include <vector>
#include <deque>
#include <complex>
using namespace std;

namespace Music
{
	//! do a convolution with a specfic wave for a desired note
	struct Convolution
	{
		//! the desired semi-tone from A3
		const double m_ht;
		//! the corresponding frequency
		const double m_freq;
		//! latency factor, used for statistical purpose (40.0)
		const double m_latency_factor;
		//! duration in seconds
		const double m_duration;

		//! the wave: the signal is convolued with
		vector< complex<double> > m_wave;

		//! computed formant
		complex<double> m_value;

		//! unique ctor
		/*!
		 * \param AFreq frequency of A3 (440.0)
		 * \param sampling_rate wave capture sampling rate (11khz;44khz)
		 * \param latency_factor latency factor [1;oo[
		 * \param gauss_factor the factor for the window fonction applied on the analysed sample (2.0)
		 * \param ht analysed semi-tone (-48;+48)
		 */
		Convolution(double latency_factor, double gauss_factor, double ht);

		//! return the size of the analyse (the algorithmical N)
		size_t size()	{return m_wave.size();}

		//! compute a convolution
		void apply(const deque<double>& buff, int start=0);
	};
}

#endif // _Convolution_h_

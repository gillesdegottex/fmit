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


#ifndef _CumulativeDiff_h_
#define _CumulativeDiff_h_

#include <deque>
using namespace std;

namespace Music
{
	//! do a Cumulative Diff with a specific wave-length of a desired note
	struct CumulativeDiff
	{
		//! the analysed semi-tone from A3
		const int m_ht;
		//! his corresponding frequency
		const double m_freq;
		//! the wave-length
		const size_t m_s;
		//! latency_factor
		double m_latency_factor;

		//! unique ctor
		/*!
		 * \param AFreq frequency of A3 (440.0)
		 * \param sampling_rate wave capture sampling rate (11khz;44khz)
		 * \param latency_factor latency factor [1;oo[
		 * \param ht analysed semi-tone (-48;+48)
		 */
		CumulativeDiff(double latency_factor, int ht);

		//! compute the error
		void receive(const deque<double>& buff, size_t start=0);

		//! computed error for the desired semi-tone (m_ht)
		double m_error;
		//!
		double m_best_s;
	};

	//! do a correlation with a specific range of wave-length around a desired note
	struct RangedCumulativeDiff
	{
		//! the analysed semi-tone from A3
		const int m_ht;
		//! his corresponding frequency
		const double m_freq;
		//! pitch tolerance for m_freq;
		const double m_pitch_tolerance;
		//! minimal to maximal scaned wave-length
		const size_t m_smin, m_smax;
		//! latency_factor
		const double m_latency_factor;

		//! unique ctor
		/*!
		 * \param AFreq frequency of A3 (440.0)
		 * \param sampling_rate wave capture sampling rate (11khz;44khz)
		 * \param latency_factor latency factor [1;oo[
		 * \param pitch_tolerance ]0;0.5]
		 * \param ht analysed semi-tone (-48;+48)
		 */
		RangedCumulativeDiff(double pitch_tolerance, double latency_factor, int ht);

		//! compute the error
		void receive(const deque<double>& buff, size_t start=0);

		//! computed error for the desired semi-tone (m_ht)
		double m_error;

		//! computed minimal error
		double m_min_error;

		//! the wave length returning the minimal error;
		int m_min_wave_length;

		static void GetMinWaveLength(double pitch_tolerance, int ht, double& error, double min_wave_length);
	};
}

#endif // _CumulativeDiff_h_

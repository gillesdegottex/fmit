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


#include "CumulativeDiffAlgo.h"

#include <cassert>
#include <cmath>
#include <deque>
#include <iostream>
#include <limits>
using namespace std;
#include <CppAddons/CAMath.h>
using namespace Math;

#include "Music.h"

//#define MUSIC_DEBUG
#ifdef MUSIC_DEBUG
#define LOG(a)	a
#else
#define LOG(a)
#endif

namespace Music
{
	void CumulativeDiffAlgo::init()
	{
		setMinMaxLength(int(GetSamplingRate()/h2f(GetSemitoneMax())), int(GetSamplingRate()/h2f(GetSemitoneMin())));
	}

	CumulativeDiffAlgo::CumulativeDiffAlgo(double noise_treshold)
	: m_noise_threshold(noise_treshold)
	, m_wave_length(0)
	{
		init();
	}

	//! return the average differance on the sample delimited by [0,size]
	// - ne pas utiliser tout size
	// - sauter des données
	double diff(const deque<double>& buff, size_t size, size_t s)
	{
		double r = 0.0;
		for(size_t i=0; i<size; i++)
			r += abs(buff[i] - buff[i+s]);
		return r / size;
	}

	void CumulativeDiffAlgo::apply(const deque<double>& buff)
	{
		if(buff.size()<2*m_max_length)
		{
			m_wave_length = 0;
			return;
		}

		double max_vol = 0.0;
		for(size_t i=0; i<m_max_length; i++)
			max_vol = max(max_vol, buff[i]);

		// use a relative threshold
		double threshold = m_noise_threshold*max_vol;

//	cerr << "min=" << min_length << " max=" << m_max_length << " threshold=" << treshold << endl;

		double r = 0.0;
		size_t s;
		for(s=m_min_length; r<=threshold && s<m_max_length; s++)
			r = diff(buff, m_max_length, s);

		while((r=diff(buff, m_max_length, s+1))>threshold && s<m_max_length)
			s++;

//	cerr << "s=" << s << " r=" << r << endl;
		double old_r = r;
		while(s+1<m_max_length && (r=diff(buff, m_max_length, s+1))<old_r)
		{
//	cerr << "s=" << s << " r=" << r << endl;
			s++;
			old_r = r;
		}

//	cerr << "s=" << s << " r=" << old_r << endl;

//	cerr << "absolute threshold=" << m_noise_threshold << " max volume="<<max_vol<<" relative threshold="<<threshold << " s="<<s << " m_max_length="<<m_max_length << endl;

		m_wave_length = (s<m_max_length)?s:0;
	}

/*
 * ~YIN version, unused because too costly for the CPU: O(n^3) ...
	double CumulativeDiffAlgo::squar_diff(const deque<double>& buff, size_t s)
	{
		double r = 0.0;
		for(size_t i=0; i<s; i++)
		{
			double d = buff[i] - buff[i+s];
			r += d*d;
		}

		return r;
	}
	double CumulativeDiffAlgo::norm_squar_diff(const deque<double>& buff, size_t s)
	{
		double r = 0.0;
		for(size_t i=0; i<s; i++)
			r += squar_diff(buff, i);

		return squar_diff(buff, s) / (r/s);
	}
	void CumulativeDiffAlgo::receive(const deque<double>& buff)
	{
		assert(m_sampling_rate>0);
		size_t min_length = size_t(m_sampling_rate * m_min_wave_length);
		size_t max_length = size_t(m_sampling_rate * m_max_wave_length);

		if(buff.size()<2*max_length)	return;

	cerr << "min=" << min_length << " max=" << max_length << " noise=" << m_noise_treshold << endl;

		double r = 1.0;
		double max_vol = 0.0;
		size_t s;
		for(s=min_length; r>m_noise_treshold && s<max_length; s++)
		{
			max_vol = max(max_vol, buff[s]);

			r = norm_squar_diff(buff, s);
//	cerr << "s=" << s << " r=" << r << endl;
		}
		s--;
	cerr << "s=" << s << " r=" << r << endl;
//	cerr << "---" << endl;
		double old_r;
		do
		{
			s++;
			old_r = r;

			r = norm_squar_diff(buff, s);
//	cerr << "s=" << s << " r=" << r << endl;
		}
		while(r<old_r && s<max_length);

		r = old_r;
		s--;
	cerr << "s=" << s << " r=" << r << endl;

	cerr << "m_noise_treshold=" << m_noise_treshold << " max_vol=" << max_vol << " r=" << r << endl;

		m_freq = 0.0;
		if(max_vol>m_noise_treshold && s<max_length)
		{
						cerr << "r=" << r << " m_noise_treshold=" << m_noise_treshold << endl;
			m_freq = double(m_sampling_rate)/s;
		}

	cerr << "m_sampling_rate=" << m_sampling_rate << " freq=" << m_freq << endl;
	}
*/
}


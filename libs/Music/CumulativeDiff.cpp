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


#include "CumulativeDiff.h"

#include <iostream>
#include "Music.h"
using namespace Music;

CumulativeDiff::CumulativeDiff(double latency_factor, int ht)
: m_ht(ht)
, m_freq(h2f(m_ht))
, m_s(size_t(GetSamplingRate()/m_freq))
, m_latency_factor(latency_factor)
{
	m_error = 0.0;

//	cout << "CumulativeDiff::CumulativeDiff ht=" << ht << " AFreq="<<GetAFreq() << " freq=" << m_freq << endl;
}

void CumulativeDiff::receive(const deque<double>& buff, size_t start)
{
	if(buff.size()<start+(m_latency_factor+1)*m_s)	return;

	m_error = 0.0;

	for(size_t i=0; i<m_latency_factor*m_s; i++)
		m_error += abs(buff[start+i] - buff[start+i+m_s]);
}

RangedCumulativeDiff::RangedCumulativeDiff(double pitch_tolerance, double latency_factor, int ht)
: m_ht(ht)
, m_freq(h2f(m_ht))
, m_pitch_tolerance(pitch_tolerance)
, m_smin(size_t(GetSamplingRate()/(m_freq + m_pitch_tolerance*(h2f(m_ht+1)-m_freq))))
, m_smax(size_t(GetSamplingRate()/(m_freq + m_pitch_tolerance*(h2f(m_ht-1)-m_freq))))
, m_latency_factor(latency_factor)
{
	m_error = 0.0;

//	cout << "CumulativeDiff::CumulativeDiff ht=" << ht << " pitch_tol=" << m_pitch_tolerance << " seek_factor=" << m_seek_factor << " s=" << m_s << " smin=" << m_smin << " smax=" << m_smax << endl;
}

void RangedCumulativeDiff::receive(const deque<double>& buff, size_t start)
{
	if(buff.size()<start+m_latency_factor*m_smax+m_smax)	return;

	m_error = 0.0;
	m_min_error = 1000.0;
//	int m=0;
	for(size_t s=m_smin; s<=m_smax; s++)
	{
		double err = 0.0;
//		size_t n = 0;

		for(size_t i=0; i<m_latency_factor*s; i++)
		{
			err += abs(buff[start+i] - buff[start+i+s]);
//			n++;
		}

//		err /= n;

		if(err<m_min_error)
		{
			m_min_wave_length = s;
			m_min_error = err;
		}
//		m_error = max(m_error, err);
		m_error = m_min_error;
//		cout << m_error << endl;
//		m_error += err;
//		m++;
	}

//	m_error /= m;
}

void RangedCumulativeDiff::GetMinWaveLength(double pitch_tolerance, int ht, double& error, double min_wave_length)
{
    (void)pitch_tolerance;
    (void)ht;
    (void)error;
    (void)min_wave_length;
}


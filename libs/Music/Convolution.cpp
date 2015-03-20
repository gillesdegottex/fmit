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


#include "Convolution.h"
#include <iostream>
using namespace std;
#include <CppAddons/CAMath.h>
#include "Music.h"
#include "SPWindow.h"

namespace Music
{
	Convolution::Convolution(double latency_factor, double gauss_factor, double ht)
	: m_ht(ht)
	, m_freq(h2f(m_ht))
	, m_latency_factor(latency_factor)
	, m_duration(m_latency_factor/m_freq)
	, m_wave(int(m_duration*GetSamplingRate()))
	{
//		cerr << "Convolution::Convolution " << ht << endl;
		double c = - 2.0*Math::Pi * m_freq / GetSamplingRate();

		double u = Usefull(Win_Sinc(gauss_factor));

		for(size_t j=0; j<m_wave.size(); j++)
			m_wave[j] = exp(complex<double>(0.0, c*j))*double(2.0/m_wave.size()) * win_sinc(j/double(m_wave.size()), gauss_factor)/u;
		// 		m_wave[j] = exp(complex<double>(0.0, c*j))*double(2.0*Math::Pi/m_wave.size()) * win_sinc(j/double(m_wave.size()), win_factor)/u;
	}

	void Convolution::apply(const deque<double>& buff, int start)
	{
		m_value = complex<double>(0.0,0.0);

		if(buff.size()-start >= m_wave.size())
		{
			for(size_t i=0; i<m_wave.size(); i++)
				m_value += m_wave[i]*buff[i+start];
		}
	}
}

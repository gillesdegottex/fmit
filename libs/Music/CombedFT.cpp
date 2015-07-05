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

#include "CombedFT.h"

#include <assert.h>
#include <iostream>
using namespace std;
using namespace Math;
#include "Music.h"
#include "SPWindow.h"
#include "FreqAnalysis.h"

#include <QTextStream>

namespace Music
{
	CombedFT::CombedFT()
	{
		m_use_audibility_treshold = false;
		m_audib_ratio = 0.1;

		m_zp_factor = 1.0;
		m_window_factor = 1.0;

		init();
	}
	void CombedFT::setZeroPaddingFactor(double zp)
	{
		if(zp!=m_zp_factor)
		{
			m_zp_factor = zp;
			init();
		}
	}
	void CombedFT::setWindowFactor(double wf)
	{
		if(wf!=m_window_factor)
		{
			m_window_factor = wf;
			init();
		}
	}

	void CombedFT::init()
	{
		if(GetSamplingRate()<=0)	return;

		m_f0 = 0.0;

		int win_size = int(m_window_factor*GetSamplingRate()/h2f(GetSemitoneMin()));// at least m_window_factor period of the lowest freq

		int best_size = 2;
		while(best_size<int(win_size))
			best_size *= 2;

		m_win = hann(best_size);

		while(best_size<int(m_zp_factor*win_size))
			best_size *= 2;

        cout << "CombedFT: INFO: window size=" << win_size << " FFT size=" << best_size << " window size factor=" << m_window_factor << " zero padding factor=" << m_zp_factor << endl;

		m_plan.resize(best_size);
		m_components.resize(m_plan.size()/2);
		m_comb.resize(m_plan.size()/2);
	}
	int CombedFT::getSampleAlgoLatency() const
	{
		return 1000*m_win.size()/GetSamplingRate();
	}
	int CombedFT::getMinSize() const
	{
		return m_win.size();
	}
	double CombedFT::getFondamentalFreq() const
	{
		return m_f0;
	}

	void CombedFT::apply(const deque<double>& buff)
	{
// 		cout << getAmplitudeTreshold() << " " << getComponentTreshold() << " " << m_audib_ratio << endl;

		if(int(buff.size())<getMinSize() || m_win.empty())	return;

		m_max_amplitude = 0.0;
		for(size_t i=0; i<m_win.size(); i++)
		{
			m_plan.in[i] = buff[i]*m_win[i];

			if(abs(buff[i])>m_max_amplitude)
				m_max_amplitude = abs(buff[i]);
		}
		for(int i=m_win.size(); i<m_plan.size(); i++)			// padd with zeros
			m_plan.in[i] = 0.0;

		m_plan.execute();

		for(size_t i=0; i<m_comb.in.size() && i<m_plan.out.size(); i++)
			m_comb.in[i] = mod(m_plan.out[i]);

		// compute max with respect of the bounds
		m_components_max = 0.0;
		int max_index = -1;
		double fmin = h2f(GetSemitoneMin());
		double fmax = h2f(GetSemitoneMax());
		for(size_t i=0; i<m_components.size(); i++)
		{
			double fi = i*double(GetSamplingRate())/m_plan.size();
			if(fmin<=fi && fi<=fmax
						&& m_comb.in[i]>m_components_max)
			{
				max_index = i;
				m_components_max = m_comb.in[i];
			}
		}

		m_f0 = 0.0;

		if(m_components_max>getComponentTreshold())
		{
			m_f0 = PeakRefinementLogParabola(m_plan.out, max_index)*double(GetSamplingRate())/m_plan.size();

			// TODO TEST *win[i]; // me semble qu'une trans de harm signal n'est pas trop discontinue aux extrémités
			m_comb.execute();

			for(size_t i=0; i<m_components.size(); i++)
				m_components[i] = 0.0;

			for(size_t i=0; i<m_comb.out.size()/2; i++)
				m_components[i] = real(m_comb.out[i]);

			// avg is not interesting
			m_components[0] = 0.0;

			// frequencies between 1 and g_res_factor are virtuals
// 			for(int i=m_comb.out.size()-(g_res_factor-1); i<m_comb.out.size(); i++)
// 				m_components[i] = 0.0;

			double step = (m_plan.size()/max_index)/2;
			// hyp: the fund freq is not greater than the max amplitude harmonic
 			// keep only multiples of the max amplitude harmonic
			for(int i=1; i<int(m_components.size())/step; i++)
				m_components[i] = m_components[int(step*i)];
			for(int i=int(m_components.size()/step); i<int(m_components.size()); i++)
				m_components[i] = 0.0;

			if(m_use_audibility_treshold)
			{
				for(int i=1; i<int(m_components.size()/step); i++)
					m_components[i] /= (1+m_audib_ratio*(i-1));
			}
			else
			{
				vector<double> temp_comp(int(m_components.size()/step), 0.0);
				for(int i=1; i<int(temp_comp.size()); i++)
					temp_comp[i] =	(m_components[i]-m_components[i-1]) +
									(m_components[i]-m_components[i+1]);
				for(int i=1; i<int(temp_comp.size()); i++)
					m_components[i] = temp_comp[i];
			}

			// find the max
			double max_amp = 0.0;
			max_index = -1;
			for(size_t i=0; i<m_components.size(); i++)
			{
// 				double fi = i*double(GetSamplingRate())/m_win.size();
				if(m_components[i]>max_amp)
				{
					max_index = i;
					max_amp = m_components[i];
				}
			}

			if(max_index>0)
	 			m_f0 /= max_index;
		}

// 		cout << " final: " << GetSamplingRate() << ":" << m_f0 << endl;
	}
	CombedFT::~CombedFT()
	{
	}
}

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


#include "MultiCumulativeDiffAlgo.h"

#include <cassert>
#include <cmath>
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
	void MultiCumulativeDiffAlgo::init()
	{
		for(size_t i=0; i<size(); i++)
		{
			if(m_diffs[i]!=NULL)	delete m_diffs[i];

			m_diffs[i] = new CumulativeDiff(1, i+GetSemitoneMin());
		}
	}

	MultiCumulativeDiffAlgo::MultiCumulativeDiffAlgo(int latency_factor, double test_complexity)
	{
		assert(GetSamplingRate()>0);

		m_test_complexity = test_complexity;

		m_diffs.resize(size());
		for(size_t i=0; i<size(); i++)
			m_diffs[i] = NULL;
		init();
	}
	bool MultiCumulativeDiffAlgo::is_minima(int ih)
	{
		if(ih+1>=0 && ih+1<int(size()))
			if(m_components[ih+1]<=m_components[ih])
				return false;
		if(ih-1>=0 && ih-1<int(size()))
			if(m_components[ih-1]<=m_components[ih])
				return false;
		return true;
	}
	double MultiCumulativeDiffAlgo::getFondamentalWaveLength() const
	{
		return int(GetSamplingRate()/h2f(m_first_fond+GetSemitoneMin(), GetAFreq()));
	}
	void MultiCumulativeDiffAlgo::apply(const deque<double>& buff)
	{
		assert(GetSamplingRate()>0);
		for(size_t i=0; i<size(); i++)
		{
			m_components[i] = 1.0;
			m_is_fondamental[i] = false;
		}

		m_first_fond = -1;

		if(buff.empty() || buff.size()<(m_test_complexity+1)*m_diffs[0]->m_s)
			return;

		double v = 0.0;
		for(size_t i=0; i<buff.size() && v<=getAmplitudeTreshold() && i<m_diffs[0]->m_s; i++)
			v = max(v, abs(buff[i]));

		if(v>getAmplitudeTreshold())
		{
			// compute all components
			m_components_max = 0.0;
			double min_comp = 1000000;
			double max_sum = 0.0;
			for(int ih=int(size())-1; ih>=0; ih--)
			{
				m_diffs[ih]->receive(buff, 0);
				m_components[ih] = m_diffs[ih]->m_error;
				m_components_max = max(m_components_max, m_components[ih]);
			}

			// test components
			for(int ih=int(size())-1; ih>=0; ih--)
			{
				bool ok = true;

				bool crit_min = true;
				// criteria: the fond and his first harmonics are minimas
				if(ok)	ok =
							is_minima(ih) &&
							is_minima(ih-12) &&
							(is_minima(ih-19) || is_minima(ih-19-1) || is_minima(ih-19+1)) &&
							is_minima(ih-24);

				crit_min = ok;

				bool crit_small_enough = true;
				if(ok)
				{
					if(m_components[ih]/m_components_max>getComponentTreshold())
						crit_small_enough = false;
					ok = crit_small_enough;
				}

//				bool crit_cross_zero = true;
				/*
				// criteria: wave should cross the zero value
				if(ok)
				{
					bool cross = true;
					for(int s=0; cross && s<int(m_diffs[ih]->m_s); s++)
					{
						double sg = Math::sgn(buff[s]);
						bool same_side = true;
						for(int i=0; same_side && i<int(m_diffs[ih]->m_s); i++)
							same_side = Math::sgn(buff[s+i])==sg;

						cross = !same_side;
					}

					ok = crit_cross_zero = cross;
				}
				*/

//				bool crit_integ_cst = true;
				// criteria: integral should be nearly constant while shifting
				// TODO
				// (the previous criteria seems sufficient to remove high comp.)

//				LOG(if(crit_min)
//					cerr << "ih=" << ih <<
//					" harm_min=(("<<is_minima(ih-12)<<","<<is_minima(ih-12-1)<<","<<is_minima(ih-12+1)<<"),("<<
//						is_minima(ih-19)<<","<<is_minima(ih-19-1)<<","<<is_minima(ih-19+1)<<"),("<<
//						is_minima(ih-24)<<","<<is_minima(ih-24-1)<<","<<is_minima(ih-24+1)<<"))"<< crit_min <<
//					" increase=" << crit_incr_err <<
//					" cross_zero=" << crit_cross_zero <<
//					" integ_cst=" << crit_integ_cst <<
//					" ok=" << ok << " c=" << m_components[ih] << endl;)

				// if all criteria are ok
				if(ok)
				{
					double sum = 0.0;
					int n=0;
					int i=0;
					double wh = 1.0;
					sum += wh*(m_components_max-m_components[ih]); n++;
					i=12;	if(ih-i>=0)	{sum+=wh*(m_components_max-m_components[ih-i]);	n++;}
					i=19;	if(ih-i>=0)	{sum+=wh*(m_components_max-m_components[ih-i]);	n++;}
					i=24;	if(ih-i>=0)	{sum+=wh*(m_components_max-m_components[ih-i]);	n++;}

					LOG(cerr << "ih=" << ih << " sum=" << sum << endl;)

					// get the "best"
					if(sum>max_sum)
					{
						size_t step = size_t(m_diffs[ih]->m_s/m_test_complexity);
						if(step<1)	step = 1;
						for(size_t s=0; ok && s<m_diffs[ih]->m_s; s+=step)
						{
							if(ih-1>=0){
								m_diffs[ih-1]->receive(buff, s);
								m_components[ih-1] = m_diffs[ih-1]->m_error;
							}
							if(ih+1<int(size())){
								m_diffs[ih+1]->receive(buff, s);
								m_components[ih+1] = m_diffs[ih+1]->m_error;
							}
							m_diffs[ih]->receive(buff, s);
							m_components[ih] = m_diffs[ih]->m_error;
							ok = is_minima(ih);
						}

						if(ok)
						{
							max_sum = sum;
							min_comp = m_components[ih];
							m_first_fond = ih;
						}
					}
				}
			}

// 			cerr << "ff: " << m_first_fond << endl;

			if(m_first_fond!=-1)
				m_is_fondamental[m_first_fond] = true;

			LOG(cerr << "m_first_fond=" << m_first_fond << endl;)
		}
	}
	MultiCumulativeDiffAlgo::~MultiCumulativeDiffAlgo()
	{
		for(size_t i=0; i<m_diffs.size(); i++)
			delete m_diffs[i];
	}
}

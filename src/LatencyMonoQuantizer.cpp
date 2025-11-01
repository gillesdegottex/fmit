// Copyright 2005 "Gilles Degottex"

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


#include "MonoQuantizer.h"

#include <iostream>
#include <map>
using namespace std;
#include <Music/Music.h>
using namespace Music;
#include "LatencyMonoQuantizer.h"

LatencyMonoQuantizer::LatencyMonoQuantizer()
{
	m_latency = 125;	// ms

	m_time.start();
}

void LatencyMonoQuantizer::quantize(double freq)
{
// 	cout << "LatencyMonoQuantizer::quantize " << freq << " m_states=" << m_states.size() << endl;

	double current_time = m_time.elapsed();

	m_states.push_front(State(current_time, freq));

	double current_center_freq = f2cf(freq);
	int max_dens = 1;
	m_avg_freq = freq;
	map<double,Note> densities;
    for(int i=0; i<int(m_states.size()); i++)
	{
		double cf = f2cf(m_states[i].m_freq);
		map<double,Note>::iterator it=densities.find(cf);
		if(it==densities.end())
			densities.insert(make_pair(cf,Note(m_states[i].m_freq)));
		else
		{
			it->second.m_avg_freq *= it->second.m_dens;
			it->second.m_avg_freq += m_states[i].m_freq;
			it->second.m_dens++;
			it->second.m_avg_freq /= it->second.m_dens;
			if(it->second.m_dens>max_dens)
			{
				max_dens = it->second.m_dens;
				current_center_freq = it->first;
				m_avg_freq = it->second.m_avg_freq;
			}
		}
	}

// 	cout << m_current_center_freq << ", " << current_center_freq << ", " << m_avg_freq << ", max_dens=" << max_dens << ", m_states.size()=" << m_states.size() << endl;

	m_confidence = double(max_dens)/m_states.size();

	// if a density is strong enough
	if(m_confidence>m_min_confidence)
	{
		if(m_current_center_freq==0.0)
		{
			if(current_center_freq>0.0)
			{
				m_current_center_freq = current_center_freq;
				// m_duration.start();
				double lag = (current_time-m_states.back().m_time)*m_min_confidence;
				// m_duration.addMSecs(int(lag));
				emit(noteStarted(m_current_center_freq, -lag));
			}
		}
		else
		{
			if(current_center_freq==0.0)
			{
				m_current_center_freq = current_center_freq;
				double lag = (current_time-m_states.back().m_time)*m_min_confidence;
				emit(noteFinished(m_current_center_freq, -lag));
			}
			else if(m_current_center_freq != current_center_freq)
			{
				double lag = (current_time-m_states.back().m_time)/2.0;	// TODO pas forcément a fait 2 ~bruit
				emit(noteFinished(m_current_center_freq, -lag));
				m_current_center_freq = current_center_freq;
				// m_duration.start();
				// m_duration.addMSecs(int(lag));
				emit(noteStarted(m_current_center_freq, -lag));
			}
		}
	}
	else if(m_current_center_freq>0.0)
	{
		m_current_center_freq = 0.0;
		double lag = (current_time-m_states.back().m_time)*m_min_confidence;
		emit(noteFinished(m_current_center_freq, -lag));
	}

	while(!m_states.empty() && (current_time-m_states.back().m_time>m_latency))
		m_states.pop_back();
}

void LatencyMonoQuantizer::reset()
{
	quantize(0.0);
	m_avg_freq = 0.0;
	m_states.clear();
	m_confidence = 1.0;
}

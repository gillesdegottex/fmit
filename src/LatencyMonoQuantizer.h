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


#ifndef _LATENCYMONOQUANTIZER_H_
#define _LATENCYMONOQUANTIZER_H_

#include <deque>
using namespace std;
#include <qobject.h>
#include <qdatetime.h>
#include <Music/Music.h>
using namespace Music;
#include "MonoQuantizer.h"

class LatencyMonoQuantizer : public MonoQuantizer
{
	QTime m_time;

	//! in millis
	float m_latency;

	struct State{
		double m_time;
		double m_freq;
		double m_confidence;
		State(double t, double f, double c=1.0) : m_time(t), m_freq(f), m_confidence(c) {}
	};
	deque<State> m_states;

	struct Note{
		int m_dens;
		double m_avg_freq;
		Note(double f) : m_dens(1), m_avg_freq(f) {}
	};

	QTime m_duration;

  public:
	LatencyMonoQuantizer();

	void setLatency(float latency)					{m_latency=latency;}
	double getLatency()								{return m_latency;}

	virtual void quantize(double freq);
	virtual void reset();

	virtual ~LatencyMonoQuantizer(){}
};

#endif // _LATENCYMONOQUANTIZER_H_

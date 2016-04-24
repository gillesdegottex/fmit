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
using namespace std;
#include <Music/Music.h>
using namespace Music;
#include "DummyMonoQuantizer.h"

DummyMonoQuantizer::DummyMonoQuantizer()
{
	m_confidence = 1.0;
}

void DummyMonoQuantizer::quantize(double freq)
{
// 	cout << "DummyMonoQuantizer::quantize " << freq << endl;

//	double current_time = m_time.elapsed();

	double current_center_freq = h2f(f2h(freq));
	m_avg_freq = freq;

	if(m_current_center_freq==0.0)
	{
		if(current_center_freq>0.0)
		{
			m_current_center_freq = current_center_freq;
			m_duration.start();
			emit(noteStarted(m_current_center_freq, 0.0));
		}
	}
	else
	{
		if(current_center_freq==0.0)
		{
			m_current_center_freq = current_center_freq;
			emit(noteFinished(m_current_center_freq, 0.0));
		}
		else if(m_current_center_freq != current_center_freq)
		{
			emit(noteFinished(m_current_center_freq, 0.0));
			m_current_center_freq = current_center_freq;
			m_duration.start();
			emit(noteStarted(m_current_center_freq, 0.0));
		}
	}
}

void DummyMonoQuantizer::reset()
{
	quantize(0.0);
	m_avg_freq = 0.0;
}

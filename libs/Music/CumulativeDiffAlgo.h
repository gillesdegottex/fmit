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


#ifndef _CumulativeDiffAlgo_h_
#define _CumulativeDiffAlgo_h_

#include <vector>
#include <deque>
#include <complex>
using namespace std;
#include "Algorithm.h"
#include "CumulativeDiff.h"

namespace Music
{
	//! Compute period with a "classical" auto-correlation algorithm
	class CumulativeDiffAlgo : public Algorithm
	{
	  protected:
		double m_noise_threshold;

		size_t m_min_length;
		size_t m_max_length;

		size_t m_wave_length;

		void init();
		virtual void AFreqChanged()							{init();}
		virtual void samplingRateChanged()					{init();}
		virtual void semitoneBoundsChanged()				{init();}

	  public:
		CumulativeDiffAlgo(double noise_treshold);

		virtual int getSampleAlgoLatency() const		{return 2*m_max_length;}

		double getNoiseThreshold() const				{return m_noise_threshold;}
		void setNoiseThreshold(double noise_threshold)	{m_noise_threshold=noise_threshold;}
		void setMinMaxLength(size_t min_length, size_t max_length)
										{m_min_length=min_length; m_max_length=max_length;}

		void apply(const deque<double>& buff);

		virtual bool hasNoteRecognized() const			{return m_wave_length>0;}
		virtual double getFondamentalWaveLength() const	{return m_wave_length;}

		virtual ~CumulativeDiffAlgo(){}
	};
}

#endif

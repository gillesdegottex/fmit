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


#ifndef _MultiCumulativeDiffAlgo_h_
#define _MultiCumulativeDiffAlgo_h_

#include <vector>
#include <deque>
using namespace std;
#include "Algorithm.h"
#include "CumulativeDiff.h"

namespace Music
{
	//! Compute error for each possible freqs with correlation algorithm
	class MultiCumulativeDiffAlgo : public Transform
	{
		double m_test_complexity;
		int m_first_fond;
		vector<bool> m_is_fondamental;

	  protected:
		void init();
		virtual void AFreqChanged()							{init();}
		virtual void samplingRateChanged()					{init();}
		virtual void semitoneBoundsChanged()				{init();}

	  public:
		//! correlation filters
		vector< CumulativeDiff* > m_diffs;

		bool is_minima(int ih);

	  public:
		double m_pitch_tolerance;

		void setTestComplexity(double test_complexity)		{m_test_complexity = test_complexity;}
		double getTestComplexity()							{return m_test_complexity;}

		virtual int getSampleAlgoLatency() const			{return int((getAlgoLatency()/1000.0)*GetSamplingRate());}
		//! in millis
		virtual double getAlgoLatency() const				{return 1000.0*(2*m_diffs[0]->m_s)/GetSamplingRate();}

		//! unique ctor
		/*!
		 * \param latency_factor latency factor for statistical purpose [1;oo[
		 * \param test_complexity
		*/
		MultiCumulativeDiffAlgo(int latency_factor, double test_complexity);

		//! overwrited computing function
		virtual void apply(const deque<double>& buff);

		virtual double getFondamentalWaveLength() const;

		virtual ~MultiCumulativeDiffAlgo();
	};
}

#endif

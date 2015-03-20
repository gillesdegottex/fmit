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


#ifndef _TimeAnalysis_h_
#define _TimeAnalysis_h_

#include <vector>
#include <deque>
#include <map>
#include <list>
#include <complex>
using namespace std;
#include "Algorithm.h"
#include "CumulativeDiff.h"

namespace Music
{
	//! Seek for the period (relative to sampling rate)
	double GetAverageWaveLengthFromApproxEnergy(const std::deque<double>& queue, double approx, int n);

	//! Seek for the period (relative to sampling rate)
	double GetAveragePeriodFromApprox(const std::deque<double>& queue, int approx, int n);

	//! Get a sample of the wave form (relative to sampling rate)
	void GetWaveSample(const std::deque<double>& queue, size_t wave_length, std::deque<double>& sample);
}

#endif // _TimeAnalysis_h_


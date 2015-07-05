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


#include "TimeAnalysis.h"

#include <cassert>
#include <cmath>
#include <deque>
#include <iostream>
#include <limits>
using namespace std;
#include <CppAddons/CAMath.h>
using namespace Math;

#include "Music.h"

namespace Music
{
	double InterpolatedPeriod(const std::deque<double>& queue, int left, int right)
	{
		double l = left - queue[left]/(queue[left+1]-queue[left]);

		double r = right - queue[right]/(queue[right+1]-queue[right]);

		return r - l;
	}

	/*
	 * on peut imaginer des cas qui mettent en échec cette procédure:
	 *	on selectionne un zéro qui n'en n'est pas un une periode plus
	 *	tard et si un autre zéro se trouve dans la zone de tolérance la longeur
	 *	ainsi calculée entre ces deux zéro (qui ne se correspondent donc pas) sera fausse.
	 *	example: une fréquence très basse avec une seule harmonique très très
	 *	haute.
	 *	- il faut utiliser des zéros significatifs ... et ... et ... et voil� .
	 *	- ou encore �carter les solutions trop �logn�es de la moyenne
	 */
	double GetAveragePeriodFromApprox(const std::deque<double>& queue, int approx, int n)
	{
		if(GetAFreq()<=0.0 || GetSamplingRate()<=0.0 || int(queue.size())<approx)
			return 0.0;

		deque<int> ups;									// the upper peeks

		// parse the whole buffer, for n zeros
		for(int i=0; int(ups.size())<n && i+1<int(queue.size()); i++)
			if(queue[i]<0 && queue[i+1]>0)				// if it cross the axis
				ups.push_back(i);

// 		cout << "approx=" << approx << " ups.size()=" << ups.size();
		if(ups.empty())
			return 0.0;

		double ht = f2hf(double(GetSamplingRate())/approx);
		int period_low_bound = int(GetSamplingRate()/h2f(ht+1))-2;
		int period_high_bound = int(GetSamplingRate()/h2f(ht-1))+2;

// 		cout << " ht=" << ht << " lb=" << period_low_bound << " rb=" << period_high_bound;

// 		cout << " periods=(";

		double period = 0.0;
		int count = 0;

		for(int i=0; i<int(ups.size()) && count<n; i++)
		{
			int i_seek = ups[i] + approx;

			int lower_i_seek = i_seek;
			int low_bound = ups[i] + period_low_bound;
			int higher_i_seek = i_seek;
			int high_bound = min(int(queue.size())-1, ups[i]+period_high_bound);

// 			cout << "{" << low_bound << ":" << i_seek << ":" << high_bound << "}";

			if(low_bound+1>=int(queue.size()))
				i = ups.size();										// stop loop
			else
			{
				if(!(queue[i_seek]<=0.0 && queue[i_seek+1]>0.0))
				{
					while(lower_i_seek>low_bound &&
						!(queue[lower_i_seek]<=0.0 && queue[lower_i_seek+1]>0.0))
						lower_i_seek--;

					while(higher_i_seek<high_bound &&
						!(queue[higher_i_seek]<=0.0 && queue[higher_i_seek+1]>0.0))
						higher_i_seek++;

					if(i_seek-lower_i_seek < higher_i_seek-i_seek)	// take the nearest to i_seek
						i_seek = lower_i_seek;
					else
						i_seek = higher_i_seek;
				}

// 				cout << i_seek << "=>";

				if(low_bound<i_seek && i_seek<high_bound)
				{
					double per = InterpolatedPeriod(queue, ups[i], i_seek);

// 					cout << "f=" << GetSamplingRate()/per << " ";

					period += per;
					count++;
				}
			}
		}

		if(count==0)
			return 0.0;

		period /= count;

// 		cout << ")=" << GetSamplingRate()/period << "(" << count << ")" << endl;

		return period;
	}

	void GetWaveSample(const std::deque<double>& queue, size_t wave_length, std::deque<double>& sample)
	{
		assert(wave_length>0);
		if(queue.size()<2*wave_length)	return;

		// find the highest peek in the second period
		int left = 0;
		double min_vol = 0;
		for(int i=int(wave_length); i<int(queue.size()) && i<int(2*wave_length); i++)
		{
			if(queue[i]<min_vol)
			{
				min_vol = queue[i];
				left = i;
			}
		}

		// adjust the right bound to the nearest minima
		int left_right = int(left + 0.9*wave_length);
		int right_right = int(left + wave_length/0.9);

		int right = int(left + wave_length);				// init to a default value

		if(right_right>=int(queue.size()))
			return;

		min_vol = 0.0;
		for(int i=left_right; i<=right_right; i++)
		{
			if(queue[i]<min_vol)
			{
				min_vol = queue[i];
				right = i;
			}
		}

		// fill in the sample
		sample.clear();
		for(int i=left; i<int(queue.size()) && i<right; i++)
			sample.push_back(queue[i]);
	}

	double GetAverageWaveLengthFromApproxEnergy(const std::deque<double>& queue, double approx, int n)
	{
		assert(GetSamplingRate()>0);

		if(queue.size()<approx*1.5)
			return 0.0;

// 		cout << queue.size() << "=>" << approx << " n=" << n << endl;

		double wave_length = 0.0;
		int count = 0;
		int seek = 0;
		while(count<n && seek<int(queue.size()) && seek!=-1)
		{
// 			cout << "new " << flush;

			// in one period, compute the energy over approx/4
			int w = int(approx/4);		// TODO ptr un peu long
			if(w<4) w=4;
			vector<double> en;
			for(int i=0; i<w/2; i++)
				en.push_back(0.0);
			for(int i=w/2; int(en.size())<int(1.25*approx); i++)
			{
				en.push_back(0.0);
				for(int j=-w/2; j<=w/2 && seek+i+j<int(queue.size()); j++)
					en.back() += queue[seek+i+j];	//*queue[seek+i+j]
			}

			// find the highest energy peak
			int i_max = 0;
			double en_max = 0.0;
			for(int i=0; i<int(en.size()); i++)
			{
				if(en[i]>en_max)
				{
					en_max = en[i];
					i_max = i;
				}
			}
			seek += i_max;

// 			cout << "max-seek=" << seek << " " << flush;

			int old_seek=seek;
			// go back to the previous zero
			while(seek>=0 && !(queue[seek]<=0 && queue[seek+1]>0) && seek>old_seek-approx/2)
				seek--;

// 			cout << "zero-seek=" << seek << " " << flush;

			if(seek<0 || seek<=old_seek-approx/2)
			{
				seek += int(approx);
// 				cout << endl;
				continue;
			}

			int left = seek;
			int right = int(left + approx);
			int sright = right;
			int downlimit = int(left + 0.75*approx);
			int bright = right;
			int uplimit = int(left + 1.25*approx);

			// look for the nearest zero of right
			while(sright+1<int(queue.size()) && sright>downlimit &&
						   !(queue[sright]<=0 && queue[sright+1]>0))
				sright--;
			while(bright+1<int(queue.size()) && bright<uplimit &&
						   !(queue[bright]<=0 && queue[bright+1]>0))
				bright++;

			if(sright>=int(queue.size()) || bright>=int(queue.size()))
			{
				seek = -1;
// 				cout << endl;
				continue;
			}

			double sw = InterpolatedPeriod(queue, left, sright);
			double bw = InterpolatedPeriod(queue, left, bright);
			// keep the nearest one after approx
			double wl = 0.0;
			if(abs(sw-approx)<abs(bw-approx))	wl = sw;
			else								wl = bw;

// 			cout << "wl=" << wl << flush;

			wave_length += wl;
			count++;

			seek += int(0.9*approx);

// 			cout << endl;
		}

// 		cout << "("<<count<<")"<< flush;

		if(count==0)	return 0.0;

		wave_length /= count;

// 		cout << GetSamplingRate()/wave_length << endl;

		return wave_length;
	}
}

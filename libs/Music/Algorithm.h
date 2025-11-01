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


#ifndef _Algorithm_h_
#define _Algorithm_h_

#include <list>
#include <vector>
#include <deque>
#include <iostream>
using namespace std;
#include "Music.h"

namespace Music
{
	class Algorithm : public SettingsListener
	{
		static list<Algorithm*> s_algos;

	  protected:
		double m_amplitude_treshold;
		double m_max_amplitude;

        virtual void samplingRateChanged()					{cout<<__FILE__<<":"<<long(__LINE__)<<" Algorithm::samplingRateChanged Not Yet Implemented"<<endl;}
        virtual void AFreqChanged()							{cout<<__FILE__<<":"<<long(__LINE__)<<" Algorithm::AFreqChanged Not Yet Implemented"<<endl;}
        virtual void semitoneBoundsChanged()				{cout<<__FILE__<<":"<<long(__LINE__)<<" Algorithm::semitoneBoundsChanged Not Yet Implemented"<<endl;}

	  public:
		Algorithm();

		inline double getAmplitudeTreshold()				{return m_amplitude_treshold;}
		inline void setAmplitudeTreshold(double t)			{m_amplitude_treshold=t;}
		inline double getMaxAmplitude()						{return m_max_amplitude;}

		virtual int getSampleAlgoLatency() const =0;
		virtual double getAlgoLatency() const				{return double(getSampleAlgoLatency())/GetSamplingRate();}

		virtual void apply(const deque<double>& buff)=0;
		virtual bool hasNoteRecognized() const =0;
		virtual double getFundamentalWaveLength() const		{return GetSamplingRate()/getFundamentalFreq();}
		virtual double getFundamentalFreq() const			{return GetSamplingRate()/getFundamentalWaveLength();}
		virtual double getFundamentalNote() const			{return f2hf(getFundamentalFreq());}
		virtual int getMinSize() const =0;

		virtual ~Algorithm();
	};

	class Transform : public Algorithm
	{
	  protected:
		double m_component_treshold;

		vector< complex<double> > m_harmonics;
		vector<double> m_components;
		double m_components_max;

	  public:
		inline double getComponentTreshold()				{return m_component_treshold;}
		inline void setComponentTreshold(double t)			{m_component_treshold=t;}

		Transform();

		size_t size() const									{return m_components.size();}

		const vector< complex<double> >& getHarmonics()		{return m_harmonics;}
		const vector<double>& getComponents()				{return m_components;}
		double getComponentsMax()							{return m_components_max;}

		virtual ~Transform(){}
	};
}

#endif // _Algorithms_h_


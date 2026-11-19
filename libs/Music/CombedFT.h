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


#ifndef _CombedFT_h_
#define _CombedFT_h_

#include <vector>
#include <deque>
#include <complex>
using namespace std;
#include <CppAddons/CAMath.h>

#include "Music.h"
#include "CFFTW3.h"
#include "Algorithm.h"

namespace Music
{
	/*! Combed FT for one voice
	 * Hypothesis: the highest energy peak takes part of the instrument sound
	 * O(2*nln(n))
	 */
	class CombedFT : public Transform
	{
		protected:
			vector<double> m_win;
			CFFTW3 m_comb;

			double m_f0;
			double m_audib_ratio;
			bool m_use_audibility_treshold;
			double m_zp_factor;
			double m_window_factor;
			bool m_force_fixed_note;
			int m_fixed_note;

			virtual void init();
			virtual void AFreqChanged()							{init();}
			virtual void samplingRateChanged()					{init();}
			virtual void semitoneBoundsChanged()				{init();}

		public:
			CombedFT();

			CFFTW3 m_plan;

			void setZeroPaddingFactor(double zp);
			double getZeroPaddingFactor() const					{return m_zp_factor;}

			void setWindowFactor(double wf);
			double getWindowFactor() const						{return m_window_factor;}

			/*! set audibility ratio of low harmonics against higher ones
			 * decrease influences of low harmonics in the evaluation of the fundamental.
			 * (avoid many too-low evaluation in high frequencies)
			 * Sometimes in high pitch notes (eg. G3 with bari-sax) a "disturbing" component appears
			 * at frequency f=G2 and so the fundamental is badly evaluated.
			 * By "disturbing" I mean: present in the spectral representation, but not audible or weak to much to change the impression of the pitch.
			 * Actually, that's a physiological setting, there is no "true" choice for a such situation.
			 * in [0, inf], 0 mean no correction, a good value seems to be 0.1
			 */
			void useAudibilityRatio(bool use)					{m_use_audibility_treshold=use;}
			void setAudibilityRatio(double audib_ratio=0.1)		{m_audib_ratio=audib_ratio;}
			double getAudibilityRatio()							{return m_audib_ratio;}

			void setForceFixedNote(bool force_fixed_note)       {m_force_fixed_note=force_fixed_note;}
			bool getForceFixedNote()                            {return m_force_fixed_note;}
			void setFixedNote(int fixed_note)                   {m_fixed_note=fixed_note;}
			int getFixedNote()                                  {return m_fixed_note;}

			virtual int getSampleAlgoLatency() const;
			virtual double getFondamentalFreq() const;
			virtual bool hasNoteRecognized() const				{return m_f0>0.0;}
			virtual int getMinSize() const;
			virtual void apply(const deque<double>& buff);

			~CombedFT();
	};
}

#endif

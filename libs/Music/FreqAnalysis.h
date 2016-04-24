// Copyright 2004-07 "Gilles Degottex"

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


#ifndef _FreqAnalysis_h_
#define _FreqAnalysis_h_

#include <vector>
#include <deque>
#include <complex>
#include <limits>
using namespace std;
#include <CppAddons/CAMath.h>
//#include <NeuralNet/mlp/LayeredNeuralNet.h>

#include "Music.h"
#include "Algorithm.h"
#include "Convolution.h"
#include <fftw3.h>

namespace Music
{
	struct Partial
	{
		double mod;
		double phase;
		double freq;		// Hz
		double noise_lvl;	// in [0,1]
	};
	struct Harmonic : Partial
	{
		int harm_number;
	};

	inline bool is_peak(double p1, double p2, double p3)
	{
		return p1<p2 && p2>p3;
	}
	inline bool is_peak(const std::vector<std::complex<double> > spectrum, int c)
	{
		assert(c>0 && c<int(spectrum.size())/2-1);

		return is_peak(Math::mod2(spectrum[c-1]), Math::mod2(spectrum[c]), Math::mod2(spectrum[c+1]));
	}

	double PeakRefinementLogParabola(const std::vector<std::complex<double> > spectrum, int peak_index);
	double PeakRefinementLogParabolaUnbiased(const std::vector<std::complex<double> > spectrum, int peak_index, double zp);

    std::vector<Harmonic> GetHarmonicStruct(const std::vector<std::complex<double> >& spectrum, double approx_f0, int nb_harm, double used_zp, double offset_tresh=0.1, bool pick_nonpeaks=false);
	double FundFreqRefinementOfHarmonicStruct(const std::vector<std::complex<double> >& spectrum, double approx_f0, int nb_harm, double used_zp);

	/*! the simpler: only one big convolution on the whole window
	 * O(N)
	 */
	class SingleResConvolutionTransform : public Transform
	{
	  protected:
		virtual void init();
		virtual void AFreqChanged()							{init();}
		virtual void samplingRateChanged()					{init();}
		virtual void semitoneBoundsChanged()				{init();}
		double m_latency_factor;
		double m_gauss_factor;

	  public:
		std::vector<Convolution*> m_convolutions;

		SingleResConvolutionTransform(double latency_factor, double gauss_factor);

		void setLatencyFactor(double latency)				{m_latency_factor=latency; init();}
		double getLatencyFactor()							{return m_latency_factor;}

		void setGaussFactor(double g)						{m_gauss_factor=g; init();}
		double getGaussFactor()								{return m_gauss_factor;}

		virtual void apply(const std::deque<double>& buff);

		virtual ~SingleResConvolutionTransform();
	};

	/*! extraction des fondamentales avec un r�aux de neurones
	 * entr�s avec la visualisation dans le plan de Gauss
	 */
	struct NeuralNetGaussAlgo : SingleResConvolutionTransform
	{
//		typedef Neuron TypeNeuron;
//		LayeredNeuralNet<TypeNeuron>* m_nn;

		virtual void init();

		NeuralNetGaussAlgo(double latency_factor, double gauss_factor);

		virtual int getSampleAlgoLatency() const {return 0;}

		virtual void apply(const deque<double>& buff);

		virtual ~NeuralNetGaussAlgo();
	};

	/*! Monophonic Algorithm: algo for one voice
	 * O(nbHT)
	 */
	class MonophonicAlgo : public SingleResConvolutionTransform
	{
	  protected:
		double m_dominant_treshold;

	  public:
		MonophonicAlgo(double latency_factor, double gauss_factor);
		//! in millis
		virtual double getAlgoLatency()	const			{return 1000.0*m_convolutions[0]->size()/GetSamplingRate();}
		virtual int getSampleAlgoLatency() const;

		inline double getDominantTreshold()			{return m_dominant_treshold;}
		inline void setDominantTreshold(double t)	{m_dominant_treshold=t;}

		virtual void apply(const deque<double>& buff);

		virtual ~MonophonicAlgo()					{}
	};

#if 0
	/*! algo for two voice
	 * O()
	 */
	struct TwoVoiceMHT : MultiHalfTone
	{
		//	typedef RemoveSyncMHT MHT;
		typedef SingleResMultiHalfTone MHT;

		MHT* m_mht;
		vector< complex<double> > m_last_sol;

		deque< vector<complex<double> > > fp;
		deque< vector<double> > argpfp;
		deque< vector<double> > modfp;

		TwoVoiceMHT(){}
		TwoVoiceMHT(double AFreq, int dataBySecond, double maxRep, double win_factor, int minHT, int maxHT);

		virtual void apply(deque<double>& buff);

		virtual ~TwoVoiceMHT();
	};

	/*! multiply "usefull" data quantity
	 * O()
	 */
	struct OneDataMultiplierMHT : MultiHalfTone
	{
		vector< SingleHalfTone* > m_sht;

		rfftw_plan m_fwd_plan;
		rfftw_plan m_bck_plan;
		fftw_real* m_in;
		fftw_real* m_out;

		int m_length;
		int m_size;
		int m_rep;

		OneDataMultiplierMHT(){}
		OneDataMultiplierMHT(double AFreq, int dataBySecond, double rep, double win_factor, int minHT, int maxHT);

		virtual void apply(deque<double>& buff);

		virtual ~OneDataMultiplierMHT();
	};

	/*! une grande convolution qui couvre toute la fen�re et qui sert "d'indicateur" �la petit r�olution se trouvant au d�ut
	 * O(nbHT*2)
	 */
	struct IndicMultiHalfTone : SingleResMultiHalfTone
	{
		vector< SingleHalfTone* > m_small_sht;

		IndicMultiHalfTone(double AFreq, int dataBySecond, double maxRep, double win_factor, int minHT, int maxHT);

		virtual void apply(deque<double>& buff);
	};

	/*! integration sur plusieurs r�olution (Ondelettes)
	 * racourcit consid�ablement la chute d'une note, mais pas l'entr�
	 * O(nbHT*maxRep/3)
	 */
	struct MultiResMultiHalfTone : MultiHalfTone
	{
		vector< vector<SingleHalfTone*> > m_sht;

		MultiResMultiHalfTone(){}
		MultiResMultiHalfTone(double AFreq, int dataBySecond, double maxRep, double win_factor, int minHT, int maxHT);

		virtual void apply(deque<double>& buff);

		virtual ~MultiResMultiHalfTone();
	};

	/*! minimum sur trois classes de r�olution
	 * - une grande qui couvre toute la fen�re (augmente la r�olution en fr�uence)
	 * - des progressivement plus petites qui commence au d�ut de la grande fen�re (augemente la r�olution en temps �la fin d'une note)
	 * - des progressivement plus petites qui finissent �la fin de la grande fen�re (augmente la r�olution en temps au d�ut d'une note)
	 * O(nbHT*maxRep/2)
	 */
	struct TriResMultiHalfTone : MultiResMultiHalfTone
	{
		TriResMultiHalfTone(double AFreq, int dataBySecond, double maxRep, double win_factor, int minHT, int maxHT);

		virtual void apply(deque<double>& buff);
	};

	/*! supprime les fr�uences syncronis�s
	 * REDO
	 * O()
	 */
	struct RemoveSyncMHT : MultiHalfTone
	{
		typedef SingleResMultiHalfTone MHT;

		MHT* m_mht;
		vector< complex<double> > m_last_sol;

		RemoveSyncMHT(){}
		RemoveSyncMHT(double AFreq, int dataBySecond, double maxRep, double win_factor, int minHT, int maxHT);

		virtual void apply(deque<double>& buff);

		virtual ~RemoveSyncMHT();
	};
#endif
}

#endif // _FreqAnalysis_h_


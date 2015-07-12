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


#include "FreqAnalysis.h"

#include <assert.h>
#include <iostream>
#include <algorithm>
using namespace std;
using namespace Math;
#include "Music.h"
#include <CppAddons/Fit.h>

namespace Music
{
	double PeakRefinementLogParabola(const vector<std::complex<double> > spectrum, int peak_index)
	{
		assert(peak_index>0 && peak_index<int(spectrum.size()-1));

		if(!(is_peak(spectrum, peak_index)))
		{
			if(peak_index+1<int(spectrum.size()-1) && is_peak(spectrum, peak_index+1))
				peak_index++;
			else if(peak_index-1>0 && is_peak(spectrum, peak_index-1))
				peak_index--;
		}

		double a,b,c,xapex,yapex;
		FitParabola(peak_index-1, log(mod(spectrum[peak_index-1])+numeric_limits<double>::min()),
					peak_index, log(mod(spectrum[peak_index])+numeric_limits<double>::min()),
					peak_index+1, log(mod(spectrum[peak_index+1])+numeric_limits<double>::min()),
					a, b, c, xapex, yapex);

		return xapex;
	}

	/*!
	 * M. Abe and J. O. Smith III
	 * “Design Criteria for Simple Sinusoidal Parameter Estimation based on Quadratic
	 * Interpolation of FFT Magnitude Peaks AM/FM Rate Estimation and Bias Correction
	 * for Time-Varying Sinusoidal Modeling,”
	 * Convention Paper Audio Engineering Society,
	 * Dept. of Music, Stanford University, August, (2004).
	 */
	double PeakRefinementLogParabolaUnbiased(const vector<std::complex<double> > spectrum, int peak_index, double zp)
	{
		double f = PeakRefinementLogParabola(spectrum, peak_index);

		double df = f - peak_index;

		double c0 = 0.247560;		// hann coefs
		double c1 = 0.084372;

		double xi_zp = c0/(zp*zp) + c1/(zp*zp*zp*zp);
		double df2 = xi_zp*(df-0.5)*(df+0.5)*df;

//		double c2 = −0.090608;
// 		double c3 = −0.055781;
// 		double nu_zp = c2/(zp*zp*zp*zp) + c3/(zp*zp*zp*zp*zp*zp);
// 		double da = nu_zp*df*df;

		return f+df2;
	}

	vector<Harmonic> GetHarmonicStruct(const vector<complex<double> >& spectrum, double approx_f0, int nb_harm, double used_zp, double offset_tresh)
	{
		double approx_f0_rel = approx_f0*spectrum.size()/double(GetSamplingRate());
		assert(approx_f0_rel>1 && approx_f0_rel<=spectrum.size()/2-1);

		vector<Harmonic> harms;

		for(int h=1; h<=nb_harm && int(h*approx_f0_rel)<int(spectrum.size()/2)-1; h++)
		{
			int c = int(h*approx_f0_rel);
			int llimit = int((h-offset_tresh)*approx_f0_rel);
            llimit = std::max(llimit, 1);
			int rlimit = int((h+offset_tresh)*approx_f0_rel);
            rlimit = std::min(rlimit, int(spectrum.size()/2)-1);
			int peak_index = -1;

			if(is_peak(spectrum, c))
				peak_index = c;
			else
			{
				int cl = c;
				bool cl_is_peak = false;
				while(!cl_is_peak && cl-1>llimit)
					cl_is_peak = is_peak(spectrum, --cl);

				int cr = c;
				bool cr_is_peak = false;
				while(!cr_is_peak && cr+1<rlimit)
					cr_is_peak = is_peak(spectrum, ++cr);

				if(cl_is_peak)
				{
					if(cr_is_peak)
						peak_index = (c-cl<cr-c)?cl:cr;
					else
						peak_index = cl;
				}
				else if(cr_is_peak)
					peak_index = cr;
			}

			if(peak_index>0)
			{
				Harmonic harm;
				harm.mod = mod(spectrum[peak_index]);
				harm.freq = PeakRefinementLogParabolaUnbiased(spectrum, peak_index, used_zp)*double(GetSamplingRate())/spectrum.size();
				harm.harm_number = h;
				harms.push_back(harm);
			}
		}

		return harms;
	}

	double FundFreqRefinementOfHarmonicStruct(const vector<complex<double> >& spectrum, double approx_f0, int nb_harm, double used_zp)
	{
		vector<Harmonic> harms = GetHarmonicStruct(spectrum, approx_f0, nb_harm, used_zp, 0.2);

		if(harms.empty())
			return 0.0;

		double sum_f0 = 0.0;

		for(size_t i=0; i<harms.size(); i++)
			sum_f0 += harms[i].freq/harms[i].harm_number; // TODO mod weigthed ???

		return sum_f0/harms.size();
	}

#if 0
	void SingleResConvolutionTransform::init()
	{
		if(GetSamplingRate()<=0)	return;

		for(size_t h=0; h<size(); h++)
			if(m_convolutions[h]!=NULL)
				delete m_convolutions[h];

		m_components.resize(GetNbSemitones());
		m_convolutions.resize(GetNbSemitones());
		m_harmonics.resize(GetNbSemitones());
		for(size_t h=0; h<size(); h++)
			m_convolutions[h] = new Convolution(m_latency_factor, m_gauss_factor, int(h)+GetSemitoneMin());
	}

	SingleResConvolutionTransform::SingleResConvolutionTransform(double latency_factor, double gauss_factor)
	: Transform(0.0, 0.0)
	, m_latency_factor(latency_factor)
	, m_gauss_factor(gauss_factor)
	{
		m_convolutions.resize(size());
		m_harmonics.resize(size());
		for(size_t h=0; h<size(); h++)
			m_convolutions[h] = NULL;
		init();
	}
	void SingleResConvolutionTransform::apply(const deque<double>& buff)
	{
		for(size_t h=0; h<size(); h++)
		{
			m_is_fondamental[h] = false;
			m_convolutions[h]->apply(buff);
			m_harmonics[h] = m_convolutions[h]->m_value;
			m_components[h] = mod(m_harmonics[h]);
		}
	}
	SingleResConvolutionTransform::~SingleResConvolutionTransform()
	{
		for(size_t i=0; i<m_convolutions.size(); i++)
			delete m_convolutions[i];
	}

// NeuralNetGaussAlgo
	void NeuralNetGaussAlgo::init()
	{
        cout << "NeuralNetGaussAlgo::init" << endl;

		SingleResConvolutionTransform::init();

//		m_fwd_plan = rfftw_create_plan(m_size, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE | FFTW_OUT_OF_PLACE | FFTW_USE_WISDOM);
	}
	NeuralNetGaussAlgo::NeuralNetGaussAlgo(double latency_factor, double gauss_factor)
	: SingleResConvolutionTransform(latency_factor, gauss_factor)
	{
		init();
	}

	void NeuralNetGaussAlgo::apply(const deque<double>& buff)
	{
//		cout << "NeuralNetGaussAlgo::apply " << m_components_treshold << endl;

		m_components_max = 0.0;
		for(size_t h=0; h<size(); h++)
		{
			m_convolutions[h]->apply(buff);
			m_harmonics[h] = m_convolutions[h]->m_value;
			m_components[h] = mod(m_harmonics[h]);
			m_components_max = max(m_components_max, m_components[h]);
		}

		m_first_fond = UNDEFINED_SEMITONE;
		for(size_t h=0; h<size(); h++)
		{
			m_is_fondamental[h] = false;

			if(m_components[h]/m_components_max>m_components_treshold && m_first_fond==UNDEFINED_SEMITONE)
			{
				m_first_fond = h;
				m_is_fondamental[h] = true;
			}
		}
	}

	NeuralNetGaussAlgo::~NeuralNetGaussAlgo()
	{
	}

// MonophonicAlgo
	MonophonicAlgo::MonophonicAlgo(double latency_factor, double gauss_factor)
	: SingleResConvolutionTransform(latency_factor, gauss_factor)
	{
		init();
	}
	int MonophonicAlgo::getSampleAlgoLatency() const
	{
		return m_convolutions[0]->size();
	}
	void MonophonicAlgo::apply(const deque<double>& buff)
	{
		for(size_t h=0; h<m_is_fondamental.size(); h++)
			m_is_fondamental[h] = false;

		m_volume_max = 0.0;
		m_components_max = 0.0;
		m_first_fond = -1;

//		cout << "buff size=" << buff.size() << " size=" << m_convolutions[m_convolutions.size()-1]->size() << endl;

		int h;
		for(h=size()-1; h>=0 && buff.size()>=m_convolutions[h]->size(); h--)
		{
			size_t i=0;
			if(h!=int(size())-1) i=m_convolutions[h+1]->size();
			for(; i<m_convolutions[h]->size(); i++)
				m_volume_max = max(m_volume_max, abs(buff[i]));

			if(m_volume_max > getVolumeTreshold())
			{
				m_convolutions[h]->apply(buff);

				double formant_mod = mod(m_convolutions[h]->m_value);

//				cout<<formant_mod<<" "<<getComponentsTreshold()<<endl;

//				if(formant_mod > getComponentsTreshold())
				{
// 					m_components[h] = min(formant_mod, max_value);
					m_components[h] = formant_mod;
					m_components_max = max(m_components_max, m_components[h]);
					m_first_fond = h;
				}
//				else m_components[h] = 0.0;
			}
			else m_components[h] = 0.0;
		}
		for(;h>=0; h--)
			m_components[h] = 0.0;

		// smash all components below a treshold of the max component
//		for(size_t h=0; h<size(); h++)
//			if(m_components[h] < m_dominant_treshold*m_components_max)
//				m_components[h] = 0.0;
//
		// the note is the first resulting component
//		for(size_t h=0; m_first_fond==-1 && h<size(); h++)
//			if(m_components[h] > 0.0)
//				m_first_fond = h;

		// correction: the note is the nearest maximum of m_note
//		if(m_first_fond!=-1)
//			while(m_first_fond+1<int(size()) && m_components[m_first_fond+1] > m_components[m_first_fond])
//				m_first_fond++;

//		cout << "m_first_fond=" << m_first_fond << endl;
	}

	TwoVoiceMHT::TwoVoiceMHT(double AFreq, int dataBySecond, double rep, double win_factor, int minHT, int maxHT)
	: m_mht(new MHT(AFreq, dataBySecond, rep, win_factor, minHT, maxHT))
	, m_last_sol(m_mht->m_components.size())
	{
		int nbHT = maxHT - minHT + 1;
		m_components.resize(nbHT);

		for(size_t i=0; i<m_last_sol.size(); i++)
			m_last_sol[i] = complex<double>(0.0,0.0);
	}
	void TwoVoiceMHT::apply(deque<double>& buff)
	{
		//	cout << "TwoVoiceMHT::apply" << endl;

		m_mht->apply(buff);

		//	double earingTreshold = 0.05;
		//	double modArgTreshold = 0.2;
		//	double modModTreshold = 0.2;
		//	ComputeDiffs(m_mht->m_components, fp, argpfp, modfp);

		//	int count = 0;
		for(size_t h=0;h<m_components.size(); h++)
		{
			//		if(m_mht->m_components[h]!=complex<double>(0.0,0.0) && count<2)
			{
				m_components[h] = m_mht->m_components[h];

				//			count++;
				//			if(fabs(argpfp[0][h])>modArgTreshold)	count++;
			}
			//		else	m_components[h] = complex<double>(0.0,0.0);
		}

		//	m_last_sol = m_mht->m_components;
	}
	TwoVoiceMHT::~TwoVoiceMHT()
	{
		delete m_mht;
	}

	RemoveSyncMHT::RemoveSyncMHT(double AFreq, int dataBySecond, double rep, double win_factor, int minHT, int maxHT)
	: m_mht(new MHT(AFreq, dataBySecond, rep, win_factor, minHT, maxHT))
	, m_last_sol(m_mht->m_components.size())
	{
		int nbHT = maxHT - minHT + 1;
		m_components.resize(nbHT);

		for(size_t i=0; i<m_last_sol.size(); i++)
			m_last_sol[i] = complex<double>(0.0,0.0);
	}
	void RemoveSyncMHT::apply(deque<double>& buff)
	{
		m_mht->apply(buff);

		double earingTreshold = 0.05;
		double syncArgTreshold = 0.3;	// 0.02 0.25 0.2
		//	double syncModTreshold = 0.2;	// 0.05 0.1 0.3

		double fourier_amplitude = 0.0;
		for(size_t h=0; h<m_mht->m_components.size(); h++)
			fourier_amplitude = max(fourier_amplitude, normm(m_mht->m_components[h]));
		vector<int> notes;

		for(size_t h=0; h<m_mht->m_components.size(); h++)                       // for each half tone
		{
			bool is_fond = false;

			if(normm(m_mht->m_components[h])>earingTreshold*fourier_amplitude) // if we can ear it
			{
				is_fond = true;

				// search for syncronisation with each discovered fondamentals
				for(size_t i=0; i<notes.size() && is_fond; i++)
				{
					double rk = m_mht->m_convolutions[h]->m_freq/m_mht->m_convolutions[notes[i]]->m_freq;
					int k = int(rk+0.5);
					if(abs(k-rk)<0.05) // TODO		// if k is nearly an integer, a potential harmonic
					{
						complex<double> ft = m_mht->m_components[notes[i]] / normm(m_mht->m_components[notes[i]]);
						complex<double> ftm1 = m_last_sol[notes[i]] / normm(m_last_sol[notes[i]]);
						complex<double> rpt = m_mht->m_components[h]/pow(ft, k);
						complex<double> rptm1 = m_last_sol[h]/pow(ftm1, k);
						//					if(h==25 && k==4)
						//						cout << abs(log(normm(rpt))-log(normm(rptm1))) << " ";
						//						cout << k << "=(arg=" << abs(arg(rpt)-arg(rptm1)) << " mod=" << abs(log(normm(rpt))-log(normm(rptm1))) << ") ";
						is_fond = abs(arg(rpt)-arg(rptm1)) > syncArgTreshold;
						//					is_fond = is_fond || abs(log(normm(rpt))-log(normm(rptm1))) > syncModTreshold;
					}

					//				is_fond = false;
				}

				if(is_fond) notes.push_back(h);       // it's a fondamentals
			}

			//		cout << endl;

			if(is_fond)		m_components[h] = m_mht->m_components[h];
			else			m_components[h] = complex<double>(0.0,0.0);
		}

		m_last_sol = m_mht->m_sol;
	}
	RemoveSyncMHT::~RemoveSyncMHT()
	{
		delete m_mht;
	}

#if 0
	NeuralNetMHT::NeuralNetMHT(double AFreq, int dataBySecond, double rep, double win_factor, int minHT, int maxHT, const string& file_name)
	: m_mht(new MHT(AFreq, dataBySecond, rep, win_factor, minHT, maxHT))
	, m_nn(new LayeredNeuralNet<TypeNeuron>())
	{
		m_nbHT = maxHT - minHT + 1;
		m_components.resize(m_nbHT);

		m_nn->load(file_name.c_str());

		assert(m_nbHT==m_nn->getInputLayer()->getNeurons().size());
		assert(m_nbHT==m_nn->getOutputLayer()->getNeurons().size());

		cout << "topology: " << m_nn->getInputLayer()->getNeurons().size();
		for(LayeredNeuralNet<TypeNeuron>::LayerIterator it = ++(m_nn->m_layerList.begin()); it !=m_nn->m_layerList.end(); it++)
			cout << " => " << (*it)->getNeurons().size();
		cout << "   [" << m_nn->getNbWeights() << " weights]" << endl;
	}
	void NeuralNetMHT::apply(deque<double>& buff)
	{
		//	cout << "NeuralNetMHT::apply" << endl;

		m_mht->apply(buff);

		vector<double> inputs(m_nbHT);

		for(int h=0; h<m_nbHT; h++)
			inputs[h] = normm(m_mht->m_components[h]);

		m_nn->computeOutputs(inputs);

		for(int h=0; h<m_nbHT; h++)
			m_components[h] = complex<double>(m_nn->getOutputLayer()->getNeurons()[h].o);
	}
	NeuralNetMHT::~NeuralNetMHT()
	{
		delete m_nn;
		delete m_mht;
	}
#endif

#endif
}

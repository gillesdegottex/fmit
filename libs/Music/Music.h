// Copyright 2004 "Gilles Degottex", 2007 "Florian Hars"

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


#ifndef _Music_h_
#define _Music_h_

#include <assert.h>
#include <cmath>
#include <string>
#include <list>
#include <vector>
#include <limits>
using namespace std;
#include <CppAddons/CAMath.h>
#include <CppAddons/StringAddons.h>

#include <iostream>

namespace Music
{
	enum NotesName{LOCAL_ANGLO, LOCAL_LATIN, LOCAL_GERMAN, LOCAL_HINDUSTANI};
	extern NotesName s_notes_name;
	inline NotesName GetNotesName()					{return s_notes_name;}
	inline void SetNotesName(NotesName type)		{s_notes_name = type;}

	extern int s_tonality;
	inline int GetTonality()						{return s_tonality;}
	inline void SetTonality(int tonality)			{s_tonality = tonality;}

	enum Tuning{CHROMATIC,WERCKMEISTER3,KIRNBERGER3,DIATONIC,MEANTONE};
	extern Tuning s_tuning;
	extern double s_semitones[13];
	inline Tuning GetTuning()						{return s_tuning;}
	void SetTuning(Tuning tuning);


	extern int s_sampling_rate;
	inline int GetSamplingRate()					{return s_sampling_rate;}
	void SetSamplingRate(int sampling_rate);

	extern double s_AFreq;
	inline double GetAFreq()						{return s_AFreq;}
	void SetAFreq(double AFreq);

	extern const int UNDEFINED_SEMITONE;
	extern int s_semitone_min;
	extern int s_semitone_max;
	inline int GetSemitoneMin()						{return s_semitone_min;}
	inline int GetSemitoneMax()						{return s_semitone_max;}
	inline int GetNbSemitones()						{return s_semitone_max-s_semitone_min+1;}
	void SetSemitoneBounds(int semitone_min, int semitone_max);

	struct SettingsListener
	{
		virtual void samplingRateChanged()			{}
		virtual void AFreqChanged()					{}
		virtual void semitoneBoundsChanged()		{}

		SettingsListener();
		virtual ~SettingsListener();
	};

	extern list<SettingsListener*>	s_settings_listeners;
	void AddSettingsListener(SettingsListener* l);
	void RemoveSettingsListener(SettingsListener* l);

	//! convert frequency to a float number of chromatic half-tones from A3
	/*!
	* \param freq the frequency to convert to \f$\in R+\f$ {Hz}
	* \param AFreq tuning frequency of the A3 (Usualy 440) {Hz}
	* \return the float number of half-tones from A3 \f$\in R\f$
	*/
	inline double f2hf(double freq, double AFreq=GetAFreq())
	{
			return 17.3123404906675624 * log(freq/AFreq); //12.0*(log(freq)-log(AFreq))/log(2.0)
	}
	//! find the halftone in the array for non-chromatic tunings
	// TODO:
	// Decide wether the step from 12/2 for linar search to log_2(12) for a
	// binary search really matters in a FFT-bound program
	/*!
	* \param relFreq the frequency divided by the frequency of the next lower A
	* \return the number of halftones above this A
	*/
	inline int f2h_find(double relFreq)
	{
		if (relFreq < s_semitones[1])
		{
			if (s_semitones[1] / relFreq > relFreq / s_semitones[0])
				return 0;
			else
				return 1;
		}
		else
		{
			int i;
			for (i = 2; i < 12; i += 1)
				if (relFreq < s_semitones[i]) { break; }

			if (s_semitones[i] / relFreq > relFreq / s_semitones[i - 1])
				return i - 1;
			else
				return i;
		}
	}
	// TODO VERIF
	// le ht doit �re le ht le plus proche de freq !! et pas un simple arrondi en dessous de la valeur r�l !!
	//! convert frequency to number of half-tones from A3
	/*!
	* \param freq the frequency to convert to \f$\in R+\f$ {Hz}
	* \param AFreq tuning frequency of the A3 (Usualy 440) {Hz}
	* \return the number of half-tones from A3. Rounded to the nearest half-tones(
	* not a simple integer convertion of \ref f2hf ) \f$\in R\f$
	*/
	inline int f2h(double freq, double AFreq=GetAFreq(), int tuning=GetTuning())
	{
		if (CHROMATIC == tuning)
		{
			double ht = f2hf(freq, AFreq);
			if(ht>0)	return int(ht+0.5);
			if(ht<0)	return int(ht-0.5);
			return	0;
		}
		else
		{
			if (freq <= 1.0) return UNDEFINED_SEMITONE;
			int oct = 0;
			while (freq < AFreq)        { freq *= 2.0; oct -= 1; }
			while (freq >= 2.0 * AFreq) { freq /= 2.0; oct += 1; }
			int ht = f2h_find(freq/AFreq);
			return (12 * oct + ht);
		}
	}
	//! convert number of chromatic half-tones to frequency
	/*!
	* \param ht number of half-tones to convert to \f
	* \param AFreq tuning frequency of the A3 (Usualy 440) {Hz}
	* \return the converted frequency
	*/
	inline double h2f(double ht, double AFreq=GetAFreq())
	{
		return AFreq * pow(2.0, ht/12.0);
	}
	//! convert number of half-tones to frequency
	/*!
	* \param ht number of half-tones to convert to \f$\in Z\f$
	* \param AFreq tuning frequency of the A3 (Usualy 440) {Hz}
	* \return the converted frequency
	*/
	inline double h2cf(int ht, double AFreq=GetAFreq(), int tuning=GetTuning())
	{
		if (CHROMATIC == tuning)
		{
			return AFreq * pow(2.0, ht/12.0);
		}
		else if (ht >= s_semitone_min)
		{
			int oct = 0;
			while(ht<0)
			{
				ht += 12;
				oct--;
			}
			while(ht>11)
			{
				ht -= 12;
				oct++;
			}
			return AFreq * pow(2.0, oct) * s_semitones[ht];
		}
		else
		{
			return 0.0;
		}
	}
	//! convert frequency to the frequency of the nearest half tone
	/*!
	* \param freq the frequency to convert to \f$\in R+\f$ {Hz}
	* \param AFreq tuning frequency of the A3 (Usualy 440) {Hz}
	* \return the converted frequency
	*/
	inline double f2cf(double freq, double AFreq=GetAFreq(), int tuning=GetTuning())
	{
		return h2cf(f2h(freq, AFreq, tuning), AFreq, tuning);
	}

	//! convert half-tones from A3 to the corresponding note name
	/*!
	* \param ht number of half-tone to convert to \f$\in Z\f$
	* \param local
	* \return its name (Do, Re, Mi, Fa, Sol, La, Si; with '#' or 'b' if needed)
	*/
	inline string h2n(int ht, NotesName local=GetNotesName(), int tonality=GetTonality(), int tunig=GetTuning(), bool show_oct=true)
	{
        (void)tunig;

		ht += tonality;

		int oct = 4;
		while(ht<0)
		{
			ht += 12;
			oct--;
		}
		while(ht>11)
		{
			ht -= 12;
			oct++;
		}

		if(ht>2)	oct++;	// octave start from C
	// 	if(oct<=0)	oct--;	// skip 0-octave in occidental notations ??

	//	char coct[3];
	//	sprintf(coct, "%d", oct);
	//	string soct = coct;

		string soct;
		if(show_oct)
			soct = StringAddons::toString(oct);

		if(local==LOCAL_ANGLO)
		{
			if(ht==0)	return "A"+soct;
			else if(ht==1)	return "Bb"+soct;
			else if(ht==2)	return "B"+soct;
			else if(ht==3)	return "C"+soct;
			else if(ht==4)	return "C#"+soct;
			else if(ht==5)	return "D"+soct;
			else if(ht==6)	return "Eb"+soct;
			else if(ht==7)	return "E"+soct;
			else if(ht==8)	return "F"+soct;
			else if(ht==9)	return "F#"+soct;
			else if(ht==10)	return "G"+soct;
			else if(ht==11)	return "G#"+soct;
		}
		else if(local==LOCAL_LATIN)
		{
			if(ht==0)	return "La"+soct;
			else if(ht==1)	return "Sib"+soct;
			else if(ht==2)	return "Si"+soct;
			else if(ht==3)	return "Do"+soct;
			else if(ht==4)	return "Do#"+soct;
			else if(ht==5)	return "Re"+soct;
			else if(ht==6)	return "Mib"+soct;
			else if(ht==7)	return "Mi"+soct;
			else if(ht==8)	return "Fa"+soct;
			else if(ht==9)	return "Fa#"+soct;
			else if(ht==10)	return "Sol"+soct;
			else if(ht==11)	return "Sol#"+soct;
		}
		else if(local==LOCAL_GERMAN)
		{
			if(ht==0)	return "A"+soct;
			else if(ht==1)	return "B"+soct;
			else if(ht==2)	return "H"+soct;
			else if(ht==3)	return "C"+soct;
			else if(ht==4)	return "C#"+soct;
			else if(ht==5)	return "D"+soct;
			else if(ht==6)	return "Eb"+soct;
			else if(ht==7)	return "E"+soct;
			else if(ht==8)	return "F"+soct;
			else if(ht==9)	return "F#"+soct;
			else if(ht==10)	return "G"+soct;
			else if(ht==11)	return "G#"+soct;
		}
		else if(local==LOCAL_HINDUSTANI)
		{
			if(ht==0)	return "D"+soct;
			else if(ht==1)	return "n"+soct;
			else if(ht==2)	return "N"+soct;
			else if(ht==3)	return "S"+soct;
			else if(ht==4)	return "r"+soct;
			else if(ht==5)	return "R"+soct;
			else if(ht==6)	return "g"+soct;
			else if(ht==7)	return "G"+soct;
			else if(ht==8)	return "m"+soct;
			else if(ht==9)	return "M"+soct;
			else if(ht==10)	return "P"+soct;
			else if(ht==11)	return "d"+soct;
		}

		return "Th#1138";
	}

//	inline int n2h(const std::string& note, NotesName local=LOCAL_ANGLO, int tonality=GetTonality())
//	{
//		// TODO
//		return -1;
//	}

    //! convert amplitude to dB
    template<class TYPE>
    TYPE lp(TYPE value)
    {
        return 20*log10(abs(value));
//         return 20*log10(abs(value)+numeric_limits<TYPE>::epsilon());
    }

    //! convert dB to amplitude
    // TODO cannot create a template so easily because the pow10 is not defined for all types
    inline double invlp(double value)
    {
        return std::pow(10, value/20.0);
//         return pow(TYPE(10), value/TYPE(20));
    }

    std::vector<double> conv(const std::vector<double>& u, const std::vector<double>& v);

	// TODO freq reffinement
}

#endif // _Music_h_

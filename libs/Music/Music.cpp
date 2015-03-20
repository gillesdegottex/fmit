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


#include "Music.h"

#include <algorithm>
#include <iostream>
using namespace std;

Music::NotesName Music::s_notes_name = Music::LOCAL_ANGLO;

int Music::s_tonality = 0;
double Music::s_semitones[13] = { 1.0000, 1.0595, 1.1225, 1.1892, 1.2599, 1.3348, 1.4142, 1.4983, 1.5874, 1.6818, 1.7818, 1.8877, 2.0000 }; // Chromatic, never used
static double semitones_werckmeister3[] = { 1.0, 1.06425952556, 1.125, 1.19729196625, 1.26134462288, 1.33785800438, 1.41901270074, 1.5, 1.59638928833, 1.68179283051, 1.78986403988, 1.89201693432, 2.0 };
static double semitones_kirnberger3[] = { 1.0, 1.06335913331, 1.12151158592, 1.19627902498, 1.26027749133, 1.33748060995, 1.41781217775, 1.49534878122, 1.59503869997, 1.68036998845, 1.788854382, 1.890416237, 2.0 };
static double semitones_diatonic[] = { 1.0, 1.06666666667, 1.125, 1.2, 1.25, 1.35, 1.44, 1.5, 1.6, 1.6875, 1.8, 1.875, 2.0 };
static double semitones_meantone[] = { 1.0, 1.06998448796, 1.11803398875, 1.19627902498, 1.25, 1.33748060995, 1.4310835056, 1.49534878122, 1.6, 1.67185076244, 1.788854382, 1.86918597653, 2.0};
Music::Tuning Music::s_tuning = Music::CHROMATIC;

int Music::s_sampling_rate = -1;
double Music::s_AFreq = 440.0;
const int Music::UNDEFINED_SEMITONE = -1000;
int Music::s_semitone_min = -48;
int Music::s_semitone_max = +48;

Music::SettingsListener::SettingsListener()
{
	s_settings_listeners.push_back(this);
}
Music::SettingsListener::~SettingsListener()
{
	s_settings_listeners.remove(this);
}

list<Music::SettingsListener*>	Music::s_settings_listeners;
void Music::AddSettingsListener(SettingsListener* l)
{
	if(find(s_settings_listeners.begin(), s_settings_listeners.end(), l)==s_settings_listeners.end())
		s_settings_listeners.push_back(l);
}
void Music::RemoveSettingsListener(SettingsListener* l)
{
	s_settings_listeners.remove(l);
}

void Music::SetTuning(Tuning tuning)
{
	int i;
	s_tuning=tuning;
	switch (tuning) {
	case CHROMATIC:
		// Nothing to do
		break;
	case WERCKMEISTER3:
		for (i = 0; i < 13; i += 1)
		{
			s_semitones[i] = semitones_werckmeister3[i];
		}
		break;
	case KIRNBERGER3:
		for (i = 0; i < 13; i += 1)
		{
			s_semitones[i] = semitones_kirnberger3[i];
		}
		break;
	case DIATONIC:
		for (i = 0; i < 13; i += 1)
		{
			s_semitones[i] = semitones_diatonic[i];
		}
		break;
	case MEANTONE:
		for (i = 0; i < 13; i += 1)
		{
			s_semitones[i] = semitones_meantone[i];
		}
		break;
	}
}

void Music::SetSamplingRate(int sampling_rate)
{
	s_sampling_rate = sampling_rate;
	for(list<Music::SettingsListener*>::iterator it=s_settings_listeners.begin(); it!=s_settings_listeners.end(); ++it)
		(*it)->samplingRateChanged();
}

void Music::SetAFreq(double AFreq)
{
	s_AFreq = AFreq;
	for(list<Music::SettingsListener*>::iterator it=s_settings_listeners.begin(); it!=s_settings_listeners.end(); ++it)
		(*it)->AFreqChanged();
}

void Music::SetSemitoneBounds(int semitone_min, int semitone_max)
{
// 	cerr << "Music::SetSemitoneBounds" << endl;

	s_semitone_min = semitone_min;
	s_semitone_max = semitone_max;

	for(list<Music::SettingsListener*>::iterator it=s_settings_listeners.begin(); it!=s_settings_listeners.end(); ++it)
		(*it)->semitoneBoundsChanged();
}

vector<double> Music::conv(const vector<double>& u, const vector<double>& v)
{
	vector<double> w(v.size());

	for(size_t i=0; i<w.size(); i++)
	{
		w[i]=0.0;
		for(size_t j=0; j<u.size(); j++)
			if(i-j>=0)
				w[i] += u[j]*v[i-j];
	}

	return w;
}

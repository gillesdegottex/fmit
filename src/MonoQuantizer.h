// Copyright 2005 "Gilles Degottex"

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


#ifndef _MONOQUANTIZER_H_
#define _MONOQUANTIZER_H_

#include <deque>
using namespace std;
#include <qobject.h>
#include <qdatetime.h>
#include <Music/Music.h>
using namespace Music;

class MonoQuantizer : public QObject
{
	Q_OBJECT

  protected:
	double m_min_confidence;

	double m_confidence;
	double m_current_center_freq;					// nearest semi-tone
	double m_avg_freq;								// precise frequency

  public:
	MonoQuantizer();

	double getMinConfidence()						{return m_min_confidence;}
	void setMinConfidence(float min_confidence)		{m_min_confidence=min_confidence;}
	double getConfidence()							{return m_confidence;}
// 	int getMinStoredRecon()							{return m_min_stored_recon;}
	bool isPlaying()								{return m_current_center_freq>0.0;}
	double getCenterFrequency()						{return m_current_center_freq;}
	double getAverageFrequency()					{return m_avg_freq;}

	virtual void quantize(double freq)=0;
	virtual void reset()=0;

	virtual ~MonoQuantizer();

  signals:
	void noteStarted(double freq, double dt);
	void noteFinished(double freq, double dt);
// 	void notePlayed(double freq, double duration, double dt);

//   public slots:
};

#endif // _MONOQUANTIZER_H_

// Copyright 2004 "Gilles Degottex"

// This file is part of "fmit"

// "fmit" is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// "fmit" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



#ifndef _GLVolumeHistory_h_
#define _GLVolumeHistory_h_

#include <deque>
using namespace std;
#include <QGLWidget>
#include <qaction.h>
class QTimer;
#include "View.h"

class GLVolumeHistory : public QGLWidget, public View
{
	Q_OBJECT

    QFont m_font;

public:
	GLVolumeHistory(QWidget* parent);

	struct Note
	{
		int ht;
		QString factor;
		deque<float> volumes;
		Note(int h);
		Note(int h, int num, int den);
		Note(int h, float cents);
		QString getName() const;
	};
	deque<Note> m_notes;
	void addNote(GLVolumeHistory::Note note);
	void addVolume(float vol);

	int m_volume_treshold;

	// settings
	QAction* setting_keep;
	virtual void save();
	virtual void load();
	virtual void clearSettings();

  public slots:
	void initializeGL();
	void paintGL();
	void resizeGL( int w, int h );
	void keepPreviousNotes(bool keep);
	void setVolumeTreshold(int treshold);
};

#endif // _GLVolumeHistory_h_

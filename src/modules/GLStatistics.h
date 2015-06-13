// Copyright 2007 "Gilles Degottex"

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



#ifndef _GLStatistics_h_
#define _GLStatistics_h_

#include <deque>
using namespace std;
#include <QGLWidget>
#include <qspinbox.h>
#include <qsettings.h>
#include <qaction.h>
class QTimer;
#include <Music/Music.h>
#include "View.h"

class GLStatistics : public QGLWidget, public View
{
	Q_OBJECT

	virtual void mouseReleaseEvent(QMouseEvent* e){View::mouseReleaseEvent(e);}

	void drawTicksCent(int r, int dx, int dy);
	void drawTextTickCent(int r, int dx);

  public:
	GLStatistics(QWidget* parent);

	struct AverageNote
	{
		int ht;
		deque<float> errs;
		float err_mean;
		float err_std;

		void addErr(float err);

		void init();
		AverageNote();
		AverageNote(int h);
		AverageNote(int h, int num, int den);
		AverageNote(int h, float cents);
		QString factor;
		QString getName() const;
	};
	vector<AverageNote> m_avg_notes;
	void addNote(int ht, float err);
	void addNote(int ht, int num, int den, float err);
	void addNote(int ht, float cents, float err);
	int m_current_ht;

	// settings
	QAction* setting_reset;
	QAction* setting_keep_hidden;
	QSpinBox* setting_keep_n_values;
	QAction* setting_show_std;
	QAction* setting_scale_auto;
	QSpinBox* setting_scale_min;
	QSpinBox* setting_scale_max;
	QAction* setting_showTolerance;
	QAction* setting_useCents;
	QSpinBox* setting_spinScale;
	virtual void save();
	virtual void load();
	virtual void clearSettings();

  public slots:
	void initializeGL();
	void paintGL();
	void resizeGL( int w, int h );

	void reset();
	void resizeScale();
};

#endif // _GLErrorHistory_h_

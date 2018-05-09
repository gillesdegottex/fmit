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

#ifndef _GLSample_h_
#define _GLSample_h_

#include <deque>
using namespace std;

#include <QOpenGLWidget>
#include <qaction.h>
#include <qspinbox.h>
class QTimer;
#include "View.h"

class GLSample : public QOpenGLWidget, public View
{
	Q_OBJECT

    QFont m_font;

    virtual void mouseReleaseEvent(QMouseEvent* e){View::mouseReleaseEvent(e);}

	double m_max_value;

  public:
	GLSample(QWidget* parent);

	struct Sample
	{
		double time;
		deque<double> data;
		double max_value;
		Sample(double t, const deque<double>& d);
	};
	deque<Sample> m_samples;

	void add(double time, const deque<double>& data);
	void clear()	{m_samples.clear();}

	// settings
	QAction* setting_hasFading;
	QSpinBox* setting_spinNumFading;
	virtual void save();
	virtual void load();
	virtual void clearSettings();

  public slots:
	void initializeGL();
	void paintGL();
	void resizeGL( int w, int h );
};

#endif // _GLSample_h_


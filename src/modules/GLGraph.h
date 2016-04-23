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


#ifndef _GLGraph_h_
#define _GLGraph_h_

#include <deque>
using namespace std;
#include <QGLWidget>
#include <qaction.h>
#include <qspinbox.h>
class QTimer;
#include <Music/Music.h>
#include "View.h"

class GLGraph : public QGLWidget, public View
{
	Q_OBJECT

    QFont m_font;

protected:
	virtual void mouseReleaseEvent(QMouseEvent* e){View::mouseReleaseEvent(e);}

	deque<double> m_pending_queue;
	deque<double> m_queue;
	int m_skip;
	deque< pair<double,double> > m_maxs;
	double m_queue_amplitude;
	bool m_new_values;

	virtual void base_paint(float graph_gray=0.0);

	void update_maxs();

  public:
	GLGraph(QWidget* parent);
	~GLGraph(){}
	void addValue(double v);
	void clearValues();
	void refreshGraph();
	int getLength() const		{return (setting_spinDuration->value()*Music::GetSamplingRate())/1000;}

	double m_treshold;

	// settings
	QAction* setting_showWaveForm;
	QAction* setting_autoScale;
	QSpinBox* setting_spinScaleFactor;
	QSpinBox* setting_spinDuration;
	QSpinBox* setting_spinMaxHeight;
	virtual void save();
	virtual void load();
	virtual void clearSettings();

 public slots:
	void initializeGL();
	void paintGL();
	void resizeGL( int w, int h );
};

#endif // _GLGraph_h_

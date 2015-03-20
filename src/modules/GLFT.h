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


#ifndef _GLFT_h_
#define _GLFT_h_

#include <vector>
using namespace std;
#include <Music/CFFTW3.h>
// using namespace Music;
#include <qgl.h>
#include <qspinbox.h>
#include <qaction.h>
#include "View.h"

class GLFT : public QGLWidget, public View
{
	Q_OBJECT

	virtual void mouseReleaseEvent(QMouseEvent* e){View::mouseReleaseEvent(e);}
	virtual void mousePressEvent(QMouseEvent* e);
	virtual void mouseDoubleClickEvent(QMouseEvent* e);
	virtual void mouseMoveEvent(QMouseEvent* e);

	vector<double> win;
	CFFTW3 m_plan;
	vector<double> m_components;
	double m_components_max;

	bool m_start_move_mouse;
	int m_press_x, m_press_y;
    double m_press_minf, m_press_maxf;

    double m_minf, m_maxf; // [Hz]
    double m_minA, m_maxA; // [dB or amplitude depending on ]
    void resetaxis();
    QString m_text;

  public:
	GLFT(QWidget* parent);
	~GLFT(){}

	deque<double> buff;

	void refreshGraph();

	double m_os;
    void setSamplingRate(int sr);

	// settings
	QAction* setting_db_scale;
	QSpinBox* setting_winlen;

  public slots:
	void spinWinLengthChanged(int num);
    void dBScaleChanged(bool db);
	void initializeGL();
	void paintGL();
	void resizeGL( int w, int h );
};

#endif // _GLFT_h_


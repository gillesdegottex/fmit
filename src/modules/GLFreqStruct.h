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


#ifndef _GLFreqStruct_h_
#define _GLFreqStruct_h_

#include <vector>
using namespace std;
#include <qgl.h>
#include <qspinbox.h>
#include <qaction.h>
#include "View.h"

class GLFreqStruct : public QGLWidget, public View
{
	Q_OBJECT

	virtual void mouseReleaseEvent(QMouseEvent* e){View::mouseReleaseEvent(e);}

  public:
	GLFreqStruct(QWidget* parent);
	~GLFreqStruct(){}

	vector<double> m_components;
	double m_components_max;

	// settings
	QSpinBox* setting_spinNumComponents;
	virtual void save();
	virtual void load();
	virtual void clearSettings();

  public slots:
	void spinNumComponentsChanged(int num);
	void initializeGL();
	void paintGL();
	void resizeGL( int w, int h );
};

#endif // _GLFreqStruct_h_


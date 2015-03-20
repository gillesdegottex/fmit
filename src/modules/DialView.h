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

#ifndef _DialView_h_
#define _DialView_h_

#include <qframe.h>
#include <qaction.h>
#include <qspinbox.h>
#include <qsettings.h>
#include "View.h"

class DialView : public QFrame, public View
{
	Q_OBJECT

	virtual void paintEvent(QPaintEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* e){View::mouseReleaseEvent(e);}
	void drawTextTickCent(QPainter& p, int bigw, int bigh, int r);
	void drawTicksCent(QPainter& p, int bigw, int bigh, int r, int ticks_size, int h);
	void drawTextTickFrac(QPainter& p, int bigw, int bigh, int num, int den);
	void drawTicksFrac(QPainter& p, int bigw, int bigh, float r, int ticks_size, int h);

	float m_error;

  public:
	DialView(QWidget* parent);

	void setError(float error);

	float m_min_error;
	float m_max_error;
	float m_avg_error;

	// settings
	QAction* setting_showTolerance;
	QAction* setting_showTrace;
	QAction* setting_useCents;
	QSpinBox* setting_spinScale;
	virtual void save();
	virtual void load();
	virtual void clearSettings();
};

#endif // _DialView_h_


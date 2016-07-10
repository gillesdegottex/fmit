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

#include "GLSample.h"

#include <cassert>
#include <cmath>
#include <iostream>
using namespace std;
#include <qtimer.h>
#include <qtooltip.h>
#include <qimage.h>
#include <qboxlayout.h>
#include <qwidgetaction.h>

GLSample::GLSample(QWidget* parent)
: QGLWidget(parent)
, View(tr("Waveform"), this)
, m_font("Helvetica")
, m_max_value(1.0)
{
	// settings
    setting_show->setIcon(QIcon(":/fmit/ui/images/module_waveformperiod.svg"));
    setting_show->setChecked(false);
	hide();

	setting_hasFading = new QAction(tr("Show fading"), this);
	setting_hasFading->setCheckable(true);
	connect(setting_hasFading, SIGNAL(toggled(bool)), this, SLOT(updateGL()));
	m_popup_menu.addAction(setting_hasFading);

	QHBoxLayout* numFadingActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* numFadingActionTitle = new QLabel(tr("Number of fading"), &m_popup_menu);
	numFadingActionLayout->addWidget(numFadingActionTitle);

	setting_spinNumFading = new QSpinBox(&m_popup_menu);
	setting_spinNumFading->setMinimum(1);
	setting_spinNumFading->setMaximum(10000);
	setting_spinNumFading->setSingleStep(1);
	setting_spinNumFading->setValue(20);
	setting_spinNumFading->setToolTip(tr("Number of fading"));
	connect(setting_spinNumFading, SIGNAL(valueChanged(int)), this, SLOT(updateGL()));
	numFadingActionLayout->addWidget(setting_spinNumFading);

	QWidget* numFadingActionWidget = new QWidget(&m_popup_menu);
	numFadingActionWidget->setLayout(numFadingActionLayout);

	QWidgetAction* numFadingAction = new QWidgetAction(&m_popup_menu);
	numFadingAction->setDefaultWidget(numFadingActionWidget);
	m_popup_menu.addAction(numFadingAction);
}

void GLSample::save()
{
	s_settings->setValue("hasFading", setting_hasFading->isChecked());
	s_settings->setValue("spinNumFading", setting_spinNumFading->value());
}
void GLSample::load()
{
	setting_hasFading->setChecked(s_settings->value("hasFading", setting_hasFading->isChecked()).toBool());
	setting_spinNumFading->setValue(s_settings->value("spinNumFading", setting_spinNumFading->value()).toInt());
}
void GLSample::clearSettings()
{
	s_settings->remove("hasFading");
	s_settings->remove("spinNumFading");
}

GLSample::Sample::Sample(double t, const deque<double>& d)
{
	time = t;
	data = d;
	max_value = 0.0;
	for(size_t i=0; i<data.size(); i++)
		max_value = max(max_value, abs(data[i]));
}

void GLSample::add(double time, const deque<double>& data)
{
	m_samples.push_front(Sample(time, data));

	if(setting_hasFading->isChecked())
		while(!m_samples.empty() && int(m_samples.size())>setting_spinNumFading->value())
			m_samples.pop_back();
	else
		while(!m_samples.empty() && m_samples.size()>1)
			m_samples.pop_back();
}

void GLSample::initializeGL()
{
	// Set the clear color to black
	glClearColor(1.0, 1.0, 1.0, 0.0);

	// glShadeModel( GL_FLAT );
	glShadeModel( GL_SMOOTH );

	glLoadIdentity();
}

void GLSample::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// name
	glColor3f(0.75,0.75,0.75);
    m_font.setPixelSize(20);
    renderText(2, 20, tr("Waveform's period"), m_font);

	// horiz lines
	glBegin(GL_LINES);
	float gray = 0.5;
	glColor3f(gray, gray, gray);
	glVertex2i(0,  height()/2);
	glVertex2i(width(),  height()/2);
	glEnd();

	if(m_max_value==0.0)	m_max_value = 1.0;

	// sample
	if(!m_samples.empty())
	{
		m_max_value = 0.0;
        for(int j=int(m_samples.size())-1; j>=0; j--)
		{
			m_max_value = max(m_max_value, abs(m_samples[j].max_value));
			if(!setting_hasFading->isChecked())	j=-1;
		}

        for(int j=int(m_samples.size())-1; j>=0; j--)
		{
			if(m_max_value!=0.0)
			{
				glBegin(GL_LINE_STRIP);
				float grey = float(j)/m_samples.size();
				glColor3f(grey, grey, grey);
                int size = int(m_samples[j].data.size());
				float step = float(width())/size;
				for(int i=size-1; i>0; i--)
					glVertex2i(int((size-i-1)*step), int((m_samples[j].data[i]/m_max_value)*height()/2 + height()/2));
				glVertex2i(width(), int((m_samples[j].data[0]/m_max_value)*height()/2 + height()/2));
				glEnd();
			}

			if(!setting_hasFading->isChecked())	j=-1;
		}
	}

	glFlush();
}

void GLSample::resizeGL( int w, int h )
{
	// Set the new viewport size
	glViewport(0, 0, (GLint)w, (GLint)h);

	// Choose the projection matrix to be the matrix
	// manipulated by the following calls
	glMatrixMode(GL_PROJECTION);

	// Set the projection matrix to be the identity matrix
	glLoadIdentity();

	// Define the dimensions of the Orthographic Viewing Volume
	glOrtho(0.0, w, 0.0, h, 0.0, 1.0);

	// Choose the modelview matrix to be the matrix
	// manipulated by further calls
	glMatrixMode(GL_MODELVIEW);
}

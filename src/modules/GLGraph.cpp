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


#include "GLGraph.h"

#include <iostream>
using namespace std;
#include <qtooltip.h>
#include <qimage.h>
#include <qboxlayout.h>
#include <qwidgetaction.h>
#include <Music/Music.h>
using namespace Music;

GLGraph::GLGraph(QWidget* parent)
: QGLWidget(parent)
, View(tr("Captured Sound"), this)
, m_font("Helvetica")
, m_skip(1)
, m_new_values(false)
{
	// settings
    setting_show->setIcon(QIcon(":/fmit/ui/images/module_capturedsound.svg"));
    setting_show->setChecked(true);

	setting_showWaveForm = new QAction(tr("Show Wave Form"), this);
	setting_showWaveForm->setCheckable(true);
	connect(setting_showWaveForm, SIGNAL(toggled(bool)), this, SLOT(update()));
	setting_showWaveForm->setChecked(true);
	m_popup_menu.addAction(setting_showWaveForm);

	setting_autoScale = new QAction(tr("Auto scale"), this);
	setting_autoScale->setCheckable(true);
	connect(setting_autoScale, SIGNAL(toggled(bool)), this, SLOT(update()));
	setting_autoScale->setChecked(false);
	m_popup_menu.addAction(setting_autoScale);

	QHBoxLayout* scaleFactorActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* scaleFactorActionTitle = new QLabel(tr("Scale factor"), &m_popup_menu);
	scaleFactorActionLayout->addWidget(scaleFactorActionTitle);

	setting_spinScaleFactor = new QSpinBox(&m_popup_menu);
	setting_spinScaleFactor->setMinimum(1);
	setting_spinScaleFactor->setMaximum(1000);
	setting_spinScaleFactor->setSingleStep(10);
	setting_spinScaleFactor->setToolTip(tr("Scale factor"));
	setting_spinScaleFactor->setValue(1000);
	connect(setting_spinScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(update()));
	scaleFactorActionLayout->addWidget(setting_spinScaleFactor);

	QWidget* scaleFactorActionWidget = new QWidget(&m_popup_menu);
	scaleFactorActionWidget->setLayout(scaleFactorActionLayout);

	QWidgetAction* scaleFactorAction = new QWidgetAction(&m_popup_menu);
	scaleFactorAction->setDefaultWidget(scaleFactorActionWidget);
	m_popup_menu.addAction(scaleFactorAction);

	QHBoxLayout* durationActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* durationActionTitle = new QLabel(tr("Duration"), &m_popup_menu);
	durationActionLayout->addWidget(durationActionTitle);

	setting_spinDuration = new QSpinBox(&m_popup_menu);
	setting_spinDuration->setMinimum(20);
	setting_spinDuration->setMaximum(120000);
	setting_spinDuration->setSingleStep(100);
	setting_spinDuration->setToolTip(tr("Duration"));
	setting_spinDuration->setValue(10000);
	connect(setting_spinDuration, SIGNAL(valueChanged(int)), this, SLOT(update()));
	durationActionLayout->addWidget(setting_spinDuration);

	QWidget* durationActionWidget = new QWidget(&m_popup_menu);
	durationActionWidget->setLayout(durationActionLayout);

	QWidgetAction* durationAction = new QWidgetAction(&m_popup_menu);
	durationAction->setDefaultWidget(durationActionWidget);
	m_popup_menu.addAction(durationAction);

	QHBoxLayout* maxHeightActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* maxHeightActionTitle = new QLabel(tr("Max height"), &m_popup_menu);
	maxHeightActionLayout->addWidget(maxHeightActionTitle);

	setting_spinMaxHeight = new QSpinBox(&m_popup_menu);
	setting_spinMaxHeight->setMinimum(10);
	setting_spinMaxHeight->setMaximum(100000);
	setting_spinMaxHeight->setSingleStep(1);
	setting_spinMaxHeight->setToolTip(tr("Max height"));
	setting_spinMaxHeight->setValue(50);
	maxHeightActionLayout->addWidget(setting_spinMaxHeight);

	QWidget* maxHeightActionWidget = new QWidget(&m_popup_menu);
	maxHeightActionWidget->setLayout(maxHeightActionLayout);

	QWidgetAction* maxHeightAction = new QWidgetAction(&m_popup_menu);
	maxHeightAction->setDefaultWidget(maxHeightActionWidget);
	m_popup_menu.addAction(maxHeightAction);
}

void GLGraph::save()
{
	s_settings->setValue("showWaveForm", setting_showWaveForm->isChecked());
	s_settings->setValue("autoScale", setting_autoScale->isChecked());
	s_settings->setValue("setting_spinScaleFactor", setting_spinScaleFactor->value());
	s_settings->setValue("spinDuration", setting_spinDuration->value());
	s_settings->setValue("spinMaxHeight", setting_spinMaxHeight->value());
}
void GLGraph::load()
{
	setting_showWaveForm->setChecked(s_settings->value("showWaveForm", setting_showWaveForm->isChecked()).toBool());
	setting_autoScale->setChecked(s_settings->value("autoScale", setting_autoScale->isChecked()).toBool());
	setting_spinScaleFactor->setValue(s_settings->value("setting_spinScaleFactor", setting_spinScaleFactor->value()).toInt());
	setting_spinDuration->setValue(s_settings->value("spinDuration", setting_spinDuration->value()).toInt());
	setting_spinMaxHeight->setValue(s_settings->value("spinMaxHeight", setting_spinMaxHeight->value()).toInt());
}
void GLGraph::clearSettings()
{
// 	cout << "GLGraph::clearSettings" << endl;
	s_settings->remove("showWaveForm");
	s_settings->remove("autoScale");
	s_settings->remove("setting_spinScaleFactor");
	s_settings->remove("spinDuration");
	s_settings->remove("spinMaxHeight");
}

void GLGraph::initializeGL()
{
	// Set the clear color to white
	glClearColor(1.0, 1.0, 1.0, 0.0);

	// glShadeModel( GL_FLAT );
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLoadIdentity();
}

void GLGraph::resizeGL( int w, int h )
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

	update_maxs();
}

void GLGraph::addValue(double v)
{
	m_pending_queue.push_front(v);
	m_queue.push_front(v);

	m_new_values = true;
}
void GLGraph::clearValues()
{
	m_queue.clear();
	m_maxs.clear();
}
void GLGraph::refreshGraph()
{
	int max_size = getLength();
	m_skip = max_size/width();
	if(m_skip<1)	m_skip = 1;

	while(!m_pending_queue.empty() && int(m_pending_queue.size()) >= m_skip)
	{
		double smin = m_pending_queue.back();
		double smax = m_pending_queue.back();
		for(int i=0; i<m_skip; i++)
		{
			m_queue.push_front(m_pending_queue.back());
			smin = min(smin, m_pending_queue.back());
			smax = max(smax, m_pending_queue.back());
			m_pending_queue.pop_back();
		}
		m_maxs.push_front(make_pair(smin, smax));
	}

	// drop unused data
	while(!m_maxs.empty() && int(m_maxs.size())>=width())
		m_maxs.pop_back();

	// drop unused data
	while(!m_queue.empty() && int(m_queue.size())>max_size)
		m_queue.pop_back();

	m_new_values = false;
}

void GLGraph::update_maxs()
{
//	cout << "GLGraph::update_maxs " << m_sampling_rate << endl;

	if(m_queue.empty())	return;

	m_maxs.clear();
	int max_size = getLength();
	m_skip = max_size/width();
	if(m_skip<1)	m_skip = 1;

	// drop unused data
	while(!m_queue.empty() && int(m_queue.size())>max_size)
		m_queue.pop_back();

	// computes maxs
	for(size_t j=0; j+m_skip<=m_queue.size(); j+=m_skip)
	{
		double smin, smax;
		smin = smax = m_queue[j+m_skip];
		for(size_t i=j; i<j+m_skip && i<m_queue.size(); i++)
		{
			smin = min(smin, m_queue[i]);
			smax = max(smax, m_queue[i]);
		}
		m_maxs.push_back(make_pair(smin, smax));
	}

	updateGL();
}

void GLGraph::base_paint(float graph_gray)
{
// 	cout << "GLGraph::base_paint " << m_queue.size() << ":" << m_maxs.size() << endl;

// 	cout<<"m_pending_queue="<<m_pending_queue.size()<<" m_queue="<<m_queue.size()<<" m_maxs="<<m_maxs.size()<<endl;

	int width = QGLWidget::width();

	// name
	glColor3f(0.75,0.75,0.75);
    m_font.setPixelSize(20);
    renderText(2, 20, tr("Captured Sound"), m_font);

	if(setting_showWaveForm->isChecked() && !m_queue.empty())
	{
		// horiz line
		glBegin(GL_LINES);
		float gray = 0.8;
		glColor3f(gray, gray, gray);
		glVertex2i(0, height()/2);
		glVertex2i(width, height()/2);
		glEnd();

		m_queue_amplitude = 0.0;
		double scale_factor;
		if(setting_autoScale->isChecked())
		{
			if(m_skip<2)
				for(size_t i=0; i<m_queue.size(); i++)
					m_queue_amplitude = max(m_queue_amplitude, abs(m_queue[i]));
			else
				for(size_t i=0; i<m_maxs.size(); i++)
					m_queue_amplitude = max(m_queue_amplitude, max(-m_maxs[i].first,m_maxs[i].second));
			scale_factor = 1.0/m_queue_amplitude;
		}
		else
			scale_factor = 1000.0/setting_spinScaleFactor->value();

		// paint the noise treshold
		glBegin(GL_LINES);
		int y = int((1+m_treshold*scale_factor)*height()/2);
		glColor3f(1.0,1.0,1.0);
		glVertex2i(0, y);
		glColor3f(1.0,0.5,0.5);
		glVertex2i(width, y);
		y = int((1-m_treshold*scale_factor)*height()/2);
		glColor3f(1.0,1.0,1.0);
		glVertex2i(0, y);
		glColor3f(1.0,0.5,0.5);
		glVertex2i(width, y);
		glEnd();

		// paint the wave form
		glColor3f(graph_gray, graph_gray, graph_gray);
		if(m_skip<2)
		{
			glBegin(GL_LINE_STRIP);

			glVertex2i(width-1, int((m_queue[0]*scale_factor)*height()/2 + height()/2));
			float istep = float(width)/m_queue.size();
			for(size_t i=1; i<m_queue.size(); i++)
				glVertex2i(width-1-int(i*istep), int((m_queue[i]*scale_factor)*height()/2 + height()/2));

			glEnd();
		}
		else
		{
			glBegin(GL_LINES);
            for(int i=0; i<int(m_maxs.size()); i++)
			{
				int x = width-1-i;
				int ymin = int((m_maxs[i].first*scale_factor)*height()/2 + height()/2);
				int ymax = int((m_maxs[i].second*scale_factor)*height()/2 + height()/2);
				glVertex2i(x, ymin);
				glVertex2i(x, ymax);
			}
			glEnd();
		}
	}
}

void GLGraph::paintGL()
{
	if(m_new_values)
		refreshGraph();

	glClear(GL_COLOR_BUFFER_BIT);

	base_paint();

	glFlush();
}


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

#include "GLFreqStruct.h"

#include <iostream>
using namespace std;
#include <qtooltip.h>
#include <qimage.h>
#include <qboxlayout.h>
#include <qwidgetaction.h>
#include <QPainter>
#include <Music/Music.h>
using namespace Music;

GLFreqStruct::GLFreqStruct(QWidget* parent)
: QOpenGLWidget(parent)
, View(tr("Harmonics"), this)
, m_font("Helvetica")
, m_components_max(1.0)
{
	// settings
    setting_show->setIcon(QIcon(":/fmit/ui/images/module_harmonics.svg"));
    setting_show->setChecked(false);
	hide();

	QHBoxLayout* numComponentsActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* numComponentsActionTitle = new QLabel(tr("Number of harmonics"), &m_popup_menu);
	numComponentsActionLayout->addWidget(numComponentsActionTitle);

	setting_spinNumComponents = new QSpinBox(&m_popup_menu);
	setting_spinNumComponents->setMinimum(1);
	setting_spinNumComponents->setMaximum(128);
	setting_spinNumComponents->setSingleStep(1);
	setting_spinNumComponents->setValue(16);
	m_components.resize(setting_spinNumComponents->value());
	setting_spinNumComponents->setToolTip(tr("Number of harmonics"));
	connect(setting_spinNumComponents, SIGNAL(valueChanged(int)), this, SLOT(spinNumComponentsChanged(int)));
	numComponentsActionLayout->addWidget(setting_spinNumComponents);

	QWidget* numComponentsActionWidget = new QWidget(&m_popup_menu);
	numComponentsActionWidget->setLayout(numComponentsActionLayout);

	QWidgetAction* numComponentsAction = new QWidgetAction(&m_popup_menu);
	numComponentsAction->setDefaultWidget(numComponentsActionWidget);
	m_popup_menu.addAction(numComponentsAction);

	for(size_t i=0; i<m_components.size(); i++)
		m_components[i] = 0.0;
}

void GLFreqStruct::save()
{
	s_settings->setValue("spinNumComponents", setting_spinNumComponents->value());
}
void GLFreqStruct::load()
{
	setting_spinNumComponents->setValue(s_settings->value("spinNumComponents", setting_spinNumComponents->value()).toInt());
}
void GLFreqStruct::clearSettings()
{
	s_settings->remove("spinNumComponents");
}

void GLFreqStruct::spinNumComponentsChanged(int num)
{
	m_components.resize(num);
}

void GLFreqStruct::initializeGL()
{
	// Set the clear color to black
	glClearColor(1.0, 1.0, 1.0, 0.0);

	glShadeModel( GL_FLAT );
//	glShadeModel( GL_SMOOTH );

	glLoadIdentity();
}

void GLFreqStruct::paintGL()
{
	if(m_components_max==0.0)	return;

	glClear(GL_COLOR_BUFFER_BIT);

	int scale_height = 12;
    int s = 2+fontMetrics().horizontalAdvance("-40dB");

//	double scale_factor = 1.0;
//	if(m_components_max>0.0)
//		scale_factor = 1.0/m_components_max;

    // bars
	glBegin(GL_QUADS);
    float step = float(width()-s)/m_components.size();
	int space = (step>2)?1:0;
	for(size_t i=0; i<m_components.size(); i++)
	{
		glColor3f(0.4, 0.4, 0.5);
//        int y = int( (scale_factor*m_components[i]) * (height()-scale_height)) + scale_height;
        int y = height() + int( (m_components[i]-m_components_max) * (height()-scale_height)/50);

//        std::cout << i << ":" << m_components_max << " " << m_components[i] << " y=" << y << std::endl;

        if(y>0)
		{
            glVertex2i(int(i*step)+s, scale_height);
            glVertex2i(int(i*step)+s, y);
            glVertex2i(int((i+1)*step)+s-space, y);
            glVertex2i(int((i+1)*step)+s-space, scale_height);
		}
	}
	glEnd();

    // horiz lines
    if(2*height()/50>2)
    {
        glBegin(GL_LINES);
        float gray = 0.875;
        glColor3f(gray, gray, gray);
        for(int h=0; h<50; h+=2)
        {
            glVertex2i(s, height()-h*(height()-scale_height)/50);
            glVertex2i(width(), height()-h*(height()-scale_height)/50);
        }
        glEnd();
    }
    if(10*height()/50>2)
    {
        glBegin(GL_LINES);
        float gray = 0.75;
        glColor3f(gray, gray, gray);
        for(int h=0; h<50; h+=10)
        {
            glVertex2i(s, height()-h*(height()-scale_height)/50);
            glVertex2i(width(), height()-h*(height()-scale_height)/50);
        }
        glEnd();
    }

    glColor3f(0.5f,0.5f,0.5f);
    QPainter painter(this);
    m_font.setPixelSize(14);
    painter.setFont(m_font);
    QFontMetrics fm(m_font);
    int dy = -fm.xHeight()/2;
    string sfraq = "-10dB";
    painter.drawText(2, 10*(height()-scale_height)/50-dy, QString(sfraq.c_str()));
    sfraq = "-20dB";
    painter.drawText(2, 20*(height()-scale_height)/50-dy, QString(sfraq.c_str()));
    sfraq = "-30dB";
    painter.drawText(2, 30*(height()-scale_height)/50-dy, QString(sfraq.c_str()));
    sfraq = "-40dB";
    painter.drawText(2, 40*(height()-scale_height)/50-dy, QString(sfraq.c_str()));

    // scale
    m_font.setPixelSize(10);
    glColor3f(0,0,0);
	for(size_t i=0; i<m_components.size(); i++)
    	painter.drawText(int((i+0.5)*step)+s-3, height()-2, QString::number(i+1));

    // name
    glColor3f(0.75,0.75,0.75);
    m_font.setPixelSize(20);
    painter.drawText(2, 20, tr("Harmonics' amplitude"));
    painter.end();

    glFlush();
}

void GLFreqStruct::resizeGL( int w, int h )
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


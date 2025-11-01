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

// TODO zoom with a rectangle

#include "GLFT.h"

#include <iostream>
using namespace std;
#include <qtooltip.h>
#include <qimage.h>
#include <qevent.h>
#include <qboxlayout.h>
#include <qwidgetaction.h>
#include <QPainter>
#include <Music/Music.h>
#include <Music/SPWindow.h>
using namespace Music;
#include <CppAddons/CAMath.h>
using namespace Math;

GLFT::GLFT(QWidget* parent)
: QOpenGLWidget(parent)
, View(tr("Fourier Transform"), this)
, m_font("Helvetica")
, m_components_max(1.0)
{
	m_start_move_mouse = true;

	m_os = 2;					// over sampling factor

	// settings
    setting_show->setIcon(QIcon(":/fmit/ui/images/module_fourier.svg"));
    setting_show->setChecked(false);
	hide();

	setting_db_scale = new QAction(tr("dB scale"), this);
	setting_db_scale->setCheckable(true);
	connect(setting_db_scale, SIGNAL(toggled(bool)), this, SLOT(dBScaleChanged(bool)));
	setting_db_scale->setChecked(true);
	m_popup_menu.addAction(setting_db_scale);
	resetaxis();

	QHBoxLayout* sizeActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* sizeActionTitle = new QLabel(tr("Size"), &m_popup_menu);
	sizeActionLayout->addWidget(sizeActionTitle);

	setting_winlen = new QSpinBox(&m_popup_menu);
	setting_winlen->setObjectName("setting_winlen");
	setting_winlen->setMinimum(1);
	setting_winlen->setMaximum(1000);
	setting_winlen->setSingleStep(1);
	setting_winlen->setValue(20);
	setting_winlen->setSuffix(" ms");
	setting_winlen->setToolTip(tr("window length"));
	connect(setting_winlen, SIGNAL(valueChanged(int)), this, SLOT(spinWinLengthChanged(int)));
	sizeActionLayout->addWidget(setting_winlen);

	QWidget* sizeActionWidget = new QWidget(&m_popup_menu);
	sizeActionWidget->setLayout(sizeActionLayout);

	QWidgetAction* sizeAction = new QWidgetAction(&m_popup_menu);
	sizeAction->setDefaultWidget(sizeActionWidget);
	m_popup_menu.addAction(sizeAction);

	QWidgetAction* helpCaption01 = new QWidgetAction(&m_popup_menu);
	helpCaption01->setDefaultWidget(new Title(tr("- Press left mouse button to move the view"), &m_popup_menu));
	m_popup_menu.addAction(helpCaption01);
	QWidgetAction* helpCaption02 = new QWidgetAction(&m_popup_menu);
	helpCaption02->setDefaultWidget(new Title(tr("- Press SHIFT key and left mouse button to zoom in and out"), &m_popup_menu));
	m_popup_menu.addAction(helpCaption02);
	QWidgetAction* helpCaption03 = new QWidgetAction(&m_popup_menu);
	helpCaption03->setDefaultWidget(new Title(tr("- Double-click to reset the view"), &m_popup_menu));
	m_popup_menu.addAction(helpCaption03);

	s_settings->add(setting_winlen);

	spinWinLengthChanged(setting_winlen->value());
}

void GLFT::refreshGraph()
{
	while(int(buff.size())>m_plan.size())
		buff.pop_back();
}

void GLFT::setSamplingRate(int sr)
{
	m_maxf=sr/2;
}

void GLFT::resetaxis()
{
	m_minf=0;
	m_maxf=Music::GetSamplingRate()/2; // sr is surely -1 because not yet defined

	if(setting_db_scale->isChecked())
	{
		m_minA = -50; // [dB]
		m_maxA = 100; // [dB]
	}
	else
	{
		m_maxA = 1; // [amplitude]
	}
}
void GLFT::dBScaleChanged(bool db)
{
    Q_UNUSED(db)

	resetaxis();
	update();
}

void GLFT::spinWinLengthChanged(int num)
{
	if(Music::GetSamplingRate()>0)
	{
		// Create window
		int winlen = int(num/1000.0*Music::GetSamplingRate());
		win = hann(winlen);
		double win_sum = 0.0;
		// normalize the window in energy
		for(size_t i=0; i<win.size(); i++)
			win_sum += win[i];
		for(size_t i=0; i<win.size(); i++)
			win[i] *= 2*win.size()/win_sum; // 2* because the positive freq are half of the energy

		// Create FFTW3 plan
		int fftlen=1;
		while(fftlen<winlen) fftlen *= 2;
		fftlen *= pow(2,m_os);
		assert(fftlen<int(Music::GetSamplingRate()));
		m_plan.resize(fftlen);
		m_components.resize(m_plan.size()/2);

        cout << "GLFT: INFO: window length=" << win.size() << "ms FFT length=" << m_plan.size() << endl;
	}
}

void GLFT::initializeGL()
{
	// Set the clear color to black
	glClearColor(1.0, 1.0, 1.0, 0.0);

	glShadeModel( GL_FLAT );
//	glShadeModel( GL_SMOOTH );

	glLoadIdentity();
}

void GLFT::mousePressEvent(QMouseEvent* e)
{
	m_start_move_mouse = true;
	m_press_x = e->position().x();
	m_press_y = e->position().y();
	m_press_minf = m_minf;
	m_press_maxf = m_maxf;

	double f = (m_maxf-m_minf)*double(m_press_x)/width()+m_minf;
	m_text = tr("Frequency %1 [Hz]").arg(f);
	update();
}
void GLFT::mouseDoubleClickEvent(QMouseEvent* e)
{
    Q_UNUSED(e)

	m_start_move_mouse = true;
	m_minf=0;
	m_maxf=Music::GetSamplingRate()/2; // sr is surely -1 because not yet defined
	resetaxis();
	update();
}
void GLFT::mouseMoveEvent(QMouseEvent* e)
{
	static int old_x;
	static int old_y;
	if(m_start_move_mouse)
	{
		old_x = e->position().x();
		old_y = e->position().y();
		m_start_move_mouse = false;
	}
	int dx = e->position().x() - old_x;
	int dy = e->position().y() - old_y;

	if(Qt::LeftButton & e->buttons())
	{
		if(Qt::ShiftModifier & e->modifiers())
		{
			double f = (m_maxf-m_minf)*double(m_press_x)/width()+m_minf;
			double zx = double(m_press_x-e->position().x())/width();
			zx = pow(8, zx);
			m_minf = f - zx*(f-m_press_minf);
			m_maxf = f + zx*(m_press_maxf-f);
		}
		else
		{
			m_minf -= (m_maxf-m_minf)*dx/width();
			m_maxf -= (m_maxf-m_minf)*dx/width();

			if(setting_db_scale->isChecked())
			{
				m_minA += (m_maxA-m_minA)*dy/height();
				m_maxA += (m_maxA-m_minA)*dy/height();
			}
			else
				m_maxA -= m_maxA*double(dy)/height();
		}

		update();
	}

	old_x = e->position().x();
	old_y = e->position().y();
}

void GLFT::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);

	if(win.size()>0 && int(buff.size())>=m_plan.size())
	{
		// Use last samples
		while(int(buff.size())>m_plan.size())
			buff.pop_back();

		int sr = Music::GetSamplingRate();

//		cout << m_plan.size() << endl;

		// name
		glColor3f(0.75,0.75,0.75);
		QPainter painter(this);
        m_font.setPixelSize(20);
        painter.setFont(m_font);
        painter.drawText(2, 20, tr("Fourier Transform"));
        painter.end();

		for(int i=0; i<int(win.size()); i++)
			m_plan.in[i] = buff[i]*win[i];
        for(int i=int(win.size()); i<int(m_plan.in.size()); i++)
			m_plan.in[i] = 0.0;

		m_plan.execute();

		for(int i=0; i<int(m_components.size()); i++)
			m_components[i] = mod(m_plan.out[i]);

//		cout << "m_minA=" << m_minA << " m_maxA=" << m_maxA << endl;
		double y;
		glBegin(GL_LINE_STRIP);
		glColor3f(0.4, 0.4, 0.5);
        for(int x=0; x<width(); x++)
		{
			int index = int(0.5+(m_minf+(m_maxf-m_minf)*double(x)/width())*m_components.size()/(sr/2.0));
			if(index<0) index=0;
            else if(index>=int(m_components.size())) index=int(m_components.size());
			y = m_components[index];
			if(setting_db_scale->isChecked())
				y = height()*(lp(y)-m_minA)/(m_maxA-m_minA);
			else
				y = height()*y*m_maxA;

			glVertex2i(x, int(y));
		}

		glEnd();

		// scale
		/*glColor3f(0,0,0);
		for(size_t i=0; i<m_components.size(); i++)
		{
			glRasterPos2i(int((i+0.5)*step)-3, 2);

//			string str = StringAddons::toString(i+1);
			string str = QString::number(i+1).toStdString();

			for(size_t i = 0; i < str.length(); i++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (unsigned char)str[i]);
		}*/
	}

	// Text
	if(m_text.length()>0)
	{
		glColor3f(0,0,0);

        QPainter painter(this);
        m_font.setPixelSize(14);
        painter.setFont(m_font);
        painter.drawText(width()/2, 12, m_text);
        painter.end();
	}

	glFlush();
}

void GLFT::resizeGL( int w, int h )
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

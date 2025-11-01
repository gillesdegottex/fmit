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

#include "GLVolumeHistory.h"

#include <iostream>
using namespace std;
#include <qtimer.h>
#include <qimage.h>
#include <QPainter>
#include <Music/Music.h>

GLVolumeHistory::Note::Note(int h)
: ht(h)
{
}
GLVolumeHistory::Note::Note(int h, int num, int den)
: ht(h)
{
	factor = QString::number(num)+"/"+QString::number(den);
}
GLVolumeHistory::Note::Note(int h, float cents)
: ht(h)
{
	factor = QString::number(cents);
}
QString GLVolumeHistory::Note::getName() const
{
	return QString::fromStdString(Music::h2n(ht))+factor;
}

GLVolumeHistory::GLVolumeHistory(QWidget* parent)
: QOpenGLWidget(parent)
, View(tr("Volume history"), this)
, m_font("Helvetica")
{
	setting_show->setCheckable(true);
    setting_show->setIcon(QIcon(":/fmit/ui/images/module_volumehistory.svg"));
    setting_show->setChecked(false);
	hide();

	m_volume_treshold = -100;

	setting_keep = new QAction(tr("Keep previous notes"), this);
	setting_keep->setCheckable(true);
	setting_keep->setChecked(false);
	connect(setting_keep, SIGNAL(toggled(bool)), this, SLOT(keepPreviousNotes(bool)));
    m_popup_menu.addAction(setting_keep);
}

void GLVolumeHistory::save()
{
	s_settings->setValue("keep", setting_keep->isChecked());
}
void GLVolumeHistory::load()
{
	setting_keep->setChecked(s_settings->value("keep", setting_keep->isChecked()).toBool());
}
void GLVolumeHistory::clearSettings()
{
	s_settings->remove("keep");
}

void GLVolumeHistory::addNote(GLVolumeHistory::Note note)
{
	m_notes.push_back(note);

	if(!setting_keep->isChecked())
		while(m_notes.size()>1)
			m_notes.pop_front();
}
void GLVolumeHistory::addVolume(float vol)
{
	m_notes.back().volumes.push_back(vol);
}

void GLVolumeHistory::keepPreviousNotes(bool keep)
{
	if(!keep)
		while(m_notes.size()>1)
			m_notes.pop_front();
}

void GLVolumeHistory::setVolumeTreshold(int treshold)
{
	m_volume_treshold = treshold;
}

void GLVolumeHistory::initializeGL()
{
	// Set the clear color to black
	glClearColor(1.0, 1.0, 1.0, 0.0);

	// glShadeModel( GL_FLAT );
	glShadeModel( GL_SMOOTH );

	glLoadIdentity();
}

void GLVolumeHistory::paintGL()
{
// 	cout << "GLVolumeHistory::paintGL " << m_notes.size() << endl;

	glClear(GL_COLOR_BUFFER_BIT);

	glLineWidth(1.0f);

	// name
	glColor3f(0.75,0.75,0.75);
	QPainter painter(this);
    m_font.setPixelSize(20);
    painter.setFont(m_font);
    painter.drawText(2, 20, tr("Volume"));
    painter.end();

	int s = 2+fontMetrics().horizontalAdvance("50%");

	// horiz lines
	if(2*height()/50>2)
	{
		glBegin(GL_LINES);
		float gray = 0.875;
		glColor3f(gray, gray, gray);
		for(int h=2; h<50; h+=2)
		{
			glVertex2i(s, h*height()/50);
			glVertex2i(width(), h*height()/50);
		}
		glEnd();
	}
	if(10*height()/50>2)
	{
		glBegin(GL_LINES);
		float gray = 0.75;
		glColor3f(gray, gray, gray);
		for(int h=10; h<50; h+=10)
		{
			glVertex2i(s, h*height()/50);
			glVertex2i(width(), h*height()/50);
		}
		glEnd();
	}

	glColor3f(0.5f,0.5f,0.5f);
	painter.begin(this);
    m_font.setPixelSize(14);
    painter.setFont(m_font);
    QFontMetrics fm(m_font);
    int dy = -fm.xHeight()/2;
    string sfraq = "[dB]";
    painter.drawText(2, height()-2, QString(sfraq.c_str()));
    sfraq = "-10";
    painter.drawText(2, 10*height()/50-dy, QString(sfraq.c_str()));
	sfraq = "-20";
    painter.drawText(2, 20*height()/50-dy, QString(sfraq.c_str()));
	sfraq = "-30";
    painter.drawText(2, 30*height()/50-dy, QString(sfraq.c_str()));
	sfraq = "-40";
    painter.drawText(2, 40*height()/50-dy, QString(sfraq.c_str()));
    painter.end();

	glColor3f(1.0,0.5,0.5);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex2f(s, height()+height()*m_volume_treshold/50);
	glVertex2f(width(), height()+height()*m_volume_treshold/50);
	glEnd();

	// volumes
	if(!m_notes.empty())
	{
		int total_size = 0;
		for(size_t i=0; i<m_notes.size(); i++)
            total_size += int(m_notes[i].volumes.size())-1;

		float step = float(width()-s)/total_size;

//		cout << "total_size=" << total_size << " step=" << step << endl;

		int curr_total = 0;
		for(size_t i=0; i<m_notes.size(); i++)
		{
			float x = s+step*curr_total;

			// if it's not the first, add a separation
			if(i>0)
			{
				glColor3f(0.75,0.75,0.75);
				glLineWidth(1.0f);
				glBegin(GL_LINES);
				glVertex2f(x, 0);	glVertex2f(x, height());
				glEnd();
			}

			// the note name
			string str = Music::h2n(m_notes[i].ht);
			glColor3f(0.0,0.0,1.0);
			painter.begin(this);
            m_font.setPixelSize(14);
            painter.setFont(m_font);
            painter.drawText(x+2, height()-2, QString(str.c_str()));
            painter.end();

			// draw the volume graph
			if(!m_notes[i].volumes.empty())
			{
// 				glColor3f(0.33,0.33,1.0);
				glColor3f(0.0f,0.0f,0.0f);
				glLineWidth(2.0f);
				glBegin(GL_LINE_STRIP);
				for(int j=0; j<int(m_notes[i].volumes.size()); j++)
					glVertex2f(x+j*step, height()+height()*m_notes[i].volumes[j]/50.0);
				glEnd();
			}

            curr_total += int(m_notes[i].volumes.size())-1;
		}
	}

	glFlush();
}

void GLVolumeHistory::resizeGL( int w, int h )
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


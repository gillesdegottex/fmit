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

#include "GLErrorHistory.h"

#include <iostream>
#include <limits>
using namespace std;
#include <qtimer.h>
#include <qtooltip.h>
#include <qimage.h>
#include <qwidgetaction.h>
#include <qboxlayout.h>
#include <QPainter>
#include <Music/Music.h>

void GLErrorHistory::Note::init()
{
	min_err = numeric_limits<float>::max();
	max_err = -numeric_limits<float>::max();
	avg_err = 0.0f;
}
GLErrorHistory::Note::Note(int h)
	: ht(h)
{
	init();
}
GLErrorHistory::Note::Note(int h, int num, int den)
	: ht(h)
{
	init();
	factor = QString::number(num)+"/"+QString::number(den);
}
GLErrorHistory::Note::Note(int h, float cents)
	: ht(h)
{
	init();
	factor = QString::number(cents);
}
QString GLErrorHistory::Note::getName() const
{
	return QString::fromStdString(Music::h2n(ht))+factor;
}
void GLErrorHistory::Note::addError(float err)
{
	min_err = (err<min_err)?err:min_err;
	max_err = (err>max_err)?err:max_err;

	if(!errors.empty())
		avg_err *= errors.size();

	errors.push_back(err);

	avg_err += err;
	avg_err /= errors.size();
}

GLErrorHistory::GLErrorHistory(QWidget* parent)
: QOpenGLWidget(parent)
, View(tr("Error history"), this)
, m_font("Helvetica")
{
    // settings
    setting_show->setIcon(QIcon(":/fmit/ui/images/module_errorhistory.svg"));
    setting_show->setChecked(true);

	setting_keep = new QAction(tr("Keep previous notes"), this);
	setting_keep->setCheckable(true);
	setting_keep->setChecked(false);
	connect(setting_keep, SIGNAL(toggled(bool)), this, SLOT(keepPreviousNotes(bool)));
	m_popup_menu.addAction(setting_keep);

    
    /* 
     * AutoScroll
     * If activated, only the last N errors are plotted.
     */ 
	setting_autoScroll = new QAction(tr("AutoScroll"), this);
	setting_autoScroll->setCheckable(true);
	setting_autoScroll->setChecked(false);
	connect(setting_autoScroll, SIGNAL(toggled(bool)), this, SLOT(autoScroll(bool)));
	m_popup_menu.addAction(setting_autoScroll);
    
    QHBoxLayout* autoScrollActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* autoScrollActionTitle = new QLabel(tr("AutoScroll Scale"), &m_popup_menu);
	autoScrollActionLayout->addWidget(autoScrollActionTitle);

	setting_hScale = new QSpinBox(&m_popup_menu);
	setting_hScale->setMinimum(1);
	setting_hScale->setMaximum(10000);
	setting_hScale->setSingleStep(1);
	setting_hScale->setValue(100);
	setting_hScale->setToolTip(tr("AutoScroll Scale"));
    setting_hScale->setEnabled(false);
    autoScrollActionLayout->addWidget(setting_hScale);
    
    QWidget* autoScrollActionWidget = new QWidget(&m_popup_menu);
    autoScrollActionWidget->setLayout(autoScrollActionLayout);

    QWidgetAction* autoScrollAction = new QWidgetAction(&m_popup_menu);
    autoScrollAction->setDefaultWidget(autoScrollActionWidget);
    m_popup_menu.addAction(autoScrollAction);




	setting_useCents = new QAction(tr("Use cents"), this);
	setting_useCents->setCheckable(true);
	setting_useCents->setChecked(true);
	connect(setting_useCents, SIGNAL(toggled(bool)), this, SLOT(update()));
	m_popup_menu.addAction(setting_useCents);


	QHBoxLayout* scaleActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* scaleActionTitle = new QLabel(tr("Scale range"), &m_popup_menu);
	scaleActionLayout->addWidget(scaleActionTitle);

	setting_spinScale = new QSpinBox(&m_popup_menu);
	setting_spinScale->setMinimum(5);
	setting_spinScale->setMaximum(50);
	setting_spinScale->setSingleStep(1);
	setting_spinScale->setValue(50);
	setting_spinScale->setToolTip(tr("Scale range (in cents)"));
	connect(setting_spinScale, SIGNAL(valueChanged(int)), this, SLOT(update()));
	scaleActionLayout->addWidget(setting_spinScale);

	QWidget* scaleActionWidget = new QWidget(&m_popup_menu);
	scaleActionWidget->setLayout(scaleActionLayout);

	QWidgetAction* scaleAction = new QWidgetAction(&m_popup_menu);
	scaleAction->setDefaultWidget(scaleActionWidget);
	m_popup_menu.addAction(scaleAction);
}

void GLErrorHistory::save()
{
	s_settings->setValue("keep", setting_keep->isChecked());
	s_settings->setValue("useCents", setting_useCents->isChecked());
	s_settings->setValue("spinScale", setting_spinScale->value());
}
void GLErrorHistory::load()
{
	setting_keep->setChecked(s_settings->value("keep", setting_keep->isChecked()).toBool());
	setting_useCents->setChecked(s_settings->value("useCents", setting_useCents->isChecked()).toBool());
	setting_spinScale->setValue(s_settings->value("spinScale", setting_spinScale->value()).toInt());
}
void GLErrorHistory::clearSettings()
{
	s_settings->remove("keep");
	s_settings->remove("useCents");
	s_settings->remove("spinScale");
}

void GLErrorHistory::addNote(GLErrorHistory::Note note)
{
	m_notes.push_back(note);

	if(!setting_keep->isChecked())
		while(m_notes.size()>1)
			m_notes.pop_front();
}
void GLErrorHistory::addError(float err)
{
	m_notes.back().addError(err);
    
    // If AutoScroll is enabled, old errors may be deleted
    if (setting_autoScroll->isChecked() )
    {   
        int size = 0;
        for(int i = m_notes.size() - 1; i >= 0; i--)
        {
            size += m_notes[i].errors.size() - 1;
            if (size > setting_hScale->value())
            {
                // Delete oldest error from this note
                m_notes[i].errors.pop_front();
                // Delete note, if it is empty
                if (m_notes[i].errors.size() == 0)
                    m_notes.erase(m_notes.begin() + i);
            }
        }
    }
}

void GLErrorHistory::keepPreviousNotes(bool keep)
{
	if(!keep)
		while(m_notes.size()>1)
			m_notes.pop_front();
}

void GLErrorHistory::autoScroll(bool autoScroll)
{
    // AutoScroll-Spinbox can be edited iff AutoScroll is enabled
    setting_hScale->setEnabled(autoScroll);
}

void GLErrorHistory::initializeGL()
{
    // Set the clear color to black
    glClearColor(1.0, 1.0, 1.0, 0.0);

    // glShadeModel( GL_FLAT );
    glShadeModel( GL_SMOOTH );

    glLoadIdentity();
}

void GLErrorHistory::drawTicksCent(int r, int ticks_size)
{
	// only work within range that is a pure multiple of r
	float range = int(setting_spinScale->value()/r)*r;

	float scale = 50.0f/setting_spinScale->value();
	if((height()-ticks_size)*r/100.0f*scale>2)
	{
		for(float i=-range; i<=range; i+=r)
		{
            int y = height()/2 + int(height()*i/100.0f*scale);
            glVertex2i(ticks_size, y);
            glVertex2i(width(), y);
		}
	}
}
void GLErrorHistory::drawTextTickCent(int r, int dy)
{
    Q_UNUSED(dy)

	// only work within range that is a pure multiple of r
	int range = int(setting_spinScale->value()/r)*r;

    QPainter painter(this);
    m_font.setPixelSize(14);
    painter.setFont(m_font);
    QFontMetrics fm(m_font);

	float scale = 50.0f/setting_spinScale->value();
	QString txt;
    for(int i=-range; i<=range; i+=r)
	{
		txt = QString::number(i);
		if(i>=0) txt = QString("  ")+txt;
		if(i==0) txt = QString("  ")+txt;
        int y = height()/2 - int(height()*i/100.0f*scale);
        if(y>fm.xHeight() && y<height()-fm.xHeight())
            painter.drawText(2, y+ fm.xHeight()/2, txt);
    }
    painter.end();
}

//void GLErrorHistory::paintEvent(QPaintEvent * event){
//    QPainter painter;
//    painter.begin(this);
//    QBrush background = QBrush(Qt::white);
//    painter.fillRect(rect(), background);
//    painter.end();
//}

void GLErrorHistory::paintGL()
{
// 	cout << "GLErrorHistory::paintGL " << m_notes.size() << endl;

    glClear(GL_COLOR_BUFFER_BIT);

    glLineWidth(1.0f);

    // name
    glColor3f(0.75,0.75,0.75);
    QPainter painter(this);
    m_font.setPixelSize(20);
    painter.setFont(m_font);
    painter.drawText(2, 20, tr("Error"));
    painter.end();

    int char_size = 9;
    int ticks_size = 2+3*char_size;
    int dy = char_size/2;

    // horiz lines
    if(setting_useCents->isChecked())
    {
        glBegin(GL_LINES);
        float gray = 0.87;
// 		glColor3f(gray, gray, gray);
// 		drawTicksCent(1, ticks_size);
        gray = 0.875;
        glColor3f(gray, gray, gray);
        drawTicksCent(2, ticks_size);
        gray = 0.75;
        glColor3f(gray, gray, gray);
        drawTicksCent(10, ticks_size);
        glEnd();
    }
    else
    {
        glBegin(GL_LINES);
            float gray = 0.5;
            glColor3f(gray, gray, gray);
            glVertex2i(ticks_size,  height()/2);
            glVertex2i(width(),  height()/2);
            gray = 0.75;
            glColor3f(gray, gray, gray);
            glVertex2i(ticks_size,  height()/4);
            glVertex2i(width(),  height()/4);
            glVertex2i(ticks_size,  3*height()/4);
            glVertex2i(width(),  3*height()/4);
            gray = 0.87;
            glColor3f(gray, gray, gray);
            glVertex2i(ticks_size,  height()/8);
            glVertex2i(width(),  height()/8);
            glVertex2i(ticks_size,  7*height()/8);
            glVertex2i(width(),  7*height()/8);
            glVertex2i(ticks_size,  3*height()/8);
            glVertex2i(width(),  3*height()/8);
            glVertex2i(ticks_size,  5*height()/8);
            glVertex2i(width(),  5*height()/8);
        glEnd();
    }

    // text marks
    float gray = 0.5;
    glColor3f(gray, gray, gray);
    if(setting_useCents->isChecked())
    {
        int grad = 10;
        if(setting_spinScale->value() <= 25) grad=5;
        if(setting_spinScale->value() <= 10) grad=1;
        drawTextTickCent(grad, dy);
    }
    else
    {
        painter.begin(this);
        m_font.setPixelSize(14);
        painter.setFont(m_font);
        QFontMetrics fm(m_font);
        string sfraq, sufraq;
        sufraq = string("1/")+QString::number(int(50/setting_spinScale->value())*2).toStdString();
        sfraq = string("-")+sufraq;
        painter.drawText(2, 3*height()/4-dy+fm.xHeight(), QString(sfraq.c_str()));
        sfraq = string("+")+sufraq;
        painter.drawText(2, height()/4-dy+fm.xHeight(), QString(sfraq.c_str()));

        sufraq = string("1/")+QString::number(int(50/setting_spinScale->value())*4).toStdString();
        sfraq = string("-")+sufraq;
        painter.drawText(2, 5*height()/8-dy+fm.xHeight(), QString(sfraq.c_str()));
        sfraq = string("+")+sufraq;
        painter.drawText(2, 3*height()/8-dy+fm.xHeight(), QString(sfraq.c_str()));
        painter.end();
    }

    // errors
    if(!m_notes.empty())
    {
        float step; // width of one error
        
        if (setting_autoScroll->isChecked())
            step = float(width() - ticks_size) / setting_hScale->value();
        else
        {
            int total_size = 0;
            for(size_t i=0; i<m_notes.size(); i++)
               total_size += int(m_notes[i].errors.size())-1;
            step = float(width() - ticks_size) / total_size;
        }

//		cout << "total_size=" << total_size << " step=" << step << endl;



        int curr_total = 0;
        for(size_t i=0; i<m_notes.size(); i++)
        {
            float x = ticks_size+step*curr_total;

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
            string str = m_notes[i].getName().toStdString();
            glColor3f(0.0,0.0,1.0);
            painter.begin(this);
            m_font.setPixelSize(14);
            painter.setFont(m_font);
            painter.drawText(x+2, height()-2, QString(str.c_str()));
            painter.end();

            // draw the error graph
            glColor3f(0.0f,0.0f,0.0f);
            glLineWidth(2.0f);
            glBegin(GL_LINE_STRIP);

            float scale;
            if(setting_useCents->isChecked())
                scale = 50.0f/setting_spinScale->value();
            else
                scale = int(50/setting_spinScale->value());

            glVertex2f(x, int((scale*m_notes[i].errors[0])*height()) + height()/2);
            for(int j=1; j<int(m_notes[i].errors.size()); j++)
                glVertex2f(x+j*step, (scale*m_notes[i].errors[j])*height() + height()/2);

            glEnd();

            curr_total += int(m_notes[i].errors.size())-1;
        }
    }

    glFlush();
}

void GLErrorHistory::resizeGL( int w, int h )
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


// Copyright 2007 "Gilles Degottex"

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

#include "GLStatistics.h"

#include <iostream>
#include <limits>
using namespace std;
#include <qtimer.h>
#include <qtooltip.h>
#include <qimage.h>
#include <qboxlayout.h>
#include <qwidgetaction.h>
#include <QPainter>
#include <Music/Music.h>

void GLStatistics::AverageNote::init()
{
	err_mean = 0.0;
	err_std = 0.0;
}
GLStatistics::AverageNote::AverageNote()
	: ht(-1000)
{
	init();
}
GLStatistics::AverageNote::AverageNote(int h)
	: ht(h)
{
	init();
}
GLStatistics::AverageNote::AverageNote(int h, int num, int den)
	: ht(h)
{
	init();
	factor = QString::number(num)+"/"+QString::number(den);
}
GLStatistics::AverageNote::AverageNote(int h, float cents)
	: ht(h)
{
	init();
	factor = QString::number(cents);
}
QString GLStatistics::AverageNote::getName() const
{
	return QString::fromStdString(Music::h2n(ht))+factor;
}
void GLStatistics::AverageNote::addErr(float err)
{
	// mean estimation
	if(!errs.empty())
		err_mean *= errs.size();
	errs.push_back(err);
 	err_mean += err;
 	err_mean /= errs.size();

	// standard deviation estimation
	err_std = 0.0;
	for(size_t i=0; i<errs.size(); i++)
	{
		float de = errs[i]-err_mean;
		err_std += de*de;
	}
	if(errs.size()>1)
	{
		err_std /= errs.size()-1;		// unbiased estimation of the variance
		err_std = sqrt(err_std);
	}
}

void GLStatistics::addNote(int ht, float err)
{
	m_current_ht = ht;

	if(isHidden() && !setting_keep_hidden->isChecked())
		return;

	if(ht<setting_scale_min->minimum() || ht>setting_scale_max->maximum())
		return;

// 	cout << "ht=" << ht << endl;

	if(!setting_scale_auto->isChecked())
	{
		resizeScale();

		// drop ht outside of scale
		if(ht<setting_scale_min->value() || ht>setting_scale_max->value())
			return;

		// add note
		m_avg_notes[ht-setting_scale_min->value()].addErr(err);
	}
	else
	{
		if(m_avg_notes.empty())
		{
			m_avg_notes.resize(1);
			m_avg_notes[0] = AverageNote(ht);
			setting_scale_min->setValue(ht);
			setting_scale_max->setValue(ht);
		}
		else if(ht<setting_scale_min->value())
		{
// 			cout << "min: ";
			int min_ht = ht;
			vector<AverageNote> avg_notes(setting_scale_max->value()-min_ht+1);
			for(int i=0; i<setting_scale_min->value()-min_ht; i++)
				avg_notes[i].ht = min_ht+i;
			for(size_t i=0; i<m_avg_notes.size(); i++)
				avg_notes[setting_scale_min->value()-min_ht+i] = m_avg_notes[i];
			m_avg_notes = avg_notes;
			setting_scale_min->setValue(ht);
/*			for(size_t i=0; i<m_avg_notes.size(); i++)
                cout << m_avg_notes[i].ht << " ";
            cout << "[" << setting_scale_min->value() << ":" << setting_scale_max->value() << "]" << endl;*/
		}
		else if(ht>setting_scale_max->value())
		{
// 			cout << "max: ";
			int max_ht = ht;
			vector<AverageNote> avg_notes(max_ht-setting_scale_min->value()+1);
			for(size_t i=0; i<m_avg_notes.size(); i++)
				avg_notes[i] = m_avg_notes[i];
			for(size_t i=m_avg_notes.size(); i<avg_notes.size(); i++)
				avg_notes[i].ht = setting_scale_min->value()+i;
			m_avg_notes = avg_notes;
			setting_scale_max->setValue(ht);
/*			for(size_t i=0; i<m_avg_notes.size(); i++)
                cout << m_avg_notes[i].ht << " ";
            cout << "[" << setting_scale_min->value() << ":" << setting_scale_max->value() << "]" << endl;*/
		}

// 		cout << setting_scale_min->value()-ht << endl;

		m_avg_notes[ht-setting_scale_min->value()].addErr(err);
	}
}
void GLStatistics::addNote(int ht, int num, int den, float err)
{
    Q_UNUSED(num)
    Q_UNUSED(den)
    Q_UNUSED(err)

    m_current_ht = ht;
}
void GLStatistics::addNote(int ht, float cents, float err)
{
    Q_UNUSED(cents)
    Q_UNUSED(err)

    m_current_ht = ht;
}

void GLStatistics::resizeScale()
{
	if(setting_scale_auto->isChecked() || setting_scale_max->value()-setting_scale_min->value()+1<1)
		return;

	if(m_avg_notes.size()==0 || m_avg_notes[0].ht!=setting_scale_min->value() || m_avg_notes[m_avg_notes.size()-1].ht!=setting_scale_max->value())
	{
		vector<AverageNote> avg_notes(setting_scale_max->value()-setting_scale_min->value()+1);
		for(int i=0; i<int(m_avg_notes.size()); i++)
		{
			int in = i+m_avg_notes[0].ht-setting_scale_min->value();
			if(in>=0 && in<int(avg_notes.size()))
				avg_notes[in] = m_avg_notes[i];
		}
		for(size_t i=0; i<avg_notes.size(); i++)
			avg_notes[i].ht = setting_scale_min->value()+i;
		m_avg_notes = avg_notes;
	}
}

GLStatistics::GLStatistics(QWidget* parent)
: QOpenGLWidget(parent)
, View(tr("Statistics"), this)
, m_font("Helvetica")
{
	// settings
    setting_show->setIcon(QIcon(":/fmit/ui/images/module_statistics.svg"));
	setting_show->setChecked(false);
	hide();

	setting_reset = new QAction(tr("Reset statistics"), this);
	setting_reset->setShortcut('r');
    connect(setting_reset, SIGNAL(triggered()), this, SLOT(reset()));
	m_popup_menu.addAction(setting_reset);

	setting_keep_hidden = new QAction(tr("Keep notes when hidden"), this);
	setting_keep_hidden->setCheckable(true);
	setting_keep_hidden->setChecked(false);
	connect(setting_keep_hidden, SIGNAL(toggled(bool)), this, SLOT(update()));
	m_popup_menu.addAction(setting_keep_hidden);

	QHBoxLayout* keepActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* keepActionTitle = new QLabel(tr("Keep n values"), &m_popup_menu);
	keepActionLayout->addWidget(keepActionTitle);

	setting_keep_n_values = new QSpinBox(&m_popup_menu);
	setting_keep_n_values->setObjectName("setting_keep_n_values");
	setting_keep_n_values->setMinimum(1);
	setting_keep_n_values->setMaximum(500);
	setting_keep_n_values->setSingleStep(3);
	setting_keep_n_values->setValue(100);
	setting_keep_n_values->setToolTip(tr("Keep this number of values in the computation of the statistics"));
 	connect(setting_keep_n_values, SIGNAL(valueChanged(int)), this, SLOT(update()));
	keepActionLayout->addWidget(setting_keep_n_values);

	QWidget* keepActionWidget = new QWidget(&m_popup_menu);
	keepActionWidget->setLayout(keepActionLayout);

	QWidgetAction* keepAction = new QWidgetAction(&m_popup_menu);
	keepAction->setDefaultWidget(keepActionWidget);
	m_popup_menu.addAction(keepAction);

	setting_keep_n_values->setEnabled(false);
	s_settings->add(setting_keep_n_values);

	setting_show_std = new QAction(tr("Show standard deviation"), this);
	setting_show_std->setCheckable(true);
	setting_show_std->setChecked(true);
	connect(setting_show_std, SIGNAL(toggled(bool)), this, SLOT(update()));
	m_popup_menu.addAction(setting_show_std);

	setting_scale_auto = new QAction(tr("Scale auto"), this);
	setting_scale_auto->setCheckable(true);
	setting_scale_auto->setChecked(true);
	setting_scale_auto->setEnabled(false);
	connect(setting_scale_auto, SIGNAL(toggled(bool)), this, SLOT(update()));
	m_popup_menu.addAction(setting_scale_auto);

	QHBoxLayout* scaleMinActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* scaleMinActionTitle = new QLabel(tr("Scale min"), &m_popup_menu);
	scaleMinActionLayout->addWidget(scaleMinActionTitle);

	setting_scale_min = new QSpinBox(&m_popup_menu);
	setting_scale_min->setObjectName("setting_scale_min");
	setting_scale_min->setMinimum(-96);
	setting_scale_min->setMaximum(96);
	setting_scale_min->setSingleStep(6);
	setting_scale_min->setValue(-12);
	setting_scale_min->setToolTip(tr("Scale min value (in semi-tones)"));
	connect(setting_scale_min, SIGNAL(valueChanged(int)), this, SLOT(update()));
	scaleMinActionLayout->addWidget(setting_scale_min);

	QWidget* scaleMinActionWidget = new QWidget(&m_popup_menu);
	scaleMinActionWidget->setLayout(scaleMinActionLayout);

	QWidgetAction* scaleMinAction = new QWidgetAction(&m_popup_menu);
	scaleMinAction->setDefaultWidget(scaleMinActionWidget);
	m_popup_menu.addAction(scaleMinAction);

	setting_scale_min->setEnabled(false);
	s_settings->add(setting_scale_min);
	connect(setting_scale_auto, SIGNAL(toggled(bool)), setting_scale_min, SLOT(setDisabled(bool)));
	connect(setting_scale_min, SIGNAL(valueChanged(int)), this, SLOT(resizeScale()));

	QHBoxLayout* scaleMaxActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* scaleMaxActionTitle = new QLabel(tr("Scale max"), &m_popup_menu);
	scaleMaxActionLayout->addWidget(scaleMaxActionTitle);

	setting_scale_max = new QSpinBox(&m_popup_menu);
	setting_scale_max->setObjectName("setting_scale_max");
	setting_scale_max->setMinimum(-96);
	setting_scale_max->setMaximum(96);
	setting_scale_max->setSingleStep(6);
	setting_scale_max->setValue(+12);
	setting_scale_max->setToolTip(tr("Scale max value (in semi-tones)"));
	connect(setting_scale_max, SIGNAL(valueChanged(int)), this, SLOT(update()));
	scaleMaxActionLayout->addWidget(setting_scale_max);

	QWidget* scaleMaxActionWidget = new QWidget(&m_popup_menu);
	scaleMaxActionWidget->setLayout(scaleMaxActionLayout);

	QWidgetAction* scaleMaxAction = new QWidgetAction(&m_popup_menu);
	scaleMaxAction->setDefaultWidget(scaleMaxActionWidget);
	m_popup_menu.addAction(scaleMaxAction);

	setting_scale_max->setEnabled(false);
	s_settings->add(setting_scale_max);
	connect(setting_scale_auto, SIGNAL(toggled(bool)), setting_scale_max, SLOT(setDisabled(bool)));
	connect(setting_scale_max, SIGNAL(valueChanged(int)), this, SLOT(resizeScale()));

	setting_showTolerance = new QAction(tr("Show tolerance"), this);
	setting_showTolerance->setCheckable(true);
	setting_showTolerance->setChecked(true);
	connect(setting_showTolerance, SIGNAL(toggled(bool)), this, SLOT(update()));
	m_popup_menu.addAction(setting_showTolerance);

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

void GLStatistics::save()
{
	s_settings->setValue("setting_keep_hidden", setting_keep_hidden->isChecked());
	s_settings->setValue("setting_show_std", setting_show_std->isChecked());
	s_settings->setValue("setting_scale_auto", setting_scale_auto->isChecked());
}
void GLStatistics::load()
{
	setting_keep_hidden->setChecked(s_settings->value("setting_keep_hidden", setting_keep_hidden->isChecked()).toBool());
	setting_show_std->setChecked(s_settings->value("setting_show_std", setting_show_std->isChecked()).toBool());
	setting_scale_auto->setChecked(s_settings->value("setting_scale_auto", setting_scale_auto->isChecked()).toBool());
}
void GLStatistics::clearSettings()
{
	s_settings->remove("setting_keep_hidden");
	s_settings->remove("setting_show_std");
	s_settings->remove("setting_scale_auto");
}

void GLStatistics::reset()
{
	m_avg_notes.clear();
	if(setting_scale_auto->isChecked())
	{
	 	setting_scale_min->setValue(0);
		setting_scale_max->setValue(0);
	}
	resizeScale();
	update();
}

void GLStatistics::initializeGL()
{
	// Set the clear color to black
	glClearColor(1.0, 1.0, 1.0, 0.0);

	// glShadeModel( GL_FLAT );
	glShadeModel( GL_SMOOTH );

	glLoadIdentity();
}

void GLStatistics::drawTicksCent(int r, int dx, int dy)
{
	// only work within range that is a pure multiple of r
	float range = int(setting_spinScale->value()/r)*r;

	float scale = 50.0f/setting_spinScale->value();

	if((height()-dy)*r/100.0f*scale>2)
	{
		for(float i=-range; i<=range; i+=r)
		{
			int y = int((height()-dy)*i/100.0f*scale) + (height()-dy)/2 + dy;
			glVertex2i(dx,  y);
			glVertex2i(width(),  y);
		}
	}
}
void GLStatistics::drawTextTickCent(int r, int dy)
{
	// only work within range that is a pure multiple of r
	int range = int(setting_spinScale->value()/r)*r;

    QPainter painter(this);
    m_font.setPixelSize(14);
    painter.setFont(m_font);
    QFontMetrics fm(m_font);

	float scale = 50.0f/setting_spinScale->value();
	QString txt;
	for(int i=-range; i<range; i+=r)
	{
		txt = QString::number(i);
		if(i>=0) txt = QString("  ")+txt;
		if(i==0) txt = QString("  ")+txt;
        painter.drawText(2, (height()-dy)/2 - int((height()-dy)*i/100.0f*scale) +fm.xHeight()/2, txt);
	}
	painter.end();
}

void GLStatistics::paintGL()
{
// 	cout << "GLStatistics::paintGL " << m_notes.size() << endl;

	glClear(GL_COLOR_BUFFER_BIT);

	glLineWidth(1.0f);
	float human_tol = Music::f2hf(441.0,440.0);
	float scale;
	if(setting_useCents->isChecked())
		scale = 50.0f/setting_spinScale->value();
	else
		scale = int(50/setting_spinScale->value());

	// name
	glColor3f(0.75,0.75,0.75);
	QPainter painter(this);
    m_font.setPixelSize(20);
    painter.setFont(m_font);
    painter.drawText(2, 20, tr("Statistics"));
    painter.end();

	int char_size = 9;
	int ticks_size = 2+3*char_size;
	int grid_width = width()-ticks_size;
//	int dy = char_size/2;
	int legend_height = 40;
	int grid_height = height()-legend_height;

	// draw green rect
	if(setting_showTolerance->isChecked())
	{
		int green_ytop = int((scale*human_tol+0.5)*grid_height) + legend_height;
		int green_ybottom = int((-scale*human_tol+0.5)*grid_height) + legend_height;

		glBegin(GL_QUADS);
		glColor3f(168/256.0,221/256.0,182/256.0);
		glVertex2i(ticks_size, green_ytop);
		glVertex2i(ticks_size, green_ybottom);
		glVertex2i(width(), green_ybottom);
		glVertex2i(width(), green_ytop);
		glEnd();

		// draw green lines
		glBegin(GL_LINES);
		glColor3f(0.48,0.80,0.57);
		glVertex2i(ticks_size, green_ytop);
		glVertex2i(width(), green_ytop);
		glVertex2i(ticks_size, green_ybottom);
		glVertex2i(width(), green_ybottom);
		glEnd();
	}

	// std
	if(setting_show_std->isChecked())
	{
		glBegin(GL_QUADS);
		glLineWidth(1.0f);
		glColor3f(0.85, 0.85, 0.85);
		for(size_t i=0; i<m_avg_notes.size(); i++)
		{
			if(!m_avg_notes[i].errs.empty())
			{
				int top = int((scale*(m_avg_notes[i].err_mean+m_avg_notes[i].err_std)+0.5)*grid_height) + legend_height;
				top = max(top, legend_height);
				int bottom = int((scale*(m_avg_notes[i].err_mean-m_avg_notes[i].err_std)+0.5)*grid_height) + legend_height;
				bottom = max(bottom, legend_height);
				int left = ticks_size+int(i*float(grid_width)/m_avg_notes.size())+1;
				int right = ticks_size+int((i+1)*float(grid_width)/m_avg_notes.size());

				glVertex2i(left, top);
				glVertex2i(left, bottom);
				glVertex2i(right, bottom);
				glVertex2i(right, top);
			}
		}
		glEnd();
	}

	// horiz lines
	if(setting_useCents->isChecked())
	{
		glBegin(GL_LINES);
		float gray = 0.87;
// 		glColor3f(gray, gray, gray);
// 		drawTicksCent(1, ticks_size);
		gray = 0.875;
		glColor3f(gray, gray, gray);
		drawTicksCent(2, ticks_size, legend_height);
		gray = 0.75;
		glColor3f(gray, gray, gray);
		drawTicksCent(10, ticks_size, legend_height);
		glEnd();
	}
	else
	{
		glBegin(GL_LINES);
			float gray = 0.5;
			glColor3f(gray, gray, gray);
			glVertex2i(ticks_size,  grid_height/2+legend_height);
			glVertex2i(width(),  grid_height/2+legend_height);
			gray = 0.75;
			glColor3f(gray, gray, gray);
			glVertex2i(ticks_size,  grid_height/4+legend_height);
			glVertex2i(width(),  grid_height/4+legend_height);
			glVertex2i(ticks_size,  3*grid_height/4+legend_height);
			glVertex2i(width(),  3*grid_height/4+legend_height);
			gray = 0.87;
			glColor3f(gray, gray, gray);
			glVertex2i(ticks_size,  grid_height/8+legend_height);
			glVertex2i(width(),  grid_height/8+legend_height);
			glVertex2i(ticks_size,  7*grid_height/8+legend_height);
			glVertex2i(width(),  7*grid_height/8+legend_height);
			glVertex2i(ticks_size,  3*grid_height/8+legend_height);
			glVertex2i(width(),  3*grid_height/8+legend_height);
			glVertex2i(ticks_size,  5*grid_height/8+legend_height);
			glVertex2i(width(),  5*grid_height/8+legend_height);
		glEnd();
	}

	// center line
// 	glBegin(GL_LINES);
// 	glColor3f(0,0,0);
// 	glVertex2i(ticks_size, grid_height/2+legend_height);
// 	glVertex2i(width(), grid_height/2+legend_height);
// 	glEnd();

	// text marks
	float gray = 0.5;
	glColor3f(gray, gray, gray);
	if(setting_useCents->isChecked())
	{
		int grad = 10;
		if(setting_spinScale->value() <= 25) grad=5;
		if(setting_spinScale->value() <= 10) grad=1;
		drawTextTickCent(grad, legend_height);
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
        painter.drawText(2, 3*grid_height/4+fm.xHeight()/2, QString(sfraq.c_str()));
        sfraq = string("+")+sufraq;
        painter.drawText(2, grid_height/4+fm.xHeight()/2, QString(sfraq.c_str()));
		sufraq = string("1/")+QString::number(int(50/setting_spinScale->value())*4).toStdString();
        sfraq = string("-")+sufraq;
        painter.drawText(2, 5*grid_height/8+fm.xHeight()/2, QString(sfraq.c_str()));
        sfraq = string("+")+sufraq;
        painter.drawText(2, 3*grid_height/8+fm.xHeight()/2, QString(sfraq.c_str()));
        painter.end();
	}

	// vertical lines
	glBegin(GL_LINES);
	gray = 0.5;
	glColor3f(gray, gray, gray);
	for(size_t i=0; i<m_avg_notes.size(); i++)
	{
		int x = ticks_size+int(i*float(grid_width)/m_avg_notes.size());
		glVertex2i(x, legend_height);
		glVertex2i(x, height());
	}
	glEnd();

	// note names
	glColor3f(0, 0, 1);
	painter.begin(this);
    m_font.setPixelSize(14);
    painter.setFont(m_font);
    QFontMetrics fm(m_font);
    for(size_t i=0; i<m_avg_notes.size(); i++)
	{
        QString str = m_avg_notes[i].getName();

        QRect rect = fm.boundingRect(str);
        painter.drawText(ticks_size+(i+0.5)*float(grid_width)/m_avg_notes.size()-rect.width()/2, height()-4-17, str);
	}

	// sizes
	glColor3f(0, 0, 1);
	for(size_t i=0; i<m_avg_notes.size(); i++)
	{
		if(!m_avg_notes[i].errs.empty())
		{
            QString str = QString::number(m_avg_notes[i].errs.size());

            QRect rect = fm.boundingRect(str);
            painter.drawText(ticks_size+(i+0.5)*float(grid_width)/m_avg_notes.size()-rect.width()/2, height()-4, str);
		}
	}
	painter.end();

	// means
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	for(size_t i=0; i<m_avg_notes.size(); i++)
	{
		if(!m_avg_notes[i].errs.empty())
		{
			int y = int((scale*(m_avg_notes[i].err_mean)+0.5)*grid_height) + legend_height;
			int left = ticks_size+int(i*float(grid_width)/m_avg_notes.size())+1;
			int right = ticks_size+int((i+1)*float(grid_width)/m_avg_notes.size());

			glVertex2i(left, y);
			glVertex2i(right, y);
		}
	}
	glEnd();

	// Marks around current note
	glBegin(GL_LINES);
	gray = 0;
	glColor3f(gray, gray, gray);
	for(size_t i=0; i<m_avg_notes.size(); i++)
	{
		if(m_avg_notes[i].ht==m_current_ht)
		{
			int x = ticks_size+int(i*float(grid_width)/m_avg_notes.size());
			glVertex2i(x, legend_height);
			glVertex2i(x, height());
			x = ticks_size+int((i+1)*float(grid_width)/m_avg_notes.size());
			glVertex2i(x, legend_height);
			glVertex2i(x, height());
		}
	}
	glEnd();

	glFlush();
}

void GLStatistics::resizeGL( int w, int h )
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

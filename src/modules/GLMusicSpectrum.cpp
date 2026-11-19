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

#include "GLMusicSpectrum.h"

#include <iostream>
#include <functional>
#include <chrono>
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

GLMusicSpectrum::GLMusicSpectrum(QWidget* parent)
: QOpenGLWidget(parent)
, View(tr("MUSIC Spectrum"), this)
, m_font("Helvetica")
, m_iirLowPass(4)
{
	m_start_move_mouse = true;

	m_overlap_len = 50;
	m_undersample_fac = 1;
	m_currentNoteCenterFreq = 0;
	m_eigen_calc.SetSize(m_overlap_len);

	// settings
    setting_show->setIcon(QIcon(":/fmit/ui/images/module_music_spectrum.svg"));
    setting_show->setChecked(false);
	hide();

	setting_db_scale = new QAction(tr("dB scale"), this);
	setting_db_scale->setCheckable(true);
	setting_db_scale->setChecked(true);
	connect(setting_db_scale, SIGNAL(toggled(bool)), this, SLOT(dBScaleChanged(bool)));
	m_popup_menu.addAction(setting_db_scale);
	resetaxis();

	setting_track_note = new QAction(tr("track note"), this);
	setting_track_note->setCheckable(true);
	setting_track_note->setChecked(true);
	connect(setting_track_note, SIGNAL(toggled(bool)), this, SLOT(trackNoteChanged(bool)));
	m_popup_menu.addAction(setting_track_note);

	QHBoxLayout* sizeActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* sizeActionTitle = new QLabel(tr("Size"), &m_popup_menu);
	sizeActionLayout->addWidget(sizeActionTitle);

	setting_music_bins = new QSpinBox(&m_popup_menu);
	setting_music_bins->setObjectName("setting_music_bins");
	setting_music_bins->setMinimum(5);
	setting_music_bins->setMaximum(10000);
	setting_music_bins->setSingleStep(1);
	setting_music_bins->setValue(500);
	setting_music_bins->setSuffix("");
	setting_music_bins->setToolTip(tr("number of bins"));
	connect(setting_music_bins, SIGNAL(valueChanged(int)), this, SLOT(spinWinLengthChanged(int)));
	sizeActionLayout->addWidget(setting_music_bins);

	QWidget* sizeActionWidget = new QWidget(&m_popup_menu);
	sizeActionWidget->setLayout(sizeActionLayout);

	QWidgetAction* sizeAction = new QWidgetAction(&m_popup_menu);
	sizeAction->setDefaultWidget(sizeActionWidget);
	m_popup_menu.addAction(sizeAction);

	QHBoxLayout* sourceCountActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* sourceCountActionTile = new QLabel(tr("Source count"), &m_popup_menu);
	sourceCountActionLayout->addWidget(sourceCountActionTile);

	setting_assumed_source_count = new QSpinBox(&m_popup_menu);
	setting_assumed_source_count->setObjectName("setting_assumed_source_count");
	setting_assumed_source_count->setMinimum(1);
	setting_assumed_source_count->setMaximum(m_overlap_len / 2);
	setting_assumed_source_count->setSingleStep(1);
	setting_assumed_source_count->setValue(4);
	setting_assumed_source_count->setSuffix("");
	setting_assumed_source_count->setToolTip(tr("Assumed number of sources"));
	connect(setting_assumed_source_count, SIGNAL(valueChanged(int)), this, SLOT(spinAssumedSourceCountChanged(int)));
	sourceCountActionLayout->addWidget(setting_assumed_source_count);

	QWidget* sourceCountActionWidget = new QWidget(&m_popup_menu);
	sourceCountActionWidget->setLayout(sourceCountActionLayout);

	QWidgetAction* sourceCountAction = new QWidgetAction(&m_popup_menu);
	sourceCountAction->setDefaultWidget(sourceCountActionWidget);
	m_popup_menu.addAction(sourceCountAction);

	QHBoxLayout* partLengthActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* partLengthActionTile = new QLabel(tr("Part Length"), &m_popup_menu);
	partLengthActionLayout->addWidget(partLengthActionTile);

	setting_part_length = new QSpinBox(&m_popup_menu);
	setting_part_length->setObjectName("setting_music_part_length");
	setting_part_length->setMinimum(m_overlap_len);
	setting_part_length->setMaximum(10000);
	setting_part_length->setSingleStep(1);
	setting_part_length->setValue(200);
	setting_part_length->setSuffix("");
	setting_part_length->setToolTip(tr("Number of new samples (after undersampling) used for each update. Smaller = more frequent updates"));
	connect(setting_part_length, SIGNAL(valueChanged(int)), this, SLOT(spinPartLengthChanged(int)));
	partLengthActionLayout->addWidget(setting_part_length);

	QWidget* partLengthActionWidget = new QWidget(&m_popup_menu);
	partLengthActionWidget->setLayout(partLengthActionLayout);

	QWidgetAction* partLengthAction = new QWidgetAction(&m_popup_menu);
	partLengthAction->setDefaultWidget(partLengthActionWidget);
	m_popup_menu.addAction(partLengthAction);

	QHBoxLayout* overlapLengthActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* overlapLengthActionTile = new QLabel(tr("Overlap Length"), &m_popup_menu);
	overlapLengthActionLayout->addWidget(overlapLengthActionTile);

	setting_overlap_length = new QSpinBox(&m_popup_menu);
	setting_overlap_length->setObjectName("setting_music_overlap_length");
	setting_overlap_length->setMinimum(4);
	setting_overlap_length->setMaximum(200);
	setting_overlap_length->setSingleStep(1);
	setting_overlap_length->setValue(m_overlap_len);
	setting_overlap_length->setSuffix("");
	setting_overlap_length->setToolTip(tr("Block (vector) length. Higher = much more CPU load but more accurate"));
	connect(setting_overlap_length, SIGNAL(valueChanged(int)), this, SLOT(spinOverlapLengthChanged(int)));
	overlapLengthActionLayout->addWidget(setting_overlap_length);

	QWidget* overlapLengthActionWidget = new QWidget(&m_popup_menu);
	overlapLengthActionWidget->setLayout(overlapLengthActionLayout);

	QWidgetAction* overlapLengthAction = new QWidgetAction(&m_popup_menu);
	overlapLengthAction->setDefaultWidget(overlapLengthActionWidget);
	m_popup_menu.addAction(overlapLengthAction);

	QWidgetAction* helpCaption01 = new QWidgetAction(&m_popup_menu);
	helpCaption01->setDefaultWidget(new Title(tr("- Press left mouse button to move the view"), &m_popup_menu));
	m_popup_menu.addAction(helpCaption01);
	QWidgetAction* helpCaption02 = new QWidgetAction(&m_popup_menu);
	helpCaption02->setDefaultWidget(new Title(tr("- Press SHIFT key and left mouse button to zoom in and out"), &m_popup_menu));
	m_popup_menu.addAction(helpCaption02);
	QWidgetAction* helpCaption03 = new QWidgetAction(&m_popup_menu);
	helpCaption03->setDefaultWidget(new Title(tr("- Double-click to reset the view"), &m_popup_menu));
	m_popup_menu.addAction(helpCaption03);

	s_settings->add(setting_music_bins);
	s_settings->add(setting_assumed_source_count);

	spinWinLengthChanged(setting_music_bins->value());

	recalcFTMatrix();
}

void GLMusicSpectrum::refreshGraph()
{
	while (!buff.empty())
	{
		m_buff_filtered.push_front(m_iirLowPass.step(buff.back()));
		buff.pop_back();
	}
	// while(int(m_buff_filtered.size()) > setting_part_length->value() * m_undersample_fac)
	// 	m_buff_filtered.pop_back();
}

void GLMusicSpectrum::setSamplingRate(int sr)
{
	Q_UNUSED(sr)
}

void GLMusicSpectrum::newNote(double centerFreq)
{
	m_currentNoteCenterFreq = centerFreq;
	if (setting_track_note->isChecked() && setting_show->isChecked())
	{
		recalcFTMatrix();
	}
	m_iirLowPass.setFC(centerFreq * 2);
	m_iirLowPass.setDt(1 / double(Music::GetSamplingRate()));
}

void GLMusicSpectrum::resetaxis()
{
	m_minf = 430;
	m_maxf = 450;

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
void GLMusicSpectrum::dBScaleChanged(bool db)
{
    Q_UNUSED(db)

	resetaxis();
	update();
	recalcFTMatrix();
}
void GLMusicSpectrum::trackNoteChanged(bool trackNote)
{
    Q_UNUSED(trackNote)

	update();
	recalcFTMatrix();
}
void GLMusicSpectrum::spinWinLengthChanged(int num)
{
	if(Music::GetSamplingRate()>0)
	{
		m_components.resize(num);
		recalcFTMatrix();
	}
}
void GLMusicSpectrum::spinAssumedSourceCountChanged(int count)
{
	Q_UNUSED(count);

	update();
}
void GLMusicSpectrum::spinOverlapLengthChanged(int length)
{
	m_overlap_len = length;
	m_eigen_calc.SetSize(m_overlap_len);
	setting_part_length->setMinimum(length);
	setting_assumed_source_count->setMaximum(m_overlap_len / 4);

	update();
}
void GLMusicSpectrum::spinPartLengthChanged(int length)
{
	setting_overlap_length->setMaximum(length);

	update();
}

void GLMusicSpectrum::initializeGL()
{
	// Set the clear color to black
	glClearColor(1.0, 1.0, 1.0, 0.0);

	glShadeModel( GL_FLAT );

	glLoadIdentity();
}

void GLMusicSpectrum::mousePressEvent(QMouseEvent* e)
{
	m_start_move_mouse = true;
	m_press_x = e->x();
	m_press_y = e->y();
	m_press_minf = m_minf;
	m_press_maxf = m_maxf;

	double f = (m_maxf-m_minf)*double(m_press_x)/width()+m_minf;
	m_text = tr("Frequency %1 [Hz]").arg(f);
	update();
}
void GLMusicSpectrum::mouseDoubleClickEvent(QMouseEvent* e)
{
    Q_UNUSED(e)

	m_start_move_mouse = true;
	m_minf=430;
	m_maxf=450;
	resetaxis();
	update();
	recalcFTMatrix();
}
void GLMusicSpectrum::mouseMoveEvent(QMouseEvent* e)
{
	std::cout << "mouseMOve " << m_minf << ' ' << m_maxf << std::endl;
	static int old_x;
	static int old_y;
	if(m_start_move_mouse)
	{
		old_x = e->x();
		old_y = e->y();
		m_start_move_mouse = false;
	}
	int dx = e->x() - old_x;
	int dy = e->y() - old_y;

	if(Qt::LeftButton & e->buttons())
	{
		if(Qt::ShiftModifier & e->modifiers())
		{
			double f = (m_maxf-m_minf)*double(m_press_x)/width()+m_minf;
			double zx = double(m_press_x-e->x())/width();
			zx = pow(8, zx);
			m_minf = f - zx*(f-m_press_minf);
			m_maxf = f + zx*(m_press_maxf-f);
		}
		else
		{
			if(!setting_track_note->isChecked())
			{
				m_minf -= (m_maxf-m_minf)*dx/width();
				m_maxf -= (m_maxf-m_minf)*dx/width();
			}

			if(setting_db_scale->isChecked())
			{
				m_minA += (m_maxA-m_minA)*dy/height();
				m_maxA += (m_maxA-m_minA)*dy/height();
			}
			else
				m_maxA -= m_maxA*double(dy)/height();
		}

		recalcFTMatrix();
		update();
	}

	old_x = e->x();
	old_y = e->y();
	std::cout << "mouseMove2 " << m_minf << ' ' << m_maxf << std::endl;

}

void GLMusicSpectrum::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);

	if(m_components.size()>0)
	{
		if (int(m_buff_filtered.size()) >= setting_part_length->value() * m_undersample_fac)
		{
			while(int(m_buff_filtered.size()) > setting_part_length->value() * m_undersample_fac)
				m_buff_filtered.pop_back();
			calcMusicSpectrum();
			//m_buff_filtered.clear();
		}

		double y;
		// axes and scales
		double minTone = f2hf(m_minf);
		double maxTone = f2hf(m_maxf);
		if (m_maxf <= 0)
		{
			maxTone = 0;
			minTone = 0;
		}
		else if (m_minf <= 0)
		{
			minTone = maxTone - 50;
		}
		double tick_step = std::pow(2, std::round(std::log2((maxTone - minTone) / width() * 100)));
		if (tick_step == 0.125)
			tick_step = 0.1;
		else if (tick_step == 0.0625)
			tick_step = 0.05;
		else if (tick_step == 0.03125)
			tick_step = 0.05;
		else if (tick_step == 0.015625)
			tick_step = 0.02;
		else if (tick_step <= 0.0078125)
			tick_step = 0.01;
		minTone = std::floor(minTone / tick_step) * tick_step;
		maxTone = std::ceil(maxTone / tick_step) * tick_step;
		const int ticks_y = 24;
		for(double halfTone = minTone; halfTone < maxTone; halfTone += tick_step)
		{
			int x = (int)((h2f(halfTone) - m_minf) / (m_maxf - m_minf) * width());
			glBegin(GL_LINE_STRIP);
			glColor3f(0.8, 0.8, 0.8);
			glVertex2i(x, ticks_y);
			glVertex2i(x, height());
			glEnd();
		}

		glColor3f(0,0,0);
		QPainter painter(this);
		for(double halfTone = minTone; halfTone < maxTone; halfTone += tick_step)
		{
			int x = (int)((h2f(halfTone) - m_minf) / (m_maxf - m_minf) * width());
			std::string text;
			if (std::abs(halfTone - std::round(halfTone)) < 1e-6)
			{
				m_font.setPixelSize(18);
				m_font.setBold(true);
				painter.setFont(m_font);
				text = h2n(std::round(halfTone));
			}
			else
			{
				m_font.setPixelSize(14);
				m_font.setBold(false);
				painter.setFont(m_font);
				std::stringstream sstream;
				sstream.setf(std::ios::fixed | std::ios::showpoint | std::ios::showpos);
				sstream.precision(2);
				sstream << halfTone - std::floor(halfTone + 0.4999);  // do not round up for 0.5 -> display 4.5 as 4 + 0.5 instead of 5 - 0.5
				text = sstream.str();
			}
			painter.drawText(x - 50, height() - ticks_y, 100, ticks_y, Qt::AlignCenter, text.c_str());
		}
		painter.end();

		glBegin(GL_LINE_STRIP);
		glColor3f(0.4, 0.4, 0.5);
        for(int x=0; x<width(); x++)
		{
			int index = int(0.5+double(x)/width()*m_components.size());
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

		// name
		glColor3f(0.75,0.75,0.75);
		painter.begin(this);
        m_font.setPixelSize(20);
		m_font.setBold(false);
        painter.setFont(m_font);
        painter.drawText(2, 20, tr("MUSIC Spectrum"));
        painter.end();
	}

	// Text
	if(m_text.length()>0)
	{
		glColor3f(0,0,0);

        QPainter painter(this);
        m_font.setPixelSize(14);
		m_font.setBold(false);
        painter.setFont(m_font);
        painter.drawText(width()/2, 12, m_text);
        painter.end();
	}

	glFlush();
}

void GLMusicSpectrum::resizeGL( int w, int h )
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

// implementation according to https://en.wikipedia.org/wiki/MUSIC_(algorithm)
void GLMusicSpectrum::calcMusicSpectrum()
{
	m_Rx_estimate.resize(m_overlap_len);
	for (int i = 0; i < m_overlap_len; i++)
	{
		m_Rx_estimate[i].resize(m_overlap_len);
	}

	m_evals.resize(m_overlap_len);

	m_evecs.resize(m_overlap_len);
	for (int i = 0; i < m_overlap_len; i++)
	{
		m_evecs[i].resize(m_overlap_len);
	}

	m_Un_Un.resize(m_overlap_len);
	for (int i = 0; i < m_overlap_len; i++)
	{
		m_Un_Un[i].resize(m_overlap_len);
	}
	int actual_assumed_source_count = setting_assumed_source_count->value() * 2;
	if (actual_assumed_source_count > m_overlap_len) {
		std::cout << "Warning: Clamping the source count number because source count > overlap len " << m_overlap_len << std::endl;
		actual_assumed_source_count = m_overlap_len;
	}
	int N = setting_part_length->value() - m_overlap_len;
	// std::cout << "calcMusicSpectrum " << N << " " << m_buff_filtered.size() << " " << m_part_len << " " << m_overlap_len << std::endl;

	m_buff_undersampled.resize(setting_part_length->value());
	for (int i = 0; i < setting_part_length->value(); i++) {
		m_buff_undersampled[i] = m_buff_filtered[i * m_undersample_fac];
	}

	// calculate Rx_estimate
	double debug_sum = 0;
	for (int i_column = 0; i_column < m_overlap_len; i_column++)
	{
		for (int i_row = 0; i_row < m_overlap_len; i_row++)
		{
			if (i_column > i_row)
			{
				m_Rx_estimate[i_column][i_row] = m_Rx_estimate[i_row][i_column];
			}
			else
			{
				m_Rx_estimate[i_column][i_row] = 0;
				for (int i = 0; i < N; i++)
				{
					m_Rx_estimate[i_column][i_row] += m_buff_undersampled[i_column + i] * m_buff_undersampled[i_row + i];
				}
				m_Rx_estimate[i_column][i_row] /= N;
				debug_sum += m_Rx_estimate[i_column][i_row];
			}
		}
	}

	// this eigen decomponosition takes about 70% of total time of this function
	m_eigen_calc.Diagonalize(m_Rx_estimate, m_evals, m_evecs, m_eigen_calc.SORT_DECREASING_EVALS);

	double components_max = 1e-5;
	for(std::size_t i_frequency = 0; i_frequency < m_components.size(); i_frequency++)
	{
		m_components[i_frequency] = 0;
		for (int i_row = actual_assumed_source_count; i_row < m_overlap_len; i_row++)
		{
			std::complex<double> temp = 0;
			for (int i_column = 0; i_column < m_overlap_len; i_column++)
			{
				temp += m_evecs[i_row][i_column] * m_dft_matrix[i_frequency][i_column];
			}
			m_components[i_frequency] += temp.real() * temp.real() + temp.imag() * temp.imag();
		}
		m_components[i_frequency] = 1 / (m_components[i_frequency] + 1e-3);
		if (m_components[i_frequency] > components_max)
			components_max = m_components[i_frequency];
	}
	//std::cout << components_max << std::endl;
	for(std::size_t i_frequency = 0; i_frequency < m_components.size(); i_frequency++)
	{
		m_components[i_frequency] /= m_overlap_len;
	}
}

void GLMusicSpectrum::recalcFTMatrix()
{
	int sampling_rate = Music::GetSamplingRate();
	if (sampling_rate > 0)
		m_undersample_fac = std::ceil(Music::GetSamplingRate() / 8 / m_maxf);

	if (setting_track_note->isChecked())
	{
		double width = m_maxf - m_minf;
		m_minf = m_currentNoteCenterFreq - width / 2;
		m_maxf = m_currentNoteCenterFreq + width / 2;
	}

	m_dft_matrix.resize(m_components.size());
	for (std::size_t i = 0; i < m_components.size(); i++)
	{
		m_dft_matrix[i].resize(m_overlap_len);
		double frequency = m_minf + (m_maxf - m_minf) * i / static_cast<double>(m_components.size());
		double frequency_adjusted = 2 * Math::Pi * frequency / (Music::GetSamplingRate() / static_cast<double>(m_undersample_fac));
		for (int i_t = 0; i_t < m_overlap_len; i_t++) {
			m_dft_matrix[i][i_t] = std::exp(std::complex<double>(0, frequency_adjusted * i_t));
		}
	}
}
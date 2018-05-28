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



#ifndef _CustomInstrumentTunerForm_h_
#define _CustomInstrumentTunerForm_h_

#include "ui_InstrumentTunerForm.h"

// #include "config.h"

#include <deque>
using namespace std;
#include <qtimer.h>
#include <qdial.h>
#include <qsettings.h>
#include <Music/Music.h>
#include <Music/TimeAnalysis.h>
#include <Music/FreqAnalysis.h>
#include <Music/MultiCumulativeDiffAlgo.h>
#include <Music/CumulativeDiffAlgo.h>
#include <Music/CombedFT.h>
#include <Music/Filter.h>
using namespace Music;
#include "AutoQSettings.h"
#include "CaptureThread.h"
#include "ui_ConfigForm.h"
#include "LatencyMonoQuantizer.h"
#include "DummyMonoQuantizer.h"
#include "modules/DialView.h"
#include "modules/GLGraph.h"
#include "modules/GLErrorHistory.h"
#include "modules/GLVolumeHistory.h"
#include "modules/MicrotonalView.h"
#include "modules/GLSample.h"
#include "modules/GLFreqStruct.h"
#include "modules/GLFT.h"
#include "modules/GLStatistics.h"

class CustomConfigForm : public QDialog, public Ui_ConfigForm
{
	Q_OBJECT

public:
	CustomConfigForm(QWidget* parent = 0)
	{
        Q_UNUSED(parent)
		setupUi(this);
	}
};

class CustomInstrumentTunerForm : public QMainWindow, public Ui_InstrumentTunerForm
{
	Q_OBJECT

  public:
	CustomConfigForm m_config_form;

	CaptureThread m_capture_thread;
	CombedFT* m_algo_combedfft;

	DialView* m_dialTune;
	GLGraph* m_glGraph;
	GLErrorHistory* m_glErrorHistory;
	GLVolumeHistory* m_glVolumeHistory;
	GLSample* m_glSample;
	GLFreqStruct* m_glFreqStruct;
	GLFT* m_glFT;
	MicrotonalView* m_microtonalView;
	GLStatistics* m_glStatistics;

	deque<double> m_queue;
	double m_freq;
	double m_compared_freq;
	double m_error;

	QTime m_time;

	CustomInstrumentTunerForm();
	AutoQSettings m_settings;
	void loadSettings();

	virtual void resizeEvent(QResizeEvent* e);
	virtual void keyPressEvent(QKeyEvent * e);

	void toggleFullScreen();

	QTime m_time_refresh;
	QTime m_time_refresh_views;
	QTimer m_timer_refresh;
	double m_last_refresh;

	// Range filters
	RectangularHighPassRTFilter m_rect_range_filter;
	FIRRTFilter m_fir_range_filter;
	DummyRTFilter m_dummy_range_filter;
	RTFilter* m_range_filter;

	// Time quantizers
	LatencyMonoQuantizer m_latency_quantizer;
	DummyMonoQuantizer m_dummy_quantizer;
	MonoQuantizer* m_quantizer;

	virtual void refresh_views();
	void refresh_data_sample();
	void refresh_data_harmonics();

	// the main function
public slots:
	virtual void helpAbout();
	virtual void configure();
	virtual void pause(bool on);
	virtual void ui_spinAFreq_valueChanged(int);
	virtual void ui_spinAOffset_valueChanged(int);
	virtual void saveSettings();
	virtual void samplingRateChanged(int sampling_rate);
	virtual void errorRaised(const QString& error);
	virtual void transportChanged(const QString& name);
	virtual void noteStarted(double freq, double dt);
	virtual void noteFinished(double freq, double dt);
	virtual void update_views();
	virtual void tuningFreqChanged(float);
	virtual void configure_ok();
    virtual void configure_cancel();
	virtual void restoreFactorySettings();
	virtual void noteRangeChanged();
	virtual void transpositionChanged();
	virtual void selectTransport(const QString & name);
	virtual void autoDetectTransport();
	void refresh();
	void update_localized_note_names();

public:
	~CustomInstrumentTunerForm();
};

#endif // _CustomInstrumentTunerForm_h_

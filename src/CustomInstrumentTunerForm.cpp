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

#include "CustomInstrumentTunerForm.h"

#include <cassert>
#include <iostream>
using namespace std;
#include <qstring.h>
#include <qaction.h>
#include <qlabel.h>
#include <qgl.h>
#include <qlayout.h>
#include <qlcdnumber.h>
#include <qdial.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qsplitter.h>
#include <qprogressbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qstatusbar.h>
#include <qdatetime.h>
#include <qradiobutton.h>

#include <qtextbrowser.h>
#include <qpushbutton.h>
#include <qwidgetaction.h>
#include <qevent.h>
#include <qabstracttextdocumentlayout.h>
#include <Music/Convolution.h>
using namespace Music;
#include "modules/View.h"
#include "qthelper.h"

#include "aboutbox.h"

CustomInstrumentTunerForm::CustomInstrumentTunerForm()
: m_config_form(this)
, m_capture_thread("fmit")
, m_algo_combedfft(NULL)
, m_settings("fmit", "fmit", "009801")	// not necessarily equal to the soft version
, m_timer_refresh(this)
, m_range_filter(&m_dummy_range_filter)
, m_quantizer(&m_latency_quantizer)
{
	setupUi(this);

	/*QWidgetAction* viewsCaption = new QWidgetAction(ui_tbViews);
	viewsCaption->setDefaultWidget(new QLabel(tr("Views"), ui_tbViews));
	ui_tbViews->addAction(viewsCaption);*/

	View::s_settings = &m_settings;
	m_settings.add(m_config_form.ui_chkFullScreen);
	m_settings.add(m_config_form.ui_chkAutoSaveOnExit);
	m_settings.add(m_config_form.ui_chkShowA4Offset);

	m_settings.add(m_config_form.ui_cbTuning);
	m_settings.add(m_config_form.ui_cbTonality);
	m_settings.add(m_config_form.ui_cbNotesName);
	m_settings.add(ui_spinAFreq);
	m_settings.add(ui_spinA3Offset);

	m_settings.add(m_config_form.ui_chkAutoDetect);
#ifdef CAPTURE_JACK
	m_settings.add(m_config_form.ui_chkJACKAutoConnect);
	m_settings.add(m_config_form.ui_txtJACKSourcePort);
#endif
#ifdef CAPTURE_ALSA
	m_settings.add(m_config_form.ui_chkALSASamplingRateMax);
	m_settings.add(m_config_form.ui_spinALSASamplingRate);
	m_settings.add(m_config_form.ui_chkALSAMixMultipleChannels);
	m_settings.add(m_config_form.ui_txtALSAPCMName);
#endif
#ifdef CAPTURE_OSS
	m_settings.add(m_config_form.ui_chkOSSSamplingRateMax);
	m_settings.add(m_config_form.ui_spinOSSSamplingRate);
	m_settings.add(m_config_form.ui_chkOSSMixMultipleChannels);
	m_settings.add(m_config_form.ui_txtOSSPCMName);
#endif
#ifdef CAPTURE_PORTAUDIO
	m_settings.add(m_config_form.ui_chkPortAudioSamplingRateMax);
	m_settings.add(m_config_form.ui_spinPortAudioSamplingRate);
	m_settings.add(m_config_form.ui_chkPortAudioMixMultipleChannels);
#endif

	m_settings.add(m_config_form.ui_spinRefreshTime);
	m_settings.add(m_config_form.ui_spinMinHT);
	m_settings.add(m_config_form.ui_spinMaxHT);

	m_settings.add(m_config_form.ui_grpRangeFiltering);
	m_settings.add(m_config_form.ui_rdRangeFilteringRectangular);
	m_settings.add(m_config_form.ui_rdRangeFilteringFIR);

	m_settings.add(m_config_form.ui_spinVolumeTreshold);
	m_settings.add(m_config_form.ui_spinWindowSizeFactor);
	m_settings.add(m_config_form.ui_chkAlgoUseSubHarmTresh);
	m_settings.add(m_config_form.ui_spinCombedFFTAudibilityRatio);

	m_settings.add(m_config_form.up_grpFreqRefinement);
	m_settings.add(m_config_form.ui_rdUseFreqRefinement);
	m_settings.add(m_config_form.ui_spinFreqRefinMaxHarm);
	m_settings.add(m_config_form.ui_rdUseTimeRefinement);
	m_settings.add(m_config_form.ui_spinTimeRefinMaxPeriod);

	m_settings.add(m_config_form.ui_grpQuantizer);
	m_settings.add(m_config_form.ui_spinErrorLatency);

	m_algo_combedfft = new CombedFT();

    for(int i=0; i<int(m_capture_thread.getTransports().size()); i++)
		m_config_form.ui_cbTransports->addItem(m_capture_thread.getTransports()[i]->getName());

	if(m_capture_thread.getTransports().empty())
		QMessageBox::critical(this, "Error", "Please compile me with a capture system ...");
	if(m_capture_thread.getTransports().size()==1)
	{
		m_config_form.ui_lblSelectedCaptureSystem->hide();
		m_config_form.ui_btnAutoDetect->hide();
		m_config_form.ui_chkAutoDetect->hide();
		m_config_form.ui_cbTransports->hide();
	}
	m_config_form.ui_grpALSA->hide();
	m_config_form.ui_grpJACK->hide();
	m_config_form.ui_grpPortAudio->hide();
	m_config_form.ui_grpOSS->hide();
    m_config_form.ui_grpQt->hide();

    m_config_form.ui_grpRangeFiltering->hide();

	ui_lblA3Offset->hide();
	ui_spinA3Offset->hide();

	connect(&m_capture_thread, SIGNAL(samplingRateChanged(int)), this, SLOT(samplingRateChanged(int)));
	connect(&m_capture_thread, SIGNAL(errorRaised(const QString&)), this, SLOT(errorRaised(const QString&)));
	connect(&m_capture_thread, SIGNAL(transportChanged(const QString&)), this, SLOT(transportChanged(const QString&)));

	connect(&m_latency_quantizer, SIGNAL(noteStarted(double,double)), this, SLOT(noteStarted(double,double)));
	connect(&m_latency_quantizer, SIGNAL(noteFinished(double,double)), this, SLOT(noteFinished(double,double)));
	connect(&m_dummy_quantizer, SIGNAL(noteStarted(double,double)), this, SLOT(noteStarted(double,double)));
	connect(&m_dummy_quantizer, SIGNAL(noteFinished(double,double)), this, SLOT(noteFinished(double,double)));

	m_dialTune = new DialView(centralWidget());
	ui_dialTuneLayout->addWidget(m_dialTune);

	m_glGraph = new GLGraph(centralWidget());
	connect(m_glGraph->setting_show, SIGNAL(toggled(bool)), this, SLOT(update_views()));
	connect(m_glGraph->setting_spinMaxHeight, SIGNAL(valueChanged(int)), this, SLOT(update_views()));
	ui_tbViews->addAction(m_glGraph->setting_show);
	ui_graphLayout->addWidget(m_glGraph);

	m_glErrorHistory = new GLErrorHistory(centralWidget());
	connect(m_glErrorHistory->setting_show, SIGNAL(toggled(bool)), this, SLOT(update_views()));
	ui_tbViews->addAction(m_glErrorHistory->setting_show);
	ui_errorLayout->addWidget(m_glErrorHistory);

	// link scales
	connect(m_dialTune->setting_spinScale, SIGNAL(valueChanged(int)), m_glErrorHistory->setting_spinScale, SLOT(setValue(int)));
	connect(m_glErrorHistory->setting_spinScale, SIGNAL(valueChanged(int)), m_dialTune->setting_spinScale, SLOT(setValue(int)));
    connect(m_dialTune->setting_useCents, SIGNAL(toggled(bool)), m_glErrorHistory->setting_useCents, SLOT(setChecked(bool)));
    connect(m_glErrorHistory->setting_useCents, SIGNAL(toggled(bool)), m_dialTune->setting_useCents, SLOT(setChecked(bool)));

	m_glVolumeHistory = new GLVolumeHistory(centralWidget());
	connect(m_config_form.ui_spinVolumeTreshold, SIGNAL(valueChanged(int)), m_glVolumeHistory, SLOT(setVolumeTreshold(int)));
	connect(m_glVolumeHistory->setting_show, SIGNAL(toggled(bool)), this, SLOT(update_views()));
	ui_tbViews->addAction(m_glVolumeHistory->setting_show);
	ui_volumeLayout->addWidget(m_glVolumeHistory);

	// link keep settings
    connect(ui_btnKeepErrorHistory, SIGNAL(toggled(bool)), m_glErrorHistory->setting_keep, SLOT(setChecked(bool)));
    connect(m_glErrorHistory->setting_keep, SIGNAL(toggled(bool)), m_glVolumeHistory->setting_keep, SLOT(setChecked(bool)));
    connect(m_glErrorHistory->setting_keep, SIGNAL(toggled(bool)), ui_btnKeepErrorHistory, SLOT(setChecked(bool)));

	m_glSample = new GLSample(centralWidget());
	connect(m_glSample->setting_show, SIGNAL(toggled(bool)), this, SLOT(update_views()));
	ui_tbViews->addAction(m_glSample->setting_show);
	ui_sampleLayout->addWidget(m_glSample);

	m_glFreqStruct = new GLFreqStruct(centralWidget());
	connect(m_glFreqStruct->setting_show, SIGNAL(toggled(bool)), this, SLOT(update_views()));
	ui_tbViews->addAction(m_glFreqStruct->setting_show);
	ui_formantsLayout->addWidget(m_glFreqStruct);

	m_glFT = new GLFT(centralWidget());
	connect(m_glFT->setting_show, SIGNAL(toggled(bool)), this, SLOT(update_views()));
	ui_tbViews->addAction(m_glFT->setting_show);
	ui_FT->addWidget(m_glFT);

	m_microtonalView = new MicrotonalView(centralWidget());
	connect(m_microtonalView->setting_show, SIGNAL(toggled(bool)), this, SLOT(update_views()));
	connect(m_microtonalView, SIGNAL(tuningFreqChanged(float)), this, SLOT(tuningFreqChanged(float)));
	ui_tbViews->addAction(m_microtonalView->setting_show);
	ui_microtonalLayout->addWidget(m_microtonalView);

	m_glStatistics = new GLStatistics(centralWidget());
	connect(m_glStatistics->setting_show, SIGNAL(toggled(bool)), this, SLOT(update_views()));
	ui_tbViews->addAction(m_glStatistics->setting_show);
	ui_microtonalLayout->addWidget(m_glStatistics);

	connect(m_dialTune->setting_spinScale, SIGNAL(valueChanged(int)), m_glStatistics->setting_spinScale, SLOT(setValue(int)));
	connect(m_glStatistics->setting_spinScale, SIGNAL(valueChanged(int)), m_dialTune->setting_spinScale, SLOT(setValue(int)));
    connect(m_dialTune->setting_useCents, SIGNAL(toggled(bool)), m_glStatistics->setting_useCents, SLOT(setChecked(bool)));
    connect(m_glStatistics->setting_useCents, SIGNAL(toggled(bool)), m_dialTune->setting_useCents, SLOT(setChecked(bool)));

    connect(m_dialTune->setting_showTolerance, SIGNAL(toggled(bool)), m_glStatistics->setting_showTolerance, SLOT(setChecked(bool)));
    connect(m_glStatistics->setting_showTolerance, SIGNAL(toggled(bool)), m_dialTune->setting_showTolerance, SLOT(setChecked(bool)));

    connect(&m_config_form, SIGNAL(accepted()), this, SLOT(configure_ok()));
    connect(&m_config_form, SIGNAL(rejected()), this, SLOT(configure_cancel()));
	connect(m_config_form.ui_btnRestoreFactorySettings, SIGNAL(clicked()), this, SLOT(restoreFactorySettings()));
	connect(m_config_form.ui_spinMinHT, SIGNAL(valueChanged(int)), this, SLOT(noteRangeChanged()));
	connect(m_config_form.ui_spinMaxHT, SIGNAL(valueChanged(int)), this, SLOT(noteRangeChanged()));
	connect(m_config_form.ui_cbTuning, SIGNAL(highlighted(int)), this, SLOT(noteRangeChanged()));
	connect(m_config_form.ui_cbTonality, SIGNAL(highlighted(int)), this, SLOT(noteRangeChanged()));
	connect(m_config_form.ui_cbNotesName, SIGNAL(highlighted(int)), this, SLOT(noteRangeChanged()));
	connect(m_config_form.ui_btnAutoDetect, SIGNAL(clicked()), this, SLOT(autoDetectTransport()));
	connect(m_config_form.ui_cbTransports, SIGNAL(activated(const QString&)), this, SLOT(selectTransport(const QString&)));
	connect(m_config_form.ui_chkALSAMixMultipleChannels, SIGNAL(toggled(bool)), &m_capture_thread, SLOT(setMixMultipleChannels(bool)));
	connect(m_config_form.ui_chkOSSMixMultipleChannels, SIGNAL(toggled(bool)), &m_capture_thread, SLOT(setMixMultipleChannels(bool)));
	connect(m_config_form.ui_chkPortAudioMixMultipleChannels, SIGNAL(toggled(bool)), &m_capture_thread, SLOT(setMixMultipleChannels(bool)));

	loadSettings();

	if(m_config_form.ui_chkAutoDetect->isChecked())
		m_capture_thread.autoDetectTransport();

	configure_ok();

	if(m_config_form.ui_chkFullScreen->isChecked())
		toggleFullScreen();

    m_config_form.adjustSize();

	m_time_refresh_views.start();
	m_time_refresh.start();
	m_time.start();
	connect((QObject*)&m_timer_refresh, SIGNAL(timeout()), this, SLOT(refresh()));
	m_timer_refresh.start(m_config_form.ui_spinRefreshTime->value());

//     cout << __FILE__ << " " << __LINE__ << endl;
}

void CustomInstrumentTunerForm::transportChanged(const QString& name)
{
	selectTransport(name);

	if(m_capture_thread.getCurrentTransportIndex()!=m_config_form.ui_cbTransports->currentIndex())
		m_config_form.ui_cbTransports->setCurrentIndex(m_capture_thread.getCurrentTransportIndex());
}
void CustomInstrumentTunerForm::selectTransport(const QString& name)
{
	m_config_form.ui_grpALSA->hide();
	m_config_form.ui_grpJACK->hide();
	m_config_form.ui_grpPortAudio->hide();
	m_config_form.ui_grpOSS->hide();
    m_config_form.ui_grpQt->hide();

	if(name=="ALSA")			m_config_form.ui_grpALSA->show();
	else if(name=="JACK")		m_config_form.ui_grpJACK->show();
	else if(name=="PortAudio")	m_config_form.ui_grpPortAudio->show();
	else if(name=="OSS")		m_config_form.ui_grpOSS->show();
    else if(name=="Qt")         m_config_form.ui_grpQt->show();
}
void CustomInstrumentTunerForm::autoDetectTransport()
{
	m_capture_thread.autoDetectTransport();

	// here transportChanged will be called
}

void CustomInstrumentTunerForm::toggleFullScreen()
{
	static bool fs = true;
	if(fs)
	{
		m_config_form.ui_chkFullScreen->setChecked(true);
		showFullScreen();
	}
	else
	{
		m_config_form.ui_chkFullScreen->setChecked(false);
		showNormal();
	}
	fs = !fs;
}

void CustomInstrumentTunerForm::noteRangeChanged()
{
    //	cout << "CustomInstrumentTunerForm::noteRangeChanged" << endl;

	m_config_form.ui_txtMinHT->setText(QString::fromStdString(h2n(m_config_form.ui_spinMinHT->value())) + " = " + QString::number(h2f(m_config_form.ui_spinMinHT->value())) + " hz");
	m_config_form.ui_txtMaxHT->setText(QString::fromStdString(h2n(m_config_form.ui_spinMaxHT->value())) + " = " + QString::number(h2f(m_config_form.ui_spinMaxHT->value())) + " hz");
}

void CustomInstrumentTunerForm::errorRaised(const QString& error)
{
    //	cout << "CustomInstrumentTunerForm::errorRaised " << error << endl;

	statusBar()->showMessage(QString("ERROR: ")+error);

	QPalette palette;
	palette.setColor(ui_lblSoundStability->backgroundRole(), QColor(180,74,74));
	ui_lblSoundStability->setPalette(palette);
}

void CustomInstrumentTunerForm::samplingRateChanged(int sampling_rate)
{
//	cout << "CustomInstrumentTunerForm::samplingRateChanged " << sampling_rate << endl;

	Music::SetSamplingRate(sampling_rate);

    m_rect_range_filter.reset(int(GetSamplingRate()/2000.0));
//         m_fir_range_filter.setImpulseResponse(fir1_lowpass(400, 2/400));
// 	m_rect_range_filter.reset(int(GetSamplingRate()/h2f(GetSemitoneMin())));
    m_glFT->spinWinLengthChanged(m_glFT->setting_winlen->value());
    m_glFT->setSamplingRate(sampling_rate);

	if(m_config_form.ui_cbTransports->currentText()=="JACK")
		m_config_form.ui_lblJACKSamplingRate->setText(QString::number(sampling_rate));
}

void CustomInstrumentTunerForm::ui_spinAFreq_valueChanged(int AFreq)
{
	double A = AFreq;
	if(m_config_form.ui_chkShowA4Offset->isChecked())
		A = h2f(ui_spinA3Offset->value()*1/100.0f, A);
	Music::SetAFreq(A);
//	cout << A << endl;
}
void CustomInstrumentTunerForm::ui_spinAOffset_valueChanged(int offset)
{
	double A = ui_spinAFreq->value();
	if(m_config_form.ui_chkShowA4Offset->isChecked())
		A = h2f(offset*1/100.0f, ui_spinAFreq->value());
	Music::SetAFreq(A);
//	cout << A << endl;
}

void CustomInstrumentTunerForm::tuningFreqChanged(float freq)
{
    //	cout << "CustomInstrumentTunerForm::tuningFreqChanged " << freq << endl;

	if(freq==0.0f)
	{
		if(m_compared_freq!=0.0f)
		{
			ui_txtNoteFreq->display(m_compared_freq);
			ui_txtNote->setText(QString::fromStdString(h2n(f2h(m_compared_freq))));
		}
	}
	else
	{
		m_compared_freq = freq;
		ui_txtNoteFreq->display(int(freq*100)/100.0f);
		ui_txtNote->setText(m_microtonalView->getTuningNoteName());
	}

	m_quantizer->reset();

	//	m_dialTune->setError(-10.0f);
}

void CustomInstrumentTunerForm::pause(bool on)
{
	m_capture_thread.togglePause(on);

	if(on)	m_timer_refresh.stop();
	else	m_timer_refresh.start(m_config_form.ui_spinRefreshTime->value());
}

void CustomInstrumentTunerForm::refresh()
{
	double elapsed_time = m_time_refresh.elapsed();
	m_time_refresh.start();

	QColor capture_failed_color(180,74,74);
	QColor prb_color(208,146,0);
	QColor empty_color(128,128,128);
	QColor ok_color(83,165,105);

	// 1/{time between refresh} = {nb refresh by seconds}
	// limit the nb of new data by fs/{nb refresh by seconds}
	// add 1 to {nb refresh by second} to eventualy recover small lags
    // (a big lag is managed by m_values.size()>getPacketSizeSinceLastLock())
	int limit = int( m_capture_thread.getSamplingRate() /
			(1.0/(m_config_form.ui_spinRefreshTime->value()/1000.0) - 1));

// 	cout << "REFRESH ";

	m_capture_thread.lock();
//     cout << "CustomInstrumentTunerForm::refresh locked, values to read=" << m_capture_thread.m_values.size() << endl;
	int nb_new_data = 0;
	while(!m_capture_thread.m_values.empty() &&
              (int(m_capture_thread.m_values.size())>m_capture_thread.getPacketSizeSinceLastLock() || nb_new_data<limit))
    {
// 		cout << m_capture_thread.m_values.back() << " ";
		double value = (*m_range_filter)(m_capture_thread.m_values.back());
// 		cout << value << " ";
		m_capture_thread.m_values.pop_back();

		m_queue.push_front(value);
		if(m_glGraph)	m_glGraph->addValue(value);
		if(m_glFT)		m_glFT->buff.push_front(value);

		nb_new_data++;
	}
	m_capture_thread.unlock();
//     cout << "CustomInstrumentTunerForm::refresh unlocked" << endl;

// 	cout << endl;

	int max_size = max(m_range_filter->getLength(), max(m_glGraph->getLength(), m_algo_combedfft->getMinSize()));
	while(!m_queue.empty() && int(m_queue.size())>max_size)
		m_queue.pop_back();

	// refresh graph data
	m_glGraph->refreshGraph();	// TODO refresh the view each time ??
	m_glFT->refreshGraph();

	// ------- Analysis stage -------

	// if something goes wrong in the capture system
	if(nb_new_data==0 || m_algo_combedfft==NULL || elapsed_time>8*m_config_form.ui_spinRefreshTime->value())
	{
		QPalette palette;
		palette.setColor(ui_lblSoundStability->backgroundRole(), capture_failed_color);
		ui_lblSoundStability->setPalette(palette);
	}
	else
	{
		m_algo_combedfft->apply(m_queue);

        // TODO hem ... use energy
		double max_component = 20*log10(m_algo_combedfft->getComponentsMax());
		ui_pgbVolume->setValue(100+int(max_component));

		double freq = 0.0;
		if(m_algo_combedfft->hasNoteRecognized())
			freq = m_algo_combedfft->getFondamentalFreq();

		double freq_rel = freq*m_algo_combedfft->m_plan.in.size()/double(GetSamplingRate());
		if(freq_rel<1 || freq_rel>(m_algo_combedfft->m_plan.out.size()/2))
			freq = 0.0;

		// frequency refinement
		if(freq>0.0 && m_config_form.up_grpFreqRefinement->isChecked())
		{
			if(m_config_form.ui_rdUseFreqRefinement->isChecked())
			{
				freq = FundFreqRefinementOfHarmonicStruct(m_algo_combedfft->m_plan.out, freq, m_config_form.ui_spinFreqRefinMaxHarm->value(), m_algo_combedfft->getZeroPaddingFactor());

			}
			else if(m_config_form.ui_rdUseTimeRefinement->isChecked())
			{
				double period = GetAveragePeriodFromApprox(m_queue, int(GetSamplingRate()/freq), m_config_form.ui_spinTimeRefinMaxPeriod->value());
				if(period>0.0)
					freq = GetSamplingRate()/period;
			}
		}

// 		cout << "2) test freq=" << m_test_freq <<endl;

		m_quantizer->quantize(freq);

		if(!m_quantizer->isPlaying())
		{
			QPalette palette;
			palette.setColor(ui_lblSoundStability->backgroundRole(), empty_color);
			ui_lblSoundStability->setPalette(palette);
		}
		else
		{
            // Use 100ms rect window
            int max_amplitude_limit = min(m_queue.size(),size_t(m_capture_thread.getSamplingRate()*100.0/1000));
            double max_amplitude = 0.0;
            for(int i=0;i<max_amplitude_limit; i++)
                max_amplitude = max(max_amplitude, fabs(m_queue[i]));

			if(max_amplitude>=1.0)
			{
				QPalette palette;
				palette.setColor(ui_lblSoundStability->backgroundRole(), prb_color);
				ui_lblSoundStability->setPalette(palette);
			}
			else
			{
				QPalette palette;
				palette.setColor(ui_lblSoundStability->backgroundRole(), ok_color);
				ui_lblSoundStability->setPalette(palette);
			}

			m_freq = m_quantizer->getAverageFrequency();
			m_error = f2hf(m_freq, m_compared_freq);

			// refresh error
			m_glErrorHistory->addError(m_error);
			m_dialTune->setError(m_error);
			m_dialTune->m_avg_error = m_glErrorHistory->m_notes.back().avg_err;
			m_dialTune->m_min_error = m_glErrorHistory->m_notes.back().min_err;
			m_dialTune->m_max_error = m_glErrorHistory->m_notes.back().max_err;
			ui_txtFreq->display(m_freq);

			// refresh intonal tuning cursor
			m_microtonalView->setAFreq(Music::GetAFreq());
			m_microtonalView->updateCursor(m_freq);

			// volume
			m_glVolumeHistory->addVolume(max_component);

			// refresh sample data
			refresh_data_sample();

			// refresh formants data
			refresh_data_harmonics();

			m_glStatistics->addNote(f2h(m_compared_freq), m_error);
		}
	}

	if(m_time_refresh_views.elapsed()>50)	// 20 images/second max
		refresh_views();
}

void CustomInstrumentTunerForm::noteStarted(double freq, double dt)
{
    Q_UNUSED(freq)
    Q_UNUSED(dt)
// 	cout << "CustomInstrumentTunerForm::noteStarted " << freq << "," << dt << endl;

	// set the compared freq
	if(m_microtonalView->setting_show->isChecked() && m_microtonalView->hasTuningFreqSelected())
		m_compared_freq = m_microtonalView->getTuningFreq();
	else
		m_compared_freq = m_quantizer->getCenterFrequency();	// h2f(f2h(freq));

	if(m_microtonalView->setting_show->isChecked() && m_microtonalView->hasTuningFreqSelected())
	{
		ui_txtNoteFreq->display(int(m_microtonalView->getTuningFreq()*100)/100.0);
		ui_txtNote->setText(m_microtonalView->getTuningNoteName());
		if(m_microtonalView->m_selected_jivalue->is_ratio)
		{
			m_glErrorHistory->addNote(GLErrorHistory::Note(m_microtonalView->setting_selectedRoot, m_microtonalView->m_selected_jivalue->num, m_microtonalView->m_selected_jivalue->den));
			m_glVolumeHistory->addNote(GLVolumeHistory::Note(m_microtonalView->setting_selectedRoot, m_microtonalView->m_selected_jivalue->num, m_microtonalView->m_selected_jivalue->den));
		}
		else
		{
			m_glErrorHistory->addNote(GLErrorHistory::Note(m_microtonalView->setting_selectedRoot, m_microtonalView->m_selected_jivalue->cents));
			m_glVolumeHistory->addNote(GLVolumeHistory::Note(m_microtonalView->setting_selectedRoot, m_microtonalView->m_selected_jivalue->cents));
		}
	}
	else
	{
		ui_txtNoteFreq->display(m_compared_freq);
		ui_txtNote->setText(QString::fromStdString(h2n(f2h(m_compared_freq))));
		m_glErrorHistory->addNote(GLErrorHistory::Note(f2h(m_compared_freq)));
		m_glVolumeHistory->addNote(GLVolumeHistory::Note(f2h(m_compared_freq)));
	}
}
void CustomInstrumentTunerForm::noteFinished(double freq, double dt)
{
    Q_UNUSED(freq)
    Q_UNUSED(dt)
	m_compared_freq = 0.0;
// 	cout << "CustomInstrumentTunerForm::noteFinished " << freq << "," << dt << endl;
}

void CustomInstrumentTunerForm::refresh_data_sample()
{
	if(m_freq==0.0f || !m_glSample->setting_show->isChecked())
	{
		m_glSample->clear();
		return;
	}

	deque<double> sample;
	GetWaveSample(m_queue, size_t(m_capture_thread.getSamplingRate()/m_freq), sample);
	m_glSample->add(m_time.elapsed(), sample);
}

void CustomInstrumentTunerForm::refresh_data_harmonics()
{
	if(!(m_algo_combedfft!=NULL &&
			m_freq>0.0f &&
			m_glFreqStruct->setting_show->isChecked()))
		return;

    for(int i=0; i<int(m_glFreqStruct->m_components.size()); i++)
        m_glFreqStruct->m_components[i] = -1e6;

    vector<Harmonic> harms = GetHarmonicStruct(m_algo_combedfft->m_plan.out, m_freq, m_glFreqStruct->m_components.size(), m_algo_combedfft->getZeroPaddingFactor(), 0.1, true);

    m_glFreqStruct->m_components_max = 0.0;
    for(int i=0; i<int(harms.size()); i++)
	{
        if(harms[i].harm_number<=int(m_glFreqStruct->m_components.size()))
		{
            m_glFreqStruct->m_components[harms[i].harm_number-1] = 20*log10(harms[i].mod*1000.0);

 			m_glFreqStruct->m_components_max = max(m_glFreqStruct->m_components_max, m_glFreqStruct->m_components[i]);
		}
	}
}

void CustomInstrumentTunerForm::refresh_views()
{
// 	cout << "CustomInstrumentTunerForm::refresh_views " << endl;

//	m_dialTune->repaint();

	if(m_glGraph->setting_show->isChecked())
		m_glGraph->updateGL();

	if(m_glErrorHistory->setting_show->isChecked())
		m_glErrorHistory->updateGL();

	if(m_glVolumeHistory->setting_show->isChecked())
		m_glVolumeHistory->updateGL();

	if(m_microtonalView->setting_show->isChecked())
		m_microtonalView->update();

	if(m_glSample->setting_show->isChecked())
		m_glSample->updateGL();

	if(m_glFreqStruct->setting_show->isChecked())
		m_glFreqStruct->updateGL();

	if(m_glFT->setting_show->isChecked())
		m_glFT->updateGL();

	if(m_glStatistics->setting_show->isChecked())
		m_glStatistics->updateGL();

	m_time_refresh_views.start();
}

void CustomInstrumentTunerForm::keyPressEvent(QKeyEvent * e)
{
	if(e->key()==Qt::Key_F)
		toggleFullScreen();
}

void CustomInstrumentTunerForm::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e)
	update_views();
}

void CustomInstrumentTunerForm::update_views()
{
	if(	!m_glGraph->setting_show->isChecked() &&
		!m_glErrorHistory->setting_show->isChecked() &&
		!m_glVolumeHistory->setting_show->isChecked() &&
		!m_glSample->setting_show->isChecked() &&
		!m_glFreqStruct->setting_show->isChecked() &&
		!m_glFT->setting_show->isChecked())
			m_dialTune->setMaximumWidth(size().width());
	else
		m_dialTune->setMaximumWidth(ui_rightLayout->minimumSize().width());

	if(m_glGraph->setting_show->isChecked() &&
			!m_glErrorHistory->setting_show->isChecked() &&
			!m_glVolumeHistory->setting_show->isChecked() &&
			!m_glSample->setting_show->isChecked() &&
			!m_glFreqStruct->setting_show->isChecked() &&
			!m_glFT->setting_show->isChecked())
		m_glGraph->setMaximumHeight(size().height());
	else
		m_glGraph->setMaximumHeight(m_glGraph->setting_spinMaxHeight->value());

	if(!m_glErrorHistory->setting_show->isChecked() && !m_glVolumeHistory->setting_show->isChecked())
		ui_btnKeepErrorHistory->hide();
	else
		ui_btnKeepErrorHistory->show();
}

void CustomInstrumentTunerForm::configure()
{
    // Checking PortAudio devices while capturing using ALSA seems to crash ALSA
    // thus, stop everything while configuring and capture again when exiting the configuration panel
    m_capture_thread.stopCapture();

	noteRangeChanged();

    if(m_capture_thread.getCurrentTransportIndex()<m_config_form.ui_cbTransports->count())
		m_config_form.ui_cbTransports->setCurrentIndex(m_capture_thread.getCurrentTransportIndex());

#ifdef CAPTURE_JACK
	// TODO set descr
	m_config_form.ui_grpJACK->setTitle(m_capture_thread.getTransport("JACK")->getDescription());
	m_config_form.ui_lblJACKSamplingRate->setText(QString::number(m_capture_thread.getSamplingRate()));
#endif

#ifdef CAPTURE_PORTAUDIO
    m_config_form.ui_grpPortAudio->setTitle(m_capture_thread.getTransport("PortAudio")->getDescription());
	m_config_form.ui_spinPortAudioSamplingRate->setValue(m_capture_thread.getSamplingRate());
	if(m_capture_thread.getTransport("PortAudio"))
	{
		try
		{
			PaError err;
			err = Pa_Initialize();
			if(err != paNoError)
				throw QString("PortAudio: CustomInstrumentTunerForm::configure:Pa_Initialize ")+Pa_GetErrorText(err);
			int	numDevices = Pa_GetDeviceCount();
			int current_index = -1;
			m_config_form.ui_cbPortAudioDeviceName->clear();
			const PaDeviceInfo* deviceInfo;
			for(int i=0; i<numDevices; i++)
			{
				deviceInfo = Pa_GetDeviceInfo(i);
				m_config_form.ui_cbPortAudioDeviceName->addItem(QString(deviceInfo->name));
				if(QString(deviceInfo->name)==m_capture_thread.getTransport("PortAudio")->getSource())
					current_index = i;
			}
            if(current_index!=-1)
                m_config_form.ui_cbPortAudioDeviceName->setCurrentIndex(current_index);
		}
		catch(QString error)
		{
            cout << "CustomInstrumentTunerForm: ERROR: " << error.toStdString() << endl;
		}
		Pa_Terminate();
	}
#endif

#ifdef CAPTURE_ALSA
	m_config_form.ui_grpALSA->setTitle(m_capture_thread.getTransport("ALSA")->getDescription());
	m_config_form.ui_spinALSASamplingRate->setValue(m_capture_thread.getSamplingRate());
#endif
#ifdef CAPTURE_OSS
	m_config_form.ui_grpOSS->setTitle(m_capture_thread.getTransport("OSS")->getDescription());
	m_config_form.ui_spinOSSSamplingRate->setValue(m_capture_thread.getSamplingRate());
#endif
#ifdef CAPTURE_QT
    m_config_form.ui_grpQt->setTitle(m_capture_thread.getTransport("Qt")->getDescription());
    m_config_form.ui_spinQtSamplingRate->setValue(m_capture_thread.getSamplingRate());
    {
        QString saved_device = m_settings.value(m_config_form.ui_cbQtDeviceName->objectName(), "default").toString();
        try
        {
            QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);

            int current_index = -1;
            m_config_form.ui_cbQtDeviceName->clear();
            for(int i=0; i<devices.count(); i++) {
                m_config_form.ui_cbQtDeviceName->addItem(devices.at(i).deviceName());
                if(devices.at(i).deviceName()==m_capture_thread.getTransport("Qt")->getSource())
                    current_index = i;
            }
            if(current_index!=-1)
                m_config_form.ui_cbQtDeviceName->setCurrentIndex(current_index);
        }
        catch(QString error)
        {
            cout << "CustomInstrumentTunerForm: ERROR: " << error.toStdString() << endl;
        }
    }
#endif

    m_config_form.adjustSize();
	m_config_form.show();
}
void CustomInstrumentTunerForm::configure_cancel()
{
    if(!pauseAction->isChecked() && !m_capture_thread.isCapturing())
        m_capture_thread.startCapture();
}
void CustomInstrumentTunerForm::configure_ok()
{
    switch(m_config_form.ui_cbTuning->currentIndex())
	{
	case 0:
		SetTuning(CHROMATIC);
		break;
	case 1:
		SetTuning(WERCKMEISTER3);
		break;
	case 2:
		SetTuning(KIRNBERGER3);
		break;
	case 3:
		SetTuning(DIATONIC);
		break;
	case 4:
		SetTuning(MEANTONE);
		break;
	default:
		SetTuning(CHROMATIC);
		break;
	}

	if(m_config_form.ui_cbTonality->currentIndex()==0)		SetTonality(0);
	else if(m_config_form.ui_cbTonality->currentIndex()==1)	SetTonality(+2);
	else													SetTonality(-3);

	if(m_config_form.ui_cbNotesName->currentIndex()==0)		SetNotesName(LOCAL_ANGLO);
	else													SetNotesName(LOCAL_LATIN);
	m_microtonalView->notesNameChanged();
	m_microtonalView->setAFreq(Music::GetAFreq());

	SetSemitoneBounds(m_config_form.ui_spinMinHT->value(), m_config_form.ui_spinMaxHT->value());

    ui_spinA3Offset->setVisible(m_config_form.ui_chkShowA4Offset->isChecked());
    ui_lblA3Offset->setVisible(m_config_form.ui_chkShowA4Offset->isChecked());

	//	if(m_note!=-1000)
	//		ui_txtNote->setText(h2n(m_note));

	//	m_dialTune->setError(-10.0f);

// 	cout << "b" << endl;

	// Capture
#ifdef CAPTURE_QT
    if(m_config_form.ui_cbTransports->currentText()=="Qt")
    {
        m_capture_thread.selectTransport("Qt");
        m_capture_thread.setSource(m_config_form.ui_cbQtDeviceName->currentText());
        if(m_config_form.ui_chkQtSamplingRateMax->isChecked())
            m_capture_thread.setSamplingRate(CaptureThread::SAMPLING_RATE_MAX);
        else
            m_capture_thread.setSamplingRate(m_config_form.ui_spinQtSamplingRate->value());
    }
#endif
#ifdef CAPTURE_ALSA
	if(m_config_form.ui_cbTransports->currentText()=="ALSA")
	{
		m_capture_thread.selectTransport("ALSA");
		m_capture_thread.setSource(m_config_form.ui_txtALSAPCMName->text());
		if(m_config_form.ui_chkALSASamplingRateMax->isChecked())
			m_capture_thread.setSamplingRate(CaptureThread::SAMPLING_RATE_MAX);
		else
			m_capture_thread.setSamplingRate(m_config_form.ui_spinALSASamplingRate->value());
	}
#endif
#ifdef CAPTURE_JACK
	if(m_config_form.ui_cbTransports->currentText()=="JACK")
	{
		m_capture_thread.selectTransport("JACK");
		if(m_config_form.ui_chkJACKAutoConnect->isChecked())
			m_capture_thread.setSource(m_config_form.ui_txtJACKSourcePort->text());
		else
			m_capture_thread.setSource("");
		m_config_form.ui_lblJACKSamplingRate->setText(QString::number(m_capture_thread.getSamplingRate()));
	}
#endif
#ifdef CAPTURE_PORTAUDIO
	if(m_config_form.ui_cbTransports->currentText()=="PortAudio")
	{
		m_capture_thread.selectTransport("PortAudio");
		m_capture_thread.setSource(m_config_form.ui_cbPortAudioDeviceName->currentText());
		if(m_config_form.ui_chkPortAudioSamplingRateMax->isChecked())
			m_capture_thread.setSamplingRate(CaptureThread::SAMPLING_RATE_MAX);
		else
			m_capture_thread.setSamplingRate(m_config_form.ui_spinPortAudioSamplingRate->value());
	}
#endif
#ifdef CAPTURE_OSS
	if(m_config_form.ui_cbTransports->currentText()=="OSS")
	{
		m_capture_thread.selectTransport("OSS");
		m_capture_thread.setSource(m_config_form.ui_txtOSSPCMName->text());
		if(m_config_form.ui_chkOSSSamplingRateMax->isChecked())
			m_capture_thread.setSamplingRate(CaptureThread::SAMPLING_RATE_MAX);
		else
			m_capture_thread.setSamplingRate(m_config_form.ui_spinOSSSamplingRate->value());
	}
#endif
	m_timer_refresh.setInterval(m_config_form.ui_spinRefreshTime->value());

	// Views
	m_glGraph->m_treshold = invlp(m_config_form.ui_spinVolumeTreshold->value());
	m_glGraph->clearValues();

// 	cout << "c" << endl;

	if(m_config_form.ui_grpRangeFiltering->isChecked())
	{
//         cout << "set filters " << GetSamplingRate() << endl;
//         m_rect_range_filter.reset(int(GetSamplingRate()/6000.0));
		m_rect_range_filter.reset(int(GetSamplingRate()/h2f(GetSemitoneMin())));
//         m_fir_range_filter.setImpulseResponse(fir1_bandpass(104, 2*1000.0/GetSamplingRate(), 2*2000.0/GetSamplingRate()));
// 		m_fir_range_filter.setImpulseResponse(fir1_highpass(int(GetSamplingRate()/h2f(GetSemitoneMin())), 0.5));

		if(m_config_form.ui_rdRangeFilteringRectangular->isChecked())
			m_range_filter = &m_rect_range_filter;
		else if(m_config_form.ui_rdRangeFilteringFIR->isChecked())
			m_range_filter = &m_fir_range_filter;
	}
	else
		m_range_filter = &m_dummy_range_filter;

	m_algo_combedfft->setWindowFactor(m_config_form.ui_spinWindowSizeFactor->value());
//	m_glFT->m_zp_factor = m_config_form.ui_spinWindowSizeFactor->value();
	m_algo_combedfft->useAudibilityRatio(m_config_form.ui_chkAlgoUseSubHarmTresh->isChecked());
	m_algo_combedfft->setAudibilityRatio(invlp(-m_config_form.ui_spinCombedFFTAudibilityRatio->value()));
	m_algo_combedfft->setAmplitudeTreshold(invlp(double(m_config_form.ui_spinVolumeTreshold->value())));
	m_algo_combedfft->setComponentTreshold(invlp(double(m_config_form.ui_spinVolumeTreshold->value())));

// 	cout << "d" << endl;

	// Quantizers
	m_quantizer->reset();
	if(m_config_form.ui_grpQuantizer->isChecked())
	{
		m_latency_quantizer.setLatency(m_config_form.ui_spinErrorLatency->value());
		m_quantizer = &m_latency_quantizer;
	}
	else
		m_quantizer = &m_dummy_quantizer;

// 	cout << invlp(-m_config_form.ui_spinCombedFFTAudibilityRatio->value()) << endl;

	if(!pauseAction->isChecked() && !m_capture_thread.isCapturing())
		m_capture_thread.startCapture();
}

void CustomInstrumentTunerForm::saveSettings()
{
	m_settings.save();
	View::saveAll();

	// views
	m_settings.setValue("width", width());
	m_settings.setValue("height", height());
	//m_settings.setValue("ui_tbViews", QVariant(ui_tbViews->isVisible()));
	//m_settings.setValue("ui_tbButtons", ui_tbButtons->isVisible());

	// sound capture
	m_settings.setValue(m_config_form.ui_cbTransports->objectName(), m_config_form.ui_cbTransports->currentText());
#ifdef CAPTURE_QT
    m_settings.setValue(m_config_form.ui_cbQtDeviceName->objectName(), m_config_form.ui_cbQtDeviceName->currentText());
#endif
#ifdef CAPTURE_PORTAUDIO
	m_settings.setValue(m_config_form.ui_cbPortAudioDeviceName->objectName(), m_config_form.ui_cbPortAudioDeviceName->currentText());
#endif
}
void CustomInstrumentTunerForm::loadSettings()
{
//     cout << __FILE__ << ":" << __LINE__ << endl;

	m_settings.load();
	View::loadAll();

	// views
	resize(m_settings.value("width", 800).toInt(), m_settings.value("height", 600).toInt());
	//ui_tbViews->setVisible(m_settings.value("ui_tbViews", ui_tbViews->isVisible()).toBool());
	//ui_tbButtons->setVisible(m_settings.value("ui_tbButtons", ui_tbButtons->isVisible()).toBool());

	// sound capture
	QString saved_transport = m_settings.value(m_config_form.ui_cbTransports->objectName(), "").toString();
	if(saved_transport!="")
		for(int i=0; i<m_config_form.ui_cbTransports->count(); i++)
			if(m_config_form.ui_cbTransports->itemText(i)==saved_transport)
				m_config_form.ui_cbTransports->setCurrentIndex(i);

#ifdef CAPTURE_QT
    {
        QString saved_device = m_settings.value(m_config_form.ui_cbQtDeviceName->objectName(), "default").toString();
        try
        {
            QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);

            int saved_index = -1;
            m_config_form.ui_cbQtDeviceName->clear();
            for(int i=0; i<devices.count(); i++)
            {
                m_config_form.ui_cbQtDeviceName->addItem(devices.at(i).deviceName());
                if(devices.at(i).deviceName()==saved_device)
                    saved_index = i;
            }
            if(saved_index!=-1)
                m_config_form.ui_cbQtDeviceName->setCurrentIndex(saved_index);
        }
        catch(QString error)
        {
    //		cout << "CustomInstrumentTunerForm: ERROR: " << error << endl;
        }
    }
#endif

#ifdef CAPTURE_PORTAUDIO
    {
        QString saved_device = m_settings.value(m_config_form.ui_cbPortAudioDeviceName->objectName(), "default").toString();
        try
        {
            PaError err;
            err = Pa_Initialize();
            if(err != paNoError)
                throw QString("PortAudio: CustomInstrumentTunerForm::loadSettings:Pa_Initialize ")+Pa_GetErrorText(err);
            int	numDevices = Pa_GetDeviceCount();
    //         cout << "PortAudio devices:"<< endl;
            int saved_index = -1;
            m_config_form.ui_cbPortAudioDeviceName->clear();
            const PaDeviceInfo* deviceInfo;
            for(int i=0; i<numDevices; i++)
            {
                deviceInfo = Pa_GetDeviceInfo(i);
    //             cout << "    " << QString(deviceInfo->name).toStdString() << endl;
                m_config_form.ui_cbPortAudioDeviceName->addItem(QString(deviceInfo->name));
                if(QString(deviceInfo->name)==saved_device)
                    saved_index = i;
            }
            if(saved_index!=-1)
                m_config_form.ui_cbPortAudioDeviceName->setCurrentIndex(saved_index);
        }
        catch(QString error)
        {
    //		cout << "CustomInstrumentTunerForm: ERROR: " << error << endl;
        }
        Pa_Terminate();
    }
#endif
}

void CustomInstrumentTunerForm::restoreFactorySettings()
{
	if(QMessageBox::question(this, tr("Restore Factory Settings"), tr("This operation is NOT reversible.\nAre you sure you want to lose all your current settings ?"), QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
	{
		m_settings.clear();
		View::clearAllSettings();

		m_settings.remove("width");
		m_settings.remove("height");
        m_settings.remove("ui_cbTransports");
		//m_settings.remove("ui_tbViews");
		//m_settings.remove("ui_tbButtons");

		m_settings.remove(m_config_form.ui_cbPortAudioDeviceName->objectName());

		QMessageBox::information(this, tr("Restore Factory Settings"), tr("You can now restart FMIT to get back factory settings"), QMessageBox::Ok, QMessageBox::NoButton);
	}
}

void CustomInstrumentTunerForm::helpAbout()
{
    AboutBox box(this);
    box.exec();

//    QString text;
//    text = "<h2>Free Music Instrument Tuner</h2>";

//    QString	fmitversion;
//    QString fmitversiongit(STR(FMITVERSIONGIT));
//    if(fmitversiongit.length()>0) {
//        fmitversion = tr("Version ") + fmitversiongit;
//    }
//    else {
//        QFile readmefile(":/fmit/README.txt");
//        readmefile.open(QFile::ReadOnly | QFile::Text);
//        QTextStream readmefilestream(&readmefile);
//        readmefilestream.readLine();
//        readmefilestream.readLine();
//        readmefilestream.readLine();
//        fmitversion = readmefilestream.readLine().simplified();
//    }
//    text += tr("<h3>")+fmitversion;

//    text += tr(" (compiled by ")+QString(COMPILER)+tr(" for ");
//    #ifdef Q_PROCESSOR_X86_32
//      text += "32bits";
//    #endif
//    #ifdef Q_PROCESSOR_X86_64
//      text += "64bits";
//    #endif
//    text += ")";

//    text += "</h3><p><h3>"+tr("Website: ")+"</h3><p>"+tr("Homepage: ")+"<a href=\"http://gillesdegottex.github.io/fmit/\">http://gillesdegottex.github.io/fmit/</a>";
//    text += "<p>"+tr("Development site: ")+"<a href=\"http://github.com/gillesdegottex/fmit\">http://github.com/gillesdegottex/fmit</a>";
////	text += tr("<p>donation link: <a href=\"http://gillesdegottex.github.io/fmit/\">http://gillesdegottex.github.io/fmit/</a>");
//    text += "<p><h3>"+tr("Author: ")+"</h3><p>Gilles Degottex <a href=\"mailto:gilles.degottex@gmail.com\">gilles.degottex@gmail.com</a>";
//#ifdef PACKAGER_STRING
//	if(PACKAGER_STRING!="")
//        text += "<p><h3>"+tr("Packager: ")+"</h3><p>"+QString(PACKAGER_STRING).replace(QChar('<'),"[").replace(QChar('>'),"]");
//#endif

//	QDialog about_dlg(this);

//	QTextBrowser* textBrowser1;
//	QPushButton* pushButton1;
//	QVBoxLayout* Form2Layout;
//	QHBoxLayout* layout1;
//	QSpacerItem* spacer1;
//	QSpacerItem* spacer2;

//    about_dlg.setObjectName("about_box");
//	about_dlg.setWindowTitle( tr("About Free Music Instrument Tuner") );

//	Form2Layout = new QVBoxLayout( &about_dlg );
//	Form2Layout->setMargin(11);
//	Form2Layout->setSpacing(6);
//	about_dlg.setLayout( Form2Layout );

//	textBrowser1 = new QTextBrowser( &about_dlg );
//	textBrowser1->setHtml(text);
//	textBrowser1->setOpenExternalLinks(true);
//	textBrowser1->setWordWrapMode(QTextOption::NoWrap);
//	textBrowser1->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	textBrowser1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	textBrowser1->document()->adjustSize();
//	textBrowser1->setMinimumSize(textBrowser1->document()->size().toSize() + QSize(10, 10));
//	Form2Layout->addWidget( textBrowser1 );

//	layout1 = new QHBoxLayout();
//	layout1->setMargin(0);
//	layout1->setSpacing(6);

//	spacer1 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
//	layout1->addItem( spacer1 );

//	pushButton1 = new QPushButton(&about_dlg);
//	pushButton1->setText( tr( "&OK" ) );
//	layout1->addWidget( pushButton1 );
//	connect(pushButton1, SIGNAL(clicked()), &about_dlg, SLOT(close()));

//	spacer2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
//	layout1->addItem( spacer2 );

//	Form2Layout->addLayout( layout1 );

//	about_dlg.exec();
}

CustomInstrumentTunerForm::~CustomInstrumentTunerForm()
{
	if(m_config_form.ui_chkAutoSaveOnExit->isChecked())
		saveSettings();
	else
	{
		m_settings.beginGroup("Auto/");
		m_settings.save(m_config_form.ui_chkAutoSaveOnExit);
		m_settings.endGroup();
	}
}

/*
bool displayInBrowser(const QString& theURL)
{
#ifdef _WIN32
	//TODO replace with less buggy ShellExecuteEx?
	if ((unsigned
int)::ShellExecute(qApp->mainWidget()->winId(),NULL,theURL,NULL,NULL,SW_SHOW)
<= 32)
{
		OFMessageBox::criticalMessageOK(QMessageBox::tr("Unable to display a
web browser. Ensure that you have a web browser installed."));
}
#else
	//TODO warn if netscape not installed
	QString aCommand("netscape ");
	aCommand += theURL;
	aCommand += " &";
    if (system(aCommand) != 0)
{
		OFMessageBox::criticalMessageOK(QMessageBox::tr("Unable to display a
netscape browser. You need to have netscape installed and in the
path."));
}

return true;
}
*/

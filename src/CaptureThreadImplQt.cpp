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


#include "CaptureThread.h"

#include <cassert>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include <QList>
#include <QSet>
#include <QCoreApplication>

// ------------------------------ Qt implementation ----------------------------
#ifdef CAPTURE_QT

CaptureThreadImplQt::CaptureThreadImplQt(CaptureThread* capture_thread)
    : CaptureThreadImpl(capture_thread, "Qt", QString("Qt (lib:")+qVersion()+")")
    , m_audioInputDevice(QAudioDeviceInfo::defaultInputDevice())
    , m_audioInput(NULL)
    , m_audioInputIODevice(NULL)
{
    m_source = "default";
}

bool CaptureThreadImplQt::is_available()
{
    try
    {
        if(QAudioDeviceInfo::availableDevices(QAudio::AudioInput).count() == 0)
            throw QString("Qt: is_available: no device available");
    }
    catch(QString error)
    {
        m_status = "N/A";
        return false;
    }

    m_status = "OK";

	return true;
}

void CaptureThreadImplQt::capture_init()
{
//    cout << "CaptureThreadImplQt::capture_init" << endl;

    set_params(false);

    if (m_audioInput) {
        if (QAudio::SuspendedState == m_state)
            m_audioInput->stop();

        m_audioInputIODevice = m_audioInput->start();
        connect(m_audioInputIODevice, SIGNAL(readyRead()),
                        this, SLOT(audioDataReady()));
    }

    m_capture_thread->m_capturing = true;
    m_capture_thread->emitCaptureStarted();
    m_capture_thread->emitCaptureToggled(true);

//    cout << "CaptureThreadImplQt::~capture_init" << endl;
}

void CaptureThreadImplQt::capture_finished()
{
    if (m_audioInput) {
        m_audioInput->stop();
        QCoreApplication::instance()->processEvents();
        m_audioInput->disconnect();
    }
    m_audioInputIODevice = NULL;

    m_capture_thread->m_capturing = false;
    m_capture_thread->emitCaptureStoped();
    m_capture_thread->emitCaptureToggled(false);
}

void CaptureThreadImplQt::startCapture()
{
//    cout << "CaptureThreadImplQt::startCapture" << endl;

    try
    {
        capture_init();
    }
    catch(QString error)
    {
        capture_finished();
        cout << "CaptureThread: ERROR: " << error.toStdString() << endl;
        m_capture_thread->emitError(error);
    }
}
void CaptureThreadImplQt::stopCapture()
{
//    cout << "CaptureThreadImplQt::stopCapture" << endl;

    try
    {
        capture_finished();
    }
    catch(QString error)
    {
        cout << "CaptureThread: ERROR: " << error.toStdString() << endl;
        m_capture_thread->emitError(error);
    }
}

void CaptureThreadImplQt::set_params(bool test) {
    Q_UNUSED(test)
//    cout << "CaptureThreadImplQt::set_params " << getASCIISource().toLatin1().constData() << endl;

    if(getASCIISource()=="")
        m_audioInputDevice = QAudioDeviceInfo::defaultInputDevice();
    else {

        QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);

        m_audioInputDevice = QAudioDeviceInfo();
        for(int i=0; m_audioInputDevice.isNull() && i<devices.count(); i++)
            if(getASCIISource()==devices.at(i).deviceName()
                || m_source==devices.at(i).deviceName())
                m_audioInputDevice = devices.at(i);
    }

    if(m_audioInputDevice.isNull())
        throw QString("Qt: Cannot open device '")+getASCIISource()+"'";

    QAudioFormat format = m_format;

    // TODO Should list all possible parameters ...
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);

    // Test channels
    format.setSampleRate(44100); // Try with a very common sampling rate
    format.setChannelCount(1);
    if(!m_audioInputDevice.isFormatSupported(format)) {
        QString err_msg = QString("ALSA: cannot set channel count to 1");
        cout << "CaptureThread: WARNING: " << err_msg.toStdString() << endl;
        format.setChannelCount(2);
        if(!m_audioInputDevice.isFormatSupported(format))
            throw QString("Qt: Cannot set number of channels to 1 or 2");
    }

    setFormatDescrsAndFns(format.sampleSize()/8, format.sampleType()==QAudioFormat::SignedInt, false, format.channelCount());

    if(m_sampling_rate==CaptureThread::SAMPLING_RATE_MAX || m_sampling_rate==CaptureThread::SAMPLING_RATE_UNKNOWN)
    {
        int old_sampling_rate = m_sampling_rate;

        cout << "CaptureThread: INFO: Qt: sampling rate set to max or undefined, try to determinate it." << endl;

        QList<int> sampling_rates;
        #ifdef Q_OS_WIN
            // The Windows audio backend does not correctly report format support
            // (see QTBUG-9100).  Furthermore, although the audio subsystem captures
            // at 11025Hz, the resulting audio is corrupted.
            sampling_rates += 8000;
        #endif

        sampling_rates += m_audioInputDevice.supportedSampleRates();

        sampling_rates = sampling_rates.toSet().toList(); // remove duplicates
        qSort(sampling_rates.begin(), sampling_rates.end(), qGreater<int>());

//        cout << "Number of sampling rates: " << sampling_rates.count() << endl;
//        int sr;
//        foreach(sr, sampling_rates)
//            cout << sr << endl;

        bool foundsr = false;
        while(!foundsr)
        {
            if(sampling_rates.empty())
                throw QString("Qt: Cannot set any sample rate");

            m_sampling_rate = sampling_rates.front();
            cout << "CaptureThread: INFO: Qt: Try sampling rate " << m_sampling_rate << " ..." << flush;

            format.setSampleRate(m_sampling_rate);

            foundsr = m_audioInputDevice.isFormatSupported(format);

            if(!foundsr)	cout << " failed" << endl;
            else            cout << " success" << endl;

            sampling_rates.pop_front();
        }

        if(old_sampling_rate!=m_sampling_rate)
            m_capture_thread->emitSamplingRateChanged();
    }
    else
    {
        format.setSampleRate(m_sampling_rate);
        if(!m_audioInputDevice.isFormatSupported(format))
            throw QString("Qt: Cannot set sampling rate");
    }

    if(format != QAudioFormat()) {
        if (m_format != format) {
            m_format = format;
            if(m_audioInput)
                delete m_audioInput;
            m_audioInput = new QAudioInput(m_audioInputDevice, m_format, this);
            m_audioInput->setNotifyInterval(100);
            if(m_audioInput->error()!=QAudio::NoError)
                throw QString("Qt: Cannot create audio input with the given device and format");
            connect(m_audioInput, SIGNAL(stateChanged(QAudio::State)),
                            this, SLOT(audioStateChanged(QAudio::State)));
//            connect(m_audioInput, SIGNAL(notify()),
//                            this, SLOT(audioNotify()));
            m_audioInputIODevice = NULL;
        }
    } else {
        throw QString("Qt: cannot set parameters");
    }

//    cout << __FILE__ << ":" << __LINE__ << endl;
}

void CaptureThreadImplQt::setSamplingRate(int value)
{
//    cout << "CaptureThreadImplQt::setSamplingRate " << value << endl;

	assert(value>0 || value==CaptureThread::SAMPLING_RATE_MAX);

	if(m_sampling_rate!=value || value==CaptureThread::SAMPLING_RATE_MAX)
	{
		bool was_running = m_capture_thread->isCapturing();
		if(was_running)	m_capture_thread->stopCapture();

		m_sampling_rate = value;

		if(m_sampling_rate==CaptureThread::SAMPLING_RATE_MAX)
		{
			try
			{
				set_params(true);
			}
			catch(QString error)
			{
                cout << "CaptureThread: ERROR: " << error.toStdString() << endl;
				m_capture_thread->emitError(error);
			}

			// it was just for testing
			capture_finished();
		}
		else
			m_capture_thread->emitSamplingRateChanged();

		if(was_running)	m_capture_thread->startCapture();
	}

// 	cout << "~CaptureThreadImplQt::setSamplingRate" << endl;
}

void CaptureThreadImplQt::audioStateChanged(QAudio::State state) {
    Q_UNUSED(state)
//    cout << "CaptureThreadImplQt::audioStateChanged" << endl;

//    if (state==QAudio::IdleState) {
//        stopPlayback();
//    } else {
//        if (QAudio::StoppedState == state) {
//            // Check error
//            QAudio::Error error = QAudio::NoError;
//            error = m_audioInput->error();
//            if (QAudio::NoError != error) {
//                reset();
//                return;
//            }
//        }
//        setState(state);
//    }
}

void CaptureThreadImplQt::audioDataReady()
{
//    cout << "CaptureThreadImplQt::audioDataReady " << m_audioInput->bytesReady() << endl;

    if(m_audioInput) {
        m_capture_thread->m_lock.lock();

        const qint64 bytesReady = m_audioInput->bytesReady();

        qint64 nbframes = bytesReady/(m_format.sampleSize()/8);

        if(m_capture_thread->m_pause) {
            // Can't find a neater way to do it
            // m_audioInput->reset() blocks the input
            for(qint64 i=0; i<nbframes; i++) {
                long int value = 0; // "Allocate" a "buffer"
                m_audioInputIODevice->read((char*)&value, m_format.sampleSize()/8);
            }
        }
        else {

            for(qint64 i=0; i<nbframes; i++) {
                long int value = 0; // "Allocate" a "buffer"
                qint64 bytesRead = m_audioInputIODevice->read((char*)&value, m_format.sampleSize()/8);

                if(bytesRead>0)
                    addValue(this, decodeValue((void*)&value, 0), i);
            }

            m_capture_thread->m_packet_size = nbframes;
            if(m_capture_thread->m_ext_lock)
            {
                m_capture_thread->m_packet_size_sll = 0;
                m_capture_thread->m_ext_lock = false;
            }
            m_capture_thread->m_packet_size_sll += nbframes;
        }

        m_capture_thread->m_lock.unlock();
    }
}

CaptureThreadImplQt::~CaptureThreadImplQt()
{
//    cout << "CaptureThreadImplQt::~CaptureThreadImplQt" << endl;

	stopCapture();
}

#endif

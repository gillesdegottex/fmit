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
// along with this program; if not, write to the Free Software, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


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
#include <QDebug>

// ------------------------------ Qt implementation ----------------------------
#ifdef CAPTURE_QT

CaptureThreadImplQt::CaptureThreadImplQt(CaptureThread* capture_thread)
    : CaptureThreadImpl(capture_thread, "Qt", QString("Qt (lib:")+qVersion()+")")
    , m_availableAudioInputDevices(QMediaDevices::audioInputs())
    , m_audioInputDevice(QMediaDevices::defaultAudioInput())
    , m_audioSource(NULL)
    , m_audioInputIODevice(NULL)
{
    m_source = "default";
    qInfo().noquote() << "CaptureThread: Qt: Constructed, default device:" << m_audioInputDevice.description();
}

bool CaptureThreadImplQt::is_available()
{
    try
    {
        m_availableAudioInputDevices = QMediaDevices::audioInputs();
        if(m_availableAudioInputDevices.count() == 0)
            throw QString("Qt: is_available: no device available");
    }
    catch(QString error)
    {
        m_status = "N/A";
        qWarning().noquote() << "CaptureThread: Qt:" << error;
        return false;
    }

    m_status = "OK";
    qInfo().noquote() << "CaptureThread: Qt: Found" << m_availableAudioInputDevices.count() << "audio input device(s)";
    for (const QAudioDevice &device : m_availableAudioInputDevices) {
        qInfo().noquote() << "  -" << device.description() << (device.isDefault() ? "(default)" : "");
    }

	return true;
}

void CaptureThreadImplQt::capture_init()
{
    set_params(false);

    if (m_audioSource) {
        if (QAudio::SuspendedState == m_state)
            m_audioSource->stop();

        m_audioInputIODevice = m_audioSource->start();
        if (m_audioInputIODevice) {
            connect(m_audioInputIODevice, &QIODevice::readyRead,
                            this, &CaptureThreadImplQt::audioDataReady);
        } else {
            qWarning().noquote() << "CaptureThread: Qt: start() returned NULL IODevice";
        }
    }

    m_capture_thread->m_capturing = true;
    m_capture_thread->emitCaptureStarted();
    m_capture_thread->emitCaptureToggled(true);
}

void CaptureThreadImplQt::capture_finished()
{
    if (m_audioSource) {
        m_audioSource->stop();
        QCoreApplication::instance()->processEvents();
        m_audioSource->disconnect();
    }
    m_audioInputIODevice = NULL;

    m_capture_thread->m_capturing = false;
    m_capture_thread->emitCaptureStoped();
    m_capture_thread->emitCaptureToggled(false);
}

void CaptureThreadImplQt::startCapture()
{
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
    if(getASCIISource()=="") {
        m_audioInputDevice = QMediaDevices::defaultAudioInput();
    } else {
        m_availableAudioInputDevices = QMediaDevices::audioInputs();

        m_audioInputDevice = QAudioDevice();
        for(int i=0; m_audioInputDevice.isNull() && i<m_availableAudioInputDevices.count(); i++)
            if(getASCIISource()==m_availableAudioInputDevices.at(i).description()
                || m_source==m_availableAudioInputDevices.at(i).description())
                m_audioInputDevice = m_availableAudioInputDevices.at(i);
    }

    if(m_audioInputDevice.isNull())
        throw QString("Qt: Cannot open device '")+getASCIISource()+"'";

    QAudioFormat format = m_format;

    // Get device's preferred format (most likely to work)
    QAudioFormat preferredFormat = m_audioInputDevice.preferredFormat();
    qInfo().noquote() << "CaptureThread: Qt: Device preferred format - rate:" << preferredFormat.sampleRate()
                      << "channels:" << preferredFormat.channelCount()
                      << "format:" << preferredFormat.sampleFormat();

    // Check if isFormatSupported() is reliable by testing with preferred format
    // (it can fail when Media Foundation is broken on Windows)
    bool backendBroken = !m_audioInputDevice.isFormatSupported(preferredFormat);
    bool usePreferred = backendBroken;
    int old_sampling_rate = m_sampling_rate;

    // Determine target sampling rate for format testing
    int targetRate;
    if (m_sampling_rate == CaptureThread::SAMPLING_RATE_MAX || m_sampling_rate == CaptureThread::SAMPLING_RATE_UNKNOWN) {
        targetRate = 44100; // Default for auto-detect
    } else {
        targetRate = m_sampling_rate; // User's explicit choice
    }

    if(backendBroken) {
        // Backend completely broken, use preferred format as-is
        cout << "CaptureThread: WARNING: Qt: isFormatSupported() unreliable, using device preferred format" << endl;
        format = preferredFormat;
        m_sampling_rate = preferredFormat.sampleRate();
    }
    else
    {
        // Fallback chain:
        // 1) Float/mono @ target rate
        // 2) Float/preferred_channels @ target rate
        // 3) preferred_format/preferred_channels @ target rate
        // 4) preferred_format/preferred_channels @ preferred_rate (final fallback)

        // 1) Float/mono @ target rate
        format.setSampleFormat(QAudioFormat::Float);
        format.setSampleRate(targetRate);
        format.setChannelCount(1);

        bool formatSupported = m_audioInputDevice.isFormatSupported(format);
        if(!formatSupported) {
            // 2) Float/preferred_channels @ target rate
            cout << "CaptureThread: WARNING: Qt: Float/mono@" << targetRate << " not supported, trying Float/" << preferredFormat.channelCount() << "ch@" << targetRate << endl;
            format.setChannelCount(preferredFormat.channelCount());
            formatSupported = m_audioInputDevice.isFormatSupported(format);
            if(!formatSupported) {
                // 3) preferred_format/preferred_channels @ target rate
                const char* prefFmtName = (preferredFormat.sampleFormat() == QAudioFormat::Float) ? "Float" :
                                         (preferredFormat.sampleFormat() == QAudioFormat::Int16) ? "Int16" :
                                         (preferredFormat.sampleFormat() == QAudioFormat::Int32) ? "Int32" :
                                         (preferredFormat.sampleFormat() == QAudioFormat::UInt8) ? "UInt8" : "Unknown";
                cout << "CaptureThread: WARNING: Qt: Float/" << preferredFormat.channelCount() << "ch@" << targetRate << " not supported, trying " << prefFmtName << "/" << preferredFormat.channelCount() << "ch@" << targetRate << endl;
                format = preferredFormat;
                format.setSampleRate(targetRate);
                formatSupported = m_audioInputDevice.isFormatSupported(format);
                if(!formatSupported) {
                    // 4) preferred format entirely
                    cout << "CaptureThread: WARNING: Qt: " << prefFmtName << "/" << preferredFormat.channelCount() << "ch@" << targetRate << " not supported, using preferred rate " << preferredFormat.sampleRate() << endl;
                    format = preferredFormat;
                    usePreferred = true;
                }
            }
        }
        m_sampling_rate = format.sampleRate();
    }

    bool isFloat = (format.sampleFormat() == QAudioFormat::Float);
    setFormatDescrsAndFns(format.bytesPerSample(), true, isFloat, format.channelCount());

    if(usePreferred)
    {
        // Backend is broken or nothing else worked, use preferred format as-is
        m_sampling_rate = preferredFormat.sampleRate();
        format.setSampleRate(m_sampling_rate);
        cout << "CaptureThread: INFO: Qt: Using device preferred format" << endl;
    }
    else if(m_sampling_rate==CaptureThread::SAMPLING_RATE_MAX || m_sampling_rate==CaptureThread::SAMPLING_RATE_UNKNOWN)
    {
        cout << "CaptureThread: INFO: Qt: sampling rate set to max or undefined, try to determine it." << endl;

        int minRate = m_audioInputDevice.minimumSampleRate();
        int maxRate = m_audioInputDevice.maximumSampleRate();

        QList<int> sampling_rates;
        sampling_rates += maxRate;
        if (maxRate >= 96000) sampling_rates += 96000;
        if (maxRate >= 88200) sampling_rates += 88200;
        sampling_rates += 48000;
        sampling_rates += 44100;
        sampling_rates += 32000;
        sampling_rates += 22050;
        sampling_rates += 16000;
        sampling_rates += 11025;
        sampling_rates += 8000;

        sampling_rates = QSet<int>(sampling_rates.begin(), sampling_rates.end()).values(); // remove duplicates
        std::sort(sampling_rates.begin(), sampling_rates.end(), std::greater<int>());

        bool foundsr = false;
        for (int rate : sampling_rates) {
            if (rate < minRate || rate > maxRate)
                continue;
            m_sampling_rate = rate;
            format.setSampleRate(m_sampling_rate);
            if (m_audioInputDevice.isFormatSupported(format)) {
                cout << "CaptureThread: INFO: Qt: Selected sampling rate " << m_sampling_rate << endl;
                foundsr = true;
                break;
            }
        }
        if(!foundsr) {
            m_sampling_rate = preferredFormat.sampleRate();
            format.setSampleRate(m_sampling_rate);
            cout << "CaptureThread: WARNING: Qt: Using device preferred sampling rate " << m_sampling_rate << endl;
        }
    }

    if(old_sampling_rate != m_sampling_rate && old_sampling_rate != CaptureThread::SAMPLING_RATE_UNKNOWN && old_sampling_rate != CaptureThread::SAMPLING_RATE_MAX)
        m_capture_thread->emitSamplingRateChanged();

    qInfo().noquote() << "CaptureThread: Qt: Final format - rate:" << format.sampleRate()
                      << "channels:" << format.channelCount()
                      << "format:" << format.sampleFormat()
                      << "bytes/sample:" << format.bytesPerSample();

    if (format.isValid()) {
        if (m_format != format) {
            m_format = format;
            if(m_audioSource)
                delete m_audioSource;
            m_audioSource = new QAudioSource(m_audioInputDevice, m_format, this);
            if(m_audioSource->isNull()) {
                delete m_audioSource;
                m_audioSource = NULL;
                throw QString("Qt: Cannot create audio source (isNull)");
            }
            m_audioSource->setBufferSize(512 * 1024);
            connect(m_audioSource, &QAudioSource::stateChanged,
                            this, &CaptureThreadImplQt::audioStateChanged);
            m_audioInputIODevice = NULL;
        }
    } else {
        throw QString("Qt: cannot set parameters (format not valid)");
    }

}

void CaptureThreadImplQt::setSamplingRate(int value)
{
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
}

void CaptureThreadImplQt::audioStateChanged(QAudio::State state) {
    Q_UNUSED(state)
}

void CaptureThreadImplQt::audioDataReady()
{
    if(m_audioSource) {
        m_capture_thread->m_lock.lock();

        const qint64 bytesAvailable = m_audioSource->bytesAvailable();

        qint64 nbframes = bytesAvailable/(m_format.bytesPerSample());

        if(m_capture_thread->m_pause) {
            // Discard data when paused
            for(qint64 i=0; i<nbframes; i++) {
                long int value = 0;
                m_audioInputIODevice->read((char*)&value, m_format.bytesPerSample());
            }
        }
        else {

            for(qint64 i=0; i<nbframes; i++) {
                long int value = 0;
                qint64 bytesRead = m_audioInputIODevice->read((char*)&value, m_format.bytesPerSample());

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
	stopCapture();
    if (m_audioSource) {
        delete m_audioSource;
        m_audioSource = NULL;
    }
}

#endif

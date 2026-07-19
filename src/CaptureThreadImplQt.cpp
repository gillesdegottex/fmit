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
    qInfo().noquote() << "CaptureThread: Qt: capture_init()";

    set_params(false);

    if (m_audioSource) {
        if (QAudio::SuspendedState == m_state)
            m_audioSource->stop();

        m_audioInputIODevice = m_audioSource->start();
        if (m_audioInputIODevice) {
            connect(m_audioInputIODevice, &QIODevice::readyRead,
                            this, &CaptureThreadImplQt::audioDataReady);
            qInfo().noquote() << "CaptureThread: Qt: capture started, IODevice ready";
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
    qInfo().noquote() << "CaptureThread: Qt: capture_finished()";
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
    qInfo().noquote() << "CaptureThread: Qt: startCapture()";

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
    qInfo().noquote() << "CaptureThread: Qt: stopCapture()";

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
    qInfo().noquote() << "CaptureThread: Qt: set_params() source='" << getASCIISource().toLatin1().constData() << "'";

    if(getASCIISource()=="") {
        m_audioInputDevice = QMediaDevices::defaultAudioInput();
        qInfo().noquote() << "CaptureThread: Qt: Using default device:" << m_audioInputDevice.description();
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
    if(backendBroken) {
        cout << "CaptureThread: WARNING: Qt: isFormatSupported() unreliable, using device preferred format" << endl;
        format = preferredFormat;
        m_sampling_rate = preferredFormat.sampleRate();
    }
    else
    {
        // Backend is reliable, try our desired format
        format.setSampleFormat(QAudioFormat::Int16);
        format.setSampleRate(44100);
        format.setChannelCount(1);

        bool formatSupported = m_audioInputDevice.isFormatSupported(format);
        if(!formatSupported) {
            cout << "CaptureThread: WARNING: Qt: isFormatSupported() returned false for mono, trying stereo" << endl;
            format.setChannelCount(2);
            formatSupported = m_audioInputDevice.isFormatSupported(format);
            if(!formatSupported) {
                cout << "CaptureThread: WARNING: Qt: isFormatSupported() returned false for stereo, trying anyway with mono" << endl;
                format.setChannelCount(1);
            }
        }
    }

    bool isFloat = (format.sampleFormat() == QAudioFormat::Float);
    setFormatDescrsAndFns(format.bytesPerSample(), true, isFloat, format.channelCount());

    if(backendBroken)
    {
        // Backend is broken, use preferred format as-is
        cout << "CaptureThread: INFO: Qt: Using device preferred format (backend broken)" << endl;
        if(m_sampling_rate != preferredFormat.sampleRate()) {
            m_sampling_rate = preferredFormat.sampleRate();
            m_capture_thread->emitSamplingRateChanged();
        }
    }
    else if(m_sampling_rate==CaptureThread::SAMPLING_RATE_MAX || m_sampling_rate==CaptureThread::SAMPLING_RATE_UNKNOWN)
    {
        int old_sampling_rate = m_sampling_rate;

        cout << "CaptureThread: INFO: Qt: sampling rate set to max or undefined, try to determine it." << endl;

        int minRate = m_audioInputDevice.minimumSampleRate();
        int maxRate = m_audioInputDevice.maximumSampleRate();
        qInfo().noquote() << "CaptureThread: Qt: Device sample rate range:" << minRate << "-" << maxRate;

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

        if(old_sampling_rate!=m_sampling_rate)
            m_capture_thread->emitSamplingRateChanged();
    }
    else
    {
        format.setSampleRate(m_sampling_rate);
        if(!m_audioInputDevice.isFormatSupported(format)) {
            cout << "CaptureThread: WARNING: Qt: isFormatSupported() failed for rate " << m_sampling_rate
                 << ", falling back to device preferred rate " << preferredFormat.sampleRate() << endl;
            m_sampling_rate = preferredFormat.sampleRate();
            format.setSampleRate(m_sampling_rate);
        }
    }

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
            qInfo().noquote() << "CaptureThread: Qt: QAudioSource created successfully";
        }
    } else {
        throw QString("Qt: cannot set parameters (format not valid)");
    }

}

void CaptureThreadImplQt::setSamplingRate(int value)
{
    qInfo().noquote() << "CaptureThread: Qt: setSamplingRate(" << value << ")";

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
    qInfo().noquote() << "CaptureThread: Qt: audioStateChanged()";
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
    qInfo().noquote() << "CaptureThread: Qt: ~CaptureThreadImplQt()";

	stopCapture();
    if (m_audioSource) {
        delete m_audioSource;
        m_audioSource = NULL;
    }
}

#endif

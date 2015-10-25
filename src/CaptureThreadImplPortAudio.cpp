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
#include <unistd.h>
#include <iostream>
#include <vector>
using namespace std;

// ------------------------------ PortAudio implementation ----------------------------
#ifdef CAPTURE_PORTAUDIO
CaptureThreadImplPortAudio::CaptureThreadImplPortAudio(CaptureThread* capture_thread)
	: CaptureThreadImpl(capture_thread, "PortAudio", QString("Portable cross-platform Audio API (lib:")+Pa_GetVersionText()+"["+QString::number(Pa_GetVersion())+"])")
{
	m_stream = NULL;

	m_source = "default";
}

bool CaptureThreadImplPortAudio::is_available()
{
	if(!m_stream)
	{
		try
		{
			m_err = Pa_Initialize();
			if(m_err != paNoError)	throw QString("PortAudio: is_available:Pa_Initialize ")+Pa_GetErrorText(m_err);

			int numDevices;

			numDevices = Pa_GetDeviceCount();
			if(numDevices < 0)
				throw QString("PortAudio: is_available:Pa_GetDeviceCount ")+Pa_GetErrorText(numDevices);
			else if(numDevices == 0)
				throw QString("PortAudio: is_available:Pa_GetDeviceCount no devices available")+Pa_GetErrorText(numDevices);

/*			const   PaDeviceInfo *deviceInfo;

			for(int i=0; i<numDevices; i++ )
			{
				deviceInfo = Pa_GetDeviceInfo( i );
                cout << deviceInfo->name << endl;
                cout << deviceInfo->defaultSampleRate << endl;
			}*/
		}
		catch(QString error)
		{
			Pa_Terminate();
			m_stream = NULL;
			m_status = "N/A";
			return false;
		}
		capture_finished();
	}

	m_status = "OK";

	return true;
}

void CaptureThreadImplPortAudio::setSamplingRate(int value)
{
// 	cout << "CaptureThreadImplPortAudio::setSamplingRate " << value << endl;

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

			try{
				// it was just for testing
				capture_finished();
			}
			catch(QString error)
			{
                cout << "CaptureThread: ERROR: " << error.toStdString() << endl;
				m_capture_thread->emitError(error);
			}
		}
		else
			m_capture_thread->emitSamplingRateChanged();

		if(was_running)	m_capture_thread->startCapture();
	}

// 	cout << "~CaptureThreadImplPortAudio::setSamplingRate" << endl;
}

int CaptureThreadImplPortAudio::PortAudioCallback( const void *inputBuffer, void *outputBuffer,
								  unsigned long framesPerBuffer,
								  const PaStreamCallbackTimeInfo* timeInfo,
								  PaStreamCallbackFlags statusFlags,
								  void *userData )
{return ((CaptureThreadImplPortAudio*)userData)->portAudioCallback(inputBuffer, framesPerBuffer, timeInfo, statusFlags);}
int CaptureThreadImplPortAudio::portAudioCallback(const void *inputBuffer,
						unsigned long framesPerBuffer,
						const PaStreamCallbackTimeInfo* timeInfo,
						PaStreamCallbackFlags statusFlags)
{
	if(m_capture_thread->m_pause)
		return 0;

	m_capture_thread->m_lock.lock();

	float *in = (float*)inputBuffer;

	for(unsigned long i=0; i<framesPerBuffer; i++)
		m_capture_thread->m_values.push_front(*in++);
//	addValue(*in++, i, m_channel_count);				// TODO

	m_capture_thread->m_packet_size = framesPerBuffer;
	if(m_capture_thread->m_ext_lock)
	{
		m_capture_thread->m_packet_size_sll = 0;
		m_capture_thread->m_ext_lock = false;
	}
	m_capture_thread->m_packet_size_sll += framesPerBuffer;

	m_capture_thread->m_lock.unlock();

	return 0;
}

void CaptureThreadImplPortAudio::set_params(bool test)
{
	m_err = Pa_Initialize();
	if(m_err != paNoError)	throw QString("PortAudio: set_params:Pa_Initialize ")+Pa_GetErrorText(m_err);

	PaStreamParameters params;
	params.device = paNoDevice;
	params.channelCount = 1;
	params.sampleFormat = paFloat32;
	params.suggestedLatency = 0;
	params.hostApiSpecificStreamInfo = NULL;

    if(getASCIISource()!="default")
	{
		int	numDevices = Pa_GetDeviceCount();
		const PaDeviceInfo* deviceInfo;
		for(int i=0; params.device==paNoDevice && i<numDevices; i++)
		{
			deviceInfo = Pa_GetDeviceInfo(i);
            if(QString(deviceInfo->name)==getASCIISource())
				params.device = i;
		}

		if(params.device==paNoDevice)
            cout << "CaptureThread: INFO: PortAudio: cannot determine selected source \"" << getASCIISource().toStdString() << "\"" << endl;
	}

	if(!test)
	{
		if(params.device==paNoDevice)
            cout << "CaptureThread: INFO: PortAudio: using default device" << endl;
		else
            cout << "CaptureThread: INFO: PortAudio: using \"" << getASCIISource().toStdString() << "\"" << endl;

		setFormatDescrsAndFns(4, true, true, 1); // TODO do something if nbchannel=1 not supported
	}

	if(m_sampling_rate==CaptureThread::SAMPLING_RATE_MAX || m_sampling_rate==CaptureThread::SAMPLING_RATE_UNKNOWN)
	{
		int old_sampling_rate = m_sampling_rate;

        cout << "CaptureThread: INFO: PortAudio: sampling rate set to max or undefined, try to determinate it." << endl;

		list<int> sampling_rates;
		sampling_rates.push_front(8000);	sampling_rates.push_front(11025);	sampling_rates.push_front(16000);
		sampling_rates.push_front(22050);	sampling_rates.push_front(24000);	sampling_rates.push_front(32000);
		sampling_rates.push_front(44100);

		m_err = -1;
		while(m_err!=paNoError)
		{
			if(sampling_rates.empty())
				throw QString("PortAudio: cannot set any sample rate (")+Pa_GetErrorText(m_err)+")";

			m_err = Pa_Initialize();
			if(m_err != paNoError)	throw QString("PortAudio: set_params:Pa_Initialize ")+Pa_GetErrorText(m_err);

			m_sampling_rate = sampling_rates.front();
            cout << "CaptureThread: INFO: PortAudio: try sampling rate " << m_sampling_rate << " ..." << flush;

// 			cout << "nbc1 " << params.channelCount << endl;

			if(params.device==paNoDevice)
				m_err = Pa_OpenDefaultStream(&m_stream, 1, 0, paFloat32, m_sampling_rate, 0, PortAudioCallback, this);
			else
				m_err = Pa_OpenStream(&m_stream, &params, NULL, m_sampling_rate, 0, paNoFlag, PortAudioCallback, this);

            if(m_err != paNoError)	cout << " failed" << endl;
            else					cout << " success" << endl;

			sampling_rates.pop_front();
		}

		if(old_sampling_rate!=m_sampling_rate)
			m_capture_thread->emitSamplingRateChanged();
	}
	else
	{
// 		cout << "nbc2 " << params.channelCount << endl;
// 		cout << "dev2 " << params.device << "/" << paNoDevice << endl;

		if(params.device==paNoDevice)
		{
			m_err = Pa_OpenDefaultStream(&m_stream, 1, 0, paFloat32, m_sampling_rate, 0, PortAudioCallback, this);
			if(m_err != paNoError)
				throw QString("PortAudio: set_params:Pa_OpenDefaultStream ")+Pa_GetErrorText(m_err);
		}
		else
		{
			m_err = Pa_OpenStream(&m_stream, &params, NULL, m_sampling_rate, 0, paNoFlag, PortAudioCallback, this);
			if(m_err != paNoError)
				throw QString("PortAudio: set_params:Pa_OpenStream ")+Pa_GetErrorText(m_err);
		}
	}
}

void CaptureThreadImplPortAudio::capture_init()
{
	set_params(false);

	m_err = Pa_StartStream(m_stream);
	if(m_err != paNoError)
		throw QString("PortAudio: capture_init:Pa_StartStream ")+Pa_GetErrorText(m_err);

	m_capture_thread->m_capturing = true;
	m_capture_thread->emitCaptureStarted();
	m_capture_thread->emitCaptureToggled(true);
}
void CaptureThreadImplPortAudio::capture_finished()
{
	if(m_stream)
	{
		if(!Pa_IsStreamStopped(m_stream))
		{
			m_err = Pa_StopStream(m_stream);
			if(m_err != paNoError)	throw QString("PortAudio: capture_finished: ")+Pa_GetErrorText(m_err);
		}

		if(m_stream)
		{
			m_err = Pa_CloseStream(m_stream);
			if(m_err != paNoError)	throw QString("PortAudio: capture_finished: ")+Pa_GetErrorText(m_err);
		}

		m_stream = NULL;

		m_capture_thread->m_capturing = false;
		m_capture_thread->emitCaptureStoped();
		m_capture_thread->emitCaptureToggled(false);
	}

	m_err = Pa_Terminate();
// 	if(m_err != paNoError)	throw QString("PortAudio: capture_finished: ")+Pa_GetErrorText(m_err);
}
void CaptureThreadImplPortAudio::startCapture()
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
void CaptureThreadImplPortAudio::stopCapture()
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
CaptureThreadImplPortAudio::~CaptureThreadImplPortAudio()
{
	stopCapture();
}

#endif

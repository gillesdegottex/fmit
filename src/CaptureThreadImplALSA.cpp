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
#include <fstream>
#include <vector>
using namespace std;

// ------------------------------ ALSA implementation ----------------------------
#ifdef CAPTURE_ALSA

#define ALSA_BUFF_SIZE 1024

void alsa_error_handler(const char *file, int line, const char *function, int err, const char *fmt, ...)
{
    cout << "alsa_error_handler: " << file << ":" << line << " " << function << " err=" << err << endl;
}

CaptureThreadImplALSA::CaptureThreadImplALSA(CaptureThread* capture_thread)
	: CaptureThreadImpl(capture_thread, "ALSA", QString("Advanced Linux Sound Architecture (lib:")+snd_asoundlib_version()+")")
{
	m_alsa_capture_handle = NULL;
	m_alsa_hw_params = NULL;
	m_alsa_buffer = NULL;
	m_format = SND_PCM_FORMAT_UNKNOWN;

	m_source = "default"; // was hw:0

	m_alive = true;
	m_in_run = false;
	m_loop = false;

// 	snd_lib_error_set_handler(alsa_error_handler);
}

bool CaptureThreadImplALSA::is_available()
{
	if(m_alsa_capture_handle==NULL)
	{
		try
		{
			int err = -1;
            if((err=snd_pcm_open(&m_alsa_capture_handle, getASCIISource().toLatin1().constData(), SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK)) < 0)
			{
				if(err==-19)	// TODO risks of changes for the error code
                    throw QString("invalid source '")+getASCIISource()+"'";
				else if(err==-16)
                    throw QString("device '")+getASCIISource()+"' busy";
				else
					throw QString("cannot open pcm: ")+QString(snd_strerror(err));
			}
		}
		catch(QString error)
		{
			m_alsa_capture_handle = NULL;

			m_status = "N/A ("+error+")";

			return false;
		}

		if(m_alsa_capture_handle!=NULL)
		{
			snd_pcm_close(m_alsa_capture_handle);
			m_alsa_capture_handle = NULL;
		}
	}

	m_status = "OK";

    //	cout << "CaptureThread: INFO: ALSA seems available" << endl;

	return true;
}

void CaptureThreadImplALSA::startCapture()
{
	if(!isRunning())
		start();

	m_loop = true;

	m_wait_for_start = true;
	while(m_wait_for_start)	// some implementations take a long time to start
		msleep(10);
}
void CaptureThreadImplALSA::stopCapture()
{
	m_loop = false;

	while(m_in_run)
		msleep(10);
}

void CaptureThreadImplALSA::set_params(bool test)
{
//	cout << "ALSA: Recognized sample formats are" << endl;
//	for (int k = 0; k < SND_PCM_FORMAT_LAST; ++(unsigned long) k) {
//		const char *s = snd_pcm_format_name((snd_pcm_format_t)k);
//		if (s)	cout << s << endl;
//	}
	int err=0;

	if(m_source=="")
		throw QString("ALSA: set the source first");
    if((err=snd_pcm_open(&m_alsa_capture_handle, getASCIISource().toLatin1().constData(), SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK)) < 0)
	{
        //					cout << "err=" << err << ":" << snd_strerror(err) << endl;

		if(err==-19)	// TODO risks of changes for the error code
            throw QString("ALSA: Invalid Source '")+getASCIISource()+"'";
		else if(err==-16)
            throw QString("ALSA: Device '")+getASCIISource()+"' busy";
		else
			throw QString("ALSA: Cannot open pcm: ")+QString(snd_strerror(err));
	}

	snd_pcm_hw_params_alloca(&m_alsa_hw_params);

	if((err=snd_pcm_hw_params_any(m_alsa_capture_handle, m_alsa_hw_params)) < 0)
		throw QString("ALSA: cannot initialize hardware parameter structure (")+QString(snd_strerror(err))+")";

	if((err=snd_pcm_hw_params_set_access(m_alsa_capture_handle, m_alsa_hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
		throw QString("ALSA: cannot set access type (")+QString(snd_strerror(err))+")";

	if(!test)
	{
		// Formats
        list<snd_pcm_format_t> formats;
        // Not a good idea to try 24b, since it is not always properly supported
        // formats.push_back(SND_PCM_FORMAT_S24);
        formats.push_back(SND_PCM_FORMAT_S16);	formats.push_back(SND_PCM_FORMAT_U16);
        formats.push_back(SND_PCM_FORMAT_S8);	formats.push_back(SND_PCM_FORMAT_U8);

        err = -1;
        while(err<0)
        {
            if(formats.empty())
                throw QString("ALSA: cannot set any format (")+QString(snd_strerror(err))+")";

            m_format = formats.front();
            cout << "CaptureThread: INFO: ALSA: try to set format to " << snd_pcm_format_description(m_format) << flush;
            err=snd_pcm_hw_params_set_format(m_alsa_capture_handle, m_alsa_hw_params, m_format);

            if(err<0)	cout << " failed" << endl;
            else		cout << " success" << endl;

            formats.pop_front();
        }

		// Channel count
		unsigned int channel_count = 1;
		if((err=snd_pcm_hw_params_set_channels_near(m_alsa_capture_handle, m_alsa_hw_params, &channel_count)) < 0)
		{
			QString err_msg = QString("ALSA: cannot set channel count (")+QString(snd_strerror(err))+")";
            cout << "CaptureThread: WARNING: " << err_msg.toStdString() << endl;
		}
		if(channel_count>1)
		{
			QString err_msg = QString("ALSA: cannot set channel count to one (")+QString::number(channel_count)+" instead)";
            cout << "CaptureThread: WARNING: " << err_msg.toStdString() << endl;
		}

		setFormatDescrsAndFns(snd_pcm_format_width(m_format)/8, snd_pcm_format_signed(m_format), false, channel_count);
	}

	if(m_sampling_rate==CaptureThread::SAMPLING_RATE_MAX || m_sampling_rate==CaptureThread::SAMPLING_RATE_UNKNOWN)
	{
		int old_sampling_rate = m_sampling_rate;

        cout << "CaptureThread: INFO: ALSA: sampling rate set to max or undefined, try to determinate it." << endl;

		list<int> sampling_rates;
		sampling_rates.push_front(8000);	sampling_rates.push_front(11025);	sampling_rates.push_front(16000);
		sampling_rates.push_front(22050);	sampling_rates.push_front(24000);	sampling_rates.push_front(32000);
		sampling_rates.push_front(44100);

		err = -1;
		while(err<0)
		{
			if(sampling_rates.empty())
				throw QString("ALSA: cannot set any sample rate (")+QString(snd_strerror(err))+")";

			m_sampling_rate = sampling_rates.front();
            cout << "CaptureThread: INFO: ALSA: try sampling rate " << m_sampling_rate << " ..." << flush;
			unsigned int rrate = m_sampling_rate;
			err = snd_pcm_hw_params_set_rate(m_alsa_capture_handle, m_alsa_hw_params, rrate, 0);

            if(err<0)	cout << " failed" << endl;
            else		cout << " success" << endl;

			sampling_rates.pop_front();
		}

		if(old_sampling_rate!=m_sampling_rate)
			m_capture_thread->emitSamplingRateChanged();
	}
	else
	{
		int err;
		int dir = 0;
		unsigned int rrate = m_sampling_rate;
		if((err = snd_pcm_hw_params_set_rate_near(m_alsa_capture_handle, m_alsa_hw_params, &rrate, &dir))<0)
			throw QString("ALSA: cannot set sampling rate (")+QString(snd_strerror(err))+")";
        if(m_sampling_rate!=int(rrate))
			m_capture_thread->emitSamplingRateChanged();
		m_sampling_rate = rrate;
	}

	if((err=snd_pcm_hw_params(m_alsa_capture_handle, m_alsa_hw_params)) < 0)
		throw QString("ALSA: cannot set parameters (")+QString(snd_strerror(err))+")";
}

void CaptureThreadImplALSA::setSamplingRate(int value)
{
// 	cout << "CaptureThreadImplALSA::setSamplingRate " << value << endl;

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

// 	cout << "~CaptureThreadImplALSA::setSamplingRate" << endl;
}

void CaptureThreadImplALSA::capture_init()
{
	set_params();

	snd_pcm_nonblock(m_alsa_capture_handle, 0);

	m_alsa_buffer = new char[m_channel_count*ALSA_BUFF_SIZE*snd_pcm_format_width(m_format)/8];

	int err=0;

	if((err=snd_pcm_prepare(m_alsa_capture_handle)) < 0)
		throw QString("ALSA: cannot prepare audio interface for use (")+QString(snd_strerror(err))+")";
}
void CaptureThreadImplALSA::capture_loop()
{
// 	cout << "CaptureThreadImplALSA::capture_loop" << endl;

	m_wait_for_start = false;
	while(m_loop)
	{
		int ret_val = snd_pcm_readi(m_alsa_capture_handle, m_alsa_buffer, ALSA_BUFF_SIZE);
        if (ret_val == -ESTRPIPE)
            continue;

		if(ret_val<0)
		{
            cout << "CaptureThread: WARNING: ALSA: " << snd_strerror(ret_val) << endl;
			while((ret_val = snd_pcm_prepare(m_alsa_capture_handle)) < 0)
			{
                if (ret_val == -ESTRPIPE)
                    continue;

				msleep(1000);
                cout << "ALSA: cannot prepare audio interface (" << snd_strerror(ret_val) << ")" << endl;
//				throw QString("ALSA: cannot prepare audio interface (")+QString(snd_strerror(ret_val))+")";
			}
		}
		else
		{
			if(!m_capture_thread->m_pause)
			{
				m_capture_thread->m_lock.lock();

// 				cout << "CaptureThreadImplALSA::capture_loop " << m_capture_thread->m_values.size() << endl;

				for(int i=0; i<ret_val*m_channel_count; i++)
					addValue(this, decodeValue(m_alsa_buffer, i), i);

				m_capture_thread->m_packet_size = ret_val;
				if(m_capture_thread->m_ext_lock)
				{
					m_capture_thread->m_packet_size_sll = 0;
					m_capture_thread->m_ext_lock = false;
				}
				m_capture_thread->m_packet_size_sll += ret_val;

				m_capture_thread->m_lock.unlock();
			}
		}
	}

// 	cout << "~CaptureThreadImplALSA::capture_loop" << endl;
}
void CaptureThreadImplALSA::capture_finished()
{
	if(m_alsa_buffer!=NULL)
	{
		delete[] m_alsa_buffer;
		m_alsa_buffer = NULL;
	}

	if(m_alsa_capture_handle!=NULL)
	{
		snd_pcm_hw_free(m_alsa_capture_handle);
		snd_pcm_close(m_alsa_capture_handle);
		m_alsa_capture_handle = NULL;
	}
}

void CaptureThreadImplALSA::run()
{
// 	cout << "CaptureThread: INFO: ALSA: capture thread entered" << endl;

// 	while(m_alive)	// TODO need to keep alsa thread alive to let PortAudio working after ALSA !!
	{
		while(m_alive && !m_loop)
			msleep(10);

		m_in_run = true;

		try
		{
            //			cout << "CaptureThread: INFO: capture thread running" << endl;

			capture_init();

			m_capture_thread->m_capturing = true;
			m_capture_thread->emitCaptureStarted();
			m_capture_thread->emitCaptureToggled(true);

			capture_loop();

			m_capture_thread->m_capturing = false;
			m_capture_thread->emitCaptureStoped();
			m_capture_thread->emitCaptureToggled(false);
		}
		catch(QString error)
		{
			m_loop = false;
            cout << "CaptureThread: ERROR: " << error.toStdString() << endl;
			m_capture_thread->emitError(error);
		}
		m_wait_for_start = false;

		capture_finished();

		m_in_run = false;

        //		cout << "CaptureThread: INFO: capture thread stop running" << endl;
	}

// 	cout << "CaptureThread: INFO: ALSA: capture thread exited" << endl;
}

CaptureThreadImplALSA::~CaptureThreadImplALSA()
{
// 	cout << "CaptureThreadImplALSA::~CaptureThreadImplALSA" << endl;

	m_alive = false;

	stopCapture();

	while(isRunning())
		msleep(10);

// 	cout << "~CaptureThreadImplALSA::~CaptureThreadImplALSA" << endl;
}

#endif

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

// ------------------------------ OSS implementation ----------------------------
#ifdef CAPTURE_OSS
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

#define OSS_BUFF_SIZE 1024

CaptureThreadImplOSS::CaptureThreadImplOSS(CaptureThread* capture_thread)
	: CaptureThreadImpl(capture_thread, "OSS", QString("Open Sound System"))
{
	m_fd_in = 0;
	m_oss_buffer = NULL;
	m_format = -1;

	m_source = "/dev/dsp";

	// 	oss_sysinfo si;	// definition ??
// 	ioctl(m_fd_in, OSS_SYSINFO, &si);
// 	m_fd_in = open(m_source, O_RDONLY, 0));
// 	m_descr = QString("Open Sound System (")+QString::number(SOUND_VERSION, 16)+":"+si.version+")");
// 	close(m_fd_in);
// 	m_fd_in = 0;
	m_descr = QString("Open Sound System (lib:")+QString::number(SOUND_VERSION, 16)+")";

	m_alive = true;
	m_in_run = false;
	m_loop = false;
}

bool CaptureThreadImplOSS::is_available()
{
	if(m_fd_in==0)
	{
		try
		{
            if((m_fd_in = open (m_source.toLatin1().constData(), O_RDONLY, 0)) == -1)
				throw QString(strerror(errno));
		}
		catch(QString error)
		{
			m_fd_in = 0;

			m_status = "N/A ("+error+")";

			return false;
		}

		capture_finished();
	}

	m_status = "OK";

    //	cout << "CaptureThread: INFO: OSS seems available" << endl;

	return true;
}

void CaptureThreadImplOSS::startCapture()
{
	if(!isRunning())
		start();

	m_loop = true;

	m_wait_for_start = true;
	while(m_wait_for_start)	// some implementations take a long time to start
		msleep(10);
}
void CaptureThreadImplOSS::stopCapture()
{
	m_loop = false;

	while(m_in_run)
		msleep(10);
}

void CaptureThreadImplOSS::set_params(bool test)
{
	if(m_source=="")
		throw QString("OSS: set the source first");
    if((m_fd_in = open (m_source.toLatin1().constData(), O_RDONLY, 0))==-1)
		throw QString("OSS: ")+QString(strerror(errno));

	if(!test)
	{
		if(m_format==-1)
		{
			// Formats
			m_format = AFMT_S16_NE; /* Native 16 bits */
			if(ioctl(m_fd_in, SNDCTL_DSP_SETFMT, &m_format)==-1)
				throw QString("OSS: cannot set format (%1)").arg(strerror(errno));

			if(m_format != AFMT_S16_NE)
				throw QString("OSS: cannot set format to signed 16bits");
		}
		else
		{
			if(ioctl(m_fd_in, SNDCTL_DSP_SETFMT, &m_format)==-1)
				throw QString("OSS: cannot set format (%1)").arg(strerror(errno));
		}

		m_format_size = 2;
		m_format_signed = true;
		m_format_float = false;

		// Channel count
		unsigned int channel_count = 1;
		if(ioctl(m_fd_in, SNDCTL_DSP_CHANNELS, &channel_count)==-1)
			throw QString("OSS: cannot set channel count to 1 (%1)").arg(strerror(errno));

		if(channel_count != 1)
			throw QString("OSS: the device doesn't support mono mode");

		/*		if(m_channel_count>1)	// TODO
		{
			QString err_msg = QString("OSS: cannot set channel count to one (")+QString::number(m_channel_count)+" instead)";
            cout << "CaptureThread: WARNING: " << err_msg << endl;
		}*/

		setFormatDescrsAndFns(2, true, false, channel_count);
	}

	if(m_sampling_rate==CaptureThread::SAMPLING_RATE_MAX || m_sampling_rate==CaptureThread::SAMPLING_RATE_UNKNOWN)
	{
		int old_sampling_rate = m_sampling_rate;

        cout << "CaptureThread: INFO: OSS: sampling rate set to max or undefined, try to determinate it." << endl;

		list<int> sampling_rates;
		sampling_rates.push_front(8000);	sampling_rates.push_front(11025);	sampling_rates.push_front(16000);
		sampling_rates.push_front(22050);	sampling_rates.push_front(24000);	sampling_rates.push_front(32000);
		sampling_rates.push_front(44100);

		int err = -1;
		while(err<0)
		{
			if(sampling_rates.empty())
				throw QString("OSS: cannot set any sample rate (%1)").arg(strerror(errno));

			m_sampling_rate = sampling_rates.front();
            cout << "CaptureThread: INFO: OSS: try sampling rate " << m_sampling_rate << " ..." << flush;
			err = ioctl(m_fd_in, SNDCTL_DSP_SPEED, &m_sampling_rate);

            if(err==-1)	cout << " failed" << endl;
            else		cout << " success" << endl;

			sampling_rates.pop_front();
		}

		if(old_sampling_rate!=m_sampling_rate)
			m_capture_thread->emitSamplingRateChanged();
	}
	else
	{
		if(ioctl(m_fd_in, SNDCTL_DSP_SPEED, &m_sampling_rate)==-1)
			throw QString("OSS: cannot set sampling rate (")+QString(strerror(errno))+")";
	}
}

void CaptureThreadImplOSS::setSamplingRate(int value)
{
// 	cout << "CaptureThreadImplOSS::setSamplingRate " << value << endl;

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

// 	cout << "~CaptureThreadImplOSS::setSamplingRate" << endl;
}

void CaptureThreadImplOSS::capture_init()
{
	set_params(false);

	m_oss_buffer = new char[m_channel_count*OSS_BUFF_SIZE*16/8];
}
void CaptureThreadImplOSS::capture_loop()
{
// 	cout << "CaptureThreadImplOSS::capture_loop" << endl;

	bool format_signed = true;
	int l=0;

	m_wait_for_start = false;
	while(m_loop)
	{
		int ret_val = read(m_fd_in, m_oss_buffer, sizeof(m_oss_buffer));

		if(ret_val==-1)
		{
            cout << "CaptureThread: WARNING: OSS: " << strerror(errno) << endl;
			msleep(1000);	// TODO which behavior ?
//  			m_loop = false;// TODO which behavior ?
		}
		else
		{
			ret_val /= m_format_size;

			if(!m_capture_thread->m_pause)
			{
				m_capture_thread->m_lock.lock();

				for(int i=0; i<ret_val*m_channel_count; i++)
					addValue(this, decodeValue(m_oss_buffer, i), i);

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

// 	cout << "~CaptureThreadImplOSS::capture_loop" << endl;
}
void CaptureThreadImplOSS::capture_finished()
{
	if(m_oss_buffer!=NULL)
	{
		delete[] m_oss_buffer;
		m_oss_buffer = NULL;
	}

	if(m_fd_in!=0)
	{
		close(m_fd_in);
		m_fd_in = 0;
	}
}

void CaptureThreadImplOSS::run()
{
// 	cout << "CaptureThread: INFO: OSS: capture thread entered" << endl;

// 	while(m_alive)	// TODO ?? need to keep oss thread alive to let PortAudio working after ALSA ??
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

// 	cout << "CaptureThread: INFO: OSS: capture thread exited" << endl;
}

CaptureThreadImplOSS::~CaptureThreadImplOSS()
{
// 	cout << "CaptureThreadImplOSS::~CaptureThreadImplOSS" << endl;

	m_alive = false;

	stopCapture();

	while(isRunning())
		msleep(10);

// 	cout << "~CaptureThreadImplOSS::~CaptureThreadImplOSS" << endl;
}

#endif

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

// ------------------------------ JACK implementation ----------------------------
#ifdef CAPTURE_JACK

#define JACK_BUFF_SIZE 1024

const size_t g_jack_sample_size = sizeof(jack_default_audio_sample_t);

CaptureThreadImplJACK::CaptureThreadImplJACK(CaptureThread* capture_thread)
: CaptureThreadImpl(capture_thread, "JACK", "Jack Audio Connection Kit")
{
	m_jack_client = NULL;
	m_jack_port = NULL;
    m_ringbuffer = NULL;

    m_alive = true;
    m_in_run = false;
    m_loop = false;
}

bool CaptureThreadImplJACK::is_available()
{
	if(m_jack_client==NULL)
	{
		try
		{
			//m_jack_client = jack_client_new((m_capture_thread->m_name+"_test").latin1());
            m_jack_client = jack_client_open((m_capture_thread->m_name+"_test").toLatin1().constData(), JackNoStartServer, NULL);
            if(m_jack_client==NULL)
				throw QString("unknown reason");
		}
		catch(QString error)
		{
			m_jack_client = NULL;
			m_status = "N/A";
			return false;
		}
		capture_finished();
	}

	m_status = "available";

	return true;
}

void CaptureThreadImplJACK::setSamplingRate(int value)
{
    (void)value;
    cout << "CaptureThread: ERROR: JACK: setSamplingRate not available with JACK ! change the JACK server sampling rate instead" << endl;
}

void CaptureThreadImplJACK::startCapture()
{
    if(!isRunning())
        start();

    m_loop = true;

    m_wait_for_start = true;
    while(m_wait_for_start) // some implementations take a long time to start
        msleep(50);
}
void CaptureThreadImplJACK::stopCapture()
{
    m_loop = false;

    while(m_in_run)
        msleep(50);
}

void CaptureThreadImplJACK::JackShutdown(void* arg){((CaptureThreadImplJACK*)arg)->jackShutdown();}
void CaptureThreadImplJACK::jackShutdown()
{
//     cout << "CaptureThreadImplJACK::jackShutdown" << endl;

	m_jack_client = NULL;
    m_jack_port = NULL;
    if(m_ringbuffer)
    {
        jack_ringbuffer_free(m_ringbuffer);
        m_ringbuffer = NULL;
    }

	m_capture_thread->emitError("JACK: server shutdown !");
//     cout << "~CaptureThreadImplJACK::jackShutdown" << endl;
}

int CaptureThreadImplJACK::JackSampleRate(jack_nframes_t nframes, void* arg){return ((CaptureThreadImplJACK*)arg)->jackSampleRate(nframes);}
int CaptureThreadImplJACK::jackSampleRate(jack_nframes_t nframes)
{
	if(m_sampling_rate!=int(nframes))
	{
        if(m_ringbuffer)
            jack_ringbuffer_free(m_ringbuffer);
        m_ringbuffer = jack_ringbuffer_create(g_jack_sample_size*nframes); // one second buffer

		m_sampling_rate = nframes;
		m_capture_thread->emitSamplingRateChanged();
	}

	return 0;
}

int CaptureThreadImplJACK::JackProcess(jack_nframes_t nframes, void* arg){return ((CaptureThreadImplJACK*)arg)->jackProcess(nframes);}
int CaptureThreadImplJACK::jackProcess(jack_nframes_t nframes)
{
// 	cout << "CaptureThreadImplJACK::jackProcess '" << nframes << "'" << endl;

    if(!m_jack_client || !m_jack_port || !m_ringbuffer) return 0;
	if(m_capture_thread->m_pause || !m_capture_thread->m_capturing || nframes<=0)	return 0;

	void* pin = jack_port_get_buffer(m_jack_port, nframes);

	if(!pin) return 0;

	jack_default_audio_sample_t* in = (jack_default_audio_sample_t*)pin;

    int characters_written = jack_ringbuffer_write(m_ringbuffer, (const char *) ((void *) (in)), g_jack_sample_size*nframes);
    if (characters_written < int(g_jack_sample_size*nframes))
        cout << "CaptureThreadImplJACK::jackProcess Can not write all frames: ringbuffer full?" << endl;

//     int toread = jack_ringbuffer_read_space(m_ringbuffer);
//     cout << "CaptureThreadImplJACK::jackProcess " << characters_written/g_jack_sample_size << " values written, " << toread/g_jack_sample_size << " to read" << endl;

//     cout << "~CaptureThreadImplJACK::jackProcess" << endl;

	return 0;
}

void CaptureThreadImplJACK::capture_init()
{
    m_jack_client = jack_client_open(m_capture_thread->m_name.toLatin1().constData(), JackNoStartServer, NULL);
	if(!m_jack_client)
		throw QString("JACK: cannot create client, JACK deamon is running ?");

	jack_set_process_callback(m_jack_client, JackProcess, (void*)this);
	jack_on_shutdown(m_jack_client, JackShutdown, (void*)this);
	jack_set_sample_rate_callback(m_jack_client, JackSampleRate, (void*)this);

    cout << "CaptureThread: INFO: JACK: sampling rate=" << jack_get_sample_rate(m_jack_client) << endl;
    m_ringbuffer = jack_ringbuffer_create(g_jack_sample_size*jack_get_sample_rate(m_jack_client)); // one second buffer

    int err=0;
	if((err=jack_activate(m_jack_client))!=0)
		throw QString("JACK: cannot activate client");

	setFormatDescrsAndFns(g_jack_sample_size, true, true, 1);

	m_jack_port = jack_port_register(m_jack_client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput,0);

    if(getASCIISource()!="")
    {
        QString dest = QString(jack_get_client_name(m_jack_client))+":input";
        cout << "CaptureThread: INFO: JACK: auto-connect '" << getASCIISource().toStdString() << "' with '" << dest.toStdString() << "'" << endl;
        if((err=jack_connect(m_jack_client, getASCIISource().toLatin1().constData(), dest.toLatin1().constData()))!=0)
            m_capture_thread->emitError(QString("JACK: Invalid source '")+getASCIISource()+"'");
    }

	int old_sampling_rate = m_sampling_rate;
	m_sampling_rate = jack_get_sample_rate(m_jack_client);
	if(m_sampling_rate!=old_sampling_rate)
		m_capture_thread->emitSamplingRateChanged();

	m_capture_thread->m_capturing = true;
	m_capture_thread->emitCaptureStarted();
	m_capture_thread->emitCaptureToggled(true);
}
void CaptureThreadImplJACK::capture_loop()
{
//     cout << "CaptureThreadImplJACK::capture_loop " << g_jack_sample_size << endl;

    char dest[JACK_BUFF_SIZE*g_jack_sample_size];

    m_wait_for_start = false;
    while(m_loop && m_jack_client && m_jack_port && m_ringbuffer)
    {
//         cout << "CaptureThreadImplJACK::capture_loop toread=" << toread/g_jack_sample_size << endl;
        int read = jack_ringbuffer_read(m_ringbuffer, dest, JACK_BUFF_SIZE*g_jack_sample_size);
        if(read>0)
        {
            if(!m_capture_thread->m_pause)
            {
//                 cout << "CaptureThreadImplJACK::capture_loop locking" << endl;
                m_capture_thread->m_lock.lock();

//                 int toread = jack_ringbuffer_read_space(m_ringbuffer);
//                 cout << "CaptureThreadImplJACK::capture_loop in deque='" << m_capture_thread->m_values.size() << "' still toread=" << toread/g_jack_sample_size << endl;

                if(g_jack_sample_size==4)
                    for(int i=0; i<int(read/g_jack_sample_size); i++)
                        addValue(this, (double)(((float*)dest)[i]), i);
                else if(g_jack_sample_size==8)
                    for(int i=0; i<int(read/g_jack_sample_size); i++)
                        addValue(this, (double)(((double*)dest)[i]), i);

                m_capture_thread->m_packet_size = (read/g_jack_sample_size);
                if(m_capture_thread->m_ext_lock)
                {
                    m_capture_thread->m_packet_size_sll = 0;
                    m_capture_thread->m_ext_lock = false;
                }
                m_capture_thread->m_packet_size_sll += (read/g_jack_sample_size);

                m_capture_thread->m_lock.unlock();
//                 cout << "CaptureThreadImplJACK::capture_loop unlocked" << endl;
            }
        }
        else
            msleep(10);
    }

//     cout << "~CaptureThreadImplJACK::capture_loop" << endl;
}
void CaptureThreadImplJACK::capture_finished()
{
// 	cout << "CaptureThreadImplJACK::capture_finished" << endl;

	if(m_jack_client!=NULL)
	{
		jack_client_close(m_jack_client);
		m_jack_client = NULL;
        if(m_ringbuffer)
        {
            jack_ringbuffer_free(m_ringbuffer);
            m_ringbuffer = NULL;
        }

		m_capture_thread->m_capturing = false;
		m_capture_thread->emitCaptureStoped();
		m_capture_thread->emitCaptureToggled(false);
	}
}
void CaptureThreadImplJACK::run()
{
//  cout << "CaptureThread: INFO: JACK: capture thread entered" << endl;

//  while(m_alive)  // TODO need to keep alsa thread alive to let PortAudio working after ALSA !! TODO same for JACK ???
    {
        while(m_alive && !m_loop)
            msleep(50);

        m_in_run = true;

        try
        {
//             cout << "CaptureThread: INFO: JACK: capture thread running" << endl;

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

//         cout << "CaptureThread: INFO: JACK: capture thread stop running" << endl;
    }

//     cout << "CaptureThread: INFO: JACK: capture thread exited" << endl;
}
CaptureThreadImplJACK::~CaptureThreadImplJACK()
{
//  cout << "CaptureThreadImplJACK::~CaptureThreadImplJACK" << endl;

    m_alive = false;

    stopCapture();

    while(isRunning())
        msleep(50);

//  cout << "~CaptureThreadImplJACK::~CaptureThreadImplJACK" << endl;
}
#endif

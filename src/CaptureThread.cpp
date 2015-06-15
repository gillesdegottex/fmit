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
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
#include <qdatetime.h>

double DecodeUnsigned8Bits(void* buffer, int i)	{return 2*(((unsigned char*)buffer)[i])/256.0 - 1;}
double DecodeSigned8Bits(void* buffer, int i)	{return (((signed char*)buffer)[i])/128.0;}
double DecodeUnsigned16Bits(void* buffer, int i){return 2*((unsigned short*)buffer)[i]/65536.0 - 1;}
double DecodeSigned16Bits(void* buffer, int i)	{return ((signed short*)buffer)[i]/32768.0;}
void AddValue2ChannelFirst(CaptureThreadImpl* impl, double value, int i)
{
	if(i%2==0)
		impl->m_capture_thread->m_values.push_front(value);
}
void AddValue2ChannelMix(CaptureThreadImpl* impl, double value, int i)
{
	if(i%2==0)
        impl->m_tmp_value = value;
// 		impl->m_capture_thread->m_values.push_front(value);
	else
        impl->m_capture_thread->m_values.push_front((impl->m_tmp_value+value)/2.0);
// 		impl->m_capture_thread->m_values[0] = (impl->m_capture_thread->m_values[0]+value)/2.0;
}
void AddValue1Channel(CaptureThreadImpl* impl, double value, int i)
{
	impl->m_capture_thread->m_values.push_front(value);
}

CaptureThread::CaptureThread(const QString& name)
{
	m_current_impl = NULL;

	m_capturing = false;
	m_packet_size = 0;
	m_ext_lock = false;
	m_packet_size_sll = 0;

	m_pause = false;
	m_mix_multiple_channel = false;

	m_name = name;

#ifdef CAPTURE_QT
    m_impls.push_back(new CaptureThreadImplQt(this));
#endif
#ifdef CAPTURE_JACK
	m_impls.push_back(new CaptureThreadImplJACK(this));
#endif
#ifdef CAPTURE_ALSA
	m_impls.push_back(new CaptureThreadImplALSA(this));
#endif
#ifdef CAPTURE_OSS
	m_impls.push_back(new CaptureThreadImplOSS(this));
#endif
#ifdef CAPTURE_PORTAUDIO
	m_impls.push_back(new CaptureThreadImplPortAudio(this));
#endif

	listTransports();
}

void CaptureThread::autoDetectTransport()
{
	bool was_capturing = isCapturing();
	if(was_capturing)
		stopCapture();

	QString old_name;
	if(m_current_impl!=NULL)
		old_name = m_current_impl->m_name;

    cout << "CaptureThread: INFO: Auto detecting a working transport ... " << flush;

	CaptureThreadImpl* impl = NULL;
	for(vector<CaptureThreadImpl*>::iterator it=m_impls.begin(); impl==NULL && it!=m_impls.end(); it++)
		if((*it)->is_available())
			impl = *it;

	if(impl!=NULL)
	{
		m_current_impl = impl;

        cout << "using " << m_current_impl->m_name.toStdString() << endl;

		if(m_current_impl->m_name!=old_name)
			emit(transportChanged(m_current_impl->m_name));

		if(was_capturing)
			startCapture();
	}
	else
	{
        cout << "no working transport !" << endl;

		if(old_name!="")
			emit(transportChanged(""));
	}
}
void CaptureThread::selectTransport(const QString& name)
{
    cout << "CaptureThread: INFO: using " << name.toStdString() << " transport" << endl;
	if(getCurrentTransport() && name==getCurrentTransport()->getName())	return;

	bool was_capturing = isCapturing();
	if(was_capturing)
		stopCapture();

	QString old_name;
	if(m_current_impl!=NULL)
		old_name = m_current_impl->m_name;

	CaptureThreadImpl* impl = NULL;
	for(vector<CaptureThreadImpl*>::iterator it=m_impls.begin(); impl==NULL && it!=m_impls.end(); it++)
		if((*it)->m_name==name)
			impl = *it;

	if(impl==NULL)
	{
        cout << "CaptureThread: ERROR: unknown transport '" << name.toStdString() << "'" << endl;
		throw QString("CaptureThread: ERROR: unknown transport '")+name+"'";
	}

	m_current_impl = impl;

	if(m_current_impl->m_name!=old_name)
		emit(transportChanged(m_current_impl->m_name));

	if(was_capturing)
		startCapture();
}
void CaptureThread::selectTransport(int index)
{
	assert(index>=0 && index<m_impls.size());

	if(m_impls[index]==getCurrentTransport())	return;
        cout << "CaptureThread: INFO: change transport to " << m_impls[index]->getName().toStdString() << " transport" << endl;

	bool was_capturing = isCapturing();
	if(was_capturing)
		stopCapture();

	m_current_impl = m_impls[index];

	emit(transportChanged(m_current_impl->m_name));

	if(was_capturing)
		startCapture();
}
const vector<CaptureThreadImpl*>& CaptureThread::getTransports() const
{
	return m_impls;
}
void CaptureThread::listTransports()
{
    cout << "CaptureThread: INFO: Built in transports" << endl;
	for(vector<CaptureThreadImpl*>::iterator it=m_impls.begin(); it!=m_impls.end(); it++)
        cout << "CaptureThread: INFO:	" << (*it)->getStatus().toStdString() << "	" << (*it)->m_name.toStdString() << "	" << (*it)->m_descr.toStdString() << endl;
}
const CaptureThreadImpl* CaptureThread::getCurrentTransport() const
{
	return m_current_impl;
}
int CaptureThread::getCurrentTransportIndex() const
{
	for(int i=0; i<m_impls.size(); i++)
		if(m_impls[i]==getCurrentTransport())
			return i;

	return -1;
}
QString CaptureThread::getCurrentTransportDescr() const
{
	if(m_current_impl==NULL)
		return "";

	return m_current_impl->m_descr;
}
const CaptureThreadImpl* CaptureThread::getTransport(const QString& name) const
{
	for(vector<CaptureThreadImpl*>::const_iterator it=m_impls.begin(); it!=m_impls.end(); it++)
		if((*it)->m_name==name)
			return *it;

	return NULL;
}
QString CaptureThread::getFormatDescr() const
{
	if(m_current_impl==NULL)
		return "";

	return ""; // TODO
}

void CaptureThread::emitError(const QString& error)
{
	emit(errorRaised(error));
}
void CaptureThread::emitSamplingRateChanged()
{
	if(m_current_impl->m_sampling_rate>0)
		emit(samplingRateChanged(m_current_impl->m_sampling_rate));
}
void CaptureThread::emitCaptureStarted()
{
	emit(captureStarted());
}
void CaptureThread::emitCaptureStoped()
{
	emit(captureStoped());
}
void CaptureThread::emitCaptureToggled(bool value)
{
	emit(captureToggled(value));
}

void CaptureThread::startCapture()
{
	if(m_current_impl==NULL)	return;

	m_current_impl->startCapture();
}
void CaptureThread::stopCapture()
{
    //	cout << "CaptureThread::stopCapture" << endl;

	if(m_current_impl==NULL)	return;

	m_current_impl->stopCapture();

    //	cout << "/CaptureThread::stopCapture" << endl;
}

void CaptureThread::toggleCapture(bool run)
{
	if(run && !m_capturing)	startCapture();
	if(!run && m_capturing)	stopCapture();
}

void CaptureThread::reset()
{
	stopCapture();
	startCapture();
}

void CaptureThread::togglePause(bool pause)
{
	m_pause = pause;
}

int CaptureThread::getSamplingRate() const
{
	if(m_current_impl==NULL)	return SAMPLING_RATE_UNKNOWN;

	return m_current_impl->m_sampling_rate;
}
void CaptureThread::setSamplingRate(int rate)
{
	if(m_current_impl!=NULL)
		m_current_impl->setSamplingRate(rate);
}
void CaptureThread::setSource(const QString& name)
{
	if(m_current_impl==NULL)
	{
        cout << "CaptureThread: setSource: ERROR: select a transport first" << endl;
		return;
	}

	if(name!=m_current_impl->m_source)
	{
		m_current_impl->m_source = name;
		if(isCapturing())
		{
			stopCapture();
			startCapture();
		}

		emit(sourceChanged(m_current_impl->m_source));
	}
}

void CaptureThread::setMixMultipleChannels(bool mix)
{
	m_mix_multiple_channel = mix;
}

CaptureThread::~CaptureThread()
{
	stopCapture();

	for(vector<CaptureThreadImpl*>::iterator it=m_impls.begin(); it!=m_impls.end(); it++)
		delete *it;
}

// -------------------------------- implementation ------------------------------

CaptureThreadImpl::CaptureThreadImpl(CaptureThread* capture_thread, const QString& name, const QString& descr)
: m_capture_thread(capture_thread)
{
	m_name = name;
	m_descr = descr;
	m_status = "";

	m_sampling_rate = CaptureThread::SAMPLING_RATE_UNKNOWN;
	m_format_size = 0;
	m_format_signed = true;
	m_channel_count = 0;
	m_source = "";
}

const QString& CaptureThreadImpl::getStatus()
{
	if(m_status=="")
		is_available();

	return m_status;
}

void CaptureThreadImpl::setFormatDescrsAndFns(int format_size, bool format_signed, bool format_float, int channel_count)
{
	m_format_size = format_size;
	m_format_signed = format_signed;
	m_format_float = format_float;
	m_channel_count = channel_count;

	if(m_format_size==2) // 16bits
	{
		if(m_format_signed)	decodeValue = DecodeSigned16Bits;
		else				decodeValue = DecodeUnsigned16Bits;
	}
	else	// 8bits
	{
		if(m_format_signed)	decodeValue = DecodeSigned8Bits;
		else				decodeValue = DecodeUnsigned8Bits;
	}

	if(m_channel_count==1)
		addValue = AddValue1Channel;
	else if(m_channel_count==2)
	{
		if(m_capture_thread->m_mix_multiple_channel)
			addValue = AddValue2ChannelMix;
		else
			addValue = AddValue2ChannelFirst;
	}

    cout << "CaptureThread: INFO: format is " << (m_format_signed?"signed":"unsigned") << " " << (m_format_float?"float":"integer") << " " << m_format_size*8 << "bits with " << m_channel_count << " channel(s)" << endl;
}

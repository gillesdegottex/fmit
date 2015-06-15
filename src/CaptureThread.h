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


#ifndef _CaptureThread_h_
#define _CaptureThread_h_

#include <deque>
#include <vector>
using namespace std;
#include <qobject.h>
#include <qthread.h>
#include <qmutex.h>

class CaptureThread;

// ----------------------- the implementations ----------------------

class CaptureThreadImpl;

void AddValue2ChannelFirst(CaptureThreadImpl* impl, double value, int i);
void AddValue2ChannelMix(CaptureThreadImpl* impl, double value, int i);
void AddValue1Channel(CaptureThreadImpl* impl, double value, int i);

class CaptureThreadImpl
{
	friend class CaptureThread;
	friend void AddValue2ChannelFirst(CaptureThreadImpl* impl, double value, int i);
	friend void AddValue2ChannelMix(CaptureThreadImpl* impl, double value, int i);
	friend void AddValue1Channel(CaptureThreadImpl* impl, double value, int i);

  protected:
	CaptureThread* m_capture_thread;
	QString m_name;
	QString m_descr;

	int m_sampling_rate;
	QString m_source;
	QString m_status;

	int m_format_size;
	bool m_format_signed;
	bool m_format_float;
	int m_channel_count;

	virtual void setSamplingRate(int rate)=0;
	virtual void startCapture()=0;
	virtual void stopCapture()=0;
	virtual bool is_available()=0;

    double m_tmp_value; // used when two channels have to be mixed down
	double (*decodeValue)(void* buffer, int i); // not used for JACK
	void (*addValue)(CaptureThreadImpl* impl, double value, int i);
	void setFormatDescrsAndFns(int format_size, bool format_signed, bool format_float, int channel_count);

  public:
	CaptureThreadImpl(CaptureThread* capture_thread, const QString& name, const QString& descr);

	const QString& getStatus();

	const QString& getName() const {return m_name;}
	const QString& getDescription() const {return m_descr;}
	const QString& getSource() const {return m_source;}

	virtual ~CaptureThreadImpl(){}
};

// ---------------------- the Qt implementation ---------------------

#ifdef CAPTURE_QT
#include <QAudioDeviceInfo>
#include <QAudioInput>
class CaptureThreadImplQt : public QObject, public CaptureThreadImpl
{
    Q_OBJECT

    void set_params(bool test=false);

    void capture_init();
    void capture_finished();

    const QList<QAudioDeviceInfo> m_availableAudioInputDevices;
    QAudioDeviceInfo    m_audioInputDevice;
    QAudioInput*        m_audioInput;
    QIODevice*          m_audioInputIODevice;

    QAudio::State       m_state;

    QAudioFormat        m_format;

  public:
    CaptureThreadImplQt(CaptureThread* capture_thread);

    virtual void setSamplingRate(int rate);
    virtual void startCapture();
    virtual void stopCapture();
    virtual bool is_available();

    ~CaptureThreadImplQt();

public slots:
    void audioStateChanged(QAudio::State state);
    void audioDataReady();
};
#endif

// ---------------------- the ALSA implementation ---------------------

#ifdef CAPTURE_ALSA
#include <alsa/asoundlib.h>
class CaptureThreadImplALSA : public CaptureThreadImpl, public QThread
{
	snd_pcm_t* m_alsa_capture_handle;
	snd_pcm_hw_params_t* m_alsa_hw_params;
	char* m_alsa_buffer;
	snd_pcm_format_t m_format;

	// view
	volatile bool m_alive;
	volatile bool m_in_run;

	// control
	volatile bool m_loop;
	volatile bool m_wait_for_start;

	void set_params(bool test=false);
	void capture_init();
	void capture_loop();
	void capture_finished();

	virtual void run();

  public:
	CaptureThreadImplALSA(CaptureThread* capture_thread);

	virtual void setSamplingRate(int rate);
	virtual void startCapture();
	virtual void stopCapture();
	virtual bool is_available();

	~CaptureThreadImplALSA();
};
#endif

// ---------------------- the JACK implementation ---------------------

#ifdef CAPTURE_JACK
#include <jack/jack.h>
#include <jack/ringbuffer.h>
class CaptureThreadImplJACK : public CaptureThreadImpl, public QThread
{
	static int JackProcess(jack_nframes_t nframes, void* arg);
	static void JackShutdown(void* arg);
	static int JackSampleRate(jack_nframes_t nframes, void* arg);

    jack_ringbuffer_t* m_ringbuffer;
	jack_client_t* m_jack_client;
	jack_port_t* m_jack_port;
	int jackSampleRate(jack_nframes_t nframes);
	int jackProcess(jack_nframes_t nframes);
	void jackShutdown();

    // view
    volatile bool m_alive;
    volatile bool m_in_run;

    // control
    volatile bool m_loop;
    volatile bool m_wait_for_start;

    void capture_init();
    void capture_loop();
	void capture_finished();

    virtual void run();

  public:
	CaptureThreadImplJACK(CaptureThread* capture_thread);

	virtual void setSamplingRate(int rate);
	virtual void startCapture();
	virtual void stopCapture();
	virtual bool is_available();
    
    ~CaptureThreadImplJACK();
};
#endif

// ---------------------- the PortAudio implementation ---------------------

#ifdef CAPTURE_PORTAUDIO
#include <portaudio.h>
class CaptureThreadImplPortAudio : public CaptureThreadImpl
{
	static int PortAudioCallback( const void *inputBuffer, void *outputBuffer,
							   unsigned long framesPerBuffer,
							   const PaStreamCallbackTimeInfo* timeInfo,
							   PaStreamCallbackFlags statusFlags,
							   void *userData );

	int portAudioCallback(const void *inputBuffer,
						  unsigned long framesPerBuffer,
						  const PaStreamCallbackTimeInfo* timeInfo,
						  PaStreamCallbackFlags statusFlags);

	PaStream* m_stream;
	PaError m_err;

	void set_params(bool test=false);
	virtual void capture_init();
	virtual void capture_finished();

  public:
	CaptureThreadImplPortAudio(CaptureThread* capture_thread);

	virtual void setSamplingRate(int rate);
	virtual void startCapture();
	virtual void stopCapture();
	virtual bool is_available();

	virtual ~CaptureThreadImplPortAudio();
};
#endif

// ---------------------- the OSS implementation ---------------------

#ifdef CAPTURE_OSS
class CaptureThreadImplOSS : public CaptureThreadImpl, public QThread
{
	int m_fd_in;
	char* m_oss_buffer;
	int m_format;

	// view
	volatile bool m_alive;
	volatile bool m_in_run;

	// control
	volatile bool m_loop;
	volatile bool m_wait_for_start;

	void set_params(bool test=false);
	void capture_init();
	void capture_loop();
	void capture_finished();

	virtual void run();

  public:
	CaptureThreadImplOSS(CaptureThread* capture_thread);

	virtual void setSamplingRate(int rate);
	virtual void startCapture();
	virtual void stopCapture();
	virtual bool is_available();

	~CaptureThreadImplOSS();
};
#endif

// --------------------- the real accessible thread -------------------------

class CaptureThread : public QObject
{
	Q_OBJECT

	friend class CaptureThreadImpl;
#ifdef CAPTURE_QT
    friend class CaptureThreadImplQt;
#endif
#ifdef CAPTURE_ALSA
	friend class CaptureThreadImplALSA;
#endif
#ifdef CAPTURE_JACK
	friend class CaptureThreadImplJACK;
#endif
#ifdef CAPTURE_PORTAUDIO
	friend class CaptureThreadImplPortAudio;
#endif
#ifdef CAPTURE_OSS
	friend class CaptureThreadImplOSS;
#endif

	vector<CaptureThreadImpl*> m_impls;
	CaptureThreadImpl* m_current_impl;

	void emitError(const QString& error);
	void emitSamplingRateChanged();
	void emitCaptureStarted();
	void emitCaptureStoped();
	void emitCaptureToggled(bool value);

	bool m_capturing;

	int m_packet_size;
	int m_packet_size_sll;
	QString m_name;

	// control
	volatile bool m_pause;
	bool m_mix_multiple_channel;

	QMutex m_lock;
	bool m_ext_lock;

  public:

	deque<double> m_values;

	enum {SAMPLING_RATE_UNKNOWN=-1, SAMPLING_RATE_MAX=0};

	CaptureThread(const QString& name="bastard_thread");

	void lock()						{m_lock.lock(); m_ext_lock=true;}
	void unlock()					{m_lock.unlock();}

	bool isCapturing() const						{return m_capturing;}
	int getSamplingRate() const;
	int getPacketSize() const						{return m_packet_size;}
	int getPacketSizeSinceLastLock() const			{return m_packet_size_sll;}
	int getNbPendingData() const					{return m_values.size();}
	const CaptureThreadImpl* getCurrentTransport() const;
	int getCurrentTransportIndex() const;
	QString getCurrentTransportDescr() const;
	QString getFormatDescr() const;
	const vector<CaptureThreadImpl*>& getTransports() const;
	const CaptureThreadImpl* getTransport(const QString& name) const;
	void listTransports();

	virtual ~CaptureThread();

  signals:
	void samplingRateChanged(int sampling_rate);
	void portNameChanged(const QString& name);
	void sourceChanged(const QString& src);
	void transportChanged(const QString& name);
	void captureStarted();
	void captureStoped();
	void captureToggled(bool run);
	void errorRaised(const QString& error);

  public slots:
	//! auto detect a working transport
	void autoDetectTransport();
	//! select a specific transport
	void selectTransport(const QString& name);
	//! select a specific transport
	void selectTransport(int index);
	//! reset capture (stop/start)
	void reset();
	//! start capture
	void startCapture();
	//! stop capture
	void stopCapture();
	//! set capture status
	void toggleCapture(bool run);
	//! set pause status
	/*! keep capture system connected, but throw away all incoming data
	 */
	void togglePause(bool pause);

	//! set the sampling rate
	/*! not always available, depending on the implementation
	 * (reset the capture system !)
	 */
	void setSamplingRate(int value);
	//! the source name
	/*! 'hw:0' for example for ALSA, something like alsa_pcm:capture_1 for JACK
	 * (reset the capture system !)
	 */
	void setSource(const QString& src);

	void setMixMultipleChannels(bool mix);
};

#endif // _CaptureThread_h_

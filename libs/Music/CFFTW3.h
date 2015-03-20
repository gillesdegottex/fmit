#ifndef _CFFTw3_h_
#define _CFFTw3_h_

#include <complex>
#include <vector>
#include <deque>
#include <fftw3.h>

class CFFTW3
{
	int m_size;

	fftw_plan m_plan;
	fftw_complex *m_in, *m_out;
	bool m_forward;

  public:
	CFFTW3(bool forward=true);
	CFFTW3(int n);
	void resize(int n);

	int size(){return m_size;}

	std::vector<double> in;
	std::vector<std::complex<double> > out;

	void execute(const std::vector<double>& in, std::vector<std::complex<double> >& out);
	void execute();

	~CFFTW3();
};

#endif

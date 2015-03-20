#include "CFFTW3.h"

#include <assert.h>
using namespace std;
#include <CppAddons/CAMath.h>
using namespace Math;

CFFTW3::CFFTW3(bool forward)
{
	m_size = 0;
	m_out = m_in = NULL;
	m_plan = NULL;
	m_forward = forward;
}
CFFTW3::CFFTW3(int n)
{
	resize(n);
}
void CFFTW3::resize(int n)
{
	assert(n>0);

	m_size = n;

	m_out = m_in = NULL;

	m_in = new fftw_complex[m_size];
	m_out = new fftw_complex[m_size];
	in.resize(m_size);
	out.resize(m_size);

	//  | FFTW_PRESERVE_INPUT
	if(m_forward)
		m_plan = fftw_plan_dft_1d(m_size, m_in, m_out, FFTW_FORWARD, FFTW_MEASURE);
	else
		m_plan = fftw_plan_dft_1d(m_size, m_in, m_out, FFTW_BACKWARD, FFTW_MEASURE);
}

void CFFTW3::execute()
{
	execute(in, out);
}

void CFFTW3::execute(const vector<double>& in, vector<std::complex<double> >& out)
{
	assert(int(in.size())>=m_size);

	for(int i=0; i<m_size; i++)
	{
		m_in[i][0] = in[i];
		m_in[i][1] = 0.0;
	}

	fftw_execute(m_plan);

	if(int(out.size())<m_size)
		out.resize(m_size);

	for(int i=0; i<m_size; i++)
		out[i] = make_complex(m_out[i]);
}

CFFTW3::~CFFTW3()
{
	if(m_plan)	fftw_destroy_plan(m_plan);
	if(m_in)	delete[] m_in;
	if(m_out)	delete[] m_out;
}

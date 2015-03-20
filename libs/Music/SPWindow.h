// Copyright 2007 "Gilles Degottex"

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


#ifndef _SPWindow_h_
#define _SPWindow_h_

#include <vector>
#include <CppAddons/CAMath.h>

namespace Music
{
	std::vector<double> hann(int n);
	std::vector<double> hamming(int n);

	inline double win_sinc(double x, double f)
	{
		if(x==0.5) return 1.0;
		double s = Math::sinc((x-0.5)*f);
		return s*s;
	}

	struct Win_Sinc
	{
		double m_f;

		Win_Sinc(double f) : m_f(f)	{}

		double operator()(double x)		{return win_sinc(x, m_f);}
	};

	//! usefull gauss fonction
	/*!
		* \param x \f$\approx\in [0,1]\f$
		* \param f width factor
	*/
	inline double win_gauss(double x, double f)
	{
		return Math::gauss((x-0.5)*f);
	}

	//! object fonction of \ref win_gauss
	struct Win_Gauss
	{
		double m_g;

		Win_Gauss(double g) : m_g(g)	{}

		double operator()(double x)		{return win_gauss(x, m_g);}
	};

	//! compute integrale of fn \f$\in [0,1]\f$
	/*! with a Simpson algorithm
	*/
	template<typename Fn>
			inline double Usefull(Fn fn, double simpsonStep=0.001)
	{
		return Math::Simpson(0.0, 1.0, fn, simpsonStep);
	}
}

#endif // _SPWindow_h_

// Copyright 2003 "Gilles Degottex"

// This file is part of "CppAddons"

// "CppAddons" is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "CppAddons" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#ifndef _Math_h_
#define _Math_h_

#include <math.h>
#include <cmath>
#include <complex>
using namespace std;

#undef min
#undef max

namespace Math
{
	template<typename TypeData> inline TypeData sgn(TypeData a)			{return (a<0)?-1:1;}

    extern const double Pi;
    extern const double Pi2;
    extern const float fPi;

    extern const double E;
    extern const float fE;

	// résoud une equation du 2ème degré
	class SolOfEq2
	{
	  public:
		enum ENError{NE_OK=0, NE_DISCRIMINENT_NEG, NE_A_AND_B_EQ_ZERO, NE_RACINE_NEG, NE_X1_AND_X2_NEG, NE_X1_AND_X2_POS};

	  private:
		ENError m_err;

		double	x1;
		double	x2;

	  public:
		double getX1(){return x1;}
		double getX2(){return x2;}
		double getPosSol();

		SolOfEq2(double a, double b, double c);
	};

	// calcul l'intérale de f sur [a;b] avec un pas de h
	// méhode de Simpson
	template<class Function>
	double Simpson(double a, double b, Function f, double h)
	{
		double I4=f(a+h/2.0), I2=0;
		for(double x4=a+(h/2.0)+h, x2=a+h; x4<b; x4+=h, x2+=h)
		{
			I4+=f(x4);
			I2+=f(x2);
		}
		return (h/6.0)*(f(a)+4*I4+2*I2+f(b));
	}

	template<typename Type>	std::complex<Type> make_complex(Type value[]){return std::complex<Type>(value[0], value[1]);}

	inline double modulo(double d1, double d2)
	{
		    return d1-int(d1/d2)*d2;
	}
	inline double mod2(const double c[2])
	{
		return c[0]*c[0]+c[1]*c[1];
	}
	inline double mod(const double c[2])
	{
		return sqrt(mod2(c));
	}
	inline double mod2(const std::complex<double>& c)
	{
		return c.real()*c.real()+c.imag()*c.imag();
	}
	inline double mod(const std::complex<double>& c)
	{
		return sqrt(mod2(c));
	}

	inline double mod_equal(double& d1, double d2)
	{
		    return d1 -= int(d1/d2)*d2;
	}

	//! gauss fonction
	/*!
		* \param x \f$\in ]-\infty,\infty[\f$
	*/
	inline double gauss(double x)
	{
		return exp(-Math::Pi*x*x);
	}

	inline double sinc(double t)
	{
		if(t==0.0)	return 1.0;

		return sin(Math::Pi*t)/(Math::Pi*t);
	}

/*	doesn't need for Linux for sure, seems to be probelatic under win32 (macro ambiquity)
	template<typename TypeData1, typename TypeData2>
	TypeData1 max(const TypeData1& a, const TypeData2& b)
	{
	return (a>b)?a:b;
}

	template<typename TypeData1, typename TypeData2>
	TypeData1 min(const TypeData1& a, const TypeData2& b)
	{
	return (a<b)?a:b;
}*/

//	template<typename TypeData> TypeData abs(const TypeData& a) {return (a<0)?-a:a;}	// include <cmath> instead
//	template<typename TypeData> TypeData abs(TypeData a)		{return (a<0)?-a:a;}	// include <cmath> instead

}

#endif


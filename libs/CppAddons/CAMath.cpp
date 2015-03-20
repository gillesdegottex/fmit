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


#include "CAMath.h"

double Math::SolOfEq2::getPosSol()
{
	if(x1<0)
	{
		if(x2<0)
		{
			m_err=NE_X1_AND_X2_NEG;
			return 0;
		}
		else		return x2;
	}
	else
	{
		if(x2>0)
		{
			m_err=NE_X1_AND_X2_POS;
			return 0;
		}
		else		return x1;
	}
}

Math::SolOfEq2::SolOfEq2(double a, double b, double c)
{
	m_err = NE_OK;

	if(a==0)
	{
		if(b==0)
		{
			m_err=NE_A_AND_B_EQ_ZERO;
			x1=0;
			x2=0;
		}
		else
		{
			x1=-c/b;
			x2=x1;
		}
	}
	else if(b==0)
	{
		double d=-c/a;
		if(d<0)
		{
			m_err=NE_RACINE_NEG;
			x1=0;
			x2=0;
		}
		else
		{
			x1=sqrt(d);
			x2=-x1;
		}
	}
	else
	{
		double d=b*b-4*a*c;
		if(d<0)
		{
			m_err=NE_DISCRIMINENT_NEG;
			x1=0;
			x2=0;
		}
		else
		{
			d=sqrt(d);

			a*=2;
			x1=(-b+d)/a;
			x2=(-b-d)/a;
		}
	}
}

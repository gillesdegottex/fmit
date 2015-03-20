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


#include "Random.h"
#include <stdlib.h>
#include <cmath>

const int Random::A = 48271;
//const int Random::M = 2147483647;
const int Random::M = RAND_MAX;
const int Random::Q = M / A;
const int Random::R = M % A;

Random Random::s_random;

Random::Random(long seed)
{
	m_haveNextNextGaussian=false;

	setSeed(seed);

	next();
}

void Random::setSeed(long seed)
{
	srand(seed);

	if(seed < 0)	seed += M+1;
	m_seed = seed;
	if(m_seed==0)	m_seed = 1;

	m_haveNextNextGaussian = false;
}

long Random::next()
{
//	int tmp = A * (m_seed % Q) - R * (m_seed / Q);
//	if(tmp>=0)	m_seed = tmp;
//	else		m_seed = tmp + M;
//
//	return m_seed;

	return rand();
}

double Random::nextGaussian()
{
//	Throw("nextGaussian", "not yet implemented");

	// See Knuth, ACP, Section 3.4.1 Algorithm C.
	if(m_haveNextNextGaussian)
	{
    	m_haveNextNextGaussian = false;
    	return m_nextNextGaussian;
	}
	else
	{
		double v1, v2, s;
    	do
		{
			v1 = 2 * nextDouble() - 1; // between -1 and 1
			v2 = 2 * nextDouble() - 1; // between -1 and 1
			s = v1 * v1 + v2 * v2;
    	}
		while (s >= 1 || s == 0);

		double multiplier = sqrt(-2 * log(s)/s);

    	m_nextNextGaussian = v2 * multiplier;
		m_haveNextNextGaussian = true;

		return v1 * multiplier;
	}
}


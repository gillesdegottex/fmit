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


#ifndef _Random_h_
#define _Random_h_

#include <time.h>

class Random
{
private:
	static const int A;
	static const int M;
	static const int Q;
	static const int R;
 
	long m_seed;
	double m_nextNextGaussian;
	bool m_haveNextNextGaussian;

public:
	Random(long seed=time(NULL));

	void setSeed(long seed);

	static Random s_random;

protected:
	long next();

public:

	int nextInt()
	{
		return (int)next();
	}

	int nextInt(int max)
	{
		return nextInt()%max;
	}

	int nextInt(int a, int b)
	{
		return nextInt()%(b-a) + a;
	}

	long nextLong()
	{
		return next();
	}

    bool nextBoolean()
	{
		return next() % 2 == 0;
	}

	float nextFloat()
	{
		return nextInt() / float(M);
	}

	double nextDouble()
	{
		return nextInt() / double(M);
	}

	char nextLetter()
	{
		return char(('z' - 'a' + 1) * nextDouble() + 'a');
	}

	char nextFigure()
	{
		return char(('9' - '0' + 1) * nextDouble() + '0');
	}

	double nextGaussian();
};

#endif


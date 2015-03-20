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


#include "SPWindow.h"
using namespace std;

vector<double> Music::hann(int n)
{
	vector<double> win(n);

	for(size_t n=0; n<win.size(); n++)
		win[n] = (1-cos(2*Math::Pi*n/(win.size()-1))) / (win.size()-1);

	return win;
}

vector<double> Music::hamming(int n)
{
	vector<double> win(n);

	double s = 0.0;
	for(size_t n=0; n<win.size(); n++)
	{
		win[n] = (0.54-0.46*cos(2*Math::Pi*n/(win.size()-1)));
		s += win[n];
	}
	for(size_t n=0; n<win.size(); n++)
		win[n] /= s;

	return win;
}

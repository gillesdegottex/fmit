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

#include "Note.h"

#include <limits>

#include "Music/Music.h"

void Note::init()
{
    min_err = std::numeric_limits<float>::max();
    max_err = -std::numeric_limits<float>::max();
	avg_err = 0.0f;
}
Note::Note(int h)
	: ht(h)
{
	init();
}
Note::Note(int h, int num, int den)
	: ht(h)
{
	init();
	factor = QString::number(num)+"/"+QString::number(den);
}
Note::Note(int h, float cents)
	: ht(h)
{
	init();
	factor = QString::number(cents);
}
QString Note::getName() const
{
    return QString(Music::h2n(ht).c_str())+factor;
}
void Note::addAnalysis(float err, float volume)
{
	min_err = (err<min_err)?err:min_err;
	max_err = (err>max_err)?err:max_err;

	if(!errors.empty())
		avg_err *= errors.size();

	errors.push_back(err);

	avg_err += err;
	avg_err /= errors.size();

	volumes.push_back(volume);
}

// Copyright 2006,2007 "Gilles Degottex"

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


#include "MonoQuantizer.h"

#include <iostream>
using namespace std;
#include <Music/Music.h>
using namespace Music;

MonoQuantizer::MonoQuantizer()
{
	m_min_confidence = 0.0;

	m_confidence = 1.0;
	m_current_center_freq = 0.0;
	m_avg_freq = 0.0;
}

MonoQuantizer::~MonoQuantizer()
{
}

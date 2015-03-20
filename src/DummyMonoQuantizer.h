// Copyright 2005 "Gilles Degottex"

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


#ifndef _DUMMYMONOQUANTIZER_H_
#define _DUMMYMONOQUANTIZER_H_

#include <deque>
using namespace std;
#include <qobject.h>
#include <qdatetime.h>
#include <Music/Music.h>
using namespace Music;
#include "MonoQuantizer.h"

class DummyMonoQuantizer : public MonoQuantizer
{
	QTime m_time;
	QTime m_duration;

  public:
	DummyMonoQuantizer();

	virtual void quantize(double freq);
	virtual void reset();

	virtual ~DummyMonoQuantizer(){}
};

#endif // _DUMMYMONOQUANTIZER_H_

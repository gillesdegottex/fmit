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


#ifndef _Singleton_h_
#define _Singleton_h_

#include <assert.h>

template<class Type> class Singleton
{
	static Type* sm_instance;

protected:
	Singleton<Type>()
	{
		assert(sm_instance==0);
		sm_instance = (Type*)this;
	}

public:
	inline static Type& getInstance()
	{
		assert(sm_instance!=0);
		return *sm_instance;
	}
};

template<class Type> Type* Singleton<Type>::sm_instance=0;

#endif

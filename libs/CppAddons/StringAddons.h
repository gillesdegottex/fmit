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


#ifndef _StringAddons_h_
#define _StringAddons_h_

#include <stdio.h>
#include <string>
#include <sstream>

#include <ctype.h>

#include <time.h>

namespace StringAddons
{
	template<typename Type> std::string toString(Type i)
	{
		std::stringstream str;
		str << i;
		return str.str();
	}
	template<typename Type> void toString(std::string& s, Type i)
	{
		std::stringstream str;
		str << i;
		s = str.str();
	}

	inline std::string ereaseEmptyChars(const std::string& str)
	{
		std::string result;

		for(unsigned int i=0; i<str.length(); i++)
			if(!isspace(str[i]))
				result += str[i];

		return result;
	}

	template<typename T> inline std::string binValue(T& d)
	{
		char* data = (char*)&d;

		std::string str;

		for(int i=sizeof(T)-1; i>=0; i--)
		{
			for(int j=7; j>=0; j--)
			{
				if(data[i] & (0x01<<j))	str += "1";
				else					str += "0";
			}

			if(i>0) str += "'";
		}

		return str;
	}

#ifdef WIN32
	std::wstring toWide(const std::string& str);
	std::string toAnsi(const std::wstring& str);
#endif

	// Undoable Out
	template<typename OstreamType> void undoable_out_clear(OstreamType& out, int n)
	{
		while(n-->0) out << "\b";
	}
	template<typename OstreamType> int undoable_out(OstreamType& out, const std::string& s, int n)
	{
		undoable_out_clear(out, n);
		std::stringstream str;
		str.precision(1);
		str << s;
		n = str.tellp();
		out << str.str();
		return n;
	}
	template<class OstreamType> int undoable_out_percent(OstreamType& out, float f, int n=0)
	{
		undoable_out_clear(out, n);
		std::stringstream str;
		str.precision(1);
		str << std::fixed << f << "%   " << std::flush;
		n = str.tellp();
		out << str.str();
		return n;
	}

/*	//-----------------------------------------------------------------------------
	// UNICODE support for converting between CHAR, TCHAR, and WCHAR strings
	//-----------------------------------------------------------------------------
	void ConvertGenericStringToAnsi( CHAR* strDestination, const TCHAR* tstrSource, int cchDestChar = -1 );
	void ConvertGenericStringToWide( WCHAR* wstrDestination, const TCHAR* tstrSource, int cchDestChar = -1 );
	void ConvertAnsiStringToGeneric( TCHAR* tstrDestination, const CHAR* strSource, int cchDestChar = -1 );
	void ConvertWideStringToGeneric( TCHAR* tstrDestination, const WCHAR* wstrSource, int cchDestChar = -1 );
*/
}

#endif

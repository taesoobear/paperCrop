//
// operatorString.cpp 
//
// Copyright 2004 by Taesoo Kwon.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//

// TypeString.cpp: implementation of the TypeString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TypeString.h"
#include "operatorString.h"
#include <stdio.h>
#include <string.h>


TString sz1::format(const char* pszFormat, ...) 
{
	TString m_str;
	va_list argptr ;
	va_start(argptr, pszFormat);
	m_str._format(pszFormat, argptr);
	return m_str;
}


// extract filename: eg) fn="c:\a\b.bmp" -> return "b.bmp", where dir="c:\a"
TString sz1::filename(TString const& fn, TString& dir)
{
	TString lfn;
	// split fn into dir+lfn
	int currDir;
	if((currDir=fn.findCharRight('/'))!=-1)
	{
		lfn=fn.right(-1*currDir-1);
		dir=fn.left(currDir);
	}
	else
	{
		dir.empty();
		lfn=fn;
	}

	return lfn;
}

TString sz1::extension(TString const& fn)
{
	int pos;
	if((pos=fn.findCharRight('.'))!=-1)
	{
		return fn.right(-1*pos-1);
	}
	TString temp;
	temp.empty();
	return temp;
}

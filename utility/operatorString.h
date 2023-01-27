//
// operatorString.h
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

#pragma once
#include "TypeString.h"


namespace sz1
{
	TString format(const char* pszFormat, ...);
	
	// extract filename: eg) fn="c:\a\b.bmp" -> return "b", where dir="c:\a\", ext=".bmp"
	TString filename(TString const& fn, TString& dir);
	
	// extract extension: eg) fn="c:\a\b.bmp" -> return "bmp"
	TString extension(TString const& fn);
}


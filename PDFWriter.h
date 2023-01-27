#pragma once
//
// PDFWriter.h
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

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include "hpdf.h"


class CImage;
class PDFWriter
{
    HPDF_Doc  pdf;
	HPDF_Font font;
	std::vector<HPDF_BYTE > im;
	std::vector<HPDF_BYTE > im4;
public:
	PDFWriter(){pdf=NULL; }
	~PDFWriter(){ HPDF_Free (pdf);}

	bool init();
	bool isValid() { return pdf!=NULL;}
	void addPage(CImage const& pageImage, int bpp); // currently only 4 and 8 is supported
	void addPageColor(CImage const& pageImage);
	void save(const char* fname);
};


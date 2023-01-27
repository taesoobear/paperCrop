//
// RightPanel.H 
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


#ifndef RIGHT_PANEL_H_
#define RIGHT_PANEL_H_
#pragma once
#include "utility/FlLayout.h"

class PDFwin;
class RightPanel :
	public FlLayout
{
public:
	PDFwin* mPDFwin;
	RightPanel(int x, int y, int w, int h,PDFwin* );
	virtual ~RightPanel(void);

	void doLUAFile(const char* filename);

	// FlLayout::
	void onCallback(FlLayout::Widget const& w, Fl_Widget * pWidget, int userData);

};
#endif

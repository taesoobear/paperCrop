
//
// PaperCrop.cpp 
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
#include "PaperCrop.h"

#include "PDFwin.h"
#include "RightPanel.h"
MainWin::MainWin(const char* title) 
	: Fl_Window(WIDTH, HEIGHT, title)		
{
	//Fl_Tile* tile=new Fl_Tile (0,0,WIDTH,HEIGHT);
	
	int ww=WIDTH-RENDERER_WIDTH;

	mPDFwin=new PDFwin(0,0,RENDERER_WIDTH,HEIGHT);
	mRightPanel	=new RightPanel(RENDERER_WIDTH,0, ww, HEIGHT, mPDFwin);
	//tile->end();
	end();		

	resizable(mPDFwin);
}

void MainWin::onIdle()
{
	mPDFwin->onIdle();
}

#include <IL/il.h>
#include <IL/ilu.h>

TString g_arg;
#ifdef _MSC_VER
//#ifdef _DEBUG	// console·Î ºôµåÇÏµµ·Ï ÇØ³ùÀ½.
int _tmain(int argc, _TCHAR* argv[])
/*#else
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
#endif*/
#else
int main(int argc, char* argv[])
#endif
{

#ifndef NO_SHOW_WIN
	if (!Fl::visual(FL_DOUBLE|FL_INDEX))
		printf("Xdbe not supported, faking double buffer with pixmaps.\n"); 
	Fl::scheme("plastic");
#endif

	ilInit();
	iluInit();

	if (argc==2)
		g_arg=argv[1];

	for (int i=0; i<argc; i++)
		printf("%s\n", argv[i]);

	MainWin win("PaperCrop");

#ifndef NO_SHOW_WIN
	win.show();

	for (;;) 
	{
		if (win.visible())
		{
			if (!Fl::check()) 
				break;	// returns immediately
		}	
		else
		{
			if (!Fl::wait()) break;	// waits until something happens
		}	

		// idle time
		win.onIdle();
	}

#endif
	return 0;

}

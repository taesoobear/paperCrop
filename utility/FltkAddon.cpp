//
// FltkAddon.cpp 
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
#include "FltkAddon.h"
#include <FL/Fl_Color_Chooser.H>

#if defined( unix)||defined(__APPLE__)
#include <unistd.h>
#endif

TString getCurrentDirectory()
{
#ifdef WIN32
	char szCurrDirectory[512];
	::GetCurrentDirectory(512, szCurrDirectory);
	return TString(szCurrDirectory);
#else
	return "~tskwon/taesoo_cmu/OgreFltk/work"; // hard coded at the moment.
#endif
}
#ifdef __APPLE__
//std::string chooseFileMac(const char* path, const char* mask);
#endif

TString _getcwd()
{   
#ifdef _MSC_VER
  // utility. It opens the file dialog and choose file.
	char szCurrDirectory[512];
	GetCurrentDirectory(512, szCurrDirectory);
	return szCurrDirectory;
#else
   	const size_t chunkSize=255;
    const int maxChunks=10240; // 2550 KiBs of current path are more than enough

    char stackBuffer[chunkSize]; // Stack buffer for the "normal" case
    if(getcwd(stackBuffer,sizeof(stackBuffer))!=NULL)
        return TString(stackBuffer);
#endif
}
void _chdir(const char* szCurrDirectory)
{
#ifdef _MSC_VER
	SetCurrentDirectory(szCurrDirectory);
#else
	chdir(szCurrDirectory);
#endif
}
#ifndef NO_GUI

bool IsFileExist(const char* filename)
{
	FILE* temp;
	temp=fopen(filename,"r");
	if(!temp) return false;
	fclose(temp);
	return true;
}
#if defined(__APPLE__) || !defined(_MSC_VER)
static TString _FlChooseFile_python(const char* message, const char* path, const char* Mask, bool bCreate)
{
	Fl::check(); // check input and redraw.

	// use filechooser.py on linux
	TString cmd;
#ifdef __APPLE__
	if(bCreate)
		cmd.format("python3 filechooser_wx.py '%s' '%s' '%s' SAVE", message, Mask, path);
	else
		cmd.format("python3 filechooser_wx.py '%s' '%s' '%s' OPEN", message, Mask, path);
#else
	if(IsFileExist("/usr/bin/python3"))
	{
		if(bCreate)
			cmd.format("python3 filechooser_wx.py '%s' '%s' '%s' SAVE", message, Mask, path);
		else
			cmd.format("python3 filechooser_wx.py '%s' '%s' '%s' OPEN", message, Mask, path);
	}
	else 
	{
		if(bCreate)
			cmd.format("python filechooser.py '%s' '%s' '%s' SAVE", message, Mask, path);
		else
			cmd.format("python filechooser.py '%s' '%s' '%s' OPEN", message, Mask, path);
	}
#endif
	printf("%s\n", cmd.ptr());
	FILE *lsofFile_p = popen(cmd.ptr(), "r");

	if (!lsofFile_p)
	{
		return TString();
	}

	char buffer[1024];
	char *line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
	pclose(lsofFile_p);

	buffer[strlen(buffer)-1]=0;
	if (strcmp(buffer, "Closed, no files selected")==0)
		return TString();
	return TString(buffer);
}
#endif
static TString _FlChooseFile_fltk(const char* message, const char* path, const char* Mask, bool bCreate)
{
	int mask;
	if(bCreate)
		mask=Fl_File_Chooser::SINGLE | Fl_File_Chooser::CREATE;
	else
		mask=Fl_File_Chooser::SINGLE ;

#if defined( _MSC_VER) 
	char szCurrDirectory[512];
	GetCurrentDirectory(512, szCurrDirectory);
#endif
	Fl_File_Chooser fc(path, Mask, mask, message);
	fc.show();
	while (fc.visible())
		Fl::wait();

	if (fc.count() > 0)
	{
		printf("%s", fc.value(1));
#ifdef _MSC_VER
		SetCurrentDirectory(szCurrDirectory);
#endif
		return TString(fc.value(1));
	}

	return TString();
}
#endif
TString FlChooseFile(const char* message, const char* path, const char* Mask, bool bCreate)
{
#ifdef NO_GUI
  return TString();
#else
#if defined(_MSC_VER) 
  // use native file chooser on windows
#ifdef USE_NATIVE_FILE_CHOOSER

	TString szCurrDirectory=_getcwd();
	printf("curr:%s\n", szCurrDirectory.ptr());


	Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
	if(bCreate)
		chooser->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	else
		chooser->type(Fl_Native_File_Chooser::BROWSE_FILE);   // let user browse a single file

	chooser->directory(path);
	chooser->title(message);        // optional title
	chooser->filter(Mask);			// optional filter
	TString fn;
	switch ( chooser->show() ) {
		case -1:    // ERROR
		fprintf(stderr, "*** ERROR show() failed:%s\n", chooser->errmsg());
		break;
		case 1:     // CANCEL
		fprintf(stderr, "*** CANCEL\n");
		break;
		default:    // USER PICKED A FILE
		fn=chooser->filename();
		break;
	}

	_chdir(szCurrDirectory);
/*
	TString pdir=sz1::parentDirectory(szCurrDirectory);
	pdir.replace('\\', '/');
	// abs to relative.
	if(fn.left(pdir.length()).toUpper()==pdir.toUpper())
	{
		fn="../"+fn.right(pdir.length()*-1);
	}*/
	return fn;
#else // 
	return _FlChooseFile_fltk(message, path, Mask, bCreate);
#endif
#else // !defined(_MSC_VER)
#ifdef __APPLE__
	if(bCreate)
		return _FlChooseFile_fltk(message, path, Mask, bCreate);
#endif
	return _FlChooseFile_python(message, path, Mask, bCreate);
#endif
#endif
}


void Fl_Callee::connect(Fl_Widget* pWidget, int callBackID)
{
	// Auto-size
	if(pWidget->h()<=20 && pWidget->labelsize()>11)
		pWidget->labelsize(11);

	switch(callBackID)
	{
	case 0:
	pWidget->callback(cbFunc0, (void*)this);
	break;
	case 1:
	pWidget->callback(cbFunc1, (void*)this);
	break;
	case 2:
	pWidget->callback(cbFunc2, (void*)this);
	break;
	case 3:
	pWidget->callback(cbFunc3, (void*)this);
	break;
	case 4:
	pWidget->callback(cbFunc4, (void*)this);
	break;
	}
}
	
void Fl_Callee::cbFunc0(Fl_Widget * pWidget, void *data)
{
	((Fl_Callee*)pWidget->user_data())->onCallback(pWidget, 0);
}
void Fl_Callee::cbFunc1(Fl_Widget * pWidget, void *data)
{
	((Fl_Callee*)pWidget->user_data())->onCallback(pWidget, 1);
}
void Fl_Callee::cbFunc2(Fl_Widget * pWidget, void *data)
{
	((Fl_Callee*)pWidget->user_data())->onCallback(pWidget, 2);
}
void Fl_Callee::cbFunc3(Fl_Widget * pWidget, void *data)
{
	((Fl_Callee*)pWidget->user_data())->onCallback(pWidget, 3);
}
void Fl_Callee::cbFunc4(Fl_Widget * pWidget, void *data)
{
	((Fl_Callee*)pWidget->user_data())->onCallback(pWidget, 4);
}

FlMenu::FlMenu()	
{
	m_nMenu=0;
	m_pMenu=NULL;
}

FlMenu::~FlMenu()
{
	if(m_nMenu)
		delete m_aMenuItem;
	// m_pMenu will be automatically deleted in FLTK way.
}

void FlMenu::init(Fl_Menu_* pMenu, int n)
{
	init(pMenu);
	size(n);
}

void FlMenu::init(Fl_Menu_* pMenu)
{
	m_pMenu=pMenu;

	if(m_pMenu->h()<=20) 
	{
		//o->labelsize(11);
		m_pMenu->textsize(11);
	}
	if(m_nMenu)
		delete m_aMenuItem;

}

void FlMenu::initChoice(int x, int y, int w, int h, const char* title)
{
	Fl_Choice* o=new Fl_Choice(x, y, w, h, title);
	init(o);
}

void FlMenu::size(int n)
{
	if(m_nMenu) 
		delete m_aMenuItem;

	m_aMenuItem=new Fl_Menu_Item[n+1];
	memset(m_aMenuItem, 0, sizeof(Fl_Menu_Item)*(n+1));
	m_aLabel.init(n);
	m_nMenu=n;
}

int FlMenu::size()	{return m_aLabel.size();}
void FlMenu::item(int item, const char* text, int shortcut_,  int user_data_,  int flags,  uchar labeltype_,  uchar labelfont_, uchar labelsize_,  unsigned labelcolor_)
{
	if(item >=m_nMenu)
	{
		printf("Error! Too many MenuItem \n");
		ASSERT(0);
		return;
	}
	if(text==0)
	{
		m_aLabel[item].format("__submenu_ended");

		memset(&m_aMenuItem[item], 0, sizeof(Fl_Menu_Item));
	}
	else
	{
		m_aLabel[item].format("%s",text);
		m_aMenuItem[item].label(m_aLabel[item].ptr());
		m_aMenuItem[item].shortcut(shortcut_);
		m_aMenuItem[item].user_data((void*)user_data_);
		m_aMenuItem[item].flags=flags;
		m_aMenuItem[item].labeltype((Fl_Labeltype)labeltype_);
		m_aMenuItem[item].labelfont(labelfont_);
		m_aMenuItem[item].labelsize(labelsize_);
		m_aMenuItem[item].labelcolor(labelcolor_);
	}

	if(item==m_nMenu-1)
	{
		m_pMenu->menu(m_aMenuItem);
	}
}
#include "FL/Fl_Scroll.H"

class Fl_Vert_Scroll : public Fl_Scroll
{
public:

  Fl_Vert_Scroll(int X,int Y,int W,int H,const char*l=0):Fl_Scroll(X,Y,W,H,l){type(Fl_Scroll::VERTICAL);}
  
// Insure the scrollbars are the last children:
void fix_scrollbar_order2() {
  Fl_Widget** a = (Fl_Widget**)array();
  if (a[children()-1] != &scrollbar) {
    int i,j; for (i = j = 0; j < children(); j++)
      if (a[j] != &hscrollbar && a[j] != &scrollbar) a[i++] = a[j];
    a[i++] = &hscrollbar;
    a[i++] = &scrollbar;
  }
}
  virtual	void resize(int X, int Y, int W, int H)
  {
	  fix_scrollbar_order2();
	  // move all the children:
	  Fl_Widget*const* a = array();
	  for (int i=children()-2; i--;) {
		Fl_Object* o = *a++;
		o->position(o->x()+X-x(), o->y()+Y-y());
		o->size(o->w()+W-w(), o->h());
	  }
	  Fl_Widget::resize(X,Y,W,H);
  }


};

int Fl_Fast_Value_Slider ::handle(int ev)
{
	if(ev==FL_ENTER)
		take_focus();

	switch(ev)
	{
	case FL_FOCUS:
	case FL_UNFOCUS:
		return 1;
	}

	if(ev== FL_KEYBOARD)
	{
		int inc;

		printf("key up %c\r", Fl::event_key());
		if(Fl::event_ctrl())
			inc=10;
		else if(Fl::event_alt())
			inc=100;
		else inc=1;

		switch (Fl::event_key()) 
		{
		case '-':
			//if (horizontal()) return 0;
			handle_drag(clamp(increment(value(),-1*inc)));
			handle_release();
			return 1;
		case '+':
			//if (horizontal()) return 0;
			handle_drag(clamp(increment(value(),1*inc)));
			handle_release();
			return 1;
		default:
			return 0;
		}
    }

	return Fl_Value_Slider::handle(ev);
}


Fl_Hor_Slider2::Fl_Hor_Slider2(int x,int y,int w,int h, const char *l )
:Fl_Slider(x+40,y, w-40, h, l)
{
	type(FL_HOR_SLIDER);
	range(0, 1.0);
	step(0.05);
	value(1.0);
	align(FL_ALIGN_LEFT);
}

#include "../image/Image.h"
#include "../image/ImagePixel.h"
void fl_draw_CImage(const CImage& imagee, const TRect& sourceRect, int x, int y)
{

	CImage& image=(CImage&)imagee;
	CImagePixel ptr(&image);

	TRect srcRect(sourceRect.left, sourceRect.top, sourceRect.right, sourceRect.bottom);

	if(srcRect.left<0)
	{
		x-=srcRect.left;
		srcRect.left=0;
	}

	if(srcRect.top<0)
	{
		y-=srcRect.top;
		srcRect.top=0;
	}

	if(srcRect.right>image.GetWidth())
		srcRect.right=image.GetWidth();

	if(srcRect.bottom>image.GetHeight())
		srcRect.bottom=image.GetHeight();

	if(srcRect.Width()<=0 || srcRect.Height()<=0) return;

	static uchar *buffer=NULL;
	static int bufferWidth=0;

	int bufferSize=srcRect.Width()*3;
	if(bufferWidth<bufferSize)
	{
		delete buffer;
		buffer=new uchar[bufferSize];
		bufferWidth=bufferSize;
	}

	for(int yy=0; yy<srcRect.Height(); yy++)
	{
		CPixelRGB8* pColor=&ptr[srcRect.top+yy][srcRect.left];

		int w=srcRect.Width()*3;
		for(int i=0; i<w; i+=3)
		{			
			buffer[i]=pColor->R;
			buffer[i+1]=pColor->G;
			buffer[i+2]=pColor->B;
			pColor++;
		}

		fl_draw_image(buffer, x, y+yy, srcRect.Width(), 1);
	}

}
int FlGenShortcut(const char* s)
{
#ifdef NO_GUI
	return 0;
#else
	TString ss(s);

	TString token;
	int shortc=0;
	for(int start=0, end=ss.length(); start!=end; )
		{
			ss.token(start, "+", token) ;

			if(token=="FL_ALT")
				shortc+=FL_ALT;
			else if(token=="FL_CTRL")
				shortc+=FL_CTRL;
			else
				shortc+=token[0];
		}
	return shortc;
#endif
}

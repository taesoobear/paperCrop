//
// RightPanel.cpp 
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
#include "RightPanel.h"
#include "PDFwin.h"
#include "utility/operatorString.h"
#ifdef USE_LUABIND
#include "WrapperLua/BaselibLUA.h"
#include "WrapperLua/MainlibLUA.h"
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
//#include <luabind/policy.hpp>
#include <luabind/operator.hpp>
using namespace luabind;
#else 
#include "WrapperLua/LUAwrapper.h"
#include "WrapperLua/OR_LUA_Stack.h"
#include "luna_baselib.h"
#include "luna_mainlib.h"
void Register_mainlib(lua_State* L);
void Register_baselib(lua_State* L);
#endif
#include "PDFWriter.h"
#include "utility/FltkAddon.h"
#include <iostream>

void reflow(CImage& inout, int desired_width, int min_gap, int max_gap, int thr_white, double right_max_margin, int figure_min_height);
void trimVertSpaces(CImage& inout, int min_gap, int max_gap, int thr_white) ;




TString processOption(const char* option)
{
	TString opt=option;
	opt.replace(") ", ")\n");
	opt.replace(" (", " \n(");
	return opt;
}

TString unprocessOption(const char* option)
{
	TString opt=option;
	opt.replace(")\n", ") ");
	opt.replace(" \n(", " (");
	return opt;
}

static int add_file_and_line(lua_State* L)
{
	luaL_dostring(L, "dbg.traceBack()");
	luaL_dostring(L, "dbg.console()");
	return 1;
}
bool CreateDirectory(const char *PathToCreate);
bool DeleteAllFiles(const char* path, const char* mask,bool bConfirm);
#ifdef USE_LUABIND
static LUAwrapper* L=NULL;
#else
static lua_State* L=NULL;
#endif
static void _initLuaEnvironment(RightPanel* win)
{

#ifdef USE_LUABIND
	delete(L);
	L=new LUAwrapper();
	luabind::set_pcall_callback(&add_file_and_line);
	addMainlibModule(L->L);
	// export member functions of PDFwin for use in LUA script.
	// to understand the following codes, please refer to "luabind" manual.
	module(L->L)[
		def("reflow", &reflow),
		def("trimVertSpaces", &trimVertSpaces),			
		def("CreateDirectory", (bool (*)(const char*))&CreateDirectory),
		def("DeleteAllFiles", &DeleteAllFiles),
		class_<PDFwin>("PDFwin")
			.def_readonly("filename", &PDFwin::_filename)
			.def_readonly("currPage", &PDFwin::mCurrPage)
			.def("getRectImage_width", &PDFwin::getRectImage_width)
			.def("load", &PDFwin::load)
			.def("getDPI_width", &PDFwin::getDPI_width)
			.def("getDPI_height", &PDFwin::getDPI_height)
			.def("getRectImage_height", &PDFwin::getRectImage_height)
			.def("setStatus", &PDFwin::setStatus)
			.def("getNumPages", &PDFwin::getNumPages)
			.def("getNumRects", &PDFwin::getNumRects)
			.def("setCurPage", &PDFwin::setCurPage)
			.def("pageChanged", &PDFwin::pageChanged)
			.def("redraw", &PDFwin::redraw)
			.def("deleteAllFiles", &PDFwin::deleteAllFiles)
			.def("deleteAllFilesWithoutConfirm", &PDFwin::deleteAllFilesWithoutConfirm),
		class_<PDFWriter>("PDFWriter")
			.def(constructor<>())
			.def("init", &PDFWriter::init)
			.def("addPage", &PDFWriter::addPage)
			.def("addPageColor", &PDFWriter::addPageColor)
			.def("save", &PDFWriter::save)
			.def("isValid",&PDFWriter::isValid)
			];


	L->setRef<PDFwin>("win", *(win->mPDFwin));
	L->setRef<FlLayout>("panel", *win);
#else
	if(L) lua_close(L);
	L=lua_open();
	luaopen_base(L);
	luaL_openlibs(L);
	Register_baselib(L);
	Register_mainlib(L);
	lunaStack ls(L);
	ls.set<PDFwin>("win", (win->mPDFwin));
	ls.set<FlLayout>("panel", win);
#endif
	

	
}

#ifdef USE_LUABIND
#define CATCH_LUABIND_ERROR(x, y) catch(luabind::error& e)\
	{\
		std::cout <<"lua error"<<x<<","<<e.what()<<"\n";\
		int n=lua_gettop(e.state());\
		Msg::msgBox("lua error %s", lua_tostring(e.state(), n));\
		ASSERT(0);\
		luaL_dostring(L->L, "dbg.traceBack()");\
		luaL_dostring(L->L, "dbg.console()");\
		_releaseScript(L, y);\
		L=NULL;\
    }
static void _releaseScript(LUAwrapper* L, FlLayout& l)
{
	if(L)
	{
		luabind::call_function<void>(L->L, "dtor");
		delete L;
	}
}

#else
static void _releaseScript(lua_State* L, FlLayout& l)
{
	if(L)
	{
		lunaStack l(L);
		l.getglobal("dtor");
		l.call(0,0);
		lua_close(L);
		L=NULL;
	}
}
static void handleLUAerror(lua_State* L)
{
	printf("handleLUAerror:\n");
	luna_printStack(L);
	luaL_dostring(L, "dbg.traceBack()");
	luaL_dostring(L, "dbg.console()");
}
#endif
extern TString g_arg;
static void _loadScript(RightPanel* win, const char* script)
{
	_initLuaEnvironment(win);
#ifdef USE_LUABIND
	if(L)
	{
		try
		{
			if (g_arg.length()>0)
				L->dostring(g_arg.ptr());
			L->dofile(script);
		}
		CATCH_LUABIND_ERROR("_loadScript", *win)
	}

	if (L)
	{
		try
		{
			luabind::call_function<void>(L->L, "ctor");
		}
		CATCH_LUABIND_ERROR("ctor", *win)
	}
#else
	if(L)
	{
		if (g_arg.length()>0)
		{
			if (luaL_dostring(L, g_arg.ptr())==1) handleLUAerror(L);
		}
		if(luaL_dofile(L, script)==1)
			handleLUAerror(L);
		lunaStack l(L);
		l.getglobal("ctor");
		l.call(0,0);
	}
#endif
}

RightPanel::RightPanel(int x, int y, int w, int h, PDFwin* pdfwin)
:FlLayout(x,y, w, h)
{
	mPDFwin=pdfwin;
	mPDFwin->mLayout=this;
	_loadScript(this, "main.lua");
}

RightPanel::~RightPanel(void)
{
	_releaseScript(L, *this);
}


#ifndef NO_SHOW_WIN
//#include <FL/Fl_Native_File_Chooser.H>
#endif
void RightPanel::onCallback(FlLayout::Widget const& w, Fl_Widget * pWidget, int userData)
{
#ifdef USE_LUABIND
	try
	{
		if(L)
		{
			bool res=luabind::call_function<bool>(L->L, "onCallback", w, userData);
			if (res) return;
		}
	}
	CATCH_LUABIND_ERROR("onCallBack", *this)
#else
		if(L)
		{
			lunaStack l(L);
			l.getglobal("onCallback");
			l.push<FlLayout::Widget>(&w);
			l<<userData;
			l.call(2,1);
			bool res;
			l>>res;
			if(res) return;
		}
#endif
	if(w.mId=="update")
	{
		mPDFwin->pageChanged();
		mPDFwin->redraw();
	}
	else if(w.mId=="Use automatic segmentation")
	{
		if(w.checkButton()->value())
		{
			findLayout("Automatic segmentation")->activate();
			redraw();
		}
		else
		{
			findLayout("Automatic segmentation")->deactivate();
			redraw();
		}
	}
	else if(w.mId=="Option")
	{
		TString fn=FlChooseFile("Choose option", "script","*.lua");
		if(fn.length())
		{
			TString ff, dir;
			ff=sz1::filename(fn, dir);
			find<Fl_Input>("Option_Input")->value(processOption(ff.left(-4)));
			redraw();
		}

	}
	else if(w.mId=="Load a PDF file")
	{
#ifndef NO_SHOW_WIN
		/*
		Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
		chooser->type(Fl_Native_File_Chooser::BROWSE_FILE);   // let user browse a single file
		chooser->title("Open a file");                        // optional title
		//chooser->preset_file("/var/tmp/somefile.txt");        // optional filename preset
		chooser->filter("PDF Files\t*.pdf");                 // optional filter
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
		*/
		TString fn=FlChooseFile("Choose a PDF file", "pdf", "*.pdf");
		if(fn.length())
		{
			mPDFwin->load(fn);

		}
#endif
	}
	else if(w.mId=="Batch process")
	{
		/*
#ifndef NO_SHOW_WIN
		Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
		chooser->type(Fl_Native_File_Chooser::BROWSE_MULTI_FILE);   // let user browse a single file
		chooser->title("Open files");                        // optional title
		//chooser->preset_file("/var/tmp/somefile.txt");        // optional filename preset
		chooser->filter("PDF Files\t*.pdf");                 // optional filter
		TStrings fn;
		switch ( chooser->show() ) {
			case -1:    // ERROR
			fprintf(stderr, "*** ERROR show() failed:%s\n", chooser->errmsg());
			break;
			case 1:     // CANCEL
			fprintf(stderr, "*** CANCEL\n");
			break;
			default:    // USER PICKED A FILE
				{
					fn.resize(chooser->count());
					for (int n = 0; n < chooser->count(); n++ ) 
      				fn[n]=chooser->filename(n);
				}
			break;
		}
		//TString fn=fl_file_chooser("Choose a PDF file", "*.pdf", NULL);
		if(fn.size())
		{

			for(int i=0; i<fn.size(); i++)
			{
				mPDFwin->load(fn[i]);
				onCallback(findWidget("Process all pages"), w.widgetRaw(), 0);
			}
		}
#endif
*/
	}
}

#ifndef _STDAFX_H_
#define _STDAFX_H_
//
// stdafx.H 
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


// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifdef _MSC_VER
#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


// Windows Header Files:
#include <windows.h>
#include <tchar.h>

#endif
// C RunTime Header Files
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <stdio.h>
#include <stdexcept>


#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

// TODO: reference additional headers your program requires here

#include <iostream>

// disable boost codepage warning.
#pragma warning( disable : 4819)

#ifdef _MSC_VER
// Fltk???dynamic liking??꺼???FL_DLL????파????시?? 
// - mainlibPython????는 경우??링크??러가 ??는경우?????외??고??FL_DLL 비추.
#define FL_DLL 

/*
#ifdef FL_DLL
#ifdef _DEBUG
#pragma comment(lib, "../fltk/test/fltkdlld.lib")
#else
#pragma comment(lib, "../fltk/test/fltkdll.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "fltkd.lib")
#else
#pragma comment(lib, "fltk.lib")
#endif
#endif


#ifdef _DEBUG
#pragma comment(lib, "fltkimagesd.lib")
#pragma comment(lib, "fltkpngd.lib")
#pragma comment(lib, "fltkzd.lib")
#pragma comment(lib, "fltkgld.lib")
#pragma comment(lib, "../lua5.0.2/lib/luad.lib")
#pragma comment(lib, "luabind-0.7/debug/luabind_lib.lib")
#else
#pragma comment(lib, "fltkimages.lib")
#pragma comment(lib, "fltkpng.lib")
#pragma comment(lib, "fltkz.lib")
#pragma comment(lib, "fltkgl.lib")
#pragma comment(lib, "../lua5.0.2/lib/lua.lib")
#pragma comment(lib, "luabind-0.7/release/luabind_lib.lib")
#endif

#pragma comment(lib, "devil/lib/DevIl.lib")
#pragma comment(lib, "devil/lib/Ilu.lib")
*/
#endif
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/x.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Int_Input.H>

#include "utility/TypeString.h"

#ifdef _DEBUG		
#define ASSERT(x) assert(x)
#define RANGE_ASSERT(x) assert(x)
#define VERIFY(x) assert(x)
#else
#define ASSERT(x) ;
#define RANGE_ASSERT(x) do {if(!(x)) throw std::runtime_error("range_error");} while(false);
#define VERIFY(x)	(x)
#endif

namespace Msg
{
	void verify(bool bExpression, const char*, ...);// release??서????어지지 ??는 verify?????고 ????경우 ??용.
	void print(const char*,...);
	void print2(const char*,...);
	void error(const char*,...);
	void msgBox(const char*,...);
	bool confirm(const char*,...);
	void flush();
	void output(const char* key, const char* formatString, ...);
	void outputState(bool bOutput);
	
	class Base
	{
	public:
		Base(){m_bOutput=true;}
		virtual ~Base(){}
		// inherit and implement followings
		virtual void print(const char* msg);
		virtual void print2(const char* msg);
		virtual void flush();
		virtual void error(const char* msg);
		virtual void msgBox(const char* msg);
		virtual void output(const char* key, const char* msg);
		// ??직 구현??됨. ??용??에???yes or no물어보는 기능.
		virtual bool confirm(const char* msg);
		bool m_bOutput;
		
	};
		
	extern Base* g_pMsgUtil;
}

#include "math/mathclass.h"

#define FLTK1
#endif

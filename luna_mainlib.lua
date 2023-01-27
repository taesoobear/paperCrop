
array.pushBack(gen_lua.number_types, 'ushort') 
array.pushBack(gen_lua.number_types, 'uint') 

bindTargetMainLib={
			
	classes={
		{
			name='SelectionRectangle',
			decl='class SelectionRectangle;',
			ctors={'()'},
			wrapperCode=[[
			static double left(SelectionRectangle& w){ return w.p1.x;}
			static double top(SelectionRectangle& w){ return w.p1.y;}
			static double right(SelectionRectangle& w){ return w.p2.x;}
			static double bottom(SelectionRectangle& w){ return w.p2.y;}
			]],
			staticMemberFunctions={[[
			static double left(SelectionRectangle& w);
			static double top(SelectionRectangle& w);
			static double right(SelectionRectangle& w);
			static double bottom(SelectionRectangle& w);
			]]}
		},
		{
			name='PDFwin',
			decl='class PDFwin;',
			properties={'std::string _filename @ filename', 'int mCurrPage @ currPage'},
			memberFunctions={[[

			void getRectSize(int pageNo, int rectNo, SelectionRectangle& rect);
			void getRectImage_width(int pageNo, int rectNo, int width, CImage& image);
			void getRectImage_height(int pageNo, int rectNo, int height, CImage& image);
			void load(const char* filename);
			double getDPI_height(int pageNo, int rectNo, int height);
			double getDPI_width(int pageNo, int rectNo, int width);
			void setStatus(const char* o);
			int getNumPages();
			int getNumRects();
			double pageCropWidth(int pageNo);
			double pageCropHeight(int pageNo);
			void setCurPage(int pageNo);
			void pageChanged();
			void redraw();
			void deleteAllFiles();
			void deleteAllFilesWithoutConfirm();

			]]}
		},
		{
			name='PDFWriter',
			decl='class PDFWriter;',
			ctors={'()'},
			memberFunctions={[[
			bool init();
			void addPage(CImage const& pageImage, int bpp); // currently only 4 and 8 is supported
			void addPageColor(CImage const& pageImage);
			void save(const char* fname);
			bool isValid()
			]]}
		},
		{
			name='__luna.worker',
			className='OR::LUAWrap::Worker',
			wrapperCode=[[
			static int __call(lua_State *L)
			{
				lunaStack lua(L);
				OR::LUAWrap::Worker* self=lua.check<OR::LUAWrap::Worker>();
				std::string workName;
				lua>>workName;

				TString w(workName.c_str());
				OR::LUAStack s(L);
				s.mCurrArg=3;
				return self->work(w,s);
			}
			]],
			custumFunctionsToRegister={'__call'},
		},
		{
			name='GlobalUI',
			inheritsFrom='OR::LUAWrap::Worker',
		},
		{
			name='FlLayout',
			inheritsFrom='OR::LUAWrap::Worker',
			memberFunctions={[[
				void begin()
				FlLayout* layout(int n);
				FlLayout* findLayout(const char* id)
				void create(const char* type, const char* id, const char* title);
				void create(const char* type, const char* id);
				void create(const char* type, const char* id, const char* title, int startSlot, int endSlot, int height);
				void resetToDefault()
				void create(const char* type, const char* id, const char* title, int startSlot, int endSlot);
				void create(const char* type, const char* id, const char* title, int startSlot);
				void newLine(); 
				void embedLayout(FlLayout* childLayout, const char* id, const char* title);
				void setLineSpace(int l);
				void setHorizSpace(int h);
				void setWidgetHeight(int h);
				void setWidgetPos(int startSlot, int endSlot); // guideline 따라 나누어져있는 영역에서 얼만큼 차지할지.
				void setUniformGuidelines(int totalSlot); 
				void updateLayout();
				void redraw()
				int minimumHeight();
				FlLayout::Widget& widgetRaw(int n); @ widget
				int widgetIndex(const char* id);
				FlLayout::Widget& findWidget(const char* id);
				void callCallbackFunction(FlLayout::Widget& w);
						 ]]}
		},
		{
			name='FlLayout.Widget',
			wrapperCode=[[
			static void checkButtonValue(FlLayout::Widget& w, int value)
			{
				w.checkButton()->value(value);
			}
			static void checkButtonValue3(FlLayout::Widget& w, bool value)
			{
				w.checkButton()->value(value);
			}
			static bool checkButtonValue2(FlLayout::Widget& w)
			{
				return w.checkButton()->value();
			}
			static void menuSize(FlLayout::Widget& w,int nsize)
			{
				w.menu()->size(nsize);
			}
			static void menuItem(FlLayout::Widget& w,int i, const char* title)
			{
				w.menu()->item(i, title);
			}
			static void menuItem2(FlLayout::Widget& w,int i, const char* title, const char* shortc)
			{
				w.menu()->item(i, title, FlGenShortcut(shortc));
			}
			static void menuValue(FlLayout::Widget& w, int v)
			{
				w.menu()->value(v);
				w.menu()->redraw();
			}
			static std::string menuText(FlLayout::Widget& w, int v)
			{
#ifndef NO_GUI
				return w.menu()->text(v);
#else
				return "";
#endif
					
			}
			static std::string menuText2(FlLayout::Widget& w)
			{
#ifndef NO_GUI
				return w.menu()->text();
#else
				return "";
#endif
			}
			static int menuValue2(FlLayout::Widget& w)
			{
				return w.menu()->value();
			}
			static void sliderValue(FlLayout::Widget& w, double v)
			{
				w.slider()->value(v);
			}
			static void sliderStep(FlLayout::Widget& w, double v)
			{
#ifndef NO_GUI
				w.slider()->step(v);
#endif
			}
			static void sliderRange(FlLayout::Widget& w, double v1, double v2)
			{
#ifndef NO_GUI
				w.slider()->range(v1, v2);
#endif
			}
			static double sliderValue2(FlLayout::Widget& w)
			{
				return w.slider()->value();
			}
			static void buttonShortcut(FlLayout::Widget& w, const char* s)
			{
#ifndef NO_GUI				
				w.button()->shortcut(FlGenShortcut(s));
				w.button()->tooltip(s);
#endif
			}
			static void buttonTooltip(FlLayout::Widget& w, const char* s)
			{
#ifndef NO_GUI				
				w.button()->tooltip(s);
#endif
			}
			static void buttonSetLabel(FlLayout::Widget& w, const char* s)
			{
#ifndef NO_GUI				
				w.button()->copy_label(s);
#endif
			}
			static void deactivate(FlLayout::Widget& w)
			{
#ifndef NO_GUI				
				w.widget<Fl_Widget>()->deactivate();
#endif
			}
			static void activate(FlLayout::Widget& w)
			{
#ifndef NO_GUI				
				w.widget<Fl_Widget>()->activate();
#endif
			}
			static const char* id(FlLayout::Widget& w)
			{
				return w.mId;
			}
			static int browserSize(FlLayout::Widget& w)
			{
#ifndef NO_GUI
				Fl_Browser* browser=w.widget<Fl_Browser>();
				return browser->size();
#else
				return 0;
#endif
			}
			static bool browserSelected(FlLayout::Widget& w, int i)
			{
#ifndef NO_GUI
				Fl_Browser* browser=w.widget<Fl_Browser>();
				return browser->selected(i);
#else
				return false;
#endif
			}

			static void browserDeselect(FlLayout::Widget& w)
			{
#ifndef NO_GUI
				Fl_Browser* browser=w.widget<Fl_Browser>();
				browser->deselect();
#endif
			}
			static void browserSelect(FlLayout::Widget& w,int i)
			{
#ifndef NO_GUI
				Fl_Browser* browser=w.widget<Fl_Browser>();
				browser->select(i);
#endif
			}
			static void browserAdd(FlLayout::Widget& w, const char* name)
			{
#ifndef NO_GUI
				Fl_Browser* browser=w.widget<Fl_Browser>();
				browser->add(name,NULL);
#endif
			}
			static void browserClear(FlLayout::Widget& w)
			{
#ifndef NO_GUI
				Fl_Browser* browser=w.widget<Fl_Browser>();
				browser->clear();
#endif
			}
			static void inputValue1(FlLayout::Widget& w, const char* text)
			{
#ifndef NO_GUI
				Fl_Input* input=w.widget<Fl_Input>();
				input->value(text);
#endif
			}
			static std::string inputValue2(FlLayout::Widget& w)
			{
				std::string str;
#ifndef NO_GUI
				Fl_Input* input=w.widget<Fl_Input>();
				str=input->value();
#endif
				return str;
			}
static void inputType(FlLayout::Widget& w, const char* str)
			{
#ifndef NO_GUI
				if (TString(str)=="FL_MULTILINE_OUTPUT")
				{
					Fl_Input* ip=w.widget<Fl_Input>();
					ip->type(FL_MULTILINE_OUTPUT);
				}
#endif
			}
			]],
			staticMemberFunctions={[[
			static void checkButtonValue(FlLayout::Widget& w, int value)
			static bool checkButtonValue2(FlLayout::Widget& w) @ checkButtonValue
			static void checkButtonValue3(FlLayout::Widget& w, bool value) @ checkButtonValue
			static void menuSize(FlLayout::Widget& w,int nsize)
			static void menuItem(FlLayout::Widget& w,int i, const char* title)
			static void menuItem2(FlLayout::Widget& w,int i, const char* title, const char* shortc) @ menuItem
			static void menuValue(FlLayout::Widget& w, int v)
			static std::string menuText(FlLayout::Widget& w, int v)
			static std::string menuText2(FlLayout::Widget& w) @ menuText
			static int menuValue2(FlLayout::Widget& w) @ menuValue
			static void sliderValue(FlLayout::Widget& w, double v)
			static void sliderStep(FlLayout::Widget& w, double v)
			static void sliderRange(FlLayout::Widget& w, double v1, double v2)
			static double sliderValue2(FlLayout::Widget& w) @ sliderValue
			static void buttonShortcut(FlLayout::Widget& w, const char* s)
			static void buttonTooltip(FlLayout::Widget& w, const char* s)
			static void buttonSetLabel(FlLayout::Widget& w, const char* s)
			static void deactivate(FlLayout::Widget& w)
			static void activate(FlLayout::Widget& w)
			static const char* id(FlLayout::Widget& w)
			static int browserSize(FlLayout::Widget& w)
			static bool browserSelected(FlLayout::Widget& w, int i)
			static void browserDeselect(FlLayout::Widget& w)
			static void browserSelect(FlLayout::Widget& w,int i)
			static void browserAdd(FlLayout::Widget& w, const char* name)
			static void browserClear(FlLayout::Widget& w)
			static void inputValue1(FlLayout::Widget& w, const char* text) @ inputValue
			static std::string inputValue2(FlLayout::Widget& w) @ inputValue
			static void inputType(FlLayout::Widget& w, const char* str)
			]]}
		},
	},
	modules={
		{
			namespace='util',
			functions={[[
				void Msg::msgBox(const char*) @ msgBox
			]]}
		},
		{
			namespace='Fltk',
			wrapperCode=[[
			static std::string chooseFile(const char* msg, const char* path, const char* mask, bool bCreate) {
				#ifndef NO_GUI
				TString temp=FlChooseFile(msg,path,mask,bCreate).ptr();
				if(temp.length()){
					return std::string(temp.ptr());
				}
				#endif
				return std::string("");
			}

			static bool ask(const char* msg)
			{ 
				#ifndef NO_GUI

				return fl_ask("%s", msg);
				#else
				return true;
				#endif
			}

			]],
			functions={[[
			static std::string chooseFile(const char* msg, const char* path, const char* mask, bool bCreate)
			static bool ask(const char* msg)
			]]}
		},
		{
			namespace='sop',
			functions={[[
			m_real sop::map(m_real t, m_real min, m_real max, m_real v1, m_real v2)
			m_real sop::clampMap(m_real t, m_real min, m_real max, m_real v1, m_real v2)
			]]}
		},
		{
			namespace='_G',
			functions={[[
			void reflow(CImage& inout, int desired_width, int min_gap, int max_gap, int thr_white, double right_max_margin, int figure_min_height);
			void trimVertSpaces(CImage& inout, int min_gap, int max_gap, int thr_white) ;
			bool CreateDirectory(const char *PathToCreate);
			bool DeleteAllFiles(const char* path, const char* mask,bool bConfirm);
			]]}
		},
	}
}


function generateMainLib() 
	-- declaration
	write([[
	class GlobalUI;
	#include "WrapperLua/OR_LUA_Stack.h"
	#include <iostream>
	#include <string>
	]])

	writeHeader(bindTargetMainLib)
	flushWritten(	'../luna_mainlib.h')
	-- write function can be used liberally.
	write(
	[[
	#include "stdafx.h"
	#include "math/Operator.h"
	#include "utility/FlLayout.h"
	#include "utility/FltkAddon.h"
	#include <FL/fl_ask.H>
	#include <FL/Fl_Browser.H>
	#ifdef _MSC_VER
	typedef unsigned short ushort;
	#endif

	]]
	)
	writeIncludeBlock(true)
	write('#include "luna_baselib.h"')
	write('#include "luna_mainlib.h"')
	write([[
	#include "PDFwin.h"
	#include "PDFWriter.h"
	void reflow(CImage& inout, int desired_width, int min_gap, int max_gap, int thr_white, double right_max_margin, int figure_min_height);
	void trimVertSpaces(CImage& inout, int min_gap, int max_gap, int thr_white) ;
	bool CreateDirectory(const char *PathToCreate);
	bool DeleteAllFiles(const char* path, const char* mask,bool bConfirm);
	]])
	writeDefinitions(bindTargetMainLib, 'Register_mainlib') -- input bindTarget can be non-overlapping subset of entire bindTarget 
	flushWritten('../luna_mainlib.cpp') -- write to cpp file only when there exist modifications -> no-recompile.
end


#include "stdafx.h"
#include "BaselibLUA.h"
#include "../utility/FlLayout.h"
#include "../utility/FltkAddon.h"

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
#include <luabind/object.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/copy_policy.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/discard_result_policy.hpp>
#include <luabind/dependency_policy.hpp>
#include <luabind/luabind.hpp>
#include <luabind/class_info.hpp> 


#include <FL/fl_ask.H>
#include <FL/Fl_Browser.H>

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
using namespace luabind;


int addItemRecursive(int curItem, bool bSubMenu, FlMenu & menu, luabind::object const& ll)
{
	for(luabind::iterator i(ll), end; i!=end; ++i)
		{
			if(luabind::type(*i)!=LUA_TTABLE)
				{
					const char* menuTitle=luabind::object_cast<const char*>(*i);
					if(bSubMenu )
						menu.item(curItem, menuTitle, 0,0, FL_SUBMENU);
					else
						menu.item(curItem, menuTitle);

					curItem++;
				}
			else
				{
					curItem=addItemRecursive(curItem, true, menu, *i);
					menu.item(curItem, 0);	// submenu�ݱ�.
					curItem++;
				}

			bSubMenu=false;
		}
	return curItem;
}


void addMainlibModule(lua_State* L)
{
	// FlLayout
	{
		Fl_Widget* (FlLayout::*create1)(const char* type, const char* id, const char* title)=&FlLayout::create;
		Fl_Widget* (FlLayout::*create2)(const char* type, const char* id, const char* title, int startSlot, int endSlot, int height)=&FlLayout::create;
		struct FlLayout_
		{
			static void create3(FlLayout* l,const char* type, const char* id, const char* title, int startSlot)
			{
				l->create(type, id, title, startSlot);
			}
			static void create4(FlLayout* l,const char* type, const char* id, const char* title, int startSlot, int endSlot)
			{
				l->create(type, id, title, startSlot, endSlot);
			}
			static void create5(FlLayout* l,const char* type, const char* id)
			{
				l->create(type, id);
			}

		};
		module(L)[
				  class_<FlLayout>("FlLayout")
				  .def("create", create1, discard_result)
				  .def("create", create2, discard_result)
				  .def("create", &FlLayout_::create3)
				  .def("create", &FlLayout_::create4)
				  .def("create", &FlLayout_::create5)
				  .def("newLine", &FlLayout::newLine)
				  .def("setLineSpace", &FlLayout::setWidgetHeight)
				  .def("setWidgetPos", &FlLayout::setWidgetPos)
				  .def("setUniformGuidelines", &FlLayout::setUniformGuidelines)
				  .def("updateLayout", &FlLayout::updateLayout)
				  .def("redraw", &FlLayout::redraw)
				  .def("minimumHeight", &FlLayout::minimumHeight)
				  .def("widget", &FlLayout::widgetRaw)
				  .def("widgetIndex", &FlLayout::widgetIndex)
				  .def("findWidget", &FlLayout::findWidget)
				  .def("resetToDefault", &FlLayout::resetToDefault)
				  .def("layout", &FlLayout::layout)
				  .def("findLayout", &FlLayout::findLayout)
				  .def("callCallbackFunction", &FlLayout::callCallbackFunction)
				  .def("__call", &FlLayout::__call)
			  
				  ];
	}

	// FlLayout::Widget
	{
		struct FlLayoutWidget_
		{
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


		};

		module(L)[class_<FlLayout::Widget>("FlLayoutWidget")
				  .def("id", FlLayoutWidget_::id)
				  .def("checkButtonValue", FlLayoutWidget_::checkButtonValue)
				  .def("checkButtonValue", FlLayoutWidget_::checkButtonValue2)
				  .def("checkButtonValue", FlLayoutWidget_::checkButtonValue3)
				  .def("buttonShortcut", FlLayoutWidget_::buttonShortcut)
				  .def("deactivate", FlLayoutWidget_::deactivate)
				  .def("activate", FlLayoutWidget_::activate)
				  .def("buttonSetLabel", FlLayoutWidget_::buttonSetLabel)
				  .def("buttonTooltip", FlLayoutWidget_::buttonTooltip)
				  .def("menuSize", FlLayoutWidget_::menuSize)
				  .def("menuItem", FlLayoutWidget_::menuItem)
				  .def("menuItem", FlLayoutWidget_::menuItem2)
				  .def("menuValue", FlLayoutWidget_::menuValue)
				  .def("menuValue", FlLayoutWidget_::menuValue2)
				  .def("sliderValue", FlLayoutWidget_::sliderValue)
				  .def("sliderValue", FlLayoutWidget_::sliderValue2)
				  .def("sliderStep", FlLayoutWidget_::sliderStep)
				  .def("sliderRange", FlLayoutWidget_::sliderRange)
				  .def("browserSize", FlLayoutWidget_::browserSize)
				  .def("browserClear", FlLayoutWidget_::browserClear)
				  .def("browserSelected", FlLayoutWidget_::browserSelected)
				  .def("browserDeselect", FlLayoutWidget_::browserDeselect)
				  .def("browserSelect", FlLayoutWidget_::browserSelect)
				  .def("browserAdd", FlLayoutWidget_::browserAdd)
				  .def("inputValue", FlLayoutWidget_::inputValue1)
				  .def("inputValue", FlLayoutWidget_::inputValue2)
				  .def("inputType", FlLayoutWidget_::inputType)
				  .def("menuText", FlLayoutWidget_::menuText)
.def("menuText", FlLayoutWidget_::menuText2)	
				  ];

	}

	// FlMenu
	{
		struct FlMenu_
		{
			static void addItem(FlMenu* pMenu, luabind::object const& table)
			{
				int numMenuItems=LUAwrapper::treeSize(table)-1;
				pMenu->size(numMenuItems);
				int item=addItemRecursive(0, false, *pMenu, table);
				ASSERT(item==numMenuItems);
			}
		};
		module(L)[class_<FlMenu>("FlMenu")
				  .def("addItem", &FlMenu_::addItem)
				  ];
	}

	
		struct FLTK_
		{
			static std::string chooseFile(const char* msg, const char* path, const char* mask, bool bCreate)
			{
#ifndef NO_GUI
				TString temp=FlChooseFile(msg,path,mask,bCreate).ptr();
				if(temp.length())
					return std::string(temp.ptr());
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
		};
		module(L, "Fltk")[
						  def("chooseFile", &FLTK_::chooseFile)
						  ,def("ask", &FLTK_::ask)
						  ];
	}

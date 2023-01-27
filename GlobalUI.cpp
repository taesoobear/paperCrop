#include "stdafx.h"
#include "GlobalUI.h"
#include "utility/FlLayout.h"
#ifdef USE_LUABIND
#include "../MainLib/WrapperLua/BaselibLUA.h"
#include "../MainLib/WrapperLua/MainlibLUA.h"

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/error.hpp>
#else

#include "WrapperLua/LUAwrapper.h"
#endif

#include "luna.h"
#include "luna_baselib.h"
#include "luna_mainlib.h"

#define CATCH_LUABIND_ERROR(x) catch(luabind::error& e)\
	{\
		printf("lua error %s, %s \n", x, e.what());\
		int n=lua_gettop(e.state());\
		Msg::msgBox("lua error %s", lua_tostring(e.state(), n));\
		ASSERT(0);\
		delete _L; _L=NULL;\
    }

#ifndef WIN32
#include <unistd.h>
#endif


void registerFlChoiceWins(lua_State* L);
// singleton class
static FlLayout* _win=NULL;
static GlobalUI* _GUI=NULL;

void registerGlobalUI(LUAwrapper* L);
void Register_baselib(lua_State*L);
void Register_mainlib(lua_State*L);
GlobalUI::GlobalUI(FlLayout* wins,int argc, char* argv[])
{
	_win=wins;
	_GUI=this;
	_param.resize(argc-1);

	for(int i=1; i<argc; i++)
		_param[i-1]=argv[i];

	_L=NULL;
	if(argc>1)
	{
#ifdef USE_LUABIND
		_L=new LUAwrapper();

		addMainlibModule(_L->L);
		registerGlobalUI(_L);
		try{
			_L->setRef<TStrings>("param", _param);
			_L->dofile(_param[0]);
		}
		CATCH_LUABIND_ERROR("ctor")
#else
		_L=new LUAwrapper();
		Register_baselib(_L->L);
		Register_mainlib(_L->L);
		lunaStack ls(_L->L);
		ls.set<TStrings>("param", &_param);
		ls.set<GlobalUI>("GUI", _GUI);
		ls.set<FlLayout>("wins", _win);
		
		//try{
			_L->dofile(_param[0]);
		//}
		//CATCH_LUABIND_ERROR("ctor")
			
#endif
	}
}
int GlobalUI::work(TString const& wn, OR::LUAStack& L)
{
	return 0;
}
#ifdef USE_LUABIND
void GlobalUI::work(const char* _wn, luabind::adl::object const& table)
{
	TString wn=_wn;
	if(wn=="showOgreTraceManager")
	{
		RE::FltkRenderer().onCallback(NULL, Hash("OgreTraceManager"));

	}
#ifndef WIN32
	else if(wn=="chdir"){
		TString temp=luabind::object_cast<const char*>(table);
		chdir(temp.ptr());
	}
	else if(wn=="printcwd"){
		char buf[1000];
		getcwd(buf,1000);
		printf("cwd:%s\n", buf);
	}
#endif

}
#endif
#ifdef USE_LUABIND
void registerGlobalUI(LUAwrapper* L)
{
	registerFlChoiceWins(L->L);
	luabind::module(L->L)
		[
		luabind::class_<GlobalUI>("GlobalUI")

		.def("__call", &GlobalUI::work)

		];
	if(_GUI)
	{
		L->setRef<GlobalUI>("GUI", *_GUI);
		L->setRef<FlChoiceWins>("wins", *_wins);
	}
	else
	{
		printf("??????\n");
	}
}
#endif
GlobalUI* getGlobalUI()
{
	return _GUI;
}
GlobalUI::~GlobalUI()
{
  if(_L) delete _L;
  _L=NULL;
}
#ifdef USE_LUABIND
 using namespace luabind;
void registerFlChoiceWins(lua_State* L)
 {
   struct FlChoiceWins_
   {
     static FlChoiceWins* choiceWin(FlChoiceWins* win, int i)
     {
       return dynamic_cast<FlChoiceWins*>(win->window(i));
     }

     static FlLayout* layout(FlChoiceWins* win, int i)
     {
       return static_cast<FlLayout*>(win->window(i));
     }
   };

   module(L)[
	     class_<FlChoiceWins>("FlChoiceWins")
	     .def("show",(void (FlChoiceWins::*)(int))&FlChoiceWins::show)
	     .def_readwrite("windowNames", &FlChoiceWins::windowNames)
	     .def("choiceWin", &FlChoiceWins_::choiceWin)
	     .def("layout", &FlChoiceWins_::layout)
	     ];
 }
#endif
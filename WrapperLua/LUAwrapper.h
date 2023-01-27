#ifndef _LUAWRAPPER_H_
#define _LUAWRAPPER_H_
#pragma once
#ifdef USE_LUABIND
/*
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>*/
#include <luabind/object.hpp>

#else

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
	//#include "luadebug.h"
}
#include "../luna.h"
#endif


#define BEGIN_LUAWRAPPER_GETVAL try{
#define END_LUAWRAPPER_GETVAL }catch(...){Msg::error("in LUAwrapper::getVal(%s)", szVarname);}
class LUAwrapper
{
public:
	LUAwrapper();
	~LUAwrapper();

	lua_State* L;

	void dofile(const char* script);
	void dostring(const char* string);

	template <class T>
	void setVal(const char* szVarName, T const& val)
	{
#ifdef USE_LUABIND
		luabind::globals(L)[szVarName]=val;
#else
		lunaStack l(L);
		l.set<T>(szVarName, new T(val), LUA_GLOBALSINDEX,true); 
#endif
	}

	// getVal<vector3>("pos", pos)
	template <class T>
	void getVal(const char* szVarname, T& val)
	{
#ifdef USE_LUABIND
		BEGIN_LUAWRAPPER_GETVAL
		val=luabind::object_cast<T>(luabind::globals(L)[szVarname]);
		END_LUAWRAPPER_GETVAL
#else
		lunaStack l(L);
		val=*l.get<T>(szVarname); 
#endif
	}

	// TString name=getValue<const char*>("pos")
	template <class T>
	T getValue(const char* szVarname)
	{
#ifdef USE_LUABIND
		BEGIN_LUAWRAPPER_GETVAL
		T val=luabind::object_cast<T>(luabind::globals(L)[szVarname]);
		return val;
		END_LUAWRAPPER_GETVAL
#else
		lunaStack l(L);
		T val=*l.get<T>(szVarname); 
		return val;
#endif
	}

	// lua: a={1,2,3}
	vector3 getVec3(const char* szVarName);

	/** usage: luabind::call_function
	///////////////////////////////////
	#include <luabind/luabind.hpp>
	#include <luabind/operator.hpp>

	TString out=luabind::call_function<const char*>(L, "function1", "param1");
	*/

	template <class T>
	void setRef(const char* szVarName, T & val)
	{
#ifdef USE_LUABIND
		luabind::globals(L)[szVarName]=&val;
#else
		lunaStack l(L);
		l.set<T>(szVarName, &val, LUA_GLOBALSINDEX,false); 
#endif
	}

#ifdef _MSC_VER
	// std::string& name=getRef<std::string>("name") -> name을 고치면 lua에 반영된다.
	template <class T>
	T& getRef(const char* szVarname)
	{
#ifdef USE_LUABIND
		BEGIN_LUAWRAPPER_GETVAL
		T* ptr=luabind::object_cast<T*>(luabind::globals(L)[szVarname]);
		return *ptr;
		END_LUAWRAPPER_GETVAL
#else
		lunaStack l(L);
		return *l.get<T>(szVarname);
#endif

	}
#endif

#ifdef USE_LUABIND
	// utility functions
	static int arraySize(luabind::object const& ll);
	/* array iterator sample
	for(luabind::iterator i(ll), end; i!=end; ++i)
	{
		const char* item=luabind::object_cast<const char*>(*i);
	}
	*/

	void unpackArray(luabind::object const& ll, TStrings& out);


	// a={{"a","b"}, "c"} -> size of (a)=5 : ( Root, LeftInternalNode, "a", "b", "c")
	//
	//									   where Root=a, LeftInternalNode={"a","b"}
	static int treeSize(luabind::object const& ll);
#endif
};



#endif

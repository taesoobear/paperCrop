#ifndef BASELIB_LUA_H
#define BASELIB_LUA_H
#pragma once
/*
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
	//#include "luadebug.h"
}

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>*/
#include <luabind/object.hpp>

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
		luabind::globals(L)[szVarName]=val;
	}

	// getVal<vector3>("pos", pos)
	template <class T>
	void getVal(const char* szVarname, T& val)
	{
		val=luabind::object_cast<T>(luabind::globals(L)[szVarname]);
	}

	// TString name=getValue<const char*>("pos")
	template <class T> 
	T getValue(const char* szVarName)
	{
		T val=luabind::object_cast<T>(luabind::globals(L)[szVarName]);
		return val;
	}

	template <class T> 
	void setRef(const char* szVarName, T & val)
	{
		luabind::globals(L)[szVarName]=&val;
	}

	// std::string& name=getRef<std::string>("name") -> name을 고치면 lua에 반영된다.
	template <class T>
	T& getRef(const char* szVarname)
	{
		T* ptr=luabind::object_cast<T*>(luabind::globals(L)[szVarname]);
		return *ptr;
	}

	// utility functions
	static int arraySize(luabind::object const& ll);
	// a={{"a","b"}, "c"} -> size of (a)=5 : ( Root, LeftInternalNode, "a", "b", "c") 
	//
	//									   where Root=a, LeftInternalNode={"a","b"}
	static int treeSize(luabind::object const& ll);	
};


#endif

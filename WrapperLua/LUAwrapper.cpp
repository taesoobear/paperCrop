#include "stdafx.h"
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
	//#include "luadebug.h"
}

#include "LUAwrapper.h"
#include "math/Operator.h"




#ifdef USE_LUABIND
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

void addBaselibModule(lua_State* L);
#endif
LUAwrapper::LUAwrapper()
{
	L = lua_open();

#ifdef USE_LUABIND
	luabind::open(L);
	addBaselibModule(L);
#else
	luaL_openlibs(L);
#endif
}

LUAwrapper::~LUAwrapper()
{
	lua_close(L);
}

void LUAwrapper::dofile(const char* pFilename )
{
	printf("pFileName=%s\n", pFilename);

	/*	- simple way without error_checking
	lua_dofile(L, pFilename );*/

	try {
		if (0 != luaL_loadfile(L, pFilename))
		{
			printf("here\n");
			TString errorMsg;
			errorMsg.format("Lua Error - Script Load\nScript Name:%s\nError Message:%s\n", pFilename, luaL_checkstring(L, -1));
			printf("%s\n", errorMsg.ptr());
			ASSERT(0);
			throw std::runtime_error(errorMsg.ptr());
		}

		if (0 != lua_pcall(L, 0, LUA_MULTRET, 0))
		{
			printf("here2\n");
			TString errorMsg;
			errorMsg.format("Lua Error - Script run\nScript Name:%s\nError Message:%s\n", pFilename, luaL_checkstring(L, -1));
			printf("%s\n", errorMsg.ptr());
			ASSERT(0);
			throw std::runtime_error(errorMsg.ptr());
		}
	}	
#ifdef USE_LUABIND
	catch(luabind::error& e)
	{
		printf("lua error %s\n", e.what());
		int n=lua_gettop(e.state());
		Msg::msgBox("lua error %s", lua_tostring(e.state(), n));
		ASSERT(0);
	}
#endif
	catch(std::exception& e)
	{
		Msg::msgBox("c++ error : %s", e.what());
		ASSERT(0);
	}
	catch (char* error)
	{
		Msg::msgBox("%s", error);
	}
	catch(...)
	{
		Msg::msgBox("some error");
		ASSERT(0);
	}

}

void LUAwrapper::dostring(const char* script)
{
	/* -simple way without error_checking
	lua_dostring(L, script);*/

	if (0 != luaL_loadbuffer(L, script, strlen(script), NULL))
	{
		TString errorMsg;
		errorMsg.format("Lua Error - String Load\nString:%s\nError Message:%s\n", script, luaL_checkstring(L, -1));
		throw std::runtime_error(errorMsg.ptr());
	}

	if (0 != lua_pcall(L, 0, LUA_MULTRET, 0))
	{
		TString errorMsg;
		errorMsg.format("Lua Error - String run\nString:%s\nError Message:%s\n", script, luaL_checkstring(L, -1));
		throw std::runtime_error(errorMsg.ptr());
	}

}

#ifdef USE_LUABIND
int LUAwrapper::arraySize(luabind::object const& ll)
{
	if (luabind::type(ll) != LUA_TTABLE)
		throw std::range_error("arraySize called for non-table object");

	int count=0;
	for (luabind::iterator i(ll), end; i != end; ++i)
		count++;
	return count;
}

int LUAwrapper::treeSize(luabind::object const& ll)
{
	// a={{"a","b"}, "c"} -> size=5 ( Root, firstInternalNode, "a", "b", "c")
	if (luabind::type(ll) != LUA_TTABLE)
	{
//		printf("%s\n", luabind::object_cast<const char*>(ll));
		return 1;
	}

	int count=0;
	for(luabind::iterator i(ll), end; i!=end; ++i)
	{
		count+=treeSize(*i);
	}

	return count+1;	// ?�개??internal node가 ?�요?�다.
}
#endif

vector3 LUAwrapper::getVec3(const char* szVarname)
{
#ifdef USE_LUABIND
	luabind::object const& ll=luabind::globals(L)[szVarname];

	if (luabind::type(ll) != LUA_TTABLE)
		throw std::range_error("vector3_assign");
	vector3 l;
	BEGIN_LUAWRAPPER_GETVAL
	l.x=luabind::object_cast<double>(ll[1]);	// lua indexing starts from 1.
	l.y=luabind::object_cast<double>(ll[2]);
	l.z=luabind::object_cast<double>(ll[3]);
	END_LUAWRAPPER_GETVAL
	return l;
#else
	assert(false);
	return vector3(0,0,0);
#endif
}

#ifdef USE_LUABIND
void LUAwrapper::unpackArray(luabind::object const& ll, TStrings& out)
{
	out.resize(arraySize(ll));

	try{

	int c=0;
	for(luabind::iterator i(ll), end; i!=end; ++i)
	{
		out[c++]=luabind::object_cast<const char*>(*i);
	}
	}
	catch(...)
	{
		Msg::error("LUAwrapper::unpackArray");
	}

}
#endif

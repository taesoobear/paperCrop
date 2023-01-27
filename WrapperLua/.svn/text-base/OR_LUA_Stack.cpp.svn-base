// deprecated. Do not use.
#include "stdafx.h"
#include "OR_LUA_Stack.h"

#ifndef _MSC_VER
    void lua_dostring(lua_State* L, const char* str);
    void lua_dofile(lua_State* L, const char* str);
#endif

namespace OR {
	const char LUAWrap::Worker::className[] = "Worker";

	Lunar<LUAWrap::Worker>::RegType LUAWrap::Worker::methods[] = {
		{"work", &LUAWrap::Worker::_work},
		{0,0}
	};

	int LUAWrap::Worker::_work(lua_State *L)
	{
		OR::LUAStack lua(L);
		TString workName;
		lua>>workName;

		return work(workName, lua);
	}

	LUAWrap::LUAWrap()
	{
		L = lua_open();
		luaL_openlibs(L);
		//luaopen_base(L );
		//luaopen_io(L );
		//luaopen_string(L );
		//luaopen_math(L );
		//luaopen_debug(L );
		//luaopen_table(L );
		
		Lunar<Worker>::Register(L);
	}


	LUAWrap::~LUAWrap()
	{
		lua_close(L);
	}

	void LUAWrap::dofile(const char* script)
	{
		/* process LUA code */
		luaL_dofile(L, script);
	}


	void LUAStack::pushInt(int a)
	{
		lua_pushnumber(L,a);
	}

	void LUAStack::pushBoolean(int b)
	{
		lua_pushboolean(L,b);
	}
	void LUAStack::pushBoolean(bool b)
	{
		if(b)
			lua_pushboolean(L,1);
		else
			lua_pushboolean(L,0);
	}

void LUAStack::pushDouble(double a)
{
	lua_pushnumber(L, a);
}
void LUAStack::pushString(const char* a)
{
	lua_pushstring(L, a);
}

bool LUAStack::isNumber()
{
	return lua_isnumber(L, mCurrArg);
}

bool LUAStack::isString()
{
	return lua_isstring(L, mCurrArg);
}


int LUAStack::popInt()
{
	int a=(int)lua_tonumber(L, mCurrArg++);
	return a;
}

bool LUAStack::popBoolean()
{
	int a=(int)lua_toboolean(L, mCurrArg++);
	return (bool)a;
}

double LUAStack::popDouble()
{
	double a=lua_tonumber(L, mCurrArg++);
	return a;
}

const char* LUAStack::popString()
{
	const char* a=lua_tostring(L, mCurrArg++);
	return a;
}

// read results from stack
// ex) LUAStack c(L);  int i,j,k;  c>>i >>j >> k;
LUAStack& operator>>( LUAStack& os, int& a)
{
	a=(int)lua_tonumber(os.L, os.mCurrArg++); return os;
}
LUAStack& operator>>( LUAStack& os, bool& a)
{
	a=(bool)lua_toboolean(os.L, os.mCurrArg++); return os;
}

LUAStack& operator>>( LUAStack& os, double& a)
{
	a=lua_tonumber(os.L, os.mCurrArg++); return os;
}
LUAStack& operator>>( LUAStack& os, std::string& a)
{
	a=lua_tostring(os.L, os.mCurrArg++); return os;
}
LUAStack& operator>>( LUAStack& os, TString& a)
{
	a=lua_tostring(os.L, os.mCurrArg++); return os;
}

}

#ifndef OR_LUA_STACK_H
#define OR_LUA_STACK_H
#pragma once


//#include "include/OR_LUA/OR_LUA_Config.h"
#include "lunar.h"


extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
	//#include "luadebug.h"
}

namespace OR {

/* 
Simple wrapper class for lua stack manipulation.
// deprecated! do not use. Use lunaStack instead.
*/
class LUAStack
{
 public:
	lua_State* L;
	int mCurrArg;
	int mNumRet;
	// for lua glue function
	LUAStack(lua_State* pState):L(pState),mCurrArg(1),mNumRet(-1){}

	// for lua function call
	LUAStack(lua_State* pState, int numOut):L(pState), mCurrArg(-numOut), mNumRet(numOut){}

	LUAStack():L(NULL), mCurrArg(-1),mNumRet(-1){}
	~LUAStack()
	{
		if(mNumRet>0)
		{
			// ???ΩÏñ¥?îÎäîÏßÄ ?ïÏù∏????
//			ASSERT(mCurrArg==0);
			// restore Lua stack
			lua_pop( L, mNumRet );
		}
	}

	////////////////////////////////////////////////////////////
	// Stack manipulation functions.
	////////////////////////////////////////////////////////////
	int numArg()				{ return lua_gettop(L);}
	double tonumber(int i)		{ return lua_tonumber(L, i);}
	const char* tostring(int i)	{ return lua_tostring(L, i);}

	// save results to stack (in a glue function)
	// ex) LUAStack c(L);    c<<3 <<4 <<5; c.push(d); return 3;

	void pushInt(int a);
	void pushDouble(double a);
	void pushString(const char* a);
	void pushBoolean(bool b);
	void pushBoolean(int b);
	template <class T> void push(T* c);


	// for boolean variable, use pushBoolean!!!
	friend LUAStack& operator<<( LUAStack& os, int a)				{ os.pushInt(a); return os;}
	friend LUAStack& operator<<( LUAStack& os, double a)			{ os.pushDouble(a); return os;}
	friend LUAStack& operator<<( LUAStack& os, TString const &a)	{ os.pushString(a.ptr()); return os;}

	// read results from stack (in a glue function)
	// ex) LUAStack c(L);  LMotion* t;  t=c.pop<LMotion>(); int i,j,k;  c>>i >>j >> k;

	// check if top is number type.
	bool isNumber();
	bool isString();

	int popInt();
	double popDouble();
	const char* popString();
	bool popBoolean();
	template <class T> T* pop(); 	//!< T must be a Lunar class

	friend LUAStack& operator>>( LUAStack& os, int& a);
	friend LUAStack& operator>>( LUAStack& os, double& a);
	friend LUAStack& operator>>( LUAStack& os, std::string& a);
	friend LUAStack& operator>>( LUAStack& os, TString& a);
	friend LUAStack& operator>>( LUAStack& os, bool& a);
};

template <class T>
T* LUAStack::pop()
{
	return Lunar<T>::check(L,mCurrArg++);
}

template <class T>
void LUAStack::push(T* c)
{
	Lunar<T>::push(L, c);
}




// deprecated  use LUAwrapper instead.
// Taesoo designed. Simple wrapper class for lua scripting  -->
class LUAWrap
{
	lua_State* L;
public:
	LUAWrap();
	~LUAWrap();

	class Worker
	{
		int _work(lua_State *L) ;
	public:
		static const char className[];
		static Lunar<Worker>::RegType methods[];

		Worker(){}
		Worker(lua_State *L){Msg::error("Worker default constructor");}
		virtual ~Worker(){}

		// reimplement this
		virtual int work(TString const& workname, OR::LUAStack& L)				{return 0;}

		int _work(TString const& workname)			{ OR::LUAStack st; return work(workname, st);}
	};

	void dofile(const char* script);
	lua_State* getState()	{return L;}


	void getGlobal(const char* var)		{ lua_getglobal(L, var);}
	void setGlobal(const char* var)		{ lua_setglobal(L, var);}

	// setWorker("Loader", loader)
	void setWorker(const char* szVarName, Worker& worker)
	{
		Lunar<LUAWrap::Worker>::push(getState(), &worker);
		setGlobal(szVarName);
	}

	template <class T>
	void setLVal(const char* szVarName, T& val)
	{
		Lunar<T>::push(getState(), &val);
		setGlobal(szVarName);
	}

	// setVal<int>("i",10)
	template <class T>
	void setVal(const char* szVarName, T const& val)
	{
		OR::LUAStack in(getState());
		in<<val;
		setGlobal(szVarName);

	}

	// getVal<TString>("name", name)
	template <class T>
	void getVal(const char* szVarName, T& val)
	{
		getGlobal(szVarName);
		OR::LUAStack out(getState(), 1);
		out >>val;
	}

	// name=getValue<TString>("name")
	template <class T>
	T getValue(const char* szVarName)
	{
		T val;
		getGlobal(szVarName);
		OR::LUAStack out(getState(), 1);
		out >>val;
		return val;
	}

	// function call.
	// Usage: outStack??scopeÍ∞Ä ?§ÌÉù?????ΩÏñ¥?®ÌõÑ Î∞îÎ°ú ?ùÎÇò?ÑÎ°ù Íµ¨ÌòÑ?úÎã§.
	// eg)
	//{
	//	LUAStack in, out;
	//	lua.prepairCall("func", in);
	//	in<<3<<4;
	//	lua.call(2, 3, out);
	//	int i,j,k;
	//	out>> i>>j>>k;
	//}
	void prepairCall(const char* func, LUAStack& inStack)
	{
		getGlobal(func);
		inStack.L=getState();
		inStack.mCurrArg=1;
	}

	void call(int numArg, int numOut, LUAStack& outStack)
	{
		lua_call(L, numArg, numOut);
		outStack.L=L;
		outStack.mNumRet=numOut;
		outStack.mCurrArg=-numOut;
	}

	


};

}

#endif

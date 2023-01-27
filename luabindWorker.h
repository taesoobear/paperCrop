#ifndef LUABIND_WORKER_
#define LUABIND_WORKER_

#pragma once

class LUAwrapper;
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
	//#include "luadebug.h"
}

namespace luabind
{
	namespace adl
	{
		class object;
	}
}


class luabindWorker
{
 public:
  virtual void __call(const char* wn, luabind::adl::object const& table)=0;
};
#endif
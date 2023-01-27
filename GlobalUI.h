#ifndef _GLOBAL_UI_H_
#define _GLOBAL_UI_H_

#pragma once
// a lua script can access all the functionalities of the ogrefltk framework.
// this is for enabling command line mode execution. e.g. batch optimization.
// this class should be written in a general way.

// can access to FlLayout, FlChoiceWin, Loader, MotionPanel, luawrapper, etc...(only inside the lua script, or a lua console.)

class LUAwrapper;
class FlLayout;
#ifdef USE_LUABIND
#include "luabindWorker.h"
#endif

#include "WrapperLua/OR_LUA_Stack.h"
class GlobalUI : public OR::LUAWrap::Worker
{
  TStrings _param;
  LUAwrapper* _L;
 public:
  GlobalUI(FlLayout* win, int argc, char* argv[]);
  ~GlobalUI();

  void releaseScript();
#ifdef USE_LUABIND
  virtual void work(const char* wn, luabind::adl::object const& table);
#endif
  
  virtual int work(TString const& workname, OR::LUAStack& L);
};
#endif

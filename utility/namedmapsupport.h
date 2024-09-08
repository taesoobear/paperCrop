#ifndef __NAMEDTYPES_H__
#define __NAMEDTYPES_H__

#pragma warning (disable: 4786)
#include <map>
#include <string>
#include <functional>

#pragma warning (disable: 4800)

struct ltstr
{
	bool operator()(const std::string &_X, const std::string &_Y) const
	{
		return _X.compare(_Y)<0;
	}
};

struct ltsz
{
	bool operator()(char* const & _X, char* const & _Y) const
	{
		return strcmp(_X, _Y)<0;
	}
};

struct ltcsz
{
	bool operator()(const char* _X, const char* _Y) const
	{
		return strcmp(_X, _Y)<0;
	}
};

struct ltint
{
	bool operator()(const int &_X, const int &_Y) const
	{
		return _X<_Y;
	}
};

#include "TypeString.h"
struct cmpTString
{
	bool operator()(TString const & _X, TString const & _Y) const
	{
		return _X.strcmp(_Y)<0;
	}
};


#pragma warning (disable: 4786)
/// char*으로 부터 int를 얻어낼때 사용
typedef std::map<char*, int, ltsz> namedmapInt2;
/// string으로 부터 void를 얻어낼때 사용
typedef std::map<std::string , void *, ltstr> namedmapVoidData;
/// string으로 부터 int를 얻어낼때 사용
typedef std::map<std::string , int, ltstr> namedmapInt;
/// int로 부터 LPVOID를 얻어낼때 사용
typedef std::map<int, void*, ltint> intmapVoidData;

#endif //__NAMEDTYPES_H__

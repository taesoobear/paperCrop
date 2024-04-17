
#ifdef _WIN32
#ifdef _MSC_VER
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Windows 헤더에서 거의 사용되지 않는 내용을 제외시킵니다.
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#endif
#else
#include <unistd.h>
#include <stdio.h>
#endif
void deleteFile( const char* filename)
{
#ifdef _WIN32
	::DeleteFile(filename);
#else
	remove(filename);
#endif
}

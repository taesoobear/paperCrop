//
// stdafx.cpp 
//
// Copyright 2004 by Taesoo Kwon.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//

// stdafx.cpp : source file that includes just the standard includes
// PaperCrop.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


Msg::Base g_cMsgUtil;
Msg::Base* Msg::g_pMsgUtil=&g_cMsgUtil;	

void Msg::verify(bool bExpression, const char* pszFormat, ...)
{
	// release������ ������� �ʴ� verify�� �ϰ� ������� ���.
	if(!bExpression)
	{
		TString temp;
		va_list argptr ;
		va_start(argptr, pszFormat);
		temp._format(pszFormat, argptr);
		g_pMsgUtil->error(temp);
	}
}
void Msg::print(const char* pszFormat,...)
{
	TString temp;
	va_list argptr ;
	va_start(argptr, pszFormat);
	temp._format(pszFormat, argptr);
	g_pMsgUtil->print(temp);
}
void Msg::print2(const char* pszFormat,...)
{
	TString temp;
	va_list argptr ;
	va_start(argptr, pszFormat);
	temp._format(pszFormat, argptr);
	g_pMsgUtil->print2(temp);
}

void Msg::error(const char* pszFormat,...)
{
	TString temp;
	va_list argptr ;
	va_start(argptr, pszFormat);
	temp._format(pszFormat, argptr);
	g_pMsgUtil->error(temp);
}
void Msg::msgBox(const char* pszFormat,...)
{
	TString temp;
	va_list argptr ;
	va_start(argptr, pszFormat);
	temp._format(pszFormat, argptr);
	g_pMsgUtil->msgBox(temp);
}
bool Msg::confirm(const char* pszFormat,...)
{
	TString temp;
	va_list argptr ;
	va_start(argptr, pszFormat);
	temp._format(pszFormat, argptr);
	return g_pMsgUtil->confirm(temp);
}
void Msg::flush()
{
	g_pMsgUtil->flush();
}

void Msg::outputState(bool bOutput)
{
	g_pMsgUtil->m_bOutput=bOutput;
}

void Msg::output(const char* key, const char* pszFormat, ...)
{
	if(!g_pMsgUtil->m_bOutput) return;
	TString temp;
	va_list argptr ;
	va_start(argptr, pszFormat);
	temp._format(pszFormat, argptr);
	g_pMsgUtil->output(key, temp);
}
		// inherit and implement followings
void Msg::Base::print(const char* msg)	{ printf("%s", msg);}
void Msg::Base::print2(const char* msg)	{ printf("                                       \r");printf("%s\r", msg); }
void Msg::Base::flush()				{ fflush(stdout);}
void Msg::Base::error(const char* msg) { msgBox(msg); ASSERT(0);throw(std::runtime_error(msg));}
void Msg::Base::msgBox(const char* msg){ fl_message("%s\n", msg);}
		// ���� �����ȵ�. ����ڿ��� yes or no����� ���.
bool Msg::Base::confirm(const char* msg) { return fl_ask("%s", msg);}
void Msg::Base::output(const char* key, const char* msg){printf("%s: %s\n", key, msg); }

#ifdef unix
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <glob.h>

bool CreateDirectory(const char *PathToCreate){
	return mkdir(PathToCreate,S_IRWXU | S_IRWXG | S_IRWXO);



}

bool DeleteAllFiles(const char* path, const char* mask, bool bConfirm)
{
  glob_t globbuf;
  char szFileName[PATH_MAX];
  sprintf(szFileName, "%s/%s", path, mask);

  glob(szFileName, 0, NULL, &globbuf);

  bConfirm = true;

  if (bConfirm && globbuf.gl_pathc)
    {
      if (!Msg::confirm("Do you really want to delete all files in %s?", path))
        {
          globfree(&globbuf);
          return false;
        }
    }

  for (unsigned int i = 0; i < globbuf.gl_pathc; i++)
    {
      if (remove(globbuf.gl_pathv[i]) != 0)
        {
        break;
        }
    }

  globfree(&globbuf);
  return true;

}



#else
bool CreateDirectory(const char *PathToCreate)
{
#ifdef _WIN32
	static char Path[1000];
	strcpy(Path,PathToCreate);
	
	// First add an ending slash
	char* Pos = strrchr(Path,'\\');
	if (!Pos || Pos[1]!=0)
	strcat(Path,"\\");
	// From the first slash to the last check and create path
	// Exception : Jump initial duble slashed UNC host name and include first directory
	if (Path[0]=='\\' && Path[1]=='\\')
	{
		Pos = strchr(Path+2,'\\');
		if (Pos)
			Pos = strchr(Pos+1,'\\');
	}
	else
		Pos = strchr(Path,'\\');
	bool AllOk = true;
	int ret;
	while (Pos && AllOk)
	{
		*Pos = 0;
		ret=GetFileAttributes(Path);
		if (ret==-1)
		{
			// ������ ������
			if (GetLastError()==ERROR_FILE_NOT_FOUND)
				AllOk = CreateDirectory(Path,NULL)==TRUE;
			else
				AllOk = false;
		}
		else if ((ret|FILE_ATTRIBUTE_DIRECTORY)!=0)
		{
			if(strchr(Pos+1,'\\')==0)
				AllOk=false;
		}
		else
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);

			AllOk = false;
		}
		*Pos = '\\';
		Pos = strchr(Pos+1,'\\');
	}
	return AllOk;
#else
	static char Path[1000];
	sprintf(Path, "mkdir -p '%s'", PathToCreate);
	system(Path);
	return true;
#endif
}


bool DeleteAllFiles(const char* path, const char* mask,bool bConfirm)
{
#ifdef _WIN32
	WIN32_FIND_DATA findData;
	char szFileName[2000];
	sprintf(szFileName,"%s\\%s", path, mask);

	char EFN[2000];
	HANDLE hFindHandle;
	hFindHandle=FindFirstFile((LPCTSTR)szFileName,&findData);
			
	if(hFindHandle==INVALID_HANDLE_VALUE)
		return false;
			
	while(1)
	{
		if(findData.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY)
		{
			if(bConfirm)
			{
				if(!Msg::confirm("Do you really want to delete all files in %s?", path))						return false;
				bConfirm=false;
			}
			sprintf(EFN,"%s\\%s",path,findData.cFileName);
			if(!DeleteFile(EFN))
			{
				return false;
			}
		}
		if(!FindNextFile(hFindHandle,&findData))
				break;
	}

	FindClose(hFindHandle);
	return true;
#else
	static char Path[1000];
	sprintf(Path, "rm '%s/*'", path);
	system(Path);
	return true;
#endif
}
#endif

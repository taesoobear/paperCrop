# -*- mode: cmake; -*-
cmake_minimum_required(VERSION 3.1)

set (CMAKE_VERBOSE_MAKEFILE  ON)
set(CMAKE_CXX_STANDARD 17) 
project (paperCrop)

set(LUNA_GEN ${paperCrop_SOURCE_DIR}/luna_gen.lua)
#set(LUA ${paperCrop_SOURCE_DIR}/lua)

include(${paperCrop_SOURCE_DIR}/modules.cmake)

if(CONSOLE)
	add_definitions(-DNO_SHOW_WIN)
endif()

if(UNIX)
  if(CMAKE_BUILD_TYPE MATCHES Release)
	add_definitions(-DNDEBUG)
  endif()

  message("Unix OS")

else()
  add_definitions(-D_SCL_SECURE_NO_DEPRECATE -D__WIN32__ -DHPDF_DLL)

  message("Windows OS") 
endif()

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  add_definitions(-DAMD64)
endif()

if(UNIX)
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
  # set(LUA_INCLUDE "/usr/include/lua5.1" )
  # set(LUA_LIB "${paperCrop_SOURCE_DIR}/../Lua5.1.4/src" "${paperCrop_SOURCE_DIR}/../lua-5.1.4/src")
  include(FindPkgConfig)
  find_package(FLTK REQUIRED)
  find_package(Lua51 REQUIRED)
  if(APPLE)
  pkg_check_modules(poppler REQUIRED )
  else()
  # find_package(Luabind REQUIRED)
  pkg_check_modules(Fontconfig REQUIRED fontconfig)
  pkg_check_modules(poppler REQUIRED )
  pkg_check_modules(DevIL REQUIRED ILU)
  endif()
  set(image_INCLUDE) # all in /usr/include 
  message("include directories:")
  message(${LUA_INCLUDE_DIR} "\n")
  message(${FLTK_INCLUDE_DIR} "\n")
  message(${DevIL_INCLUDE_DIR} "\n")
  #  message(${Luabind_INCLUDE_DIR} "\n")
  message(${FLTK_LIBRARIES} "\n")
  message(${Fontconfig_LIBRARIES} "\n")
  message("poppler:")
  message(${poppler_INCLUDE_DIRS} "\n")
  message(${poppler_LIBRARIES} "\n")
  message(${DevIL_INCLUDE_DIRS} "\n")

  if(NOT poppler_LIBRARIES) # if empty
	  if (APPLE)
		  link_directories(/usr/local/lib)
		  link_directories(/opt/homebrew/lib)
	  endif()
	  set(poppler_LIBRARIES poppler) # set default
  endif()
else()
  set(LUA_INCLUDE "${paperCrop_SOURCE_DIR}/../Lua5.1.4/include")
  set(LUA_LIB "${paperCrop_SOURCE_DIR}/../lua5.1.4/lua5.1.lib")
  set(image_INCLUDE 
	"${paperCrop_SOURCE_DIR}/devil/include"
	"${paperCrop_SOURCE_DIR}/FreeImage/Dist"
	)

endif()

if (UNIX)
else()
  # Additional sources
  #  add_subdirectory(luabind-0.9)
endif()

add_subdirectory(libharu-2.0.8)

if(UNIX)
  # add_subdirectory(poppler) # moved out my modified portion so that system library can be used
else()

  set(CMAKE_SOURCE_DIR "${paperCrop_SOURCE_DIR}/poppler-0.16.2")
  set(FREETYPE_INCLUDE_DIRS "${paperCrop_SOURCE_DIR}/ext/freetype231/include")
  set(FREETYPE_LIBRARY "${paperCrop_SOURCE_DIR}/ext/freetype231/freetype231mt.lib")
  set(JPEG_INCLUDE_DIR "${paperCrop_SOURCE_DIR}/ext/jpeg")
  set(JPEG_LIBRARY "${paperCrop_SOURCE_DIR}/ext/jpeg/jpeg_s.lib")
  set(ZLIB_INCLUDE_DIRS "${paperCrop_SOURCE_DIR}/ext/zlib")
  set(ZLIB_LIBRARIES "${paperCrop_SOURCE_DIR}/ext/zlib/zlib_s.lib")
  set(ICONV_INCLUDE_DIR "c:/Program Files/GnuWin32/include")
  set(ICONV_LIBRARIES 
	"c:/Program Files/GnuWin32/lib/libiconv.lib"
	"c:/Program Files/GnuWin32/lib/libcharset.lib"
	)
  set(ICONV_SECOND_ARGUMENT_IS_CONST on)
  include_directories(${ICONV_INCLUDE_DIR} ${paperCrop_SOURCE_DIR}/poppler-0.16.2)
  add_subdirectory(poppler-0.16.2)
endif()


if(UNIX)
	if(CONSOLE)
		include_directories(

			${libharu_BINARY_DIR}/include
			${LUA_INCLUDE_DIR}
			${paperCrop_SOURCE_DIR}
			${paperCrop_SOURCE_DIR}/libharu-2.0.8/include
			${poppler_INCLUDE_DIRS}
			/usr/include/poppler
			${DevIL_INCLUDE_DIRS}
			./include 
			.
			./unix
			)
	else()
		include_directories(
			${libharu_BINARY_DIR}/include
			${LUA_INCLUDE_DIR}
			${FLTK_INCLUDE_DIR}
			${paperCrop_SOURCE_DIR}
			${paperCrop_SOURCE_DIR}/libharu-2.0.8/include
			${poppler_INCLUDE_DIRS}
			/usr/include/poppler
			/usr/local/include/poppler
			/opt/homebrew/include/poppler
			${DevIL_INCLUDE_DIRS}
			./include 
			.
			./unix
			)
	endif()
else()
  include_directories(

	${LUA_INCLUDE}
	${paperCrop_SOURCE_DIR}	
	${image_INCLUDE}
	${paperCrop_SOURCE_DIR}/../fltk
	${paperCrop_SOURCE_DIR}/libharu-2.0.8/include
	#${paperCrop_SOURCE_DIR}/luabind-0.9
	${paperCrop_SOURCE_DIR}/poppler-0.16.2/poppler
	${paperCrop_SOURCE_DIR}/poppler-0.16.2/goo
	${paperCrop_SOURCE_DIR}/ext/jpeg
	${paperCrop_SOURCE_DIR}/ext/zlib
	${paperCrop_SOURCE_DIR}/ext/freetype231/include
	${paperCrop_SOURCE_DIR}/poppler-0.16.2/poppler
	${paperCrop_SOURCE_DIR}/poppler-0.16.2/splash
	${ICONV_INCLUDE_DIR}
	)
endif()

if(UNIX)
  link_directories(

	#${paperCrop_SOURCE_DIR}/../FLTK/lib
	${paperCrop_SOURCE_DIR}/libharu-2.0.8
	# ${paperCrop_SOURCE_DIR}/devil/lib
	${paperCrop_SOURCE_DIR}/../FreeImage/Dist
	# ${paperCrop_SOURCE_DIR}/poppler/
	${paperCrop_SOURCE_DIR}/../Freetype/objs/.libs 
	#${paperCrop_SOURCE_DIR}/luabind-0.9/Release
	${LUA_LIB}
	${Fontconfig_LIBRARY_DIRS}
	${poppler_LIBRARY_DIRS}
	${DevIL_LIBRARY_DIRS}
	/usr/lib/
	)
else()
	#link_directories(${paperCrop_SOURCE_DIR}/build/luabind-0.9/release)
  link_directories(${paperCrop_SOURCE_DIR}/build/poppler-0.16.2/release)
endif()

if(UNIX)
  set (FILE_CHOOSER
	"./fltk/Fl_Native_File_Chooser-0.84/Fl_Native_File_Chooser.cxx"
	)
else()	
  set (FILE_CHOOSER
	"./fltk/Fl_Native_File_Chooser-0.84/Fl_Native_File_Chooser_WIN32.cxx"
	"./fltk/Fl_Native_File_Chooser-0.84/FL/Fl_Native_File_Chooser_WIN32.H"
	)
endif()

add_custom_command(
	OUTPUT 
	"${paperCrop_SOURCE_DIR}/luna_mainlib.cpp" 
	"${paperCrop_SOURCE_DIR}/luna_baselib.cpp" 
	"${paperCrop_SOURCE_DIR}/luna_baselib.h"
	"${paperCrop_SOURCE_DIR}/luna_mainlib.h"
	DEPENDS 
	"${LUNA_GEN}" 
	"${paperCrop_SOURCE_DIR}/luna_baselib.lua"
	"${paperCrop_SOURCE_DIR}/luna_mainlib.lua"
	PRE_BUILD
	COMMAND lua "${LUNA_GEN}" "../luna_baselib.lua"
	)

set(ctarget 
  "./luna_baselib.cpp"
  "./luna_baselib.h"
  "./luna_mainlib.cpp"
  "./luna_mainlib.h"
  )

set(src
  "./GlobalUI.cpp"
  "./GlobalUI.h"
  "./luna.h"
  "./luna.cpp"
  "./ImageSegmentation.cpp"
  "./ImageSegmentation.h"
  "./PaperCrop.cpp"
  "./PaperCrop.h"
  "./PDFwin.cpp"
  "./PDFwin.h"
  "./PDFWriter.cpp"
  "./PDFWriter.h"
  #"./poppler.cpp"
  "./RightPanel.cpp"
  "./RightPanel.h"
  "./stdafx.cpp"
  "./stdafx.h"
  #  "./WrapperLua/BaselibLUA.cpp"
  #  "./WrapperLua/BaselibLUA.h"
  #  "./WrapperLua/BaselibLUA2.cpp"
  #  "./WrapperLua/BaselibLUA2.h"
  #  "./WrapperLua/BaselibLUA3.cpp"
  #  "./WrapperLua/BaselibLUA3.h"
  #  "./WrapperLua/MainlibLUA.cpp"
  #  "./WrapperLua/MainlibLUA.h"
  "./WrapperLua/LUAwrapper.cpp"
  "./WrapperLua/OR_LUA_Stack.cpp"
  "./WrapperLua/OR_LUA_Stack.h"
  "./math/bitVectorN.cpp"
  "./math/bitVectorN.h"
  "./math/interval.cpp"
  "./math/interval.h"
  "./math/intervals.cpp"
  "./math/intervals.h"
  "./math/math_macro.h"
  "./math/mathclass.h"
  "./math/matrix.cpp"
  "./math/matrix.h"
  "./math/matrix3.cpp"
  "./math/matrix3.h"
  "./math/matrix_n.cpp"
  "./math/matrix_n.h"
  "./math/Operator.cpp"
  "./math/Operator.h"
  "./math/operatorTemplate.hpp"
  "./math/quater.cpp"
  "./math/quater.h"
  "./math/template_math.h"
  "./math/template_matrix.h"
  "./math/tvector.cpp"
  "./math/tvector.h"
  "./math/transf.cpp"
  "./math/transf.h"
  "./math/vector.cpp"
  "./math/vector.h"
  "./math/vector3N.cpp"
  "./math/vector3N.h"
  "./math/quaterN.cpp"
  "./math/quaterN.h"
  "./math/vector_n.cpp"
  "./math/vector_n.h"
  "./image/Image.cpp"
  "./image/Image.h"
  "./image/ImagePixel.cpp"
  "./image/ImagePixel.h"
  "./image/ImageProcessor.cpp"
  "./image/ImageProcessor.h"
  "./utility/FlLayout.cpp"
  "./utility/FlLayout.h"
  "./utility/FltkAddon.cpp"
  "./utility/FltkAddon.h"
  "./utility/operatorString.cpp"
  "./utility/operatorString.h"
  "./utility/TypeString.cpp"
  "./utility/TypeString.h"
  "./utility/TextFile.cpp"
  "./utility/TextFile.h"
  )


if(CONSOLE)
	add_executable(paperCrop
		${src}
		${ctarget}
		"./console/dummies.cpp"
		)
else()
	add_executable(paperCrop
		${src}
		${ctarget}
		)
endif()


if(APPLE)

  target_link_libraries(paperCrop
	${Fontconfig_LIBRARIES}
	${FLTK_LIBRARIES}
	${DevIL_LIBRARIES}
	# fltk_images
	# fltk
	hpdf
	ILU
	IL
	${poppler_LIBRARIES}
	freeimage
	z
	jpeg
	${LUA_LIBRARY}
	#luabind
	)
elseif(UNIX)

  target_link_libraries(paperCrop
	${Fontconfig_LIBRARIES}
	${FLTK_LIBRARIES}
	${DevIL_LIBRARIES}
	# fltk_images
	# fltk
	hpdf
	X11
	ILU
	IL
	${poppler_LIBRARIES}
	freeimage
	z
	jpeg
	${LUA_LIBRARY}
	#luabind
	)

endif()


IF(MSVC)
  target_link_libraries(paperCrop
	${LUA_LIB}
	#luabind
	poppler
	debug ${paperCrop_SOURCE_DIR}/ext/freetype231/freetype231mt_d.lib 
	debug ${paperCrop_SOURCE_DIR}/ext/zlib/zlib_ds.lib 
	debug ${paperCrop_SOURCE_DIR}/ext/jpeg/jpeg_ds.lib 
	debug ${paperCrop_SOURCE_DIR}/../fltk/test/fltkdlld.lib
	debug ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkimagesd.lib
	debug ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkpngd.lib
	debug ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkzd.lib
	debug ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkgld.lib
	optimized ${paperCrop_SOURCE_DIR}/ext/freetype231/freetype231mt.lib 
	optimized ${paperCrop_SOURCE_DIR}/ext/zlib/zlib_s.lib 
	optimized ${paperCrop_SOURCE_DIR}/ext/jpeg/jpeg_s.lib 
	optimized ${paperCrop_SOURCE_DIR}/../fltk/test/fltkdll.lib
	optimized ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkimages.lib
	optimized ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkpng.lib
	optimized ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkz.lib
	optimized ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkgl.lib
	${paperCrop_SOURCE_DIR}/devil/lib/DevIl.lib
	${paperCrop_SOURCE_DIR}/devil/lib/Ilu.lib
	${paperCrop_SOURCE_DIR}/FreeImage/Dist/FreeImage.lib
	${paperCrop_SOURCE_DIR}/libharu-2.0.8/libhpdf.lib
	comctl32.lib 
	winspool.lib 
	Msimg32.lib 
	wininet.lib
	${ICONV_LIBRARIES}

	
	)
endif()

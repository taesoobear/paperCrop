//
// PDFwin.H 
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


#pragma once

#include "utility/TypeString.h"
#include "image/Image.h"

class PDFmodel;

class PDFwin;
class SelectionRectangle : public TRect
{
public:
	Double2D  p1, p2;	// doc coord in [0,1] range

	SelectionRectangle (){left=-1;right=0;}	// invalid.
	bool isValid();
	void updateScreenCoord(PDFwin const& win);
	int handle(PDFwin const& win, TString const& event);
};

class FlLayout;
class PDFwin : public Fl_Double_Window
{
	std::list<SelectionRectangle>::iterator findRect(int x, int y);

public:
	PDFwin(int x, int y, int w, int h);
	~PDFwin(void);

	void load(const char* filename);

	std::string _filename;
	void pageChanged();

	int getNumPages();
	int getNumRects();
	void getRectSize(int pageNo, int rectNo, SelectionRectangle& rect);
	void getRectImage_width(int pageNo, int rectNo, int width, CImage& image);
	void getRectImage_height(int pageNo, int rectNo, int height, CImage& image);
	double getDPI_height(int pageNo, int rectNo, int height);
	double getDPI_width(int pageNo, int rectNo, int width);
	double pageCropWidth(int pageNo);
	double pageCropHeight(int pageNo);
	void setStatus(const char* o);
	
	// LUA interface functions.
	void deleteAllFiles();
	void deleteAllFilesWithoutConfirm();
	void setCurPage(int pageNo);
	
//private:
	// Fl_Window::
	virtual void draw();
	virtual int handle(int event);

	void onIdle();
	PDFmodel* mModel;
	int mCurrPage;
	TString mState;
	FlLayout* mLayout;

	TString mOutDir;

	Int2D toWindowCoord(double x, double y) const;
	Double2D   toDocCoord(int x, int y) const;

	std::list<SelectionRectangle> mRects;
	std::list<SelectionRectangle>::iterator mSelectedRect;
	SelectionRectangle& selectedRect()	{return *mSelectedRect;}
};

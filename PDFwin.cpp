//
// PDFwin.cpp 
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

#include "stdafx.h"
#include "PDFwin.h"

#include <vector>

#include "utility/FlLayout.h"
#include "utility/FltkAddon.h"

// poppler headers
#include "PDFDoc.h"
#include "GlobalParams.h"
// #include "GooMutex.h"
#include <goo/GooString.h>
#include "Link.h"
#include "Object.h" /* must be included before SplashOutputDev.h */
#include "PDFDoc.h"
#include <splash/SplashBitmap.h>
#include <SplashOutputDev.h>
#include "TextOutputDev.h"
#include "NullOutputDev.h"
#include "SplashMod.h"
#include "image/Image.h"
#include "image/ImagePixel.h"
#include "math/Operator.h"
#include <cstdlib>

#include "ImageSegmentation.h"

PDFmodel* gModel=NULL;
enum {TC_c=0, TC_x, TC_y, TC_w, TC_h, TC_fx, TC_fy};
// #define DEBUG_FONT_DETECTION
#define USE_FONT_DETECTION
// #define USE_NULLOUTPUTDEV


class PDFmodel
{
public:

	PDFDoc* _pdfDoc ;
	SplashOutputDev* _outputDev;
	NullOutputDev* _nullOutputDev;
	std::vector<CImage* > _bmpCache;
	std::vector<intmatrixn* > _textCache;	
	int _textCacheState;
	PDFmodel()
	{
		if (!globalParams)
		{
			printf("error loading pdf");
			return ;
		}
		_pdfDoc =NULL;
		_outputDev=NULL;
		_nullOutputDev=NULL;
		_textCacheState=-1;// do not cache.
		gModel=this;
	}

	~PDFmodel()
	{
		delete _pdfDoc;
		delete _outputDev;
		delete _nullOutputDev;
		gModel=NULL;
	}
	
	bool load(const char* fileName)
	{
		delete _pdfDoc;
		delete _outputDev;
		delete _nullOutputDev;

		for (int i=0; i<_bmpCache.size(); i++)	delete _bmpCache[i];
		_bmpCache.resize(0);
		for (int i=0; i<_textCache.size(); i++)	delete _textCache[i];
		_textCache.resize(0);

#ifdef __APPLE__
		_pdfDoc= new PDFDoc(std::unique_ptr<GooString>(new GooString(fileName)));
#else
		_pdfDoc= new PDFDoc(new GooString(fileName));
#endif
		if (!_pdfDoc->isOk()) {
			printf("error loading pdf");
			return false;
		}

		bool bitmapTopDown = true;

		SplashColor white;
		white[0]=0xff;
		white[1]=0xff;
		white[2]=0xff;

		// _outputDev = new SplashOutputDev_mod(splashModeRGB8, 4, gFalse, white, bitmapTopDown);
		_outputDev = new SplashOutputDev(splashModeRGB8, 4, false, white, bitmapTopDown);
		if(!_outputDev)
		{
			printf("error loading pdf");
			return false;
		}
#ifdef USE_NULLOUTPUTDEV
		_nullOutputDev=new NullOutputDev();
#endif
#ifdef POPPLER_VERSION
		_outputDev->startDoc(_pdfDoc);
#else
		_outputDev->startDoc(_pdfDoc->getXRef());
#endif

		_bmpCache.resize(_pdfDoc->getNumPages());
		for (int i=0; i<_bmpCache.size(); i++)	_bmpCache[i]=NULL;
		_textCache.resize(_pdfDoc->getNumPages());
		for (int i=0; i<_textCache.size(); i++)	_textCache[i]=NULL;

		return true;
	}

	bool cacheValid(int w, int h, int pageNo)
	{
		// empty cache when too many pages are cached.
		int count=0;
		for(int i=0; i<_bmpCache.size(); i++)
		{
			if(_bmpCache[i])
				count++;
		}

		const int MAX_CACHE=10;	// due to memory overhead.
		if(count>MAX_CACHE)
		{
			for(int i=0; i<_bmpCache.size(); i++)
			{
				delete _bmpCache[i]; _bmpCache[i]=NULL;
				delete _textCache[i]; _textCache[i]=NULL;				
			}
		}

		if(_bmpCache.size()<=pageNo)
			return false;


		CImage* bmp=_bmpCache[pageNo];
		if(bmp==NULL
			|| (bmp->GetWidth()>w || bmp->GetHeight()>h)
			|| (bmp->GetWidth()<w && bmp->GetHeight()<h))
			return false;

		
		return true;

	}

	double pageCropWidth(int pageNo)
	{
		int rotate=_pdfDoc->getPageRotate(pageNo+1);
		if(rotate==90 || rotate==270)
			return _pdfDoc->getPageCropHeight(pageNo+1);

		return _pdfDoc->getPageCropWidth(pageNo+1);
	}

	double pageCropHeight(int pageNo)
	{
		int rotate=_pdfDoc->getPageRotate(pageNo+1);
		if(rotate==90 || rotate==270)
			return _pdfDoc->getPageCropWidth(pageNo+1);

		return _pdfDoc->getPageCropHeight(pageNo+1);
	}

	double calcDPI_width(int w, int pageNo)
	{
		return (double)72* w/pageCropWidth(pageNo);
	}

	double calcDPI_height(int h, int pageNo)
	{
		return (double)72* h/pageCropHeight(pageNo);
	}

	int	calcHeight_DPI(double DPI, int pageNo)
	{
		int h=int(DPI*pageCropHeight(pageNo)/double(72)+0.5);
		return h;
	}
	
	int	calcWidth_DPI(double DPI, int pageNo)
	{
		int w=int(DPI*pageCropWidth(pageNo)/double(72)+0.5);
		return w;
	}
	
	// 0 <= pageNo <numPage
	CImage* getPage(int w, int h, int pageNo)
	{
		if(!cacheValid(w,h,pageNo))
		{
			// poppler uses 1 indexing for pageNo.

			double DPI_W=calcDPI_width(w, pageNo);
			double DPI_H=calcDPI_height(h, pageNo);
			double DPI=MIN(DPI_W, DPI_H);

			delete _textCache[pageNo]; _textCache[pageNo]=new intmatrixn();
			_textCacheState=pageNo;
			//_pdfDoc->getPageRotate(pageNo+1)
			_pdfDoc->displayPage(_outputDev, pageNo+1, DPI, DPI, 0, false, true, false);
#ifdef USE_NULLOUTPUTDEV
			_nullOutputDev->setInfo(_pdfDoc->getPageRotate(pageNo+1),
									_pdfDoc->getCatalog()->getPage(pageNo+1)->getCropBox()->x1,
									_pdfDoc->getCatalog()->getPage(pageNo+1)->getCropBox()->y1,
									_pdfDoc->getPageCropWidth(pageNo+1),
									_pdfDoc->getPageCropHeight(pageNo+1),
									_outputDev->getBitmap()->getWidth(),
									_outputDev->getBitmap()->getHeight());
		
			_pdfDoc->displayPage(_nullOutputDev, pageNo+1, DPI, DPI, 0, false, true, false);
#endif
			_textCacheState=-1;
				/*//_pdfDoc->displayPageSlice(_outputDev, pageNo+1, DPI, DPI, 0, gFalse, gTrue, gFalse, 20,20, 400,400);
				if(zoom2<zoom1)
				{
					if(h==_outputDev->getBitmap()->getHeight())
						bOkay=true;
					else
					{
						vDPI=72.0*h/_pdfDoc->getPageCropHeight(pageNo+1);
					}
				}
				else
				{
					if(w==_outputDev->getBitmap()->getWidth())
						bOkay=true;
					else
					{
						double tt=72*w/_pdfDoc->getPageCropHeight(pageNo+1);
					}
				}

			}
			while(!bOkay);*/

			SplashBitmap *temp=_outputDev->takeBitmap();

			CImage* ptr=new CImage();
			ptr->SetData(temp->getWidth(), temp->getHeight(), temp->getDataPtr(), temp->getRowSize());
			delete _bmpCache[pageNo]; _bmpCache[pageNo]=ptr;
			delete temp;
		}

		return _bmpCache[pageNo];
	}

	void notifyDraw(int c, int x, int y, int w, int h, int fx, int fy)
	{
#ifdef USE_FONT_DETECTION
		if(_textCacheState!=-1)
		{
			// ĳ�� �ϸ� �ȵɵ�.. �ʹ� ����. �׳� �׸���???.
			//printf("%c", c);
			//fflush(stdout);
			const int MAX_CHAR_PER_PAGE=30000;
			if(_textCache[_textCacheState]->rows()<MAX_CHAR_PER_PAGE)
				_textCache[_textCacheState]->pushBack(intvectorn(7, c, x, y, w, h, fx, fy));
		}
#endif
	}
};


void notifyDraw(int c, int x, int y, int w, int h, int fx, int fy)
{
	if(gModel)
	{
		gModel->notifyDraw(c,x,y,w,h,fx,fy);
	}
}


double PDFwin::pageCropWidth(int pageNo)
{
	return mModel->pageCropWidth(pageNo);
}
double PDFwin::pageCropHeight(int pageNo)
{
	return mModel->pageCropHeight(pageNo);
}
int PDFwin::getNumPages()
{
	return mModel->_pdfDoc ->getNumPages();
}
int PDFwin::getNumRects()
{
	return mRects.size();
}

SelectionRectangle& find(std::list<SelectionRectangle>& mRects, int rectNo)
{
	int _rectNo=0;

	SelectionRectangle* R;
	for(std::list<SelectionRectangle>::iterator i=mRects.begin();
		i!=mRects.end(); ++i)
	{
		if(_rectNo==rectNo) 
		{
			R=&(*i);
			break;
		}
		_rectNo++;
	}
		
	return *R;
}


double PDFwin::getDPI_width(int pageNo, int rectNo, int width)
{
	SelectionRectangle& rect=::find(mRects, rectNo);
	int totalWidth=int((double)width/(rect.p2.x-rect.p1.x)+0.5);

	double DPI = mModel->calcDPI_width(totalWidth, pageNo);
	return DPI;
}

void PDFwin::getRectSize(int pageNo, int rectNo, SelectionRectangle& rect)
{
	rect=::find(mRects, rectNo);
}

void PDFwin::getRectImage_width(int pageNo, int rectNo, int width, CImage& image)
{
	SelectionRectangle& rect=::find(mRects, rectNo);

	PDFDoc* _pdfDoc=mModel->_pdfDoc ;
	SplashOutputDev* _outputDev=mModel->_outputDev;

	int totalWidth=int((double)width/(rect.p2.x-rect.p1.x)+0.5);

	FlLayout* layout=mLayout->findLayout("Automatic segmentation");
	int maxWidth=int((double)width/(1.0/(layout->findSlider("N columns")->value()+1.0))+0.5);

	totalWidth=MIN(maxWidth, totalWidth);

	double DPI = mModel->calcDPI_width(totalWidth, pageNo);
	int totalHeight=mModel->calcHeight_DPI(DPI, pageNo);
	
	int left=int(rect.p1.x*(double)totalWidth+0.5);
	int top=int(rect.p1.y*(double)totalHeight+0.5);
	int right=int(rect.p2.x*(double)totalWidth+0.5);
	int bottom=int(rect.p2.y*(double)totalHeight+0.5);

	_pdfDoc->displayPageSlice(_outputDev, pageNo+1, DPI, DPI, 0, false, true, false, left,top, right-left,bottom-top);
	
	SplashBitmap* bmp=_outputDev->takeBitmap();

	image.SetData(bmp->getWidth(), bmp->getHeight(), bmp->getDataPtr(), bmp->getRowSize());
	//ASSERT(image.GetWidth()==width || totalWidth==maxWidth);
	ASSERT(image.GetHeight()==bottom-top);

	delete bmp;
}

double PDFwin::getDPI_height(int pageNo, int rectNo, int height)
{
	SelectionRectangle& rect=::find(mRects, rectNo);
	int totalHeight=int((double)height/(rect.p2.y-rect.p1.y)+0.5);

	double DPI = mModel->calcDPI_height(totalHeight, pageNo);

	return DPI;
}

void PDFwin::getRectImage_height(int pageNo, int rectNo, int height, CImage& image)
{
	SelectionRectangle& rect=::find(mRects, rectNo);
	int totalHeight=int((double)height/(rect.p2.y-rect.p1.y)+0.5);

	double DPI=getDPI_height(pageNo, rectNo, height);
	
	PDFDoc* _pdfDoc=mModel->_pdfDoc ;
	SplashOutputDev* _outputDev=mModel->_outputDev;

	int totalWidth=mModel->calcWidth_DPI(DPI, pageNo);
	
	int left=int(rect.p1.x*(double)totalWidth+0.5);
	int top=int(rect.p1.y*(double)totalHeight+0.5);
	int bottom=int(rect.p2.y*(double)totalHeight+0.5);
	int right=int(rect.p2.x*(double)totalWidth+0.5);

	_pdfDoc->displayPageSlice(_outputDev, pageNo+1, DPI, DPI, 0, false, true, false, left,top, right-left,height);
	
	SplashBitmap* bmp=_outputDev->takeBitmap();

	image.SetData(bmp->getWidth(), bmp->getHeight(), bmp->getDataPtr(), bmp->getRowSize());
	ASSERT(image.GetWidth()==right-left);
	ASSERT(image.GetHeight()==height);

	delete bmp;
}

void PDFwin::setStatus(const char* o)
{
	mLayout->find<Fl_Box>("Status")->copy_label(o);
	mLayout->redraw();
	Fl::check();
}

void SelectionRectangle::updateScreenCoord(PDFwin const& win)
{
	Int2D pos1=win.toWindowCoord(p1.x, p1.y);
	Int2D pos2=win.toWindowCoord(p2.x, p2.y);

	if(pos1.x > pos2.x) std::swap(pos1.x, pos2.x);
	if(pos1.y > pos2.y) std::swap(pos1.y, pos2.y);

	left=pos1.x;
	top=pos1.y;
	right=pos2.x+1;
	bottom=pos2.y+1;

}
bool SelectionRectangle ::isValid()	
{return (left+1)!=right;}

int SelectionRectangle ::handle(PDFwin const& win, TString const& event){
	Double2D   docx=win.toDocCoord(Fl::event_x(), Fl::event_y());
	//Int2D cursor=win.toWindowCoord(docx.x, docx.y);
	//ASSERT(cursor.x==Fl::event_x());
	//ASSERT(cursor.y==Fl::event_y());
	//Int2D cursor(Fl::event_x(), Fl::event_y());
	
	if(event=="push")
	{
		p1=docx;
		p2=p1;
		updateScreenCoord(win);
		return 1;
	}
	else if(event=="drag")
	{
		p2=docx;
		updateScreenCoord(win);
		return 1;
	}
	return 0;
}

PDFwin::PDFwin(int x, int y, int w, int h) 
: Fl_Double_Window(x,y,w,h)
{
	globalParams .reset( new GlobalParams());

	Fl_Box* b=new Fl_Box(0,0, w, h);
	b->box(FL_DOWN_FRAME);

	resizable(this);
	end();

	mModel=NULL;
	mState="none";

	// use zero index.
	mCurrPage=0;
	mSelectedRect=mRects.end();
	_filename=" ";
}

void PDFwin::load(const char* filename)
{
	_filename=" ";
	delete mModel;
	mModel=new PDFmodel();
	if(!mModel->load(filename))
	{
		mState.format("Error! opening %s", filename);
		delete mModel;
		mModel=NULL;
	}
	_filename=filename;
	mOutDir=TString(filename).left(-4);
	mCurrPage=0;
	pageChanged();
	redraw();
	
	if(mModel)
	{
		mLayout->findButton("Process current page")->activate();
		mLayout->findButton("Process all pages")->activate();
		mLayout->redraw();
	}
}

PDFwin::~PDFwin(void)
{
	delete mModel;
	//delete globalParams;
	//globalParams=NULL;

}

std::list<SelectionRectangle>::iterator PDFwin::findRect(int x, int y)
{
	std::list<SelectionRectangle>::iterator i;
	for(i=mRects.begin(); i!=mRects.end(); ++i)
	{
		if((*i).contains(Int2D(x, y)))
		{
			return i;
		}
	}
	return i;
}

int PDFwin::handle(int event)
{
	static bool mbMoveRect=false;
	static TRect mPushedRect;
	static Int2D mPushedCursor;
	switch(event)
	{
/* Receiving Drag and Drop */
    case FL_DND_ENTER:
    case FL_DND_RELEASE:
    case FL_DND_LEAVE:
    case FL_DND_DRAG:
        return 1;

	case FL_PASTE:
		{
			TString fn=Fl::event_text();

			if(fn.length() && fn.right(4).toUpper()==".PDF")
			{
				load(fn);
			}
			else
				Msg::msgBox("Error! Unknown file format");
			// If there is a callback registered, call it.
            // The callback must access Fl::event_text() to
            // get the string or file path(s) that was dropped.
            // Note that do_callback() is not called directly.
            // Instead it will be executed by the FLTK main-loop
            // once we have finished handling the DND event.
            // This allows caller to popup a window or change widget focus.
            //if(callback() && ((when() & FL_WHEN_RELEASE) || (when() & FL_WHEN_CHANGED)))
              //  Fl::add_timeout(0.0, Fl_DND_Box::callback_deferred, (void*)this);
            return 1;
		}
	case FL_KEYUP:
		if(mModel)
		{
			if(Fl::event_key()==FL_Page_Down)
			{
				mCurrPage=(mCurrPage+1)%mModel->_pdfDoc->getNumPages();
				pageChanged();
				redraw();
			}
			else if(Fl::event_key()==FL_Page_Up)
			{
				mCurrPage=(mCurrPage+mModel->_pdfDoc->getNumPages()-1)%mModel->_pdfDoc->getNumPages();
				pageChanged();
				redraw();
			}
			else if(Fl::event_key()==FL_Delete)
			{
				if(mSelectedRect!=mRects.end())
				{
					mRects.erase(mSelectedRect);
					mSelectedRect=mRects.end();
					redraw();
				}
			}
		}
		break;
	case FL_ENTER:
		return 1;
	case FL_LEAVE:
		cursor(FL_CURSOR_DEFAULT);
		return 1;
	case FL_MOVE:
		{
			if(mSelectedRect!=mRects.end() &&
				mSelectedRect->corner(Int2D(Fl::event_x(), Fl::event_y())))
			{
				cursor(FL_CURSOR_NWSE);
				return 1;
			}
			if(findRect(Fl::event_x(), Fl::event_y())==mRects.end())
				cursor(FL_CURSOR_DEFAULT);
			else
				cursor(FL_CURSOR_MOVE);
		}
		return 1;
	case FL_PUSH:
		if(mModel && Fl::event_button() == FL_LEFT_MOUSE)
		{
			if(mSelectedRect!=mRects.end())
			{
				int corner=mSelectedRect->corner(Int2D(Fl::event_x(), Fl::event_y()));
				if(corner)
				{
					if(corner==1)
						std::swap(mSelectedRect->p1, mSelectedRect->p2);
					cursor(FL_CURSOR_NWSE);
					return 1;
				}
			}
			
			std::list<SelectionRectangle>::iterator i
				=findRect(Fl::event_x(), Fl::event_y());

			if(i!=mRects.end())
			{
				// move selected rect
				mSelectedRect=i;
				redraw();
				mbMoveRect=true;
				mPushedCursor.x=Fl::event_x();
				mPushedCursor.y=Fl::event_y();
				mPushedRect=*mSelectedRect;
				cursor(FL_CURSOR_MOVE);

				return 1;
			}

			

			mRects.push_back(SelectionRectangle());
			mSelectedRect=mRects.end();
			mSelectedRect--;
			if(selectedRect().handle(*this, "push"))
			{
				redraw();
				cursor(FL_CURSOR_HAND);
				return 1;
			}
		}
		return 0;
		break;
	case FL_DRAG:
		{
			if(mbMoveRect)
			{
				cursor(FL_CURSOR_MOVE);

				mSelectedRect->p1=
					toDocCoord(mPushedRect.left+Fl::event_x()-mPushedCursor.x,
							mPushedRect.top+Fl::event_y()-mPushedCursor.y);
				mSelectedRect->p2=
					toDocCoord(mPushedRect.right+Fl::event_x()-mPushedCursor.x,
							mPushedRect.bottom+Fl::event_y()-mPushedCursor.y);

				mSelectedRect->updateScreenCoord(*this);
				redraw();
				return 1;
			}

			if(selectedRect().handle(*this, "drag"))
				redraw();
			cursor(FL_CURSOR_HAND);

			return 1;
		}
		break;
	case FL_RELEASE:
		{
			mbMoveRect=false;
			
			if(mRects.size())
			{
				if(selectedRect().isValid())
				{
					if(selectedRect().p1.x> selectedRect().p2.x)
						std::swap(selectedRect().p1.x, selectedRect().p2.x);
				
					if(selectedRect().p1.y> selectedRect().p2.y)
						std::swap(selectedRect().p1.y, selectedRect().p2.y);
				}
				else
				{
					mRects.erase(mSelectedRect);
					mSelectedRect=mRects.end();
					redraw();
				}
				
			}
			return 1;
		}
		break;
	}
	return Fl_Double_Window::handle(event);
}

void PDFwin::onIdle()
{
}

Int2D PDFwin::toWindowCoord(double x, double y) const
{
	CImage* bmp=mModel->getPage(w()-2, h()-2, mCurrPage);

	int x1=1;
	int y1=1;
	
	// draw pdf at center of the panel
	if(bmp->GetWidth()<w()-2)
		x1+=(w()-2-bmp->GetWidth())/2;

	if(bmp->GetHeight()<h()-2)
		y1+=(h()-2-bmp->GetHeight())/2;


	int x2=x1+bmp->GetWidth();
	int y2=y1+bmp->GetHeight();

	return Int2D(sop::interpolateInt(x, x1,x2), sop::interpolateInt(y, y1, y2)); 
}

Double2D   PDFwin::toDocCoord(int x, int y) const
{
	CImage* bmp=mModel->getPage(w()-2, h()-2, mCurrPage);

	int x1=1;
	int y1=1;
	
	// draw pdf at center of the panel
	if(bmp->GetWidth()<w()-2)
		x1+=(w()-2-bmp->GetWidth())/2;

	if(bmp->GetHeight()<h()-2)
		y1+=(h()-2-bmp->GetHeight())/2;

	int x2=x1+bmp->GetWidth();
	int y2=y1+bmp->GetHeight();

	return Double2D(sop::clampMap(double(x), double(x1), double(x2)), 
		sop::clampMap(double(y), double(y1), double(y2)));
}

bool DeleteAllFiles(const char* path, const char* mask,bool bConfirm);
void PDFwin::deleteAllFiles()
{
	DeleteAllFiles(mOutDir, "*",true);
}

void PDFwin::deleteAllFilesWithoutConfirm()
{
	DeleteAllFiles(mOutDir, "*",false);

}

void PDFwin::setCurPage(int pageNo)
{
	mCurrPage=pageNo;
	pageChanged();
	redraw();
}

void PDFwin::pageChanged()
{
	int ww=w()-2;
	int hh=h()-2;

	if(!mModel) return;

	if(!mModel->cacheValid(ww, hh, mCurrPage))
	{
		mState="load";
		redraw();
		Fl::check();
		mState="none";
	}

	CImage* bmp_orig=mModel->getPage(ww, hh, mCurrPage);
	
	
	if(mLayout->findCheckButton("Use automatic segmentation")->value())
	{
		CImage temp;
		temp.CopyFrom(*bmp_orig);
#ifdef DEBUG_FONT_DETECTION
		CImage* bmp=bmp_orig;
#else
		CImage* bmp=&temp;
#endif	
		{

			intmatrixn& textCache=*mModel->_textCache[mCurrPage];


#ifdef USE_FONT_DETECTION
			const bool drawLetterBoundingBox=true;
			const bool drawDetectedSpace=true;
#else
			const bool drawLetterBoundingBox=false;
			const bool drawDetectedSpace=false;
#endif
			if(drawLetterBoundingBox)
			{
				CImagePixel ip(bmp);
				for(int i=0; i<textCache.rows()-1; i++)
				{
					int* info=textCache[i];
					int c=info[TC_c];
					int x=info[TC_x];
					int y=info[TC_y];
					int w=info[TC_w];
					int h=info[TC_h];
					int fx=info[TC_fx];
					int fy=info[TC_fy];
					if (std::abs(y-textCache[i+1][TC_y])<=3) // draw excluding trailing space
						{

#ifdef DEBUG_FONT_DETECTION
					ip.DrawLineBox(TRect(x-fx,y-fy, x-fx+w, y-fy+h), CPixelRGB8(0,0,0));				
#else
					ip.DrawBox(TRect(x-fx,y-fy, x-fx+w, y-fy+h), CPixelRGB8(0,0,0));
#endif
					ip.DrawHorizLine(x, y, w, CPixelRGB8(255,0,0));
						}
					// else printf("%d %d %d %d\n", x,y, textCache[i+1][TC_x], textCache[i+1][TC_y]);
				}
			}


			FlLayout* layout=mLayout->findLayout("Automatic segmentation");
			double min_text_gap=layout->findSlider("MIN text-gap")->value();
			if(drawDetectedSpace)
			{
				CImagePixel ip(bmp);
				for(int i=1; i<textCache.rows(); i++)
				{
					int* pinfo=textCache[i-1];
					int* info=textCache[i];

					if(pinfo[TC_y]==info[TC_y])
					{						
						int prevEnd=pinfo[TC_x]-pinfo[TC_fx]+pinfo[TC_w]-1;
						int curStart=info[TC_x]-info[TC_fx];

						int bottom=MIN(pinfo[TC_y]-pinfo[TC_fy], info[TC_y]-info[TC_fy]);
						int top=MAX(pinfo[TC_y]-pinfo[TC_fy]+pinfo[TC_h], info[TC_y]-info[TC_fy]+info[TC_h]);
						int space_thr=MAX(pinfo[TC_h], pinfo[TC_w]);
						space_thr=MAX(space_thr, info[TC_h]);
						space_thr=MAX(space_thr, info[TC_w]);
						space_thr*=min_text_gap;

						if(curStart-prevEnd<=space_thr && curStart>prevEnd)
						{
							// mark as space.
							ip.DrawBox(TRect(prevEnd, bottom, curStart, top), CPixelRGB8(0,0,0));
						}
					}					
				}

			}


		}	

		SummedAreaTable t(*bmp);//bmp->getWidth(), bmp->getHeight(), bmp->getDataPtr(), bmp->getRowSize());

		
		


		FlLayout* layout=mLayout->findLayout("Automatic segmentation");
		double min_gap_percentage=layout->findSlider("MIN gap")->value();
		double margin_percentage=layout->findSlider("Margin")->value();
		int thr_white=layout->findSlider("white point")->value();
		double max_width=1.0/layout->findSlider("N columns")->value();
		double cropT=layout->findSlider("Crop T")->value()/100.0;
		double cropB=layout->findSlider("Crop B")->value()/100.0;
		double cropL=layout->findSlider("Crop L")->value()/100.0;
		double cropR=layout->findSlider("Crop R")->value()/100.0;


//		TRect domain(0,0, bmp->GetWidth(), bmp->GetHeight());

		TRect domain(cropL*bmp->GetWidth(),cropT*bmp->GetHeight(), (1-cropR)*bmp->GetWidth()
, (1-cropB)*bmp->GetHeight());

	
		ImageSegmentation s(t, true, domain, 0, min_gap_percentage, thr_white);
		s.segment();
		std::list<TRect> results;
		s.getResult(results, max_width, margin_percentage);

		/*

		std::list<TRect> results;
		int min_gap=int((double)bmp->GetWidth()*min_gap_percentage*0.01 +0.5);
		int min_box_size=100;	// 10�ȼ������� �ڽ��� ������.

		CImagePixel orig(bmp_orig);

		std::list<index2> LRcorners;
		std::list<index2> ULcorners;
		for(int i=0, ni=bmp->GetWidth()-min_box_size-min_gap; i<ni; i++)
		{
			for(int j=0, nj=bmp->GetHeight()-min_box_size-min_gap; j<nj; j++)
			{
				// search for LR corner
				TRect outerBox(i, j, i+min_box_size+min_gap, j+min_box_size+min_gap);
				TRect innerBox(i, j, i+min_box_size, j+min_box_size);
				
				int sum1=t.sum(outerBox);
				int sum2=t.sum(innerBox);

				int area=outerBox.Width()*outerBox.Height()-
					innerBox.Width()*innerBox.Height();
				if(sum1-sum2 >= thr_white*area)
				{
					//orig.SetPixel(i+min_box_size+min_gap/2,j+min_box_size+min_gap/2,CPixelRGB8(255,0,0));

					LRcorners.push_back(index2(i+min_box_size+min_gap/2,j+min_box_size+min_gap/2));
				}

				// search for UL corner
				{
					TRect outerBox(i, j, i+min_box_size+min_gap, j+min_box_size+min_gap);
					TRect innerBox(i+min_gap, j+min_gap, i+min_box_size+min_gap, j+min_box_size+min_gap);
					
					int sum1=t.sum(outerBox);
					int sum2=t.sum(innerBox);

					int area=outerBox.Width()*outerBox.Height()-
						innerBox.Width()*innerBox.Height();
					if(sum1-sum2 >= thr_white*area)
					{
			//			orig.SetPixel(i+min_gap/2,j+min_gap/2,CPixelRGB8(0,0,255));
						ULcorners.push_back(index2(i+min_gap/2,j+min_gap/2));
					}
				}
			}
		}


		{
			std::list<index2>::iterator i;

			for(i=ULcorners.begin(); i!=ULcorners.end(); ++i)
			{
				orig.SetPixel((*i).x(), (*i).y(), CPixelRGB8(0,0,255));
			}
		}
		
		*/
		mRects.clear();

		int x=toWindowCoord(0,0).x;
		int y=toWindowCoord(0,0).y;

		std::list<TRect>::iterator i;
		for(i=results.begin(); i!=results.end(); i++)
		{
			mRects.push_back(SelectionRectangle());
			mSelectedRect=mRects.end();
			mSelectedRect--;
			SelectionRectangle& mRect=selectedRect();
			mRect.p1=toDocCoord((*i).left+x, (*i).top+y);
			mRect.p2=toDocCoord((*i).right+x, (*i).bottom+y);
			mRect.updateScreenCoord(*this);
		}
	}
}

void PDFwin::draw()
{
	Fl_Double_Window::draw();


	int ww=w()-2;
	int hh=h()-2;

	if(!mModel) return;

	if(mState=="load")
	{
		fl_draw("loading page (please wait)", 0,0,w(), h(), FL_ALIGN_CENTER);
		return;
	}

	CImage* bmp=mModel->getPage(ww, hh, mCurrPage);
	std::list<SelectionRectangle> ::iterator i;
	for(i=mRects.begin(); i!=mRects.end(); ++i)
		(*i).updateScreenCoord(*this);
	
	int x=toWindowCoord(0,0).x;
	int y=toWindowCoord(0,0).y;
	//fl_draw_image(bmp->getDataPtr(), x, y, bmp->getWidth() , bmp->getHeight(), 3, bmp->getRowSize());
	
	fl_draw_CImage(*bmp, TRect(0, 0, bmp->GetWidth(), bmp->GetHeight()), x, y);

	// draw rects
	int c=0;

	for(i=mRects.begin(); i!=mRects.end(); ++i)
	{
		SelectionRectangle& mRect=*i;
		if(mRect.isValid())
		{
			c++;
			Fl_Boxtype type=FL_SHADOW_FRAME;
			if(i!=mSelectedRect)
				type=FL_BORDER_FRAME;

			fl_draw_box( type, mRect.left, mRect.top, mRect.Width(), mRect.Height(), FL_BLACK);

			TString temp;
			temp.format("Crop %d", c);
			fl_draw(temp, mRect.left, mRect.top, mRect.Width(), mRect.Height(), FL_ALIGN_TOP);

			if(i==mSelectedRect)
			{
				for(int j=0; j<2; j++)
				{
					int x=mRect.left;
					int y=mRect.top;
					if(j==1)
					{
						x=mRect.right;
						y=mRect.bottom;
					}
					fl_color(FL_WHITE);
					fl_begin_polygon();
					fl_arc(x, y, 3, 0,360);
					fl_end_polygon();
					fl_color(FL_BLACK);
					fl_begin_line();
					fl_arc(x-3, y-3, 7, 7, 0,360);
					fl_end_line();
				}
			}

		}
	}

	/*TString temp;
	temp.format("selected %d", mSelectedRect);
	fl_draw(temp, 0,0,w(), h(), FL_ALIGN_CENTER);
	*/

	
FlLayout* layout=mLayout->findLayout("Automatic segmentation");
double cropT=layout->findSlider("Crop T")->value()/100.0;
double cropB=layout->findSlider("Crop B")->value()/100.0;
double cropL=layout->findSlider("Crop L")->value()/100.0;
double cropR=layout->findSlider("Crop R")->value()/100.0;

int wCropL=toWindowCoord(cropL,cropT).x;
int wCropT=toWindowCoord(cropL,cropT).y;
int wCropR=toWindowCoord(cropR,cropB).x;
int wCropB=toWindowCoord(cropR,cropB).y;

fl_draw_box( FL_BORDER_FRAME, 0, 0, ww, wCropT, FL_BLACK);
fl_draw_box( FL_BORDER_FRAME, 0, hh-wCropB, ww, wCropB, FL_BLACK);
fl_draw_box( FL_BORDER_FRAME, 0, 0, wCropL, hh, FL_BLACK);
fl_draw_box( FL_BORDER_FRAME, ww-wCropR, 0, wCropR, hh, FL_BLACK);
}

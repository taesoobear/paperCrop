//
// ImageSegmentation.cpp 
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
#include "ImageSegmentation.h"
#include "image/ImagePixel.h"
#include "math/Operator.h"
#include "image/ImageProcessor.h"
#include "math/intervals.h"

void SummedAreaTable::_init()
{
	CImagePixel pixel(mImage);
	_width=mImage->GetWidth();
	_height=mImage->GetHeight();
	mTable.setSize(_width+1, _height+1);

	mTable.row(0).setAllValue(0);
	mTable.column(0).setAllValue(0);


	for(int y=0; y<_height; y++)
	{
		for(int x=0; x<_width; x++)
		{
			int value=((int)pixel.Pixel(x,y).R+(int)pixel.Pixel(x,y).G+(int)pixel.Pixel(x,y).B)/3;	// 8bit precision. 
			mTable(x+1,y+1)=mTable(x,y+1)+mTable(x+1,y)-mTable(x,y)+value;
		}
	}

	//Imp::SafeDelete(Imp::DrawMatrix(mTable), "summed.png");

	//#define TEST_SUMMED_AREA_TABLE
#ifdef TEST_SUMMED_AREA_TABLE
	CImage& image=mImage;
	intvectorn vertHistogram2;
	vertHistogram2.setSize(image.GetHeight());

	getVertHistogram(TRect(0,0, image.GetWidth(), image.GetHeight()), vertHistogram2);

	intvectorn vertHistogram;
	vertHistogram.setSize(image.GetHeight());

	for(int i=0; i<image.GetHeight(); i++)
	{
		CPixelRGB8 avg=image.GetHorizLine(i).average();
		vertHistogram[i]=((int)avg.R+(int)avg.G+(int)avg.B)/3;
	}

	vertHistogram.toVectorn().op0(v0::drawSignals("vertHistogram.png"));
	vertHistogram2.toVectorn().op0(v0::drawSignals("vertHistogram2.png"));
#endif

}
SummedAreaTable::SummedAreaTable(int width, int height, uchar* dataPtr, int stride)
{
	mImage=new CImage();
	mImageIsReference=false;
	mImage->SetData(width, height, dataPtr, stride);

	_init();	
}

SummedAreaTable::SummedAreaTable(CImage const& image)
{
	mImage=(CImage*)&image;
	mImageIsReference=true;
	_init();
}
SummedAreaTable::~SummedAreaTable()
{
	if(!mImageIsReference)
		delete mImage;
}
int SummedAreaTable::sum(TRect const& rect) const
{
	return mTable(rect.right, rect.bottom)-mTable(rect.right, rect.top)- mTable(rect.left, rect.bottom)+mTable(rect.left, rect.top);
}

int SummedAreaTable::average(TRect const& rect) const
{
	return sum(rect)/((rect.right-rect.left)*(rect.bottom-rect.top));
}

void SummedAreaTable::getVertHistogram(TRect const& domain, intvectorn& histogram) const
{
	const CImage& image=*mImage;
	histogram.setSize(domain.Height());

	TRect temp;
	temp.left=domain.left;
	temp.right=domain.right;
	for(int i=domain.top; i<domain.bottom; i++)
	{
		temp.top=i;
		temp.bottom=i+1;
		histogram[i-domain.top]=average(temp);
	}
}

void SummedAreaTable::getHorizHistogram(TRect const& domain, intvectorn& histogram) const
{
//	const CImage& image=mImage;
	histogram.setSize(domain.Width());

	TRect temp;
	temp.top=domain.top;
	temp.bottom=domain.bottom;
	for(int i=domain.left; i<domain.right; i++)
	{
		temp.left=i;
		temp.right=i+1;
		histogram[i-domain.left]=average(temp);
	}
}

// thr_white보다 밝으면 여백으로 간주. 여백의 두께가 단축 기준으로 min_gap_percentage보다 좁으면 여백으로 처리 안함. 
ImageSegmentation::ImageSegmentation(SummedAreaTable const& s, bool bHorizSplit, TRect const& domain,
									 ImageSegmentation* parent, double min_gap_percentage, int thr_white)
									 :mTable(s),
mDomain(domain)
{
	mParent=parent;
	mMinGapPercentage=min_gap_percentage;
	mThrWhite=thr_white;
	mbHorizSplit=bHorizSplit;
}

ImageSegmentation::~ImageSegmentation()
{
	for(int i=0; i<mChildren.size(); i++)
		delete mChildren[i];

	mChildren.clear();
}

void ImageSegmentation::segment()
{
	int min_gap=int((double)mTable.getImageWidth()*mMinGapPercentage*0.01 +0.5);

	intvectorn histogram;

	if(mbHorizSplit)
		mTable.getVertHistogram(mDomain, histogram);
	else
		mTable.getHorizHistogram(mDomain, histogram);

	intIntervals validRegion;
	bool bWhite=true;
	int i;
	for(i=0; i<histogram.size(); i++)
	{
		if(bWhite)// search non-white region
		{
			if(histogram[i]<mThrWhite)
			{
				// check min_gap_percentage
				if(validRegion.size()&& i-validRegion.end(validRegion.size()-1) < min_gap)
				{
					//printf("skipping narrow white region\n");
				}
				else validRegion.pushBack(i,i+1);
				bWhite=false;
			}
		}
		else // search white region
		{
			if(histogram[i]>=mThrWhite)
			{
				validRegion.end(validRegion.size()-1)=i;
				bWhite=true;
			}
		}
	}
	if(!bWhite)
		validRegion.end(validRegion.size()-1)=i;

	if(validRegion.size()==1 && validRegion.start(0)==0 && validRegion.end(0)==i)
		return;
	
	mChildren.resize(validRegion.size());

	TRect domain;
	
	for(int i=0; i<mChildren.size(); i++)
	{

		if(mbHorizSplit)
		{
			domain.left=mDomain.left;
			domain.right=mDomain.right;
			domain.top=validRegion.start(i)+mDomain.top;
			domain.bottom=validRegion.end(i)+mDomain.top;
		}
		else
		{
			domain.left=validRegion.start(i)+mDomain.left;
			domain.right=validRegion.end(i)+mDomain.left;
			domain.top=mDomain.top;
			domain.bottom=mDomain.bottom;
		}

		mChildren[i]=new ImageSegmentation(mTable, !mbHorizSplit, domain, this, mMinGapPercentage, mThrWhite);
		mChildren[i]->segment();
	}
}

bool overlapHoriz(TRect const& rect, int x)
{
	if(x>=rect.left && x<=rect.right)
		return true;
	return false;
}

bool overlapVert(TRect const& rect, int y)
{
	if(y>=rect.top && y<=rect.bottom)
		return true;
	return false;
}

struct HorizLine
{
	int left, right;
	int y;
	HorizLine(TRect const& rect, int i)
	{
		left=rect.left; right=rect.right; 
		if(i==0)
			y=rect.top;
		else
			y=rect.bottom;
	}
};

struct VertLine
{
	int top, bottom;
	int x;
	VertLine(TRect const& rect, int i)
	{
		top=rect.top; bottom=rect.bottom; 
		if(i==0)
			x=rect.left;
		else
			x=rect.right;
	}
};

bool intersect(HorizLine const& h, VertLine const& l)
{
	if(l.x < h.left || l.x > h.right)
		return false;
	if(h.y < l.top || h.y > l.bottom)
		return false;
	return true;
}

bool contains(TRect const& a, TRect const & b, double margin=0)
{
	if(a.contains(Int2D(b.left, b.top), margin) && a.contains(Int2D(b.right, b.bottom), margin))
		return true;
	return false;
}

bool intersect(TRect const& a, TRect const& b)
{
	// VE test
	if(a.contains(Int2D(b.left, b.top)) || a.contains(Int2D(b.left, b.bottom)) ||
		a.contains(Int2D(b.right, b.top)) || a.contains(Int2D(b.right, b.bottom)) )
		return true;

	
	if(b.contains(Int2D(a.left, a.top)) || b.contains(Int2D(a.left, a.bottom)) ||
		b.contains(Int2D(a.right, a.top)) || b.contains(Int2D(a.right, a.bottom)) )
		return true;

	// EE test
	if( intersect(HorizLine(a,0), VertLine(b,0)) ||
		intersect(HorizLine(a,0), VertLine(b,1)) ||
		intersect(HorizLine(a,1), VertLine(b,0)) ||
		intersect(HorizLine(a,1), VertLine(b,1)) ||
		intersect(HorizLine(b,0), VertLine(a,0)) ||
		intersect(HorizLine(b,0), VertLine(a,1)) ||
		intersect(HorizLine(b,1), VertLine(a,0)) ||
		intersect(HorizLine(b,1), VertLine(a,1)))
		return true;

	return false;
}

TRect merge(TRect const & prevRect, TRect const & rect)
{
	TRect mergeRect;
	mergeRect.left=MIN(prevRect.left, rect.left);
	mergeRect.right=MAX(prevRect.right, rect.right);
	mergeRect.top=prevRect.top;
	mergeRect.bottom=rect.bottom;
	return mergeRect;
}

bool mergeable(std::vector<TRect> & results, int i, int j, int max_width, int min_gap)
{
	TRect& prevRect=results[i];
	TRect& rect=results[j];

	if(prevRect.Width() < max_width && rect.Width() < max_width 
		&& rect.bottom >= prevRect.bottom-min_gap
		&& rect.top >= prevRect.top-min_gap)
	{
		if((rect.left -min_gap <= prevRect.left  && prevRect.right <=rect.right +min_gap) ||
			(prevRect.left -min_gap  <=rect.left  && rect.right <=prevRect.right +min_gap) )
		{
			bool bFindInterupt=false;

			for(int k=0; k<results.size(); k++)
			{
				if(k==i || k==j) continue;

				TRect& t=results[k];

				TRect mergeRect=merge(prevRect, rect);
				mergeRect.enlarge(min_gap);

				if(intersect(mergeRect, t) && !contains(mergeRect, t))
					bFindInterupt=true;
			}
			if(!bFindInterupt) return true;
		}
	}
	return false;
}


void ImageSegmentation::getResult(std::list<TRect>& results, double _max_width, double margin_percentage)
{
	results.clear();

	int max_width=int((double)mTable.getImageWidth()*_max_width+0.5);
	int min_gap=int((double)mTable.getImageWidth()*mMinGapPercentage*0.01 +0.5);
	int margin=int((double)mTable.getImageWidth()*margin_percentage*0.01 +0.5);

	std::vector<TRect> _results;
	_getResult(_results);

	// post-processing: merge small rectangles into one when possible.

	for(int i=0; i<_results.size(); ++i)
	{
		for(int j=i+1; j<_results.size(); ++j)
		{
			if(mergeable(_results, i, j, max_width, min_gap))
			{
				TRect& prevRect=_results[i];
				TRect& rect=_results[j];

				prevRect=merge(prevRect, rect);

				TRect mergeRect=prevRect;
				mergeRect.enlarge(min_gap);
				
				rect=TRect(-1,-1,-1,-1);
				for(int k=0; k<_results.size(); k++)
				{
					if(k==i || k==j) continue;
					TRect& t=_results[k];
					if(contains(mergeRect, t))
						t=TRect(-1,-1,-1,-1);
				}

			}
		}
	}

	for(int i=0; i<_results.size(); ++i)
	{
		if(_results[i].left!=-1)
		{
			//_results[i].enlarge(-1*min_gap);
			_results[i].enlarge(margin);
			results.push_back(_results[i]);
		}
	}
}

void ImageSegmentation::_getResult(std::vector<TRect>& results)
{
	if(mChildren.size())
	{
		for(int i=0; i<mChildren.size(); i++)
		{
			mChildren[i]->_getResult(results);
		}
	}
	else
	{
		results.push_back(mDomain);
	}
}




// thr_white보다 밝으면 여백으로 간주. 여백의 두께가 min_gap_pixels보다 좁으면 여백으로 처리 안함, max_gap 보다 넓으면 max_gap만큼만 사용되도록한다.
void detectLines(SummedAreaTable const& s, bool bHorizSplit, TRect const& domain, int min_gap, int max_gap, int thr_white, intIntervals& lines)
{
	intvectorn histogram;

	if(bHorizSplit)
		s.getVertHistogram(domain, histogram);
	else
		s.getHorizHistogram(domain, histogram);

	intIntervals validRegion;	// non-white regions
	bool bWhite=true;
	int i;
	for(i=0; i<histogram.size(); i++)
	{
		if(bWhite) // search non-white region
		{
			if(histogram[i]<thr_white)
			{
				// check min_gap_percentage
				if(validRegion.size()&& i-validRegion.end(validRegion.size()-1) < min_gap)
				{
					//printf("skipping narrow white region\n");
				}
				else	// valid region start here. 
					validRegion.pushBack(i,i+1);
				bWhite=false;
			}
		}
		else // search white region
		{
			if(histogram[i]>=thr_white)
			{
				// valid region end here.
				validRegion.end(validRegion.size()-1)=i;
				bWhite=true;
			}
		}
	}
	if(!bWhite)
		validRegion.end(validRegion.size()-1)=i;

	int half_max_gap=max_gap/2;
	int start, end;
	if(bHorizSplit)
	{
		start=domain.top;
		end=domain.bottom;
	}
	else
	{
		start=domain.left;
		end=domain.right;
	}

	
	lines.resize(0);

	for(int region=0; region<validRegion.size(); region++)
	{
		int startWhiteAboveRegion=region==0? start: validRegion.end(region-1);
		int endWhiteBelowRegion=region==validRegion.size()-1? end: validRegion.start(region+1);

		int midAbove=(startWhiteAboveRegion+validRegion.start(region))/2;
		int midBelow=(endWhiteBelowRegion+validRegion.end(region))/2;
		lines.pushBack(MAX(midAbove, validRegion.start(region)-half_max_gap),
						MIN(midBelow, validRegion.end(region)+half_max_gap));
	}
}

void trimVertSpaces(CImage& inout, int min_gap, int max_gap, int thr_white) 
{
	CImage in;
	in.CopyFrom(inout);

	SummedAreaTable s(in);
	TRect domain(0,0, in.GetWidth(), in.GetHeight());
	intIntervals lines;
	
	detectLines(s, true, domain, min_gap, max_gap, thr_white, lines);

	int h=0;
	for(int l=0; l<lines.size(); l++) h+=lines.end(l)-lines.start(l);

	if (h==0) h=10;

	inout.Create(in.GetWidth(), h);

	h=0;
	for(int l=0; l<lines.size(); l++) 
	{
		TRect r(0, lines.start(l), in.GetWidth(), lines.end(l));
		Imp::blit(inout, in, r, 0, h);
		h+=lines.end(l)-lines.start(l);
	}
}

void reflow(CImage& inout, int desired_width, int min_gap, int max_gap, int thr_white, double right_max_margin, int figure_min_height)
{
	CImage in;
	in.CopyFrom(inout);

	SummedAreaTable s(in);
	TRect domain(0,0, in.GetWidth(), in.GetHeight());
	intIntervals lines;
	
	detectLines(s, true, domain, min_gap, max_gap, thr_white, lines);

	int nsplit=in.GetWidth()/desired_width+1;
	ASSERT(nsplit==2);	// at the moment...

	std::vector<TRect> splitLines;
	intvectorn leftMargin;
	for(int l=0; l<lines.size(); l++)
	{
		if(lines.end(l)-lines.start(l)>figure_min_height)
		{
			// figure mode.
			leftMargin.pushBack(-1);// denotes figure.
			splitLines.push_back(TRect(0, lines.start(l), in.GetWidth(), lines.end(l)));
			continue;
		}

		intIntervals words;
		TRect domain(0, lines.start(l), in.GetWidth(), lines.end(l));
		// detect words
		detectLines(s, false, domain, 0, 0, thr_white, words);

		int space=words.start(0);
		
		
		int space2=in.GetWidth()-words.end(words.size()-1);
		int nextStart=in.GetWidth()/2;
		for(int ns=0; ns<nsplit-1; ns++)
		{

			// example 1:
			// --------
			// if desired_width==5->
			// line1: ---    or  ----  or -----
			// line2: -----      ----     ---

			// example 2:
			// ~~------		// 시작부분의 space가 둘로 쪼개진다.
			// if desired_width==5->
			// line1: ~--    or  ~---  or ~----
			// line2: ~----      ~---     ~---


			int searchStart=desired_width*0.8+space/nsplit+1;
			int searchEnd=desired_width+space/nsplit-1;

			int maxLen=0;
			int argMax=-1;
			
			for(int wordGap=0; wordGap<words.size()+1; wordGap++)
			{
				int wordGapStart, wordGapEnd;
				if(wordGap==0)
				{
					wordGapStart=0;
					wordGapEnd=words.start(wordGap);
				}
				else if(wordGap==words.size())
				{
					wordGapStart=words.end(words.size()-1);
					wordGapEnd=in.GetWidth();					
				}
				else
				{
					wordGapStart=words.end(wordGap-1);
					wordGapEnd=words.start(wordGap);
				}

				if(wordGapEnd< searchStart || wordGapStart>= searchEnd)
					continue;

				int wordGapLen=wordGapEnd-wordGapStart;
				if(wordGapLen>maxLen)
				{
					maxLen=wordGapLen;
					argMax=wordGap;
				}				
			}

			TRect rect_in;
			rect_in.left=-100;
			if(argMax==-1)
			{
				rect_in=TRect(space, lines.start(l), in.GetWidth()/2, lines.end(l));
				splitLines.push_back(rect_in);
				leftMargin.pushBack(space/2);				
			}
			else
			{
				if(argMax==0)
					// empty line.
					nextStart=words.start(0);
				else
				{
					rect_in=TRect(space, lines.start(l), words.end(argMax-1), lines.end(l));
					splitLines.push_back(rect_in);
					leftMargin.pushBack(space/2);
					if(argMax<words.size())
						nextStart=words.start(argMax);
					else
						// empty line 2.
						nextStart=-1;
				}
			}
#ifdef _DEBUG
			if(rect_in.left!=-100)
			{
				ASSERT(rect_in.left>=0);
				ASSERT(rect_in.right<=in.GetWidth());
				ASSERT(rect_in.top>=0);
				ASSERT(rect_in.bottom<=in.GetHeight());
				ASSERT(rect_in.right>rect_in.left);
				ASSERT(rect_in.bottom>rect_in.top);
				ASSERT(leftMargin.back()+rect_in.Width()<=desired_width);
				/*if(leftMargin.back()+rect_in.Width()>desired_width)
				{
					leftMargin.back()-=leftMargin.back()+rect_in.Width()-desired_width;
					ASSERT(leftMargin.back()>=0);
				}*/
			}
#endif
		}
		if(nextStart!=-1 && nextStart<words.end(words.size()-1))
		{
			TRect rect_in(nextStart, lines.start(l), words.end(words.size()-1), lines.end(l));
			splitLines.push_back(rect_in);
			if(leftMargin.size())
				leftMargin.pushBack(leftMargin.back());
			else
				leftMargin.pushBack(0);

			ASSERT(rect_in.left>=0);
			ASSERT(rect_in.right<=in.GetWidth());
			ASSERT(rect_in.top>=0);
			ASSERT(rect_in.bottom<=in.GetHeight());
			ASSERT(rect_in.right>rect_in.left);
			ASSERT(rect_in.bottom>rect_in.top);
			ASSERT(leftMargin.back()+rect_in.Width()<=desired_width);

		}
	}

	int h=0;
	for(int sl=0; sl<splitLines.size(); sl++) 
	{
		if(leftMargin[sl]==-1)
		{
			TRect &r=splitLines[sl];
			int newHeight=r.Height()* desired_width/r.Width();
			h+=newHeight;
		}
		else
			h+=splitLines[sl].Height();
	}
	
	inout.Create(desired_width, h);

	CImagePixel p(&inout);
	p.DrawBox(TRect(0,0, inout.GetWidth(), inout.GetHeight()), CPixelRGB8(255,255,255));

	h=0;
	for(int sl=0; sl<splitLines.size(); sl++) 
	{
		if(leftMargin[sl]==-1)
		{
			// figure mode
			CImage temp;
			TRect &r=splitLines[sl];
			Imp::crop(temp, in, r.left, r.top, r.right, r.bottom);
			int newHeight=r.Height()* desired_width/r.Width();

			Imp::resize(temp, desired_width, newHeight);
			Imp::blit(inout, temp, TRect(0,0, temp.GetWidth(), temp.GetHeight()), 0,h);
			h+=newHeight;
		}
		else
		{
			Imp::blit(inout, in, splitLines[sl], leftMargin[sl], h);				
			h+=splitLines[sl].Height();
		}
	}
}

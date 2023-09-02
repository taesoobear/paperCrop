#ifndef IMAGE_SEGMENTATION_H_
#define IMAGE_SEGMENTATION_H_
#pragma once

//
// ImageSegmentation.h
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

#include "image/Image.h"
class SummedAreaTable
{
	CImage* mImage;
	bool mImageIsReference;
	int _width, _height;

public:
	int getImageWidth()	const {return _width;} 
	int getImageHeight() const {return _height;}

	intmatrixn mTable;
	SummedAreaTable(int width, int height, uchar* dataPtr, int stride);
	SummedAreaTable(CImage const& image);
	~SummedAreaTable();

	void _init();
	int sum(TRect const& rect) const;
	int average(TRect const& rect) const;
	void getVertHistogram(TRect const& domain, intvectorn& histogram) const;
	void getHorizHistogram(TRect const& domain, intvectorn& histogram) const;
};

class ImageSegmentation
{
	SummedAreaTable const& mTable;
	TRect mDomain;
	ImageSegmentation* mParent;
	std::vector<ImageSegmentation*> mChildren;
	bool mbHorizSplit;
	double mMinGapPercentageX; //relative to image width
	double mMinGapPercentageY; //relative to image width

	int mThrWhite;
public:
	//ImageSegmentation(SummedAreaTable const& s, bool bHorizSplit, TRect const& domain,ImageSegmentation* mParent=NULL, double min_gap_percentage=2, int thr_white=254);
	ImageSegmentation(SummedAreaTable const& s, bool bHorizSplit, TRect const& domain,ImageSegmentation* mParent, double min_gap_percentageX, double min_gap_percentageY, int thr_white);
	~ImageSegmentation();

	void segment();
	void getResult(std::list<TRect>& results, double _max_width, double margin);

	void _getResult(std::vector<TRect>& results);
};
#endif

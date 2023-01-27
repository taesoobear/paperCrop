//
// ImageProcessor.h 
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

#include "Image.h"

namespace Imp
{
	// new API
	void drawBox(CImage& inout, TRect const& t, int R, int G, int B);
	void sharpen(CImage& inout, double factor, int iterations);
	void contrast(CImage& inout, double factor);
	void gammaCorrect(CImage& inout, double factor);
	void dither(CImage& inout, int levels);
	void resize(CImage& inout, int width, int height);
	void downsample4(CImage& out, CImage const& in);
	void blit(CImage& out, CImage const& in, TRect const& rect_in, int x, int y);
	void concatVertical(CImage& out, CImage const& a, CImage const& b);
	void crop(CImage& out, CImage const& in, int left, int top, int right, int bottom);
	void rotateRight(CImage& other);
	void rotateLeft(CImage& other);

	// old API (but not deprecated yet)
	CImage* Clone(CImage* pInput);
	void Crop(CImage* pOutput, CImage *pInput, const TRect &rect);	//!< 이미 생성된 output이미지에 크롭할 영역 저장
	CImage* Crop(CImage* pInput, const TRect& rect);	//!< crop된 image생성
	CImage* CropCentered(CImage *pInput, int width, int height);
	CImage* RotateRight(CImage* pInput);
	CImage* RotateLeft(CImage* pInput);
	CImage* RotateHalf(CImage* pInput);
	CImage* Resize(CImage* pInput, int width, int height);
	CImage* StitchHoriz(CImage* pLeft, CImage* pRight);	
	//!< vertex:0 to n-1 , edge : integer 2개, 즉 list의 길이는 2의배수여야 함. motion graph등을 debug할때 사용 가능
	void DrawGraph(CImage* pInput, int numVertex, std::list<int>& listEdge);	
	
	CPixelRGB8 GetColor(int i);

	void SafeDelete(CImage* pImage, const char* filename);

	enum {LINE_CHART, BAR_CHART};

	CImage* Plot(const vectorn& x, const vectorn& y);
	CImage* Plot(const matrixn& samples, const vectorn& min, const vectorn& max);
	CImage* DrawChart(const vectorn& vector, int chart_type, float min=0, float max=0); 	//!< 0 frame부터 n-1프레임까지의 float value를 그린다.
	CImage* DrawChart(const matrixn& matrix, int chart_type, vectorn& aMin, vectorn& aMax, double* aY=NULL);	//!< 0 frame부터 n-1프레임까지의 여러 signal들을 그린다.
	CImage* DrawChart(const matrixn& matrix, int chart_type, float min=0, float max=0, float horizLine=FLT_MAX);		//!< 0 frame부터 n-1프레임까지의 여러 signal들을 그린다.
	CImage* DrawChart(const matrixn& matrix, float min=0, float max=0);						//!< 0 frame부터 n-1프레임까지의 멀티 dimensional signal을 그린다.
	CImage* DrawChart(const bitvectorn& ab, CPixelRGB8 color);
	CImage* DrawChart(const intvectorn& ab, const char* colormapfile="resource/default/colormap.bmp");
	//CImage* DrawChartText(const intvectorn& ab, TArray<TString>* translationTable=NULL);
	CImage* DrawMatrix(matrixn& matrix);
	CImage* DrawMatrix(matrixn& matrix, double& min, double& max);
	CImage* DrawMatrix2(matrixn& matrix, double min=0, double max=0);
	CImage* DrawSpectrum(vectorn& signal, int window, int offset);	// draw chart
	
	void LoadMatrix(matrixn& matrix, const char* filename);
	void SaveMatrix(matrixn& matrix, const char* filename);
	// filename을 "filename_prefix_minvalue_maxvalue.bmp"로 한다.
	void SaveMatrixAndInfo(matrixn& matrix, const char* filename_prefix);	
	
	
	/// pInput을 2x2 matrix pTransf를 가지고 transform한 이미지를 return한다.
	/*!
		(source_x,source_y)*matrix=(target_x,target_y)
	*/
	CImage* Transform(matrixn& transf, CImage* pInput);
	CImage* Rotate(float radian, CImage* pInput);	
	
	void ChangeChartPrecision(int precision);
	void DefaultPrecision();

	// private
	namespace _private
	{
		extern int g_nChartPrecision;
		void DrawChart(CImage* pInput, int numFrame, double* aValue,float min, float max, CPixelRGB8 color, int xoffset, int yoffset, int xdelta,int chart_type);
	}
}




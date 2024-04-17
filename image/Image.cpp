//
// Image.cpp
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

// Image.cpp: implementation of the CImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Image.h"
#ifndef NO_DEVIL
#include <IL/il.h>
#include <IL/ilu.h>
#endif
#ifndef NO_FREEIMAGE
#include "FreeImage.h"
#endif
CPixelsView CPixels::range(int start, int end, int step)
{
	return _range<CPixelsView >(start,end,step);
}

const CPixelsView CPixels::range(int start, int end, int step) const
{
	return ((CPixels*)this)->range(start, end, step);
}

CPixelRGB8 CPixels::average() const
{
	long sumR=0, sumG=0, sumB=0;

	for(int i=0; i<size(); i++)
	{
		sumR+=(long)value(i).R;
		sumG+=(long)value(i).G;
		sumB+=(long)value(i).B;
	}
	sumR/=size();
	sumG/=size();
	sumB/=size();
	return CPixelRGB8(sumR, sumG, sumB);
}

;
int CImage::__uniqueID=1;

CImage::CImage()
	:
	_size(0,0),
	_dataPtr(NULL),
	_stride(0),
	_id(__uniqueID)
{
#ifndef NO_DEVIL
	if(__uniqueID==1)
	{
		ilInit();
		iluInit();
	}

	__uniqueID++;
	ILuint images[1];
	ilGenImages(1, images);
	_id=images[0];
#else
	__uniqueID++;
#endif
}

CImage::~CImage()
{
#ifndef NO_DEVIL
	if(_dataPtr)
	{
		ILuint images[1];
		images[0]=_id;
		ilDeleteImages(1, images);
	}
#else
	if(_dataPtr)
	{
		delete [] _dataPtr;
		_dataPtr=NULL;
	}
#endif
}

int CImage::GetWidth() const
{return _size.x;}
int CImage::GetHeight()const
{return _size.y;}

void CImage::CopyFrom(CImage const& other)
{


	_size.x=other.GetWidth();
	_size.y=other.GetHeight();
	#ifndef NO_DEVIL
	ilBindImage(_id);
	ilCopyImage(other._id);
	_dataPtr=ilGetData();
	_stride=GetWidth()*3;
#else
	_dataPtr=new uchar[_size.x*_size.y*3];
	_stride=GetWidth()*3;
	for(int y=0; y<_size.y; y++)
		memcpy(&_dataPtr[y*_stride], &other._dataPtr[y*other._stride], _stride);

	#endif
}

void CImage::SetData(int width, int height, uchar* dataPtr, int stride)
{
#ifndef NO_DEVIL
	ilBindImage(_id);
	ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);
	_size=Int2D(width, height);

	for(int i=0; i<GetHeight(); i++)
	{
		int ii=GetHeight()-i-1;
		ilSetPixels(0,i,0, GetWidth(), 1, 1, IL_RGB, IL_UNSIGNED_BYTE, dataPtr+stride*ii);
	}

	_dataPtr=ilGetData();
	_stride=GetWidth()*3;
#else
	_size.x=width;
	_size.y=height;
	_dataPtr=new uchar[width*height*3];
	_stride=GetWidth()*3;
	for(int y=0; y<_size.y; y++)
		memcpy(&_dataPtr[y*_stride], &dataPtr[(_size.y-y-1)*stride], _stride);
#endif
}

// assumes RGB8 format.
bool CImage::Create(int width, int height)
{
#ifndef NO_DEVIL
	_size.x=width;
	_size.y=height;
	ilBindImage(_id);
	ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);
	_dataPtr=ilGetData();
	_stride=GetWidth()*3;
#else
	_size.x=width;
	_size.y=height;
	_dataPtr=new uchar[width*height*3];
	_stride=GetWidth()*3;
	memset((void*)_dataPtr, 200, width*height*3);
#endif
	return true;
}

void CImage::GetVertLine(int i, CPixels& out)
{
	out.setSize(GetHeight());

	for(int j=0; j<GetHeight(); j++)
	{
		out[j]=*GetPixel(i,j);
	}
}

static void CImage_flipY(CImage& inout)
{
	CPixelRGB8 * line_swap=new CPixelRGB8[inout.GetWidth()];
	for(int i=0; i<inout.GetHeight()/2; i++)
	{
		CPixelRGB8* line1=inout.GetPixel(0,i);
		CPixelRGB8* line2=inout.GetPixel(0,inout.GetHeight()-i-1);
		memcpy(line_swap, line1, sizeof(CPixelRGB8)*inout.GetWidth());
		memcpy(line1, line2, sizeof(CPixelRGB8)*inout.GetWidth());
		memcpy(line2, line_swap, sizeof(CPixelRGB8)*inout.GetWidth());
	}
}
bool CImage::Load(const char* filename)
{
#ifndef NO_FREEIMAGE
	FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(filename,0);
	FIBITMAP* imagen = FreeImage_Load(formato, filename);
	FIBITMAP* temp = imagen;  
	imagen = FreeImage_ConvertTo32Bits(imagen);
	FreeImage_Unload(temp);

	_size.x = FreeImage_GetWidth(imagen);
	_size.y = FreeImage_GetHeight(imagen);
	_dataPtr=new unsigned char[32*_size.x*_size.y];
	_stride=GetWidth()*3;

	char* pixeles = (char*)FreeImage_GetBits(imagen);

	for(int j= 0; j<_size.x*_size.y; j++)
	{
		_dataPtr[j*3+0]= pixeles[j*4+2];
		_dataPtr[j*3+1]= pixeles[j*4+1];
		_dataPtr[j*3+2]= pixeles[j*4+0];
		//_dataPtr[j*4+3]= pixeles[j*4+3];
	}
	FreeImage_Unload(imagen);
#else
    #ifndef NO_DEVIL
	ilBindImage(_id);
	ilLoadImage(filename);
	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	_size.x= ilGetInteger(IL_IMAGE_WIDTH);
	_size.y= ilGetInteger(IL_IMAGE_HEIGHT);
	_dataPtr=ilGetData();
	_stride=GetWidth()*3;
	#endif
#endif
	if (TString(filename).right(3).toUpper()=="JPG" ||TString(filename).right(3).toUpper()=="PNG")
	{
		CImage_flipY(*this);
		_flipped=true;
	}
	else
		_flipped=false;


	//Save("out.jpg");
	return true;
}


namespace Imp
{
	void DeleteFile( const char* filename);
}

namespace Imp
{
	bool IsFileExist(const char* filename);
}

#ifndef NO_FREEIMAGE
//#pragma comment(lib, "FreeImage/Dist/FreeImage.lib")

/** Generic image writer
@param dib Pointer to the dib to be saved
@param lpszPathName Pointer to the full file name
@param flag Optional save flag constant
@return Returns true if successful, returns false otherwise
*/
bool GenericWriter(FIBITMAP* dib, const char* lpszPathName, int flag=0) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	BOOL bSuccess = FALSE;

	if(dib) {
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(lpszPathName);
		if(fif != FIF_UNKNOWN ) {
			// check that the plugin has sufficient writing and export capabilities ...
			WORD bpp = FreeImage_GetBPP(dib);
			if(FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp)) {
				// ok, we can save the file
				bSuccess = FreeImage_Save(fif, dib, lpszPathName, flag);
				// unless an abnormal bug, we are done !
			}
		}
	}
	return (bSuccess == TRUE) ? true : false;
}


static void CImage_SaveFreeImage(CImage & image, const char* filename, int BPP=-1)
{
//#define DEVIL_FLIP_Y
#if (defined (_MSC_VER)) && (defined (DEVIL_FLIP_Y))
	printf("here\n");
   	CImage_flipY(image); // somehow some versions of ms windwows devil shows different behavior.
#endif
//	applyFloydSteinberg(image, 4);

	BYTE* bits=image._dataPtr;
	int width=image.GetWidth();
	int height=image.GetHeight();
	int scan_width=image._stride;

	{
		BYTE* pixels=bits;
		int w=width;
		int h=height;
		unsigned char temp;
		int pos;
		//---------------------------- swap RGB to BGR
		for (int i = 0; i < w*h; i++){
			pos=i*3;
			temp = pixels[pos];
			pixels[pos  ] = pixels[pos+2];
			pixels[pos+2] = temp;
		}
	}
	// convert a 32-bit raw buffer (top-left pixel first) to a FIBITMAP
	// ----------------------------------------------------------------
	FIBITMAP *dst = FreeImage_ConvertFromRawBits(bits, width, height, scan_width,
			24, 0, 0, 0, FALSE);
	{
		BYTE* pixels=bits;
		int w=width;
		int h=height;
		unsigned char temp;
		int pos;
		//---------------------------- back to RGB
		for (int i = 0; i < w*h; i++){
			pos=i*3;
			temp = pixels[pos];
			pixels[pos  ] = pixels[pos+2];
			pixels[pos+2] = temp;
		}
	}


	
	TString ext=TString(filename).right(3).toUpper();

	FIBITMAP *dst_grey=NULL;
	FIBITMAP *dst2=NULL;
	
	if(ext=="BMP")
	{
		if (BPP==-1) BPP=4;
	}
	else if(ext=="GIF")
	{
		if (BPP==-1) BPP=8;
	}
	else if(BPP==-1) 
		BPP=24;

	if (BPP==4)
	{
		dst_grey=FreeImage_ConvertToGreyscale(dst);
		dst2 = FreeImage_ConvertTo4Bits(dst_grey);
		GenericWriter(dst2, filename);
		FreeImage_Unload(dst_grey);
		FreeImage_Unload(dst2);
	}
	else if(BPP==8)
	{
		dst_grey=FreeImage_ConvertToGreyscale(dst);		
		GenericWriter(dst_grey, filename);
		FreeImage_Unload(dst_grey);
	}
	else
		GenericWriter(dst, filename);

	//FIBITMAP *dst2 = FreeImage_Dither(dst, FID_FS);
	FreeImage_Unload(dst);
#if (defined (_MSC_VER)) && (defined (DEVIL_FLIP_Y))
	CImage_flipY(image);
#endif
}
#endif

bool CImage::Save(const char* filename) const
{
	/*
	if(Imp::IsFileExist(filename))
	{
	//	if(!Msg::confirm("Do you want to overwrite file %s?", filename))
	//		return false;
		deleteFile(filename);
	}

	TString ext=TString(filename).right(3).toUpper();
	if(ext=="BMP" || ext=="GIF")
	{
		CImage_SaveFreeImage(*this, filename);
	}
	else
	{
		if (TString(filename).right(3).toUpper()=="JPG")
		{
			printf("flipping..\n");

		//if (_flipped)
			CImage_flipY(*this);
		}
		ilBindImage(_id);
		ilSaveImage(filename);
		if (TString(filename).right(3).toUpper()=="JPG")
		//if (_flipped)
			CImage_flipY(*this);
	}
	*/
	save(filename, 24);
	return 1;
}

bool CImage::save(const char* filename, int BPP) const
{
	if(Imp::IsFileExist(filename))
	{
//		if(!Msg::confirm("Do you want to overwrite file %s?", filename))
//			return false;
		deleteFile(filename);
	}

#ifndef NO_FREEIMAGE
	CImage_SaveFreeImage((CImage&)(*this), filename, BPP);
#endif
	return true;
}

/**
* @author Ralph Hauwert
* 
* The ImageDithering class allows for simple image color quantization and dithering.
* It aims to offer some well-known spatial dithering algorithms for the AS3 Language.
* 
* ralph@unitzeroone.com
* http://www.unitzeroone.com
*/

/**
* applyFloydSteinberg(bitmapData:BitmapData, levels:int);
* 
* Floyd-Steinberg dithering is an image dithering algorithm first published in 1976 by Robert W. Floyd and Louis Steinberg.
* Using error diffusion, this form of dithering is visually much better then ordered diffusion like Bayer's.
* R.W. Floyd, L. Steinberg, "An adaptive algorithm for spatial grey scale". Proceedings of the Society of Information Display 17, 75??7 (1976).
*/
#include "ImagePixel.h"
void gammaCorrection(CImage& _bitmapData, double fGamma)
{
	unsigned char gammaTable[256];

	int lr;
	for(int i=0; i<256; i++)
	{
		int lr=int( pow( ((double)i)/255.0, fGamma)*255.0+0.5);
		lr=	lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;
		gammaTable[i]= (unsigned char)lr;
	}
	CImagePixel bitmapData(&_bitmapData);

	for(int y=0; y<bitmapData.Height();y++)
	{
		CPixelRGB8* line=bitmapData[y];

		for(int x=0; x<bitmapData.Width();x++)
		{
			line[x].R=gammaTable[line[x].R];
			line[x].G=gammaTable[line[x].G];
			line[x].B=gammaTable[line[x].B];
		}		
	}
}

void applyFloydSteinberg(CImage& _bitmapData, int _levels)
{
	double levels=(double)_levels;
	int lNorm= 256/_levels;
	int hlNorm=lNorm/2;
	//The FS kernel...note the 16th. Optimisation can still be done.
	double d1= 7/16.0;
	double d2= 3/16.0;
	double d3= 5/16.0;
	double d4= 1/16.0;

	int lc, r, g, b, nr, ng, nb, er, eg, eb, lr, lg, lb;
	int x=0;
	int y=0;

	int lNorm2=256/(_levels+1);
	int hlNorm2=lNorm2/2;
	CImagePixel bitmapData(&_bitmapData);

//#define USE_DEC_DYNRANGE
#ifdef USE_DEC_DYNRANGE
#define DEC_DYN(x)	(x)*lNorm2/lNorm+hlNorm2;
	for(y=0; y<bitmapData.Height();y++)
	{
		CPixelRGB8* line=bitmapData[y];

		for(x=0; x<bitmapData.Width();x++)
		{
			line[x].R=DEC_DYN(int(line[x].R))
			line[x].G=DEC_DYN(int(line[x].G))
			line[x].B=DEC_DYN(int(line[x].B))
		}		
	}
	
#endif

#define QUANTIZE2(x) (x)/lNorm*lNorm+hlNorm;

#ifndef COLOR_QUANTIZE
	for(y=0; y<bitmapData.Height();y++)
	{
		CPixelRGB8* line=bitmapData[y];
		for(x=0; x<bitmapData.Width();x++)
		{
			//Retrieve current RGB value.
			CPixelRGB8& c = line[x];
			r = c.R;
			g = c.G;
			b = c.B;
			lr =(((int)r + int(g)*2+ int(b))/4);
			lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;
			c.R=lr;
			c.G=lr;
			c.B=lr;
		}
	}
#endif
	for(y=0; y<bitmapData.Height()-1;y++)
	{
		CPixelRGB8* line=bitmapData[y];
		CPixelRGB8* nline=bitmapData[y+1];

		for(x=1; x<bitmapData.Width()-1;x++)
		{
			//Retrieve current RGB value.
			CPixelRGB8& c = line[x];
			r = c.R;
			g = c.G;
			b = c.B;

#ifdef COLOR_QUANTIZE
			//Normalize and scale to the number of levels.
			nr = QUANTIZE2(r);
			ng = QUANTIZE2(g);
			nb = QUANTIZE2(b);

			//Set the current pixel.
			c.R=nr;
			c.G=ng;
			c.B=nb;

			//Quantization error.
			er = (r)-nr;
			eg = (g)-ng;
			eb = (b)-nb;

			//Apply the kernel.
			//+1,0
			{
				CPixelRGB8& lc = line[x+1];
				lr = (int)lc.R + (d1*er);
				lg = (int)lc.G + (d1*eg);
				lb = (int)lc.B + (d1*eb);

				//Clip & Set
				lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;
				lg < 0 ? lg = 0: lg > 255 ? lg = 255 : lg;
				lb < 0 ? lb = 0: lb > 255 ? lb = 255 : lb;

				lc.R=lr;
				lc.G=lg;
				lc.B=lb;
			}

			//-1,+1
			{
				CPixelRGB8& lc = nline[x-1];
				lr = (int)lc.R + (d2*er);
				lg = (int)lc.G + (d2*eg);
				lb = (int)lc.B + (d2*eb);

				//Clip & Set
				lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;
				lg < 0 ? lg = 0: lg > 255 ? lg = 255 : lg;
				lb < 0 ? lb = 0: lb > 255 ? lb = 255 : lb;
				
				lc.R=lr;
				lc.G=lg;
				lc.B=lb;
			}

			//0,+1
			{
				CPixelRGB8& lc = nline[x];
				lr = (int)lc.R + (d3*er);
				lg = (int)lc.G + (d3*eg);
				lb = (int)lc.B + (d3*eb);

				//Clip & Set
				lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;
				lg < 0 ? lg = 0: lg > 255 ? lg = 255 : lg;
				lb < 0 ? lb = 0: lb > 255 ? lb = 255 : lb;

				lc.R=lr;
				lc.G=lg;
				lc.B=lb;
			}

			//+1,+1
			{
				CPixelRGB8& lc = nline[x+1];
				lr = (int)lc.R + (d4*er);
				lg = (int)lc.G + (d4*eg);
				lb = (int)lc.B + (d4*eb);

				//Clip & Set
				lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;
				lg < 0 ? lg = 0: lg > 255 ? lg = 255 : lg;
				lb < 0 ? lb = 0: lb > 255 ? lb = 255 : lb;

				lc.R=lr;
				lc.G=lg;
				lc.B=lb;
			}
#else
			//Normalize and scale to the number of levels.
			nr = QUANTIZE2(r);

			//Set the current pixel.
			c.R=nr;
			c.G=nr;
			c.B=nr;

			//Quantization error.
			er = (r)-nr;

			//Apply the kernel.
			//+1,0
			{
				CPixelRGB8& lc = line[x+1];
				lr = (int)lc.R + (d1*er);

				//Clip & Set
				lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;

				lc.R=lr;
				lc.G=lr;
				lc.B=lr;
			}

			//-1,+1
			{
				CPixelRGB8& lc = nline[x-1];
				lr = (int)lc.R + (d2*er);

				//Clip & Set
				lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;
				
				lc.R=lr;
				lc.G=lr;
				lc.B=lr;
			}

			//0,+1
			{
				CPixelRGB8& lc = nline[x];
				lr = (int)lc.R + (d3*er);

				//Clip & Set
				lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;

				lc.R=lr;
				lc.G=lr;
				lc.B=lr;
			}

			//+1,+1
			{
				CPixelRGB8& lc = nline[x+1];
				lr = (int)lc.R + (d4*er);

				//Clip & Set
				lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;

				lc.R=lr;
				lc.G=lr;
				lc.B=lr;
			}
#endif
		}
	}

	for(y=0; y<bitmapData.Height();y++)
	{
		CPixelRGB8* line=bitmapData[y];

		for(x=0; x<bitmapData.Width();x++)
		{
			CPixelRGB8& c = line[x];
			r = c.R;
			g = c.G;
			b = c.B;

			//Normalize and scale to the number of levels
			// : quantization
			nr = QUANTIZE2(r);
			ng = QUANTIZE2(g);
			nb = QUANTIZE2(b);

			c.R=nr;
			c.G=ng;
			c.B=nb;
		}
	}
}

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

vector3 rgb2hsv(vector3 const& _in)
{
	rgb in;
	in.r=_in.x;
	in.g=_in.y;
	in.b=_in.z;
	hsv out=rgb2hsv(in);
	return vector3(out.h, out.s, out.v);
}
vector3 hsv2rgb(vector3 const& _in)
{
	hsv in;
	in.h=_in.x;
	in.s=_in.y;
	in.v=_in.z;
	rgb out=hsv2rgb(in);
	return vector3(out.r, out.g, out.b);
}

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}

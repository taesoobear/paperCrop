//
// PDFWriter.cpp 
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
#include "PDFWriter.h"
#include "image/Image.h"

//#pragma comment(lib, "libharu-2.0.8/libhpdf.lib")

jmp_buf env;

#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler  (HPDF_STATUS   error_no,
                HPDF_STATUS   detail_no,
                void         *user_data)
{
    printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
                (HPDF_UINT)detail_no);
    longjmp(env, 1);
}



void PDFWriter::save(const char* fname)
{
    HPDF_SaveToFile (pdf, fname);
	HPDF_Free(pdf);
	pdf=NULL;
}

bool PDFWriter::init()
{
	if(pdf)
		HPDF_Free(pdf);

    pdf = HPDF_New (error_handler, NULL);
    if (!pdf) {
        printf ("error: cannot create PdfDoc object\n");
        return false;
    }

	/* error-handler */
    if (setjmp(env)) {
        HPDF_Free (pdf);
		pdf=NULL;
        return false;
    }

	HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

    /* create default-font */
    font = HPDF_GetFont (pdf, "Helvetica", NULL);

	return true;
}

void PDFWriter::addPage(CImage const& pageImage, int bpp)
{
	/* add a new page object. */
    HPDF_Page page = HPDF_AddPage (pdf);

	int w=pageImage.GetWidth();
	int h=pageImage.GetHeight();

    HPDF_Page_SetWidth (page, w);
    HPDF_Page_SetHeight (page, h);

	im.resize(w*h);

	int k=0;
	for(int i=0; i<h; i++)
	{
		//CPixelRGB8* line=pageImage.GetPixel(0,h-i-1);
		CPixelRGB8* line=pageImage.GetPixel(0,i);
		for(int j=0; j<w; j++)
		{
			CPixelRGB8& a=line[j];
			im[k++]=(unsigned char)(((int)a.R+(int)a.G+(int)a.B)/3);
		}
	}

	ASSERT(k==w*h);

	HPDF_BYTE* ptr=&im[0];
	if (bpp==4 || bpp==2 || bpp==1)
	{
		im4.resize(w*h/2+h+1);
		int c=0;
		int k=0;
		int koffset=0;
		for(int i=0; i<h; i++)
		  {
			for(int j=0; j<w; j++)
			{
				if (k%2==0)
				{
					im4[c]=((im[k+koffset]>>4)&0xFF)<<4;
				}
				else
				{
					im4[c]=(im[k+koffset]>>4)&0xFF | im4[c];
					c++;
				}
				k++;
			}
			if (w%2==1) {c++;k++;koffset--;}
		  }
		ptr=&im4[0];
		bpp=4;
	}
	else
		assert(bpp==8);

    /* load GrayScale raw-image (1bit) file from memory. */
    HPDF_Image image = HPDF_LoadRawImageFromMem (pdf, ptr, w, h,
//                HPDF_CS_DEVICE_GRAY, 8);
//                HPDF_CS_DEVICE_RGB, 2);
                HPDF_CS_DEVICE_GRAY, bpp);

    
    /* Draw image to the canvas. (normal-mode with actual size.)*/
    HPDF_Page_DrawImage (page, image, 0, 0, w, h);

}

void PDFWriter::addPageColor(CImage const& pageImage)
{
	/* add a new page object. */
    HPDF_Page page = HPDF_AddPage (pdf);

	int w=pageImage.GetWidth();
	int h=pageImage.GetHeight();

    HPDF_Page_SetWidth (page, w);
    HPDF_Page_SetHeight (page, h);

	im.resize(w*h*3);

	int k=0;
	for(int i=0; i<h; i++)
	{
		//CPixelRGB8* line=pageImage.GetPixel(0,h-i-1);
		CPixelRGB8* line=pageImage.GetPixel(0,i);
		for(int j=0; j<w; j++)
		{
			CPixelRGB8& a=line[j];
			im[k++]=a.R;
			im[k++]=a.G;
			im[k++]=a.B;
		}
	}

	ASSERT(k==w*h);

    /* load GrayScale raw-image (1bit) file from memory. */
    HPDF_Image image = HPDF_LoadRawImageFromMem (pdf, &im[0], w, h,
                HPDF_CS_DEVICE_RGB, 8);

    
    /* Draw image to the canvas. (normal-mode with actual size.)*/
    HPDF_Page_DrawImage (page, image, 0, 0, w, h);

}

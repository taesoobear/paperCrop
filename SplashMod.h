#ifndef SPLASH_OUTPUT_DEV_MOD
#define SPLASH_OUTPUT_DEV_MOD

#include <splash/SplashFont.h>
#include <splash/SplashGlyphBitmap.h>
#include <splash/Splash.h>
#include <splash/SplashMath.h>

void notifyDraw(int c, int x, int y, int w, int h, int fx, int fy);
class SplashOutputDev_mod: public SplashOutputDev {
 public:
	virtual ~SplashOutputDev_mod(){}
  // Constructor.
  SplashOutputDev_mod(SplashColorMode colorModeA, int bitmapRowPadA,
		  bool reverseVideoA, SplashColorPtr paperColorA,
		  bool bitmapTopDownA = true)
	  :SplashOutputDev(colorModeA, bitmapRowPadA, reverseVideoA, paperColorA,
					   bitmapTopDownA){}

// Transform a point from user space to device space.
inline void transform(SplashCoord *matrix,
			      SplashCoord xi, SplashCoord yi,
			      SplashCoord *xo, SplashCoord *yo) {
  //                          [ m[0] m[1] 0 ]
  // [xo yo 1] = [xi yi 1] *  [ m[2] m[3] 0 ]
  //                          [ m[4] m[5] 1 ]
  *xo = xi * matrix[0] + yi * matrix[2] + matrix[4];
  *yo = xi * matrix[1] + yi * matrix[3] + matrix[5];
}
 void fillChar(SplashCoord*matrix, SplashCoord x, SplashCoord y,
			     int c, SplashFont *font) {
  SplashGlyphBitmap glyph;
  SplashCoord xt, yt;
  int x0, y0, xFrac, yFrac;
  SplashClipResult clipRes;
  transform(matrix, x, y, &xt, &yt);
  x0 = splashFloor(xt);
  xFrac = splashFloor((xt - x0) * splashFontFraction);
  y0 = splashFloor(yt);
  yFrac = splashFloor((yt - y0) * splashFontFraction);
  if (font->getGlyph(c, xFrac, yFrac, &glyph, x0, y0, getSplash()->getClip(), &clipRes)) {
	  notifyDraw(c, x0, y0, glyph.w, glyph.h, glyph.x, glyph.y);
  }
}
void drawChar(GfxState *state, double x, double y,
			       double dx, double dy,
			       double originX, double originY,
			       CharCode code, int nBytes,
			       Unicode *u, int uLen) {
	SplashOutputDev::drawChar(state,x,y,dx,dy,originX,originY,code, nBytes,u,uLen);
	SplashFont* font=getCurrentFont();
	if(state->getRender()!=3 && font && !state->getFillColorSpace()->isNonMarking())
		{
			
			fillChar(getSplash()->getMatrix(), (SplashCoord)(x-originX), (SplashCoord)(y-originY), code, font);
		}
}
};


#endif

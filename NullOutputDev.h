
#ifndef NULLOUTPUTDEV_H
#define NULLOUTPUTDEV_H

void notifyDraw(int c, int x, int y, int w, int h, int fx, int fy);
//------------------------------------------------------------------------
// OutputDev
//------------------------------------------------------------------------

class NullOutputDev :public OutputDev {
public:

  // Constructor.
 NullOutputDev():OutputDev() { }

  // Destructor.
  virtual ~NullOutputDev() {}

  //----- get info about output device

  // Does this device use upside-down coordinates?
  // (Upside-down means (0,0) is the top left corner of the page.)
  virtual bool upsideDown() { return true; }

  // Does this device use drawChar() or drawString()?
  virtual bool useDrawChar() { return true; }

  // Does this device use beginType3Char/endType3Char?  Otherwise,
  // text in Type 3 fonts will be drawn with drawChar/drawString.
  virtual bool interpretType3Chars() { return false; }

  // Does this device need non-text content?
  virtual bool needNonText() { return false; }

  double DPI_W, DPI_H;
  int width, height;
  int rotated;
  double x1,y1,x2,y2;
  void setInfo(int rot, double x1, double y1,double a, double b, int w, int h)
  {
	  this->x1=x1;
	  this->y1=y1;
	  rotated=rot;
	  DPI_W=a; DPI_H=b;
	  width=w; height=h;
  }
  virtual void drawChar(GfxState * state, double x, double y,
			double dx, double dy,
			double originX, double originY,
			CharCode code, int nBytes, Unicode * u, int uLen) 
  {
	  // check for invisible text -- this is used by Acrobat Capture
	
	  GfxFont *gfxFont;
	  GfxFontType fontType;
#ifndef OLD_LINUX
	  if (!(gfxFont = state->getFont().get())) {
#else
	  if (!(gfxFont = state->getFont())) {
#endif
		  return;
	  }
	  fontType = gfxFont->getType();
	  if (fontType == fontType3) {
		  return;
	  }
	  x-=originX;
	  y-=originY;
	
	  if (!state->getFillColorSpace()->isNonMarking()) {
		  if (rotated==0) {
			  /* double x2,y2; */
			  /* state->transform(x,y, &x2,&y2); */
			  /* x=x2;y=y2; */

			  int a,b,c,d;
			  a=( x-x1)/DPI_W*width;
			  b=height-(y-y1)/DPI_H*height;
			  c=dx/DPI_W*width;
			  d=dy/DPI_H*height;
			  
			  int td=int(c+0.5);
			  notifyDraw(code,int(a+0.5),int(b+0.5)-td,std::max(int(c+0.5),1),std::max(td,1),0,0); 
		  }
		  else if (rotated==90 || rotated==180 ){
			  
		  }
		  else {
			  /* printf("rotated %d\n",rotated); */
		  }
	  }

  }
};

#endif

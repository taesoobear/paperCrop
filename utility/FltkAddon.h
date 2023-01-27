#ifndef _FLTKADDON_H_
#define _FLTKADDON_H_
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


class FlMenu;
class Fl_Scroll;

/// FlChooseFile("choose", "../resource/classification", "*.asf");
TString FlChooseFile(const char* message, const char* path, const char* Mask, bool bCreate=false);


/// Fltk EventReciver may inherit this class. 
class Fl_Callee
{
public:
	Fl_Callee(){};
	virtual~Fl_Callee(){};

	/** Usage: connect(new Fl_Button(0,0,80,20,"SomeButton"), 0);

	FlCallee클래스와 달리 어떤 가정도 사용하지 않았다. 그대신 유저 데이타를 사용할 수 없다.
	편의상 콜백펑션을 미리 정의된 5개중에 한개를 사용할수 있다.
	(callBackID 는 {0,1,2,3,4} 중에 하나.)
	*/	

	void connect(Fl_Widget* pWidget, int callBackID=0);
	
protected:
	/// you can distinguish the caller based on pWidget, pWidget->label(), or callBackID.
	virtual void onCallback(Fl_Widget * pWidget, int callBackID)=0;

private:
	static void cbFunc0(Fl_Widget * pWidget, void *data);
	static void cbFunc1(Fl_Widget * pWidget, void *data);
	static void cbFunc2(Fl_Widget * pWidget, void *data);
	static void cbFunc3(Fl_Widget * pWidget, void *data);
	static void cbFunc4(Fl_Widget * pWidget, void *data);
};

/// Dynamically allocated menu. (Each instance can be connected to different FlCallee.
class FlMenu
{
public:
	FlMenu();
	~FlMenu();

	void init(Fl_Menu_* pMenu, int n);
	void init(Fl_Menu_* pMenu);
	void initChoice(int x, int y, int w, int h, const char* title=NULL);

	void size(int n);
	int size();

	void item(int item, const char* text, int shortcut_=0,  int user_data_=0,  int flags=0,  uchar labeltype_=0,  uchar labelfont_=0, uchar labelsize_=0,  unsigned labelcolor_=0);
	void beginSubMenu(int item, const char* text)	{	this->item(item, text, 0, 0, FL_SUBMENU);}
	void endSubMenu(int item)						{	this->item(item, 0);}
	
	bool isValidItem(int item)						{ return (item>=0 && item<size() && m_aMenuItem[item].flags!=FL_SUBMENU && m_aMenuItem[item].text!=NULL);}

	Fl_Menu_& menu()						{return *m_pMenu;}
	TString const& label(int iItem) const	{return m_aLabel[iItem];}
	TString const& label()	const			{return label(value());}
	bool operator==(Fl_Widget* pWidget) const	{ return (Fl_Widget* )m_pMenu==pWidget;}

	// get user data of the current choice.
	void* userData()		{ return (m_aMenuItem[m_pMenu->value()].user_data()); }
	void* userData(int n)	{ return (m_aMenuItem[n].user_data()); }
	void value(int n)	{ m_pMenu->value(n);}
	int value()	const	{ return m_pMenu->value(); }
	void redraw()		{ m_pMenu->redraw(); }
private:
	friend class FlCallee;
	friend class FlLayout;
    int m_nMenu;
	TStrings m_aLabel;
	Fl_Menu_* m_pMenu;
	Fl_Menu_Item* m_aMenuItem;
};

class FlChoice : public Fl_Choice
{		
public:	
	FlMenu menu;

	/*******************************************
	* Static allocation
	*	static Fl_Menu_Item menu[] = {
	*	// text, shortcut, callback_, user_data_, flags...
	*	{"left+top", 0, cbFunc, data1},
	*	{"left+bottom", 0, cbFunc, data2},
	*	{"right+top", 0, cbFunc, data3},
	*	{"right+bottom", 0, cbFunc, data4},
	*	{0}
	*	};
	*   (new Fl_Choice(0,0,100,20))->copy(menu); -> complicated code.
	*
	* Dynamic Allocation
	*	FlChoice* menu=new FlChoice(0,0,100,20);
	*   
	*   menu->size(4);l
	*	menu->item(0, "left+top", 0, data1);
	*	menu->item(1, "left+bottom", 0, data2);
	*	menu->item(2, "right+top", 0, data3);
	*	menu->item(3, "right+bottom", 0, data4);
	*   
	*
	* ?ʿ??Ҷ????? update?Ϸ��?, size(4); item(0..3); redraw(); ?̰?���� ???ָ? ?ȴ?.
	*/

	FlChoice(int x, int y, int w, int h, const char* l=0)
		:Fl_Choice(x,y,w,h,l)
	{
		menu.init(this);
	}

	void size(int n)	{ menu.size(n);}
	int size()			{ return menu.size();}

	void item(int item, const char* text, int shortcut_=0,  int user_data_=0,  int flags=0,  uchar labeltype_=0,  uchar labelfont_=0, uchar labelsize_=0,  unsigned labelcolor_=0)
	{
		menu.item(item, text, shortcut_, user_data_, flags, labeltype_, labelfont_, labelsize_, labelcolor_);
	}

	Fl_Menu_Item& item(int item);

	void beginSubMenu(int item, const char* text)
	{
		menu.item(item, text, 0, 0, FL_SUBMENU);
	}

	void endSubMenu(int item)
	{
		menu.item(item, 0);
	}

};


class Fl_Fast_Value_Slider : public Fl_Value_Slider
{
public:
	Fl_Fast_Value_Slider (int X, int Y, int W, int H, const char* l=0):Fl_Value_Slider(X,Y,W,H,l){}
	virtual ~Fl_Fast_Value_Slider (){}

    virtual int handle(int);
};

class Fl_Hor_Slider2 : public Fl_Slider 
{
public:
	Fl_Hor_Slider2(int x,int y,int w,int h, const char *l = 0);
};

class CImage;
class TRect;
void fl_draw_CImage(const CImage& imagee, const TRect& sourceRect, int x, int y);
int FlGenShortcut(const char* s);
#endif

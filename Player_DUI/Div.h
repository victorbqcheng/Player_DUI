/***************************************************************************
Author: victor cheng
**************************************************************************/
#pragma once

#include <Windows.h>
#include <WinGDI.h>

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <gdiplus.h>

#include <functional>

#include "Types.h"

class CDiv;
class CUIMgr;

class CMouseEvent
{
public:
	CDiv* pTarget;
	int nMouseX;
	int nMouseY;
};

typedef void(*EVENT_CALLBACK)(CMouseEvent e);
typedef EVENT_CALLBACK MOUSEDOWN;
typedef EVENT_CALLBACK MOUSEUP;
typedef EVENT_CALLBACK CLICK;
typedef EVENT_CALLBACK MOUSEMOVE;
typedef EVENT_CALLBACK MOUSELEAVE;


class CDiv
{
	friend class CUIMgr;
public:
	//遍历方向：DIR_DOWN-遍历子元素; DIR_UP-遍历父元素
	enum DIRECTION
	{
		DIR_DOWN,
		DIR_UP,
	};
	typedef std::function<void(CMouseEvent e)> EVENT_CALLBACK_WRAPPER;
	typedef EVENT_CALLBACK_WRAPPER MOUSEDOWN_CALLBACK_WRAPPER;
	typedef EVENT_CALLBACK_WRAPPER MOUSEUP_CALLBACK_WRAPPER;
	typedef EVENT_CALLBACK_WRAPPER CLICK_CALLBACK_WRAPPER;
	typedef EVENT_CALLBACK_WRAPPER MOUSEMOVE_CALLBACK_WRAPPER;
	typedef EVENT_CALLBACK_WRAPPER MOUSELEAVE_CALLBACK_WRAPPER;

public:
	CDiv(std::string strID);
public:
	~CDiv(void);
	std::string const& getID();
	CUIMgr* getUIMgr();

	void setPosition(int nLeft, int nTop);
	POINT getPosition();
	POINT getAbsPosition();

	void setWidth(int nWidth);
	int getWidth();
	
	void setHeight(int nHeight);
	int getHeight();
	
	void setVisible(bool bVisible);
	bool isVisible();
	
	void setZIndex(int nZIndex);
	int getZIndex();
	
	void setClip(bool bClip);
	bool isClip();

	void setTransparent(bool bTrans);
	bool isTransparent();
	
	void addChild(CDiv* pDivChild);
	CDiv* getChildByID(std::string const& strID);
	void setParent(CDiv* pDivParent);

	HRGN getRgn();

	void setShowFrame(bool bShowFrame);

	void setDraggable(bool b);
	bool isDraggable();
	
	void setText(std::string const& strText);
	void setTextFormat(UINT format);
	void setTextColor(COLORREF color);
	//
	void setFontName(std::string strFontName);
	void setFontSize(int nFontSize);

	void setBorderColor(COLORREF color);
	void setBorderWidth(int nWidth);
	//
	void setBackgroundColor(COLORREF color);

	void setBackgroundImage(std::wstring fileName, Gdiplus::Rect srcRc);
	void setBackgroundImage(char* data, int width, int height);	//data:RGB数据.播放器使用
	
	//message callback
	void setMouseDownCb(MOUSEDOWN md);
	void setMouseDownCb(MOUSEDOWN_CALLBACK_WRAPPER cb);
	void setMouseUpCb(MOUSEUP mu);
	void setMouseUpCb(MOUSEUP_CALLBACK_WRAPPER cb);
	void setClickCb(CLICK click);
	void setClickCb(CLICK_CALLBACK_WRAPPER cb);
	void setMouseMoveCb(MOUSEMOVE mm);
	void setMouseMoveCb(MOUSEMOVE_CALLBACK_WRAPPER cb);
	void setMouseLeaveCb(MOUSELEAVE ml);
	void setMouseLeaveCb(MOUSELEAVE_CALLBACK_WRAPPER cb);
private:
	//message handler
	virtual void onPaint(HDC hDC);
	//return value:true--msg has been handled, do not need to 
	virtual bool onLButtonDown(int x, int y);
	virtual bool onLButtonUp(int x, int y);
	virtual bool onMouseMove(int x, int y);
	virtual bool onMouseLeave();
protected:
	//由父元素调用该函数，来更新子元素的状态
	//void syncParentVisible(bool bParentVisible);
	void updateRgn(DIRECTION dir=DIR_DOWN);
	void combineChildrenRgn();
	void setUIMgr(CUIMgr* pUIMgr);
	HFONT myCreateFont(std::string strFontName, int nFontSize);
protected:
	std::string m_strID;
	CUIMgr* m_pUIMgr;
	//位置
	int m_nLeftRelative;
	int m_nTopRelative;
	//大小
	int m_nWidth;
	int m_nHeight;
	//可见
	bool m_bVisible = true;

	HRGN m_rgnClip = NULL;		//如果clip则m_rgn就是自己的矩形范围，如果不clip,则m_rgn是自己的矩形与子元素的rgn的并集
	HRGN m_rgn = NULL;
	bool m_bClip;
	//背景
	HBRUSH m_hBackgroundBrush;
	COLORREF m_backgroundColor;
	ImgSprite m_bkImgSprite;
	char* m_bkImgData = NULL;
	int m_nBkImgDataWidth = 0;
	int m_nBkImgDataHeight = 0;
	//边框
	COLORREF m_borderColor;
	int m_nBorderWidth;
	HPEN m_hBorderPen;
	int m_nZIndex;
	//文字
	UINT m_textFormat;
	COLORREF m_textColor;
	HFONT m_hTextFont = NULL;
	std::string m_strFontName;
	int m_nFontSize;
	std::string m_strText;

	bool m_bDraggable = false;
	//
	bool m_bTransparent = false;	//true:鼠标消息可以穿透
	//
	bool m_bFocus;
	//
	bool m_bMouseMove;
	//子元素
	std::map<int, std::deque<CDiv*> > m_children;	//z-index -- children
	//
	bool m_bShowFrame = true;		//是否显示区域边框
	HBRUSH m_hBrForRgn = NULL;
	HRGN m_hRgnClipOld = NULL;

	CDiv* m_pDivParent;
	//callback functions
	MOUSEDOWN m_mousedown = NULL;
	EVENT_CALLBACK_WRAPPER m_mdcbWrapper;
	MOUSEUP m_mouseup = NULL;
	EVENT_CALLBACK_WRAPPER m_mucbWrapper;
	CLICK m_click = NULL;
	CLICK_CALLBACK_WRAPPER m_clickWrapper;
	MOUSEMOVE m_mm = NULL;
	MOUSEMOVE_CALLBACK_WRAPPER m_mmcbWrapper;
	MOUSELEAVE m_ml = NULL;
	MOUSELEAVE_CALLBACK_WRAPPER m_mlcbWrapper;
};

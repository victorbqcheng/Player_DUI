/***************************************************************************
Author: victor cheng
**************************************************************************/
#include "pch.h"
#include "Div.h"
#include "UIMgr.h"
#include "util.h"

CDiv::CDiv(std::string strID)
	:m_backgroundBrush(Gdiplus::Color::Transparent),
	m_pen(Gdiplus::Color(0, 0, 0, 0), float(0.0)),
	m_textBrush(Gdiplus::Color::Black)
{
	m_strID = strID;
	m_nLeftRelative = 0;
	m_nTopRelative = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nZIndex = 0;
	m_bFocus = false;
	m_bClip = false;
	m_bMouseMove = false;
	m_pDivParent = NULL;
	m_bVisible = true;
	//
	m_strFormat.SetAlignment(Gdiplus::StringAlignmentNear);
	m_strFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
	m_strFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
	m_strFontName = "微软雅黑";
	m_nFontSize = 12;
	//
	m_mousedown = NULL;
	m_mouseup = NULL;
	m_click = NULL;
	m_mm = NULL;
	m_ml = NULL;
	//
	
}

CDiv::~CDiv(void)
{
}

std::string const& CDiv::getID()
{
	return m_strID;
}

CUIMgr* CDiv::getUIMgr()
{
	return m_pUIMgr;
}
void CDiv::setPosition(int nLeft, int nTop)
{
	m_nLeftRelative = nLeft;
	m_nTopRelative = nTop;
}
POINT CDiv::getPosition()
{
	POINT pt = { m_nLeftRelative, m_nTopRelative };
	return pt;
}
void CDiv::setWidth(int nWidth)
{
	m_nWidth = nWidth;
}
int CDiv::getWidth()
{
	return m_nWidth;
}
void CDiv::setHeight(int nHeight)
{
	m_nHeight = nHeight;
}

int CDiv::getHeight()
{
	return m_nHeight;
}

POINT CDiv::getAbsPosition()
{
	POINT pt = { 0 };
	if (m_pDivParent != NULL)
	{
		POINT ptParent = m_pDivParent->getAbsPosition();
		pt.x = ptParent.x + m_nLeftRelative;
		pt.y = ptParent.y + m_nTopRelative;
	}
	else
	{
		pt.x = m_nLeftRelative;
		pt.y = m_nTopRelative;
	}
	return pt;
}
//
void CDiv::setVisible(bool bVisible)
{
	if (bVisible == false && m_strID == "speed_menu")
	{
		int i = 0;
	}
	m_bVisible = bVisible;
}

bool CDiv::isVisible()
{
	//只有当父元素和自己都是可见状态时，才可见
	return m_bVisible;
}

void CDiv::setZIndex(int nZIndex)
{
	m_nZIndex = nZIndex;
}

int CDiv::getZIndex()
{
	return m_nZIndex;
}

void CDiv::setClip(bool bClip)
{
	m_bClip = bClip;
}

bool CDiv::isClip()
{
	return m_bClip;
}

void CDiv::setTransparent(bool bTrans)
{
	m_bTransparent = bTrans;
}

bool CDiv::isTransparent()
{
	return m_bTransparent;
}

void CDiv::setBackgroundColor(Gdiplus::Color color)
{
	m_backgroundColor2 = color;
	m_backgroundBrush.SetColor(color);
}

//
void CDiv::setBackgroundImage(std::wstring fileName, Gdiplus::Rect srcRc)
{
	m_bkImgSprite.img = Gdiplus::Image::FromFile(fileName.c_str());
	m_bkImgSprite.srcRc = srcRc;
}

void CDiv::setBackgroundImage(char* data, int width, int height)
{
	m_bkImgData = data;
	m_nBkImgDataWidth = width;
	m_nBkImgDataHeight = height;
}

void CDiv::setShowFrame(bool bShowFrame)
{
	m_bShowFrame = bShowFrame;
}

void CDiv::setDraggable(bool b)
{
	m_bDraggable = b;
}

bool CDiv::isDraggable()
{
	return m_bDraggable;
}
//
void CDiv::setFontName(std::string strFontName)
{
	m_strFontName = strFontName;
}
void CDiv::setFontSize(int nFontSize)
{
	m_nFontSize = nFontSize;
}

void CDiv::setAlignment(ALIGNMENT h_align, ALIGNMENT v_align)
{
	m_strFormat.SetAlignment(Gdiplus::StringAlignment(h_align));
	m_strFormat.SetLineAlignment(Gdiplus::StringAlignment(v_align));
}

void CDiv::setTextColor(Gdiplus::Color color)
{
	m_textBrush.SetColor(color);
}

//
void CDiv::addChild(CDiv* pDivChild)
{
	int nZIndex = pDivChild->getZIndex();
	if (m_children.find(nZIndex) != m_children.end())
	{
		m_children[nZIndex].push_back(pDivChild);
	}
	else
	{
		std::deque<CDiv*> dq;
		dq.push_front(pDivChild);
		m_children[nZIndex] = dq;
	}
	pDivChild->setParent(this);
	pDivChild->setUIMgr(m_pUIMgr);
}
CDiv* CDiv::getChildByID(std::string const& strID)
{
	CDiv* pChild = NULL;
	for (auto it = m_children.begin(); it != m_children.end(); it++)
	{
		std::deque<CDiv*>& dq = it->second;
		for (size_t j = 0; j < dq.size(); j++)
		{
			CDiv* pDiv = dq[j];
			if (pDiv->getID() == strID)
			{
				pChild = pDiv;
				break;
			}
			else
			{
				pChild = pDiv->getChildByID(strID);
				if (pChild != NULL)
				{
					break;
				}
			}
		}
	}
	return pChild;
}
//
void CDiv::setParent(CDiv* pDivParent)
{
	m_pDivParent = pDivParent;
}
//
void CDiv::setMouseDownCb(MOUSEDOWN md)
{
	m_mousedown = md;
}

void CDiv::setMouseDownCb(MOUSEDOWN_CALLBACK_WRAPPER cb)
{
	this->m_mdcbWrapper = cb;
}

void CDiv::setMouseUpCb(MOUSEUP mu)
{
	m_mouseup = mu;
}

void CDiv::setMouseUpCb(MOUSEUP_CALLBACK_WRAPPER cb)
{
	this->m_mucbWrapper = cb;
}

//
void CDiv::setClickCb(CLICK click)
{
	m_click = click;
}
void CDiv::setClickCb(CLICK_CALLBACK_WRAPPER cb)
{
	m_clickWrapper = cb;
}

void CDiv::setLButtonDbClickCb(LBUTTON_DB_CLICK_CALLBACK_WRAPPER cb)
{
	m_lbuttonDbClickWrapper = cb;
}

//
void CDiv::setMouseMoveCb(MOUSEMOVE mm)
{
	m_mm = mm;
}
void CDiv::setMouseMoveCb(MOUSEMOVE_CALLBACK_WRAPPER cb)
{
	m_mmcbWrapper = cb;
}
//
void CDiv::setMouseLeaveCb(MOUSELEAVE ml)
{
	m_ml = ml;
}
void CDiv::setMouseLeaveCb(MOUSELEAVE_CALLBACK_WRAPPER cb)
{
	m_mlcbWrapper = cb;
}
//
void CDiv::setText(std::string const& strText)
{
	m_strText = strText;
}
//
bool CDiv::onLButtonDown(int x, int y)
{
	bool bRet = false;
	if (!hitTest(x, y))
	{
		return bRet;
	}
	bRet = true;
	if (this->isTransparent())	//透明元素,允许消息穿透
	{
		bRet = false;
	}
	//
	for (std::map<int, std::deque<CDiv*> >::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		for (auto it2 = it->second.rbegin(); it2 != it->second.rend(); it2++)
		{
			if ((*it2)->onLButtonDown(x, y))
			{
				bRet = true;
				break;
			}
		}
	}
	//
	m_bFocus = true;
	CMouseEvent e;
	e.pTarget = this;
	e.nMouseX = x;
	e.nMouseY = y;
	if (m_mousedown != NULL)
	{
		m_mousedown(e);
	}
	if (m_mdcbWrapper)
	{
		m_mdcbWrapper(e);
	}
	if (this->isDraggable())
	{
		PostMessage(m_pUIMgr->getHwndContainer(), WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
		//SendMessage(m_pUIMgr->getHwndContainer(), WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
	return bRet;
}
bool CDiv::onLButtonUp(int x, int y)
{
	bool bRet = false;
	if (!hitTest(x, y))
	{
		return bRet;
	}
	bRet = true;
	//
	if (this->isTransparent())	//透明元素,允许消息穿透
	{
		bRet = false;
	}
	//
	for (std::map<int, std::deque<CDiv*> >::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		for (auto it2 = it->second.rbegin(); it2 != it->second.rend(); it2++)
		{
			if ((*it2)->onLButtonUp(x, y))
			{
				bRet = true;
				break;
			}
		}
	}

	POINT pt = { x, y };
	if (m_bFocus)
	{
		m_bFocus = false;

		CMouseEvent e;
		e.pTarget = this;
		e.nMouseX = x;
		e.nMouseY = y;
		if (m_mouseup != NULL)
		{
			m_mouseup(e);
		}
		//
		if (m_mucbWrapper)
		{
			m_mucbWrapper(e);
		}
		//
		if (m_click != NULL)
		{
			m_click(e);
		}
		//
		if (m_clickWrapper)
		{
			m_clickWrapper(e);
		}
	}
	return bRet;
}

bool CDiv::onLButtonDbClick(int x, int y)
{
	bool bRet = false;
	if (!hitTest(x, y))
	{
		return bRet;
	}
	bRet = true;
	if (this->isTransparent())	//透明元素,允许消息穿透
	{
		bRet = false;
	}
	//
	for (std::map<int, std::deque<CDiv*> >::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		for (auto it2 = it->second.rbegin(); it2 != it->second.rend(); it2++)
		{
			if ((*it2)->onLButtonDbClick(x, y))
			{
				bRet = true;
				break;
			}
		}
	}
	//
	m_bFocus = true;
	CMouseEvent e;
	e.pTarget = this;
	e.nMouseX = x;
	e.nMouseY = y;

	if (m_lbuttonDbClickWrapper)
	{
		m_lbuttonDbClickWrapper(e);
	}
	return bRet;
}

bool CDiv::onMouseMove(int x, int y)
{
	bool bRet = false;
	if (!hitTest(x, y))
	{
		onMouseLeave();
		return bRet;
	}
	bRet = true;
	if (this->isTransparent())	//透明元素,允许消息穿透
	{
		bRet = false;
	}
	//
	for (std::map<int, std::deque<CDiv*> >::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		for (auto it2 = it->second.rbegin(); it2 != it->second.rend(); it2++)
		{
			if ((*it2)->onMouseMove(x, y))
			{
				//bRet = true;
			}
		}
	}

	//if (PtInRegion(m_rgn, x, y) /*&& m_bMouseMove == false*/)
	{
		m_bMouseMove = true;
		CMouseEvent e;
		e.pTarget = this;
		e.nMouseX = x;
		e.nMouseY = y;
		if (m_mm != NULL)
		{
			m_mm(e);
		}
		if (m_mmcbWrapper)
		{
			m_mmcbWrapper(e);
		}
	}
	return bRet;
}
bool CDiv::onMouseLeave()
{
	bool bRet = false;
	if (m_bMouseMove)
	{
		m_bMouseMove = false;

		for (std::map<int, std::deque<CDiv*> >::iterator it = m_children.begin(); it != m_children.end(); it++)
		{
			for (auto it2 = it->second.rbegin(); it2 != it->second.rend(); it2++)
			{
				(*it2)->onMouseLeave();
			}
		}

		bRet = true;
		CMouseEvent e;
		e.pTarget = this;
		e.nMouseX = 0;
		e.nMouseY = 0;
		if (m_ml != NULL)
		{
			m_ml(e);
		}
		if (m_mlcbWrapper)
		{
			m_mlcbWrapper(e);
		}
	}
	return bRet;
}

void CDiv::onPaint(Gdiplus::Graphics& g)
{
	if (m_bVisible == false)
		return;
	
	Gdiplus::Region rgnOld;
	auto enter = [this, &g, &rgnOld]()
	{
		if (this->isClip())		//update clip region
		{
			POINT ptAbs = getAbsPosition();
			g.GetClip(&rgnOld);
			Gdiplus::Rect rect{ ptAbs.x, ptAbs.y, m_nWidth, m_nHeight };
			g.SetClip(rect, Gdiplus::CombineMode::CombineModeIntersect);
		}
	};
	auto leave = [this, &g, &rgnOld]()
	{
		if (this->isClip())		//restore clip region
		{
			g.SetClip(&rgnOld);
		}
	};
	SafeLeave<void(), void()> sl(enter, leave);

	POINT ptAbs = this->getAbsPosition();
	Gdiplus::Rect rect{ptAbs.x, ptAbs.y, m_nWidth, m_nHeight};
	g.FillRectangle(&m_backgroundBrush, rect);
	g.DrawRectangle(&m_pen, rect);
	
	if (m_bkImgData != NULL)
	{
		Gdiplus::Bitmap bmp(m_nBkImgDataWidth, m_nBkImgDataHeight,
			m_nBkImgDataWidth * 3, PixelFormat24bppRGB,
			(BYTE*)m_bkImgData);
		g.DrawImage(&bmp, rect, 0, 0, m_nBkImgDataWidth, m_nBkImgDataHeight, Gdiplus::UnitPixel);
	}
	//background-image
	if (m_bkImgSprite.img != NULL)
	{
		g.DrawImage(m_bkImgSprite.img, rect,
			m_bkImgSprite.srcRc.X,
			m_bkImgSprite.srcRc.Y,
			m_bkImgSprite.srcRc.Width,
			m_bkImgSprite.srcRc.Height,
			Gdiplus::UnitPixel);
	}
 	
	Gdiplus::Font font(util::str_2_wstr(m_strFontName).c_str(), 12);
	Gdiplus::RectF rectf{float(ptAbs.x), float(ptAbs.y), float(m_nWidth), float(m_nHeight)};
	g.DrawString(util::str_2_wstr(m_strText).c_str(), -1, &font,rectf, &m_strFormat, &m_textBrush);
	for (std::map<int, std::deque<CDiv*> >::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			(*it2)->onPaint(g);
		}
	}
}

void CDiv::setBorderColor(Gdiplus::Color color)
{
	m_pen.SetColor(color);
}

void CDiv::setBorderWidth(int nWidth)
{
	m_pen.SetWidth(float(nWidth));
}

bool CDiv::hitTest(int x, int y)
{
	if (m_bVisible == false)
		return false;

	POINT ptAbs = getAbsPosition();
	RECT rcAbs{ ptAbs.x, ptAbs.y, ptAbs.x + m_nWidth, ptAbs.y + m_nHeight };
	if (isClip())
	{
		if (PtInRect(&rcAbs, POINT{ x, y }))
		{
			return true;
		}
		return false;
	}
	if (!isClip())
	{
		if (PtInRect(&rcAbs, POINT{ x, y }))
		{
			return true;
		}

		for (std::map<int, std::deque<CDiv*> >::iterator it = m_children.begin(); it != m_children.end(); it++)
		{
			for (auto it2 = it->second.rbegin(); it2 != it->second.rend(); it2++)
			{
				if ((*it2)->hitTest(x, y))
				{
					return true;
				}
			}
		}
	}
	return false;
}

// void CDiv::updateRgn(DIRECTION dir /*= DIR_DOWN*/)
// {
// 	POINT ptAbs = this->getAbsPosition();
// 	if (isVisible())
// 	{
// 		BOOL bRet = SetRectRgn(m_rgn, ptAbs.x, ptAbs.y, ptAbs.x + m_nWidth - 1, ptAbs.y + m_nHeight - 1);
// 	}
// 	else
// 	{
// 		//不可见的元素不占空间
// 		BOOL bRet = SetRectRgn(m_rgn, 0, 0, 0, 0);
// 	}
// 
// 	if (dir == DIR_DOWN)
// 	{
// 		for (auto it = m_children.begin(); it != m_children.end(); it++)
// 		{
// 			std::deque<CDiv*>& dq = it->second;
// 			for (auto it2 = dq.begin(); it2 != dq.end(); it2++)
// 			{
// 				(*it2)->updateRgn();
// 			}
// 		}
// 		combineChildrenRgn();
// 	}
// 
// 	if (dir == DIR_UP)
// 	{
// 		combineChildrenRgn();
// 		//继续更新父元素rgn
// 		if (m_pDivParent != NULL)
// 		{
// 			m_pDivParent->updateRgn(DIR_UP);
// 		}
// 	}
//}

//
// void CDiv::combineChildrenRgn()
// {
// 	//当m_bVisible==false时, 无需合并子元素rgn
// 	if (m_bVisible == false)
// 	{
// 		return;
// 	}
// 	//当m_bClip==true时,超出自己范围的子元素被剪裁,无需合并子元素rgn
// 	if (m_bClip == true)
// 	{
// 		return;
// 	}
// 	for (auto it = m_children.begin(); it != m_children.end(); it++)
// 	{
// 		std::deque<CDiv*>& dq = it->second;
// 		for (auto it2 = dq.begin(); it2 != dq.end(); it2++)
// 		{
// 			HRGN hRgn = (*it2)->getRgn();
// 			::CombineRgn(m_rgn, m_rgn, hRgn, RGN_OR);
// 		}
// 	}
//}

void CDiv::setUIMgr(CUIMgr* pUIMgr)
{
	m_pUIMgr = pUIMgr;
	for (auto it = m_children.begin(); it != m_children.end(); it++)
	{
		std::deque<CDiv*>& dq = it->second;
		for (auto it2 = dq.begin(); it2 != dq.end(); it2++)
		{
			(*it2)->setUIMgr(pUIMgr);
		}
	}
}
HFONT CDiv::myCreateFont(std::string strFontName, int nFontSize)
{
	HFONT hFont = ::CreateFontA(nFontSize, 0, 0, 0,
		FW_DONTCARE,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY,
		VARIABLE_PITCH,
		strFontName.c_str());
	return hFont;
}

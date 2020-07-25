/***************************************************************************
Author: victor cheng
**************************************************************************/
#include "pch.h"
#include "Div.h"
#include "UIMgr.h"
#include "CTools.h"

CDiv::CDiv(std::string strID)
{
	m_strID = strID;
	m_nZIndex = 0;
	m_bFocus = false;
	
	m_bMouseMove = false;
	m_pDivParent = NULL;
	//m_strFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
	//
	m_mousedown = NULL;
	m_mouseup = NULL;
	m_click = NULL;
	m_mm = NULL;
	m_ml = NULL;
	//
	//m_borderColor = Corona::Color(255, 255, 0, 0);
	//m_backgroundColor = Corona::Color(255, 255, 0, 0);
}

CDiv::~CDiv(void)
{
}

DivPtr CDiv::build(std::string const& id)
{
	auto pDiv = std::make_shared<CDiv>(id);
	return pDiv;
}

void CDiv::setID(std::string const& id)
{
	this->m_strID = id;
}

DivPtr CDiv::clone(DivPtr pParent)
{
	DivPtr p = CDiv::build("");
	*p = *this;
	p->m_children.clear();
	p->m_pDivParent = NULL;
	for (auto it = m_children.begin(); it != m_children.end(); it++)
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			DivPtr pChild = (*it2)->clone(p);
			p->addChild(pChild);
		}
	}
	if (pParent)
	{
		pParent->addChild(p);
	}
	return p;
}

std::string const& CDiv::getID()
{
	return m_strID;
}

CUIMgr* CDiv::getUIMgr()
{
	return m_pUIMgr;
}

void CDiv::setLeft(int left, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_nLeftRelative = left;
		m_styleHover.m_nLeftRelative = left;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_nLeftRelative = left;
	}
}

void CDiv::setTop(int top, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_nTopRelative = top;
		m_styleHover.m_nTopRelative = top;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_nTopRelative = top;
	}
}

void CDiv::setPosition(int nLeft, int nTop, DIV_STATE state /* = STATE_NORMAL*/)
{
	setLeft(nLeft);
	setTop(nTop);
}
POINT CDiv::getPosition()
{
	if (m_bMouseMove)
	{
		POINT pt = { m_styleHover.m_nLeftRelative, m_styleHover.m_nTopRelative };
		return pt;
	}
	else
	{
		POINT pt = { m_styleNormal.m_nLeftRelative, m_styleNormal.m_nTopRelative };
		return pt;
	}
	
}
void CDiv::setWidth(int nWidth, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_nWidth = nWidth;
		m_styleHover.m_nWidth = nWidth;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_nWidth = nWidth;
	}
}
int CDiv::getWidth()
{
	if (m_bMouseMove)
	{
		return m_styleHover.m_nWidth;
	}
	else
	{
		return m_styleNormal.m_nWidth;
	}
	
}
void CDiv::setHeight(int nHeight, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_nHeight = nHeight;
		m_styleHover.m_nHeight = nHeight;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_nHeight = nHeight;
	}
}

int CDiv::getHeight()
{
	if (m_bMouseMove)
	{
		return m_styleHover.m_nHeight;
	}
	else
	{
		return m_styleNormal.m_nHeight;
	}
}

POINT CDiv::getAbsPosition()
{
	POINT pt = { 0 };
	int l = m_styleNormal.m_nLeftRelative;
	int t = m_styleNormal.m_nTopRelative;
	if (m_bMouseMove)
	{
		int l = m_styleHover.m_nLeftRelative;
		int t = m_styleHover.m_nTopRelative;
	}
	if (m_pDivParent != NULL)
	{
		POINT ptParent = m_pDivParent->getAbsPosition();
		pt.x = ptParent.x + l;
		pt.y = ptParent.y + t;
	}
	else
	{
		pt.x = l;
		pt.y = t;
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
	m_styleNormal.m_bVisible = bVisible;
	m_styleHover.m_bVisible = bVisible;
}

bool CDiv::isVisible()
{
	//只有当父元素和自己都是可见状态时，才可见
	return m_styleNormal.m_bVisible;
}

void CDiv::setZIndex(int nZIndex)
{
	m_nZIndex = nZIndex;
}

int CDiv::getZIndex()
{
	return m_nZIndex;
}

void CDiv::setClip(bool bClip, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_bClip = bClip;
		m_styleHover.m_bClip = bClip;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_bClip = bClip;
	}
}

bool CDiv::isClip()
{
	if (m_bMouseMove)
	{
		return m_styleHover.m_bClip;
	}
	else
	{
		return m_styleNormal.m_bClip;
	}
}

void CDiv::setTransparent(bool bTrans)
{
	m_bTransparent = bTrans;
}

bool CDiv::isTransparent()
{
	return m_bTransparent;
}

void CDiv::setBackgroundColor(Corona::Color color, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_backgroundColor = color;
		m_styleHover.m_backgroundColor = color;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_backgroundColor = color;
	}
}

void CDiv::setBackgroundImage(char* data, int width, int height)
{
	m_styleNormal.m_bkImgData = data;
	m_styleNormal.m_nBkImgDataWidth = width;
	m_styleNormal.m_nBkImgDataHeight = height;

	m_styleHover.m_bkImgData = data;
	m_styleHover.m_nBkImgDataWidth = width;
	m_styleHover.m_nBkImgDataHeight = height;
}

void CDiv::setBackgroundImage(std::wstring const& fileName, Corona::Rect const& srcRc, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_bkImgSprite.fileName = fileName;
		m_styleNormal.m_bkImgSprite.srcRect = srcRc;
		m_styleHover.m_bkImgSprite.fileName = fileName;
		m_styleHover.m_bkImgSprite.srcRect = srcRc;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_bkImgSprite.fileName = fileName;
		m_styleHover.m_bkImgSprite.srcRect = srcRc;
	}
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
void CDiv::setText(std::wstring const& strText, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_strText = strText;
		m_styleHover.m_strText = strText;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_strText = strText;
	}
}
//
void CDiv::setFontName(std::wstring const& strFontName, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_font.fontName = strFontName;
		m_styleHover.m_font.fontName = strFontName;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_font.fontName = strFontName;
	}
}
void CDiv::setFontSize(int nFontSize, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_font.fontSize = nFontSize;
		m_styleHover.m_font.fontSize = nFontSize;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_font.fontSize = nFontSize;
	}
}

void CDiv::setAlignment(Corona::ALIGNMENT h_align, Corona::ALIGNMENT v_align, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_strFormat.alignment = h_align;
		m_styleNormal.m_strFormat.lineAlignment = v_align;
		m_styleHover.m_strFormat.alignment = h_align;
		m_styleHover.m_strFormat.lineAlignment = v_align;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_strFormat.alignment = h_align;
		m_styleHover.m_strFormat.lineAlignment = v_align;
	}
}

void CDiv::setFormatFlags(Corona::StringFormatFlags formatFlags, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_strFormat.formatFlags = formatFlags;
	}
	else
	{
		m_styleHover.m_strFormat.formatFlags = formatFlags;
	}
}

void CDiv::setTextColor(Corona::Color color, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_textColor = color;
		m_styleHover.m_textColor = color;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_textColor = color;
	}
}
void CDiv::setBorderColor(Corona::Color color, DIV_STATE state /* = STATE_NORMAL*/)
{
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_borderColor = color;
		m_styleHover.m_borderColor = color;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_borderColor = color;
	}
}

void CDiv::setBorderWidth(int nWidth, DIV_STATE state /* = STATE_NORMAL*/)
{
	//m_pen.SetWidth(float(nWidth));
	if (state == STATE_NORMAL)
	{
		m_styleNormal.m_borderWidth = nWidth;
		m_styleHover.m_borderWidth = nWidth;
	}
	else  //(state == STATE_HOVER)
	{
		m_styleHover.m_borderWidth = nWidth;
	}
}
//
void CDiv::addChild(DivPtr pDivChild)
{
	int nZIndex = pDivChild->getZIndex();
	if (m_children.find(nZIndex) != m_children.end())
	{
		m_children[nZIndex].push_back(pDivChild);
	}
	else
	{
		std::deque<DivPtr> dq;
		dq.push_front(pDivChild);
		m_children[nZIndex] = dq;
	}
	pDivChild->setParent(this);
	pDivChild->setUIMgr(m_pUIMgr);
}
DivPtr CDiv::getChildByID(std::string const& strID)
{
	DivPtr pChild = NULL;
	for (auto it = m_children.begin(); it != m_children.end(); it++)
	{
		std::deque<DivPtr>& dq = it->second;
		for (size_t j = 0; j < dq.size(); j++)
		{
			DivPtr pDiv = dq[j];
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
	for (auto it = m_children.begin(); it != m_children.end(); it++)
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
	for (auto it = m_children.begin(); it != m_children.end(); it++)
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
	for (auto it = m_children.begin(); it != m_children.end(); it++)
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
	DivPtr move_div = NULL;
	for (auto it = m_children.begin(); it != m_children.end(); it++)
	{
		for (auto it2 = it->second.rbegin(); it2 != it->second.rend(); it2++)
		{
			if ((*it2)->onMouseMove(x, y))
			{
				move_div = (*it2);
				//break;
			}
		}
	}
// 	for (auto it = m_children.begin(); it != m_children.end(); it++)
// 	{
// 		for (auto it2 = it->second.rbegin(); it2 != it->second.rend(); it2++)
// 		{
// 			//
// 			if ((*it2) != move_div)
// 			{
// 				(*it2)->onMouseLeave();
// 			}
// 		}
// 	}

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

		for (auto it = m_children.begin(); it != m_children.end(); it++)
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

void CDiv::onPaint(Corona::CGraphic& g)
{
	Style* style = &m_styleNormal;
	if (m_bMouseMove)
	{
		style = &m_styleHover;
	}
	if (style->m_bVisible == false)
		return;

	auto enter = [this, &g, &style]()
	{
		if (this->isClip())		//update clip region
		{
			g.save_clip();
			POINT ptAbs = getAbsPosition();
			g.set_clip(Corona::Rect{ptAbs.x, ptAbs.y, style->m_nWidth, style->m_nHeight});
		}
	};
	auto leave = [this, &g]()
	{
		if (this->isClip())		//restore clip region
		{
			g.restore_clip();
		}
	};
	SafeLeave<void(), void()> sl(enter, leave);

	POINT ptAbs = this->getAbsPosition();
	Corona::Rect rect{ ptAbs.x, ptAbs.y, style->m_nWidth, style->m_nHeight };
 	g.fill_rectangle(rect.x, rect.y, rect.width, rect.height, style->m_backgroundColor);
	//todo:background image
	if (style->m_bkImgSprite.fileName != L"")
	{
		g.draw_image(style->m_bkImgSprite.fileName, rect, style->m_bkImgSprite.srcRect);
	}
	if (style->m_bkImgData != NULL)
	{
		Corona::Rect srcRect{0, 0, style->m_nBkImgDataWidth, style->m_nBkImgDataHeight};
		g.draw_image(style->m_bkImgData, rect, srcRect);
	}
	g.draw_rectangle(rect.x, rect.y, rect.width, rect.height, style->m_borderColor, style->m_borderWidth);
	g.draw_string(style->m_strText.c_str(), style->m_font, rect , style->m_textColor, style->m_strFormat);
	
	for (auto it = m_children.begin(); it != m_children.end(); it++)
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			(*it2)->onPaint(g);
		}
	}
}

bool CDiv::hitTest(int x, int y)
{
	Style* style = &m_styleNormal;
	if (m_bMouseMove)
	{
		style = &m_styleHover;
	}
	if (style->m_bVisible == false)
		return false;

	POINT ptAbs = getAbsPosition();
	RECT rcAbs{ ptAbs.x, ptAbs.y, ptAbs.x + style->m_nWidth, ptAbs.y + style->m_nHeight };
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

		for (auto it = m_children.begin(); it != m_children.end(); it++)
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
		std::deque<DivPtr>& dq = it->second;
		for (auto it2 = dq.begin(); it2 != dq.end(); it2++)
		{
			(*it2)->setUIMgr(pUIMgr);
		}
	}
}

Corona::Rect CDiv::calcChldrenBounds()
{
	Style* style = &m_styleNormal;
	if (m_bMouseMove)
	{
		style = &m_styleHover;
	}

	POINT ptAbs = getAbsPosition();
	Corona::Rect rect{ptAbs.x, ptAbs.y, style->m_nWidth, style->m_nHeight};
	Corona::Rect result;

	for (auto it = m_children.begin(); it != m_children.end(); it++)
	{
		for (auto it2 = it->second.rbegin(); it2 != it->second.rend(); it2++)
		{
			Corona::Rect rc = calcChldrenBounds();
			result = Corona::Rect::Union(rect, rc);
		}
	}

	return result;
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

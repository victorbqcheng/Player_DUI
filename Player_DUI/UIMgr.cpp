/***************************************************************************
Author: victor cheng 2019
**************************************************************************/


#include "pch.h"


#include "UIMgr.h"
#include "Div.h"

static std::vector<std::string> splitString(std::string str, std::string sep)
{
	std::vector<std::string> result;
	std::string::size_type pos;

	str += sep;
	std::string::size_type size = str.size();
	for(std::string::size_type i=0; i<size; i++)
	{
		pos = str.find(sep, i);
		if(pos < size)
		{
			std::string s = str.substr(i, pos-i);
			result.push_back(s);
			i = pos + sep.size() - 1;
		}
	}
	return result;
}

#define REGISTER_PARSE_ATTR_METHOD(attr)\
	m_vecParseAttrMethods.push_back(&CUIMgr::parseAttr_##attr);

CUIMgr::CUIMgr( int nID )
{
	m_nTimerID = nID;
	m_hRgnClip = CreateRectRgn(0, 0, 0, 0);
	REGISTER_PARSE_ATTR_METHOD(left);
	REGISTER_PARSE_ATTR_METHOD(width);
	REGISTER_PARSE_ATTR_METHOD(height);
	REGISTER_PARSE_ATTR_METHOD(bkcolor);
	REGISTER_PARSE_ATTR_METHOD(borderWidth);
	REGISTER_PARSE_ATTR_METHOD(borderColor);
	REGISTER_PARSE_ATTR_METHOD(fontName);
	REGISTER_PARSE_ATTR_METHOD(fontSize);
	REGISTER_PARSE_ATTR_METHOD(text);
	REGISTER_PARSE_ATTR_METHOD(textColor);
	REGISTER_PARSE_ATTR_METHOD(textFormat);
	REGISTER_PARSE_ATTR_METHOD(onclick);
}

CUIMgr::~CUIMgr(void)
{
	DeleteObject(m_hRgnClip);
}

bool CUIMgr::hookWndMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_CREATE:
		{
			m_hWndContainer = hWnd;
			SetTimer(hWnd, m_nTimerID, 20, NULL);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			this->onLButtonDown(x, y);
		}
		break;
	case WM_LBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			this->onLButtonUp(x, y);
		}
		break;
	case WM_MOUSEMOVE:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			this->onMouseMove(x, y);
		}
		break;
	
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);

			HDC hDC = GetDC(hWnd);
			onPaint(hWnd, hDC);
			ReleaseDC(hWnd, hDC);

		}
		break;
	case WM_TIMER:
		{
			HDC hDC = GetDC(hWnd);
			//onPaint(hWnd, hDC);
			ReleaseDC(hWnd, hDC);
		}
		break;
	case WM_SIZE:
		{
			int w = LOWORD(lParam);
			int h = HIWORD(lParam);
			SetRectRgn(m_hRgnClip, 0, 0, w, h);
		}
		break;
	}
	return true;
}
bool CUIMgr::parseResource(std::string strFileName)
{
	MSXML2::IXMLDOMDocument2Ptr xmlDoc;
	xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));

	MSXML2::IXMLDOMNodePtr divNode = NULL;
	xmlDoc->load(strFileName.c_str());
	
	divNode = xmlDoc->selectSingleNode("div");
	
	CDiv* pDiv = parseDiv(divNode);
	this->addElement(pDiv);
	return true;
}
bool CUIMgr::parseResource(char const* buf)
{
	MSXML2::IXMLDOMDocument2Ptr xmlDoc;
	xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));

	MSXML2::IXMLDOMNodePtr divNode = NULL;
	xmlDoc->loadXML(buf);

	divNode = xmlDoc->selectSingleNode("div");

	CDiv* pDiv = parseDiv(divNode);
	this->addElement(pDiv);
	return true;
}
bool CUIMgr::parseResource(int nResID)
{
	HRSRC hRes = ::FindResourceA(NULL, MAKEINTRESOURCEA(nResID), "xml");
	if(NULL == hRes)
	{
		return FALSE;
	}
	HGLOBAL hG = ::LoadResource(NULL, hRes);
	if(hG == NULL)
	{
		return FALSE;
	}
	char* buf = (char*)LockResource(hG);
	return this->parseResource(buf);
}
void CUIMgr::registerClickHandler(std::string strHandlerName, CLICK ck)
{
	m_mapClickHandler[strHandlerName] = ck;
}
CDiv* CUIMgr::parseDiv(DOMNode divNode)
{
	DOMNodeAttrs attrs = divNode->Getattributes();
	DOMNode idAttr = attrs->getNamedItem("id");
	VARIANT varID = idAttr->GetnodeValue();
	std::string strID = (_bstr_t)varID.bstrVal;

	CDiv* pDiv = new CDiv(strID);

	for(size_t i=0; i<m_vecParseAttrMethods.size(); i++)
	{
		(this->*m_vecParseAttrMethods[i])(pDiv, attrs);
	}

	DOMNodes childrenNode = divNode->selectNodes("div");
	for(int i=0; i<childrenNode->Getlength(); i++)
	{
		DOMNode divNodeChild = childrenNode->Getitem(i);
		CDiv* pDivChild = parseDiv(divNodeChild);
		pDiv->addChild(pDivChild);
	}

	return pDiv;
}
void CUIMgr::parseAttr_left(DivPtr& pDiv, DOMNodeAttrs const& attrs)
{
	int nLeft = 0;
	int nTop = 0;
	DOMNode leftAttr = attrs->getNamedItem("left");
	if(leftAttr != NULL)
	{
		VARIANT varLeft = leftAttr->GetnodeValue();
		nLeft = atoi((_bstr_t)varLeft.bstrVal);
	}

	DOMNode topAttr = attrs->getNamedItem("top");
	if(topAttr != NULL)
	{
		VARIANT varTop = topAttr->GetnodeValue();
		nTop = atoi((_bstr_t)varTop.bstrVal);
	}
	pDiv->setPosition(nLeft, nTop);
}
void CUIMgr::parseAttr_width(DivPtr& pDiv, DOMNodeAttrs const& attrs)
{
	DOMNode widthAttr = attrs->getNamedItem("width");
	if(widthAttr != NULL)
	{
		VARIANT varWidth = widthAttr->GetnodeValue();
		std::string strWidth = (_bstr_t)varWidth.bstrVal;
		if(strWidth == "full")
		{
		}
		else
		{
			int nWidth = atoi(strWidth.c_str());
			pDiv->setWidth(nWidth);
		}
	}
}
void CUIMgr::parseAttr_height(DivPtr& pDiv, DOMNodeAttrs const& attrs)
{
	DOMNode heightAttr = attrs->getNamedItem("height");
	if(heightAttr != NULL)
	{
		VARIANT varHeight = heightAttr->GetnodeValue();
		std::string strHeight = (_bstr_t)varHeight.bstrVal;
		if(strHeight == "full")
		{
		}
		else
		{
			int nHeight = atoi(strHeight.c_str());
			pDiv->setHeight(nHeight);
		}
	}
}
void CUIMgr::parseAttr_bkcolor(DivPtr& pDiv, DOMNodeAttrs const& attrs)
{
	DOMNode bkcolorAttr = attrs->getNamedItem("bkcolor");
	if(bkcolorAttr != NULL)
	{
		VARIANT varBkcolor = bkcolorAttr->GetnodeValue();
		std::string strBkColor = ((_bstr_t)varBkcolor.bstrVal);
		std::vector<std::string> vecBkColor = splitString(strBkColor, ",");
		int nRed = atoi(vecBkColor[0].c_str());
		int nGreen = atoi(vecBkColor[1].c_str());
		int nBlue = atoi(vecBkColor[2].c_str());
		pDiv->setBackgroundColor(RGB(nRed, nGreen, nBlue));
	}
}
void CUIMgr::parseAttr_borderWidth(DivPtr& pDiv, DOMNodeAttrs const& attrs)
{
	DOMNode borderWidthAttr = attrs->getNamedItem("borderWidth");
	if(borderWidthAttr != NULL)
	{
		VARIANT varBorderWidth = borderWidthAttr->GetnodeValue();
		int nBorderWidth = atoi((_bstr_t)varBorderWidth.bstrVal);
		pDiv->setBorderWidth(nBorderWidth);
	}
}
void CUIMgr::parseAttr_borderColor(DivPtr& pDiv, DOMNodeAttrs const& attrs)
{
	DOMNode borderColorAttr = attrs->getNamedItem("borderColor");
	if(borderColorAttr != NULL)
	{
		VARIANT varBorderColor = borderColorAttr->GetnodeValue();
		std::string strBorderColor = (_bstr_t)varBorderColor.bstrVal;
		std::vector<std::string> vecBorderColor = splitString(strBorderColor, ",");
		int nRed = atoi(vecBorderColor[0].c_str());
		int nGreen = atoi(vecBorderColor[1].c_str());
		int nBlue = atoi(vecBorderColor[2].c_str());
		pDiv->setBorderColor(RGB(nRed, nGreen, nBlue));
	}
}
void CUIMgr::parseAttr_fontName(DivPtr& pDiv, DOMNodeAttrs const& attrs)
{
	DOMNode fontNameAttr = attrs->getNamedItem("fontName");
	if(fontNameAttr != NULL)
	{
		VARIANT varFontName = fontNameAttr->GetnodeValue();
		std::string strFontName = (_bstr_t)varFontName.bstrVal;
		pDiv->setFontName(strFontName);
	}
}
void CUIMgr::parseAttr_fontSize(DivPtr& pDiv, DOMNodeAttrs const& attrs)
{
	DOMNode fontSizeAttr = attrs->getNamedItem("fontSize");
	if(fontSizeAttr != NULL)
	{
		VARIANT varFontSize = fontSizeAttr->GetnodeValue();
		int nFontSize = atoi((_bstr_t)varFontSize.bstrVal);
		pDiv->setFontSize(nFontSize);
	}
}
void CUIMgr::parseAttr_text(DivPtr& pDiv, DOMNodeAttrs const& attrs)
{
	DOMNode textAttr = attrs->getNamedItem("text");
	if(textAttr != NULL)
	{
		VARIANT varText = textAttr->GetnodeValue();
		std::string strText = (_bstr_t)varText.bstrVal;
		pDiv->setText(strText);
	}
}
void CUIMgr::parseAttr_textColor(DivPtr& pDiv, DOMNodeAttrs const& attrs)
{
	DOMNode textColorAttr = attrs->getNamedItem("textColor");
	if(textColorAttr != NULL)
	{
		VARIANT varTextColor = textColorAttr->GetnodeValue();
		std::string strTextColor = (_bstr_t)varTextColor.bstrVal;
		std::vector<std::string> vecTextColor = splitString(strTextColor, ",");
		int nRed = atoi(vecTextColor[0].c_str());
		int nGreen = atoi(vecTextColor[1].c_str());
		int nBlue = atoi(vecTextColor[2].c_str());
		pDiv->setTextColor(RGB(nRed, nGreen, nBlue));
	}
}
void CUIMgr::parseAttr_onclick(DivPtr& pDiv, DOMNodeAttrs const& attrs)
{
	DOMNode onclickAttr = attrs->getNamedItem("onclick");
	if(onclickAttr != NULL)
	{
		VARIANT varOnclick = onclickAttr->GetnodeValue();
		std::string strOnclick = (_bstr_t)varOnclick.bstrVal;
		std::map<std::string, CLICK>::iterator it = m_mapClickHandler.find(strOnclick);
		if(it != m_mapClickHandler.end())
		{
			pDiv->setClickCb(it->second);
		}
	}
}
void CUIMgr::parseAttr_textFormat(DivPtr& pDiv, DOMNodeAttrs const& attrs)
{
	DOMNode textFormatAttr = attrs->getNamedItem("textFormat");
	if(textFormatAttr != NULL)
	{
		VARIANT varTextFormat = textFormatAttr->GetnodeValue();
		std::string strTextFormat = (_bstr_t)varTextFormat.bstrVal;
		if(strTextFormat == "HCENTER")
		{
			pDiv->setTextFormat(DT_CENTER);
		}
		else if(strTextFormat == "VCENTER")
		{
			pDiv->setTextFormat(DT_VCENTER|DT_SINGLELINE);
		}
		else if(strTextFormat == "CENTER")
		{
			pDiv->setTextFormat(DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
		else if(strTextFormat == "LEFT")
		{
			pDiv->setTextFormat(DT_LEFT);
		}
		else if(strTextFormat == "RIGHT")
		{
			pDiv->setTextFormat(DT_RIGHT);
		}
	}
}
void CUIMgr::addElement(CDiv* pElement)
{
	m_elements[pElement->getID()] = pElement;
	pElement->setUIMgr(this);
	int nZIndex = pElement->getZIndex();
	if(m_elements2.find(nZIndex) != m_elements2.end())
	{
		m_elements2[nZIndex].push_back(pElement);
	}
	else
	{
		std::deque<CDiv*> dq;
		dq.push_front(pElement);
		m_elements2[nZIndex] = dq;
	}
}
CDiv* CUIMgr::getElementByID(std::string strID)
{
	CDiv* pDiv = NULL;

	if(m_elements.find(strID) != m_elements.end())
	{
		pDiv = m_elements[strID];
	}
	else
	{
		for (auto it = m_elements.begin(); it!=m_elements.end(); it++)
		{
			pDiv = it->second->getChildByID(strID);
			if (pDiv != NULL)
			{
				break;
			}
		}
	}

	return pDiv;
}
HWND CUIMgr::getHwndContainer()
{	
	return m_hWndContainer;
}
void CUIMgr::onLButtonDown(int x, int y)
{
	for(std::map<int, std::deque<CDiv*> >::iterator it=m_elements2.begin(); it!=m_elements2.end(); it++)
	{
		for(int i = (int)it->second.size()-1; i>=0; i--)
		{
			if (it->second[i]->onLButtonDown(x, y))
			{
				//return;
				//	break;	?
			}
			if (it->second[i]->isDraggable())
			{
				//PostMessage(m_hWndContainer, WM_SYSCOMMAND, SC_MOVE | 0x02, 0);
			}
		}
	}
}
void CUIMgr::onLButtonUp(int x, int y)
{
	for(std::map<int, std::deque<CDiv*> >::iterator it=m_elements2.begin(); it!=m_elements2.end(); it++)
	{
		for(size_t i=0; i<it->second.size(); i++)
		{
			it->second[i]->onLButtonUp(x, y);
		}
	}
}
void CUIMgr::onMouseMove(int x, int y)
{
	for(std::map<int, std::deque<CDiv*> >::iterator it=m_elements2.begin(); it!=m_elements2.end(); it++)
	{
		for(size_t i=0; i<it->second.size(); i++)
		{
			it->second[i]->onMouseMove(x, y);
		}
	}
}
void CUIMgr::onMouseLeave()
{
	for(std::map<int, std::deque<CDiv*> >::iterator it=m_elements2.begin(); it!=m_elements2.end(); it++)
	{
		for(size_t i=0; i<it->second.size(); i++)
		{
			it->second[i]->onMouseLeave();
		}
	}
}
void CUIMgr::onPaint(HWND hWnd, HDC hDC)
{
	HDC hDCMem = CreateCompatibleDC(hDC);
	{
		RECT rc = {0};
		GetClientRect(hWnd, &rc);
		int nWidth = rc.right - rc.left;
		int nHeight = rc.bottom - rc.top;
		HBITMAP hBmpMem = CreateCompatibleBitmap(hDC, nWidth, nHeight);
		SelectObject(hDCMem, hBmpMem);
		{
			::SetBkMode(hDCMem, TRANSPARENT);
			SelectClipRgn(hDCMem, m_hRgnClip);
			for(std::map<int, std::deque<CDiv*> >::iterator it=m_elements2.begin(); it!=m_elements2.end(); it++)
			{
				for(size_t i=0; i<it->second.size(); i++)
				{
					it->second[i]->onPaint(hDCMem);
				}
			}
			SelectClipRgn(hDCMem, NULL);
			BitBlt(hDC, 0, 0, nWidth, nHeight, hDCMem, 0, 0, SRCCOPY);
		}
		DeleteObject(hBmpMem);
	}
	DeleteObject(hDCMem);
}


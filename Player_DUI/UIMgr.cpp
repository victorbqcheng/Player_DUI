/***************************************************************************
Author: victor cheng 2019
**************************************************************************/


#include "pch.h"

#include <functional>

#include "UIMgr.h"
#include "Div.h"
#include "CTools.h"
#include "ComponentsCollection.h"



CUIMgr::CUIMgr( int nID )
{
	m_nTimerID = nID;
}

CUIMgr::~CUIMgr(void)
{
}

DivPtr CUIMgr::buildDiv(std::string const& id)
{
	auto pDiv = std::make_shared<CDiv>(id);
	return pDiv;
}

bool CUIMgr::hookWndMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_CREATE:
		{
			//ComponentsCollection::Instance().parseComponents("./res/components");
			//parseResource2("./res/main.xml");
			attach(hWnd);		
			SetTimer(hWnd, m_nTimerID, 40, NULL);
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
	case WM_LBUTTONDBLCLK:
		{ 
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			this->onLButtonDbClick(x, y);
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
		}
		break;
	case WM_TIMER:
		{
			render();
		}
		break;
	case WM_SIZE:
		{
			int w = LOWORD(lParam);
			int h = HIWORD(lParam);
		}
		break;
	}
	return true;
}


bool CUIMgr::parseResource2(std::string fileName)
{
	DivPtr pRoot = m_parseXML.parseResource(CTools::str_2_wstr(fileName));
	if (pRoot)
	{
		setRoot(pRoot);
		return true;
	}
	return false;
}

void CUIMgr::registerClickHandler(std::string strHandlerName, EVENT_CALLBACK_WRAPPER ck)
{
	m_mapClickHandler[strHandlerName] = ck;
}

void CUIMgr::attach(HWND hWnd)
{
	m_hWndContainer = hWnd;
	m_graphic.attach(hWnd);
}

void CUIMgr::detach()
{
}

void CUIMgr::setRoot(DivPtr pRoot)
{
	m_rootDiv = pRoot;
	m_rootDiv->setUIMgr(this);
}

DivPtr CUIMgr::getElementByID(std::string strID)
{
	DivPtr pDiv = NULL;

	if (m_rootDiv)
	{
		if (m_rootDiv->getID() == strID)
			return m_rootDiv;
		else
		{
			pDiv = m_rootDiv->getChildByID(strID);
			return pDiv;
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
	if (m_rootDiv)
	{
		m_rootDiv->onLButtonDown(x, y);
	}
}
void CUIMgr::onLButtonUp(int x, int y)
{
	if (m_rootDiv)
	{
		m_rootDiv->onLButtonUp(x, y);
	}
}

void CUIMgr::onLButtonDbClick(int x, int y)
{
	if (m_rootDiv)
	{
		m_rootDiv->onLButtonDbClick(x, y);
	}
}

void CUIMgr::onMouseMove(int x, int y)
{
	if (m_rootDiv)
	{
		m_rootDiv->onMouseMove(x, y);
	}
}
void CUIMgr::onMouseLeave()
{
	if (m_rootDiv)
	{
		m_rootDiv->onMouseLeave();
	}
}

void CUIMgr::render()
{
	m_graphic.begin();
	if (m_rootDiv)
	{
		m_rootDiv->onPaint(m_graphic);
	}
	m_graphic.end();
}


// void CUIMgr::render()
// {
// 	HDC hDC = GetDC(m_hWndContainer);
// 	Gdiplus::Graphics g(m_hDCMem);
// 	g.ResetClip();
// 	for (auto it = m_elements2.begin(); it != m_elements2.end(); it++)
// 	{
// 		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
// 		{
// 			(*it2)->onPaint(g);
// 		}
// 	}
// 	BitBlt(hDC, 0, 0, 2000, 2000, m_hDCMem, 0, 0, SRCCOPY);
// 	ReleaseDC(m_hWndContainer, hDC);
// }

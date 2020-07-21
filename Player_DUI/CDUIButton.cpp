#include "pch.h"
#include "CDUIButton.h"
#include "UIMgr.h"

void CDUIButton::mouseMoveCb(CMouseEvent e)
{
	m_pDivNormal->setVisible(false);
	m_pDivHover->setVisible(true);
}
void CDUIButton::mouseLeaveCb(CMouseEvent e)
{
	m_pDivNormal->setVisible(true);
	m_pDivHover->setVisible(false);
}

CDUIButton::CDUIButton(std::string id): CDiv(id)
{
	m_pDivNormal = CUIMgr::buildDiv("button_normal");
	{
		m_pDivNormal->setVisible(true);
	}
	m_pDivHover = CUIMgr::buildDiv("button_hover");
	{
		m_pDivHover->setVisible(false);
	}
	m_pDivDown = CUIMgr::buildDiv("button_down");
	{
		m_pDivDown->setVisible(false);
	}

	this->setMouseMoveCb(std::bind(&CDUIButton::mouseMoveCb, this, std::placeholders::_1));
	this->setMouseLeaveCb(std::bind(&CDUIButton::mouseLeaveCb, this, std::placeholders::_1));
	
	this->addChild(m_pDivNormal);
	this->addChild(m_pDivHover);
	this->addChild(m_pDivDown);
}

void CDUIButton::setButtonWidth(int nWidth)
{
	this->setWidth(nWidth);
	m_pDivNormal->setWidth(nWidth);
	m_pDivHover->setWidth(nWidth);
	m_pDivDown->setWidth(nWidth);
}

void CDUIButton::setButtonHeight(int nHeight)
{
	this->setHeight(nHeight);
	m_pDivNormal->setHeight(nHeight);
	m_pDivHover->setHeight(nHeight);
	m_pDivDown->setHeight(nHeight);
}

void CDUIButton::setBackgroundImage(std::wstring normal, Corona::Rect rcNormal, std::wstring hover, Corona::Rect rcHover, std::wstring down, Corona::Rect rcDown)
{
	m_pDivNormal->setBackgroundImage(normal, rcNormal);
	m_pDivHover->setBackgroundImage(hover, rcHover);
	m_pDivDown->setBackgroundImage(down, rcDown);
}


#include "pch.h"
#include "CDUIProgress.h"
#include "UIMgr.h"

CDUIProgress::CDUIProgress(std::string id):CDiv(id)
{
	this->setHeight(m_nDefaultHeight);
	this->setBackgroundColor(Corona::Color(0, 0, 0, 0));

	m_pDivCurrentProgress = CUIMgr::buildDiv("");
	m_pDivCurrentProgress->setHeight(m_nDefaultHeight);
	m_pDivCurrentProgress->setPosition(0, 0);
	m_pDivCurrentProgress->setBackgroundColor(Corona::Color(50, 0, 255, 0));
	this->addChild(m_pDivCurrentProgress);

	m_pDivHandle = CUIMgr::buildDiv("");
	m_pDivHandle->setWidth(m_handleLen);
	m_pDivHandle->setHeight(m_handleLen);
	m_pDivHandle->setPosition(0, 0);
	m_pDivHandle->setBackgroundColor(Corona::Color(255, 0, 0));
	m_pDivHandle->setBackgroundColor(Corona::Color(0, 0, 255), STATE_HOVER);
	this->addChild(m_pDivHandle);
}

CDUIProgress::~CDUIProgress()
{

}

void CDUIProgress::setPercentage(double percentage)
{
	m_percentage = percentage;
	int len = int(this->getWidth() * percentage + 1);
	m_pDivCurrentProgress->setWidth(len);

	int dist = this->getWidth() - m_pDivHandle->getWidth();
	int left = int(dist * percentage);
	POINT pt = m_pDivHandle->getPosition();
	m_pDivHandle->setPosition(left, pt.y);
}

void CDUIProgress::setPercentageColor(Corona::Color clr)
{
	m_pDivCurrentProgress->setBackgroundColor(clr);
}

void CDUIProgress::setHeight(int nHeight, DIV_STATE state /*= STATE_NORMAL*/)
{
	CDiv::setHeight(nHeight);
	
	if (m_pDivCurrentProgress != NULL)
	{
		m_pDivCurrentProgress->setHeight(nHeight);
	}
	if (m_pDivHandle != NULL)
	{
		m_pDivHandle->setWidth(nHeight);
		m_pDivHandle->setHeight(nHeight);
	}
	//m_pDivHandle->setPosition()
}

void CDUIProgress::setHandleLen(int n)
{
	if (n < this->getHeight())
	{
		return;
	}
	
	m_pDivHandle->setWidth(n);
	m_pDivHandle->setHeight(n);
	POINT pt = m_pDivHandle->getPosition();
	int top = -(n - this->getHeight()) / 2;
	m_pDivHandle->setPosition(pt.x, top);
}

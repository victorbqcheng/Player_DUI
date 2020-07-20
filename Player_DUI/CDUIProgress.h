#pragma once
#include "Div.h"
class CDUIProgress : public CDiv
{
public:
	CDUIProgress(std::string id);
	virtual ~CDUIProgress();
	void setPercentage(double percentage);
	void setPercentageColor(Corona::Color clr);
	virtual void setHeight(int nHeight) override;
	void setHandleLen(int n);
private:
	CDiv* m_pDivCurrentProgress = NULL;
	CDiv* m_pDivHandle = NULL;
	double m_percentage = 0.0;
	int m_nDefaultHeight = 5;
	int m_handleLen = m_nDefaultHeight;
};


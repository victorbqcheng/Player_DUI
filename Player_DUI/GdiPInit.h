#pragma once

#include <gdiplus.h>
using namespace Gdiplus;

class GdiPInit
{
public:
	GdiPInit();
	~GdiPInit();
private:
	ULONG_PTR token;
};


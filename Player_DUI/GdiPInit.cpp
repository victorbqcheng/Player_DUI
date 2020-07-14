#include "pch.h"
#include "GdiPInit.h"

GdiPInit::GdiPInit()
{
	GdiplusStartupInput input;
	GdiplusStartup(&token, &input, NULL);
}

GdiPInit::~GdiPInit()
{
	GdiplusShutdown(token);
}
GdiPInit init;
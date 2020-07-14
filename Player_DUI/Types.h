#pragma once
#include <Gdiplus.h>
class Types
{
};

struct ImgSprite
{
	Gdiplus::Image* img;
	ImgSprite():srcRc(0, 0, 0, 0), img(NULL)
	{
	}
	Gdiplus::Rect srcRc;
};


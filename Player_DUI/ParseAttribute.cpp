#include "pch.h"
#include "ParseAttribute.h"
#include "CTools.h"


#define METHOD(attr)\
	std::bind(&ParseAttribute::parse##attr, this, _1, _2, _3)

ParseAttribute::ParseAttribute()
{
	using namespace std::placeholders;
	m_methods = {
		METHOD(Left),
		METHOD(Left),
		METHOD(Top),
		METHOD(Width),
		METHOD(Height),
		METHOD(ZIndex),
		METHOD(Visible),
		METHOD(Clip),
		METHOD(Draggable),
		METHOD(Transparent),
		METHOD(BackgroundColor),
		METHOD(BackgroundImage),
		METHOD(Border),
		METHOD(Text),
		METHOD(Font)
	};
}

void ParseAttribute::parseLeft(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("left"))
	{
		int left = obj["left"].asInt();
		pDiv->setLeft(left, state);
	}

}

void ParseAttribute::parseTop(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("top"))
	{
		int top = obj["top"].asInt();
		pDiv->setTop(top, state);
	}
}

void ParseAttribute::parseWidth(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("width"))
	{
		int width = obj["width"].asInt();
		pDiv->setWidth(width, state);
	}
}

void ParseAttribute::parseHeight(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("height"))
	{
		int height = obj["height"].asInt();
		pDiv->setHeight(height, state);
	}
}

void ParseAttribute::parseZIndex(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("z-index"))
	{
		int zindex = obj["z-index"].asInt();
		pDiv->setZIndex(zindex);
	}
}

void ParseAttribute::parseVisible(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("visible"))
	{
		std::string visible = obj["visible"].asString();
		if (visible == "true")
		{
			pDiv->setVisible(true);
		}
		else
		{
			pDiv->setVisible(false);
		}
	}
}

void ParseAttribute::parseClip(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("clip"))
	{
		std::string clip = obj["clip"].asString();
		if (clip == "true")
		{
			pDiv->setClip(true, state);
		}
		else
		{
			pDiv->setClip(false, state);
		}
	}
}

void ParseAttribute::parseDraggable(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("drag"))
	{
		std::string drag = obj["drag"].asString();
		if (drag == "true")
		{
			pDiv->setDraggable(true);
		}
		else
		{
			pDiv->setDraggable(false);
		}
	}
}

void ParseAttribute::parseTransparent(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("transparent"))
	{
		std::string trans = obj["transparent"].asString();
		if (trans == "true")
		{
			pDiv->setTransparent(true);
		}
		else
		{
			pDiv->setTransparent(false);
		}
	}
}

struct U
{
	BYTE a;
	BYTE r;
	BYTE g;
	BYTE b;
};

unsigned long hexString2UL(std::string& str)
{
	std::string s = "";
	if (str[0] == '#')
	{
		s = str.substr(1);
	}
	s = "0x" + s;
	unsigned long ul = strtoul(s.c_str(), NULL, 16);
	return ul;
}

void ParseAttribute::parseBackgroundColor(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("background-color"))
	{
		std::string bkcolor = obj["background-color"].asString();
		unsigned long ul = hexString2UL(bkcolor);
		Corona::Color color(ul);
		pDiv->setBackgroundColor(color, state);
	}
}

void ParseAttribute::parseBackgroundImage(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("background-image"))
	{
		std::string url = "";
		Corona::Rect rect;
		Json::Value bkimgValue = obj["background-image"];
		if (bkimgValue.isMember("url"))
		{
			url = bkimgValue["url"].asString();
			//设置图片的宽和高, 如果有设置具体的宽高值,后面会重新设置
			std::tie(rect.width, rect.height) = Corona::Image::get_image_size(CTools::str_2_wstr(url));
		}
		if (bkimgValue.isMember("left"))
		{
			rect.x = bkimgValue["left"].asInt();
		}
		if (bkimgValue.isMember("top"))
		{
			rect.y = bkimgValue["top"].asInt();
		}
		if (bkimgValue.isMember("width"))
		{
			rect.width = bkimgValue["width"].asInt();
		}
		if (bkimgValue.isMember("height"))
		{
			rect.height = bkimgValue["height"].asInt();
		}
		pDiv->setBackgroundImage(CTools::str_2_wstr(url), rect, state);
	}
}

void ParseAttribute::parseBorder(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("border"))
	{
		Json::Value borderValue = obj["border"];
		if (borderValue.isMember("color"))
		{
			std::string borderColor = borderValue["color"].asString();
			Corona::Color color;
			(*(long*)(&color)) = hexString2UL(borderColor);
			pDiv->setBorderColor(color, state);
		}
		if (borderValue.isMember("width"))
		{
			int width = borderValue["width"].asInt();
			pDiv->setBorderWidth(width, state);
		}
	}
}

void ParseAttribute::parseText(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("text"))
	{
		Json::Value textValue = obj["text"];
		if (textValue.isMember("text"))
		{
			std::string text = textValue["text"].asString();
			pDiv->setText(CTools::str_2_wstr(text), state);
		}
		if (textValue.isMember("color"))
		{
			std::string textColor = textValue["color"].asString();
			Corona::Color color;
			(*(long*)(&color)) = hexString2UL(textColor);
			pDiv->setTextColor(color, state);
		}
		if (textValue.isMember("format"))
		{
			Json::Value formatValue = obj["format"];
			parseTextFormat(pDiv, formatValue, state);
		}
	}
}

void ParseAttribute::parseTextFormat(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	std::map<std::string, Corona::ALIGNMENT> m{
		{"near", Corona::ALIGNMENT_NEAR},
		{"center", Corona::ALIGNMENT_CENTER},
		{"far", Corona::ALIGNMENT_FAR},
	};
	Corona::ALIGNMENT al = Corona::ALIGNMENT_NEAR;
	Corona::ALIGNMENT lineAl = Corona::ALIGNMENT_NEAR;
	if (obj.isMember("alignment"))
	{
		std::string strAl = obj["alignment"].asString();
		if (m.find(strAl) != m.end())
		{
			al = m[strAl];
		}
	}
	if (obj.isMember("line-alignment"))
	{
		std::string strLineAl = obj["line-alignment"].asString();
		if (m.find(strLineAl) != m.end())
		{
			lineAl = m[strLineAl];
		}
	}
	pDiv->setAlignment(al, lineAl, state);
}

void ParseAttribute::parseFont(DivPtr& pDiv, Json::Value& obj, DIV_STATE state /*= STATE_NORMAL*/)
{
	if (obj.isMember("font"))
	{
		Corona::Font font;
		Json::Value fontValue = obj["font"];
		if (fontValue.isMember("name"))
		{
			std::string fontName = fontValue["name"].asString();
			pDiv->setFontName(CTools::str_2_wstr(fontName), state);
		}
		if (fontValue.isMember("size"))
		{
			int nSize = fontValue["size"].asInt();
			pDiv->setFontSize(nSize, state);
		}
	}
}


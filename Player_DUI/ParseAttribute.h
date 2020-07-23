#pragma once

#include <functional>
#include <json/json.h>
#include "pugixml/pugixml.hpp"
#include "div.h"

class ParseAttribute
{
	using METHOD = std::function<void(DivPtr&, Json::Value&, DIV_STATE)>;
public:
	ParseAttribute();
	//"left"
	void parseLeft(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"top"
	void parseTop(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"width"
	void parseWidth(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"height"
	void parseHeight(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"z-index"
	void parseZIndex(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"visible":true|false
	void parseVisible(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"clip":true|false
	void parseClip(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"drag":true|false
	void parseDraggable(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"transparent":true|false
	void parseTransparent(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"background-color":"#FFFF0000" --ARGB
	void parseBackgroundColor(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"background-image":{"url","left","top","width","height"}
	void parseBackgroundImage(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"border":{"color", "width"}
	void parseBorder(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"text:"{"text", "color", "format":{}}
	void parseText(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//
	void parseTextFormat(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//"font":{"name", "size"}
	void parseFont(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	//
	void parseOnMouseDown(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	void parseOnMouseUp(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	void parseOnMouseMove(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
	void parseOnMouseLeave(DivPtr& pDiv, Json::Value& obj, DIV_STATE state = STATE_NORMAL);
public:
	std::vector<METHOD> m_methods;
};


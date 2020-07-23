#pragma once
#include <functional>
#include <json/json.h>
#include "pugixml/pugixml.hpp"
#include "ParseAttribute.h"
#include "div.h"

using PARSE_ATTR_METHOD = std::function<void(DivPtr&, Json::Value&, DIV_STATE state)>;

class CParseXml
{
public:
	CParseXml();
	virtual ~CParseXml();
	DivPtr parseResource(std::wstring fileName);
private:
	void parseHeadNode();
	DivPtr parseBodyNode();
	DivPtr parseDivNode(pugi::xml_node& node);
	void setCustomSelectorAttr(DivPtr& pDiv, pugi::xml_node& node);
private:
	pugi::xml_document m_doc;
	Json::Value m_jsonRoot;
	ParseAttribute m_parseAttr;
};




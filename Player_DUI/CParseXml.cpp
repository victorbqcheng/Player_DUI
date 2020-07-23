#include "pch.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <functional>
#include "CParseXml.h"
#include "UIMgr.h"
#include "CTools.h"
#include "ParseAttribute.h"
#include "ComponentsCollection.h"


#define REGISTER_PARSE_ATTR_METHOD(attr)\
	m_parseAttrMethods.push_back(ParseAttribute::parse##attr);
CParseXml::CParseXml()
{
	//m_parseAttrMethods.push_back(ParseAttribute::parseLeft);
}

CParseXml::~CParseXml()
{
	
}

DivPtr CParseXml::parseResource(std::wstring fileName)
{
	if (!m_doc.load_file(CTools::wstr_2_str(fileName).c_str()))
		return NULL;
	parseHeadNode();
	return parseBodyNode();
}

void CParseXml::parseHeadNode()
{
	std::vector<std::string> files;
	std::string fileName = "";
	pugi::xml_node headNode = m_doc.child("root").child("head");
	if (headNode)
	{
		for (auto styleNode : headNode.children("style"))
		{
			fileName = styleNode.first_child().value();
			files.push_back(fileName);
		}
	}

	std::ifstream ifs(fileName);
	if (!ifs.fail())
	{
		Json::CharReaderBuilder builder;
		builder["collectComments"] = true;
		JSONCPP_STRING errs;
		if (!parseFromStream(builder, ifs, &m_jsonRoot, &errs))
		{
			throw errs;
		}
	}
}

DivPtr CParseXml::parseBodyNode()
{
	DivPtr rootDiv = NULL;
	pugi::xml_node bodyNode = m_doc.child("root").child("body");
	if (bodyNode)
	{
		rootDiv = CUIMgr::buildDiv("body");
		
		for (auto& childNode:bodyNode.children())
		{
			if (std::string(childNode.name()) == "div")
			{
				DivPtr div = parseDivNode(childNode);
				if (div)
				{
					rootDiv->addChild(div);
				}
			}
			else
			{
				std::string selector = childNode.name();
				DivPtr pComp = ComponentsCollection::Instance().getComponent(selector);
				if (pComp)
				{
					setCustomSelectorAttr(pComp, childNode);
					rootDiv->addChild(pComp);
				}
			}
		}
	}
	return rootDiv;
}

DivPtr CParseXml::parseDivNode(pugi::xml_node& divNode)
{
	std::string id = divNode.attribute("id").as_string();
	DivPtr pDiv = CUIMgr::buildDiv(id);

	std::string cls = divNode.attribute("class").as_string();
	std::string class_ = "." + cls;
	if (m_jsonRoot.isMember(class_.c_str()))
	{
		auto obj = m_jsonRoot[class_];
		for (auto f : m_parseAttr.m_methods)
		{
			f(pDiv, obj, STATE_NORMAL);
		}

		if (obj.isMember("hover"))
		{
			auto hoverObj = obj["hover"];
			for (auto f : m_parseAttr.m_methods)
			{
				f(pDiv, hoverObj, STATE_HOVER);
			}
		}
	}

	//´ÓjsonÉèÖÃstyle
	std::string id_ = "#" + id;
	if (m_jsonRoot.isMember(id_.c_str()))
	{
		auto obj = m_jsonRoot[id_];
		for (auto f : m_parseAttr.m_methods)
		{
			f(pDiv, obj, STATE_NORMAL);
		}
		if (obj.isMember("hover"))
		{
			auto hoverObj = obj["hover"];
			for (auto f : m_parseAttr.m_methods)
			{
				f(pDiv, hoverObj, STATE_HOVER);
			}
		}
	}

	for (auto& childNode : divNode.children())
	{
		if (std::string(childNode.name()) == "div")
		{
			DivPtr childDiv = parseDivNode(childNode);
			if (childDiv)
			{
				pDiv->addChild(childDiv);
			}
		}
		else
		{
			std::string selector = childNode.name();
			DivPtr pComp = ComponentsCollection::Instance().getComponent(selector);
			if (pComp)
			{
				setCustomSelectorAttr(pComp, childNode);
				pDiv->addChild(pComp);
			}
		}
	}

// 	for (auto childDivNode : divNode.children())
// 	{
// 		DivPtr childDiv = parseDivNode(childDivNode);
// 		pDiv->addChild(childDiv);
// 	}

	return pDiv;
}

void CParseXml::setCustomSelectorAttr(DivPtr& pDiv, pugi::xml_node& node)
{
	std::string id = node.attribute("id").as_string();
	pDiv->setID(id);

	std::string id_ = "#" + id;
	if (m_jsonRoot.isMember(id_.c_str()))
	{
		auto obj = m_jsonRoot[id_];
		for (auto f : m_parseAttr.m_methods)
		{
			f(pDiv, obj, STATE_NORMAL);
		}
		if (obj.isMember("hover"))
		{
			auto hoverObj = obj["hover"];
			for (auto f : m_parseAttr.m_methods)
			{
				f(pDiv, hoverObj, STATE_HOVER);
			}
		}
	}
}


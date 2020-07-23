#include "pch.h"
#include <fstream>
#include <filesystem>
#include <json/json.h>
#include "pugixml/pugixml.hpp"
namespace fs = std::filesystem;

#include "ComponentsCollection.h"


ComponentsCollection& ComponentsCollection::Instance()
{
	static ComponentsCollection inst;
	return inst;
}

void ComponentsCollection::parseComponents(std::string strPath)
{
	if (fs::directory_entry(strPath).status().type() != fs::file_type::directory)
		return;

	fs::directory_iterator it(strPath);
	for (auto folder : it)
	{
		std::string strFolder = folder.path().string();
		std::string selector = "";
		DivPtr pComp = NULL;
		std::tie(selector, pComp) = parseComponent(strFolder);
		if (pComp)
		{
			m_collections[selector] = pComp;
		}
	}
}

std::tuple<std::string, DivPtr> ComponentsCollection::parseComponent(std::string strPath)
{
	std::string selector = "";
	DivPtr pDiv = NULL;
	if (fs::directory_entry(strPath).status().type() != fs::file_type::directory)
		return std::make_tuple(selector, pDiv);

	std::string xmlFile = "";
	std::string jsonFile = "";
	fs::directory_iterator it(strPath);
	for (auto file : it)
	{
		if (file.path().extension().string() == ".xml")
			xmlFile = file.path().string();
		else if (file.path().extension().string() == ".json")
			jsonFile = file.path().string();
	}
	if (xmlFile != "" && jsonFile != "")
	{
		Json::Value jsonRoot;
		std::ifstream ifs(jsonFile);
		Json::CharReaderBuilder builder;
		builder["collectComments"] = true;
		JSONCPP_STRING errs;
		if (!parseFromStream(builder, ifs, &jsonRoot, &errs))
		{
			throw errs;
		}

		pugi::xml_document doc;
		if (!doc.load_file(xmlFile.c_str()))
		{
			throw "!doc.load_file" + xmlFile;
		}

// 		for (auto& childNode : doc.children("div"))
// 		{
// 			parseDivNode(childNode, jsonRoot);
// 		}
		std::tie(selector, pDiv) = parseDivNode(doc.child("div"), jsonRoot);
	}
	return std::make_tuple(selector, pDiv);;
}

DivPtr ComponentsCollection::getComponent(std::string selector)
{
	if (m_collections.find(selector) != m_collections.end())
	{
		return m_collections[selector]->clone(NULL);
	}
	return NULL;
}

std::tuple<std::string, DivPtr> ComponentsCollection::parseDivNode(pugi::xml_node& divNode, Json::Value& value)
{
	std::string id = divNode.attribute("id").as_string();
	DivPtr pDiv = CDiv::build(id);

	std::string cls = divNode.attribute("class").as_string();
	std::string class_ = "." + cls;

	std::string selector = divNode.attribute("selector").as_string();

	if (value.isMember(class_.c_str()))
	{
		auto obj = value[class_];
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
	if (value.isMember(id_.c_str()))
	{
		auto obj = value[id_];
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

	for (auto& childNode : divNode.children("div"))
	{
		DivPtr pChild;
		std::string s = "";
		std::tie(s, pChild) = parseDivNode(childNode, value);
		if (pChild)
		{
			pDiv->addChild(pChild);
		}
	}
	return std::make_tuple(selector, pDiv);
}

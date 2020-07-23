#pragma once

#include <map>
#include "ParseAttribute.h"
#include "Div.h"

class ComponentsCollection
{
private:
	ComponentsCollection(){}
public:
	static ComponentsCollection& Instance();
	void parseComponents(std::string path);
	std::tuple<std::string, DivPtr> parseComponent(std::string path);
	DivPtr getComponent(std::string selector);
	
private:
	std::tuple<std::string, DivPtr> parseDivNode(pugi::xml_node& divNode, Json::Value& value);
	std::map<std::string, DivPtr> m_collections;
	ParseAttribute m_parseAttr;
};


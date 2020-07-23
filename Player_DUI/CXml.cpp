#include "pch.h"

#include <any>

#include "CXml.h"
#include "CTools.h"

namespace Corona
{
	CXml::CXml()
	{

	}

	CXml::~CXml()
	{

	}

	bool CXml::load(std::wstring fileName)
	{
		pugi::xml_parse_result result = m_doc.load_file(fileName.c_str());
		if (!result)
			return false;
		return true;
	}

	Corona::CDomNode CXml::child(std::string name)
	{
		pugi::xml_node node = m_doc.child(name.c_str());
		CDomNode result;
		result.node = node;
		return result;
	}

	std::string CDomNode::attribute(std::string name)
	{
		return node.attribute(name.c_str()).as_string();
	}

}



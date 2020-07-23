#pragma once

#include <iostream>
#include "pugixml/pugixml.hpp"

namespace Corona
{
	class CAttribute
	{
	public:

	private:
	};
	class CDomNode
	{
		friend class CXml;
	public:
		std::string attribute(std::string name);
	private:
		pugi::xml_node node;
	};
	class CXml
	{
	public:
		CXml();
		virtual ~CXml();
		virtual bool load(std::wstring fileName);
		virtual CDomNode child(std::string name);
	private:
		pugi::xml_document m_doc;
	};
}



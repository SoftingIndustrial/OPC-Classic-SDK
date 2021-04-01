#ifndef _XMLDOCUMENT_H_
#define _XMLDOCUMENT_H_

#include "msxml6.h"

#include <atlbase.h>


class Commands
{
public:
	Commands(
		const tstring& aName,
		const tstring& aDescription,
		const tstring& anAddress)
	{
		m_name = aName;
		m_address = anAddress;
		m_description = aDescription;
	}

	const tstring& getName(
		void)
	{
		return m_name;
	}

	const tstring& getAddress(
		void)
	{
		return m_address;
	}

	const tstring& getDescription(
		void)
	{
		return m_description;
	}

private:
	tstring m_name;
	tstring m_address;
	tstring m_description;
};

class XmlDocument
{
public:
	XmlDocument(
		void)
	{
		//init
		TESTHR(CoInitialize(NULL));
		CoCreateInstance(__uuidof(DOMDocument30), NULL,
						 CLSCTX_INPROC_SERVER, __uuidof(IXMLDOMDocument),
						 (void**)&m_docPtr);
	}

	virtual ~XmlDocument(
		void)
	{
		CoUninitialize();
	}

	bool load(
		tstring xmlText)
	{
		VARIANT_BOOL varOut((bool)TRUE);
		Variant varXml(xmlText.c_str());
		BSTR bstrXmlText = varXml.bstrVal;
		HRESULT hr = m_docPtr->loadXML(bstrXmlText, &varOut);
		return (varOut == VARIANT_TRUE);
	}

	void getCommands(
		std::vector<Commands>& commandList)
	{
		USES_CONVERSION;
		IXMLDOMElement* pXMLDocElement = NULL;
		m_docPtr->get_documentElement(&pXMLDocElement);
		IXMLDOMNodeList* pXMLDomNodeList = NULL;
		Variant commandName(tstring(_T("//nsPV:Command")).c_str());
		BSTR command = commandName.bstrVal;
		pXMLDocElement->selectNodes(command, &pXMLDomNodeList);
		long count = 0;
		pXMLDomNodeList->get_length(&count);

		for (int i = 0; i < count; i++)
		{
			IXMLDOMNode* pNode = NULL;
			pXMLDomNodeList->get_item(i, &pNode);
			IXMLDOMNamedNodeMap* attributeMap = NULL;
			pNode->get_attributes(&attributeMap);
			IXMLDOMNode* attribute;
			Variant commandName(_T("Name"));
			attributeMap->getNamedItem(commandName.bstrVal, &attribute);
			BSTR text;
			attribute->get_text(&text);
			tstring aName = (LPTSTR)text;
			attribute = NULL;
			Variant commandDescription(_T("Description"));
			attributeMap->getNamedItem(commandDescription.bstrVal, &attribute);
			attribute->get_text(&text);
			tstring aDescription = (LPTSTR)text;
			attribute = NULL;
			Variant commandAddress(_T("Address"));
			HRESULT hr = attributeMap->getNamedItem(commandAddress.bstrVal, &attribute);
			attribute->get_text(&text);
			tstring anAddress = (LPTSTR)text;

			if (FAILED(hr) || anAddress.empty() == true)
			{
				commandAddress.setBSTR(_T("Adresse"));
				attributeMap->getNamedItem(commandAddress.bstrVal, &attribute);
				attribute->get_text(&text);
				anAddress = (LPTSTR)text;
			} // end if .. else

			Commands command(aName, aDescription, anAddress);
			commandList.push_back(command);
		}
	}

	void insertDeclaration(
		void)
	{
		IXMLDOMProcessingInstruction* spXMLDecl;
		BSTR xmlSection = Variant(_T("xml")).bstrVal;
		BSTR xmlVersion = Variant(_T("version=\"1.0\" encoding=\"UTF-8\"")).bstrVal;
		m_docPtr->createProcessingInstruction(xmlSection, xmlVersion, &spXMLDecl);
		IXMLDOMNode* outChild;
		m_docPtr->appendChild(spXMLDecl, &outChild);
	} // end insertDeclaration

	void appendRoot(
		const tstring& rootName,
		const tstring& namespaceName)
	{
		m_docPtr->createElement(Variant(rootName.c_str()).bstrVal, &m_rootElement);
		IXMLDOMNode* outChild;
		m_docPtr->appendChild(m_rootElement, &outChild);
		m_rootElement->setAttribute(Variant(_T("xmlns:xsi")).bstrVal, Variant(namespaceName.c_str()));
	} // end appendChild

	void createNode(
		const tstring& aNodeName,
		const tstring& nameAttribute,
		const tstring& descriptionAttribute,
		const tstring& addressAttribute,
		const tstring& nameSpaceName)
	{
		IXMLDOMElement* element;
		m_docPtr->createElement(Variant(aNodeName.c_str()).bstrVal, &element);
		IXMLDOMNode* outElement;
		m_rootElement->appendChild(element, &outElement);
		// Add the Attribute1 attribute
		element->setAttribute(Variant(_T("Name")).bstrVal, Variant(nameAttribute.c_str()));
		element->setAttribute(Variant(_T("Description")).bstrVal, Variant(descriptionAttribute.c_str()));
		element->setAttribute(Variant(_T("Address")).bstrVal, Variant(addressAttribute.c_str()));
		element->setAttribute(Variant(_T("xmlns:xsi")).bstrVal, Variant(nameSpaceName.c_str()));
		BSTR bstrText;
		m_rootElement->get_xml(&bstrText);
		USES_CONVERSION;
		m_text = (LPTSTR)bstrText;
	}

	const tstring& innerXml(
		void)
	{
		return m_text;
	} // end innnerXml

	inline void TESTHR(
		HRESULT _hr)
	{
		//if FAILED(_hr) throw(_hr);
	}

private:
	IXMLDOMDocument* m_docPtr;
	IXMLDOMElement* m_rootElement;
	tstring m_text;
};

#endif

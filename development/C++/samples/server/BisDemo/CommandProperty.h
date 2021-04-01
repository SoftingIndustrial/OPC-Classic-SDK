#ifndef _COMMANDPROPERTY_H_
#define _COMMANDPROPERTY_H_

#include "DataPointTypeProperty.h"

#include "XmlDocument.h"

using namespace SoftingOPCToolboxServer;

static tstring XML_CMD_NAMESPACE = _T("file:///S3K/Proxyverwalter");


//-----------------------------------------------------------------------------
// CommandProperty
//-----------------------------------------------------------------------------
class CommandProperty : public DataPointTypeProperty
{
public:
	CommandProperty(
		int aDataPointId,
		tstring& aName):
		DataPointTypeProperty(aDataPointId)
	{
		setId(aDataPointId);
		tstringstream stream;
		stream << aDataPointId;
		setItemId(stream.str());
		setName(aName);
		setDescription(aName);
		setDatatype(VT_BSTR);
		setAccessRights(EnumAccessRights_READABLE);
		m_xmlDocument = new XmlDocument();
		m_xmlDocument->insertDeclaration();
		m_xmlDocument->appendRoot(tstring(_T("nsPV:Commands")), XML_CMD_NAMESPACE);
	} // end ctr

	virtual ~CommandProperty(
		void)
	{
	} // end dtor

	virtual void handleReadRequest(
		DaRequest& aRequest)
	{
		_ASSERT(aRequest.getPropertyId() == getId());
		aRequest.setResult(EnumResultCode_S_OK);
		DateTime time;
		time.now();
		ValueQT aValue(Variant(m_xmlDocument->innerXml().c_str()), EnumQuality_GOOD, time);
		aRequest.setValue(aValue);
		aRequest.complete();
	}   //  end HandleReadRequest

	void addCommand(
		tstring& aName,
		tstring& aDescription,
		tstring& anAddress)
	{
		// TODO: add command here
		m_xmlDocument->createNode(_T("nsPV:Command"), aName, aDescription, anAddress, XML_CMD_NAMESPACE);
	} // end AddCommand

	void addCommand(
		tstring& aName,
		tstring& aDescription)
	{
		addCommand(aName, aDescription, tstring());
	} // end AddCommand

private:
	XmlDocument* m_xmlDocument;
};  //  end CommandProperty

#endif

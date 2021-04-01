#ifndef _SODASPROPERTY_H_
#define _SODASPROPERTY_H_

#include <opcda.h>
#include "SOCmnElement.h"
#include "SODaSItemTag.h"

#pragma pack(push, 4)


class SODaSTag;

//-----------------------------------------------------------------------------
// SODaSPropertyData                                                          |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSPropertyData :
	public SOCmnData
{
public:
	SODaSPropertyData();
	const SODaSPropertyData& operator=(IN SODaSPropertyData& propS);

	// data of property
	DWORD m_pid;          // property id
	SOCmnString m_itemID;   // item id
	SOCmnString m_descr;    // description
	HRESULT m_result;     // result
	VARTYPE m_datatype;     // data type
	SOCmnVariant m_value; // value
	SOCmnString m_name;   // name
	WORD m_accessRights;  // access rights
}; // SODaSPropertyData




//-----------------------------------------------------------------------------
// SODaSProperty                                                              |
//-----------------------------------------------------------------------------

class SODaSTag;

class SODAS_EXPORT SODaSProperty :
	public SODaSItemTag,
	virtual public SOCmnElementNameIntString
{
public:
	SODaSProperty(void);
	SODaSProperty(IN DWORD propertyId, IN LPCTSTR name = NULL);

	// object identity
	virtual BOOL isUniqueName(void) const;

	// watch overload
	virtual BOOL setParentBranch(IN SOCmnElement* parent);

	// property data
	void getPropertyData(OUT SODaSPropertyData* propData);

	// item tags item id
	virtual SOCmnString getItemID(void);

	// property id
	BOOL setPropertyId(IN DWORD propertyId);
	DWORD getPropertyId(void);

	// description
	virtual BOOL setDescription(IN LPCTSTR descr);
	SOCmnString getDescription(void) const;

	// request handling
	virtual void handleRequest(IN SODaSRequest* request);
	virtual BYTE getDeviceIOMode(IN SODaSRequest* req);

	// special release method
	virtual LONG release(void);

	// standard properties
	static BOOL isStandardTagProperty(IN DWORD propId);
	static BOOL isStandardNodeProperty(IN DWORD propId);
	static void getStandardTagProperties(IN VARTYPE tagNativeDatatype, IN BOOL isSOAP,
										 OUT SOCmnList<SODaSPropertyData> &propertyList);
	static void getStandardNodeProperties(IN BOOL isSOAP, OUT SOCmnList<SODaSPropertyData> &propertyList);
	static DWORD getStandardPropertyIDByName(IN LPCTSTR propName);
	static LPCTSTR getStandardPropertyNameByID(IN DWORD propID, OUT SOCmnString& propItemID);

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

protected:
	SOCmnString m_description;

	virtual ~SODaSProperty();

	virtual void onSetDescription(IN LPCTSTR descr);

	// handle standard properties
	BOOL handleStandardProperties(IN SODaSRequest* request);

	virtual SOCmnObject* getObjectCmnElementIName(void);
}; // SODaSProperty

inline DWORD SODaSProperty::getPropertyId(void)
{
	return getNameInt();
}

inline SOCmnString SODaSProperty::getDescription(void) const
{
	return m_description;
}

#define GenericTagProperty SODaSProperty

#pragma pack(pop)
#endif

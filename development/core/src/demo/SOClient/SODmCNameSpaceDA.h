#ifndef _SODMCNAMESPACEDA_H_
#define _SODMCNAMESPACEDA_H_

#include "SODmCPropFrm.h"

#ifdef SODMC_DA

//-----------------------------------------------------------------------------
// SODmCPPTagWrite                                                           |
//-----------------------------------------------------------------------------

class SODmCPPTagWrite : public SODmCPPage
{
	DECLARE_DYNAMIC(SODmCPPTagWrite)
public:
	SODmCPPTagWrite(IN SODmCPSheet* sheet);
	~SODmCPPTagWrite();

	virtual void onOK(void);                // specific onOK handler
	virtual BOOL loadAttributes(void);

	void setObject(IN SOCltBrowseObject* obj)
	{
		m_obj = obj;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// SODmCPPage virtuals
	virtual void enableControls(BOOL enable);

	SOCltBrowseObject* m_obj;   // node object
	VARTYPE m_datatype;
	DWORD m_accessRights;

	// read values
	CString m_readValue;
	CString m_readQuality;
	CString m_readTimestamp;
	// write values
	CString m_writeValue;

	// controls
	SODmCPEdit m_edReadValue;
	SODmCPEdit m_edReadQuality;
	SODmCPEdit m_edReadTimestamp;
	SODmCPEdit m_edWriteValue;

	DECLARE_MESSAGE_MAP()
}; // SODmCPPTagWrite


//-----------------------------------------------------------------------------
// SODmCPPProperties                                                          |
//-----------------------------------------------------------------------------

class SODmCPPProperties  : public SODmCPPage
{
	DECLARE_DYNAMIC(SODmCPPProperties)
public:
	SODmCPPProperties(IN SODmCPSheet* sheet);
	~SODmCPPProperties();

	void setObject(IN SOCltBrowseObject* node); // set node object

	virtual void onOK(void);    // specific onOK handler

protected:
	virtual BOOL OnInitDialog(void);                    // initialize page
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// SODmCPPage virtuals
	virtual void enableControls(BOOL enable);
	virtual BOOL loadAttributes(void);
	virtual void clearAttributes(void);

	void getProperties(void);   // get properties from server

	SODmCPListCtrl m_liProperties;

	SOCltBrowseObject* m_obj;   // node object

	DECLARE_MESSAGE_MAP()
}; //SODmCPPProperties

inline void SODmCPPProperties::setObject(IN SOCltBrowseObject* node)
{
	m_obj = node;
}


//-----------------------------------------------------------------------------
// SODmCPPProperty                                                            |
//-----------------------------------------------------------------------------

class SODmCPPProperty  : public SODmCPPage
{
	DECLARE_DYNAMIC(SODmCPPProperty)
public:
	SODmCPPProperty(IN SODmCPSheet* sheet);
	~SODmCPPProperty();

	void setObject(IN SODaCProperty* property); // set property object

	virtual void onOK(void);    // specific onOK handler
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// PPage virtuals
	virtual void enableControls(BOOL enable);
	virtual BOOL loadAttributes(void);
	virtual void clearAttributes(void);

	void getProperty(void); // get properties from server

	// current attributes
	CString m_itemID;
	DWORD m_propID;
	CString m_propName;
	CString m_description;
	CString m_value;
	CString m_datatype;

	// controls
	SODmCPEdit m_edItemID;
	SODmCPEdit m_edPropID;
	SODmCPEdit m_edDescription;
	SODmCPEdit m_edValue;
	SODmCPEdit m_edDatatype;
	SODmCPEdit m_edPropName;

	SODaCProperty* m_obj;   // property object

	DECLARE_MESSAGE_MAP()
}; //SODmCPPProperty

inline void SODmCPPProperty::setObject(IN SODaCProperty* prop)
{
	m_obj = prop;
}

#endif

#endif

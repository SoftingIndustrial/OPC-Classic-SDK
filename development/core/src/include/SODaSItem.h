#ifndef _SODASITEM_H_
#define _SODASITEM_H_

#pragma pack(push, 4)


#include <opcda.h>
#include "SOCmnElement.h"
#include "SODaSItemTag.h"
#include "SODaSRequest.h"
#include "SODaSGroup.h"

//-----------------------------------------------------------------------------
// SODaSItemData                                                              |
//-----------------------------------------------------------------------------
class SODAS_EXPORT SODaSItemData : public SOCmnData
{
public:
	SODaSItemData(void)
	{
		m_reqDatatype = VT_EMPTY;
		m_nativeDatatype = VT_EMPTY;
		m_accessRights = 0;
		m_result = E_FAIL;
		m_userData = 0;
	}
	SODaSItemData(SODaSItemData* src)
	{
		m_itemID = src->m_itemID;
		m_itemPath = src->m_itemPath;
		m_reqDatatype = src->m_reqDatatype;
		m_nativeDatatype = src->m_nativeDatatype;
		m_accessRights = src->m_accessRights;
		m_result = src->m_result;
		m_userData = src->m_userData;
	}

	SOCmnString m_itemID;     // item id
	VARTYPE m_reqDatatype;    // requested data type
	SOCmnString m_itemPath;   // item path

	VARTYPE m_nativeDatatype; // native data type
	DWORD m_accessRights;     // access rights

	HRESULT m_result;
	DWORD m_userData;
};

//-----------------------------------------------------------------------------
// SODaSItemICyclic                                                           |
//-----------------------------------------------------------------------------
class SODaSXmlServer;

class SODAS_EXPORT SODaSItemICyclic
{
	friend class SODaSTransaction;
	friend class SODaSXmlServer;
protected:
	SODaSItemICyclic();
	virtual ~SODaSItemICyclic();

	virtual void createCyclicData(void);
	virtual void destroyCyclicData(void);

	// last value send to the OPC client
	virtual BOOL setSendValue(IN size_t connection, IN SOCmnVariant& value, IN WORD quality);

public:
	virtual BOOL getSendValue(IN size_t connection, OUT SOCmnVariant& value, OUT WORD* quality);
}; // SODaSItemICyclic


//-----------------------------------------------------------------------------
// SODaSItemCyclicFull                                                        |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSItemCyclicFull : virtual public SODaSItemICyclic
{
protected:
	SOCmnList<SOCmnVariant> m_sendValues;   // previous values send to the OPC clients
	SOCmnDWORDList m_sendQualities;         // previous qualities send to the OPC client

	SODaSItemCyclicFull();
	virtual ~SODaSItemCyclicFull();

	virtual void createCyclicData(void);
	virtual void destroyCyclicData(void);

	// last value send to the OPC client
	virtual BOOL setSendValue(IN size_t connection, IN SOCmnVariant& value, IN WORD quality);

public:
	virtual BOOL getSendValue(IN size_t connection, OUT SOCmnVariant& value, OUT WORD* quality);
}; // SODaSItemCyclicFull


//-----------------------------------------------------------------------------
// SODaSItemCyclicOne                                                         |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSItemCyclicOne : virtual public SODaSItemICyclic
{
protected:
	WORD m_sendQuality;         // previous quality send to the OPC client
	SOCmnVariant m_sendValue;   // previous value send to the OPC clients

	SODaSItemCyclicOne();

	virtual void destroyCyclicData(void);

	// last value send to the OPC client
	virtual BOOL setSendValue(IN size_t connection, IN SOCmnVariant& value, IN WORD quality);

public:
	virtual BOOL getSendValue(IN size_t connection, OUT SOCmnVariant& value, OUT WORD* quality);
}; // SODaSItemCyclicOne


//-----------------------------------------------------------------------------
// SODaSItemIAttr                                                             |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSItemIAttr
{
	friend class SODaSItem;
protected:
	SODaSItemIAttr();
	virtual ~SODaSItemIAttr();

	virtual void onSetDeadBand(IN FLOAT deadband);

	virtual SOCmnObject* getObjectDaSItemIAttr(void);

public:
	// item deadband
	virtual HRESULT setDeadBand(IN FLOAT deadband);
	virtual HRESULT getDeadBand(OUT FLOAT& deadband);
	virtual HRESULT clearDeadBand(void);

	// client handle from OPC client
	virtual BOOL setXMLClientHandle(IN LPCTSTR clientHandle);
	virtual BOOL getXMLClientHandle(OUT SOCmnStringEx& clientHandle);
}; // SODaSItemIAttr


//-----------------------------------------------------------------------------
// SODaSItemAttrFull                                                          |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSItemAttrFull : virtual public SODaSItemIAttr
{
protected:
	SODaSItemAttrFull();
	FLOAT m_deadband;         // item deadband
#ifdef SOFEATURE_SOAP
	SOCmnString m_clientHandle;
#endif

public:
	// item deadband
	virtual HRESULT setDeadBand(IN FLOAT deadband);
	virtual HRESULT getDeadBand(OUT FLOAT& deadband);
	virtual HRESULT clearDeadBand(void);
#ifdef SOFEATURE_SOAP
	virtual BOOL setXMLClientHandle(IN LPCTSTR clientHandle);
	virtual BOOL getXMLClientHandle(OUT SOCmnStringEx& clientHandle);
#endif
}; // SODaSItemAttrFull



//-----------------------------------------------------------------------------
// SODaSItem                                                                  |
//-----------------------------------------------------------------------------

#define SODASITEM_NO_DEADBAND -1.0

class SODAS_EXPORT SODaSItem :
	public SOCmnElement,
	virtual public SODaSItemICyclic,
	virtual public SODaSItemIAttr
{
	friend class SODaSRequest;
	friend class SODaSGroup;
	friend class SODaSServer;
	friend class SODaSTransaction;

public:
	SODaSItem();

	// watch overload
	virtual BOOL setParentBranch(IN SOCmnElement* parent);

	// validate name
	virtual HRESULT validateName(IN LPCTSTR name, IN OPTIONAL BOOL checkIfUnique = TRUE);

	// cache object
	SODaSCache* getCache(void);

	// server object
	SODaSServer* getServer(void);

	// item tag object
	virtual BOOL setItemTag(IN SODaSItemTag* tag, IN BOOL goingToActivate);
	SODaSItemTag* getItemTag(void);
	virtual SOCmnString getItemID(void);

	// activation state
	virtual BOOL setActive(IN BOOL active);
	BOOL getActive(void) const;
	BOOL isActive(void);

	// client handle from OPC client
	virtual BOOL setClientHandle(IN OPCHANDLE clientHandle);
	OPCHANDLE getClientHandle(void) const;

	// requested datatype
	virtual HRESULT setReqDatatype(IN VARTYPE reqDatatype);
	VARTYPE getReqDatatype(void) const;

	// access path
	virtual BOOL setAccessPath(IN LPCTSTR accessPath);
	virtual SOCmnString getAccessPath(void);

	// server handle
	OPCHANDLE getServerHandle(void) const;

	// object trace id
	virtual LPCTSTR getObjTraceId(void);

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

	// cyclic request
	virtual SODaSRequest* getCyclicRequest(void);
	virtual void setCyclicRequest(IN SODaSRequest* pReq);

protected:
	virtual ~SODaSItem();

	SODaSItemTag* m_itemTag;  // tag object
	OPCHANDLE m_clientHandle; // client handle
	VARTYPE m_reqDatatype;    // requested data type
	BYTE m_active;            // activation state

	// callbacks
	virtual void onRead(IN BOOL deviceRead);
	virtual void onWrite(void);
	virtual void onSetActive(IN BOOL active);
	virtual void onSetClientHandle(IN OPCHANDLE clientHandle);
	virtual void onSetReqDatatype(IN VARTYPE requestedType);
	virtual void onSetItemTag(IN SODaSItemTag* tag);

	virtual SOCmnObject* getObjectDaSItemIAttr(void);
}; // SODaSItem


inline BOOL SODaSItem::getActive(void) const
{
	return (BOOL)m_active;
}

inline BOOL SODaSItem::isActive(void)
{
	SOCmnPointer<SODaSGroup> grp = (SODaSGroup*)getParentBranch();

	if (grp.isNotNull())
	{
		return ((m_active) && (grp->getActive()));
	}
	else
	{
		return FALSE;
	}
}

inline OPCHANDLE SODaSItem::getClientHandle(void) const
{
	return m_clientHandle;
}

inline VARTYPE SODaSItem::getReqDatatype(void) const
{
	return m_reqDatatype;
}

inline OPCHANDLE SODaSItem::getServerHandle(void) const
{
	return (OPCHANDLE)this->getOpcHandle();
}

#define GenericItem SODaSItemDefault




//-----------------------------------------------------------------------------
// SODaSItemDefault                                                           |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSItemDefault :
	public SODaSItem,
	virtual public SODaSItemAttrFull,
#ifndef SOOS_WINDOWS_CE
	virtual public SODaSItemCyclicFull
#else
	virtual public SODaSItemCyclicOne
#endif
{
public:
	SODaSItemDefault(void);
}; // SODaSItemDefault

#pragma pack(pop)
#endif

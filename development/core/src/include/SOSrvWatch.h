#ifndef _SOSRVWATCH_H_
#define _SOSRVWATCH_H_

#ifdef SOFEATURE_WATCH

#include "SOCmnObject.h"
#include "SOProxyDef.h"
#include "SOSrvXML.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOSrvWatchHandler                                                          |
//-----------------------------------------------------------------------------

class SOSrvWatch;

class SOSRV_EXPORT SOSrvWatchHandler : public SOCmnObject
{
	friend class SOSrvWatch;
public:
	SOSrvWatchHandler(void);

protected:
	virtual SOCmnObject* getObject(IN LPCTSTR objSpace, IN LPCTSTR objType);
	virtual void getRootObjects(OUT SOCmnList<SOCmnObject> objList);

	// get statistics
	virtual HRESULT getStatistics(SOCmnStringEx& szXML);

	// DA only
	virtual BOOL hasCacheObject(IN SOCmnObject* tag, IN SOCmnObject* cache, OUT BOOL& uniqueCache);
	virtual BOOL setCollectCacheChanges(IN SOCmnObject* tag, IN BOOL doCollect);
	virtual SOCmnObject* getNameSpaceObjectByItemID(IN LPCTSTR itemID);
}; // SOSrvWatchHandler


//-----------------------------------------------------------------------------
// SOSrvWatchData                                                             |
//-----------------------------------------------------------------------------

class SOSRV_EXPORT SOSrvWatchData : public SOCmnData
{
	friend class SOSrvWatch;
public:
	SOSrvWatchData(void);
	virtual ~SOSrvWatchData(void);

protected:
	SOCmnPointer<SOCmnObject> m_object;
	DWORD m_clientHandle;
	SOCmnStringList m_elementList;
}; // SOSrvWatchData


//-----------------------------------------------------------------------------
// SOSrvWatch                                                                 |
//-----------------------------------------------------------------------------

class SOSrvWatchMethodData;

class SOSRV_EXPORT SOSrvWatch : public SOCmnObject
{
public:
	SOSrvWatch(void);

	virtual SOCmnObject* getObject(IN SOSrvWatchMethodData* pMD);

	// interface implementations
	virtual HRESULT getChildren(IN LPCTSTR parent, IN BOOL withAttributes, OUT SOCmnStringEx& children);
	virtual HRESULT collectChanges(IN LPCTSTR objects, IN DWORD doCollect);
	virtual HRESULT getChanges(OUT SOCmnStringEx& changes);
	virtual HRESULT getStatistics(SOCmnStringEx& statistics);
	virtual HRESULT getObject(IN LPCTSTR objectDescription, OUT SOCmnStringEx& object);

	SOSrvWatchHandler* getDAHandler(void)
	{
		m_DAHandler.addRef();
		return m_DAHandler;
	}
	void setDAHandler(IN SOSrvWatchHandler* handler)
	{
		m_DAHandler = handler;
		m_DAHandler.addRef();
	}
	SOSrvWatchHandler* getAEHandler(void)
	{
		m_AEHandler.addRef();
		return m_AEHandler;
	}
	void setAEHandler(IN SOSrvWatchHandler* handler)
	{
		m_AEHandler = handler;
		m_AEHandler.addRef();
	}
	SOSrvWatchHandler* getCustomHandler(void)
	{
		m_customHandler.addRef();
		return m_customHandler;
	}
	void setCustomHandler(IN SOSrvWatchHandler* handler)
	{
		m_customHandler = handler;
		m_customHandler.addRef();
	}

	SOCmnSync* getStatisticsLock(void)
	{
		return &m_getStatisticsLock;
	}

	static BOOL attributeChange(IN SOCmnObject* pObj, IN LPCTSTR attrName, IN LPCTSTR attrValue);

	virtual BOOL changeAttribute(IN SOCmnObject* pObj, IN LPCTSTR attrName, IN LPCTSTR attrValue);
	virtual BOOL addObject(IN SOCmnObject* pParent, IN SOCmnObject* pObj);
	virtual BOOL removeObject(IN SOCmnObject* pParent, IN SOCmnObject* pObj);


	static DWORD convertObjectType(LPCTSTR objectType);

	static BOOL createElementText(SOCmnStringEx& szXML, LPCTSTR element, LPCTSTR text);
	static BOOL createElementText(SOCmnStringEx& szXML, LPCTSTR element, DWORD dw);
	static BOOL createElementTextWithAttributes(SOCmnStringEx& szXML, LPCTSTR element, LPCTSTR text, LPCTSTR attr);
	static BOOL addElementText(SOCmnStringEx& szXML, LPCTSTR parentElement, LPCTSTR element, LPCTSTR text);

	static void addStartElementText(SOCmnStringEx& szXML, LPCTSTR element);
	static void addEndElementText(SOCmnStringEx& szXML, LPCTSTR element);

protected:
	SOCmnPointer<SOSrvWatchHandler> m_customHandler;
	SOCmnPointer<SOSrvWatchHandler> m_DAHandler;
	SOCmnPointer<SOSrvWatchHandler> m_AEHandler;
	SOCmnList<SOSrvWatchData> m_changesList;
	SOCmnStringEx m_addObjects;
	SOCmnStringEx m_removeObjects;
	SOCmnSync m_getStatisticsLock;
	SOCmnDateTime m_statTime;
}; // SOSrvWatch


class SOSRV_EXPORT SOSrvWatchMethodData : public SOCmnData
{
public:
	SOSrvWatchMethodData(void);
	virtual ~SOSrvWatchMethodData(void);

	DWORD m_clientHandle;
	DWORD m_serverHandle;
	SOCmnString m_objectType;
	SOCmnString m_objectSpace;
	SOCmnStringList m_attributes;
}; // SOSrvWatchMethodData

#define SOSRVWATCHMETHOD_STATE_SETTRACE      0x10
#define SOSRVWATCHMETHOD_STATE_TRACEOPTIONS  0x11
#define SOSRVWATCHMETHOD_STATE_TRACEOLEVEL   0x12
#define SOSRVWATCHMETHOD_STATE_TRACEOFILE    0x13
#define SOSRVWATCHMETHOD_STATE_TRACEOBUFFER  0x14

#define SOSRVWATCHMETHOD_STATE_GETOBJECT     0x20
#define SOSRVWATCHMETHOD_STATE_COLLECTCHANGES 0x30
#define SOSRVWATCHMETHOD_STATE_OBJECT        0x81
#define SOSRVWATCHMETHOD_STATE_OBJECT_ATTRIBUTES 0x82

#define SOSRVWATCHMETHODDATA_TYPE_NONE       0x0000
#define SOSRVWATCHMETHODDATA_TYPE_OBJ_SP     0x2000
#define SOSRVWATCHMETHODDATA_TYPE_OBJ_TY     0x2010
#define SOSRVWATCHMETHODDATA_TYPE_OBJ_SH     0x2020
#define SOSRVWATCHMETHODDATA_TYPE_OBJ_CH     0x2030
#define SOSRVWATCHMETHODDATA_TYPE_OBJ_ATS    0x2040
#define SOSRVWATCHMETHODDATA_TYPE_TRCLERR    0x1205
#define SOSRVWATCHMETHODDATA_TYPE_TRCLWRN    0x1206
#define SOSRVWATCHMETHODDATA_TYPE_TRCLINF    0x1207
#define SOSRVWATCHMETHODDATA_TYPE_TRCLDBG    0x1208
#define SOSRVWATCHMETHODDATA_TYPE_TRCFENA    0x1301
#define SOSRVWATCHMETHODDATA_TYPE_TRCFNAM    0x1302
#define SOSRVWATCHMETHODDATA_TYPE_TRCFNA2    0x1303
#define SOSRVWATCHMETHODDATA_TYPE_TRCFSIZ    0x1304
#define SOSRVWATCHMETHODDATA_TYPE_TRCBENA    0x1401
#define SOSRVWATCHMETHODDATA_TYPE_TRCBSIZ    0x1404
#define SOSRVWATCHMETHODDATA_TYPE_TRCENABLE  0x0001
#define SOSRVWATCHMETHODDATA_TYPE_TRCSIZE    0x0004

class SOSRV_EXPORT SOSrvWatchMethod : public SOSrvXmlMethod
{
public:
	SOSrvWatchMethod(void);

	virtual void handleStartElement(LPCSTR name, LPCSTR* atts);
	virtual void handleEndElement(LPCSTR name);
	virtual void handleCharacterData(LPCSTR s, INT len);

	SOCmnStringList* getMethodElements(void)
	{
		return &m_methodElements;
	}
	SOCmnList<SOSrvWatchMethodData> *getMethodData(void)
	{
		return &m_methodData;
	}

protected:
	SOCmnStringList m_methodElements;
	SOCmnList<SOSrvWatchMethodData> m_methodData;
	SOSrvWatchMethodData* m_pCurMethodData;
	WORD m_charDataType;
	SOCmnString m_charDataElementName;

	~SOSrvWatchMethod(void);
}; // SOSrvWatchMethod

#pragma pack(pop)

#endif // SOFEATURE_WATCH

#endif

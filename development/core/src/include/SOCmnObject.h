#ifndef _SOCMNOBJECT_H_
#define _SOCMNOBJECT_H_

#include "SOCmnCfg.h"
#include "SOCmnOsCompat.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// SOCmnObject                                                                |
//-----------------------------------------------------------------------------

// object types
#define SOCMNOBJECT_TYPE_ENTRY       (0x00010000)
#define SOCMNOBJECT_TYPE_CALLBACK    (0x00020000)
#define SOCMNOBJECT_TYPE_REQUEST     (0x00040000)
#define SOCMNOBJECT_TYPE_CACHE       (0x00080000)
#define SOCMNOBJECT_TYPE_TRANSACTION (0x00100000)
#define SOCMNOBJECT_TYPE_REFRESH     (0x00200000)
#define SOCMNOBJECT_TYPE_EVENT       (0x04000000)
#define SOCMNOBJECT_TYPE_OTHER       (0x08000000)

#define SOCMNOBJECT_TYPE_ELEMENT     (0x80000000)

#define SOCMNOBJECT_TYPE_LEAF               (SOCMNOBJECT_TYPE_ELEMENT | 0x1000)
#define SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT (SOCMNOBJECT_TYPE_ELEMENT | 0x2000)
#define SOCMNOBJECT_TYPE_BRANCH             (SOCMNOBJECT_TYPE_ELEMENT | 0x4000)
#define SOCMNOBJECT_TYPE_ROOT               (SOCMNOBJECT_TYPE_ELEMENT | 0x8000)
#define SOCMNOBJECT_TYPE_ITEM_TAG           (SOCMNOBJECT_TYPE_ELEMENT | 0x0800)
#define SOCMNOBJECT_TYPE_SERVER             (SOCMNOBJECT_TYPE_BRANCH  | 0x0400)
#define SOCMNOBJECT_TYPE_SERVER_DA          (SOCMNOBJECT_TYPE_BRANCH  | 0x0401)
#define SOCMNOBJECT_TYPE_SERVER_AE          (SOCMNOBJECT_TYPE_BRANCH  | 0x0500)
#define SOCMNOBJECT_TYPE_GROUP              (SOCMNOBJECT_TYPE_BRANCH  | 0x0002)
#define SOCMNOBJECT_TYPE_ITEM               (SOCMNOBJECT_TYPE_LEAF    | 0x0001)
#define SOCMNOBJECT_TYPE_NODE               (SOCMNOBJECT_TYPE_BRANCH  | SOCMNOBJECT_TYPE_ITEM_TAG | 0x0004)
#define SOCMNOBJECT_TYPE_ENTRY_DAC          (SOCMNOBJECT_TYPE_BRANCH  | 0x0100 | SOCMNOBJECT_TYPE_ROOT | SOCMNOBJECT_TYPE_ENTRY)
#define SOCMNOBJECT_TYPE_TAG                (SOCMNOBJECT_TYPE_LEAF    | SOCMNOBJECT_TYPE_ITEM_TAG | 0x0002 | SOCMNOBJECT_TYPE_ROOT)
#define SOCMNOBJECT_TYPE_PROPERTY           (SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT | SOCMNOBJECT_TYPE_ITEM_TAG | 0x0001)
#define SOCMNOBJECT_TYPE_SUBSCRIPTION       (SOCMNOBJECT_TYPE_LEAF    | 0x0004)
#define SOCMNOBJECT_TYPE_CATEGORY           (SOCMNOBJECT_TYPE_BRANCH  | 0x0008)
#define SOCMNOBJECT_TYPE_ATTRIBUTE          (SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT | 0x0002)
#define SOCMNOBJECT_TYPE_ATTRIBUTE_VALUE    (SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT | 0x0010)
#define SOCMNOBJECT_TYPE_CONDITION_CLASS    (SOCMNOBJECT_TYPE_BRANCH  | 0x0010)
#define SOCMNOBJECT_TYPE_CONDITION          (SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT | 0x0004)
#define SOCMNOBJECT_TYPE_SUBCONDITION_CLASS (SOCMNOBJECT_TYPE_LEAF    | 0x0008)
#define SOCMNOBJECT_TYPE_SOURCE             (SOCMNOBJECT_TYPE_LEAF    | 0x0020)
#define SOCMNOBJECT_TYPE_AREA               (SOCMNOBJECT_TYPE_BRANCH  | 0x0080)
#define SOCMNOBJECT_TYPE_AREA_BROWSER       (SOCMNOBJECT_TYPE_ADDITIONAL_ELEMENT | SOCMNOBJECT_TYPE_ROOT | 0x0008)
#define SOCMNOBJECT_TYPE_EVENT_BROWSER      (SOCMNOBJECT_TYPE_ELEMENT | SOCMNOBJECT_TYPE_OTHER | 0x0001)
#define SOCMNOBJECT_TYPE_NAMESPACE_BROWSER  (SOCMNOBJECT_TYPE_ELEMENT | SOCMNOBJECT_TYPE_OTHER | SOCMNOBJECT_TYPE_ROOT | 0x0002)

#define SOCMNOBJECT_TYPE_ALL                    0xFFFFFFFF

// object key types
#define SOCMNOBJECT_KEYTYPE_DWORD  0x1
#define SOCMNOBJECT_KEYTYPE_STRING 0x2

// object flags
#define SOCMNOBJECT_FLAG_COLLECT_CHANGES            0x01
#define SOCMNOBJECT_FLAG_COLLECT_ADDITIONAL_CHANGES 0x02
#define SOCMNOBJECT_FLAG_COLLECT_MASK               0x03


class SOCMN_EXPORT SOCmnObject
{
public:
	SOCmnObject(IN DWORD objType = SOCMNOBJECT_TYPE_OTHER);

	DWORD getOpcHandle(void) const;

	// reference counting
	virtual LONG addRef(void);
	virtual LONG release(void);
	LONG getRefCount(void) const;

	// object identity
	virtual BOOL getKey(OUT SOCmnString& key);
	virtual BOOL getKey(OUT DWORD& key);
	virtual BOOL hasKey(IN LPCTSTR key);
	virtual BOOL hasKey(IN DWORD key);
	virtual BOOL hasKeyType(IN DWORD keyType);

	// object type
	DWORD getObjectType(void) const;
	BOOL is(DWORD objectType) const;
	virtual SOCmnString getObjTypeString(void);

	// object trace id strings
	virtual BOOL setObjTraceId(IN LPCTSTR traceId);
	virtual BOOL isInitObjTraceId(void);
	virtual LPCTSTR getObjTraceId(void);

	// object state
	virtual BYTE getObjectState(void) const;
	virtual BOOL setObjectState(IN BYTE state);
	virtual BYTE getTargetObjectState(void) const;
	virtual BOOL setTargetObjectState(IN BYTE state);

	// application specific data
	virtual BYTE getUserData(void) const;
	virtual void setUserData(IN BYTE userData);

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
	virtual BYTE getCollectChanges(void) const;
	virtual BOOL setCollectChanges(IN BYTE doCollect);
#endif // SOFEATURE_WATCH

protected:
	virtual ~SOCmnObject(void);

	// object state callbacks
	virtual void onSetObjectState(void);
	virtual void onSetTargetObjectState(void);

	virtual LPCTSTR getActObjTraceIdString(void);

	DWORD m_opcHandle;           // OPC Handle assigned by OPC Handle Manager
	LONG m_refCount;             // reference count
	DWORD m_objectType;          // object type
#ifdef SOFEATURE_CMN_TRACE
	SOCmnString m_objectTraceId; // object trace id string
#endif
	BYTE m_objectState;          // object state
	BYTE m_objectStateTarget;    // target state of the object
	BYTE m_userData;             // application specific data
	BYTE m_flags;                // flags (e.g. collect changes)

}; // SOCmnObject


inline LONG SOCmnObject::getRefCount(void) const
{
	return m_refCount;
}

inline DWORD SOCmnObject::getObjectType(void) const
{
	return m_objectType;
}

inline BOOL SOCmnObject::is(DWORD objectType) const
{
	return ((m_objectType & objectType) == objectType);
}

#define GenericObject SOCmnObject




//-----------------------------------------------------------------------------
// SOCmnPointer                                                               |
//-----------------------------------------------------------------------------

#define GenericPointer SOCmnPointer

template <class T> class SOCmnPointer
{
public:
	// constructors
	SOCmnPointer(void)
	{
		p = NULL;
	}
	SOCmnPointer(IN T* p_)
	{
		p = p_;
	}
	SOCmnPointer(IN const SOCmnPointer<T>& p_)
	{
		if ((p = p_.p) != NULL)
		{
			p->addRef();
		}
	}

	// destructor
	~SOCmnPointer(void)
	{
		if (p)
		{
			p->release();
		}
	}

	// reference count methods
	LONG release(void)
	{
		LONG ret = -1;
		T* p_ = p;
		p = NULL;

		if (p_)
		{
			ret = p_->release();
		}

		return ret;
	}
	void addRef(void)
	{
		if (p)
		{
			p->addRef();
		}
	}

	// operators
	operator T* (void)
	{
		return (T*)p;
	}
	operator T** (void)
	{
		return (T**)&p;
	}
	T& operator*(void)
	{
		return *p;
	}
	T** operator&()
	{
		return &p;
	}
	T* operator->()
	{
		return p;
	}
	SOCmnPointer& operator=(IN T* p_)
	{
		if (p)
		{
			p->release();
		}

		p = p_;
		return *this;
	}
	SOCmnPointer& operator=(IN const SOCmnPointer<T>& ptr)
	{
		// guard against self-assignemnt
		if (&p == &ptr.p) // this==&ptr evaluates this==ptr.operator&()
		{
			return *this;
		}

		if (p)
		{
			p->release();
		}

		p = ptr.p;

		if (p)
		{
			p->addRef();
		}

		return *this;
	}
	bool operator!() const
	{
		return (p == NULL);
	}
	BOOL isNotNull() const
	{
		return (p != NULL);
	}

	void setWithAddRef(IN T* p_)
	{
		if (p)
		{
			p->release();
		}

		p = p_;

		if (p)
		{
			p->addRef();
		}
	}

	void set(IN T* p_)
	{
		if (p)
		{
			p->release();
		}

		p = p_;
	}
private:
	T* p;
}; // SOCmnPointer

#pragma pack(pop)
#endif

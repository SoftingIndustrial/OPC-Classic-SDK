#ifndef _SODASCACHE_H_
#define _SODASCACHE_H_

#include <opcda.h>
#include "SOCmnObject.h"
#include "SODaSItemTag.h"

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SODaSCache                                                                 |
//-----------------------------------------------------------------------------

#define SODASCACHE_NOTIFYWATCH_VALUE      0x01
#define SODASCACHE_NOTIFYWATCH_QUALITY    0x02
#define SODASCACHE_NOTIFYWATCH_TIMESTAMP  0x04

class SODaSTag;
class SODaSRequest;

class SODAS_EXPORT SODaSCache : public SOCmnObject
{
	friend class SODaSItemTag;
	friend class SODaSTag;

public:
	SODaSCache(void);

	void init(void);    // initialize

	// combined setting of the cache values
	virtual BOOL get(OUT SOCmnVariant& value, OUT WORD* quality, OUT SOCmnDateTime* timeStamp,
					 OPTIONAL IN SODaSRequest* req = NULL, OPTIONAL IN VARTYPE typeWanted = VT_EMPTY, OPTIONAL IN LCID localeID = LOCALE_SYSTEM_DEFAULT,
					 OPTIONAL OUT HRESULT* pConvRes = NULL);
	virtual BOOL set(IN SOCmnVariant& value, IN WORD quality, OPTIONAL IN SOCmnDateTime* timeStamp = NULL, OPTIONAL IN SODaSRequest* req = NULL, OPTIONAL IN LCID localeID = LOCALE_SYSTEM_DEFAULT);
	virtual BOOL update(IN SOCmnVariant& value, IN WORD quality, OPTIONAL IN SOCmnDateTime* timeStamp = NULL, OPTIONAL IN SODaSRequest* req = NULL, OPTIONAL IN LCID localeID = LOCALE_SYSTEM_DEFAULT);

	// cached value
	virtual BOOL setValue(IN SOCmnVariant& value, OPTIONAL IN SODaSRequest* req = NULL, OPTIONAL IN LCID localeID = LOCALE_SYSTEM_DEFAULT);
	virtual BOOL updateValue(IN SOCmnVariant& value, OPTIONAL IN SODaSRequest* req = NULL, OPTIONAL IN LCID localeID = LOCALE_SYSTEM_DEFAULT);
	virtual BOOL getValue(OUT SOCmnVariant& value);
	virtual BOOL getValue(IN SODaSRequest* req, IN VARTYPE typeWanted, IN LCID localeID, OUT SOCmnVariant& value, OPTIONAL OUT HRESULT* pConvRes = NULL);
	virtual BOOL getValue(IN VARTYPE typeWanted, OUT SOCmnVariant& value, OPTIONAL IN LCID localeID = LOCALE_SYSTEM_DEFAULT, OPTIONAL OUT HRESULT* pConvRes = NULL);

	// size of cache value
	ULONG getValueSize(void);
	static ULONG getValueSize(IN SOCmnVariant& val);

	// quality of cached value
	virtual BOOL setQuality(IN WORD quality, IN SODaSRequest* req = NULL);
	WORD getQuality(void) const;
	virtual WORD getQuality(IN SODaSRequest* req);

	// time of cached value
	virtual BOOL setTimeStamp(IN SOCmnDateTime& timeStamp, IN SODaSRequest* req = NULL);
	BOOL getTimeStamp(OUT SOCmnDateTime& timeStamp) const;
	virtual BOOL getTimeStamp(IN SODaSRequest* req, OUT SOCmnDateTime& timeStamp);

	// native datatype
	virtual BOOL setDatatype(IN VARTYPE datatype);
	VARTYPE getDatatype(void) const;

	// datatype conversion
	static BOOL defaultSupportDatatype(IN VARTYPE dataType);
	virtual BOOL supportDatatype(IN VARTYPE dataType);
	virtual BOOL changeDatatype(IN SOCmnVariant& value, IN VARTYPE type,
								OUT SOCmnVariant& changedValue, OPTIONAL IN LCID localeID = LOCALE_SYSTEM_DEFAULT,
								OPTIONAL IN SODaSItemTag* itemTag = NULL, OPTIONAL OUT HRESULT* pConvRes = NULL);

	// get all item tags which are connected to the cache
	virtual void getAllItemTags(OUT SOCmnList<SODaSItemTag> &itemTagList);

	// object trace id
	virtual LPCTSTR getObjTraceId(void);

	// check if cache set at least once
	BOOL isSet(void)
	{
		return m_timeStamp.isSet();
	}

	// value sting
	LPCTSTR getValueString(IN SOCmnString& buffer, OPTIONAL IN LCID localeID = SOSRVCONVERTER_LCID_DEFAULT);

	void clearValue(void)
	{
		m_value.clear();
	}

protected:
	virtual ~SODaSCache();

	// callbacks
	virtual void onSetValue(IN SOCmnVariant& value);
	virtual void onSetQuality(IN WORD quality);
	virtual void onSetTimeStamp(IN SOCmnDateTime& timeStamp);
	virtual void onSetDatatype(IN VARTYPE datatype);

	SOCmnVariant m_value;      // cached value
	SOCmnDateTime m_timeStamp; // time of cached value
	WORD m_quality;            // quality of cached value
	VARTYPE m_datatype;        // native datatype of cached value

	// set helpers
	virtual BOOL doSetValue(IN SOCmnVariant& value, IN SODaSRequest* req, IN LCID localeID);
	virtual BOOL doUpdateValue(IN SOCmnVariant& value, IN SODaSRequest* req, IN LCID localeID);

	// helper for getAllItemTags
	void addItemTagsOfNode(IN SOCmnElement* node, OUT SOCmnList<SODaSItemTag> &itemTagList);

#ifdef SOFEATURE_WATCH
	// watch helpers
	void notifyWatch(IN BYTE what);
#endif
}; // SODaSCache

inline VARTYPE SODaSCache::getDatatype(void) const
{
	return m_datatype;
}

inline WORD SODaSCache::getQuality(void) const
{
	return m_quality;
}

inline BOOL SODaSCache::getTimeStamp(OUT SOCmnDateTime& timeStamp) const
{
	timeStamp = m_timeStamp;
	return TRUE;
}

#define GenericCache SODaSCache

#pragma pack(pop)
#endif

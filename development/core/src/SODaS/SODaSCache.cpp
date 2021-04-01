//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - SODaS                              |
//                                                                            |
//  Filename    : SODaSCache.cpp                                              |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Cache classes                                               |
//                - SODaSCache: base class for cache objects                  |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#include "opcerror.h"
#include "SODaSCache.h"
#include "SODaSItemTag.h"
#include "SODaSNode.h"
#include "SODaSTag.h"
#include "SODaSProperty.h"
#include "SOCmnTrace.h"
#include "SODaSEntry.h"



//-----------------------------------------------------------------------------
// SODaSCache                                                                 |
//-----------------------------------------------------------------------------

SODaSCache::SODaSCache(void)
	: SOCmnObject(SOCMNOBJECT_TYPE_CACHE)
{
	m_datatype = VT_EMPTY;
	setObjTraceId(NULL);
	m_value.Clear();
	m_quality = OPC_QUALITY_WAITING_FOR_INITIAL_DATA;
}

SODaSCache::~SODaSCache(void)
{
}


//-----------------------------------------------------------------------------
// init
//
// - initialize cache
//
void SODaSCache::init(void)
{
	m_value.init(m_datatype);
} // init


//-----------------------------------------------------------------------------
// getObjTraceId
//
// - get objects trace string id
//
// returns:
//		trace id string
//
LPCTSTR SODaSCache::getObjTraceId(void)
{
	if (!isInitObjTraceId())
	{
		SOCmnString id;
		id.format(_T("[%s|%8.8lX]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle());
		setObjTraceId(id);
	}

	return getActObjTraceIdString();
} // getObjTraceId



//-----------------------------------------------------------------------------
// get
//
// - get cached value, quality and time stamp
// - convert value to the requested datatype
//
// returns:
//		TRUE  - all ok
//		FALSE - can't copy cached value
//				can't convert to requested datatype
//				bad access rights
//
BOOL SODaSCache::get(
	OUT SOCmnVariant& value,            // value
	OUT WORD* quality,              // quality
	OUT SOCmnDateTime* timeStamp,       // time stamp
	OPTIONAL IN SODaSRequest* req,  // request to get the values for
	OPTIONAL IN VARTYPE typeWanted, // wanted data type
	OPTIONAL IN LCID localeID,      // locale id for value conversion
	OPTIONAL OUT HRESULT* pConvRes)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	BOOL ret = TRUE;

	if (value)
	{
		if (req)
		{
			ret = getValue(req, typeWanted, localeID, value, pConvRes);
		}
		else
		{
			ret = getValue(typeWanted, value, localeID, pConvRes);
		}
	}

	if (quality)
	{
		if (ret)
		{
			*quality = getQuality(req);
		}
		else
		{
			*quality = OPC_QUALITY_BAD;
		}
	}

	if (timeStamp)
	{
		getTimeStamp(req, *timeStamp);
	}

	return ret;
} // get


//-----------------------------------------------------------------------------
// set
//
// - set cached value, quality and time stamp
// - convert value to native datatype of tag
//
// returns:
//		TRUE  - all ok
//		FALSE - can't convert to native datatype or write only tag
//
BOOL SODaSCache::set(
	IN SOCmnVariant& value,              // new value
	IN WORD quality,                  // new quality
	OPTIONAL IN SOCmnDateTime* timeStamp, // new time stamp
	OPTIONAL IN SODaSRequest* req,
	OPTIONAL IN LCID localeID)        // locale id for conversions
{
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
	BOOL ret;
	pObjectLock->lock();

	if ((ret = doSetValue(value, req, localeID)) == TRUE)
	{
		SOCmnDateTime ft;
		onSetQuality(quality);
		m_quality = quality;

		if (!timeStamp)
		{
			ft.now();
			timeStamp = &ft;
		}

		onSetTimeStamp(*timeStamp);
		m_timeStamp = *timeStamp;
	}

	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
#ifdef SOFEATURE_WATCH

	if (ret)
		if (getCollectChanges())
		{
			notifyWatch(SODASCACHE_NOTIFYWATCH_VALUE | SODASCACHE_NOTIFYWATCH_QUALITY | SODASCACHE_NOTIFYWATCH_TIMESTAMP);
		}

#endif
	return ret;
} // set


//-----------------------------------------------------------------------------
// update
//
// - update cached value, quality and time stamp
// - convert value to native datatype of tag
//
// returns:
//		TRUE  - all ok
//		FALSE - can't convert to native datatype or write only tag
//
BOOL SODaSCache::update(
	IN SOCmnVariant& value,              // new value
	IN WORD quality,                  // new quality
	OPTIONAL IN SOCmnDateTime* timeStamp, // new time stamp
	OPTIONAL IN SODaSRequest* req,
	OPTIONAL IN LCID localeID)        // locale id for conversions
{
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
	BOOL ret;
	pObjectLock->lock();

	if ((ret = doUpdateValue(value, req, localeID)) == TRUE)
	{
		SOCmnDateTime ft;
		onSetQuality(quality);
		m_quality = quality;

		if (!timeStamp)
		{
			ft.now();
			timeStamp = &ft;
		}

		onSetTimeStamp(*timeStamp);
		m_timeStamp = *timeStamp;
	}

	pObjectLock->unlock();
	SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
#ifdef SOFEATURE_WATCH

	if (ret)
		if (getCollectChanges())
		{
			notifyWatch(SODASCACHE_NOTIFYWATCH_VALUE | SODASCACHE_NOTIFYWATCH_QUALITY | SODASCACHE_NOTIFYWATCH_TIMESTAMP);
		}

#endif
	return ret;
} // update


//-----------------------------------------------------------------------------
// setValue
//
// - set cached value
// - convert value to native datatype of tag
//
// returns:
//		TRUE  - all ok
//		FALSE - can't convert to native datatype or write only tag
//
BOOL SODaSCache::setValue(
	IN SOCmnVariant& value,     // new value
	OPTIONAL IN SODaSRequest* req,
	OPTIONAL IN LCID localeID)  // locale id
{
	BOOL ret;
	ret = doSetValue(value, req, localeID);
#ifdef SOFEATURE_WATCH

	if (ret)
		if (getCollectChanges())
		{
			notifyWatch(SODASCACHE_NOTIFYWATCH_VALUE);
		}

#endif
	return ret;
} // setValue

//-----------------------------------------------------------------------------
// doSetValue
//
// - set cached value
// - convert value to native datatype of tag
//
// returns:
//		TRUE  - all ok
//		FALSE - can't convert to native datatype or write only tag
//
BOOL SODaSCache::doSetValue(
	IN SOCmnVariant& value,     // new value
	OPTIONAL IN SODaSRequest* /* req */,
	OPTIONAL IN LCID localeID)  // locale id
{
	SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
	BOOL ok = TRUE;
	SOCmnDateTime timeStamp;

	if ((m_datatype == value.vt) || (m_datatype == VT_EMPTY))
	{
		timeStamp.now();
		onSetTimeStamp(timeStamp);
		m_timeStamp = timeStamp;
		onSetValue(value);
		pObjectLock->lock();
		m_value = value;
		pObjectLock->unlock();
	}
	else
	{
		SOCmnVariant tmp;
		tmp = value;
		ok = changeDatatype(tmp, m_datatype, tmp, localeID);

		if (ok)
		{
			timeStamp.now();
			onSetTimeStamp(timeStamp);
			m_timeStamp = timeStamp;
			onSetValue(tmp);
			pObjectLock->lock();
			m_value = tmp;
			pObjectLock->unlock();
		}
	}

	SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
	return ok;
} // doSetValue


//-----------------------------------------------------------------------------
// updateValue
//
// - update cached value
//
// returns:
//		TRUE  - all ok
//		FALSE - can't convert to native datatype or write only tag
//
BOOL SODaSCache::updateValue(
	IN SOCmnVariant& value,     // new value
	OPTIONAL IN SODaSRequest* req,
	OPTIONAL IN LCID localeID)  // locale id
{
	BOOL ret;
	ret = doUpdateValue(value, req, localeID);
#ifdef SOFEATURE_WATCH

	if (ret)
		if (getCollectChanges())
		{
			notifyWatch(SODASCACHE_NOTIFYWATCH_VALUE);
		}

#endif
	return ret;
} // updateValue

//-----------------------------------------------------------------------------
// doUpdateValue
//
// - update cached value
//
// returns:
//		TRUE  - all ok
//		FALSE - can't convert to native datatype or write only tag
//
BOOL SODaSCache::doUpdateValue(
	IN SOCmnVariant& value,     // new value
	OPTIONAL IN SODaSRequest* req,
	OPTIONAL IN LCID localeID)  // locale id
{
	if (!(m_datatype & VT_ARRAY))
	{
		return doSetValue(value, req, localeID);
	}
	else
	{
		// cache value is an array
		// -> don't set the whole array! set only the elements of the value new
		LONG lBoundC, uBoundC;
		LONG lBoundV, uBoundV;
		LONG lBound, uBound;
		BYTE* dataC, *dataV;
		SOCmnDateTime timeStamp;
		SOCmnVariant tmp;

		if (m_value.vt != value.vt)
		{
			if (!changeDatatype(value, m_value.vt, tmp, localeID))
			{
				return FALSE;
			}

			value = &tmp;
		}

		if (m_value.parray->rgsabound[0].cElements == 0)
		{
			return doSetValue(value, req, localeID);    // set array if no current elements
		}

		timeStamp.now();
		onSetTimeStamp(timeStamp);
		m_timeStamp = timeStamp;
		onSetValue(value);
		int elementSize = 0;

		switch (m_value.vt & ~VT_ARRAY)
		{
		case VT_UI1:
		case VT_I1:
			elementSize = 1;
			break;

		case VT_I2:
		case VT_BOOL:
		case VT_UI2:
			elementSize = 2;
			break;

		case VT_I4:
		case VT_UI4:
		case VT_INT:
		case VT_UINT:
		case VT_R4:
		case VT_BSTR:
			elementSize = 4;
			break;

		case VT_I8:
		case VT_UI8:
		case VT_R8:
		case VT_DATE:
		case VT_CY:
			elementSize = 8;
			break;

		case VT_VARIANT:
			elementSize = 16;
			break;
		}

		SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
		pObjectLock->lock();
		SafeArrayGetLBound(m_value.parray, 1, &lBoundC);
		SafeArrayGetUBound(m_value.parray, 1, &uBoundC);
		SafeArrayGetLBound(value.parray, 1, &lBoundV);
		SafeArrayGetUBound(value.parray, 1, &uBoundV);

		if ((lBoundV > (uBoundC + 1)) || ((uBoundV  + 1) < lBoundC))
		{
			// new array has to connect to old
			pObjectLock->unlock();
			return FALSE;
		}

		uBound = (uBoundC < uBoundV) ? uBoundV : uBoundC;
		lBound = (lBoundC > lBoundV) ? lBoundV : lBoundC;

		if ((lBoundC > lBoundV) || (uBoundC < uBoundV))
		{
			SAFEARRAYBOUND dim;
			dim.lLbound = lBound;
			dim.cElements = (uBound - lBound + 1);
			::SafeArrayRedim(m_value.parray, &dim);
			SafeArrayAccessData(m_value.parray, (void**)&dataC);

			if (lBoundC > lBound)
			{
				// have to move data to the offset
				memmove(dataC + ((lBoundC - lBound) * elementSize), dataC, (uBoundC - lBoundC + 1) * elementSize);
				memset(dataC, 0, ((lBoundC - lBound) * elementSize));
			}

			if (uBoundC < uBound)
			{
				memset(dataC + (uBoundC * elementSize), 0, ((uBound - uBoundC) * elementSize));
			}

			SafeArrayUnaccessData(m_value.parray);
			lBoundC = lBound;
			uBoundC = uBound;
		}

		SafeArrayAccessData(m_value.parray, (void**)&dataC);
		SafeArrayAccessData(value.parray, (void**)&dataV);

		if ((value.vt != (VT_BSTR | VT_ARRAY)) && (value.vt != (VT_VARIANT | VT_ARRAY)))
		{
			memcpy(dataC + ((lBoundV - lBound) * elementSize), dataV, (uBoundV - lBoundV + 1) * elementSize);
		}
		else
		{
			if (value.vt == (VT_BSTR | VT_ARRAY))
			{
				BSTR* bstrC, *bstrV;
				LONG iC, iV;
				bstrC = (BSTR*)dataC;
				bstrV = (BSTR*)dataV;

				for (iV = 0, iC = (lBoundV - lBound); iV <= (uBoundV - lBoundV); iV++, iC++)
				{
					if (bstrC[iC])
					{
						::SysFreeString(bstrC[iC]);
					}

					bstrC[iC] = ::SysAllocString(bstrV[iV]);
				}
			}
			else
			{
				VARIANT* varC, *varV;
				LONG iC, iV;
				varC = (VARIANT*)dataC;
				varV = (VARIANT*)dataV;

				for (iV = 0, iC = (lBoundV - lBound); iV <= (uBoundV - lBoundV); iV++, iC++)
				{
					if (varC[iC].vt != 0)
					{
						SOCmnVariant::variantClear(&varC[iC]);
					}

					SOCmnVariant::variantCopy(&varC[iC], &varV[iV]);
				}
			}
		}

		SafeArrayUnaccessData(m_value.parray);
		SafeArrayUnaccessData(value.parray);
		pObjectLock->unlock();
		SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
		return TRUE;
	}
} // doUpdateValue


//-----------------------------------------------------------------------------
// getValue
//
// - get cached value
// - convert value to the requested datatype
//
// returns:
//		TRUE  - all ok
//		FALSE - can't copy cached value
//				can't convert to requested datatype; but value is copied !
//				bad access rights
//
BOOL SODaSCache::getValue(
	OUT SOCmnVariant& value)    // cache value
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	BOOL ok;
	value.clear();
	ok = m_value.copyTo(value);
	return ok;
} // getValue

BOOL SODaSCache::getValue(
	IN VARTYPE typeWanted,      // datatype wanted
	OUT SOCmnVariant& value,        // cache value
	OPTIONAL IN LCID localeID,  // locale id
	OPTIONAL OUT HRESULT* pConvRes)
{
	BOOL ok = getValue(value);

	if ((ok) && ((typeWanted != VT_EMPTY) && (typeWanted != (VT_EMPTY | VT_ARRAY)) &&
				 (typeWanted != value.vt)))
	{
		ok = changeDatatype(value, typeWanted, value, localeID, NULL, pConvRes);

		if (!ok)
		{
			SOCmnVariant::variantClear(value);
		}
	}

	return ok;
} // getValue

BOOL SODaSCache::getValue(
	IN SODaSRequest* req,  // request which accesses the cache
	IN VARTYPE typeWanted, // datatype wanted
	IN LCID localeID,      // locale id
	OUT SOCmnVariant& value,   // cache value
	OPTIONAL OUT HRESULT* pConvRes)
{
	SOCmnPointer<SODaSItemTag> itemTag = req->getItemTag();
	SOCmnPointer<SODaSServer> server = req->getServer();

	if ((!itemTag) || (!server))
	{
		return FALSE;
	}

	// check access rights
	if ((itemTag->getAccessRights(server) & OPC_READABLE) != OPC_READABLE)
	{
		if (pConvRes)
		{
			*pConvRes = OPC_E_BADRIGHTS;
		}

		::VariantInit(value);
		return FALSE;
	}
	else
	{
		return getValue(typeWanted, value, localeID, pConvRes);
	}
} // getValue


//-----------------------------------------------------------------------------
// getQuality
//
// - get quality
//
// returns:
//		quality
//
WORD SODaSCache::getQuality(
	IN SODaSRequest* req)   // request
{
	if (req)
	{
		SOCmnPointer<SODaSItemTag> itemTag = req->getItemTag();
		SOCmnPointer<SODaSServer> server = req->getServer();

		if ((!itemTag) || (!server))
		{
			return OPC_QUALITY_BAD;
		}

		if ((itemTag->getAccessRights(server) & OPC_READABLE) != OPC_READABLE)
		{
			return OPC_QUALITY_BAD;
		}
	}

	return m_quality;
} // getQuality


//-----------------------------------------------------------------------------
// setQuality
//
// - set quality
//
// returns:
//		quality
//
BOOL SODaSCache::setQuality(
	IN WORD quality,    // quality
	IN SODaSRequest* /* req */)
{
	onSetQuality(quality);
	m_quality = quality;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		notifyWatch(SODASCACHE_NOTIFYWATCH_QUALITY);
	}

#endif
	return TRUE;
} // setQuality


//-----------------------------------------------------------------------------
// getTimeStamp
//
// - get quality
//
// returns:
//		TRUE
//
BOOL SODaSCache::getTimeStamp(
	IN SODaSRequest* req,       // request
	OUT SOCmnDateTime& timeStamp)   // time stamp
{
	if (req)
	{
		SOCmnPointer<SODaSItemTag> itemTag = req->getItemTag();
		SOCmnPointer<SODaSServer> server = req->getServer();

		if ((!itemTag) || (!server))
		{
			timeStamp.init();
			return TRUE;
		}

		if ((itemTag->getAccessRights(server) & OPC_READABLE) != OPC_READABLE)
		{
			timeStamp.init();
			return TRUE;
		}
	}

	timeStamp = m_timeStamp;
	return TRUE;
} // getTimeStamp


//-----------------------------------------------------------------------------
// setTimeStamp
//
// - set time stamp
//
// returns:
//		time stamp
//
BOOL SODaSCache::setTimeStamp(
	IN SOCmnDateTime& timeStamp,    // time stamp
	IN SODaSRequest* /* req */)
{
	onSetTimeStamp(timeStamp);
	m_timeStamp = timeStamp;
#ifdef SOFEATURE_WATCH

	if (getCollectChanges())
	{
		notifyWatch(SODASCACHE_NOTIFYWATCH_TIMESTAMP);
	}

#endif
	return TRUE;
} // setTimeStamp


//-----------------------------------------------------------------------------
// onSetValue
//
// - called before value is set
// - default implementation: do nothing
//
void SODaSCache::onSetValue(
	IN SOCmnVariant& /* value */)   // value
{} // onSetValue


//-----------------------------------------------------------------------------
// onSetQuality
//
// - called before quality is set
// - default implementation: do nothing
//
void SODaSCache::onSetQuality(
	IN WORD /* quality */)  // quality
{} // onSetQuality


//-----------------------------------------------------------------------------
// onSetTimeStamp
//
// - called before time stamp is set
// - default implementation: do nothing
//
void SODaSCache::onSetTimeStamp(
	IN SOCmnDateTime& /* timeStamp */) // time stamp
{} // onSetTimeStamp


//-----------------------------------------------------------------------------
// setDatatype
//
// - sets datatype
//
// returns:
//		TRUE
//
BOOL SODaSCache::setDatatype(
	IN VARTYPE datatype)    // new native datatype
{
	if (!supportDatatype(datatype))
	{
		return FALSE;
	}

	onSetDatatype(datatype);
	m_datatype = datatype;
	return TRUE;
} // setDatatype


//-----------------------------------------------------------------------------
// onSetDatatype
//
// - called before datatype is set
// - default implementation: do nothing
//
void SODaSCache::onSetDatatype(
	IN VARTYPE /* datatype */)  // datatype
{} // onSetDatatype


//-----------------------------------------------------------------------------
// defaultSupportDatatype
//
// - check if the datatype is one of the default datatypes supported by the toolkit
//
// returns:
//		TRUE  - datatype supported
//		FALSE - datatype not supported
//
BOOL SODaSCache::defaultSupportDatatype(
	IN VARTYPE dataType)    // data type
{
	return SOCmnVariant::supportDatatype(dataType);
} // defaultSupportDatatype


//-----------------------------------------------------------------------------
// supportDatatype
//
// - check if the datatype is supported by the application
//
// returns:
//		TRUE  - datatype supported
//		FALSE - datatype not supported
//
BOOL SODaSCache::supportDatatype(
	IN VARTYPE dataType)    // data type
{
	return defaultSupportDatatype(dataType);
} // supportDatatype


//-----------------------------------------------------------------------------
// changeDatatype
//
// - change the datatype of a value
//
// returns:
//		TRUE  - datatype changed
//		FALSE - datatype could not be changed
//
BOOL SODaSCache::changeDatatype(
	IN SOCmnVariant& value,                 // value
	IN VARTYPE type,                   // new datatype
	OUT SOCmnVariant& changedValue,         // changed value
	OPTIONAL IN LCID localeID,         // locale id
	OPTIONAL IN SODaSItemTag* /* itemTag */, // item tag
	OPTIONAL OUT HRESULT* pConvRes)    // conversion result
{
	return SOCmnVariant::changeDatatype(value, type, localeID, changedValue, pConvRes);
} // changeDatatype


//-----------------------------------------------------------------------------
// getValueSize
//
// - get the size in bytes of the value
//
// returns:
//		value size
//
ULONG SODaSCache::getValueSize(void)
{
	return getValueSize(m_value);
} // getValueSize

//-----------------------------------------------------------------------------
// getValueSize
//
// - get the size in bytes of the value
//
// returns:
//		value size
//
ULONG SODaSCache::getValueSize(
	IN SOCmnVariant& value) // value
{
	if (!value)
	{
		return 0;
	}

	ULONG size  = sizeof(VARIANT);
	USES_CONVERSION;

	if ((value.vt & VT_ARRAY) == 0)
	{
		// no array
		if (value.vt == VT_BSTR)
		{
			size += 6 + (DWORD)(_tcslen(OLE2CT(value.bstrVal)) * 2);
		}
	}
	else
	{
		// array
		if (value.parray->cDims == 1)
		{
			size += sizeof(SAFEARRAY) +
					(value.parray->cbElements * value.parray->rgsabound[0].cElements);

			if (value.vt == (VT_BSTR | VT_ARRAY))
			{
				BSTR* bstr;
				ULONG i;
				SafeArrayAccessData(value.parray, (void**)&bstr);

				for (i = 0; i < value.parray->rgsabound[0].cElements; i++)
				{
					size += 6 + (DWORD)(_tcslen(OLE2CT(bstr[i])) * 2);
				}

				SafeArrayUnaccessData(value.parray);
			}
		}
		else
		{
			_ASSERTION(FALSE, "only arrays with one dimesion supported!");
		}
	}

	return size;
} // getValueSize


//-----------------------------------------------------------------------------
// getAllItemTags
//
// - all item tags using the cache
//
void SODaSCache::getAllItemTags(
	OUT SOCmnList<SODaSItemTag> &itemTagList)   // item tag list
{
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	DWORD i;

	for (i = 0; i < SODAS_SERVER_INSTANCE_COUNT; i++)
	{
		SOCmnPointer<SODaSNameSpaceRoot> nsRoot = engine->getNameSpaceRoot((BYTE)i);
		addItemTagsOfNode(nsRoot, itemTagList);
	}
} // getAllItemTags


//-----------------------------------------------------------------------------
// addItemTagsOfNode
//
// - add all item tags of the given node which use the cache to the list
//
void SODaSCache::addItemTagsOfNode(
	IN SOCmnElement* branch,                    // node
	OUT SOCmnList<SODaSItemTag> &itemTagList)   // item tag list
{
	SOCmnListPosition pos;
	SOCmnList<SODaSProperty> propList;
	SODaSProperty* prop;
	SOCmnListPosition posProp;
	{
		SOCmnPointer<SODaSCache> cache;
		SOCmnList<SOCmnElement> leafList(branch->getLeafList());
		SODaSTag* tag;
		pos = leafList.getStartPosition();

		while (pos)
		{
			tag = (SODaSTag*)leafList.getNext(pos);

			if (tag->hasCache(this))
			{
				tag->addRef();
				itemTagList.AddTail(tag);
			}

			propList = (SOCmnList<SODaSProperty> *)tag->getAdditionalList();

			if (!!propList)
			{
				posProp = propList.getStartPosition();

				while (posProp)
				{
					prop = (SODaSProperty*)propList.getNext(posProp);

					if (prop->hasCache(this))
					{
						prop->addRef();
						itemTagList.AddTail(prop);
					}
				}

				propList.autoUnlock();
			}
		}

		leafList.autoUnlock();
	}
	SOCmnList<SOCmnElement> branchList(branch->getBranchList());
	SODaSNode* node;
	pos = branchList.getStartPosition();

	while (pos)
	{
		node = (SODaSNode*)branchList.getNext(pos);
		propList = (SOCmnList<SODaSProperty> *)node->getAdditionalList();

		if (!!propList)
		{
			posProp = propList.getStartPosition();

			while (posProp)
			{
				prop = (SODaSProperty*)propList.getNext(posProp);

				if (prop->hasCache(this))
				{
					prop->addRef();
					itemTagList.AddTail(prop);
				}
			}

			propList.autoUnlock();
		}

		addItemTagsOfNode(node, itemTagList);
	}
} // addItemTagsOfNode


LPCTSTR SODaSCache::getValueString(IN SOCmnString& buffer, OPTIONAL IN LCID localeID)
{
	SOCmnSingleObjectLock<SOCmnSync> objectLock(this);
	SOCmnPointer<SOSrvConverter> conv = getSOSrvConverter();
	return conv->variant2string(buffer, m_value, localeID);
}


#ifdef SOFEATURE_WATCH
void SODaSCache::notifyWatch(
	IN BYTE what)
{
	SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();
	SOCmnPointer<SOSrvConverter> conv = getSOSrvConverter();
	SOCmnString buffer;
	SOCmnStringEx szXML;
	SOCmnStringEx szRawAttr;

	if (what & SODASCACHE_NOTIFYWATCH_VALUE)
	{
		SOCmnSync* pObjectLock = SOCmnObjectLock::getObjectLock(this);
		pObjectLock->lock();
		conv->variant2string(buffer, m_value);
		pObjectLock->unlock();
		SOCmnObjectLock::releaseObjectLock(this, pObjectLock);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_VALUE_T, buffer);
		entry->watchAttributeChange(this, SOWATCH_ATTRIBUTE_VALUE_T, szXML);
	}

	if (what & SODASCACHE_NOTIFYWATCH_QUALITY)
	{
		szXML.empty();
		szRawAttr.format(_T("%s = \"%u\""), SOWATCH_ATTR_RAW_T, m_quality);
		SOSrvWatch::createElementTextWithAttributes(szXML, SOWATCH_ATTRIBUTE_QUALITY_T, conv->quality2string(buffer, m_quality), szRawAttr);
		entry->watchAttributeChange(this, SOWATCH_ATTR_RAW_T, szXML);
	}

	if (what & SODASCACHE_NOTIFYWATCH_TIMESTAMP)
	{
		szXML.empty();
		szRawAttr.format(_T("%s = \"%lu\""), SOWATCH_ATTR_RAW_T, DWORD(m_timeStamp.getMicroseconds() / 1000));
		SOSrvWatch::createElementTextWithAttributes(szXML, SOWATCH_ATTRIBUTE_TIMESTAMP_T, conv->datetime2string(buffer, m_timeStamp), szRawAttr);
		entry->watchAttributeChange(this, SOWATCH_ATTRIBUTE_TIMESTAMP_T, szXML);
	}
}
#endif

#endif // SODAS

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
//  Filename    : SODaSTag.cpp                                                |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Namespace Tag classes                                       |
//                - SODaSTag: base class for namespace tag object             |
//                - SODaSTagDefault: default configuration for tag objects    |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#include "SODaSItemTag.h"
#include "SODaSTag.h"
#include "SODaSCache.h"
#include "SOCmnTrace.h"
#include "SODaSEntry.h"
#include "SODaSItem.h"
#include "SODaSServer.h"
#include <opcerror.h>




//-----------------------------------------------------------------------------
// SODaSTag                                                                   |
//-----------------------------------------------------------------------------

SODaSTag::SODaSTag(void)
	: SODaSItemTag(SOCMNOBJECT_TYPE_TAG)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_TAG);
#endif
}

SODaSTag::~SODaSTag(void)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_TAG);
#endif
}

//-----------------------------------------------------------------------------
// getObjectElementIName
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaSTag::getObjectCmnElementIName(void)
{
	return (SOCmnObject*)this;
} // getObjectElementIName


//-----------------------------------------------------------------------------
// isUniqueName
//
// - are both names unique in the whole server
//
// returns:
//		FALSE - no global unique name
//
BOOL SODaSTag::isUniqueName(void)
const
{
	return FALSE;
} // isUniqueName


//-----------------------------------------------------------------------------
// trustUniqueChilds
//
// - trust that provided child names are unique
//
// returns:
//		TRUE  - trust the names
//		FALSE - check the names
//
BOOL SODaSTag::trustUniqueChilds(void)
{
	return TRUE;
} // trustUniqueChilds


//-----------------------------------------------------------------------------
// release
//
// - check if to delete dynamic created tags
//
// returns:
//		reference count
//
LONG SODaSTag::release(void)
{
	if (getDynamic())
	{
		SOCmnPointer<SOCmnElement> parent = getParentBranch();
		SOCmnList<SODaSProperty> *pList = (SOCmnList<SODaSProperty> *)getAdditionalList();
		SODaSProperty* prop;
		SOCmnListPosition pos;
		int nRefs = 2;
#ifdef SOFEATURE_WATCH

		if (getCollectChanges())
		{
			nRefs++;
		}

#endif

		if (pList != NULL)
		{
			SOCmnList<SODaSProperty> propList(pList);

			if (!!propList)
			{
				pos = propList.getStartPosition();

				while (pos)
				{
					prop = propList.getNext(pos);

					if (prop->getRefCount() == 1)
					{
						nRefs ++;
					}
				}

				propList.autoUnlock();
			}
		}

		if (getRefCount() == nRefs)
		{
			if (!(!parent))
			{
				SOCmnList<SOCmnElement> leafList(parent->getLeafList());
				leafList.lock();

				if (getRefCount() == nRefs)
				{
					setDynamic(FALSE);  // prevent recursion
					removeAllAdditionalElements();
					parent->removeLeaf(this);
				}

				leafList.unlock();
			}
		}
	}

	return SOCmnObject::release();
} // release


//-----------------------------------------------------------------------------
// setParentBranch
//
// - set parent branch of this object
// - check if the elements name is unique in parent
//
// returns:
//		TRUE  - branch set
//		FALSE - no unique name
//
BOOL SODaSTag::setParentBranch(
	IN SOCmnElement* parent)    // parent branch
{
	BOOL ret;
#ifdef SOFEATURE_WATCH
	BOOL doCollect = FALSE;
	SOCmnPointer<SOCmnElement> prevParent = getParentBranch();

	if (prevParent.isNotNull())
	{
		doCollect = prevParent->getCollectChanges();
	}
	else if (parent)
	{
		doCollect = parent->getCollectChanges();
	}

#endif
	ret = SOCmnElement::setParentBranch(parent);
#ifdef SOFEATURE_WATCH

	if ((doCollect || (getCollectChanges())) && (ret))
	{
		SOCmnPointer<SOSrvEntry> entry = ::getSOSrvEntry();

		if (parent != NULL)
		{
			entry->watchAddObject(parent, this);
		}
		else if ((parent == NULL) && (prevParent.isNotNull()))
		{
			entry->watchRemoveObject(prevParent, this);
			prevParent.release();
		}
	}

#endif
	return ret;
} // setParentBranch


//-----------------------------------------------------------------------------
// getDeviceIOMode
//
// - get the device I/O mode of the tag
//
// returns:
//		device I/O mode
//
BYTE SODaSTag::getDeviceIOMode(
	IN SODaSRequest* req)   // request
{
	if (req)
		if ((getUpdateMode() == SODaSItemTag::report) && (req->getOperation() == SODaSRequest::read))
		{
			SOCmnPointer<SODaSCache> cache = getCache();

			if (cache.isNotNull())
				if (cache->isSet())
				{
					return SODaSItemTag::cache;    // use the cache value for event driven read operations
				}
		}

	if (!(m_deviceIOAndUpdateMode & SODASITEMTAG_MASK_DEVICEIOMODE))    // default I/O mode for tags is queued
	{
		return SODaSItemTag::queue;
	}
	else
	{
		return m_deviceIOAndUpdateMode & SODASITEMTAG_MASK_DEVICEIOMODE;
	}
} // getDeviceIOMode

#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SODaSTag::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_DA_NAMESPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_DA_TAG);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOCmnString buffer;
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnPointer<SODaSCache> cache = getCache();
		SOCmnDateTime timeStamp;
		WORD quality;
		SOCmnStringEx szRawAttr;
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_NAME_T, getName());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PATH_T, getPath());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATE_T, getDeviceState());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATEINFO_T, getDeviceStateInfo());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ITEMID_T, getItemID());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_VALUE_T, cache->getValueString(buffer));
		quality = cache->getQuality();
		szRawAttr.format(_T("%s = \"%u\""), SOWATCH_ATTR_RAW_T, quality);
		SOSrvWatch::createElementTextWithAttributes(szXML, SOWATCH_ATTRIBUTE_QUALITY_T, conv->quality2string(buffer, quality), szRawAttr);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DATATYPE_T, conv->vartype2string(buffer, getNativeDatatype()));
		cache->getTimeStamp(timeStamp);
		szRawAttr.format(_T("%s = \"%lu\""), SOWATCH_ATTR_RAW_T, DWORD(timeStamp.getMicroseconds() / 1000));
		SOSrvWatch::createElementTextWithAttributes(szXML, SOWATCH_ATTRIBUTE_TIMESTAMP_T, conv->datetime2string(buffer, timeStamp), szRawAttr);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_ACCESSRIGHTS_T, conv->accessRights2string(buffer, getAccessRights()));

		if (getEUType() != OPC_NOENUM)
		{
			SOCmnVariant euInfo;
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_EUTYPE_T, conv->euType2string(buffer, getEUType()));
			getEUInfo(euInfo);
			SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_EUINFO_T, conv->variant2string(buffer, euInfo));
		}

		SOCmnList<SOCmnElement> *pList = getAdditionalList();
		BOOL hasProps = FALSE;

		if (pList != NULL)
			if (pList->isInit())
			{
				SOCmnList<SODaSProperty> propList((SOCmnList<SODaSProperty> *)pList);
				hasProps = !propList.isEmpty();
			}

		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_HASPROPERTIES_T, conv->bool2string(buffer, hasProps));
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH



//-----------------------------------------------------------------------------
// SODaSTagDefault                                                            |
//-----------------------------------------------------------------------------

SODaSTagDefault::SODaSTagDefault(void)
	: SOCmnElementListAdditional(0),
	  SODaSTag()
{}

SODaSTagDefault::SODaSTagDefault(
	IN LPCTSTR tagName) // node name
	: SOCmnElementListAdditional(0),
	  SODaSTag()
{
	setName(tagName);
}

//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaSTagDefault::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList

#endif // SODAS


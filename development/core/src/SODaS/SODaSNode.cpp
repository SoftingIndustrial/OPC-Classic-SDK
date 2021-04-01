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
//  Filename    : SODaSNode.cpp                                               |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Namespace branch classes                                    |
//                - SODaSNode: branch object in namespace tree                |
//                - SODaSNodeDefault: default configuration of node object    |
//                - SODaSNameSpaceRoot: root object of namespace tree         |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SODAS

#include "SODaSNode.h"
#include "SODaSTag.h"
#include "SODaSCache.h"
#include "SOCmnTrace.h"
#include "SODaSEntry.h"
#include "SODaSItem.h"
#include "SODaSServer.h"
#include <opcerror.h>



//-----------------------------------------------------------------------------
// SODaSNode                                                                  |
//-----------------------------------------------------------------------------

SODaSNode::SODaSNode()      // hash table size of node map
	: SODaSItemTag(SOCMNOBJECT_TYPE_NODE)
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->incObjectCountStatistics(SOCMNOBJECT_TYPE_NODE);
#endif
}

SODaSNode::~SODaSNode()
{
#ifdef SOFEATURE_WATCH
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	entry->decObjectCountStatistics(SOCMNOBJECT_TYPE_NODE);
#endif
}

//-----------------------------------------------------------------------------
// release
//
// - check if to delete dynamic created nodes
//
// returns:
//		reference count
//
LONG SODaSNode::release(void)
{
	if (getDynamic())
	{
		SOCmnPointer<SOCmnElement> parent = getParentBranch();
		SOCmnList<SODaSProperty> *pList = (SOCmnList<SODaSProperty> *)getAdditionalList();
		SODaSProperty* prop;
		SOCmnListPosition pos;
		int nRefs = 2;  // no tag in this branch !!
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
			if (parent.isNotNull())
			{
				SOCmnList<SOCmnElement> branchList(parent->getBranchList());
				branchList.lock();

				if (getRefCount() == nRefs)
				{
					setDynamic(FALSE);  // prevent recursion
					removeAllAdditionalElements();
					parent->removeBranch(this);
				}

				branchList.unlock();
			}
		}
	}

	return SOCmnObject::release();
} // release


//-----------------------------------------------------------------------------
// getObjectElementIName
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaSNode::getObjectCmnElementIName(void)
{
	return (SOCmnObject*)this;
} // getObjectElementIName


//-----------------------------------------------------------------------------
// addLeaf
//
// - adds leaf to leaf list, if leaf is a tag
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SODaSNode::addLeaf(
	IN SOCmnElement* newLeaf)
{
	if (!newLeaf)
	{
		return FALSE;
	}

	if (newLeaf->is(SOCMNOBJECT_TYPE_TAG))
	{
		return SOCmnElement::addLeaf(newLeaf);
	}
	else
	{
		return FALSE;
	}
} // addLeaf


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to branch list, if branch is a node
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SODaSNode::addBranch(
	IN SOCmnElement* newBranch)
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (newBranch->is(SOCMNOBJECT_TYPE_NODE))
	{
		return SOCmnElement::addBranch(newBranch);
	}
	else
	{
		return FALSE;
	}
} // addBranch


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
BOOL SODaSNode::setParentBranch(
	IN SOCmnElement* parent)    // parent branch
{
	BOOL ret;
#ifdef SOFEATURE_WATCH
	BOOL doCollect = FALSE;
	SOCmnPointer<SOCmnElement> prevPar = getParentBranch();

	if (prevPar.isNotNull())
	{
		doCollect = prevPar->getCollectChanges();
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
		else if ((parent == NULL) && (prevPar.isNotNull()))
		{
			entry->watchRemoveObject(prevPar, this);
		}
	}

#endif
	return ret;
} // setParentBranch


//-----------------------------------------------------------------------------
// getTreeDelimiter
//
// - get delimter of the tree of the element
//
// returns:
//		primary tree delimiter
//
TCHAR SODaSNode::getTreeDelimiter(
	OUT LPCTSTR* accept,    // accepted characters
	OUT LPCTSTR* bad)       // not accepted characters
{
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	SOCmnPointer<SODaSNameSpaceRoot> root = entry->getNameSpaceRoot();

	if (root.isNotNull())
	{
		return root->getTreeDelimiter(accept, bad);
	}

	return SOCmnElement::s_delimCreate;
} // getTreeDelimiter


//-----------------------------------------------------------------------------
// isUniqueName
//
// - are both names unique in the whole server
//
// returns:
//		FALSE - no global unique name
//
BOOL SODaSNode::isUniqueName(void)
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
BOOL SODaSNode::trustUniqueChilds(void)
{
	return TRUE;
} // trustUniqueChilds


//-----------------------------------------------------------------------------
// getDeviceIOMode
//
// - get the device I/O mode of the tag
//
// returns:
//		device I/O mode
//
BYTE SODaSNode::getDeviceIOMode(
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
		return SODaSItemTag::noDeviceIO;
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
void SODaSNode::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_DA_NAMESPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_DA_NODE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_NAME_T, getName());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PATH_T, getPath());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATE_T, getDeviceState());
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_DEVSTATEINFO_T, getDeviceStateInfo());

		if (getDeviceIOMode(NULL) != SODaSItemTag::noDeviceIO)
		{
			SOCmnPointer<SODaSCache> cache = getCache();
			SOCmnString buffer;
			SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
			SOCmnStringEx szRawAttr;
			WORD quality;
			SOCmnDateTime timeStamp;
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
		}

		SOCmnList<SOCmnElement> *pList = getAdditionalList();
		BOOL hasProps = FALSE;
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnString buffer;

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
// SODaSNodeDefault                                                           |
//-----------------------------------------------------------------------------

SODaSNodeDefault::SODaSNodeDefault(void)
	: SOCmnElementListLeafBranchAdditional(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING,
										   SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING, SOCMNLIST_TYPE_ORG_LIST),
	SODaSNode()
{
	m_branchList.ensureType(SOCMNLIST_TYPE_OBJECT_KEY_STRING);
	m_leafList.ensureType(SOCMNLIST_TYPE_OBJECT_KEY_STRING);
}

SODaSNodeDefault::SODaSNodeDefault(
	IN LPCTSTR nodeName) // node name
	: SOCmnElementListLeafBranchAdditional(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING,
										   SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING, SOCMNLIST_TYPE_ORG_LIST),
	SODaSNode()
{
	m_branchList.ensureType(SOCMNLIST_TYPE_OBJECT_KEY_STRING);
	m_leafList.ensureType(SOCMNLIST_TYPE_OBJECT_KEY_STRING);
	setName(nodeName);
}

//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaSNodeDefault::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList




//-----------------------------------------------------------------------------
// SODaSNameSpaceRoot                                                         |
//-----------------------------------------------------------------------------

SODaSNameSpaceRoot::SODaSNameSpaceRoot(void)
	: SOCmnElementListLeafBranch(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING,
								 SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_OBJECT_KEY_STRING),
	SOCmnElement(SOCMNOBJECT_TYPE_ROOT)
{
	SOCmnString id;
	// tree delimiters
	m_delimNScreate = _T('.');
	m_delimNSaccept = _T(".,/\\!|");
#ifdef SOFEATURE_DCOM
	m_badNSchars = _T("\"`'#");
#endif
#ifdef SOFEATURE_SOAP
	m_badNSchars = _T("\"`'#<>&");
#endif
	m_trustUniqueChilds = TRUE;
	m_useTagLinks = FALSE;
	id.format(_T("[%s|%8.8lX| %s]"), (LPCTSTR)getObjTypeString(), (DWORD)this->getOpcHandle(), _T("DaS Name Space Root"));
	setObjTraceId(id);
}


//-----------------------------------------------------------------------------
// getObjectCmnElementIList
//
// - get object using the implementation of the interface
//
// returns:
//		object
//
SOCmnObject* SODaSNameSpaceRoot::getObjectCmnElementIList(void)
{
	return (SOCmnObject*)this;
} // getObjectCmnElementIList


//-----------------------------------------------------------------------------
// addLeaf
//
// - adds leaf to leaf list, if leaf is a tag
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SODaSNameSpaceRoot::addLeaf(
	IN SOCmnElement* newLeaf)
{
	if (!newLeaf)
	{
		return FALSE;
	}

	if (newLeaf->is(SOCMNOBJECT_TYPE_TAG))
	{
		return SOCmnElement::addLeaf(newLeaf);
	}
	else
	{
		return FALSE;
	}
} // addLeaf


//-----------------------------------------------------------------------------
// addBranch
//
// - adds branch to branch list, if branch is a node
//
// returns:
//		TRUE  - unique name
//		FALSE - no unique name
//
BOOL SODaSNameSpaceRoot::addBranch(
	IN SOCmnElement* newBranch)
{
	if (!newBranch)
	{
		return FALSE;
	}

	if (newBranch->is(SOCMNOBJECT_TYPE_NODE))
	{
		return SOCmnElement::addBranch(newBranch);
	}
	else
	{
		return FALSE;
	}
} // addBranch


//-----------------------------------------------------------------------------
// getTreeDelimiter
//
// - get delimter of the tree of the element
//
// returns:
//		primary tree delimiter
//
TCHAR SODaSNameSpaceRoot::getTreeDelimiter(
	OUT LPCTSTR* accept,    // accepted characters
	OUT LPCTSTR* bad)       // not accepted characters
{
	if (accept)
	{
		*accept = m_delimNSaccept;
	}

	if (bad)
	{
		*bad = m_badNSchars;
	}

	return m_delimNScreate;
} // getTreeDelimiter


//-----------------------------------------------------------------------------
// getTag
//
// - get the tag object with the given qulaified name
//
// returns:
//		tag object
//		NULL - no tag object with this name
//
SODaSTag* SODaSNameSpaceRoot::getTag(
	IN LPCTSTR fullName,
	IN OPTIONAL BYTE serverInstance)
{
	if (serverInstance >= SODAS_SERVER_INSTANCE_COUNT)
	{
		return NULL;
	}

	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSNameSpaceRoot> root;
	SODaSTag* tag;
	root = engine->getNameSpaceRoot(serverInstance);
	tag = (SODaSTag*)root->findLeaf(fullName, TRUE);

	if ((SODaSTag*)tag)
	{
		return tag;
	}
	else
	{
		return NULL;
	}
} // getTag


//-----------------------------------------------------------------------------
// getNode
//
// - get the node object with the given qulaified name
//
// returns:
//		node object
//		NULL - no node object with this name
//
SODaSNode* SODaSNameSpaceRoot::getNode(
	IN LPCTSTR fullName,
	IN OPTIONAL BYTE serverInstance)
{
	if (serverInstance >= SODAS_SERVER_INSTANCE_COUNT)
	{
		return NULL;
	}

	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSNameSpaceRoot> root;
	SODaSNode* node;
	root = engine->getNameSpaceRoot(serverInstance);
	node = (SODaSNode*)root->findBranch(fullName, TRUE);

	if ((SODaSNode*)node)
	{
		return node;
	}
	else
	{
		return NULL;
	}
} // getNode


//-----------------------------------------------------------------------------
// getElement
//
// - get the object with the given qulaified name
//
// returns:
//		namespace element object
//		NULL - no object with this name
//
SOCmnElement* SODaSNameSpaceRoot::getElement(
	IN LPCTSTR fullName,
	IN OPTIONAL BYTE serverInstance)
{
	if (serverInstance >= SODAS_SERVER_INSTANCE_COUNT)
	{
		return NULL;
	}

	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSNameSpaceRoot> root;
	SOCmnElement* element = NULL;
	root = engine->getNameSpaceRoot(serverInstance);

	if (fullName[0] != _T('\0'))
	{
		element = root->findLeaf(fullName, TRUE);

		if (!element)
		{
			element = root->findBranch(fullName, TRUE);
		}
	}
	else
	{
// not compliant with the compliance test -> removed
//	element = root;
//	element->addRef();
	}

	return element;
} // getElement


//-----------------------------------------------------------------------------
// trustUniqueChilds
//
// - trust that provided child names are unique
//
// returns:
//		TRUE  - trust the names
//		FALSE - check the names
//
BOOL SODaSNameSpaceRoot::trustUniqueChilds(void)
{
	return m_trustUniqueChilds;
} // trustUniqueChilds


#ifdef SOFEATURE_WATCH

//-----------------------------------------------------------------------------
// getObjWatchData
//
// - get the watch data of the object
//
void SODaSNameSpaceRoot::getObjWatchData(
	IN SOCmnStringEx& szXML,                       // XML data
	IN BOOL withAttributes,                      // add attributes to watch data
	IN LPTSTR objNodeName)                       // name of XML object node
{
	szXML.getBuffer(256);
	SOSrvWatch::addStartElementText(szXML, objNodeName);
	// object parameter elements
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SPACE_T, SOWATCH_SPACE_DA_NAMESPACE);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_TYPE_T, SOWATCH_TYPE_DA_NAMESPACEROOT);
	SOSrvWatch::createElementText(szXML, SOWATCH_ELEMENT_SERVERHANDLE_T, (DWORD)this->getOpcHandle());

	// object attributes
	if (withAttributes)
	{
		SOSrvWatch::addStartElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
		SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
		SOCmnPointer<SOSrvConverter> conv = ::getSOSrvConverter();
		SOCmnPointer<SODaSNameSpaceRoot> nsRoot;
		BYTE i;

		for (i = 0; i < SODAS_SERVER_INSTANCE_COUNT; i++)
		{
			nsRoot = entry->getNameSpaceRoot(i);
#ifdef SOFEATURE_DCOM

			if ((SODaSNameSpaceRoot*)nsRoot == this)
			{
				SOCmnString buffer;
				CLSID clsid;
				entry->getCLSID(clsid, i);
				SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_CLSID_T, conv->CLSID2string(buffer, clsid));
				break;
			}

#endif
		}

		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PATHDELIMITER_T, SOCmnString(getTreeDelimiter(NULL, NULL)));
		SOSrvWatch::createElementText(szXML, SOWATCH_ATTRIBUTE_PROPERTYDELIMITER_T, SOCmnString(entry->getPropertyDelimiter()));
		SOSrvWatch::addEndElementText(szXML, SOWATCH_ELEMENT_ATTRIBUTES_T);
	}

	SOSrvWatch::addEndElementText(szXML, objNodeName);
} // getObjWatchData

#endif // SOFEATURE_WATCH

#endif // SODAS

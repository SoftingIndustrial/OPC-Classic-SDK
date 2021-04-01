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
//                           OPC Toolkit - SOXpS                              |
//                                                                            |
//  Filename    : SOXpSCallbacks.cpp                                          |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : Callback functions for configuration DLL                    |
//                                                                            |
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "SOXpS.h"
#include "DllMain.h"
#include "SOXpSCallbacks.h"
#include "SOXpSDAObjects.h"
#include "SOXpSAEObjects.h"
#include "SOXpSmc.h"


_SOXpSInitialize g_fpSOXpSInitialize = NULL;
_SOXpSStart g_fpSOXpSStart = NULL;
_SOXpSStop g_fpSOXpSStop = NULL;
_SOXpSBuildDANameSpace g_fpSOXpSBuildDANameSpace = NULL;
_SOXpSHandleDARequests g_fpSOXpSHandleDARequests = NULL;
_SOXpSActivateDAItems g_fpSOXpSActivateDAItems = NULL;
_SOXpSBuildAEAreaSpace g_fpSOXpSBuildAEAreaSpace = NULL;
_SOXpSBuildAEEventSpace g_fpSOXpSBuildAEEventSpace = NULL;
_SOXpSActivateAESubscription g_fpSOXpSActivateAESubscription = NULL;
_SOXpSReceivedAEAcknowledge g_fpSOXpSReceivedAEAcknowledge = NULL;
_SOXpSTranslateToItemIDs g_fpSOXpSTranslateToItemIDs = NULL;

HINSTANCE g_dllInstance = NULL;

SOCmnSync g_lockFireAEEvents;
SOCmnSync g_lockCompletedDARequests;
SOCmnSync g_lockChangeAEConditions;
SOCmnSync g_lockReceivedAEAcknowledge;
SOCmnSync g_lockTranslateToItemID;
SOCmnList<SOAeSAttributeValueDADescription> *g_translateToItemIDList = NULL;

extern void addToMessageLog(IN DWORD id, IN LPCTSTR par);

extern BOOL g_isService;
#ifdef SOXPS_SERVICE
extern void WINAPI serviceControlHandler(IN DWORD ctrlCode);
#endif
#ifdef SOXPS_SERVER
extern HWND g_hWnd;
#endif
extern SOCmnString *g_pShutdownReason;

BOOL loadCfgDLL(void)
{
	g_dllInstance = LoadLibrary(_T("SOXpSCfg.DLL"));
	if (g_dllInstance == NULL)
	{
		// no SOXpSCfg DLL - try to load DLL with the name of the server
		TCHAR mfn[1024];
		TCHAR name[1024];
		HANDLE hFF;
		WIN32_FIND_DATA findFileData;

		name[0] = _T('\0');
		GetModuleFileName(getInstanceHandle(), mfn, 1024);

		if (INVALID_HANDLE_VALUE != (hFF = FindFirstFile(mfn, &findFileData)))
		{
			_tcscpy(name, findFileData.cFileName);
			_tcscpy(name + (_tcslen(name) - 4), _T(".dll"));
			FindClose(hFF);
		}

		g_dllInstance = LoadLibrary(name);
		if (g_dllInstance == NULL)
		{
			_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("Can't load the SOXpSCfg.DLL or the %s"), name));
			_stprintf(mfn, _T("Can't load the SOXpSCfg.DLL or the %s"), name);
			if (!g_isService)
				MessageBox(NULL, mfn, _T("XPress OPC Server Tool"), MB_ICONSTOP | MB_OK | MB_DEFAULT_DESKTOP_ONLY);
			addToMessageLog(SOXPS_ERROR_NO_DLL, name);
			return FALSE;
		}
	}

	g_fpSOXpSInitialize = (_SOXpSInitialize)GetProcAddress(g_dllInstance, "SOXpSInitialize");
	if (!g_fpSOXpSInitialize)
	{
		_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_USER1, _T("XPRESS"), (_T("The SOXpSCfg.DLL has no function SOXpSInitialize")));
		if (!g_isService)
			MessageBox(NULL, _T("The DLL exports no function SOXpSInitialize"), _T("XPress OPC Server Tool"), MB_ICONSTOP | MB_OK | MB_DEFAULT_DESKTOP_ONLY);
		addToMessageLog(SOXPS_ERROR_NO_INITIALIZE, NULL);
		return FALSE;
	}

	g_fpSOXpSStart = (_SOXpSStart)GetProcAddress(g_dllInstance, "SOXpSStart");
	g_fpSOXpSStop = (_SOXpSStop)GetProcAddress(g_dllInstance, "SOXpSStop");

	g_fpSOXpSBuildDANameSpace = (_SOXpSBuildDANameSpace)GetProcAddress(g_dllInstance, "SOXpSBuildDANameSpace");
	g_fpSOXpSHandleDARequests = (_SOXpSHandleDARequests)GetProcAddress(g_dllInstance, "SOXpSHandleDARequests");
	g_fpSOXpSActivateDAItems = (_SOXpSActivateDAItems)GetProcAddress(g_dllInstance, "SOXpSActivateDAItems");

#ifdef SOXPS_AE
	g_fpSOXpSBuildAEAreaSpace = (_SOXpSBuildAEAreaSpace)GetProcAddress(g_dllInstance, "SOXpSBuildAEAreaSpace");
	g_fpSOXpSBuildAEEventSpace = (_SOXpSBuildAEEventSpace)GetProcAddress(g_dllInstance, "SOXpSBuildAEEventSpace");

	g_fpSOXpSActivateAESubscription = (_SOXpSActivateAESubscription)GetProcAddress(g_dllInstance, "SOXpSActivateAESubscription");
	g_fpSOXpSReceivedAEAcknowledge = (_SOXpSReceivedAEAcknowledge)GetProcAddress(g_dllInstance, "SOXpSReceivedAEAcknowledge");
	g_fpSOXpSTranslateToItemIDs = (_SOXpSTranslateToItemIDs)GetProcAddress(g_dllInstance, "SOXpSTranslateToItemIDs");
#endif
	return TRUE;
}


//-----------------------------------------------------------------------------
// _SOXpSCreateNode
//
// - create node object and add it to the DA namespace
//
// returns:
//		TRUE  - node created and added
//		FALSE - can't add node to tree
//
DWORD SOXPSCFG_CALL _SOXpSCreateNode(
	IN SOXpSItemHandle parentNodeHandle, // handle of parent node object
	IN LPCTSTR name,                     // node name
	IN VARTYPE datatype,                 // native datatype
	IN DWORD accessRights,               // access rights
	IN WORD ioMode,                      // device io mode and update mode
	IN DWORD userData,                   // user data
	OUT SOXpSItemHandle *nodeHandle)     // node handle
{
	SOCmnPointer<SOCmnElement> root;
	SOCmnPointer<SOXpSNode> node;
	DWORD ret = FALSE;

	SOCMN_TRY
	{
		SOCmnElement *parent = NULL;

		if ((!SOXPS_IS_VALID_POINTER_TO_OBJECT_OR_NULL(parentNodeHandle, SODaSNodeDefault)) ||
			(!SOXPS_IS_VALID_WRITE_POINTER_OR_NULL(nodeHandle)) )
		{
			SetLastError(ERROR_INVALID_HANDLE);
			return FALSE;
		}
		SOXPS_INIT_WRITE_POINTER(nodeHandle)
		if (!SOXPS_IS_VALID_POINTER(name))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		if (parentNodeHandle != 0)
		{
			parent = (SOCmnElement *)parentNodeHandle;
		}
		else
		{
			SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
			root = entry->getNameSpaceRoot();
			parent = (SOCmnElement *)root;
		}

		if (parent)
		{
			SOCMN_ALLOCATE_OBJECT_C(node, SOXpSNode(), (SOXpSNode *))
			node->setXpSUserData(userData);
			ret = SUCCEEDED(node->setName(name));
			if (ioMode != SOXPS_IOMODE_NOIO)
			{
				node->setDeviceIOMode((BYTE)(ioMode & 0x00FF));
				if (ioMode & 0x8000)
					node->setUpdateMode(SODaSItemTag::report);
				else
					node->setUpdateMode(SODaSItemTag::poll);
				ret &= node->setNativeDatatype(datatype);
				ret &= node->setAccessRights(accessRights);
			}
			if (ret)
			{
				ret = parent->addBranch((SODaSNode *)node);
			}
			if (!ret)
				SetLastError(ERROR_INVALID_PARAMETER);
		}
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		ret = FALSE;
	}

	if ((ret) && (nodeHandle))
		*nodeHandle = (SOXpSItemHandle)((SOCmnElement *)((SODaSNode *)node));
	else if (nodeHandle)
		*nodeHandle = NULL;

	return ret;
} // _SOXpSCreateNode


//-----------------------------------------------------------------------------
// _SOXpSCreateTag
//
// - create tag object and add it to the DA namespace
//
// returns:
//		TRUE  - tag created and added
//		FALSE - can't add tag to tree
//
DWORD SOXPSCFG_CALL _SOXpSCreateTag(
	IN SOXpSItemHandle parentNodeHandle, // handle of parent node object
	IN LPCTSTR name,                     // tag name
	IN VARTYPE datatype,                 // native datatype
	IN DWORD accessRights,               // access rights
	IN WORD ioMode,                      // device io mode and update mode
	IN DWORD userData,                   // user data
	OUT SOXpSItemHandle *tagHandle)      // tag handle
{
	SOCmnPointer<SOCmnElement> root;
	SOCmnPointer<SOXpSTag> tag;
	DWORD ret = FALSE;

	SOCMN_TRY
	{
		SOCmnElement *parent;

		if ((!SOXPS_IS_VALID_POINTER_TO_OBJECT_OR_NULL(parentNodeHandle, SODaSNodeDefault)) ||
			(!SOXPS_IS_VALID_WRITE_POINTER_OR_NULL(tagHandle)) )
		{
			SetLastError(ERROR_INVALID_HANDLE);
			return FALSE;
		}
		SOXPS_INIT_WRITE_POINTER(tagHandle)
		if (!SOXPS_IS_VALID_POINTER(name))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		if (parentNodeHandle != 0)
			parent = (SOCmnElement *)parentNodeHandle;
		else
		{
			SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
			root = entry->getNameSpaceRoot();
			parent = (SOCmnElement *)root;
		}

		if (parent)
		{
			SOCMN_ALLOCATE_OBJECT_C(tag, SOXpSTag(), (SOXpSTag *))
			tag->setXpSUserData(userData);
			ret = SUCCEEDED(tag->setName(name));
			tag->setDeviceIOMode((BYTE)(ioMode & 0x00FF));
			if (ioMode & 0x8000)
				tag->setUpdateMode(SODaSItemTag::report);
			else
				tag->setUpdateMode(SODaSItemTag::poll);
			ret &= tag->setNativeDatatype(datatype);
			ret &= tag->setAccessRights(accessRights);
			if (ret)
			{
				ret = parent->addLeaf((SODaSTag *)tag);
			}
			if (!ret)
				SetLastError(ERROR_INVALID_PARAMETER);
		}
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		ret = FALSE;
	}

	if ((ret) && (tagHandle))
		*tagHandle = (SOXpSItemHandle)((SOCmnElement *)((SODaSTag *)tag));
	else if (tagHandle)
		*tagHandle = NULL;

	return ret;
} // _SOXpSCreateTag


//-----------------------------------------------------------------------------
// _SOXpSCreateProperty
//
// - create property object and add it to the DA namespace
//
// returns:
//		TRUE  - property created and added
//		FALSE - can't add property to tree
//
DWORD SOXPSCFG_CALL _SOXpSCreateProperty(
	IN SOXpSItemHandle parentHandle,     // handle of parent object
	IN DWORD propertyID,                 // property id
	IN LPCTSTR description,              // property description
	IN VARTYPE datatype,                 // datatype
	IN DWORD accessRights,               // access rights
	IN WORD ioMode,                      // device io mode and update mode
	IN LPVARIANT constantValue,          // constant value
	IN DWORD userData,                   // user data
	OUT SOXpSItemHandle *propertyHandle) // property handle
{
	SOCmnPointer<SOXpSProperty> prop;
	DWORD ret = FALSE;

	SOCMN_TRY
	{
		if ((!SOXPS_IS_VALID_POINTER_TO_SIZE(parentHandle, __min(sizeof(SODaSNodeDefault), sizeof(SOXpSTag)))) ||
			(!SOXPS_IS_VALID_WRITE_POINTER_OR_NULL(propertyHandle)) )
		{
			SetLastError(ERROR_INVALID_HANDLE);
			return FALSE;
		}
		SOXPS_INIT_WRITE_POINTER(propertyHandle)
		if ((!SOXPS_IS_VALID_POINTER_OR_NULL(description)) || (!SOXPS_IS_VALID_POINTER_OR_NULL(constantValue)))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		SOCmnElement *parent;

		parent = (SOCmnElement *)parentHandle;
		if (parent)
		{
			SOCMN_ALLOCATE_OBJECT_C(prop, SOXpSProperty(), (SOXpSProperty *))
			prop->setXpSUserData(userData);
			ret = prop->setPropertyId(propertyID);
			if (constantValue)
			{
				ret &= prop->setNativeDatatype(constantValue->vt);
				ret &= prop->setAccessRights(OPC_READABLE);
			}
			else
			{
				prop->setDeviceIOMode((BYTE)(ioMode & 0x00FF));
				if (ioMode & 0x8000)
					prop->setUpdateMode(SODaSItemTag::report);
				else
					prop->setUpdateMode(SODaSItemTag::poll);
				ret &= prop->setNativeDatatype(datatype);
				ret &= prop->setAccessRights(accessRights);
			}
			if ((ret) && (description))
				ret = SUCCEEDED(prop->setDescription(description));
			if ((ret) && (constantValue))
			{
				SOCmnVariant constVal(constantValue);
				ret = prop->setConstantValue(constVal);
			}
			if (ret)
				ret = parent->addAdditionalElement((SODaSProperty *)prop);
			if (!ret)
				SetLastError(ERROR_INVALID_PARAMETER);
		}
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		ret = FALSE;
	}

	if ((ret) && (propertyHandle))
		*propertyHandle = (SOXpSItemHandle)((SOCmnElement *)((SODaSProperty *)prop));
	else if (propertyHandle)
		*propertyHandle = NULL;

	return ret;
} // _SOXpSCreateProperty


//-----------------------------------------------------------------------------
// _SOXpSCreateArea
//
// - create area object and add it to the AE areaspace
//
// returns:
//		TRUE  - area created and added
//		FALSE - can't add area to tree
//
DWORD SOXPSCFG_CALL _SOXpSCreateArea(
	IN SOXpSAreaHandle parentAreaHandle,// handle of parent area
	IN LPCTSTR name,					// area name
	OUT SOXpSAreaHandle *areaHandle)	// area handle
{
	SOCmnPointer<SOCmnElement> root;
	SOCmnPointer<SOAeSArea> area;
	DWORD ret = FALSE;

	SOCMN_TRY
	{
		if ((!SOXPS_IS_VALID_POINTER_TO_OBJECT_OR_NULL(parentAreaHandle, SOAeSAreaDefault)) ||
			(!SOXPS_IS_VALID_WRITE_POINTER_OR_NULL(areaHandle)) )
		{
			SetLastError(ERROR_INVALID_HANDLE);
			return FALSE;
		}
		SOXPS_INIT_WRITE_POINTER(areaHandle)
		if (!SOXPS_IS_VALID_POINTER(name))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		SOCmnElement *parent;

		if (parentAreaHandle != 0)
			parent = (SOCmnElement *)parentAreaHandle;
		else
		{
			SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
			root = entry->getAreaSpaceRoot();
			parent = (SOCmnElement *)root;
		}

		if (parent)
		{
			SOCMN_ALLOCATE_OBJECT_C(area, SOXpSArea(name), (SOAeSArea *))
			ret = parent->addBranch((SOAeSArea *)area);
			if (!ret)
				SetLastError(ERROR_INVALID_PARAMETER);
		}
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		ret = FALSE;
	}

	if ((ret) && (areaHandle))
		*areaHandle = (SOXpSAreaHandle)((SOCmnElement *)((SOAeSArea *)area));
	else if (areaHandle)
		*areaHandle = NULL;

	return ret;
} // _SOXpSCreateArea


//-----------------------------------------------------------------------------
// _SOXpSCreateSource
//
// - create source object and add it to the AE areaspace
//
// returns:
//		TRUE  - source created and added
//		FALSE - can't add source to tree
//
DWORD SOXPSCFG_CALL _SOXpSCreateSource(
	IN SOXpSAreaHandle parentAreaHandle,// handle of parent area
	IN LPCTSTR name,					// source name
	OUT SOXpSSourceHandle *sourceHandle)// source handle
{
	SOCmnPointer<SOCmnElement> root;
	SOCmnPointer<SOAeSSource> source;
	DWORD ret = FALSE;

	SOCMN_TRY
	{
		if ((!SOXPS_IS_VALID_POINTER_TO_OBJECT_OR_NULL(parentAreaHandle, SOAeSAreaDefault)) ||
			(!SOXPS_IS_VALID_WRITE_POINTER_OR_NULL(sourceHandle)) )
		{
			SetLastError(ERROR_INVALID_HANDLE);
			return FALSE;
		}
		SOXPS_INIT_WRITE_POINTER(sourceHandle)
		if (!SOXPS_IS_VALID_POINTER(name))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		SOCmnElement *parent;

		if (parentAreaHandle != 0)
			parent = (SOCmnElement *)parentAreaHandle;
		else
		{
			SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
			root = entry->getAreaSpaceRoot();
			parent = (SOCmnElement *)root;
		}

		if (parent)
		{
			SOCMN_ALLOCATE_OBJECT_C(source, SOXpSSource(name), (SOAeSSource *))
			ret = parent->addLeaf((SOAeSSource *)source);
			if (!ret)
				SetLastError(ERROR_INVALID_PARAMETER);
		}
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		ret = FALSE;
	}

	if ((ret) && (sourceHandle))
		*sourceHandle = (SOXpSSourceHandle)((SOCmnElement *)((SOAeSSource *)source));
	else if (sourceHandle)
		*sourceHandle = NULL;

	return ret;
} // _SOXpSCreateSource


//-----------------------------------------------------------------------------
// _SOXpSCreateCondition
//
// - create condition object and add it to the AE areaspace
//
// returns:
//		TRUE  - condition created and added
//		FALSE - can't add condition to tree
//
DWORD SOXPSCFG_CALL _SOXpSCreateCondition(
	IN SOXpSSourceHandle parentSourceHandle,// source handle
	IN LPCTSTR conditionClassName,			// name of condition class
	OUT SOXpSConditionHandle *condHandle)	// condition handle
{
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> root = entry->getEventSpaceRoot();
	SOCmnPointer<SOAeSConditionClass> condClass;
	SOCmnPointer<SOAeSCondition> cond;
	DWORD ret = FALSE;

	SOCMN_TRY
	{
		if ((!SOXPS_IS_VALID_POINTER_TO_OBJECT(parentSourceHandle, SOAeSSourceDefault)) ||
			(!SOXPS_IS_VALID_WRITE_POINTER_OR_NULL(condHandle)) )
		{
			SetLastError(ERROR_INVALID_HANDLE);
			return FALSE;
		}
		SOXPS_INIT_WRITE_POINTER(condHandle)
		if (!SOXPS_IS_VALID_POINTER(conditionClassName))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		SOCmnElement *parent;

		parent = (SOCmnElement *)parentSourceHandle;
		condClass = root->findConditionClass(conditionClassName);

		if ((parent) && (!!condClass))
		{
			SOCMN_ALLOCATE_OBJECT_C(cond, SOXpSCondition, (SOAeSCondition *))
			ret = cond->setConditionClass(condClass);
			if (ret)
				ret = parent->addAdditionalElement((SOAeSCondition *)cond);
			if (!ret)
				SetLastError(ERROR_INVALID_PARAMETER);
		}
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		ret = FALSE;
	}

	if ((ret) && (condHandle))
		*condHandle = (SOXpSConditionHandle)((SOCmnElement *)((SOAeSCondition *)cond));
	else if (condHandle)
		*condHandle = NULL;

	return ret;
} // _SOXpSCreateCondition


//-----------------------------------------------------------------------------
// _SOXpSCreateCategory
//
// - create category object and add it to the AE eventspace
//
// returns:
//		TRUE  - category created and added
//		FALSE - can't add category to tree
//
DWORD SOXPSCFG_CALL _SOXpSCreateCategory(
	IN DWORD eventType,						// event type
	IN DWORD categoryID,					// category id
	IN LPCTSTR description,					// category description
	OUT SOXpSCategoryHandle *categoryHandle)// category handle
{
	SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> root = entry->getEventSpaceRoot();
	SOCmnPointer<SOAeSCategory> category;
	DWORD ret = FALSE;

	SOCMN_TRY
	{
		if (!SOXPS_IS_VALID_WRITE_POINTER_OR_NULL(categoryHandle))
		{
			SetLastError(ERROR_INVALID_HANDLE);
			return FALSE;
		}
		SOXPS_INIT_WRITE_POINTER(categoryHandle)
		if (!SOXPS_IS_VALID_POINTER(description))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		SOCMN_ALLOCATE_OBJECT_C(category, SOAeSCategory(categoryID, description, eventType), (SOAeSCategory *))
		ret = root->addBranch((SOAeSCategory *)category);
		if (!ret)
			SetLastError(ERROR_INVALID_PARAMETER);
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		ret = FALSE;
	}

	if ((ret) && (categoryHandle))
		*categoryHandle = (SOXpSCategoryHandle)((SOCmnElement *)((SOAeSCategory *)category));
	else if (categoryHandle)
		*categoryHandle = NULL;

	return ret;
} // _SOXpSCreateCategory


//-----------------------------------------------------------------------------
// _SOXpSCreateAttribute
//
// - create attribute object and add it to the AE eventspace
//
// returns:
//		TRUE  - attribute created and added
//		FALSE - can't add attribute to tree
//
DWORD SOXPSCFG_CALL _SOXpSCreateAttribute(
	IN SOXpSCategoryHandle categoryHandle,	// category handle
	IN DWORD attributeID,					// attribute id
	IN LPCTSTR description,					// attribute description
	IN VARTYPE datatype)					// attribute datatype
{
	SOCmnPointer<SOAeSAttribute> attr;
	DWORD ret = FALSE;

	SOCMN_TRY
	{
		if (!SOXPS_IS_VALID_POINTER_TO_OBJECT(categoryHandle, SOAeSCategory))
		{
			SetLastError(ERROR_INVALID_HANDLE);
			return FALSE;
		}
		if (!SOXPS_IS_VALID_POINTER(description))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		SOAeSCategory *category = (SOAeSCategory *)categoryHandle;
		SOCMN_ALLOCATE_OBJECT_C(attr, SOAeSAttribute(attributeID, description, datatype), (SOAeSAttribute *))
		ret = category->addAdditionalElement((SOAeSAttribute *)attr);
		if (!ret)
			SetLastError(ERROR_INVALID_PARAMETER);
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		ret = FALSE;
	}

	return ret;
} // _SOXpSCreateAttribute


//-----------------------------------------------------------------------------
// _SOXpSCreateConditionClass
//
// - create condition class object and add it to the AE eventspace
//
// returns:
//		TRUE  - condition class created and added
//		FALSE - can't add condition class to tree
//
DWORD SOXPSCFG_CALL _SOXpSCreateConditionClass(
	IN SOXpSCategoryHandle categoryHandle,			// category handle
	IN LPCTSTR conditionClassName,					// condition class name
	IN LPCTSTR inactiveSubCondMessage,				// message of inactive sub condition
	IN DWORD inactiveSubCondAckRequired,			// ack req of inactive sub condition
	IN DWORD singleStateConditionClass,				// single state ?
	IN LPCTSTR definition,							// definition of single state condition
	IN DWORD activeSubCondSeverity,					// severity of active single state condition
	IN LPCTSTR activeSubCondMessage,				// message of active single state condition
	IN DWORD activeSubCondAckRequired,				// ack req of active single state condition
	OUT SOXpSConditionClassHandle *condClassHandle) // condition class handle
{
	SOCmnPointer<SOAeSConditionClass> condClass;
	DWORD ret = FALSE;

	SOCMN_TRY
	{
		if ((!SOXPS_IS_VALID_POINTER_TO_OBJECT(categoryHandle, SOAeSCategory)) ||
			(!SOXPS_IS_VALID_WRITE_POINTER_OR_NULL(condClassHandle)) )
		{
			SetLastError(ERROR_INVALID_HANDLE);
			return FALSE;
		}
		SOXPS_INIT_WRITE_POINTER(condClassHandle)
		if ( (!SOXPS_IS_VALID_POINTER(conditionClassName)) ||
			 (!SOXPS_IS_VALID_POINTER_OR_NULL(inactiveSubCondMessage)) )
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		SOAeSCategory *category = (SOAeSCategory *)categoryHandle;

		if (singleStateConditionClass)
		{
			if ( (!SOXPS_IS_VALID_POINTER_OR_NULL(definition)) ||
				 (!SOXPS_IS_VALID_POINTER_OR_NULL(activeSubCondMessage)) )
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return FALSE;
			}

			SOCMN_ALLOCATE_OBJECT_C(condClass, SOXpSConditionClassSingleState(), (SOAeSConditionClass *))
			SOXpSConditionClassSingleState *singleCondClass = (SOXpSConditionClassSingleState *)((SOAeSConditionClass *) condClass);
			ret = SUCCEEDED(condClass->setName(conditionClassName));
			ret &= singleCondClass->setDefinition(definition);
			ret &= singleCondClass->setActiveMessage(activeSubCondMessage);
			ret &= singleCondClass->setActiveSeverity(activeSubCondSeverity);
			ret &= singleCondClass->setActiveAckRequired(activeSubCondAckRequired);
			ret &= singleCondClass->setInactiveMessage(inactiveSubCondMessage);
			ret &= singleCondClass->setInactiveAckRequired(inactiveSubCondAckRequired);
		}
		else
		{
			SOCMN_ALLOCATE_OBJECT_C(condClass, SOXpSConditionClassMultipleState(), (SOAeSConditionClass *))
			SOXpSConditionClassMultipleState *multipleCondClass = (SOXpSConditionClassMultipleState *)((SOAeSConditionClass *) condClass);
			ret = SUCCEEDED(condClass->setName(conditionClassName));
			ret &= multipleCondClass->setInactiveMessage(inactiveSubCondMessage);
			ret &= multipleCondClass->setInactiveAckRequired(inactiveSubCondAckRequired);
		}

		if (ret)
			ret = category->addBranch((SOAeSConditionClass *)condClass);

		if (!ret)
			SetLastError(ERROR_INVALID_PARAMETER);
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		ret = FALSE;
	}

	if ((ret) && (condClassHandle))
		*condClassHandle = (SOXpSConditionClassHandle)((SOCmnElement *)((SOAeSConditionClass *)condClass));
	else if (condClassHandle)
		*condClassHandle = NULL;

	return ret;
} // _SOXpSCreateConditionClass


//-----------------------------------------------------------------------------
// _SOXpSCreateSubConditionClass
//
// - create sub condition class object and add it to the AE eventspace
//
// returns:
//		TRUE  - sub condition class created and added
//		FALSE - can't add sub condition class to tree
//
DWORD SOXPSCFG_CALL _SOXpSCreateSubConditionClass(
	IN SOXpSConditionClassHandle conditionClassHandle,	// condition class handle
	IN LPCTSTR name,									// sub condition name
	IN DWORD severity,									// sub condition severity
	IN LPCTSTR description,								// sub condition description
	IN DWORD ackRequired, 								// sub condition ack required
	IN LPCTSTR definition)								// sub condition definition
{
	SOCmnPointer<SOAeSSubConditionClass> subCondClass;
	DWORD ret = FALSE;

	SOCMN_TRY
	{
		if (!SOXPS_IS_VALID_POINTER_TO_SIZE(conditionClassHandle, __min(sizeof(SOXpSConditionClassSingleState), sizeof(SOXpSConditionClassMultipleState))))
		{
			SetLastError(ERROR_INVALID_HANDLE);
			return FALSE;
		}
		if ((!SOXPS_IS_VALID_POINTER(name)) || (!SOXPS_IS_VALID_POINTER(description)) || (!SOXPS_IS_VALID_POINTER(definition)))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		SOAeSConditionClass *condClass = (SOAeSConditionClass *)conditionClassHandle;
		SOCMN_ALLOCATE_OBJECT_C(subCondClass, SOAeSSubConditionClass(name, severity, description, ackRequired, definition), (SOAeSSubConditionClass *));
		ret = condClass->addLeaf((SOAeSSubConditionClass *)subCondClass);
		if (!ret)
			SetLastError(ERROR_INVALID_PARAMETER);
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		ret = FALSE;
	}

	return ret;
} // _SOXpSCreateSubConditionClass


//-----------------------------------------------------------------------------
// _SOXpSFireAEEvents
//
// - fire some events
//
DWORD SOXPSCFG_CALL _SOXpSFireAEEvents(
	IN DWORD numEvents,					// number of events
	IN SOXpSAEEventData *eventArray)	// event data array
{
	SOCmnList<SOAeSEvent> eventList;
	SOCmnPointer<SOAeSEvent> event;
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnPointer<SOAeSCreator> creator = entry->getCreator();
	SOCmnPointer<SOAeSCategory> cat;
	SOCmnList<SOAeSAttributeValue> attrValList;
	SOCmnList<SOAeSAttribute> attrList;
	SOCmnPointer<SOAeSAttributeValue> attrVal;
	SOAeSAttribute *attr;
	SOCmnListPosition pos;
	DWORD i;
	DWORD j;
	DWORD ret = TRUE;
	SOCmnSingleLock<SOCmnSync> lock(&g_lockFireAEEvents);

	SOCMN_TRY
	{
		if (!SOXPS_IS_VALID_ARRAY(eventArray, numEvents))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		eventList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);

		for (i = 0; i < numEvents; i++)
		{
			event = NULL;
			if (eventArray[i].m_evtType == SOXPS_EVENTTYPE_SIMPLE)	{
				event = SOAeSEvent::createSimpleEvent(eventArray[i].m_eventCategory,
										eventArray[i].m_sourcePath, eventArray[i].m_severity,
										eventArray[i].m_message);
			}
			else if (eventArray[i].m_evtType == SOXPS_EVENTTYPE_TRACKING)	{
				event = SOAeSEvent::createTrackingEvent(eventArray[i].m_eventCategory,
										eventArray[i].m_sourcePath, eventArray[i].m_severity,
										eventArray[i].m_message, eventArray[i].m_actorID );
			}

			if (!!event)
			{
				cat = event->getCategory();
				attrList = (SOCmnList<SOAeSAttribute> *)cat->getAdditionalList();
				attrValList = event->getAttributeValueList();

				if ((eventArray[i].m_occurenceTime.dwLowDateTime != 0) ||
					(eventArray[i].m_occurenceTime.dwHighDateTime != 0))
					event->setOccurrenceTime(&eventArray[i].m_occurenceTime);

				pos = attrList.getStartPosition();
				j = 0;
				while (pos)
				{
					attr = attrList.getNext(pos);
					attrVal = creator->createAttributeValue();

					// initialize attribute value
					attrVal->setAttribute(attr);
					attrVal->setValue(&(eventArray[i].m_eventAttrArray[j]));

					// add attribute value
					attrValList.add(attrVal);

					j++;
				}
				attrValList.autoUnlock();

				eventList.add((SOAeSEvent *)event);
			}
			else
			{
				SetLastError(ERROR_INVALID_DATA);
				ret = FALSE;
			}
		}

		SOAeSEvent::fireEvents(&eventList);
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		return FALSE;
	}
	return ret;
} // _SOXpSFireAEEvents


//-----------------------------------------------------------------------------
// _SOXpSCompletedDARequests
//
// - complete some requests
//
DWORD SOXPSCFG_CALL _SOXpSCompletedDARequests(
	IN DWORD numRequests,					// number of requests
	IN SOXpSDARequestData *requestArray)	// request array
{
	DWORD ret = FALSE;
	SOCmnSingleLock<SOCmnSync> lock(&g_lockCompletedDARequests);

	SOCMN_TRY
	{
		if (!SOXPS_IS_VALID_ARRAY(requestArray, numRequests))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		ret = processRequestData(numRequests, requestArray);
		if (!!g_requestThreadWriteQueue)
			removeHandledRequests(&g_requestThreadWriteQueue);
		if (!!g_requestThreadReadQueue)
			removeHandledRequests(&g_requestThreadReadQueue);
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		return FALSE;
	}
	return ret;
} // _SOXpSCompletedDARequests


//-----------------------------------------------------------------------------
// _SOXpSChangeAEConditions
//
// - change some conditions
//
DWORD SOXPSCFG_CALL _SOXpSChangeAEConditions(
	IN DWORD numConditions,				// number of conditions
	IN SOXpSAEConditionData *condArray)	// condition array
{
	SOCmnPointer<SOAeSEntry> entry = ::getSOAeSEntry();
	SOCmnPointer<SOAeSCreator> creator = entry->getCreator();
	SOCmnPointer<SOAeSAreaSpaceRoot> areaRoot;
	SOCmnPointer<SOXpSCondition> cond;
	SOCmnPointer<SOAeSSource> src;
	DWORD i, j;
	DWORD ret = TRUE;
	DWORD prevRet;
	SOCmnSingleLock<SOCmnSync> lock(&g_lockChangeAEConditions);
	WORD changeMask;
	FILETIME now;

	CoFileTimeNow(&now);

	SOCMN_TRY
	{
		if (!SOXPS_IS_VALID_ARRAY(condArray, numConditions))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		areaRoot = entry->getAreaSpaceRoot();

		for (i = 0; i < numConditions; i++)
		{
			if (condArray[i].m_condHandle)
			{
				cond = (SOXpSCondition *)condArray[i].m_condHandle;
				cond.addRef();
			}
			else if ((condArray[i].m_sourcePath) && (condArray[i].m_condName))
			{
				src = (SOAeSSource *)areaRoot->findLeaf(condArray[i].m_sourcePath, TRUE);
				cond = (SOXpSCondition *)src->findAdditionalElement(condArray[i].m_condName);
			}

			if (!!cond)
			{
				changeMask = 0;
				cond->setQuality(condArray[i].m_quality);

				if (condArray[i].m_ackID)
				{
					cond->setAckID(condArray[i].m_ackID);
				}
				else
				{

					SOCmnDateTime dt; dt.now();
					SOCmnString szDt;
					cond->setAckID(dt.toString(szDt));
				}

				if (condArray[i].m_mask)
				{
					cond->enableInstanceData((BYTE)condArray[i].m_mask);
					if (condArray[i].m_mask & SOXPS_CONDITIONDATA_ACKREQUIRED)
						cond->setAckRequired(condArray[i].m_ackRequired);
					if (condArray[i].m_mask & SOXPS_CONDITIONDATA_MESSAGE)
						cond->setMessage(condArray[i].m_message);
					if (condArray[i].m_mask & SOXPS_CONDITIONDATA_SEVERITY)
						cond->setSeverity(condArray[i].m_severity);
				}
				else
					cond->enableInstanceData(FALSE);

				if (condArray[i].m_condAttrArray)
				{
					SOCmnPointer<SOAeSCategory> cat;
					SOCmnList<SOAeSAttributeValue> attrValList;
					SOCmnList<SOAeSAttribute> attrList;
					SOCmnPointer<SOAeSAttributeValue> attrVal;
					SOAeSAttribute *attr;
					SOCmnListPosition pos;

					cat = cond->getCategory();
					attrList = (SOCmnList<SOAeSAttribute> *)cat->getAdditionalList();
					attrValList = cond->getAttributeValueList();

					attrValList.removeAll();
					pos = attrList.getStartPosition();
					j = 0;
					while (pos)
					{
						attr = attrList.getNext(pos);
						attrVal = creator->createAttributeValue();

						// initialize attribute value
						attrVal->setAttribute(attr);
						attrVal->setValue(&(condArray[i].m_condAttrArray[j]));

						// add attribute value
						attrValList.add(attrVal);

						j++;
					}
					attrValList.autoUnlock();

					changeMask |= OPC_CHANGE_ATTRIBUTE;
				}

				if (condArray[i].m_acknowledge != SOXPS_ACKNOWLEDGE_NO_CHANGE)
				{
					WORD state = cond->getObjectState();

					if (condArray[i].m_acknowledge == SOXPS_ACKNOWLEDGE_ACK)
					{
						if ((state & OPC_CONDITION_ACKED) == 0)
						{ // add acknowledge
							cond->setObjectState((BYTE)(state | OPC_CONDITION_ACKED));
							changeMask |= OPC_CHANGE_ACK_STATE;
						}
					}
					else if (condArray[i].m_acknowledge == SOXPS_ACKNOWLEDGE_NACK)
					{
						if ((cond->getObjectState() & OPC_CONDITION_ACKED) == OPC_CONDITION_ACKED)
						{	// remove acknowledge
							cond->setObjectState((BYTE)(state & ~OPC_CONDITION_ACKED));
							changeMask |= OPC_CHANGE_ACK_STATE;
						}
					}
				}

				prevRet = ret;
				if (condArray[i].m_activate == FALSE)
					ret &= cond->activate(FALSE, &now, changeMask);
				else
				{
					SOCmnPointer<SOAeSConditionClass> condClass = cond->getConditionClass();

					if (condClass->isSingleStateCondition())
						ret &= cond->activate(condArray[i].m_activate, &now, changeMask);
					else
					{
						if (condArray[i].m_actSubCond)
							ret &= cond->activate(condArray[i].m_actSubCond, &now, changeMask);
						else
							ret = FALSE;
					}
				}
				if (prevRet != ret)
					SetLastError(ERROR_INVALID_DATA);
			}
			else
			{
				ret = FALSE;
				SetLastError(ERROR_PATH_NOT_FOUND);
			}
		}
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		return FALSE;
	}

	return ret;
} // _SOXpSChangeAEConditions


//-----------------------------------------------------------------------------
// _SOXpSTranslatedItemID
//
// - add translated item id to translation list
//
DWORD SOXPSCFG_CALL _SOXpSTranslatedItemID(
	IN DWORD attributeID,
	IN LPCTSTR itemID,
	IN LPCTSTR nodeName,
	IN CLSID *classID)
{
	SOCMN_TRY
	{
		if ((!SOXPS_IS_VALID_POINTER(itemID)) || (!SOXPS_IS_VALID_POINTER(classID)))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		if (g_translateToItemIDList)
		{
			SOAeSAttributeValueDADescription *descr;
			SOCmnListPosition pos;

			pos = g_translateToItemIDList->getStartPosition();
			while (pos)
			{
				descr = g_translateToItemIDList->getNext(pos);
				if (descr->m_attrID == attributeID)
				{
					descr->m_itemID = itemID;
					descr->m_nodeName = nodeName;
					memcpy(&descr->m_clsid, classID, sizeof(CLSID));
					return TRUE;
				}
			}
		}
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		return FALSE;
	}

	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
}


//-----------------------------------------------------------------------------
// _SOXpSShutdown
//
// - shutdown request to all clients
//
DWORD SOXPSCFG_CALL _SOXpSShutdown(
	IN LPCTSTR reason) // shutdown reason
{
	SOCMN_TRY
	{
		if ((!SOXPS_IS_VALID_POINTER(reason)))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		{
		SOCmnPointer<SOAsSEntry> entry = getSOAsSEntry();
		entry->revokeClassObjects();
		}
		g_pShutdownReason = new SOCmnString(reason);
#ifdef SOXPS_SERVICE
		serviceControlHandler(SERVICE_CONTROL_STOP);
#endif
#ifdef SOXPS_SERVER
		PostMessage(g_hWnd, WM_CLOSE, 0, 0);
#endif
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		return FALSE;
	}

	return TRUE;
}


DWORD SOXPSCFG_CALL _SOXpSSetDeviceState(IN SOXpSDeviceHandle deviceHandle, IN LPCTSTR devState, IN LPCTSTR devStateInfo)
{
	SOCMN_TRY
	{
		if (!SOXPS_IS_VALID_POINTER_TO_OBJECT(deviceHandle, SOCmnElement))
		{
			SetLastError(ERROR_INVALID_HANDLE);
			return FALSE;
		}

		if ((!SOXPS_IS_VALID_POINTER(devState)) || (!SOXPS_IS_VALID_POINTER(devStateInfo)))
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		SOCmnElement *device = (SOCmnElement *)deviceHandle;
		switch (device->getObjectType())
		{
			case SOCMNOBJECT_TYPE_NODE:
				((SOXpSNode *)device)->setDeviceState(devState, devStateInfo);
			break;
			case SOCMNOBJECT_TYPE_TAG:
				((SOXpSTag *)device)->setDeviceState(devState, devStateInfo);
			break;
			case SOCMNOBJECT_TYPE_PROPERTY:
				((SOXpSProperty *)device)->setDeviceState(devState, devStateInfo);
			break;
			case SOCMNOBJECT_TYPE_AREA:
				((SOXpSArea *)device)->setDeviceState(devState, devStateInfo);
			break;
			case SOCMNOBJECT_TYPE_SOURCE:
				((SOXpSSource *)device)->setDeviceState(devState, devStateInfo);
			break;
			case SOCMNOBJECT_TYPE_CONDITION:
				((SOXpSCondition *)device)->setDeviceState(devState, devStateInfo);
			break;
			default:
				SetLastError(ERROR_INVALID_HANDLE);
				return FALSE;
		}

		if (device->getCollectChanges())
		{
			SOSrvWatch::attributeChange(device, SOWATCH_ATTRIBUTE_DEVSTATE_T, devState);
			SOSrvWatch::attributeChange(device, SOWATCH_ATTRIBUTE_DEVSTATEINFO_T, devStateInfo);
		}
	}
	SOCMN_CATCH(...)
	{
		SetLastError(ERROR_EXCEPTION_IN_SERVICE);
		return FALSE;
	}

	return TRUE;
}

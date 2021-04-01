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
//                           OPC Toolkit - Demo                               |
//                                                                            |
//  Filename    : SODmCGroup.cpp                                              |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : DA group classes                                            |
//                - SODmCComGroup: Toolkit group object class                 |
//                - SODmCPPGroup: Group property page                         |
//                                                                            |
//  This code and information is provided "as is" without warranty of         |
//  any kind, either expressed or implied, including but not limited          |
//  to the implied warranties of merchantability and/or fitness for a         |
//  particular purpose.                                                       |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "SOClient.h"
#include "SODmCServerDA.h"
#include "SODmCGroup.h"
#include "SODmCItem.h"
#include "SODmCPropFrm.h"
#include "SODmCObjectView.h"
#include "SODmCMainView.h"


#ifdef SODMC_DCOM

//-----------------------------------------------------------------------------
// SODmCComGroup                                                                 |
//-----------------------------------------------------------------------------

SODmCComGroup::SODmCComGroup()
{
	m_treeHandle = 0;
}

//-----------------------------------------------------------------------------
// onSetParentBranch
//
// - called after toolkit object is insertet into the tree
//
void SODmCComGroup::onSetParentBranch()
{
	SODmCObjectView* objview = getObjectView();

	if (!objview)
	{
		return;
	}

	if (m_parent)
	{
		SODmCCmnServerDA* pCmnSrv = NULL;
		pCmnSrv = (SODmCCmnServerDA*)(SODmCComServerDA*)m_parent;
		m_treeHandle = SODmCTreeInsert(&objview->GetTreeCtrl(), pCmnSrv->getTreeHandle(), getName(), FALSE, (SOCltElement*)this, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, TVI_LAST);

		if (IsWindow(objview->GetTreeCtrl().m_hWnd))
		{
			objview->GetTreeCtrl().Expand(pCmnSrv->getTreeHandle(), TVE_EXPAND);
		}
	}
	else
	{
		if (m_treeHandle)
		{
			objview->GetTreeCtrl().DeleteItem(m_treeHandle);
			m_treeHandle = 0;
			getPropertyFrame()->removedObject(this);
		}
	}
} // onSetParentBranch


//-----------------------------------------------------------------------------
// onError
//
// - called after an error occured
//
void SODmCComGroup::onError(IN HRESULT res, IN DWORD errorcode)
{
	SODmCMainView* mainview = getMainView();
	SOCmnString et;
	getErrorDescription(errorcode, et, 0x0800);
	mainview->insertError((SOCltElement*)this, res, errorcode, et);
} // onError


//-----------------------------------------------------------------------------
// onSetName
//
// - called after the name of the group changed
//
void SODmCComGroup::onSetName(void)
{
	SOCmnString name = getName();

	if (m_treeHandle)
	{
		SODmCObjectView* objview = getObjectView();
		objview->GetTreeCtrl().SetItemText(m_treeHandle, name);
	}


	// update the group name in the main views item list
	SOCmnList<SODmCItem> itemList = (SOCmnList<SODmCItem> *)getLeafList();
	SOCmnListPosition pos = itemList.getStartPosition();
	SODmCItem* item;

	while (pos)
	{
		item = itemList.getNext(pos);

		if (item->getListIndex() != -1)
		{
			SODmCMainView* mainview = getMainView();
			mainview->getListDA()->SetItemText(item->getListIndex(), 6, name);
		}
	}


	SODmCPropertyFrame* pPropertyFrame = getPropertyFrame();
	
	if (pPropertyFrame != NULL)
	{
		SODmCPSheet* pPropertySheet = pPropertyFrame->getPropertySheet();

		if (pPropertySheet != NULL)
		{
			pPropertySheet->setName(this, name);
		}		
	}

} // onSetName


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - called after the state of the object changed
//
void SODmCComGroup::onSetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setObjectState((SOCmnObject*)this, getObjectState());
} // onSetObjectState


//-----------------------------------------------------------------------------
// onSetTargetObjectState
//
// - called after the state of the target object changed
//
void SODmCComGroup::onSetTargetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setTargetObjectState((SOCmnObject*)this, getTargetObjectState());
} // onSetTargetObjectState


//-----------------------------------------------------------------------------
// beforeInterfaceCall
//
// - called before an COM interface call
//
void SODmCComGroup::beforeInterfaceCall(void)
{
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->addToWorkingList(this);
	}

	getApp()->BeginWaitCursor();
} // beforeInterfaceCall


//-----------------------------------------------------------------------------
//afterInterfaceCall
//
// - called after an COM interface call
//
void SODmCComGroup::afterInterfaceCall(void)
{
	getApp()->EndWaitCursor();
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->removeFromWorkingList(this);
		INVOKE_CALLBACK(SODmCComGroup, onSetTargetObjectState);
	}
} // afterInterfaceCall

#endif // SODMC_DCOM

#ifdef SODMC_SOAP

//-----------------------------------------------------------------------------
// SODmCXmlGroup                                                                 |
//-----------------------------------------------------------------------------

SODmCXmlGroup::SODmCXmlGroup()
{
	m_treeHandle = 0;
}

//-----------------------------------------------------------------------------
// onSetParentBranch
//
// - called after toolkit object is insertet into the tree
//
void SODmCXmlGroup::onSetParentBranch()
{
	SODmCObjectView* objview = getObjectView();

	if (!objview)
	{
		return;
	}

	if (m_parent)
	{
		SODmCCmnServerDA* pCmnSrv = NULL;
		pCmnSrv = (SODmCCmnServerDA*)(SODmCXmlServerDA*)m_parent;
		m_treeHandle = SODmCTreeInsert(&objview->GetTreeCtrl(), pCmnSrv->getTreeHandle(), getName(), FALSE, (SOCltElement*)this, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, TVI_LAST);

		if (IsWindow(objview->GetTreeCtrl().m_hWnd))
		{
			objview->GetTreeCtrl().Expand(pCmnSrv->getTreeHandle(), TVE_EXPAND);
		}
	}
	else
	{
		if (m_treeHandle)
		{
			objview->GetTreeCtrl().DeleteItem(m_treeHandle);
			m_treeHandle = 0;
			getPropertyFrame()->removedObject(this);
		}
	}
} // onSetParentBranch


//-----------------------------------------------------------------------------
// onError
//
// - called after an error occured
//
void SODmCXmlGroup::onError(IN HRESULT res, IN DWORD errorcode)
{
	SODmCMainView* mainview = getMainView();
	SOCmnString et;
	getErrorDescription(errorcode, et, 0x0800);
	mainview->insertError((SOCltElement*)this, res, errorcode, et);
} // onError


//-----------------------------------------------------------------------------
// onSetName
//
// - called after the name of the group changed
//
void SODmCXmlGroup::onSetName(void)
{
	if (m_treeHandle)
	{
		SODmCObjectView* objview = getObjectView();
		objview->GetTreeCtrl().SetItemText(m_treeHandle, getName());
	}

	// update the group name in the main views item list
	SOCmnList<SODmCItem> itemList = (SOCmnList<SODmCItem> *)getLeafList();
	SOCmnListPosition pos = itemList.getStartPosition();
	SODmCItem* item;

	while (pos)
	{
		item = itemList.getNext(pos);

		if (item->getListIndex() != -1)
		{
			SODmCMainView* mainview = getMainView();
			mainview->getListDA()->SetItemText(item->getListIndex(), 6, getName());
		}
	}

	getPropertyFrame()->getPropertySheet()->setName(this, getName());
} // onSetName


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - called after the state of the object changed
//
void SODmCXmlGroup::onSetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setObjectState((SOCmnObject*)this, getObjectState());
} // onSetObjectState


//-----------------------------------------------------------------------------
// onSetTargetObjectState
//
// - called after the state of the target object changed
//
void SODmCXmlGroup::onSetTargetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setTargetObjectState((SOCmnObject*)this, getTargetObjectState());
} // onSetTargetObjectState


//-----------------------------------------------------------------------------
// beforeInterfaceCall
//
// - called before an COM interface call
//
void SODmCXmlGroup::beforeInterfaceCall(void)
{
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->addToWorkingList(this);
	}

	getApp()->BeginWaitCursor();
} // beforeInterfaceCall


//-----------------------------------------------------------------------------
//afterInterfaceCall
//
// - called after an COM interface call
//
void SODmCXmlGroup::afterInterfaceCall(void)
{
	getApp()->EndWaitCursor();
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->removeFromWorkingList(this);
		INVOKE_CALLBACK(SODmCXmlGroup, onSetTargetObjectState);
	}
} // afterInterfaceCall

#endif // SODMC_SOAP

#ifdef SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// SODmCTPGroup                                                                 |
//-----------------------------------------------------------------------------

SODmCTPGroup::SODmCTPGroup()
{
	m_treeHandle = 0;
}

//-----------------------------------------------------------------------------
// onSetParentBranch
//
// - called after toolkit object is insertet into the tree
//
void SODmCTPGroup::onSetParentBranch()
{
	SODmCObjectView* objview = getObjectView();

	if (!objview)
	{
		return;
	}

	if (m_parent)
	{
		SODmCCmnServerDA* pCmnSrv = NULL;
		pCmnSrv = (SODmCCmnServerDA*)(SODmCTPServerDA*)m_parent;
		m_treeHandle = SODmCTreeInsert(&objview->GetTreeCtrl(), pCmnSrv->getTreeHandle(), getName(), FALSE, (SOCltElement*)this, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, TVI_LAST);

		if (IsWindow(objview->GetTreeCtrl().m_hWnd))
		{
			objview->GetTreeCtrl().Expand(pCmnSrv->getTreeHandle(), TVE_EXPAND);
		}
	}
	else
	{
		if (m_treeHandle)
		{
			objview->GetTreeCtrl().DeleteItem(m_treeHandle);
			m_treeHandle = 0;
			getPropertyFrame()->removedObject(this);
		}
	}
} // onSetParentBranch


//-----------------------------------------------------------------------------
// onError
//
// - called after an error occured
//
void SODmCTPGroup::onError(IN HRESULT res, IN DWORD errorcode)
{
	SODmCMainView* mainview = getMainView();
	SOCmnString et;
	getErrorDescription(errorcode, et, 0x0800);
	mainview->insertError((SOCltElement*)this, res, errorcode, et);
} // onError


//-----------------------------------------------------------------------------
// onSetName
//
// - called after the name of the group changed
//
void SODmCTPGroup::onSetName(void)
{

	if (m_treeHandle)
	{
		SODmCObjectView* objview = getObjectView();
		objview->GetTreeCtrl().SetItemText(m_treeHandle, getName());
	}

	// update the group name in the main views item list
	SOCmnList<SODmCItem> itemList = (SOCmnList<SODmCItem> *)getLeafList();
	SOCmnListPosition pos = itemList.getStartPosition();
	SODmCItem* item;

	while (pos)
	{
		item = itemList.getNext(pos);

		if (item->getListIndex() != -1)
		{
			SODmCMainView* mainview = getMainView();
			mainview->getListDA()->SetItemText(item->getListIndex(), 6, getName());
		}
	}

	getPropertyFrame()->getPropertySheet()->setName(this, getName());
} // onSetName


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - called after the state of the object changed
//
void SODmCTPGroup::onSetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setObjectState((SOCmnObject*)this, getObjectState());
} // onSetObjectState


//-----------------------------------------------------------------------------
// onSetTargetObjectState
//
// - called after the state of the target object changed
//
void SODmCTPGroup::onSetTargetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setTargetObjectState((SOCmnObject*)this, getTargetObjectState());
} // onSetTargetObjectState


//-----------------------------------------------------------------------------
// beforeInterfaceCall
//
// - called before an COM interface call
//
void SODmCTPGroup::beforeInterfaceCall(void)
{
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->addToWorkingList(this);
	}

	getApp()->BeginWaitCursor();
} // beforeInterfaceCall


//-----------------------------------------------------------------------------
//afterInterfaceCall
//
// - called after an COM interface call
//
void SODmCTPGroup::afterInterfaceCall(void)
{
	getApp()->EndWaitCursor();
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->removeFromWorkingList(this);
		INVOKE_CALLBACK(SODmCTPGroup, onSetTargetObjectState);
	}
} // afterInterfaceCall

#endif // SOFEATURE_TUNNEL


//-----------------------------------------------------------------------------
// SODmCPPGroup                                                               |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPGroup, SODmCPPage)

BEGIN_MESSAGE_MAP(SODmCPPGroup, SODmCPPage)
	ON_BN_CLICKED(IDC_GROUP_VALID_UPDATE_RATE, onClickUpdateRate)
	ON_BN_CLICKED(IDC_GROUP_VALID_TIME_BIAS, onClickTimeBias)
	ON_BN_CLICKED(IDC_GROUP_VALID_DEADBAND, onClickDeadband)
	ON_BN_CLICKED(IDC_GROUP_VALID_LOCALE_ID, onClickLocaleID)
	ON_BN_CLICKED(IDC_GROUP_VALID_KEEP_ALIVE_TIME, onClickKeepAliveTime)
	ON_CBN_SELCHANGE(IDC_GROUP_CONNECTION_TYPE, onChangeConnectionType)
END_MESSAGE_MAP()

SODmCPPGroup::SODmCPPGroup(IN SODmCPSheet* sheet)
	: SODmCPPage(sheet, IDD_PP_GROUP)
{
	m_obj = NULL;
	m_validUpdateRate = FALSE;
	m_validTimeBias = FALSE;
	m_validDeadband = FALSE;
	m_validLocaleID = FALSE;
	m_validKeepAliveTime = FALSE;
	m_public = FALSE;
	m_preventLoadAttributes = FALSE;
	m_edName.m_firstControl = TRUE;
	m_edClientHandle.m_lastControl = TRUE;
}

SODmCPPGroup::~SODmCPPGroup()
{
}


//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPGroup::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_GROUP_NAME, m_groupName);
	DDX_Control(pDX, IDC_GROUP_NAME, m_edName);
	DDX_CBString(pDX, IDC_GROUP_CONNECTION_TYPE, m_conType);
	DDX_Control(pDX, IDC_GROUP_CONNECTION_TYPE, m_cbConType);
	DDX_CBString(pDX, IDC_GROUP_DATA_RETRIEVAL, m_dataRet);
	DDX_Control(pDX, IDC_GROUP_DATA_RETRIEVAL, m_cbDataRet);
	DDX_Text(pDX, IDC_GROUP_UPDATE_RATE, m_updateRate);
	DDX_Control(pDX, IDC_GROUP_UPDATE_RATE, m_edUpdateRate);
	DDX_Check(pDX, IDC_GROUP_VALID_UPDATE_RATE, m_validUpdateRate);
	DDX_Control(pDX, IDC_GROUP_VALID_UPDATE_RATE, m_buValidUpdateRate);
	DDX_Text(pDX, IDC_GROUP_KEEP_ALIVE_TIME, m_keepAliveTime);
	DDX_Control(pDX, IDC_GROUP_KEEP_ALIVE_TIME, m_edKeepAliveTime);
	DDX_Check(pDX, IDC_GROUP_VALID_KEEP_ALIVE_TIME, m_validKeepAliveTime);
	DDX_Control(pDX, IDC_GROUP_VALID_KEEP_ALIVE_TIME, m_buValidKeepAliveTime);
	DDX_Text(pDX, IDC_GROUP_TIME_BIAS, m_timeBias);
	DDX_Control(pDX, IDC_GROUP_TIME_BIAS, m_edTimeBias);
	DDX_Check(pDX, IDC_GROUP_VALID_TIME_BIAS, m_validTimeBias);
	DDX_Control(pDX, IDC_GROUP_VALID_TIME_BIAS, m_buValidTimeBias);
	DDX_Text(pDX, IDC_GROUP_DEADBAND, m_deadband);
	DDX_Control(pDX, IDC_GROUP_DEADBAND, m_edDeadband);
	DDX_Check(pDX, IDC_GROUP_VALID_DEADBAND, m_validDeadband);
	DDX_Control(pDX, IDC_GROUP_VALID_DEADBAND, m_buValidDeadband);
	DDX_CBString(pDX, IDC_GROUP_LOCALE_ID, m_localeID);
	DDX_Control(pDX, IDC_GROUP_LOCALE_ID, m_cbLocaleID);
	DDX_Check(pDX, IDC_GROUP_VALID_LOCALE_ID, m_validLocaleID);
	DDX_Control(pDX, IDC_GROUP_VALID_LOCALE_ID, m_buValidLocaleID);
	DDX_Check(pDX, IDC_PUBLIC, m_public);
	DDX_Control(pDX, IDC_PUBLIC, m_buPublic);
	DDX_Text(pDX, IDC_GROUP_SERVER_HANDLE, m_serverHandle);
	DDX_Control(pDX, IDC_GROUP_SERVER_HANDLE, m_edServerHandle);
	DDX_Text(pDX, IDC_GROUP_CLIENT_HANDLE, m_clientHandle);
	DDX_Control(pDX, IDC_GROUP_CLIENT_HANDLE, m_edClientHandle);
} // DoDataExchange


//-----------------------------------------------------------------------------
// loadAttributes
//
// - load the current attributes from the object
//
// returns:
//		TRUE  - attributes loaded
//		FALSE - no object set
//
BOOL SODmCPPGroup::loadAttributes(void)
{
	if (m_preventLoadAttributes)
	{
		return TRUE;
	}
	
	if (m_obj)
	{
		m_groupName = (LPCTSTR)m_obj->getName();

		switch (m_obj->getConnectionType())
		{
		case SODAC_CFG_CONNECTION_TYPE_ADDGROUP:
			m_conType = _T("AddGroup");
			break;

		case SODAC_CFG_CONNECTION_TYPE_GETPUBLICGROUPBYNAME:
			m_conType = _T("GetPublicGroupByName");
			break;

		default:
			m_conType = _T("");
			break;
		}

		switch (m_obj->getDataRetrieval())
		{
		case SODAC_CFG_DATA_RETRIEVAL_ASYNCADVISE:
			m_dataRet = _T("Asynchron Advises");
			break;

		case SODAC_CFG_DATA_RETRIEVAL_SYNCREAD:
			m_dataRet = _T("Synchron Reads");
			break;

		case SODAC_CFG_DATA_RETRIEVAL_POLLEDREFRESH:
			m_dataRet = _T("Polled Refresh");
			break;

		default:
			m_dataRet = _T("");
			break;
		}

		m_updateRate = m_obj->getUpdateRate();
		m_timeBias = m_obj->getTimeBias();
		m_deadband = m_obj->getDeadBand();
		m_keepAliveTime = m_obj->getKeepAliveTime();

		if (m_obj->isConnected())
		{
			queryLocaleIDs(m_obj->getObjectState());
		}

		LCID localeID = m_obj->getLCID();
		LCID2string(localeID, m_localeID);
		m_public = m_obj->getPublic();
		m_validUpdateRate = ((m_obj->getValidParameters() & SODAC_GRPPAR_UPDATERATE) == SODAC_GRPPAR_UPDATERATE);
		m_validTimeBias = ((m_obj->getValidParameters() & SODAC_GRPPAR_TIMEBIAS) == SODAC_GRPPAR_TIMEBIAS);
		m_validDeadband = ((m_obj->getValidParameters() & SODAC_GRPPAR_DEADBAND) == SODAC_GRPPAR_DEADBAND);
		m_validLocaleID = ((m_obj->getValidParameters() & SODAC_GRPPAR_LCID) == SODAC_GRPPAR_LCID);
		m_validKeepAliveTime = ((m_obj->getValidParameters() & SODAC_GRPPAR_KEEPALIVETIME) == SODAC_GRPPAR_KEEPALIVETIME);
		UpdateData(FALSE);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // loadAttributes


//-----------------------------------------------------------------------------
// storeAttributes
//
// - store the current attributes to the object
//
// returns:
//		TRUE  - changed attributes stored
//		FALSE - no object set
//
BOOL SODmCPPGroup::storeAttributes(void)
{
	if (m_obj)
	{
		DWORD updateWhat = 0;
		BOOL reconnect = FALSE;
		UpdateData(TRUE);

		if (m_oGroupName != m_groupName)
		{
			if (SUCCEEDED(m_obj->setName(m_groupName)))
			{
				if (m_obj->getConnectionType() == SODAC_CFG_CONNECTION_TYPE_GETPUBLICGROUPBYNAME)
				{
					reconnect = TRUE;    // no name update on public groups
				}
				else
				{
					updateWhat |= SODAC_GRPPAR_NAME;
				}

				getApp()->setDirtyFlag();
			}
		}

		m_obj->resetValidParameters();

		if (m_validUpdateRate)
		{
			if ((m_oUpdateRate != m_updateRate) ||
				(m_oValidUpdateRate != m_validUpdateRate))
			{
				updateWhat |= SODAC_GRPPAR_UPDATERATE;
				getApp()->setDirtyFlag();
			}

			m_obj->setReqUpdateRate(m_updateRate);
		}

		if ((m_oTimeBias != m_timeBias) ||
			(m_oValidTimeBias != m_validTimeBias))
		{
			updateWhat |= SODAC_GRPPAR_TIMEBIAS;
			getApp()->setDirtyFlag();

			if (m_validTimeBias)
			{
				m_obj->setTimeBias(m_timeBias);
			}
			else
			{
				m_obj->setTimeBias(0);
				m_obj->setValidParameters(m_obj->getValidParameters() & ~SODAC_GRPPAR_TIMEBIAS);
			}
		}

		if ((m_oDeadband != m_deadband) ||
			(m_oValidDeadband != m_validDeadband))
		{
			updateWhat |= SODAC_GRPPAR_DEADBAND;
			getApp()->setDirtyFlag();

			if (m_validDeadband)
			{
				m_obj->setDeadBand(m_deadband);
			}
			else
			{
				m_obj->setDeadBand(0.0);
				m_obj->setValidParameters(m_obj->getValidParameters() & ~SODAC_GRPPAR_DEADBAND);
			}
		}

		LCID localeID;
		localeID = string2LCID(m_localeID);

		if ((m_oLocaleID != m_localeID) ||
			(m_oValidLocaleID != m_validLocaleID))
		{
			updateWhat |= SODAC_GRPPAR_LCID;
			getApp()->setDirtyFlag();

			if (m_validLocaleID)
			{
				m_obj->setLCID(localeID);
			}
			else
			{
				m_obj->setLCID(0);
				m_obj->setValidParameters(m_obj->getValidParameters() & ~SODAC_GRPPAR_LCID);
			}
		}

		if ((m_oKeepAliveTime != m_keepAliveTime) ||
			(m_oValidKeepAliveTime != m_validKeepAliveTime))
		{
			updateWhat |= SODAC_GRPPAR_KEEPALIVETIME;
			getApp()->setDirtyFlag();

			if (m_validKeepAliveTime)
			{
				m_obj->setReqKeepAliveTime(m_keepAliveTime);
			}
			else
			{
				m_obj->setReqKeepAliveTime(0);
				m_obj->setValidParameters(m_obj->getValidParameters() & ~SODAC_GRPPAR_KEEPALIVETIME);
			}
		}

		if (m_oPublic != m_public)
		{
			if (m_obj->getConnectionType() == SODAC_CFG_CONNECTION_TYPE_ADDGROUP)
			{
				updateWhat |= SODAC_GRPPAR_PUBLIC;
				getApp()->setDirtyFlag();
			}

			m_obj->setPublic(m_public);
		}

		if (m_conType != m_oConType)
		{
			reconnect = TRUE;
			getApp()->setDirtyFlag();
		}

		if (m_dataRet != m_oDataRet)
		{
			reconnect = TRUE;
			getApp()->setDirtyFlag();
		}

		if ((reconnect) || ((updateWhat) && (m_obj->getTargetObjectState() >= SOCltElement::connected) && (m_obj->getObjectState() != m_obj->getTargetObjectState())))
		{
			BYTE targetState = m_obj->getTargetObjectState();
			BOOL started, connected;
			connected = (targetState == SODaCGroup::connected);
			started = (targetState == SODaCGroup::started);

			if ((connected) || (started))
			{
				m_preventLoadAttributes = TRUE;
				m_obj->disconnect();
				m_preventLoadAttributes = FALSE;
			}

			if (m_conType != m_oConType)
			{
				if (m_conType == _T("GetPublicGroupByName"))
				{
					m_obj->setConnectionType((SODaCGroup::connectionType)SODAC_CFG_CONNECTION_TYPE_GETPUBLICGROUPBYNAME);
				}
				else
				{
					m_obj->setConnectionType((SODaCGroup::connectionType)SODAC_CFG_CONNECTION_TYPE_ADDGROUP);
				}
			}

			if (m_dataRet != m_oDataRet)
			{
				if (m_dataRet == _T("Asynchron Advises"))
				{
					m_obj->setDataRetrieval((SODaCGroup::dataRetrieval)SODAC_CFG_DATA_RETRIEVAL_ASYNCADVISE);
				}
				else if (m_dataRet == _T("Polled Refresh"))
				{
					m_obj->setDataRetrieval((SODaCGroup::dataRetrieval)SODAC_CFG_DATA_RETRIEVAL_POLLEDREFRESH);
				}
				else
				{
					m_obj->setDataRetrieval((SODaCGroup::dataRetrieval)SODAC_CFG_DATA_RETRIEVAL_SYNCREAD);
				}
			}

			if ((connected) || (started))
			{
				if (started)
				{
					m_obj->start(TRUE);
				}
				else
				{
					m_obj->connect(TRUE);
				}
			}
		}
		else
		{
			// write changed parameters to server
			if ((updateWhat) && (m_obj->isConnected()))
			{
				
				if (SUCCEEDED(m_obj->update(FALSE, updateWhat)))
				{					
					m_obj->update(TRUE, updateWhat);
					loadAttributes();
				}
				else
				{
					m_obj->update(TRUE, updateWhat, SOCLT_EXEC_ASYNC);
					resetCurrentAttributes();
				}
			}
		}

		if (m_obj->getPublic())
		{
			m_buPublic.EnableWindow(FALSE);
		}
		
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // storeAttributes


//-----------------------------------------------------------------------------
// clearAttributes
//
// - clear the attribute values of the mask
//
void SODmCPPGroup::clearAttributes(void)
{
	m_groupName.Empty();
	m_conType.Empty();
	m_dataRet.Empty();
	m_updateRate = 0;
	m_timeBias = 0;
	m_deadband = 0;
	m_keepAliveTime = 0;
	m_localeID.Empty();
	m_validUpdateRate = FALSE;
	m_validTimeBias = FALSE;
	m_validDeadband = FALSE;
	m_validLocaleID = FALSE;
	m_validKeepAliveTime = FALSE;
	m_public = FALSE;
} // clearAttributes


//-----------------------------------------------------------------------------
// saveCurrentAttributes
//
// - save the current attributes for reset
//
void SODmCPPGroup::saveCurrentAttributes(void)
{
	UpdateData(TRUE);
	m_oGroupName = m_groupName;
	m_oConType = m_conType;
	m_oDataRet = m_dataRet;
	m_oUpdateRate = m_updateRate;
	m_oKeepAliveTime = m_keepAliveTime;
	m_oTimeBias = m_timeBias;
	m_oDeadband = m_deadband;
	m_oLocaleID = m_localeID;
	m_oValidUpdateRate = m_validUpdateRate;
	m_oValidTimeBias = m_validTimeBias;
	m_oValidDeadband = m_validDeadband;
	m_oValidLocaleID = m_validLocaleID;
	m_oValidKeepAliveTime = m_validKeepAliveTime;
	m_oPublic = m_public;
} // saveCurrentAttributes


//-----------------------------------------------------------------------------
// resetCurrentAttributes
//
// - reset the current attributes
//
void SODmCPPGroup::resetCurrentAttributes(void)
{
	m_groupName = m_oGroupName;
	m_conType = m_oConType;
	m_dataRet = m_oDataRet;
	m_updateRate = m_oUpdateRate;
	m_keepAliveTime = m_oKeepAliveTime;
	m_timeBias = m_oTimeBias;
	m_deadband = m_oDeadband;
	m_localeID = m_oLocaleID;
	m_validUpdateRate = m_oValidUpdateRate;
	m_validTimeBias = m_oValidTimeBias;
	m_validDeadband = m_oValidDeadband;
	m_validLocaleID = m_oValidLocaleID;
	m_validKeepAliveTime = m_oValidKeepAliveTime;
	m_public = m_oPublic;
	UpdateData(FALSE);
	enableControls(TRUE);
} // resetCurrentAttributes


//-----------------------------------------------------------------------------
// enableControls
//
// - en/disable the controls
//
void SODmCPPGroup::enableControls(IN BOOL enable)   // enable or disable
{
	enableButton(ID_PROPERTY_APPLY, TRUE);
	enableButton(ID_PROPERTY_RESET, TRUE);
	enableTimeBias(TRUE);
	enableDeadband(TRUE);
	enableLocaleID(TRUE);
	SOCmnPointer<SODaCServer> server = (SODaCServer*)m_obj->getParentBranch();
	BOOL enableKAT = FALSE;

	if (server.isNotNull())
	{
		enableKAT = (server->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30);
	}

	if (!enableKAT)
	{
		m_buValidKeepAliveTime.EnableWindow(FALSE);
	}

	enableKeepAliveTime(enableKAT);
	m_buPublic.EnableWindow(FALSE);

	if (m_conType == _T("GetPublicGroupByName"))
	{
		m_buValidUpdateRate.EnableWindow(TRUE);
	}
	else
	{
		m_validUpdateRate = TRUE;
		m_buValidUpdateRate.EnableWindow(FALSE);

		if (m_obj)
		{
			if ((m_obj->isConnected()) && (!m_public))
			{
				m_buPublic.EnableWindow(TRUE);
			}
		}
	}

	UpdateData(FALSE);
	enableUpdateRate(TRUE);
} // enableControls


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - object state has changed
//
void SODmCPPGroup::onSetObjectState(IN BYTE state)  // new object state
{
	queryLocaleIDs(state);
	loadAttributes();
} // onSetObjectState


//-----------------------------------------------------------------------------
// enableUpdateRate
//
// - en/disable update rate edit field
//
void SODmCPPGroup::enableUpdateRate(IN BOOL enable) // enable or disable
{
	m_edUpdateRate.SetReadOnly(!m_buValidUpdateRate.GetCheck());
} // enableUpdateRate


//-----------------------------------------------------------------------------
// enableTimeBias
//
// - en/disable time bias edit field
//
void SODmCPPGroup::enableTimeBias(IN BOOL enable)   // enable or disable
{
	m_edTimeBias.SetReadOnly(!m_buValidTimeBias.GetCheck());
} // enableTimeBias


//-----------------------------------------------------------------------------
// enableDeadband
//
// - en/disable deadband edit field
//
void SODmCPPGroup::enableDeadband(IN BOOL enable)   // enable or disable
{
	m_edDeadband.SetReadOnly(!m_buValidDeadband.GetCheck());
} // enableDeadband


//-----------------------------------------------------------------------------
// enableLocaleID
//
// - en/disable locale id combo box
//
void SODmCPPGroup::enableLocaleID(IN BOOL enable)   // enable or disable
{
	m_cbLocaleID.EnableWindow(m_buValidLocaleID.GetCheck());
} // enableLocaleID


//-----------------------------------------------------------------------------
// enableKeepAliveTime
//
// - en/disable keep alive time edit field
//
void SODmCPPGroup::enableKeepAliveTime(IN BOOL enable)  // enable or disable
{
	m_edKeepAliveTime.SetReadOnly(!m_buValidKeepAliveTime.GetCheck());
} // enableKeepAliveTime


//-----------------------------------------------------------------------------
// onClickUpdateRate
//
// - changed update rate check button
//
void SODmCPPGroup::onClickUpdateRate()
{
	UpdateData(TRUE);
	enableUpdateRate(TRUE);
	UpdateData(FALSE);
} // onClickUpdateRate


//-----------------------------------------------------------------------------
// onClickTimeBias
//
// - changed time bias check button
//
void SODmCPPGroup::onClickTimeBias()
{
	UpdateData(TRUE);
	enableTimeBias(TRUE);
	UpdateData(FALSE);
} // onClickTimeBias


//-----------------------------------------------------------------------------
// onClickDeadband
//
// - changed deadband check button
//
void SODmCPPGroup::onClickDeadband()
{
	UpdateData(TRUE);
	enableDeadband(TRUE);
	UpdateData(FALSE);
} // onClickDeadband


//-----------------------------------------------------------------------------
// onClickLocaleID
//
// - changed locale id check button
//
void SODmCPPGroup::onClickLocaleID()
{
	UpdateData(TRUE);
	enableLocaleID(TRUE);
	UpdateData(FALSE);
} // onClickLocaleID


//-----------------------------------------------------------------------------
// onClickKeepAliveTime
//
// - changed keep alive time check button
//
void SODmCPPGroup::onClickKeepAliveTime()
{
	UpdateData(TRUE);
	enableKeepAliveTime(TRUE);
	UpdateData(FALSE);
} // onClickKeepAliveTime


//-----------------------------------------------------------------------------
// onChangeConnectionType
//
// - changed connection type of the group
//
void SODmCPPGroup::onChangeConnectionType(void)
{
	UpdateData(TRUE);
	enableControls(TRUE);
	UpdateData(FALSE);

	// remove all items if selected GetPublicGroupByName
	if (m_conType == _T("GetPublicGroupByName"))
	{
		m_obj->removeAllLeafs(FALSE);
	}
} // onChangeConnectionType


//-----------------------------------------------------------------------------
// queryLocaleIDs
//
// - get the available locale ids from the server
//
void SODmCPPGroup::queryLocaleIDs(IN BYTE state)    // object state
{
	if (state >= SOCltElement::connected)
	{
#ifdef SODMC_DCOM
		SOCmnPointer<SODaCServer> server = (SODaCServer*)m_obj->getParentBranch();

		if (server.isNotNull())
		{
			SODmCComServerDA* pComSrv = NULL;

			if (server->getProtocol() == SOCLT_PROTOCOL_DCOM)
			{
				pComSrv = (SODmCComServerDA*)(SODaCServer*)server;
			}

			if (pComSrv)
			{
				if (pComSrv->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV20)
				{
					// fill combo box with the available LCIDs
					SOCmnDWORDList list;
					CString localeID;
					m_cbLocaleID.ResetContent();

					if (SUCCEEDED(pComSrv->queryAvailableLCIDs(list)))
					{
						SOCmnListPosition pos;
						DWORD lcid;
						pos = list.getStartPosition();

						while (pos)
						{
							lcid = list.getNext(pos);
							LCID2string(lcid, localeID);
							m_cbLocaleID.AddString(localeID);
						}
					}
				}
			}
		}

#endif // SODMC_DCOM
	}
} // queryLocaleIDs

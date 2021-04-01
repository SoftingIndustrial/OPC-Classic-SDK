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
//  Filename    : SODmCServerAE.cpp                                           |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : AE server classes                                           |
//                - SODmCServerAE: Toolkit AE server object class             |
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
#include "SODmCPropFrm.h"
#include "SODmCServerAE.h"
#include "SODmCObjectView.h"
#include "SODmCMainView.h"
#include "SODmCSubscription.h"


//-----------------------------------------------------------------------------
// SODmCCmnServerAE                                                              |
//-----------------------------------------------------------------------------

SODmCCmnServerAE::SODmCCmnServerAE()
{
	m_treeHandle = 0;
}



//-----------------------------------------------------------------------------
// SODmCComServerAE                                                              |
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// setParentBranch
//
// - called to insert object into the tree
//
BOOL SODmCComServerAE::setParentBranch(IN SOCmnElement* parent)
{
	if (!SOAeCServer::setParentBranch(parent))
	{
		return FALSE;
	}

	if (!getApp()->getOptions()->cmOn)
	{
		removeConnectionMonitor();
	}

	SODmCObjectView* objview = getObjectView();

	if (!objview)
	{
		return TRUE;
	}

	if (IsWindow(objview->GetTreeCtrl().m_hWnd))
	{
		objview->GetTreeCtrl().Expand(objview->getTreeHandleAE(), TVE_EXPAND);
	}

	if (m_parent)
	{
		m_treeHandle = SODmCTreeInsert(&objview->GetTreeCtrl(), objview->getTreeHandleAE(), getURL(), TRUE, (SOCltElement*)this, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, TVI_LAST);

		if (IsWindow(objview->GetTreeCtrl().m_hWnd))
		{
			objview->GetTreeCtrl().Expand(objview->getTreeHandleAE(), TVE_EXPAND);
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

	if (!m_parent)
	{
		SOCmnPointer<SOAeCEntry> entry = ::getSOAeCEntry();
		SOCmnList<SOCmnElement> serverList = entry->getBranchList();

		if (serverList.isEmpty())
		{
			objview->deleteAETree();
		}
	}

	return TRUE;
} // setParentBranch


//-----------------------------------------------------------------------------
// onError
//
// - called after an error occured
//
void SODmCComServerAE::onError(IN HRESULT res, IN DWORD errorcode)
{
	SODmCMainView* mainview = getMainView();
	SOCmnString et;
	getErrorDescription(errorcode, et, 0x0800);
	mainview->insertError((SOCltElement*)this, res, errorcode, et);
}


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - called after the state of the object changed
//
void SODmCComServerAE::onSetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setObjectState((SOCmnObject*)this, getObjectState());
} // onSetObjectState


//-----------------------------------------------------------------------------
// onSetTargetObjectState
//
// - called after the state of the target object changed
//
void SODmCComServerAE::onSetTargetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setTargetObjectState((SOCmnObject*)this, getTargetObjectState());
} // onSetTargetObjectState


//-----------------------------------------------------------------------------
// beforeInterfaceCall
//
// - called before an COM interface call
//
void SODmCComServerAE::beforeInterfaceCall(void)
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
void SODmCComServerAE::afterInterfaceCall(void)
{
	getApp()->EndWaitCursor();
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->removeFromWorkingList(this);
		INVOKE_CALLBACK(SODmCComServerAE, onSetTargetObjectState);
	}
} // afterInterfaceCall


//-----------------------------------------------------------------------------
// Serialization
BOOL SODmCComServerAE::serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring)
{
	if (!SOAeCServer::serialize(ar, isStoring))
	{
		return FALSE;
	}

	if (isStoring)
	{
		// store own data
		ar << getProgID();
		ar << getDescription();
	}
	else
	{
		if (getApp()->getFileVersion() >= MAKELONG(4, 3))
		{
			SOCmnString progID;
			SOCmnString description;
			ar >> progID;
			setProgID(progID);
			ar >> description;
			setDescription(description);
		}
	}

	return TRUE;
} // serialize


SOCltComServerImpl* SODmCComServerAE::getObjectCltComServerImpl(void)
{
	return (SOCltComServerImpl*)this;
}


#ifdef SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// SODmCTPServerAE                                                              |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// setParentBranch
//
// - called to insert object into the tree
//
BOOL SODmCTPServerAE::setParentBranch(IN SOCmnElement* parent)
{
	if (!SOAeCServer::setParentBranch(parent))
	{
		return FALSE;
	}

	if (!getApp()->getOptions()->cmOn)
	{
		removeConnectionMonitor();
	}

	SODmCObjectView* objview = getObjectView();

	if (!objview)
	{
		return TRUE;
	}

	if (IsWindow(objview->GetTreeCtrl().m_hWnd))
	{
		objview->GetTreeCtrl().Expand(objview->getTreeHandleAE(), TVE_EXPAND);
	}

	if (m_parent)
	{
		m_treeHandle = SODmCTreeInsert(&objview->GetTreeCtrl(), objview->getTreeHandleAE(), getURL(), TRUE, (SOCltElement*)this, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, TVI_LAST);

		if (IsWindow(objview->GetTreeCtrl().m_hWnd))
		{
			objview->GetTreeCtrl().Expand(objview->getTreeHandleAE(), TVE_EXPAND);
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

	if (!m_parent)
	{
		SOCmnPointer<SOAeCEntry> entry = ::getSOAeCEntry();
		SOCmnList<SOCmnElement> serverList = entry->getBranchList();

		if (serverList.isEmpty())
		{
			objview->deleteAETree();
		}
	}

	return TRUE;
} // setParentBranch


//-----------------------------------------------------------------------------
// onError
//
// - called after an error occured
//
void SODmCTPServerAE::onError(IN HRESULT res, IN DWORD errorcode)
{
	SODmCMainView* mainview = getMainView();
	SOCmnString et;
	getErrorDescription(errorcode, et, 0x0800);
	mainview->insertError((SOCltElement*)this, res, errorcode, et);
}


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - called after the state of the object changed
//
void SODmCTPServerAE::onSetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setObjectState((SOCmnObject*)this, getObjectState());
} // onSetObjectState


//-----------------------------------------------------------------------------
// onSetTargetObjectState
//
// - called after the state of the target object changed
//
void SODmCTPServerAE::onSetTargetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setTargetObjectState((SOCmnObject*)this, getTargetObjectState());
} // onSetTargetObjectState


//-----------------------------------------------------------------------------
// beforeInterfaceCall
//
// - called before an COM interface call
//
void SODmCTPServerAE::beforeInterfaceCall(void)
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
void SODmCTPServerAE::afterInterfaceCall(void)
{
	getApp()->EndWaitCursor();
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->removeFromWorkingList(this);
		INVOKE_CALLBACK(SODmCTPServerAE, onSetTargetObjectState);
	}
} // afterInterfaceCall


#endif // end SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// SODmCPPServerAE                                                            |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPServerAE, SODmCPPage)

BEGIN_MESSAGE_MAP(SODmCPPServerAE, SODmCPPage)
END_MESSAGE_MAP()

SODmCPPServerAE::SODmCPPServerAE(IN SODmCPSheet* sheet, IN UINT resID)
	: SODmCPPage(sheet, resID)
{
	m_obj = NULL;
}

SODmCPPServerAE::~SODmCPPServerAE()
{
}


//-----------------------------------------------------------------------------
// connect
//
// - connect server to OPC server
// returns:
//		TRUE  - server is connected
//    FALSE - can't connect server or no server set
//
BOOL SODmCPPServerAE::connect(void)
{
	if (m_obj)
	{
		if (!m_obj->isConnected())
		{
			return m_obj->connect(FALSE);
		}
		else
		{
			return TRUE;
		}
	}

	return FALSE;
} // connect




//-----------------------------------------------------------------------------
// SODmCPPServerAEAttributes                                                  |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPServerAEAttributes, SODmCPPServerAE)

BEGIN_MESSAGE_MAP(SODmCPPServerAEAttributes, SODmCPPServerAE)
	ON_BN_CLICKED(IDC_SERVER_VALID_LOCALE_ID, onClickLocaleID)
	ON_BN_CLICKED(IDC_SERVER_VALID_CLIENT_NAME, onClickClientName)
END_MESSAGE_MAP()

SODmCPPServerAEAttributes::SODmCPPServerAEAttributes(IN SODmCPSheet* sheet)
	: SODmCPPServerAE(sheet, IDD_PP_SERVERAE_ATTRIBUTES)
{
	m_validLocaleID = FALSE;
	m_validClientName = FALSE;
	m_edURL.m_firstControl = TRUE;
	m_liAvailableFilters.m_lastControl = TRUE;
}

SODmCPPServerAEAttributes::~SODmCPPServerAEAttributes()
{
}

//-----------------------------------------------------------------------------
// OnInitDialog
//
// - Initialize list controls
//
// returns:
//		return of base class
//
BOOL SODmCPPServerAEAttributes::OnInitDialog(void)
{
	BOOL ret = SODmCPPServerAE::OnInitDialog();
	CRect rect;
	m_liAvailableFilters.SetBkColor(0x00CED3D6);
	m_liAvailableFilters.SetTextBkColor(0x00CED3D6);
	m_liAvailableFilters.GetClientRect(&rect);
	m_liAvailableFilters.InsertColumn(0, _T("A"), LVCFMT_LEFT, rect.Width() - 17);
	return ret;
} // OnInitDialog


//-----------------------------------------------------------------------------
// enableControls
//
// - Virtual method for control enabling
//
void SODmCPPServerAEAttributes::enableControls(IN BOOL enable)  // enable or disable
{
	enableButton(ID_PROPERTY_RESET, TRUE);
	enableButton(ID_PROPERTY_APPLY, TRUE);
	enableLocaleID(TRUE);
	enableClientName(TRUE);
} // enableControls


//-----------------------------------------------------------------------------
// loadAttributes
//
// - load the current attributes from the object
// returns:
//		TRUE  - attributes loaded
//		FALSE - no object set
//
BOOL SODmCPPServerAEAttributes::loadAttributes(void)
{
	if (m_obj)
	{
		m_URL = m_obj->getURL();
		LCID localeID;
		localeID = m_obj->getLCID();
		LCID2string(localeID, m_localeID);
		m_validLocaleID = ((m_obj->getValidParameters() & SOCLT_SRVPAR_LCID) == SOCLT_SRVPAR_LCID);
		m_validClientName = ((m_obj->getValidParameters() & SOCLT_SRVPAR_CLIENT_NAME) == SOCLT_SRVPAR_CLIENT_NAME);

		if (m_validClientName)
		{
			m_clientName = m_obj->getClientName();
		}
		else
		{
			m_clientName = _T("?");
		}

		queryLocaleIDs();
		queryAvailableFilters();
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
// returns:
//		TRUE  - changed attributes stored
//		FALSE - no object set
//
BOOL SODmCPPServerAEAttributes::storeAttributes(void)
{
	if (m_obj)
	{
		DWORD updateWhat = 0;
		DWORD getWhat = SOCLT_SRVPAR_ALL;
		BOOL reconnect = FALSE;
		UpdateData(TRUE);
		m_obj->resetValidParameters();

		if (m_validLocaleID)
		{
			if ((m_oLocaleID != m_localeID) ||
				(m_oValidLocaleID != m_validLocaleID))
			{
				LCID localeID;
				localeID = string2LCID(m_localeID);
				m_obj->setLCID(localeID);
				updateWhat |= SOCLT_SRVPAR_LCID;
				getWhat &= ~SOCLT_SRVPAR_LCID;
				getApp()->setDirtyFlag();
			}
		}
		else
		{
			if (m_oValidLocaleID != m_validLocaleID)
			{
				reconnect = TRUE;
				getApp()->setDirtyFlag();
			}
		}

		if (m_validClientName)
		{
			if ((m_oClientName != m_clientName) ||
				(m_oValidClientName != m_validClientName))
			{
				m_obj->setClientName(m_clientName);
				updateWhat |= SOCLT_SRVPAR_CLIENT_NAME;
				getWhat &= ~SOCLT_SRVPAR_CLIENT_NAME;
				getApp()->setDirtyFlag();
			}
		}
		else
		{
			if (m_oValidClientName != m_validClientName)
			{
				reconnect = TRUE;
				getApp()->setDirtyFlag();
			}
		}

		if (reconnect)
		{
			BYTE targetState = m_obj->getTargetObjectState();
			BOOL started, connected;
			connected = (targetState == SOAeCServer::connected);
			started = (targetState == SOAeCServer::started);

			if ((connected) || (started))
			{
				m_obj->disconnect();

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
			if (m_obj->isConnected())
			{
				if (updateWhat)
				{
					// write changed parameters to server
					if (SUCCEEDED(m_obj->update(FALSE, updateWhat)))
					{
						m_obj->update(TRUE, updateWhat);
						loadAttributes();
					}
				}

				if (getWhat)
				{
					m_obj->update(TRUE, getWhat);
					loadAttributes();
				}
			}
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
void SODmCPPServerAEAttributes::clearAttributes(void)
{
	m_URL.Empty();
	m_clientName.Empty();
	m_localeID.Empty();
	m_validLocaleID = FALSE;
	m_validClientName = FALSE;
} // clearAttributes


//-----------------------------------------------------------------------------
// saveCurrentAttributes
//
// - save the current attributes for reset
//
void SODmCPPServerAEAttributes::saveCurrentAttributes(void)
{
	UpdateData(TRUE);
	m_oURL = m_URL;
	m_oClientName = m_clientName;
	m_oLocaleID = m_localeID;
	m_oValidLocaleID = m_validLocaleID;
	m_oValidClientName = m_validClientName;
} // saveCurrentAttributes


//-----------------------------------------------------------------------------
// resetCurrentAttributes
//
// - reset the current attributes
//
void SODmCPPServerAEAttributes::resetCurrentAttributes(void)
{
	m_URL = m_oURL;
	m_clientName = m_oClientName;
	m_localeID = m_oLocaleID;
	m_validLocaleID = m_oValidLocaleID;
	m_validClientName = m_oValidClientName;
	UpdateData(FALSE);
	enableControls(TRUE);
} // resetCurrentAttributes


//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPServerAEAttributes::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPServerAE::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SERVER_URL, m_URL);
	DDX_Control(pDX, IDC_SERVER_URL, m_edURL);
	DDX_Text(pDX, IDC_SERVER_CLIENT_NAME, m_clientName);
	DDX_Control(pDX, IDC_SERVER_CLIENT_NAME, m_edClientName);
	DDX_Check(pDX, IDC_SERVER_VALID_CLIENT_NAME, m_validClientName);
	DDX_Control(pDX, IDC_SERVER_VALID_CLIENT_NAME, m_buValidClientName);
	DDX_CBString(pDX, IDC_SERVER_LOCALE_ID, m_localeID);
	DDX_Control(pDX, IDC_SERVER_LOCALE_ID, m_cbLocaleID);
	DDX_Check(pDX, IDC_SERVER_VALID_LOCALE_ID, m_validLocaleID);
	DDX_Control(pDX, IDC_SERVER_VALID_LOCALE_ID, m_buValidLocaleID);
	DDX_Control(pDX, IDC_SERVERAE_AVAILABLE_FILTERS, m_liAvailableFilters);
} // DoDataExchange


//-----------------------------------------------------------------------------
// enableLocaleID
//
// - enable locale id field for OPC V2.0 OPC servers
//
void SODmCPPServerAEAttributes::enableLocaleID(
	IN BOOL enable) // enable or disable
{
	m_buValidLocaleID.EnableWindow(enable);

	if (enable)
	{
		enable = m_buValidLocaleID.GetCheck();
	}

	m_cbLocaleID.EnableWindow(enable);
} // enableLocaleID


//-----------------------------------------------------------------------------
// enableClientName
//
// - enable client name field for OPC V2.0 OPC servers
//
void SODmCPPServerAEAttributes::enableClientName(IN BOOL enable)    // enable or disable
{
	m_buValidClientName.EnableWindow(enable);

	if (enable)
	{
		enable = m_buValidClientName.GetCheck();
	}

	m_edClientName.SetReadOnly(!enable);

	if (!enable)
	{
		m_clientName = _T("?");
	}
} // enableClientName


//-----------------------------------------------------------------------------
// onClickLocaleID
//
// - clicked locale id check button
//
void SODmCPPServerAEAttributes::onClickLocaleID()
{
	UpdateData(TRUE);
	enableLocaleID(TRUE);
	UpdateData(FALSE);
} // onClickLocaleID


//-----------------------------------------------------------------------------
// onClickClientName
//
// - clicked client name check button
//
void SODmCPPServerAEAttributes::onClickClientName()
{
	UpdateData(TRUE);
	enableClientName(TRUE);
	UpdateData(FALSE);
} // onClickClientName


//-----------------------------------------------------------------------------
// queryLocaleIDs
//
// - get the available locale ids from the server
//
void SODmCPPServerAEAttributes::queryLocaleIDs(void)
{
	if (m_obj->isConnected())
	{
#ifdef SODMC_DCOM
		SODmCComServerAE* pComObj = NULL;

		if (m_obj->getProtocol() == SOCLT_PROTOCOL_DCOM)
		{
			pComObj = (SODmCComServerAE*)m_obj;
		}

		if (pComObj)
		{
			// fill combo box with the available LCIDs
			SOCmnDWORDList list;
			CString localeID;
			m_cbLocaleID.ResetContent();

			if (SUCCEEDED(pComObj->queryAvailableLCIDs(list)))
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

#endif // SODMC_DCOM
	}
} // queryLocaleIDs


//-----------------------------------------------------------------------------
// queryAvailableFilters
//
// - get the available filters from the server
//
void SODmCPPServerAEAttributes::queryAvailableFilters(void)
{
	m_liAvailableFilters.DeleteAllItems();

	if (m_obj->isConnected())
	{
		IOPCEventServer* iES;

		if (SUCCEEDED(m_obj->queryInterface(IID_IOPCEventServer, (void**)&iES)))
		{
			DWORD filerMask;
			int i = 0;

			if (SUCCEEDED(iES->QueryAvailableFilters(&filerMask)))
			{
				if (filerMask & OPC_FILTER_BY_EVENT)
				{
					m_liAvailableFilters.InsertItem(i, _T("Event type"));
				}

				i++;

				if (filerMask & OPC_FILTER_BY_CATEGORY)
				{
					m_liAvailableFilters.InsertItem(i, _T("Category"));
				}

				i++;

				if (filerMask & OPC_FILTER_BY_SEVERITY)
				{
					m_liAvailableFilters.InsertItem(i, _T("Severity"));
				}

				i++;

				if (filerMask & OPC_FILTER_BY_AREA)
				{
					m_liAvailableFilters.InsertItem(i, _T("Area"));
				}

				i++;

				if (filerMask & OPC_FILTER_BY_SOURCE)
				{
					m_liAvailableFilters.InsertItem(i, _T("Source"));
				}

				i++;
			}

			iES->Release();
		}
	}
} // queryAvailableFilters


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - object state has changed
//
void SODmCPPServerAEAttributes::onSetObjectState(IN BYTE state) // new object state
{
	loadAttributes();
} // onSetObjectState




//-----------------------------------------------------------------------------
// SODmCPPServerAEStatus                                                      |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPServerAEStatus, SODmCPPServerAE)

BEGIN_MESSAGE_MAP(SODmCPPServerAEStatus, SODmCPPServerAE)
END_MESSAGE_MAP()

SODmCPPServerAEStatus::SODmCPPServerAEStatus(IN SODmCPSheet* sheet)
	: SODmCPPServerAE(sheet, IDD_PP_SERVERAE_STATUS)
{
	m_edVendorInfo.m_firstControl = TRUE;
	m_edServerState.m_lastControl = TRUE;
}

SODmCPPServerAEStatus::~SODmCPPServerAEStatus()
{
}

//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPServerAEStatus::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPServerAE::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SERVER_START_TIME, m_startTime);
	DDX_Text(pDX, IDC_SERVER_CURRENT_TIME, m_currentTime);
	DDX_Text(pDX, IDC_SERVER_UPDATE_TIME, m_updateTime);
	DDX_Text(pDX, IDC_SERVER_VERSION, m_version);
	DDX_Text(pDX, IDC_SERVER_VENDOR_INFO, m_vendorInfo);
	DDX_Text(pDX, IDC_SERVER_STATE, m_serverState);
	DDX_Control(pDX, IDC_SERVER_START_TIME, m_edStartTime);
	DDX_Control(pDX, IDC_SERVER_CURRENT_TIME, m_edCurrentTime);
	DDX_Control(pDX, IDC_SERVER_UPDATE_TIME, m_edUpdateTime);
	DDX_Control(pDX, IDC_SERVER_VERSION, m_edVersion);
	DDX_Control(pDX, IDC_SERVER_VENDOR_INFO, m_edVendorInfo);
	DDX_Control(pDX, IDC_SERVER_STATE, m_edServerState);
} // DoDataExchange


//-----------------------------------------------------------------------------
// enableControls
//
// - en/disable the controls
//
void SODmCPPServerAEStatus::enableControls(IN BOOL enable)  // enable or disable
{
	enableButton(ID_PROPERTY_RESET, FALSE);
	enableButton(ID_PROPERTY_APPLY, TRUE);
} // enableControls


//-----------------------------------------------------------------------------
// onOK
//
// - reload attributes on OK
//
void SODmCPPServerAEStatus::onOK(void)
{
	loadAttributes();
} // onOK


//-----------------------------------------------------------------------------
// loadAttributes
//
// - load the current attributes from the object
//
// returns:
//		TRUE  - attributes loaded
//		FALSE - no object set
//
BOOL SODmCPPServerAEStatus::loadAttributes(void)
{
	if (m_obj)
	{
		if (m_obj->isConnected())
		{
			SOAeCServerStatus serverStatus;
			HRESULT res;

			if (SUCCEEDED(res = m_obj->getStatus(serverStatus)))
			{
				m_vendorInfo = (LPCTSTR)serverStatus.m_vendorInfo;
				SOCmnString startTime;
				SOCmnString currentTime;
				DATETIME2STRING(startTime, serverStatus.m_startTime);
				DATETIME2STRING(currentTime, serverStatus.m_currentTime);
				m_startTime = (LPCTSTR)startTime;
				m_currentTime = (LPCTSTR)currentTime;

				if (serverStatus.m_lastUpdateTime.isSet())
				{
					SOCmnString updateTime;
					DATETIME2STRING(updateTime, serverStatus.m_lastUpdateTime);
					m_updateTime = (LPCTSTR)updateTime;
				}
				else
				{
					m_updateTime.Empty();
				}

				m_version = serverStatus.m_productVersion;

				switch (serverStatus.m_state)
				{
				case OPC_STATUS_RUNNING:
					m_serverState = _T("running");
					break;

				case OPC_STATUS_FAILED:
					m_serverState = _T("failed");
					break;

				case OPC_STATUS_NOCONFIG:
					m_serverState = _T("no configuration");
					break;

				case OPC_STATUS_SUSPENDED:
					m_serverState = _T("suspended");
					break;

				case OPC_STATUS_TEST:
					m_serverState = _T("test");
					break;

				case OPC_STATUS_COMM_FAULT:
					m_serverState = _T("communication fault");
					break;

				default:
					m_serverState = _T("");
					break;
				}

				UpdateData(FALSE);
			}

			return SUCCEEDED(res);
		}
	}

	return FALSE;
} // loadAttributes


//-----------------------------------------------------------------------------
// clearAttributes
//
// - clear the attribute values of the mask
//
void SODmCPPServerAEStatus::clearAttributes(void)
{
	m_startTime.Empty();
	m_currentTime.Empty();
	m_updateTime.Empty();
	m_version.Empty();
	m_vendorInfo.Empty();
	m_serverState.Empty();
} // clearAttributes




//-----------------------------------------------------------------------------
// SODmCPPServerAEBrowse                                                      |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPServerAEBrowse, SODmCPPServerAE)

BEGIN_MESSAGE_MAP(SODmCPPServerAEBrowse, SODmCPPServerAE)
END_MESSAGE_MAP()

SODmCPPServerAEBrowse::SODmCPPServerAEBrowse(IN SODmCPSheet* sheet)
	: SODmCPPServerAE(sheet, IDD_PP_SERVERAE_BROWSE)
{
	m_edStringFilter.m_firstControl = TRUE;
	m_edStringFilter.m_lastControl = TRUE;
}

SODmCPPServerAEBrowse::~SODmCPPServerAEBrowse()
{
}


//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPServerAEBrowse::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPServerAE::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ITEM_FILTER, m_stringFilter);
	DDX_Control(pDX, IDC_ITEM_FILTER, m_edStringFilter);
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
BOOL SODmCPPServerAEBrowse::loadAttributes(void)
{
	if (m_obj)
	{
		SODmCCmnServerAE* pCmnSrv = NULL;

		if (m_obj->getProtocol() == SOCLT_PROTOCOL_DCOM)
		{
			pCmnSrv = (SODmCCmnServerAE*)(SODmCComServerAE*)(SOAeCServer*)m_obj;
		}

#ifdef SOFEATURE_TUNNEL
		else
		{
			pCmnSrv = (SODmCCmnServerAE*)(SODmCTPServerAE*)(SOAeCServer*)m_obj;
		}

#endif
		m_stringFilter = pCmnSrv->getStringFilter();
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
BOOL SODmCPPServerAEBrowse::storeAttributes(void)
{
	if (m_obj)
	{
		UpdateData(TRUE);
		SODmCCmnServerAE* pCmnSrv = NULL;

		if (m_obj->getProtocol() == SOCLT_PROTOCOL_DCOM)
		{
			pCmnSrv = (SODmCCmnServerAE*)(SODmCComServerAE*)(SOAeCServer*)m_obj;
		}

#ifdef SOFEATURE_TUNNEL
		else
		{
			pCmnSrv = (SODmCCmnServerAE*)(SODmCTPServerAE*)(SOAeCServer*)m_obj;
		}

#endif
		pCmnSrv->setStringFilter(m_stringFilter);
		SOCmnPointer<SOAeCServer> srv = getObjectView()->getSelectedServerAE();

		if (srv == m_obj)
		{
			SODmCMainView* mainview = getMainView();
			SOCmnPointer<SOAeCSubscription> subs = getObjectView()->getSelectedSubscription();
			mainview->changeSelectedSubscription(subs, TRUE);
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
void SODmCPPServerAEBrowse::clearAttributes(void)
{
	m_stringFilter = _T("");
} // clearAttributes


//-----------------------------------------------------------------------------
// saveCurrentAttributes
//
// - save the current attributes for reset
//
void SODmCPPServerAEBrowse::saveCurrentAttributes(void)
{
	UpdateData(TRUE);
	m_oStringFilter = m_stringFilter;
} // saveCurrentAttributes


//-----------------------------------------------------------------------------
// resetCurrentAttributes
//
// - reset the current attributes
//
void SODmCPPServerAEBrowse::resetCurrentAttributes(void)
{
	m_stringFilter = m_oStringFilter;
	UpdateData(FALSE);
} // resetCurrentAttributes


//-----------------------------------------------------------------------------
// enableControls
//
// - en/disable the controls
//
void SODmCPPServerAEBrowse::enableControls(IN BOOL enable)  // enable or disable
{
	enableButton(ID_PROPERTY_APPLY, TRUE);
	enableButton(ID_PROPERTY_RESET, TRUE);
} // enableControls



//-----------------------------------------------------------------------------
// SODmCPPServerAESecurity                                                    |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPServerAESecurity, SODmCPPServerAE)

BEGIN_MESSAGE_MAP(SODmCPPServerAESecurity, SODmCPPServerAE)
END_MESSAGE_MAP()

SODmCPPServerAESecurity::SODmCPPServerAESecurity(IN SODmCPSheet* sheet)
	: SODmCPPServerAE(sheet, IDD_PP_SERVER_SECURITY)
{
	m_storeSecInfo = FALSE;
	m_edUserName.m_firstControl = TRUE;
	m_buStoreSecInfo.m_lastControl = TRUE;
}

SODmCPPServerAESecurity::~SODmCPPServerAESecurity()
{
}


//-----------------------------------------------------------------------------
// enableControls
//
// - Virtual method for control enabling
//
void SODmCPPServerAESecurity::enableControls(IN BOOL enable)    // enable or disable
{
#ifdef SODMC_DCOM
	SODmCComServerAE* pComObj = NULL;

	if (m_obj->getProtocol() == SOCLT_PROTOCOL_DCOM)
	{
		pComObj = (SODmCComServerAE*)m_obj;
	}

	if (pComObj)
	{
		if ((!pComObj->isConnected()) || (pComObj->isPrivateSecurityAvailable()))
		{
			enableButton(ID_PROPERTY_RESET, TRUE);
			enableButton(ID_PROPERTY_APPLY, TRUE);
			m_buStoreSecInfo.EnableWindow(TRUE);
			m_edUserName.SetReadOnly(FALSE);
			m_edPassword.SetReadOnly(FALSE);
			return;
		}
	}

#endif //SODMC_DCOM
	enableButton(ID_PROPERTY_RESET, FALSE);
	enableButton(ID_PROPERTY_APPLY, FALSE);
	m_buStoreSecInfo.EnableWindow(FALSE);
	m_edUserName.SetReadOnly(TRUE);
	m_edPassword.SetReadOnly(TRUE);
} // enableControls


//-----------------------------------------------------------------------------
// loadAttributes
//
// - load the current attributes from the object
// returns:
//		TRUE  - attributes loaded
//		FALSE - no object set
//
BOOL SODmCPPServerAESecurity::loadAttributes(void)
{
	if (m_obj)
	{
		m_userName = m_obj->getUserName();
		m_password = m_obj->getPassword();
		m_storeSecInfo = !m_userName.IsEmpty();
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
// returns:
//		TRUE  - changed attributes stored
//		FALSE - no object set
//
BOOL SODmCPPServerAESecurity::storeAttributes(void)
{
#ifdef SODMC_DCOM
	SODmCComServerAE* pComObj = NULL;

	if (m_obj->getProtocol() == SOCLT_PROTOCOL_DCOM)
	{
		pComObj = (SODmCComServerAE*)m_obj;
	}

	if (pComObj)
	{
		UpdateData(TRUE);

		if ((m_edUserName.GetStyle() & ES_READONLY) == 0)
		{
			// support security
			if (m_storeSecInfo)
			{
				if (m_oUserName != m_userName)
				{
					pComObj->setUserName(m_userName);
					getApp()->setDirtyFlag();
				}

				if (m_oPassword != m_password)
				{
					pComObj->setPassword(m_password);
					getApp()->setDirtyFlag();
				}
			}
			else
			{
				if (!m_oUserName.IsEmpty())
				{
					pComObj->setUserName(_T(""));
					getApp()->setDirtyFlag();
				}

				if (m_oPassword.IsEmpty())
				{
					pComObj->setPassword(_T(""));
					getApp()->setDirtyFlag();
				}
			}

			if (pComObj->isConnected())
			{
				if (m_userName.IsEmpty())
				{
					pComObj->logoff();
				}
				else
				{
					pComObj->logon(m_userName, m_password);
				}
			}
		}

		return TRUE;
	}
	else
#endif // SODMC_DCOM
		return FALSE;
} // storeAttributes


//-----------------------------------------------------------------------------
// clearAttributes
//
// - clear the attribute values of the mask
//
void SODmCPPServerAESecurity::clearAttributes(void)
{
	m_userName.Empty();
	m_password.Empty();
	m_storeSecInfo = FALSE;
} // clearAttributes


//-----------------------------------------------------------------------------
// saveCurrentAttributes
//
// - save the current attributes for reset
//
void SODmCPPServerAESecurity::saveCurrentAttributes(void)
{
	UpdateData(TRUE);
	m_oUserName = m_userName;
	m_oPassword = m_password;
} // saveCurrentAttributes


//-----------------------------------------------------------------------------
// resetCurrentAttributes
//
// - reset the current attributes
//
void SODmCPPServerAESecurity::resetCurrentAttributes(void)
{
	m_userName = m_oUserName;
	m_password = m_oPassword;
	UpdateData(FALSE);
	enableControls(TRUE);
} // resetCurrentAttributes


//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPServerAESecurity::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPServerAE::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SERVER_USER_NAME, m_userName);
	DDX_Control(pDX, IDC_SERVER_USER_NAME, m_edUserName);
	DDX_Text(pDX, IDC_SERVER_PASSWORD, m_password);
	DDX_Control(pDX, IDC_SERVER_PASSWORD, m_edPassword);
	DDX_Check(pDX, IDC_SERVER_STORE_SEC_INFO, m_storeSecInfo);
	DDX_Control(pDX, IDC_SERVER_STORE_SEC_INFO, m_buStoreSecInfo);
} // DoDataExchange





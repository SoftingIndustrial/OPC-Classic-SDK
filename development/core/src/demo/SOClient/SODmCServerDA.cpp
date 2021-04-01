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
//  Filename    : SODmCServerDA.cpp                                           |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : DA server classes                                           |
//                - SODmCServerDA: Toolkit DA server object class             |
//                - SODmCPPServerDA: Base class for all server property pages |
//                - SODmCPPServerDAAttributes: Server attribute property page |
//                - SODmCPPServerDAStatus: Server status property page        |
//                - SODmCPPServerDABrowse: Server browse filter property page |
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


//-----------------------------------------------------------------------------
// SODmCCmnServerDA                                                           |
//-----------------------------------------------------------------------------

SODmCCmnServerDA::SODmCCmnServerDA()
{
	m_treeHandle = 0;
	m_stringFilter = _T("");
	m_accessRightsFilter = 0;
	m_datatypeFilter = VT_EMPTY;
}


#ifdef SODMC_DCOM

//-----------------------------------------------------------------------------
// SODmCComServerDA                                                           |
//-----------------------------------------------------------------------------

SODmCComServerDA::SODmCComServerDA()
{
}

//-----------------------------------------------------------------------------
// setParentBranch
//
// - called to insert object into the tree
//
BOOL SODmCComServerDA::setParentBranch(IN SOCmnElement* parent)
{
	if (!SODaCServer::setParentBranch(parent))
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

	if (m_parent)
	{
		m_treeHandle = SODmCTreeInsert(&objview->GetTreeCtrl(), objview->getTreeHandleDA(), getURL(), TRUE, (SOCltElement*)this, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, TVI_LAST);

		if (IsWindow(objview->GetTreeCtrl().m_hWnd))
		{
			objview->GetTreeCtrl().Expand(objview->getTreeHandleDA(), TVE_EXPAND);
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
		SOCmnPointer<SODaCEntry> entry = ::getSODaCEntry();
		SOCmnList<SOCmnElement> serverList = entry->getBranchList();

		if (serverList.isEmpty())
		{
			objview->deleteDATree();
		}
	}

	return TRUE;
} // setParentBranch


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - called after the state of the object changed
//
void SODmCComServerDA::onSetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	SOCmnPointer<SODaCServer>selServer = getObjectView()->getSelectedServerDA();

	if ((SODaCServer*)selServer == (SODaCServer*)this)
	{
		SODmCMainView* mainview = getMainView();

		if (getObjectState() == SOCltElement::disconnected)
		{
			mainview->changeSelectedServerDA(NULL, FALSE);
		}
		else
		{
			mainview->changeSelectedServerDA(this, FALSE);
		}
	}

	getPropertyFrame()->getPropertySheet()->setObjectState((SOCmnObject*)this, getObjectState());
} // onSetObjectState


//-----------------------------------------------------------------------------
// onSetTargetObjectState
//
// - called after the state of the target object changed
//
void SODmCComServerDA::onSetTargetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setTargetObjectState((SOCmnObject*)this, getTargetObjectState());
} // onSetTargetObjectState


//-----------------------------------------------------------------------------
// onError
//
// - called after an error occured
//
void SODmCComServerDA::onError(IN HRESULT res, IN DWORD errorcode)
{
	SODmCMainView* mainview = getMainView();
	SOCmnString et;
	getErrorDescription(errorcode, et, 0x0800);
	mainview->insertError((SOCltElement*)this, res, errorcode, et);
} // onError


//-----------------------------------------------------------------------------
// beforeInterfaceCall
//
// - called before an COM interface call
//
void SODmCComServerDA::beforeInterfaceCall(void)
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
void SODmCComServerDA::afterInterfaceCall(void)
{
	getApp()->EndWaitCursor();
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->removeFromWorkingList(this);
		INVOKE_CALLBACK(SODmCComServerDA, onSetTargetObjectState);
	}
} // afterInterfaceCall


//-----------------------------------------------------------------------------
// Serialization
BOOL SODmCComServerDA::serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring)
{
	if (!SODaCServer::serialize(ar, isStoring))
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

SOCltComServerImpl* SODmCComServerDA::getObjectCltComServerImpl(void)
{
	return (SOCltComServerImpl*)this;
}

#endif // SODMC_DCOM

#ifdef SODMC_SOAP

//-----------------------------------------------------------------------------
// SODmCXmlServerDA                                                           |
//-----------------------------------------------------------------------------

SODmCXmlServerDA::SODmCXmlServerDA()
{
}

//-----------------------------------------------------------------------------
// setParentBranch
//
// - called to insert object into the tree
//
BOOL SODmCXmlServerDA::setParentBranch(IN SOCmnElement* parent)
{
	if (!SODaCServer::setParentBranch(parent))
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

	if (m_parent)
	{
		m_treeHandle = SODmCTreeInsert(&objview->GetTreeCtrl(), objview->getTreeHandleDA(), getURL(), TRUE, (SOCltElement*)this, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, TVI_LAST);

		if (IsWindow(objview->GetTreeCtrl().m_hWnd))
		{
			objview->GetTreeCtrl().Expand(objview->getTreeHandleDA(), TVE_EXPAND);
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
		SOCmnPointer<SODaCEntry> entry = ::getSODaCEntry();
		SOCmnList<SOCmnElement> serverList = entry->getBranchList();

		if (serverList.isEmpty())
		{
			objview->deleteDATree();
		}
	}

	return TRUE;
} // setParentBranch


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - called after the state of the object changed
//
void SODmCXmlServerDA::onSetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	SOCmnPointer<SODaCServer>selServer = getObjectView()->getSelectedServerDA();

	if ((SODaCServer*)selServer == (SODaCServer*)this)
	{
		SODmCMainView* mainview = getMainView();

		if (getObjectState() == SOCltElement::disconnected)
		{
			mainview->changeSelectedServerDA(NULL, FALSE);
		}
		else
		{
			mainview->changeSelectedServerDA(this, FALSE);
		}
	}

	getPropertyFrame()->getPropertySheet()->setObjectState((SOCmnObject*)this, getObjectState());
} // onSetObjectState


//-----------------------------------------------------------------------------
// onSetTargetObjectState
//
// - called after the state of the target object changed
//
void SODmCXmlServerDA::onSetTargetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setTargetObjectState((SOCmnObject*)this, getTargetObjectState());
} // onSetTargetObjectState


//-----------------------------------------------------------------------------
// onError
//
// - called after an error occured
//
void SODmCXmlServerDA::onError(IN HRESULT res, IN DWORD errorcode)
{
	SODmCMainView* mainview = getMainView();
	SOCmnString et;
	getErrorDescription(errorcode, et, 0x0800);
	mainview->insertError((SOCltElement*)this, res, errorcode, et);
} // onError


//-----------------------------------------------------------------------------
// beforeInterfaceCall
//
// - called before an COM interface call
//
void SODmCXmlServerDA::beforeInterfaceCall(void)
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
void SODmCXmlServerDA::afterInterfaceCall(void)
{
	getApp()->EndWaitCursor();
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->removeFromWorkingList(this);
		INVOKE_CALLBACK(SODmCXmlServerDA, onSetTargetObjectState);
	}
} // afterInterfaceCall

#endif // SODMC_SOAP

#ifdef SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// SODmCTPServerDA                                                           |
//-----------------------------------------------------------------------------

SODmCTPServerDA::SODmCTPServerDA()
{
}

//-----------------------------------------------------------------------------
// setParentBranch
//
// - called to insert object into the tree
//
BOOL SODmCTPServerDA::setParentBranch(IN SOCmnElement* parent)
{
	if (!SODaCServer::setParentBranch(parent))
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

	if (m_parent)
	{
		m_treeHandle = SODmCTreeInsert(&objview->GetTreeCtrl(), objview->getTreeHandleDA(), getURL(), TRUE, (SOCltElement*)this, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, TVI_LAST);

		if (IsWindow(objview->GetTreeCtrl().m_hWnd))
		{
			objview->GetTreeCtrl().Expand(objview->getTreeHandleDA(), TVE_EXPAND);
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
		SOCmnPointer<SODaCEntry> entry = ::getSODaCEntry();
		SOCmnList<SOCmnElement> serverList = entry->getBranchList();

		if (serverList.isEmpty())
		{
			objview->deleteDATree();
		}
	}

	return TRUE;
} // setParentBranch


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - called after the state of the object changed
//
void SODmCTPServerDA::onSetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	SOCmnPointer<SODaCServer>selServer = getObjectView()->getSelectedServerDA();

	if ((SODaCServer*)selServer == (SODaCServer*)this)
	{
		SODmCMainView* mainview = getMainView();

		if (getObjectState() == SOCltElement::disconnected)
		{
			mainview->changeSelectedServerDA(NULL, FALSE);
		}
		else
		{
			mainview->changeSelectedServerDA(this, FALSE);
		}
	}

	getPropertyFrame()->getPropertySheet()->setObjectState((SOCmnObject*)this, getObjectState());
} // onSetObjectState


//-----------------------------------------------------------------------------
// onSetTargetObjectState
//
// - called after the state of the target object changed
//
void SODmCTPServerDA::onSetTargetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setTargetObjectState((SOCmnObject*)this, getTargetObjectState());
} // onSetTargetObjectState


//-----------------------------------------------------------------------------
// onError
//
// - called after an error occured
//
void SODmCTPServerDA::onError(IN HRESULT res, IN DWORD errorcode)
{
	SODmCMainView* mainview = getMainView();
	SOCmnString et;
	getErrorDescription(errorcode, et, 0x0800);
	mainview->insertError((SOCltElement*)this, res, errorcode, et);
} // onError


//-----------------------------------------------------------------------------
// beforeInterfaceCall
//
// - called before an COM interface call
//
void SODmCTPServerDA::beforeInterfaceCall(void)
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
void SODmCTPServerDA::afterInterfaceCall(void)
{
	getApp()->EndWaitCursor();
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->removeFromWorkingList(this);
		INVOKE_CALLBACK(SODmCTPServerDA, onSetTargetObjectState);
	}
} // afterInterfaceCall

#endif // SOFEATURE_TUNNEL


//-----------------------------------------------------------------------------
// SODmCPPServerDA                                                            |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPServerDA, SODmCPPage)

BEGIN_MESSAGE_MAP(SODmCPPServerDA, SODmCPPage)
END_MESSAGE_MAP()

SODmCPPServerDA::SODmCPPServerDA(IN SODmCPSheet* sheet, IN UINT resID)
	: SODmCPPage(sheet, resID)
{
	m_obj = NULL;
}

SODmCPPServerDA::~SODmCPPServerDA()
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
BOOL SODmCPPServerDA::connect(void)
{
	if (m_obj)
	{
		if (!m_obj->isConnected())
		{
			return SUCCEEDED(m_obj->connect(FALSE));
		}
		else
		{
			return TRUE;
		}
	}

	return FALSE;
} // connect




//-----------------------------------------------------------------------------
// SODmCPPServerDAAttributes                                                  |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPServerDAAttributes, SODmCPPServerDA)

BEGIN_MESSAGE_MAP(SODmCPPServerDAAttributes, SODmCPPServerDA)
	ON_BN_CLICKED(IDC_SERVER_VALID_LOCALE_ID, onClickLocaleID)
	ON_BN_CLICKED(IDC_SERVER_VALID_CLIENT_NAME, onClickClientName)
END_MESSAGE_MAP()

SODmCPPServerDAAttributes::SODmCPPServerDAAttributes(IN SODmCPSheet* sheet)
	: SODmCPPServerDA(sheet, IDD_PP_SERVER_ATTRIBUTES)
{
	m_validLocaleID = FALSE;
	m_validClientName = FALSE;
	m_edURL.m_firstControl = TRUE;
	m_cbUseOpcVersion.m_lastControl = TRUE;
}

SODmCPPServerDAAttributes::~SODmCPPServerDAAttributes()
{
}


//-----------------------------------------------------------------------------
// enableControls
//
// - Virtual method for control enabling
//
void SODmCPPServerDAAttributes::enableControls(IN BOOL enable)  // enable or disable
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
BOOL SODmCPPServerDAAttributes::loadAttributes(void)
{
	if (m_obj)
	{
		m_URL = (LPCTSTR)m_obj->getURL();
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

		m_opcVersion = _T("");
		WORD useOpcVersion = m_obj->getForcedOPCVersion();

		switch (useOpcVersion)
		{
		case SODAC_VERSION_OPC_DAV10A:
			m_useOpcVersion = _T("DA 1.0");
			break;

		case SODAC_VERSION_OPC_DAV20:
			m_useOpcVersion = _T("DA 2.0");
			break;

		default:
			m_useOpcVersion = _T("Default");
			break;
		}

		if (m_obj->isConnected())
		{
			if (m_obj->getSupportedOPCVersion() < SODAC_VERSION_OPC_DAV20)
			{
				// OPC V 1.0A
				m_localeID = _T("");
				m_clientName = _T("?");
				m_validLocaleID = FALSE;
				m_validClientName = FALSE;
				m_opcVersion = _T("DA 1.0");
			}
			else
			{
				queryLocaleIDs(m_obj->getObjectState());

				if (m_obj->getSupportedOPCVersion() == SODAC_VERSION_OPC_DAV20)
				{
					m_opcVersion = _T("DA 2.0");
				}
				else if (m_obj->getSupportedOPCVersion() == SODAC_VERSION_OPC_DAV30)
				{
					m_opcVersion = _T("DA 3.0");
				}
				else if (m_obj->getSupportedOPCVersion() == SODAC_VERSION_OPC_XMLDAV10)
				{
					m_opcVersion = _T("XML-DA");
				}
				else if (m_obj->getSupportedOPCVersion() == SODAC_VERSION_OPC_TPDA)
				{
					m_opcVersion = _T("Tunnel");
				}
			}
		}

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
BOOL SODmCPPServerDAAttributes::storeAttributes(void)
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

		if (m_oUseOpcVersion != m_useOpcVersion)
		{
			WORD useOpcVersion = SODAC_VERSION_OPC_USEDEFAULT;

			if (m_useOpcVersion == _T("DA 1.0"))
			{
				useOpcVersion = SODAC_VERSION_OPC_DAV10A;
			}
			else if (m_useOpcVersion == _T("DA 2.0"))
			{
				useOpcVersion = SODAC_VERSION_OPC_DAV20;
			}

			reconnect = m_obj->forceOPCVersion(useOpcVersion);
		}

		if (reconnect)
		{
			BYTE targetState = m_obj->getTargetObjectState();
			BOOL started, connected;
			connected = (targetState == SODaCServer::connected);
			started = (targetState == SODaCServer::started);

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
					if (m_obj->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV20)
					{
						m_obj->update(TRUE, getWhat);
					}

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
void SODmCPPServerDAAttributes::clearAttributes(void)
{
	m_URL.Empty();
	m_clientName.Empty();
	m_localeID.Empty();
	m_validLocaleID = FALSE;
	m_validClientName = FALSE;
	m_useOpcVersion = _T("Default");
} // clearAttributes


//-----------------------------------------------------------------------------
// saveCurrentAttributes
//
// - save the current attributes for reset
//
void SODmCPPServerDAAttributes::saveCurrentAttributes(void)
{
	UpdateData(TRUE);
	m_oURL = m_URL;
	m_oClientName = m_clientName;
	m_oLocaleID = m_localeID;
	m_oValidLocaleID = m_validLocaleID;
	m_oValidClientName = m_validClientName;
	m_oOpcVersion = m_opcVersion;
	m_oUseOpcVersion = m_useOpcVersion;
} // saveCurrentAttributes


//-----------------------------------------------------------------------------
// resetCurrentAttributes
//
// - reset the current attributes
//
void SODmCPPServerDAAttributes::resetCurrentAttributes(void)
{
	m_URL = m_oURL;
	m_clientName = m_oClientName;
	m_localeID = m_oLocaleID;
	m_validLocaleID = m_oValidLocaleID;
	m_validClientName = m_oValidClientName;
	m_opcVersion = m_oOpcVersion;
	m_useOpcVersion = m_oUseOpcVersion;
	UpdateData(FALSE);
	enableControls(TRUE);
} // resetCurrentAttributes


//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPServerDAAttributes::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPServerDA::DoDataExchange(pDX);
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
	DDX_Text(pDX, IDC_SERVER_OPC_VERSION, m_opcVersion);
	DDX_Control(pDX, IDC_SERVER_OPC_VERSION, m_edOpcVersion);
	DDX_CBString(pDX, IDC_SERVER_USE_OPC_VERSION, m_useOpcVersion);
	DDX_Control(pDX, IDC_SERVER_USE_OPC_VERSION, m_cbUseOpcVersion);
} // DoDataExchange


//-----------------------------------------------------------------------------
// enableLocaleID
//
// - enable locale id field for OPC V2.0 OPC servers
//
void SODmCPPServerDAAttributes::enableLocaleID(
	IN BOOL enable) // enable or disable
{
	if ((m_obj) && (enable))
	{
		if (m_obj->isConnected())
		{
			enable = (m_obj->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV20);

			if (!enable)
			{
				m_validLocaleID  = FALSE;
			}
		}
	}

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
void SODmCPPServerDAAttributes::enableClientName(IN BOOL enable)    // enable or disable
{
	if ((m_obj) && (enable))
	{
		if (m_obj->isConnected())
		{
			enable = (m_obj->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV20);

			if (!enable)
			{
				m_validClientName  = FALSE;
			}
		}
	}

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
void SODmCPPServerDAAttributes::onClickLocaleID()
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
void SODmCPPServerDAAttributes::onClickClientName()
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
void SODmCPPServerDAAttributes::queryLocaleIDs(IN BYTE state)   // object state
{
	if (state >= SOCltElement::connected)
	{
#ifdef SODMC_DCOM
		SODmCComServerDA* pComObj = NULL;

		if (m_obj->getProtocol() == SOCLT_PROTOCOL_DCOM)
		{
			pComObj = (SODmCComServerDA*)m_obj;
		}

		if (pComObj)
		{
			if (pComObj->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV20)
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
		}

#endif // SODMC_DCOM
	}
} // queryLocaleIDs


//-----------------------------------------------------------------------------
// onSetObjectState
//
// - object state has changed
//
void SODmCPPServerDAAttributes::onSetObjectState(IN BYTE state) // new object state
{
	queryLocaleIDs(state);
	loadAttributes();
} // onSetObjectState




//-----------------------------------------------------------------------------
// SODmCPPServerDAStatus                                                      |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPServerDAStatus, SODmCPPServerDA)

BEGIN_MESSAGE_MAP(SODmCPPServerDAStatus, SODmCPPServerDA)
END_MESSAGE_MAP()

SODmCPPServerDAStatus::SODmCPPServerDAStatus(IN SODmCPSheet* sheet)
	: SODmCPPServerDA(sheet, IDD_PP_SERVER_STATUS)
{
	m_edVendorInfo.m_firstControl = TRUE;
	m_edServerState.m_lastControl = TRUE;
}

SODmCPPServerDAStatus::~SODmCPPServerDAStatus()
{
}

//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPServerDAStatus::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPServerDA::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SERVER_START_TIME, m_startTime);
	DDX_Text(pDX, IDC_SERVER_CURRENT_TIME, m_currentTime);
	DDX_Text(pDX, IDC_SERVER_UPDATE_TIME, m_updateTime);
	DDX_Text(pDX, IDC_SERVER_STATUS_INFO, m_statusInfo);
	DDX_Text(pDX, IDC_SERVER_VERSION, m_version);
	DDX_Text(pDX, IDC_SERVER_VENDOR_INFO, m_vendorInfo);
	DDX_Text(pDX, IDC_SERVER_STATE, m_serverState);
	DDX_Control(pDX, IDC_SERVER_START_TIME, m_edStartTime);
	DDX_Control(pDX, IDC_SERVER_CURRENT_TIME, m_edCurrentTime);
	DDX_Control(pDX, IDC_SERVER_UPDATE_TIME, m_edUpdateTime);
	DDX_Control(pDX, IDC_SERVER_STATUS_INFO, m_edStatusInfo);
	DDX_Control(pDX, IDC_SERVER_VERSION, m_edVersion);
	DDX_Control(pDX, IDC_SERVER_VENDOR_INFO, m_edVendorInfo);
	DDX_Control(pDX, IDC_SERVER_STATE, m_edServerState);
} // DoDataExchange


//-----------------------------------------------------------------------------
// enableControls
//
// - en/disable the controls
//
void SODmCPPServerDAStatus::enableControls(IN BOOL enable)  // enable or disable
{
	enableButton(ID_PROPERTY_RESET, FALSE);
	enableButton(ID_PROPERTY_APPLY, TRUE);
} // enableControls


//-----------------------------------------------------------------------------
// onOK
//
// - reload attributes on OK
//
void SODmCPPServerDAStatus::onOK(void)
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
BOOL SODmCPPServerDAStatus::loadAttributes(void)
{
	if (m_obj)
	{
		if (m_obj->isConnected())
		{
			SODaCServerStatus serverStatus;
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
				m_statusInfo = serverStatus.m_statusInfo;

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
void SODmCPPServerDAStatus::clearAttributes(void)
{
	m_startTime.Empty();
	m_currentTime.Empty();
	m_updateTime.Empty();
	m_statusInfo.Empty();
	m_version.Empty();
	m_vendorInfo.Empty();
	m_serverState.Empty();
} // clearAttributes




//-----------------------------------------------------------------------------
// SODmCPPServerDABrowse                                                      |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPServerDABrowse, SODmCPPServerDA)

BEGIN_MESSAGE_MAP(SODmCPPServerDABrowse, SODmCPPServerDA)
	ON_BN_CLICKED(IDC_DA2_FILTER, onClickDA2)
	ON_BN_CLICKED(IDC_DA3_FILTER, onClickDA3)
END_MESSAGE_MAP()

SODmCPPServerDABrowse::SODmCPPServerDABrowse(IN SODmCPSheet* sheet)
	: SODmCPPServerDA(sheet, IDD_PP_SERVER_BROWSE)
{
	m_edStringFilter.m_firstControl = TRUE;
	m_edVendorFilter.m_lastControl = TRUE;
}

SODmCPPServerDABrowse::~SODmCPPServerDABrowse()
{
}


//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPServerDABrowse::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPServerDA::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_ITEM_DATATYPE, m_datatype);
	DDX_Control(pDX, IDC_ITEM_DATATYPE, m_cbDatatype);
	DDX_CBString(pDX, IDC_ITEM_ACCESS_RIGHTS, m_accessRights);
	DDX_Control(pDX, IDC_ITEM_ACCESS_RIGHTS, m_cbAccessRights);
	DDX_Text(pDX, IDC_ITEM_FILTER, m_stringFilter);
	DDX_Control(pDX, IDC_ITEM_FILTER, m_edStringFilter);
	DDX_Text(pDX, IDC_VENDOR_FILTER, m_vendorFilter);
	DDX_Control(pDX, IDC_VENDOR_FILTER, m_edVendorFilter);
	DDX_Control(pDX, IDC_DA2_FILTER, m_buDA2Filter);
	DDX_Control(pDX, IDC_DA3_FILTER, m_buDA3Filter);
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
BOOL SODmCPPServerDABrowse::loadAttributes(void)
{
#ifdef SODMC_DCOM
	SODmCComServerDA* pComObj = NULL;

	if (m_obj->getProtocol() == SOCLT_PROTOCOL_DCOM)
	{
		pComObj = (SODmCComServerDA*)m_obj;
	}

	if (pComObj)
	{
		SOCmnString str;

		if (pComObj->getAccessRightsFilter() == 0)
		{
			m_accessRights = _T("ALL ACCESS RIGHTS");
		}
		else
		{
			m_accessRights = accessRights2string(&str, pComObj->getAccessRightsFilter());
		}

		m_stringFilter = pComObj->getStringFilter();
		m_vendorFilter = pComObj->getVendorFilter();
		m_datatype = vartype2string(&str, pComObj->getDatatypeFilter());
		UpdateData(FALSE);
		return TRUE;
	}

#endif // SODMC_DCOM
	return FALSE;
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
BOOL SODmCPPServerDABrowse::storeAttributes(void)
{
#ifdef SODMC_DCOM
	SODmCComServerDA* pComObj = NULL;

	if (m_obj->getProtocol() == SOCLT_PROTOCOL_DCOM)
	{
		pComObj = (SODmCComServerDA*)m_obj;
	}

	if (pComObj)
	{
		UpdateData(TRUE);
		pComObj->setStringFilter(m_stringFilter);

		if (m_buDA3Filter.GetCheck() == BST_CHECKED)
		{
			pComObj->setDatatypeFilter(VT_EMPTY);
			pComObj->setAccessRightsFilter(0);
			pComObj->setVendorFilter(m_vendorFilter);
		}
		else
		{
			pComObj->setVendorFilter(_T(""));
			pComObj->setDatatypeFilter(string2vartype(m_datatype));
			pComObj->setAccessRightsFilter(string2accessRights(m_accessRights));
		}

		SOCmnPointer<SODaCServer> srv = getObjectView()->getSelectedServerDA();

		if (srv == pComObj)
		{
			SODmCMainView* mainview = getMainView();
			mainview->changeSelectedServerDA(pComObj, TRUE);
		}

		return TRUE;
	}

#endif // SODMC_DCOM
	return FALSE;
} // storeAttributes


//-----------------------------------------------------------------------------
// clearAttributes
//
// - clear the attribute values of the mask
//
void SODmCPPServerDABrowse::clearAttributes(void)
{
	m_accessRights = _T("ALL ACCESS RIGHTS");
	m_stringFilter = _T("");
	m_vendorFilter = _T("");
	m_datatype = _T("EMPTY");
} // clearAttributes


//-----------------------------------------------------------------------------
// saveCurrentAttributes
//
// - save the current attributes for reset
//
void SODmCPPServerDABrowse::saveCurrentAttributes(void)
{
	UpdateData(TRUE);
	m_oAccessRights = m_accessRights;
	m_oStringFilter = m_stringFilter;
	m_oVendorFilter = m_vendorFilter;
	m_oDatatype = m_datatype;
} // saveCurrentAttributes


//-----------------------------------------------------------------------------
// resetCurrentAttributes
//
// - reset the current attributes
//
void SODmCPPServerDABrowse::resetCurrentAttributes(void)
{
	m_accessRights = m_oAccessRights;
	m_stringFilter = m_oStringFilter;
	m_vendorFilter = m_oVendorFilter;
	m_datatype = m_oDatatype;
	UpdateData(FALSE);
} // resetCurrentAttributes


//-----------------------------------------------------------------------------
// enableControls
//
// - en/disable the controls
//
void SODmCPPServerDABrowse::enableControls(IN BOOL enable)  // enable or disable
{
	enableButton(ID_PROPERTY_APPLY, TRUE);
	enableButton(ID_PROPERTY_RESET, TRUE);

	if (m_obj)
	{
		if (m_obj->getSupportedOPCVersion() >= SODAC_VERSION_OPC_DAV30)
		{
			m_buDA3Filter.EnableWindow(TRUE);
			m_edVendorFilter.SetReadOnly(FALSE);
			m_buDA3Filter.SetCheck(BST_CHECKED);
		}
		else
		{
			m_buDA3Filter.EnableWindow(FALSE);
			m_edVendorFilter.SetReadOnly(TRUE);
			m_buDA2Filter.SetCheck(BST_CHECKED);
		}
	}
} // enableControls

//-----------------------------------------------------------------------------
// onClickDA2
//
// - clicked DA2 filter check button
//
void SODmCPPServerDABrowse::onClickDA2()
{
	m_buDA3Filter.SetCheck(BST_UNCHECKED);
} // onClickDA2

//-----------------------------------------------------------------------------
// onClickDA3
//
// - clicked DA3 filter check button
//
void SODmCPPServerDABrowse::onClickDA3()
{
	m_buDA2Filter.SetCheck(BST_UNCHECKED);
} // onClickDA3



//-----------------------------------------------------------------------------
// SODmCPPServerDASecurity                                                    |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPServerDASecurity, SODmCPPServerDA)

BEGIN_MESSAGE_MAP(SODmCPPServerDASecurity, SODmCPPServerDA)
END_MESSAGE_MAP()

SODmCPPServerDASecurity::SODmCPPServerDASecurity(IN SODmCPSheet* sheet)
	: SODmCPPServerDA(sheet, IDD_PP_SERVER_SECURITY)
{
	m_storeSecInfo = FALSE;
	m_edUserName.m_firstControl = TRUE;
	m_buStoreSecInfo.m_lastControl = TRUE;
}

SODmCPPServerDASecurity::~SODmCPPServerDASecurity()
{
}


//-----------------------------------------------------------------------------
// enableControls
//
// - Virtual method for control enabling
//
void SODmCPPServerDASecurity::enableControls(IN BOOL enable)    // enable or disable
{
#ifdef SODMC_DCOM
	SODmCComServerDA* pComObj = NULL;

	if (m_obj->getProtocol() == SOCLT_PROTOCOL_DCOM)
	{
		pComObj = (SODmCComServerDA*)m_obj;
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

#endif // SODMC_DCOM
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
BOOL SODmCPPServerDASecurity::loadAttributes(void)
{
#ifdef SODMC_DCOM
	SODmCComServerDA* pComObj = NULL;

	if (m_obj->getProtocol() == SOCLT_PROTOCOL_DCOM)
	{
		pComObj = (SODmCComServerDA*)m_obj;
	}

	if (pComObj)
	{
		m_userName = pComObj->getUserName();
		m_password = pComObj->getPassword();
		m_storeSecInfo = !m_userName.IsEmpty();
		UpdateData(FALSE);
		return TRUE;
	}

#endif // SODMC_DCOM
	return FALSE;
} // loadAttributes


//-----------------------------------------------------------------------------
// storeAttributes
//
// - store the current attributes to the object
// returns:
//		TRUE  - changed attributes stored
//		FALSE - no object set
//
BOOL SODmCPPServerDASecurity::storeAttributes(void)
{
#ifdef SODMC_DCOM
	SODmCComServerDA* pComObj = NULL;

	if (m_obj->getProtocol() == SOCLT_PROTOCOL_DCOM)
	{
		pComObj = (SODmCComServerDA*)m_obj;
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

#endif // SODMC_DCOM
	return FALSE;
} // storeAttributes


//-----------------------------------------------------------------------------
// clearAttributes
//
// - clear the attribute values of the mask
//
void SODmCPPServerDASecurity::clearAttributes(void)
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
void SODmCPPServerDASecurity::saveCurrentAttributes(void)
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
void SODmCPPServerDASecurity::resetCurrentAttributes(void)
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
void SODmCPPServerDASecurity::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPServerDA::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SERVER_USER_NAME, m_userName);
	DDX_Control(pDX, IDC_SERVER_USER_NAME, m_edUserName);
	DDX_Text(pDX, IDC_SERVER_PASSWORD, m_password);
	DDX_Control(pDX, IDC_SERVER_PASSWORD, m_edPassword);
	DDX_Check(pDX, IDC_SERVER_STORE_SEC_INFO, m_storeSecInfo);
	DDX_Control(pDX, IDC_SERVER_STORE_SEC_INFO, m_buStoreSecInfo);
} // DoDataExchange






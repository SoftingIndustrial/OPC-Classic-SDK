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
//  Filename    : SODmCSubscription.cpp                                       |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : AE subscription classes                                     |
//                - SODmCSubscription: Toolkit subscription object class      |
//                - SODmCPPSubscription: Base class for subscription          |
//                                       property pages                       |
//                - SODmCPPSubscriptionAttributes: Attribute property page    |
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
#include "SODmCSubscription.h"
#include "SODmCObjectView.h"
#include "SODmCMainView.h"
#include "SODmCServerAE.h"


//-----------------------------------------------------------------------------
// SODmCComSubscription                                                          |
//-----------------------------------------------------------------------------

SODmCComSubscription::SODmCComSubscription()
{
	m_treeHandle = 0;
	m_label = _T("Subscription");
}

//-----------------------------------------------------------------------------
// onSetParentBranch
//
// - called after toolkit object is insertet into the tree
//
void SODmCComSubscription::onSetParentBranch()
{
	SODmCObjectView* objview = getObjectView();

	if (!objview)
	{
		return;
	}

	if (m_parent)
	{
		m_treeHandle = SODmCTreeInsert(&objview->GetTreeCtrl(), ((SODmCComServerAE*)m_parent)->getTreeHandle(), getLabel(), FALSE, (SOCltElement*)this, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, TVI_LAST);

		if (IsWindow(objview->GetTreeCtrl().m_hWnd))
		{
			objview->GetTreeCtrl().Expand(((SODmCComServerAE*)m_parent)->getTreeHandle(), TVE_EXPAND);
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
// onSetObjectState
//
// - called after the state of the object changed
//
void SODmCComSubscription::onSetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	SOCmnPointer<SOAeCSubscription> selSub = getObjectView()->getSelectedSubscription();

	if ((SOAeCSubscription*)selSub == this)
	{
		SODmCMainView* mainview = getMainView();

		if (getObjectState() == SOCltElement::disconnected)
		{
			mainview->changeSelectedSubscription(NULL, FALSE);
		}
		else
		{
			mainview->changeSelectedSubscription(this, FALSE);
		}
	}

	if (getObjectState() == SOCltElement::started)
	{
		refresh();    // fills condition list
	}
	else
	{
		// clear conditon list
		SODmCMainView* mainview = getMainView();
		mainview->getListAEC()->DeleteAllItems();
	}
} // onSetObjectState


//-----------------------------------------------------------------------------
// onSetTargetObjectState
//
// - called after the state of the target object changed
//
void SODmCComSubscription::onSetTargetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setTargetObjectState((SOCmnObject*)this, getTargetObjectState());
} // onSetTargetObjectState


//-----------------------------------------------------------------------------
// onError
//
// - called after an error occured
//
void SODmCComSubscription::onError(IN HRESULT res, IN DWORD errorcode)
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
void SODmCComSubscription::beforeInterfaceCall(void)
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
void SODmCComSubscription::afterInterfaceCall(void)
{
	getApp()->EndWaitCursor();
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->removeFromWorkingList(this);
		INVOKE_CALLBACK(SODmCComSubscription, onSetTargetObjectState);
	}
} // afterInterfaceCall


//-----------------------------------------------------------------------------
// setLabel
//
// - set tree control label string
//
void SODmCComSubscription::setLabel(LPCTSTR label)
{
	m_label = label;
} // setLabel


//-----------------------------------------------------------------------------
// serialize
//
// - serialize additional data
//
BOOL SODmCComSubscription::serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring)
{
	if (!SOAeCSubscription::serialize(ar, isStoring))
	{
		return FALSE;
	}

	if (isStoring)
	{
		// store own data
		ar << getLabel();
	}
	else
	{
		SOCmnString label;
		ar >> label;
		setLabel(label);
	}

	return TRUE;
} // serialize

#ifdef SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// SODmCTPSubscription                                                          |
//-----------------------------------------------------------------------------

SODmCTPSubscription::SODmCTPSubscription()
{
	m_treeHandle = 0;
	m_label = _T("Subscription");
}

//-----------------------------------------------------------------------------
// onSetParentBranch
//
// - called after toolkit object is insertet into the tree
//
void SODmCTPSubscription::onSetParentBranch()
{
	SODmCObjectView* objview = getObjectView();

	if (!objview)
	{
		return;
	}

	if (m_parent)
	{
		m_treeHandle = SODmCTreeInsert(&objview->GetTreeCtrl(), ((SODmCTPServerAE*)m_parent)->getTreeHandle(), getLabel(), FALSE, (SOCltElement*)this, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, SODMCOBJECTVIEW_IMG_OBJ_DEFAULT, TVI_LAST);

		if (IsWindow(objview->GetTreeCtrl().m_hWnd))
		{
			objview->GetTreeCtrl().Expand(((SODmCTPServerAE*)m_parent)->getTreeHandle(), TVE_EXPAND);
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
// onSetObjectState
//
// - called after the state of the object changed
//
void SODmCTPSubscription::onSetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	SOCmnPointer<SOAeCSubscription> selSub = getObjectView()->getSelectedSubscription();

	if ((SOAeCSubscription*)selSub == this)
	{
		SODmCMainView* mainview = getMainView();

		if (getObjectState() == SOCltElement::disconnected)
		{
			mainview->changeSelectedSubscription(NULL, FALSE);
		}
		else
		{
			mainview->changeSelectedSubscription(this, FALSE);
		}
	}

	if (getObjectState() == SOCltElement::started)
	{
		refresh();    // fills condition list
	}
	else
	{
		// clear conditon list
		SODmCMainView* mainview = getMainView();
		mainview->getListAEC()->DeleteAllItems();
	}
} // onSetObjectState


//-----------------------------------------------------------------------------
// onSetTargetObjectState
//
// - called after the state of the target object changed
//
void SODmCTPSubscription::onSetTargetObjectState(void)
{
	SODmCObjectView::setObjectStateImage(m_treeHandle, -1, getObjectState(), getTargetObjectState());
	getPropertyFrame()->getPropertySheet()->setTargetObjectState((SOCmnObject*)this, getTargetObjectState());
} // onSetTargetObjectState


//-----------------------------------------------------------------------------
// onError
//
// - called after an error occured
//
void SODmCTPSubscription::onError(IN HRESULT res, IN DWORD errorcode)
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
void SODmCTPSubscription::beforeInterfaceCall(void)
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
void SODmCTPSubscription::afterInterfaceCall(void)
{
	getApp()->EndWaitCursor();
	SODmCObjectView* objectview = getObjectView();

	if (objectview)
	{
		objectview->removeFromWorkingList(this);
		INVOKE_CALLBACK(SODmCTPSubscription, onSetTargetObjectState);
	}
} // afterInterfaceCall


//-----------------------------------------------------------------------------
// setLabel
//
// - set tree control label string
//
void SODmCTPSubscription::setLabel(LPCTSTR label)
{
	m_label = label;
} // setLabel


//-----------------------------------------------------------------------------
// serialize
//
// - serialize additional data
//
BOOL SODmCTPSubscription::serialize(IN OUT SOCltStorage& ar, IN BOOL isStoring)
{
	if (!SOAeCSubscription::serialize(ar, isStoring))
	{
		return FALSE;
	}

	if (isStoring)
	{
		// store own data
		ar << getLabel();
	}
	else
	{
		SOCmnString label;
		ar >> label;
		setLabel(label);
	}

	return TRUE;
} // serialize

#endif // end SODmCTPSubscription

//-----------------------------------------------------------------------------
// SODmCPPSubscription                                                                |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPSubscription, SODmCPPage)

BEGIN_MESSAGE_MAP(SODmCPPSubscription, SODmCPPage)
END_MESSAGE_MAP()

SODmCPPSubscription::SODmCPPSubscription(
	IN SODmCPSheet* sheet,  // property sheet
	IN UINT resID)      // resource id of property page
	: SODmCPPage(sheet, resID)
{
	m_obj = NULL;
}

SODmCPPSubscription::~SODmCPPSubscription()
{
}




//-----------------------------------------------------------------------------
// SODmCPPSubscriptionAttributes                                                      |
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(SODmCPPSubscriptionAttributes, SODmCPPSubscription)

BEGIN_MESSAGE_MAP(SODmCPPSubscriptionAttributes, SODmCPPSubscription)
END_MESSAGE_MAP()

SODmCPPSubscriptionAttributes::SODmCPPSubscriptionAttributes(IN SODmCPSheet* sheet)
	: SODmCPPSubscription(sheet, IDD_PP_SUBSCRIPTION_ATTRIBUTES)
{
	m_obj = NULL;
	m_edBufferTime.m_firstControl = TRUE;
	m_edClientHandle.m_lastControl = TRUE;
}

SODmCPPSubscriptionAttributes::~SODmCPPSubscriptionAttributes()
{
}


//-----------------------------------------------------------------------------
// DoDataExchange
//
// - do data exchange between class members and controls
//
void SODmCPPSubscriptionAttributes::DoDataExchange(CDataExchange* pDX)
{
	SODmCPPSubscription::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SUBSCRIPTION_MAX_SIZE, m_maxSize);
	DDX_Control(pDX, IDC_SUBSCRIPTION_MAX_SIZE, m_edMaxSize);
	DDX_Text(pDX, IDC_SUBSCRIPTION_BUFFER_TIME, m_bufferTime);
	DDX_Control(pDX, IDC_SUBSCRIPTION_BUFFER_TIME, m_edBufferTime);
	DDX_Text(pDX, IDC_SUBSCRIPTION_CLIENT_HANDLE, m_clientHandle);
	DDX_Control(pDX, IDC_SUBSCRIPTION_CLIENT_HANDLE, m_edClientHandle);
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
BOOL SODmCPPSubscriptionAttributes::loadAttributes(void)
{
	if (m_obj)
	{
		m_maxSize = m_obj->getMaxSize();
		m_bufferTime = m_obj->getBufferTime();
		m_clientHandle.Format(_T("0x%8.8X"), m_obj);
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
BOOL SODmCPPSubscriptionAttributes::storeAttributes(void)
{
	if (m_obj)
	{
		DWORD updateWhat = 0;
		UpdateData(TRUE);

		if (m_oMaxSize != m_maxSize)
		{
			m_obj->setMaxSize(m_maxSize);
			updateWhat |= SOAEC_SUBSPAR_MAXSIZE;
			getApp()->setDirtyFlag();
		}

		if (m_oBufferTime != m_bufferTime)
		{
			m_obj->setBufferTime(m_bufferTime);
			updateWhat |= SOAEC_SUBSPAR_BUFFERTIME;
			getApp()->setDirtyFlag();
		}

		// write changed parameters to server
		if (updateWhat)
		{
			m_obj->update(FALSE, updateWhat);
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
void SODmCPPSubscriptionAttributes::clearAttributes(void)
{
	m_clientHandle.Empty();
	m_bufferTime = 0;
	m_maxSize = 0;
} // clearAttributes


//-----------------------------------------------------------------------------
// saveCurrentAttributes
//
// - save the current attributes for reset
//
void SODmCPPSubscriptionAttributes::saveCurrentAttributes(void)
{
	UpdateData(TRUE);
	m_oMaxSize = m_maxSize;
	m_oBufferTime = m_bufferTime;
} // saveCurrentAttributes


//-----------------------------------------------------------------------------
// resetCurrentAttributes
//
// - reset the current attributes
//
void SODmCPPSubscriptionAttributes::resetCurrentAttributes(void)
{
	m_maxSize = m_oMaxSize;
	m_bufferTime = m_oBufferTime;
	UpdateData(FALSE);
} // resetCurrentAttributes


//-----------------------------------------------------------------------------
// enableControls
//
// - en/disable the controls
//
void SODmCPPSubscriptionAttributes::enableControls(IN BOOL enable)  // enable or disable
{
	enableButton(ID_PROPERTY_APPLY, enable);
	enableButton(ID_PROPERTY_RESET, enable);
} // enableControls



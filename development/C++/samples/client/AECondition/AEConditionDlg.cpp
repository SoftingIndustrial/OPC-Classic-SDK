#include "stdafx.h"

#include "AECondition.h"
#include "AEConditionDlg.h"

#include "ClientEnums.h"
#include "ClientApplication.h"
#include "ClientCommon.h"
#include ".\aeconditiondlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SoftingOPCToolboxClient;

//-------------------------------------------------------------
//MyAeSubscription
//--------------------------------------------------------------

tstring MyAeSubscription::getStateToString(EnumConditionState state)
{
	tstring stateToString = _T("");

	if ((state & EnumConditionState_ACTIVE) == EnumConditionState_ACTIVE)
	{
		stateToString += _T(" ACT");
	}

	if ((state & EnumConditionState_ENABLED) == EnumConditionState_ENABLED)
	{
		stateToString += _T(" ENA");
	}

	if ((state & EnumConditionState_ACKED) == EnumConditionState_ACKED)
	{
		stateToString += _T(" ACK");
	}

	if (state == 0)
	{
		stateToString = _T("DIS");
	}

	return stateToString;
} //end getStateToString

void MyAeSubscription::handleAeConditionsChanged(const std::vector<AeCondition*>& someAeConditions)
{
	unsigned long i;
	int nItem;
	CAEConditionDlg::conditionList.DeleteAllItems();
	ASSERT(CAEConditionDlg::conditionList.GetItemCount() == 0);

	for (i = 0; i < someAeConditions.size(); i++)
	{
		AeCondition* condition = someAeConditions[i];
		tstring state = getStateToString(condition->getState());
		LPTSTR lvItemText = _tcsdup(state.c_str());
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.lParam = (LPARAM)condition;
		lvItem.iItem = 0;
		lvItem.iSubItem = 0;
		lvItem.pszText = lvItemText;
		nItem = CAEConditionDlg::conditionList.InsertItem(&lvItem);
		_TCHAR severity[6];
		_stprintf(severity, _T("%d"), condition->getSeverity());
		tstring time = condition->getOcurrenceTime().toString();
		CAEConditionDlg::conditionList.SetItemText(nItem, 1, condition->getSourcePath().c_str());
		CAEConditionDlg::conditionList.SetItemText(nItem, 2, condition->getConditionName().c_str());
		CAEConditionDlg::conditionList.SetItemText(nItem, 3, severity);
		CAEConditionDlg::conditionList.SetItemText(nItem, 4, condition->getMessage().c_str());
		CAEConditionDlg::conditionList.SetItemText(nItem, 5, time.c_str());
		CAEConditionDlg::conditionList.SetItemText(nItem, 6, condition->getActorId().c_str());
		CAEConditionDlg::conditionList.SetItemText(nItem, 7, condition->getSubConditionName().c_str());
		free(lvItemText);
	} //end for
} //end handleAeConditionsChanged

/////////////////////////////////////////////////////////////////////////////
// CAEConditionDlg dialog
CListCtrl CAEConditionDlg::conditionList;
CAEConditionDlg::CAEConditionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAEConditionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAEConditionDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAEConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, conditionList);
	DDX_Control(pDX, IDC_CONDITON_STATE_LIST, m_conditionStateList);
}

BEGIN_MESSAGE_MAP(CAEConditionDlg, CDialog)
	//{{AFX_MSG_MAP(CAEConditionDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ACKNOWLEDGE_CONDITIONS, OnBnClickedAcknowledgeConditions)
	ON_BN_CLICKED(IDC_ENABLE_CONDITIONS, OnBnClickedEnableConditions)
	ON_BN_CLICKED(IDC_DISABLE_CONDITIONS, OnBnClickedDisableConditions)
	ON_BN_CLICKED(IDC_BUTTON_CONDITION_STATE, OnBnClickedButtonConditionState)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAEConditionDlg message handlers

CAEConditionDlg::~CAEConditionDlg()
{
	if (m_session->getCurrentState() != EnumObjectState_DISCONNECTED)
	{
		m_session->disconnect(NULL);
	}

	m_application->removeAeSession(m_session);
	m_application->terminate();
	releaseApplication();
	delete m_session;
	delete m_subscription;
}
BOOL CAEConditionDlg::OnInitDialog()
{
	long result = S_OK;
	CDialog::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon
	VERIFY(m_imgList.Create(IDB_BITMAP_TREE, 16, 0, RGB(255, 0, 255)));
	conditionList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	conditionList.InsertColumn(0, _T("ConditionState"), LVCFMT_LEFT, 90, -1);
	conditionList.InsertColumn(1, _T("Source"), LVCFMT_LEFT, 150, -1);
	conditionList.InsertColumn(2, _T("ConditionName"), LVCFMT_LEFT, 100, -1);
	conditionList.InsertColumn(3, _T("Severity"), LVCFMT_LEFT, 80, -1);
	conditionList.InsertColumn(4, _T("Message"), LVCFMT_LEFT, 100, -1);
	conditionList.InsertColumn(5, _T("Time"), LVCFMT_LEFT, 100, -1);
	conditionList.InsertColumn(6, _T("ActorID"), LVCFMT_LEFT, 100, -1);
	conditionList.InsertColumn(7, _T("SubConditionName"), LVCFMT_LEFT, 120, -1);
	m_application = getApplication();
	m_application->setVersionOtb(447);
	m_application->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_CLIENT,
		EnumTraceGroup_CLIENT,
		_T("Trace.txt"),
		1000000,
		2);

	//  TODO - binary license activation
	//  Fill in your binary license activation keys here
	//
	//  NOTE: you can activate one or all of the features at the same time

	//  activate the COM-AE Client Feature
	//result = getApplication()->activate(EnumFeature_AE_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return FALSE;
	}

	//  END TODO - binary license activation

	//Initialize the application
	if (SUCCEEDED(m_application->initialize()))
	{
		m_session = new AeSession(_T("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}"));

		if (!m_session->getValid())
		{
			return FALSE;
		}

		m_subscription = new MyAeSubscription(m_session);
		m_session->connect(TRUE, TRUE, NULL);
		m_subscription->refreshAeConditions(NULL);
	} //end if

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAEConditionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAEConditionDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CAEConditionDlg::OnBnClickedAcknowledgeConditions()
{
	std::vector<AeCondition*> conditions = m_subscription->getAeConditionList();
	std::vector<long> results(conditions.size(), 0);
	m_session->acknowledgeAeConditions(
		_T("aUser"),
		_T("AECondition"),
		conditions,
		results,
		NULL);
}

void CAEConditionDlg::OnBnClickedEnableConditions()
{
	std::vector<AeCondition*> conditions = m_subscription->getAeConditionList();
	std::vector<tstring> sources;
	unsigned long i = 0;

	for (i = 0; i < conditions.size(); i++)
	{
		sources.push_back(conditions[i]->getSourcePath());
	} //end for

	m_session->enableAeConditionsBySource(
		sources,
		true,
		NULL);
	sources.clear();
}

void CAEConditionDlg::OnBnClickedDisableConditions()
{
	std::vector<AeCondition*> conditions = m_subscription->getAeConditionList();
	std::vector<tstring> sources;
	unsigned long i = 0;

	for (i = 0; i < conditions.size(); i++)
	{
		sources.push_back(conditions[i]->getSourcePath());
	} //end for

	m_session->enableAeConditionsBySource(
		sources,
		false,
		NULL);
	sources.clear();
}

void CAEConditionDlg::OnBnClickedButtonConditionState()
{
	std::vector<AeCondition*> conditions = m_subscription->getAeConditionList();

	if (conditions.size() == 0)
	{
		MessageBox(_T("The AE Condition List is empty! Wait until it populates with values!"));
		return;
	} //end if

	AeConditionState conditionState;
	unsigned long count  = 0, result, i;
	std::vector<AeAttribute*> attributes(count, NULL);
	tstring sourcePath = conditions[0]->getSourcePath();
	tstring conditionName = conditions[0]->getConditionName();
	_TCHAR buffer[200];
	memset(buffer, 0, 200);
	result = m_session->getAeConditionState(
				 sourcePath,
				 conditionName,
				 attributes,
				 conditionState,
				 NULL);
	m_conditionStateList.DeleteAllItems();

	if (SUCCEEDED(result))
	{
		_stprintf(buffer, _T(" Source Path: %s"), sourcePath.c_str());
		LVITEM lvItem1;
		lvItem1.mask = LVIF_TEXT;
		lvItem1.iItem = 0;
		lvItem1.iSubItem = 0;
		lvItem1.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem1);
		_stprintf(buffer, _T(" Condition Name: %s"), conditionName.c_str());
		LVITEM lvItem2;
		lvItem2.mask = LVIF_TEXT;
		lvItem2.iItem = 1;
		lvItem2.iSubItem = 0;
		lvItem2.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem2);
		tstring state = m_subscription->getStateToString(conditionState.getState());
		_stprintf(buffer, _T(" State: %s"), state.c_str());
		LVITEM lvItem3;
		lvItem3.mask = LVIF_TEXT;
		lvItem3.iItem = 2;
		lvItem3.iSubItem = 0;
		lvItem3.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem3);
		_stprintf(buffer, _T(" Quality: %d"), conditionState.getQuality());
		LVITEM lvItem4;
		lvItem4.mask = LVIF_TEXT;
		lvItem4.iItem = 3;
		lvItem4.iSubItem = 0;
		lvItem4.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem4);
		tstring activeTime = (conditionState.getConditionActiveTime()).toString();
		_stprintf(buffer, _T(" ActiveTime: %s"), activeTime.c_str());
		LVITEM lvItem5;
		lvItem5.mask = LVIF_TEXT;
		lvItem5.iItem = 4;
		lvItem5.iSubItem = 0;
		lvItem5.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem5);
		tstring inactiveTime = (conditionState.getConditionInactiveTime()).toString();
		_stprintf(buffer, _T(" InactiveTime: %s"), inactiveTime.c_str());
		LVITEM lvItem6;
		lvItem6.mask = LVIF_TEXT;
		lvItem6.iItem = 5;
		lvItem6.iSubItem = 0;
		lvItem6.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem6);
		tstring ackTime = (conditionState.getConditionAckTime()).toString();
		_stprintf(buffer, _T(" AckTime: %s"), ackTime.c_str());
		LVITEM lvItem7;
		lvItem7.mask = LVIF_TEXT;
		lvItem7.iItem = 6;
		lvItem7.iSubItem = 0;
		lvItem7.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem7);
		_stprintf(buffer, _T(" Ack id: %s"), conditionState.getAcknowledgerId().c_str());
		LVITEM lvItem8;
		lvItem8.mask = LVIF_TEXT;
		lvItem8.iItem = 71;
		lvItem8.iSubItem = 0;
		lvItem8.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem8);
		_stprintf(buffer, _T("Ack comment: %s"), conditionState.getAcknowledgerComment().c_str());
		LVITEM lvItem9;
		lvItem9.mask = LVIF_TEXT;
		lvItem9.iItem = 8;
		lvItem9.iSubItem = 0;
		lvItem9.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem9);
		tstring activeSubconditionTime = (conditionState.getSubConditionActiveTime()).toString();
		_stprintf(buffer, _T(" Active subcondition time: %s"), activeSubconditionTime.c_str());
		LVITEM lvItem10;
		lvItem10.mask = LVIF_TEXT;
		lvItem10.iItem = 9;
		lvItem10.iSubItem = 0;
		lvItem10.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem10);
		_stprintf(buffer, _T(" Active subcondition name: %s"), conditionState.getActiveSubConditionName().c_str());
		LVITEM lvItem11;
		lvItem11.mask = LVIF_TEXT;
		lvItem11.iItem = 10;
		lvItem11.iSubItem = 0;
		lvItem11.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem11);
		_stprintf(buffer, _T(" Active subcondition description: %s"), conditionState.getActiveSubConditionDescription().c_str());
		LVITEM lvItem12;
		lvItem12.mask = LVIF_TEXT;
		lvItem12.iItem = 11;
		lvItem12.iSubItem = 0;
		lvItem12.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem12);
		_stprintf(buffer, _T(" Active subcondition definition: %s"), conditionState.getActiveSubConditionDefinition().c_str());
		LVITEM lvItem13;
		lvItem13.mask = LVIF_TEXT;
		lvItem13.iItem = 12;
		lvItem13.iSubItem = 0;
		lvItem13.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem13);
		_stprintf(buffer, _T(" Active subcondition severity: %d"), conditionState.getActiveSubConditionSeverity());
		LVITEM lvItem14;
		lvItem14.mask = LVIF_TEXT;
		lvItem14.iItem = 13;
		lvItem14.iSubItem = 0;
		lvItem14.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem14);
		std::vector<tstring> subConditionNames = conditionState.getSubConditionsNames();
		std::vector<tstring> subConditionsDefinitions = conditionState.getSubConditionsDefinitions();
		std::vector<tstring> subConditionsDescriptions = conditionState.getSubConditionsDescriptions();
		std::vector<unsigned long> subConditionsSeverities = conditionState.getSubConditionsSeverities();
		_stprintf(buffer, _T(" Number of subconditions: %d"), subConditionNames.size());
		LVITEM lvItem15;
		lvItem15.mask = LVIF_TEXT;
		lvItem15.iItem = 14;
		lvItem15.iSubItem = 0;
		lvItem15.pszText = buffer;
		m_conditionStateList.InsertItem(&lvItem15);

		for (i = 0; i < subConditionNames.size(); i++)
		{
			_stprintf(buffer, _T("[%3.3d]Subcondition name: %s"), i, subConditionNames[i].c_str());
			LVITEM lvItem1;
			lvItem1.mask = LVIF_TEXT;
			lvItem1.iItem = 15 + i;
			lvItem1.iSubItem = 0;
			lvItem1.pszText = buffer;
			m_conditionStateList.InsertItem(&lvItem1);
			_stprintf(buffer, _T("[%3.3d]Subcondition definition: %s"), i, subConditionsDefinitions[i].c_str());
			LVITEM lvItem2;
			lvItem2.mask = LVIF_TEXT;
			lvItem2.iItem = 15 + i;
			lvItem2.iSubItem = 0;
			lvItem2.pszText = buffer;
			m_conditionStateList.InsertItem(&lvItem2);
			_stprintf(buffer, _T("[%3.3d]Subcondition description: %s"), i, subConditionsDescriptions[i].c_str());
			LVITEM lvItem3;
			lvItem3.mask = LVIF_TEXT;
			lvItem3.iItem = 15 + i;
			lvItem3.iSubItem = 0;
			lvItem3.pszText = buffer;
			m_conditionStateList.InsertItem(&lvItem3);
			_stprintf(buffer, _T("[%3.3d]Subcondition severity: %d"), i, subConditionsSeverities[i]);
			LVITEM lvItem4;
			lvItem4.mask = LVIF_TEXT;
			lvItem4.iItem = 15 + i;
			lvItem4.iSubItem = 0;
			lvItem4.pszText = buffer;
			m_conditionStateList.InsertItem(&lvItem4);
		} //end for
	} //end if
}

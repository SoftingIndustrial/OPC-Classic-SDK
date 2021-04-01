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
//  Filename    : SODmCMultiWriteDlg.cpp                                      |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : classes of advanced write freature                          |
//                - SODmCMultiWriteDlg: main view class                       |
//                                                                            |
//  This code and information is provided "as is" without warranty of         |
//  any kind, either expressed or implied, including but not limited          |
//  to the implied warranties of merchantability and/or fitness for a         |
//  particular purpose.                                                       |
//                                                                            |
//-----------------------------------------------------------------------------


#include "stdafx.h"

#ifdef SODMC_DA

#include "SOClient.h"
#include "SODmCMultiWriteDlg.h"

// SODmCMultiWriteDlg dialog

IMPLEMENT_DYNAMIC(SODmCMultiWriteDlg, CDialog)
SODmCMultiWriteDlg::SODmCMultiWriteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SODmCMultiWriteDlg::IDD, pParent)
{
	m_pGridCtrl = NULL;
	m_lstGroupList.create(SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_DATA | SOCMNLIST_TYPE_ORG_MAP);
}

SODmCMultiWriteDlg::~SODmCMultiWriteDlg()
{
	m_lstGroupList.destroy();
	delete m_pGridCtrl;
}

void SODmCMultiWriteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL SODmCMultiWriteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	OnInitialUpdate();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void SODmCMultiWriteDlg::OnSize(UINT nType, int cx , int cy)
{
	CDialog::OnSize(nType, cx , cy);
	CButton* pOkButton  = (CButton*) GetDlgItem(IDOK);
	CButton* pCancelButton  = (CButton*) GetDlgItem(IDCANCEL);
	CRect rect;
	pOkButton->GetWindowRect(rect);
	pOkButton->SetWindowPos(NULL, (cx / 2) - (rect.Width() + 10) , cy - 10 - rect.Height(), 0, 0, SWP_NOSIZE);
	pCancelButton->SetWindowPos(NULL, (cx / 2) + 10, cy - 10 - rect.Height(), 0, 0, SWP_NOSIZE);
	m_pGridCtrl->SetWindowPos(NULL, 0, 0, cx, cy - 15 - rect.Height(), SWP_SHOWWINDOW);
}

void SODmCMultiWriteDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	LONG lHeight = lpRect->bottom - lpRect->top;
	LONG lWidth  = lpRect->right - lpRect->left;
	CString szCaption;

	if (lWidth < MULTIWRITE_MINWIDTH)
	{
		switch (nSide)
		{
		case 1:
		case 4:
		case 7:
		{
			lpRect->left = lpRect->right - MULTIWRITE_MINWIDTH;
			break;
		}

		case 2:
		case 5:
		case 8:
		{
			lpRect->right = lpRect->left + MULTIWRITE_MINWIDTH;
			break;
		}

		default:
			break;
		} // end switch
	}

	if (lHeight < MULTIWRITE_MINHEIGHT)
	{
		switch (nSide)
		{
		case 3:
		case 4:
		case 5:
		{
			lpRect->top = lpRect->bottom - MULTIWRITE_MINHEIGHT;
			break;
		}

		case 6:
		case 7:
		case 8:
		{
			lpRect->bottom = lpRect->top + MULTIWRITE_MINHEIGHT;
			break;
		}

		default:
			break;
		} // end switch
	}

	CDialog::OnSizing(nSide, lpRect);
}

void SODmCMultiWriteDlg::OnInitialUpdate()
{
	if (m_pGridCtrl == NULL)
	{
		// Create the Gridctrl object
		m_pGridCtrl = new CGridCtrl;

		if (!m_pGridCtrl)
		{
			return;
		}

		// Create the Gridctrl window
		CRect rect;
		GetClientRect(rect);
		rect.right -= 100;
		m_pGridCtrl->Create(rect, this, IDC_OCTCFGGRID);
		// initialize grid control
		initializeGridCtrl(*m_pGridCtrl);
		addCaptionRowToGridCtrl(*m_pGridCtrl);
	}
}

void SODmCMultiWriteDlg::initializeGridCtrl(CGridCtrl& gridCtrl)
{
	gridCtrl.SetTextBkColor(RGB(0xFF, 0xFF, 0xFF));
	gridCtrl.SetFixedColumnCount(0);
	gridCtrl.SetFixedRowCount(1); // caption row
	gridCtrl.SetRowCount(1);      // One row ought to be enough ... for the begining
	gridCtrl.SetEditable(TRUE);
	gridCtrl.SetColumnResize(TRUE);
	gridCtrl.EnableSelection(FALSE);
	gridCtrl.EnableDragAndDrop(FALSE);
	gridCtrl.EnableTitleTips(FALSE);
	gridCtrl.EnableToolTips(FALSE);
}

void SODmCMultiWriteDlg::getComboOptions(CStringArray& rOptions)
{
	rOptions.Add(_T("EMPTY"));
	rOptions.Add(_T("I1"));
	rOptions.Add(_T("UI1"));
	rOptions.Add(_T("I2"));
	rOptions.Add(_T("UI2"));
	rOptions.Add(_T("I4"));
	rOptions.Add(_T("UI4"));
	rOptions.Add(_T("I8"));
	rOptions.Add(_T("UI8"));
	rOptions.Add(_T("R4"));
	rOptions.Add(_T("R8"));
	rOptions.Add(_T("BOOL"));
	rOptions.Add(_T("BSTR"));
	rOptions.Add(_T("DATE"));
	rOptions.Add(_T("CY"));
	rOptions.Add(_T("I1 ARRAY"));
	rOptions.Add(_T("UI1 ARRAY"));
	rOptions.Add(_T("I2 ARRAY"));
	rOptions.Add(_T("UI2 ARRAY"));
	rOptions.Add(_T("I4 ARRAY"));
	rOptions.Add(_T("UI4 ARRAY"));
	rOptions.Add(_T("I8 ARRAY"));
	rOptions.Add(_T("UI8 ARRAY"));
	rOptions.Add(_T("R4 ARRAY"));
	rOptions.Add(_T("R8 ARRAY"));
	rOptions.Add(_T("BOOL ARRAY"));
	rOptions.Add(_T("BSTR ARRAY"));
	rOptions.Add(_T("DATE ARRAY"));
	rOptions.Add(_T("CY ARRAY"));
	rOptions.Add(_T("VARIANT ARRAY"));
}

void SODmCMultiWriteDlg::addCaptionRowToGridCtrl(CGridCtrl& gridCtrl)
{
	gridCtrl.SetColumnCount(4);
	// set column widths
	CRect rect;
	gridCtrl.GetClientRect(&rect);
	gridCtrl.SetColumnWidth(CFG_COL_ITEM, CFG_COLWIDTH_0);
	gridCtrl.SetColumnWidth(CFG_COL_DATATYPE, CFG_COLWIDTH_1);
	gridCtrl.SetColumnWidth(CFG_COL_VALUE, CFG_COLWIDTH_2);
	gridCtrl.SetColumnWidth(CFG_COL_INDEX, CFG_COLWIDTH_3);
	gridCtrl.ExpandLastColumn();
	//Set format
	gridCtrl.SetItemFormat(0, CFG_COL_ITEM    , DT_CENTER | DT_WORDBREAK);
	gridCtrl.SetItemFormat(0, CFG_COL_DATATYPE, DT_CENTER | DT_WORDBREAK);
	gridCtrl.SetItemFormat(0, CFG_COL_VALUE   , DT_CENTER | DT_WORDBREAK);
	gridCtrl.SetItemFormat(0, CFG_COL_INDEX   , DT_CENTER | DT_WORDBREAK);
	// add caption row
	gridCtrl.SetItemText(0, CFG_COL_ITEM, CFG_SZHEAD_ITEM);
	gridCtrl.SetItemText(0, CFG_COL_DATATYPE, CFG_SZHEAD_DATATYPE);
	gridCtrl.SetItemText(0, CFG_COL_VALUE, CFG_SZHEAD_VALUE);
	gridCtrl.SetItemText(0, CFG_COL_INDEX, CFG_SZHEAD_INDEX);
	SODmCMainView* pMainView = getMainView();
	CListCtrl* pDA_List = pMainView->getListDA();
	UINT uiCnt = pDA_List->GetSelectedCount();
	gridCtrl.SetRowCount(uiCnt + 1);
	POSITION pos = pDA_List->GetFirstSelectedItemPosition();
	int iRow = 0;
	int iGroupCount = 0;

	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = pDA_List->GetNextSelectedItem(pos);
			iRow++;
			SODaCItem* pItem = (SODaCItem*)pDA_List->GetItemData(nItem);
			// Insert ItemID of actual tag
			gridCtrl.SetItemText(iRow, CFG_COL_ITEM, pItem->getItemID());
			gridCtrl.SetItemState(iRow, CFG_COL_ITEM, GVIS_READONLY);
			gridCtrl.SetItemData(iRow, CFG_COL_ITEM, (LPARAM)(SODmCItem*)pItem);

			// combo box with desired Datatype defaulted to native
			if (gridCtrl.SetCellType(iRow, CFG_COL_DATATYPE, RUNTIME_CLASS(CGridCellCombo)))
			{
				CStringArray options;
				getComboOptions(options);
				CGridCellCombo* pCell = (CGridCellCombo*) gridCtrl.GetCell(iRow, CFG_COL_DATATYPE);
				pCell->SetOptions(options);
				pCell->SetStyle(CBS_DROPDOWNLIST); //CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE
				pCell->SetText(VARTYPE2STR(pItem->getNativeDatatype()));
			}

			// Insert Value of actual tag
			SOCmnVariant varItemValue;
			SOCmnString szBuffer;
			pItem->getReadValue(varItemValue);
			VARIANT2STRING(&szBuffer, varItemValue, FALSE);
			gridCtrl.SetItemText(iRow, CFG_COL_VALUE, szBuffer);

			if (((pItem->getAccessRights() & OPC_WRITEABLE) == 0) || (!pItem->isConnected()))
			{
				gridCtrl.SetItemState(iRow, CFG_COL_VALUE, GVIS_READONLY);
				gridCtrl.SetItemBkColour(iRow, CFG_COL_VALUE, COLORREADONLY);
			}

			if ((varItemValue.vt & VT_ARRAY) == VT_ARRAY)
			{
				LONG lBound, uBound;
				LONG i;
				SOCmnString valStr;
				SafeArrayGetLBound(varItemValue.parray, 1, &lBound);
				SafeArrayGetUBound(varItemValue.parray, 1, &uBound);

				if (gridCtrl.SetCellType(iRow, CFG_COL_INDEX, RUNTIME_CLASS(CGridCellCombo)))
				{
					CStringArray options;
					CGridCellCombo* pCell = (CGridCellCombo*) gridCtrl.GetCell(iRow, CFG_COL_INDEX);

					for (i = lBound; i <= uBound; i++)
					{
						valStr.format(_T("%d"), i);
						options.Add(valStr);
					}

					valStr.format(_T("%d"), lBound);
					pCell->SetText(valStr);
					pCell->SetOptions(options);
					pCell->SetStyle(CBS_SIMPLE); //CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE
				}
			}
			else
			{
				gridCtrl.SetItemText(iRow, CFG_COL_INDEX, _T(""));
				gridCtrl.SetItemState(iRow, CFG_COL_INDEX, GVIS_READONLY);
				gridCtrl.SetItemBkColour(iRow, CFG_COL_INDEX, COLORREADONLY);
			}

			if ((pItem->getAccessRights() & OPC_WRITEABLE) && (pItem->isConnected()))
			{
				SOCmnPointer<SODaCGroup> grp = (SODaCGroup*)pItem->getParentBranch();
				SODmCItemList* pList = (SODmCItemList*) m_lstGroupList.findKey(grp->getOpcHandle());

				if (pList != NULL)
				{
					pList->m_itemList.add(pItem);
				}
				else
				{
					pList = new SODmCItemList();

					if (pList != NULL)
					{
						pList->m_itemList.add(pItem);
						pList->m_group = grp;
					}

					m_lstGroupList.add(pList, grp->getOpcHandle());
					iGroupCount++;
				}

				gridCtrl.SetItemData(iRow, CFG_COL_VALUE, (LPARAM)(SODaCGroup*)grp);
			}
		}
	}
}

BEGIN_MESSAGE_MAP(SODmCMultiWriteDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// SODmCMultiWriteDlg message handlers

void SODmCMultiWriteDlg::OnBnClickedOk()
{
	if (m_pGridCtrl != NULL)
	{
		int iCnt = m_pGridCtrl->GetRowCount();
		UpdateData(TRUE);   // Update Values

		for (int iRow = 1 ; iRow < iCnt ; iRow++)
		{
			SODmCItem* pItem = (SODmCItem*) m_pGridCtrl->GetItemData(iRow, CFG_COL_ITEM);
			VARTYPE datatype = string2vartype(m_pGridCtrl->GetItemText(iRow, CFG_COL_DATATYPE));
			SOCmnString valueString(m_pGridCtrl->GetItemText(iRow, CFG_COL_VALUE));
			SOCmnString idxString(m_pGridCtrl->GetItemText(iRow, CFG_COL_INDEX));
			DWORD startIdx = 0;

			if (idxString.isEmpty())
			{
				startIdx = 0;
			}
			else
			{
				startIdx = _ttol(idxString);
			}

			SOCmnVariant value;

			if (datatype == VT_EMPTY)
			{
				datatype = pItem->getNativeDatatype();
			}

			if (convertWriteStringToVariant(pItem, valueString, datatype, startIdx, value))
			{
				pItem->setWriteValue(value);
			}
		}

		SOCmnListPosition pos = m_lstGroupList.getStartPosition();

		while (pos) // loop for each group
		{
			SODmCItemList* pList = m_lstGroupList.GetNext(pos);
			SODaCGroup* pGrp = pList->m_group;

			if (pGrp->getDataRetrieval() == SODaCGroup::drAsyncAdvise)
			{
				pGrp->asyncWrite(&pList->m_itemList);
			}
			else //synched
			{
				pGrp->syncWrite(&pList->m_itemList);
			}
		}
	}
}

#endif // SODMC_DA

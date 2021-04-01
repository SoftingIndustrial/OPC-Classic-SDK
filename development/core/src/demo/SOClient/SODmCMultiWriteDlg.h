#ifndef _SODMCMULTIWRITEDLG_H_
#define _SODMCMULTIWRITEDLG_H_

#ifdef SODMC_DA

#include "GridCtrl_SRC\GridCellCombo.h"
#include "GridCtrl_SRC\GridCellNumeric.h"
#include "GridCtrl_SRC\GridBtnCell.h"
#include "SODmCMainView.h"
#include "SODmCItem.h"
#include "SODmCGroup.h"

#include "resource.h"

// check box and browse button has different columns
#define CFG_COLWIDTH_0  200 // Item
#define CFG_COLWIDTH_1  110 // Datatype
#define CFG_COLWIDTH_2  225 // Value
#define CFG_COLWIDTH_3  75  // StartIndex

// grid control columns
#define CFG_COL_ITEM        0   // item (disabled edit field)
#define CFG_COL_DATATYPE    1   // datatype (dropdown list)
#define CFG_COL_VALUE       2   // value (edit field) 
#define CFG_COL_INDEX       3   // value (edit field) 

#define CFG_SZHEAD_ITEM     _T("Item")  // item's caption   (disabled edit field)
#define CFG_SZHEAD_DATATYPE _T("Datatype")      // datatype (dropdown list)
#define CFG_SZHEAD_VALUE    _T("Value")         // value (edit field) 
#define CFG_SZHEAD_INDEX    _T("Startindex")    // value (edit field) 

#define COLORREADONLY RGB(214,211,206)

#define MULTIWRITE_MINWIDTH 300
#define MULTIWRITE_MINHEIGHT 200

#define IDC_OCTCFGGRID  100

// SODmCMultiWriteDlg dialog


class SODmCItemList : public SOCmnData
{
public:
	SODmCItemList()
	{
		m_itemList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT);
	}
	~SODmCItemList()
	{
		m_itemList.destroy();
		m_group.release();
	}

	SOCmnPointer<SODaCGroup> m_group;
	SOCmnList<SODaCItem> m_itemList;
};

class SODmCMultiWriteDlg : public CDialog
{
	DECLARE_DYNAMIC(SODmCMultiWriteDlg)

public:
	SODmCMultiWriteDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SODmCMultiWriteDlg();

	// AFX Message handlers
	afx_msg void OnSize(UINT nType, int cx , int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnBnClickedOk();

	// Dialog Data
	enum { IDD = IDD_MULTIWRITE };
	CGridCtrl* m_pGridCtrl;
	void initializeGridCtrl(CGridCtrl& gridCtrl);
	void OnInitialUpdate();
	void addCaptionRowToGridCtrl(CGridCtrl& gridCtrl);
	void getComboOptions(CStringArray& rOptions);

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	SOCmnList<SODmCItemList> m_lstGroupList;
};

#endif // SODMC_DA

#endif // _SODMCMULTIWRITEDLG_H_

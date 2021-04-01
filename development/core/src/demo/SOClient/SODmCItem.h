#ifndef _SODMCITEM_H_
#define _SODMCITEM_H_

#include "SODmCPropFrm.h"

#ifdef SODMC_DA

//-----------------------------------------------------------------------------
// SODmCItem                                                                  |
//-----------------------------------------------------------------------------

class SODmCItem : public SODaCItemDefault
{
public:
	SODmCItem();

	HTREEITEM getTreeHandle(void) const;
	void setTreeHandle(HTREEITEM handle);

	void setListIndex(int i);
	int getListIndex(void) const;

	void readFailed(IN HRESULT res, IN WORD quality);
	void writeFailed(IN HRESULT res);

	void writeStringValue(IN LPCTSTR valueString, IN VARTYPE datatype,
						  IN OPTIONAL DWORD startIdx = 0);

	void setLogToFile(BOOL log)
	{
		m_logToFile = log;
	};
	BOOL getLogToFile()
	{
		return m_logToFile;
	};
	void setLogFile(SOCmnString& logFile)
	{
		m_logFile = logFile;
	};
	SOCmnString getLogFile(void)
	{
		return m_logFile;
	};

protected:
	HTREEITEM m_treeHandle;
	int m_listIdx;
	BOOL m_logToFile;
	SOCmnString m_logFile;

	virtual void onSetObjectState(void);
	virtual void onSetParentBranch();
	virtual void onSetTargetObjectState(void);
	virtual void onSetItemID(void);
	virtual void onSetReadValue();
	virtual void onSetReadQuality();
	virtual void onSetReadTimeStamp();
	virtual void onSetReadResult(void);
	virtual void onSetWriteResult(void);
	virtual void onError(IN HRESULT res, IN DWORD errorcode);
	virtual void setRead(IN HRESULT res, IN SOCmnVariant* value, IN WORD quality, IN SOCmnDateTime* timeStamp, IN DWORD transactionID);

	BOOL putArrayElement(IN LPVARIANT array, IN LONG idx, IN LPCTSTR strVal);
}; // SODmCItem

inline HTREEITEM SODmCItem::getTreeHandle(void) const
{
	return m_treeHandle;
}

inline void SODmCItem::setTreeHandle(HTREEITEM handle)
{
	m_treeHandle = handle;
}

inline void SODmCItem::setListIndex(int listIdx)
{
	m_listIdx = listIdx;
}

inline int SODmCItem::getListIndex(void) const
{
	return m_listIdx;
}

inline void SODmCItem::readFailed(IN HRESULT res, IN WORD quality)
{
	setReadResult(res);
	setReadQuality(quality);
}

inline void SODmCItem::writeFailed(IN HRESULT res)
{
	setWriteResult(res);
}




//-----------------------------------------------------------------------------
// SODmCPPItem                                                                |
//-----------------------------------------------------------------------------

class SODmCPPItem  : public SODmCPPage
{
	DECLARE_DYNAMIC(SODmCPPItem)
public:
	SODmCPPItem(IN SODmCPSheet* sheet, IN UINT resId);
	~SODmCPPItem();

	void setObject(IN SODmCItem* itm);  // set item object

protected:
	SODmCItem* m_obj;   // item object

	DECLARE_MESSAGE_MAP()
}; //SODmCPPItem

inline void SODmCPPItem::setObject(IN SODmCItem* itm)
{
	m_obj = itm;
}




//-----------------------------------------------------------------------------
// SODmCPPItemAttributes                                                      |
//-----------------------------------------------------------------------------

class SODmCPPItemAttributes : public SODmCPPItem
{
	DECLARE_DYNAMIC(SODmCPPItemAttributes)
public:
	SODmCPPItemAttributes(IN SODmCPSheet* sheet);
	~SODmCPPItemAttributes();

	void onSetObjectState(BYTE state);  // changed object state
	void onSetItemID(LPCTSTR itemID)
	{
		m_itemID = itemID;
		UpdateData(FALSE);
	}

protected:
	virtual BOOL OnInitDialog(void);                    // initialize page
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// changed item id in edit field
	afx_msg void onChangeID();

	// double click to access path list control
	afx_msg void onDblclkAccessPath(NMHDR* pNMHDR, LRESULT* pResult);

	// check button handlers
	afx_msg void onClickDeadband();

	// SODmCPPage virtuals
	virtual void enableControls(IN BOOL enable);
	virtual BOOL loadAttributes(void);
	virtual BOOL storeAttributes(void);
	virtual void saveCurrentAttributes(void);
	virtual void resetCurrentAttributes(void);
	virtual void clearAttributes(void);

	void insertAccessPaths(void);
	void loadServerGivenAttributes(void);

	// current attributes
	CString m_itemID;
	CString m_accessPath;
	CString m_datatype;
	CString m_serverHandle;
	CString m_clientHandle;
	CString m_nativeDatatype;
	CString m_accessRights;
	CString m_EUType;
	FLOAT m_deadband;
	BOOL m_validDeadband;

	// saved attributes (for reset)
	CString m_oDatatype;
	CString m_oItemID;
	CString m_oAccessPath;
	FLOAT m_oDeadband;
	BOOL m_oValidDeadband;

	// controls
	SODmCPEdit m_edItemID;
	SODmCPEdit m_edAccessPath;
	SODmCPComboBox m_cbDatatype;
	SODmCPEdit m_edClientHandle;
	SODmCPEdit m_edServerHandle;
	SODmCPEdit m_edNativeDatatype;
	SODmCPEdit m_edAccessRights;
	SODmCPEdit m_edEUType;
	SODmCPListCtrl m_liEUInfo;
	SODmCPListCtrl m_liAccessPath;
	SODmCPEdit m_edDeadband;
	SODmCPButton m_buValidDeadband;

	DECLARE_MESSAGE_MAP()
}; // SODmCPPItemAttributes




#endif

#endif

#ifndef _SOCLIENT_H_
#define _SOCLIENT_H_

// SOClient error codes
#define SODMC_ERR_CONVERT_DA_WRITE_VALUE  0xE1000001
#define SODMC_ERR_BAD_READ_RESULT         0xE1000002
#define SODMC_ERR_BAD_WRITE_RESULT        0xE1000003
#define SODMC_ERR_OPEN_FILE               0xE1000004



//-----------------------------------------------------------------------------
// SODmCApp                                                                   |
//-----------------------------------------------------------------------------

// application options
typedef struct _tagSODmCAppOptions
{
	BOOL timeOnly;     // show local time or XML date time string
	BOOL valuePure;    // show value with or without type information
	BOOL cmOn;         // connection monitor on or off
	BOOL viewIOErrors; // show I/O errors in the error pane
	BOOL trcOn;        // enable OPC call trace
	BOOL valueDec;     // enable OPC call trace
	BOOL alphaAEOn;    // enable AE Alphasort
	BOOL alphaItemOn;  // enable DA Alphasort
} SODmCAppOptions;

class SODmCApp : public CWinApp
{
public:
	SODmCApp();

	// application init/exit
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// file menu handler
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg BOOL OnOpenRecentFile(IN UINT nID);

	// version of current file
	DWORD getFileVersion(void)
	{
		return m_fileVersion;
	}

	// dirty flag
	void setDirtyFlag(void)
	{
		m_dirtyFlag = TRUE;
	}
	void resetDirtyFlag(void)
	{
		m_dirtyFlag = FALSE;
	}
	BOOL getDirtyFlag(void)
	{
		return m_dirtyFlag;
	}

	int doSaveChanges(void);

	inline SODmCAppOptions* getOptions(void)
	{
		return &m_options;
	}

	void enableTrace(BOOL enable);

protected:
	SOCmnString m_fileName; // current file name
	DWORD m_fileVersion;    // version of current file
	BOOL m_dirtyFlag;       // file dirty flag
	SODmCAppOptions m_options;  // application options

	void doOpen(IN LPCTSTR fileName);
	void doImport(IN LPCTSTR fileName);
	int doFileNew();

	void doRegister(void);
	void doUnregister(void);

	DECLARE_MESSAGE_MAP()

//>>> SJ 04.09.2008 help functionality added
// =Overridables
protected:
	virtual void  OnHelpFinder();
	virtual void  WinHelp(DWORD dwData, UINT nCmd);

// =Helpers
private:
	void          ShowHelp(DWORD dwData, UINT nCmd);
//<<< SJ 04.09.2008 help functionality added

}; // SODmCApp

extern SODmCApp* getApp(void);

#ifdef SODMC_DA

//-----------------------------------------------------------------------------
// SODmCCreatorDA                                                             |
//-----------------------------------------------------------------------------

class SODmCCreatorDA : public SODaCCreator
{
public:
	virtual SODaCServer* createServer(IN BYTE protocolType, IN SODaCEntry* parent);
	virtual SODaCGroup* createGroup(IN BYTE protocolType, IN SODaCServer* parent);
	virtual SODaCItem* createItem(IN SODaCGroup* parent);
}; // SODmCCreatorDA

#endif

#ifdef SODMC_AE

//-----------------------------------------------------------------------------
// SODmCCreatorAE                                                             |
//-----------------------------------------------------------------------------

class SODmCCreatorAE : public SOAeCCreator
{
public:
	virtual SOAeCServer* createServer(IN BYTE protocolType, IN SOAeCEntry* parent);
	virtual SOAeCSubscription* createSubscription(IN BYTE protocolType, IN SOAeCServer* parent);
	virtual SOAeCEvent* createEvent(IN SOAeCSubscription* subs, IN ONEVENTSTRUCT* pData);
}; // SODmCCreatorAE

#endif

#ifdef SODMC_DCOM

//-----------------------------------------------------------------------------
// SODmCServerBrowser                                                         |
//-----------------------------------------------------------------------------

class SODmCServerBrowser : public SOCltServerBrowser
{
protected:
	virtual void onError(IN HRESULT res, IN DWORD errorcode);
}; // SODmCServerBrowser

#endif // SODMC_DCOM



//-----------------------------------------------------------------------------
// Global functions                                                           |
//-----------------------------------------------------------------------------

// tree and list handling
HTREEITEM SODmCTreeInsert(IN CTreeCtrl* tree, IN HTREEITEM hParent, IN LPCTSTR pszText,
						  IN BOOL cChildren, IN void* lParam, IN int iImage, IN int iSelectedImage, IN HTREEITEM hInsertAfter);
int SODmCListInsert(IN CListCtrl* list, IN LPCTSTR pszText, IN void* lParam, IN int iImage);
int SODmCListPush(IN CListCtrl* list, IN LPCTSTR pszText, IN void* lParam, IN int iImage);

// conversion routines
void LCID2string(IN LCID lcid, OUT CString& string);
LCID string2LCID(IN CString string);
VARTYPE string2vartype(IN CString datatype);
WORD string2accessRights(IN CString rights);

// time conversion
#define FILETIME2STRING(a, b) { SOCmnDateTime dt(b); if (getApp()->getOptions()->timeOnly) dt.toString(*a); else *a = dt.getXMLDateTime(); }
#define DATETIME2STRING(a, dt) { if (getApp()->getOptions()->timeOnly) dt.toString(a); else a = dt.getXMLDateTime(); }

LPCTSTR variant2stringHex(IN SOCmnString* buffer, IN LPCVARIANT var, OPTIONAL IN LCID localeID = LOCALE_SYSTEM_DEFAULT, IN BOOL readFormat = TRUE);
LPCTSTR variantWithType2stringHex(IN SOCmnString* buffer, IN LPCVARIANT var, OPTIONAL IN LCID localeID = LOCALE_SYSTEM_DEFAULT);
LPCTSTR VARIANT2STRING(IN SOCmnString* buffer, IN LPCVARIANT var, OPTIONAL IN BOOL readFormat = TRUE);

BOOL convertWriteStringToVariant(SOCltElement* obj, IN LPCTSTR valueString, IN VARTYPE datatype, IN DWORD startIdx, OUT SOCmnVariant& value);

// windows controls version
#define COMCTL32_VERSION_IE4    MAKELONG(71, 4)
DWORD getComCtlVersion(void);

#endif

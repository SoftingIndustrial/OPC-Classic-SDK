//-----------------------------------------------------------------------------
#ifndef _SOCLTBROWSE_H_
#define _SOCLTBROWSE_H_

#include "SOCltServer.h"

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SOCltBrowseObject                                                          |
//-----------------------------------------------------------------------------

class SOCLT_EXPORT SOCltBrowseObject :
	public SOCmnElement
{
public:
	SOCltBrowseObject(IN DWORD objType);
	virtual ~SOCltBrowseObject();

	// Returns the children list
	virtual BOOL expand(IN DWORD type, OUT SOCmnList<SOCltBrowseObject> &objList) = 0;
	virtual SOCmnString getDisplayName(void);       // Returns string to be shown in the tree
	virtual SOCmnString getPath(void);                  // Returns the fully qualified object's name
	virtual BOOL isExpandable(void);

	virtual SOCltBrowseObject* getBrowser(void) = 0;

	virtual SOCmnStringList* getPathList(void)
	{
		return NULL;
	}
	virtual void setItemID(IN LPCTSTR /* itemID */, IN LPCTSTR /* parentItemID */) { }
	virtual SOCmnString getItemID(IN OPTIONAL BOOL retrive = TRUE);
	virtual SOCmnString getItemPath(void)
	{
		return _T("");
	}
};

#pragma pack(pop)

#endif // _SOCLTBROWSE_H_

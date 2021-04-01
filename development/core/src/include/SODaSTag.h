#ifndef _SODASTAG_H_
#define _SODASTAG_H_

#include <opcda.h>
#include "SOCmnElement.h"
#include "SODaSItemTag.h"
#include "SODaSProperty.h"

#pragma pack(push, 4)



//-----------------------------------------------------------------------------
// SODaSTag                                                                   |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSTag :
	public SODaSItemTag,
	virtual public SOCmnElementNameString
{
public:
	SODaSTag();

	virtual BOOL isUniqueName(void) const;
	virtual BOOL trustUniqueChilds(void);

	// watch overload
	virtual BOOL setParentBranch(IN SOCmnElement* parent);

	// namespace elements lists
	inline SOCmnList<SODaSProperty> *getPropertyList(void)
	{
		return (SOCmnList<SODaSProperty> *)getAdditionalList();
	}

	// I/O mode
	virtual BYTE getDeviceIOMode(IN SODaSRequest* req);

	// special release method for dynamic created tags
	virtual LONG release(void);

#ifdef SOFEATURE_WATCH
	// objects watch data
	virtual void getObjWatchData(IN SOCmnStringEx& szXML,
								 IN BOOL withAttributes, IN LPTSTR objNodeName);
#endif

protected:
	virtual ~SODaSTag();

	virtual SOCmnObject* getObjectCmnElementIName(void);
}; // SODaSTag

#define GenericTag SODaSTagDefault




//-----------------------------------------------------------------------------
// SODaSTagDefault                                                            |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SODaSTagDefault :
	public SODaSTag,
	virtual public SOCmnElementListAdditional,
	virtual public SODaSItemTagConnectQueueStore
{
public:
	SODaSTagDefault(void);
	SODaSTagDefault(IN LPCTSTR tagName);

protected:
	virtual SOCmnObject* getObjectCmnElementIList(void);
}; // SODaSTagDefault

#pragma pack(pop)
#endif

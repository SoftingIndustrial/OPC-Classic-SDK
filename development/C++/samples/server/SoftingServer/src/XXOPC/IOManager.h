#ifndef _IOMANAGER_H_
#define _IOMANAGER_H_

#include "IOManager.h"
#include "MyTransaction.h"

using namespace SoftingOPCToolboxServer;

class IOManager
{
public:
	IOManager(void) {}
	virtual ~IOManager(void) {}

	// handle direct IO requests
	virtual long handleReadRequests(MyTransaction& aTransaction);
	virtual long handleWriteRequests(MyTransaction& aTransaction);

	// handle cyclic IO
	virtual void cyclicStateChanged(std::vector<DaAddressSpaceElement*>& anElementList);
};  //  end class MyTransaction

#endif  //  _MYTRANSACTION_H_

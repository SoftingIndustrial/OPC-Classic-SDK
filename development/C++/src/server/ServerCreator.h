#ifndef _SERVERCREATOR_H_
#define _SERVERCREATOR_H_

#include "../Enums.h"
#include "ServerEnums.h"

#include <vector>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


namespace SoftingOPCToolboxServer
{

class DaAddressSpaceRoot;
class DaAddressSpaceElement;
class DaRequest;
class DaTransaction;
class DaSession;

class AeAddressSpaceRoot;
class AeAddressSpaceElement;
class WebTemplate;
class AeAttribute;

class Application;

class TBC_EXPORT Creator
{

public:

	Creator() {}
	virtual ~Creator() {}

	virtual DaAddressSpaceElement* createDaAddressSpaceElement();
	virtual AeAddressSpaceElement* createAeAddressSpaceElement();

	// Creates a singleton DaAddressSpaceRoot
	virtual DaAddressSpaceRoot* createDaAddressSpaceRoot(void);

	// Creates a new instance of DaAddressSpaceElement
	virtual DaAddressSpaceElement* createInternalDaAddressSpaceElement(
		tstring& anItemId,
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle);

	// Creates a new instance of DaAddressSpaceElement (extended version)
	virtual DaAddressSpaceElement* createInternalDaAddressSpaceElementEx(
		tstring& anItemId,
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle,
		EnumAddressSpaceElementData elementDataFilter,
		EnumAccessRights accessRights,
		EnumIoMode ioMode,
		BOOL hasChildren,
		BOOL isBrowsable,
		VARENUM dataType);

	// Creates a new instance of "Request"
	virtual DaRequest* createRequest(
		EnumTransactionType aTransactionType,
		unsigned long aSessionHandle,
		DaAddressSpaceElement* anElement,
		int aPropertyId,
		unsigned long aRequestHandle);

	// Creates a new instance of "Transaction"
	virtual DaTransaction* createTransaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionHandle);

	// Creates a new instance of "Session"
	// NOTE: please be note thet the session can be also an AE Session.
	//  The name was not changed due to backwords compatibility issues
	virtual DaSession* createSession(
		EnumSessionType aType,
		unsigned long aHandle);

	// Creates a singleton AeAddressSpaceRoot
	virtual AeAddressSpaceRoot* createAeAddressSpaceRoot(void);

	// Creates a new instance of AddressSpaceElement
	virtual AeAddressSpaceElement* createInternalAeAddressSpaceElement(
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle);

	virtual WebTemplate* createWebTemplate(void);

};  //  end  class Creator

}   //  end namespace

#pragma pack(pop)
#endif  //  end _CREATOR_H_

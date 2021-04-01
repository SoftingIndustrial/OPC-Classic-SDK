#ifndef _OPCSERVER_H_
#define _OPCSERVER_H_

#include "ServerApplication.h"

#include "MyDaAddressSpaceElement.h"
#include "MyTransaction.h"
#include "MyDaAddressSpaceRoot.h"

#include "MyCreator.h"

extern const TCHAR g_itemFullPath[256];
extern const TCHAR g_itemNameFormat[16];
extern const unsigned int g_itemsTotalCount;
extern const unsigned int g_qualities[17];
extern const TCHAR g_itemName[256];
extern TCHAR g_namespaceDelimiter[2];

using namespace SoftingOPCToolboxServer;

class OpcServer
{
public:

	OpcServer(void);
	virtual ~OpcServer(void);

	long initialize(void);
	long start(void);
	long stop(void);
	long ready(void);
	long terminate(void);

	void fireDataChanges(void);
	void emptyValues(void);

	long prepare(IN MyCreator* pCreator);
	long processCommandLine(IN tstring commandLine);

	long buildAddressSpace(void);
	void rebuildRandomNamespace();
	void printChildren(AddressSpaceElement* element, unsigned char bLevel);
	void showObjectTree(void);
	void updateAddressSpaceElements();
	void OpcServer::freeElement(MyDaAddressSpaceElement* element);


	void setServiceName(tstring serviceName);

	void trace(
		EnumTraceLevel aLevel,
		EnumTraceGroup aMask,
		const TCHAR* anObjectID,
		const TCHAR* aMessage,
		...);

private:
	ShutdownRequestHandler m_ShutdownRequest;
	DaAddressSpaceElement *m_objectBasedParentElement;
	DaAddressSpaceElement *m_stringBasedParentElement;

};  //  end class OpcServer

//	Public OpcServer's handlers
OpcServer* getOpcServer(void);
void createOpcServer(void);
void destroyOpcServer(void);

#endif //   _OPCSERVER_H_

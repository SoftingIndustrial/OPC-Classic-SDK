#ifndef _OPCSERVER_H_
#define _OPCSERVER_H_

#include "ServerApplication.h"

#include "MyDaAddressSpaceElement.h"
#include "MyRequest.h"
#include "MyTransaction.h"
#include "MyDaAddressSpaceRoot.h"

#include "MyCreator.h"

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

	long prepare(IN MyCreator* pCreator);
	long processCommandLine(IN tstring commandLine);

	long buildAddressSpace(void);

	void changeSimulationValues(void);

	long processValueChangeNode(MyDaAddressSpaceElement* nodeToChange);

	long processValueChangeLeaf(MyDaAddressSpaceElement* leafToChange);

	void setServiceName(tstring serviceName);

	void trace(
		EnumTraceLevel aLevel,
		EnumTraceGroup aMask,
		const TCHAR* anObjectID,
		const TCHAR* aMessage,
		...);

	long initializeMaps(void);

private:
	long parseConfigFile(tstring nsConfigFileName);
	long addAddressSpaceElement(void);

	//member to store the current line during parsing
	tstring m_currentLine[10];
	// Map to associate data types
	std::map<tstring, VARENUM> m_mapDataType;
	// Map to associate access level
	std::map<tstring, EnumAccessRights> m_mapAccessLevel;
	//Map to associate parentsId with address space elements
	std::map<tstring, MyDaAddressSpaceElement*> m_mapParent;

	tstring m_configFile;
	MyCreator* m_creator;
	DaAddressSpaceRoot* m_daRoot;
	ShutdownRequestHandler m_ShutdownRequest;
	MyDaAddressSpaceElement* m_pDaElement;
};  // end class OpcServer

// Public OpcServer's handlers
OpcServer* getOpcServer(void);
void createOpcServer(void);
void destroyOpcServer(void);

#endif // _OPCSERVER_H_


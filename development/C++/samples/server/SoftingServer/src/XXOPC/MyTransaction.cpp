#include "stdafx.h"
#include "MyTransaction.h"
#include "OpcServer.h"
#include "IOManager.h"

using namespace SoftingOPCToolboxServer;

long MyTransaction::handleReadRequests(void)
{
	return getOpcServer()->getIOManager()->handleReadRequests(*this);
}   //  end handleReadRequests

long MyTransaction::handleWriteRequests(void)
{
	return getOpcServer()->getIOManager()->handleWriteRequests(*this);
}   //  end handleWriteRequests


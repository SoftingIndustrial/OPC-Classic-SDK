#ifndef _MYDASESSION_H_
#define _MYDASESSION_H_

#include "ClientServerStatus.h"
#include "resource.h"

#ifdef TBC_OS_WINDOWS
#include "Da\ClientDaSession.h"
#endif

#ifdef TBC_OS_LINUX
#include "ClientDaSession.h"
#endif

using namespace SoftingOPCToolboxClient;

class CDABrowseDlg;

class MyDaSession :
	public SoftingOPCToolboxClient::DaSession
{
public:

	MyDaSession(const tstring& url, CDABrowseDlg* dialog);
	bool isConnected() const;
	void handleStateChangeCompleted(EnumObjectState state);

private:
	bool m_connected;
	CDABrowseDlg* m_dialog;

};

#endif // _MYDASESSION_H_
#include "stdafx.h"
#include "MyDaSession.h"
#include "DABrowseDlg.h"

MyDaSession::MyDaSession(const tstring& url, CDABrowseDlg* dialog): DaSession(url)
{
	m_connected = false;
	m_dialog = dialog;
}

bool MyDaSession::isConnected() const
{
	return m_connected;
}

void MyDaSession::handleStateChangeCompleted(EnumObjectState state)
{
	if (state == EnumObjectState_CONNECTED)
	{
		m_connected = true;
		if ((m_dialog != NULL) && (m_dialog->m_ready))
		{
			m_dialog->m_label.SetWindowText(_T("Connected"));
		}
	}
	else
	{
		m_connected = false;
		if ((m_dialog != NULL) && (m_dialog->m_ready))
		{
			m_dialog->m_label.SetWindowText(_T("Disconnected"));
		}
	}
}


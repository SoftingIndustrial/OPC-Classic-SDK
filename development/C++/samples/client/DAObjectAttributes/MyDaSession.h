#ifndef _MYDASESSION_H_
#define _MYDASESSION_H_

#ifdef TBC_OS_WINDOWS
#include "Da\ClientDaSession.h"
#endif

#ifdef TBC_OS_LINUX
#include "ClientDaSession.h"
#endif

class MyDaSession :
	public SoftingOPCToolboxClient::DaSession
{
public:

	MyDaSession(const tstring& url) :
		SoftingOPCToolboxClient::DaSession(url) {}

	virtual ~MyDaSession()
	{} // end dtor

	unsigned char handleShutdownRequest(const tstring& reason)
	{
		// Log the result
		// reconnect automatically
		return TRUE;
	} // end handleShutdownRequest

	void print()
	{
		_tprintf(_T("\n%s\n"), _T("Session"));
		_tprintf(_T("  URL: %s\n"), getUrl().c_str());
		_tprintf(_T("  Client Name: %s\n"), getClientName().c_str());
		_tprintf(_T("  LCID: %s\n"), getLocaleId().c_str());
		tstring supportedSpec = GetOPCSpecificationString(getSupportedOpcSpecification());
		tstring forcedSpec = GetOPCSpecificationString(getForcedOpcSpecification());
		_tprintf(_T("  Supported OPC Specification: %s\n"), supportedSpec.c_str());
		_tprintf(_T("  Forced OPC Specification: %s\n"), forcedSpec.c_str());
	}// end print

	tstring GetOPCSpecificationString(EnumOPCSpecification specification)
	{
		tstring spec;

		switch (specification)
		{
		case EnumOPCSpecification_DEFAULT:
			spec = _T("default");
			break;

		case EnumOPCSpecification_DA10:
			spec = _T("DA1");
			break;

		case EnumOPCSpecification_DA20:
			spec = _T("DA2");
			break;

		case EnumOPCSpecification_DA30 :
			spec = _T("DA3");
			break;

		case EnumOPCSpecification_XMLDA10:
			spec = _T("XML-DA");
			break;

		case EnumOPCSpecification_AE10:
			//nothing to do
			break;
		} //end switch

		return spec;
	}

}; // end class MyDaSession

#endif

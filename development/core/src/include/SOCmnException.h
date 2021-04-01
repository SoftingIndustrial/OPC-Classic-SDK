#ifndef _SOCMNEXCEPTION_H_
#define _SOCMNEXCEPTION_H_

#pragma pack(push, 4)


#ifndef SOOS_WINDOWS_CE
#define SOCMN_TRY       try
#define SOCMN_CATCH(e)  catch(e)
#define SOCMN_THROW(e)  throw e;
#else
#define SOCMN_TRY
#define SOCMN_CATCH(e)  if (FALSE)
#define SOCMN_THROW(e)  {;}
#endif

#define SOCMNEXCEPTION_ID_OUTOFMEMORY   0x00000001
#define SOCMNEXCEPTION_ID_SERIALIZATION 0x00000002
#define SOCMNEXCEPTION_DESCRIPTION_OUTOFMEMORY      _T("Out of memory")
#define SOCMNEXCEPTION_DESCRIPTION_SERIALIZATION    _T("Serialization error")

//-----------------------------------------------------------------------------
// SOCmnException                                                             |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnException
{
public:
	SOCmnException();
	SOCmnException(IN DWORD id, IN LPCTSTR description);
	~SOCmnException();

	DWORD m_id;                 // exception id
	SOCmnString m_description;  // exception description
}; // SOCmnException

#define SOCMN_THROW_EXCEPTION(id, desc) SOCMN_THROW(SOCmnException(id, desc))
#define SOCMN_THROW_EXCEPTION_OUTOPFMEMORY() SOCMN_THROW_EXCEPTION(SOCMNEXCEPTION_ID_OUTOFMEMORY, SOCMNEXCEPTION_DESCRIPTION_OUTOFMEMORY)
#define SOCMN_THROW_EXCEPTION_SERIALIZATION() SOCMN_THROW_EXCEPTION(SOCMNEXCEPTION_ID_SERIALIZATION, SOCMNEXCEPTION_DESCRIPTION_SERIALIZATION)

#pragma pack(pop)
#endif

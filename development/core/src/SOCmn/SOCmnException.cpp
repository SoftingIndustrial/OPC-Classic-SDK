#include "stdafx.h"
#include <SOCmn.h>
#include <SOCmnException.h>


//-----------------------------------------------------------------------------
// SOCmnException                                                             |
//-----------------------------------------------------------------------------

SOCmnException::SOCmnException()
{
	m_id = 0;
}

SOCmnException::SOCmnException(IN DWORD id, IN LPCTSTR description)
{
	m_id = id;
	m_description = description;
}

SOCmnException::~SOCmnException()
{
}



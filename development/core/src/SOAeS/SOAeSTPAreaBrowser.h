#ifndef _SOAESTPAREABROWSER_H_
#define _SOAESTPAREABROWSER_H_

#ifdef SOFEATURE_TUNNEL


//-----------------------------------------------------------------------------
// SOAeSComAreaBrowser                                                        |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SOAeSTPAreaBrowser : public SOCmnTPReceiver
{
public:
	SOAeSTPAreaBrowser(void);
	virtual ~SOAeSTPAreaBrowser(void);
	virtual LONG handleRequest(IN SOCmnTPCall* pCall, WORD interfaceId, BYTE functionId);

	void changeBrowsePosition(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
	void browseOPCAreas(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
	void getQualifiedAreaName(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
	void getQualifiedSourceName(IN SOAeSServer* pServer, IN SOCmnTPCall* pCall);
};  //  enc class SOAeSTPAreaBrowser

#endif // SOFEATURE_DCOM

#endif

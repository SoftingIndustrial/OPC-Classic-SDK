#ifndef _SOCMNXML_H_
#define _SOCMNXML_H_

#pragma pack(push, 4)


#define BOOL2XMLSTR(a) ((a) ? _T("true") : _T("false"))

inline BOOL xmlStr2Bool(LPCTSTR a)
{
	if (_tcsicmp(a, _T("true")) == 0)
	{
		return TRUE;
	}

	if (_tcsicmp(a, _T("1")) == 0)
	{
		return TRUE;
	}

	return FALSE;
}
#define XMLSTR2BOOL(a) (xmlStr2Bool(a))

#ifdef SOFEATURE_CMN_XML

extern const TCHAR g_szArrayOfByte[];
extern const TCHAR g_szArrayOfShort[];
extern const TCHAR g_szArrayOfUnsignedShort[];
extern const TCHAR g_szArrayOfInt[];
extern const TCHAR g_szArrayOfUnsignedInt[];
extern const TCHAR g_szArrayOfLong[];
extern const TCHAR g_szArrayOfUnsignedLong[];
extern const TCHAR g_szArrayOfFloat[];
extern const TCHAR g_szArrayOfDecimal[];
extern const TCHAR g_szArrayOfDouble[];
extern const TCHAR g_szArrayOfBoolean[];
extern const TCHAR g_szArrayOfString[];
extern const TCHAR g_szArrayOfDateTime[];
extern const TCHAR g_szArrayOfAnyType[];

// eXpat defines
#ifndef SOFLAG_USE_EXPAT_SHARED_LIB
#define XML_STATIC
#else
#ifdef SOOS_WINDOWS
#pragma comment(lib, "libexpat.lib")
#endif
#endif
#include "expat.h"

#define SCMNXML_HANDLER_ELEMENTDECL           0x00000001
#define SCMNXML_HANDLER_ATTLISTDECL           0x00000002
#define SCMNXML_HANDLER_XMLDECL               0x00000004
#define SCMNXML_HANDLER_PROCESSINGINSTRUCTION 0x00000008
#define SCMNXML_HANDLER_ELEMENT               0x00000030
#define SCMNXML_HANDLER_STARTELEMENT          0x00000010
#define SCMNXML_HANDLER_ENDELEMENT            0x00000020
#define SCMNXML_HANDLER_CHARACTERDATA         0x00000040
#define SCMNXML_HANDLER_COMMENT               0x00000080
#define SCMNXML_HANDLER_CDATASECTION          0x00000300
#define SCMNXML_HANDLER_STARTCDATASECTION     0x00000100
#define SCMNXML_HANDLER_ENDCDATASECTION       0x00000200
#define SCMNXML_HANDLER_DEFAULT               0x00000400
#define SCMNXML_HANDLER_DEFAULTEXPAND         0x00000800
#define SCMNXML_HANDLER_DOCTYPEDECL           0x00003000
#define SCMNXML_HANDLER_STARTDOCTYPEDECL      0x00002000
#define SCMNXML_HANDLER_ENDDOCTYPEDECL        0x00001000
#define SCMNXML_HANDLER_UNPARSEDENTITYDECL    0x00004000
#define SCMNXML_HANDLER_NOTATIONDECL          0x00008000
#define SCMNXML_HANDLER_NAMESPACEDECL         0x00030000
#define SCMNXML_HANDLER_STARTNAMESPACEDECL    0x00010000
#define SCMNXML_HANDLER_ENDNAMESPACEDECL      0x00020000
#define SCMNXML_HANDLER_NOTSTANDALONE         0x00040000
#define SCMNXML_HANDLER_SKIPPEDENTITY         0x00080000
#define SCMNXML_HANDLER_SETENTITYDECL         0x00100000
#define SCMNXML_HANDLER_ALL                   0xFFFFFFFF

//-----------------------------------------------------------------------------
// SOCmnXML                                                                   |
//-----------------------------------------------------------------------------

class SOCMN_EXPORT SOCmnXML : public SOCmnObject
{
public:
	SOCmnXML();
	~SOCmnXML();

	virtual BOOL create(LPCSTR encod, CHAR namespaceSeparator);
	virtual void destroy(void);

	virtual BOOL parseBuffer(LONG len, BOOL isFinal = TRUE);
	virtual LPSTR getBuffer(LONG len);
	virtual BOOL parse(LPCSTR buffer, LONG len, BOOL isFinal = TRUE);

	virtual void setHandler(DWORD whichHandler);

	XML_Parser getParser(void)
	{
		return m_parser;
	}

	virtual void handleElementDecl(LPCSTR /* name */, XML_Content* /* model */) {}
	virtual void handleAttlistDecl(LPCSTR /*elname */, LPCSTR /* attname */, LPCSTR /* att_type */, LPCSTR /* dflt */, INT /* isRequired */) {}
	virtual void handleXmlDecl(LPCSTR /*version */, LPCSTR /* encoding */, INT /* standalone */) {}
	virtual void handleProcessingInstruction(LPCSTR /* target */, LPCSTR /* data */) {}
	virtual void handleStartElement(LPCSTR /* name */, LPCSTR* /* atts */) {}
	virtual void handleEndElement(LPCSTR /* name */) {}
	virtual void handleCharacterData(LPCSTR /* s */, INT /* len */) {}
	virtual void handleComment(LPCSTR /* data */) {}
	virtual void handleStartCdataSection(void) {}
	virtual void handleEndCdataSection(void) {}
	virtual void handleDefault(LPCSTR /* s */, INT /* len */) {}
	virtual void handleStartDoctypeDecl(LPCSTR /* doctypeName */, LPCSTR /* sysid */, LPCSTR /* pubid */, INT /*hasInternalSubset*/) {}
	virtual void handleEndDoctypeDecl(void) {}
	virtual void handleEntityDecl(LPCSTR /* entityName */, INT /* is_parameter_entity */, LPCSTR /* value */, INT /* value_length */, LPCSTR /* base */, LPCSTR /* systemId */, LPCSTR /* publicId */, LPCSTR /* notationName */) {}
	virtual void handleUnparsedEntityDecl(LPCSTR /* entityName */, LPCSTR /* base */, LPCSTR /* systemId */, LPCSTR /* publicId */, LPCSTR /* notationName */) {}
	virtual void handleNotationDecl(LPCSTR /* notationName */, LPCSTR /* base */, LPCSTR /* systemId */, LPCSTR /* publicId */) {}
	virtual void handleStartNamespaceDecl(LPCSTR /* prefix */, LPCSTR /* uri */) {}
	virtual void handleEndNamespaceDecl(LPCSTR /* prefix */) {}
	virtual INT handleNotStandalone(void)
	{
		return XML_STATUS_ERROR;
	}
	virtual void handleSkippedEntity(LPCSTR /* entityName */, INT /* isParameterEntity */) {}

	void defaultCurrent(void)
	{
		XML_DefaultCurrent(m_parser);
	}
	void setReturnNSTriplet(int do_nst)
	{
		XML_SetReturnNSTriplet(m_parser, do_nst);
	}
	BOOL setEncoding(LPCSTR encoding)
	{
		return (XML_SetEncoding(m_parser, encoding) == XML_STATUS_OK);
	}
	BOOL setBase(LPCSTR base)
	{
		return (XML_SetBase(m_parser, base) == XML_STATUS_OK);
	}
	LPCSTR getBase(void)
	{
		return XML_GetBase(m_parser);
	}
	INT getSpecifiedAttributeCount(void)
	{
		return XML_GetSpecifiedAttributeCount(m_parser);
	}
	INT getIdAttributeIndex(void)
	{
		return XML_GetIdAttributeIndex(m_parser);
	}
	enum XML_Error getErrorCode(void)
	{
		return XML_GetErrorCode(m_parser);
	}
	INT getCurrentLineNumber(void)
	{
		return XML_GetCurrentLineNumber(m_parser);
	}
	INT getCurrentColumnNumber(void)
	{
		return XML_GetCurrentColumnNumber(m_parser);
	}
	LONG getCurrentByteIndex(void)
	{
		return XML_GetCurrentByteIndex(m_parser);
	}
	INT getCurrentByteCount(void)
	{
		return XML_GetCurrentByteCount(m_parser);
	}
	LPCSTR getInputContext(INT* offset, INT* size)
	{
		return XML_GetInputContext(m_parser, offset, size);
	}
	void freeContentModel(XML_Content* model)
	{
		XML_FreeContentModel(m_parser, model);
	}
	void memFree(void* ptr)
	{
		XML_MemFree(m_parser, ptr);
	}
	LPCSTR getErrorString(enum XML_Error code)
	{
		return XML_ErrorString(code);
	}

protected:
	XML_Parser m_parser;

	static void __cdecl elementDeclH(void* userData, LPCSTR name, XML_Content* model)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleElementDecl(name, model);
	}
	static void __cdecl attlistDeclH(void* userData, LPCSTR elname, LPCSTR attname, LPCSTR att_type, LPCSTR dflt, INT isrequired)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleAttlistDecl(elname, attname, att_type, dflt, isrequired);
	}
	static void __cdecl xmlDeclH(void* userData, LPCSTR version, LPCSTR encoding, INT standalone)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleXmlDecl(version, encoding, standalone);
	}
	static void __cdecl processingInstructionH(void* userData, LPCSTR target, LPCSTR data)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleProcessingInstruction(target, data);
	}
	static void __cdecl startElementH(void* userData, LPCSTR name, LPCSTR* atts)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleStartElement(name, atts);
	}
	static void __cdecl endElementH(void* userData, LPCSTR name)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleEndElement(name);
	}
	static void __cdecl characterDataH(void* userData, LPCSTR s, INT len)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleCharacterData(s, len);
	}
	static void __cdecl commentH(void* userData, LPCSTR data)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleComment(data);
	}
	static void __cdecl startCdataSectionH(void* userData)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleStartCdataSection();
	}
	static void __cdecl endCdataSectionH(void* userData)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleEndCdataSection();
	}
	static void __cdecl defaultH(void* userData, LPCSTR s, INT len)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleDefault(s, len);
	}
	static void __cdecl startDoctypeDeclH(void* userData, LPCSTR doctypeName, LPCSTR sysid, LPCSTR pubid, INT has_internal_subset)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleStartDoctypeDecl(doctypeName, sysid, pubid, has_internal_subset);
	}
	static void __cdecl endDoctypeDeclH(void* userData)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleEndDoctypeDecl();
	}
	static void __cdecl entityDeclH(void* userData, LPCSTR entityName, INT is_parameter_entity, LPCSTR value, INT value_length, LPCSTR base, LPCSTR systemId, LPCSTR publicId, LPCSTR notationName)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleEntityDecl(entityName, is_parameter_entity, value, value_length, base, systemId, publicId, notationName);
	}
	static void __cdecl unparsedEntityDeclH(void* userData, LPCSTR entityName, LPCSTR base, LPCSTR systemId, LPCSTR publicId, LPCSTR notationName)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleUnparsedEntityDecl(entityName, base, systemId, publicId, notationName);
	}
	static void __cdecl notationDeclH(void* userData, LPCSTR notationName, LPCSTR base, LPCSTR systemId, LPCSTR publicId)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleNotationDecl(notationName, base, systemId, publicId);
	}
	static void __cdecl startNamespaceDeclH(void* userData, LPCSTR prefix, LPCSTR uri)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleStartNamespaceDecl(prefix, uri);
	}
	static void __cdecl endNamespaceDeclH(void* userData, LPCSTR prefix)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleEndNamespaceDecl(prefix);
	}
	static INT __cdecl notStandaloneH(void* userData)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		return pT->handleNotStandalone();
	}
	static void __cdecl skippedEntityH(void* userData, LPCSTR entityName, INT is_parameter_entity)
	{
		SOCmnXML* pT = ((SOCmnXML*)userData);
		pT->handleSkippedEntity(entityName, is_parameter_entity);
	}
}; // SOCmnXML



class SOCMN_EXPORT SOCmnXmlValue : public SOCmnData
{
public:
	SOCmnXmlValue(void);
	virtual ~SOCmnXmlValue(void);

#ifdef SOFEATURE_CMN_VARIANT
	VARTYPE XmlArrayElementType2Vartype();

	// static XML helpers
	static DWORD decodeBase64String2Variant(SOCmnString& szBase64, SOCmnVariant& ui1Array);
	static DWORD decodeBase64String2String(SOCmnString& szBase64, SOCmnString& string);
	static SOCmnString encodeBytes2base64(SOCmnVariant& ui1Array);
#endif


	BYTE m_simpleType;
	SOCmnString m_datatype;
	SOCmnString m_value;
	SOCmnList<SOCmnXmlValue> m_array;
};

#define SOCMNXMLFILE_STATE_INIT   0xFE
#define SOCMNXMLFILE_STATE_ERROR  0xFF

class SOCMN_EXPORT SOCmnXmlFile : public SOCmnXML
{
public:
	SOCmnXmlFile(LPCTSTR fileName);

	virtual void handleStartElement(LPCSTR name, LPCSTR* atts);
	virtual void handleEndElement(LPCSTR name);
	virtual void handleStartNamespaceDecl(LPCSTR prefix, LPCSTR uri);

	BOOL readContents(void);
	void freeContents(void);

	virtual BOOL parse(void);

	LPCTSTR expandNamespace(IN OUT SOCmnString& szXML);
	BOOL includesNamespacePrefix(IN SOCmnString& szXML)
	{
		return (-1 != szXML.find(_T(':')));
	}
	BOOL includesNamespace(IN SOCmnString& szXML)
	{
		return (-1 != szXML.find(_T('|')));
	}
	static BOOL removeNamespace(OUT IN SOCmnString& szXML, IN LPCTSTR nspace);
	static void removeNamespace(OUT IN SOCmnString& szXML);

	BYTE* getContents(void)
	{
		return m_pData;
	}
	DWORD getContentsLength(void)
	{
		return m_lenData;
	}

protected:
	SOCmnStringList m_elementStack;
	SOCmnStringList m_namespaceList;
	SOCmnString m_defNamespace;
	SOCmnString m_fileName;
	DWORD m_lenData;
	BYTE* m_pData;

	virtual ~SOCmnXmlFile(void);
}; // SOCmnXmlFile

#endif // SOFEATURE_CMN_XML

inline WORD SwapWord(const WORD wData)
{
	return MAKEWORD(HIBYTE(wData), LOBYTE(wData));
}

inline DWORD SwapDWord(const DWORD dwData)
{
	return MAKELONG(SwapWord(HIWORD(dwData)), SwapWord(LOWORD(dwData)));
}

#ifdef SOFEATURE_XML_METHOD

//-----------------------------------------------------------------------------
// SOCmnXmlMethod                                                             |
//-----------------------------------------------------------------------------

#define SOCMNXMLMETHOD_NO 0x00

#define SOCMNXMLMETHOD_STATE_INIT          0x00
#define SOCMNXMLMETHOD_STATE_END           0x01
#define SOCMNXMLMETHOD_STATE_ERROR         0x02
#define SOCMNXMLMETHOD_STATE_SOAP_ENVELOPE 0x04
#define SOCMNXMLMETHOD_STATE_SOAP_BODY     0x05

class SOCMN_EXPORT SOCmnXmlMethod : public SOCmnXML
{
public:
	SOCmnXmlMethod(void);

	void setExpectedMethod(BYTE mthState, LPCTSTR mthSZ)
	{
		m_expMethodState = mthState;
		m_expMethodString = mthSZ;
	}
	LPCTSTR getExpectedMethodString(void)
	{
		return m_expMethodString;
	}
	BYTE getExpectedMethodState(void)
	{
		return m_expMethodState;
	}

	BYTE getMethod(void)
	{
		return m_xmlMethod;
	}
	void setMethod(BYTE xM)
	{
		m_xmlMethod = xM;
	}

	virtual void handleStartElement(LPCSTR name, LPCSTR* atts);
	virtual void handleEndElement(LPCSTR name);
	virtual void handleStartNamespaceDecl(LPCSTR prefix, LPCSTR uri);

	LPCTSTR expandNamespace(IN OUT SOCmnString& szXML);
	BOOL includesNamespacePrefix(IN SOCmnString& szXML)
	{
		return (-1 != szXML.find(_T(':')));
	}
	BOOL includesNamespace(IN SOCmnString& szXML)
	{
		return (-1 != szXML.find(_T('|')));
	}
	BOOL removeNamespace(OUT IN SOCmnString& szXML, IN LPCTSTR nspace);

	virtual BOOL parse(LPCSTR buffer, LONG len, BOOL isFinal = TRUE)
	{
		return SOCmnXML::parse(buffer, len, isFinal);
	}
	BOOL parse(void)
	{
		return SOCmnXML::parse((CHAR*)m_pData, m_lenData);
	}

	SOCmnDateTime getRcvTime(void)
	{
		return m_rcvTime;
	}

	BYTE getFlags(void)
	{
		return m_flags;
	}

protected:
	BYTE m_xmlMethod;
	BYTE m_expMethodState;
	BYTE m_flags;
	SOCmnDateTime m_rcvTime;
	DWORD m_lenData;
	BYTE* m_pData;
	SOCmnString m_expMethodString;
	SOCmnStringList m_elementStack;
	SOCmnStringList m_namespaceList;
	SOCmnString m_defNamespace;

	~SOCmnXmlMethod(void);
}; // SOCmnXmlMethod

#endif // SOFEATURE_XML_METHOD

#pragma pack(pop)

#endif // _SOCMNXML_H_

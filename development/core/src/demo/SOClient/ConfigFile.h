#ifndef _CONFIGFILE_H_
#define _CONFIGFILE_H_

class SOCmnXmlTreeElement :
	public SOCmnElement,
	virtual public SOCmnElementNameString,
	virtual public SOCmnElementListBranch
{
public:
	SOCmnXmlTreeElement(LPCTSTR elementName, SOCmnXmlTreeElement* pParentElement);

	SOCmnList<SOCmnXmlTreeElement> *getXmlElements(void)
	{
		return (SOCmnList<SOCmnXmlTreeElement> *)getBranchList();
	}
	SOCmnStringList* getXmlAttributes(void)
	{
		return &m_xmlAttributes;
	}

	void addXmlAttributes(LPCSTR* atts);

	void storeToString(SOCmnStringEx& data);

protected:
	SOCmnStringList m_xmlAttributes;

	virtual ~SOCmnXmlTreeElement(void);

	virtual BOOL trustUniqueChilds(void)
	{
		return TRUE;
	}
};

class SOCmnXmlTree :
	public SOCmnXmlFile
{
public:
	SOCmnXmlTree(LPCTSTR fileName);

	SOCmnList<SOCmnXmlTreeElement> *getXmlElements(void)
	{
		return (SOCmnList<SOCmnXmlTreeElement> *)m_root->getBranchList();
	}
	SOCmnXmlTreeElement* getXmlRoot(void)
	{
		m_root.addRef();
		return m_root;
	}

	HRESULT store(LPCTSTR aFileName, DWORD anEstimatedSize);

protected:
	SOCmnPointer<SOCmnXmlTreeElement> m_root;
	SOCmnPointer<SOCmnXmlTreeElement> m_pCurXmlElement;

	virtual ~SOCmnXmlTree(void);

	virtual void handleStartElement(LPCSTR name, LPCSTR* atts);
	virtual void handleEndElement(LPCSTR name);
};

#endif //_CONFIGFILE_H_

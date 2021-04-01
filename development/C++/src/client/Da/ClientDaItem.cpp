#include "OSCompat.h"
#include "ClientDaItem.h"
#include "ClientDaSubscription.h"

#include "../../ValueQT.h"
#include "../ClientApplication.h"

#include "OTClient.h"
using namespace SoftingOPCToolboxClient;

DaItem::DaItem(
	tstring itemId,
	DaSubscription* parentSubscription)
{
	m_daSubscription = parentSubscription;
	m_valueQT        = NULL;
	long result = E_FAIL;
	result = parentSubscription->addDaItem(
				 itemId,
				 this);

	if (FAILED(result))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::Constructor"),
			_T(" Item could not be added to the subscription"));
	}   //  end if
} // end constructor

DaItem::~DaItem()
{
} // end destructor

DaSubscription* DaItem::getDaSubscription()
{
	return m_daSubscription;
} //end
void DaItem::setDaSubscription(DaSubscription* aDaSubscription)
{
	m_daSubscription = aDaSubscription;
} //end

ValueQT* DaItem::getValueQT()
{
	return m_valueQT;
} //end getValueQT

void DaItem::setValueQT(ValueQT* aValueQT)
{
	m_valueQT = aValueQT;
} //end setValueQT

const Variant& DaItem::getValue() const
{
	return m_valueQT->getData();
} //end getValue

tstring DaItem::getId()
{
	tstring id;
	OTCObjectAttributes objectAttributes;
	unsigned long whatAttributes[1];
	long results[1];
	unsigned long handles[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_ID;
		handles[0] = this->getHandle();
		long updateResult = OTCUpdateDAItemAttributes(
								m_daSubscription->getHandle(),
								1,
								handles,
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("DaItem::getId::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DAITEM_ID,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		id = objectAttributes.m_daItemID;
		OTFreeMemory(objectAttributes.m_daItemID);
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::getId::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return id;
} //end

void DaItem::setId(tstring id)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long handles[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
#ifdef TBC_OS_WINDOWS
	objectAttributes.m_daItemID = _tcsdup(id.c_str());
#endif
#ifdef TBC_OS_LINUX
	objectAttributes.m_daItemID = strdup(id.c_str());
#endif
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_DAITEM_ID,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::setId::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_ID;
		handles[0] = this->getHandle();
		long updateResult = OTCUpdateDAItemAttributes(
								m_daSubscription->getHandle(),
								1,
								handles,
								0,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("DaItem::setId::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	free(objectAttributes.m_daItemID);
} //end


tstring DaItem::getPath()
{
	tstring path;
	OTCObjectAttributes objectAttributes;
	unsigned long whatAttributes[1];
	long results[1];
	unsigned long handles[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_PATH;
		handles[0] = this->getHandle();
		long updateResult = OTCUpdateDAItemAttributes(
								m_daSubscription->getHandle(),
								1,
								handles,
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("DaItem::getpath::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DAITEM_PATH,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		path = objectAttributes.m_daItemPath;
		OTFreeMemory(objectAttributes.m_daItemPath);
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::getPath::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return path;
} //end

void DaItem::setPath(tstring path)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long handles[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
#ifdef TBC_OS_WINDOWS
	objectAttributes.m_daItemPath = _tcsdup(path.c_str());
#endif
#ifdef TBC_OS_LINUX
	objectAttributes.m_daItemPath = strdup(path.c_str());
#endif
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_DAITEM_PATH,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::setPath::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_PATH;
		handles[0] = this->getHandle();
		long updateResult = OTCUpdateDAItemAttributes(
								m_daSubscription->getHandle(),
								1,
								handles,
								0,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("DaItem::setPath::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	free(objectAttributes.m_daItemPath);
} //end


VARTYPE DaItem::getNativeDataType()
{
	VARTYPE nativeDataType = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long handles[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_NATIVE_DATATYPE;
		handles[0] = this->getHandle();
		long updateResult = OTCUpdateDAItemAttributes(
								m_daSubscription->getHandle(),
								1,
								handles,
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("DaItem::getNativeDataType::OTCUpdateDAItemAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DAITEM_NATIVE_DATATYPE,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		nativeDataType = objectAttributes.m_daItemNativeDatatype;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::getNativeDataType::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return nativeDataType;
} //end getNativeDataType



VARTYPE DaItem::getRequestedDataType()
{
	VARTYPE requestedDataType = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long handles[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE;
		handles[0] = this->getHandle();
		long updateResult = OTCUpdateDAItemAttributes(
								m_daSubscription->getHandle(),
								1,
								handles,
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("DaItem::getRequestedDataType::OTCUpdateDAItemAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DAITEM_REQUESTED_DATATYPE,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		requestedDataType = objectAttributes.m_daItemRequestedDatatype;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::getRequestedDataType::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return requestedDataType;
} //end getRequestedDataType

void DaItem::setRequestedDataType(VARTYPE requestedDataType)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long handles[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_daItemRequestedDatatype = requestedDataType;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_DAITEM_REQUESTED_DATATYPE,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::setRequestedDataType::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE;
		handles[0] = this->getHandle();
		long updateResult = OTCUpdateDAItemAttributes(
								m_daSubscription->getHandle(),
								1,
								handles,
								0,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("DaItem::setRequestedDataType::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setRequestedDataType

EnumAccessRights DaItem::getAccessRights()
{
	unsigned long accessRights = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long handles[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_ACCESSRIGHTS;
		handles[0] = this->getHandle();
		long updateResult = OTCUpdateDAItemAttributes(
								m_daSubscription->getHandle(),
								1,
								handles,
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("DaItem::getAccessRights::OTCUpdateDAItemAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DAITEM_ACCESSRIGHTS,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		accessRights = objectAttributes.m_daItemAccessRights;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::getAccessRights::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return (EnumAccessRights)accessRights;
} //end getAccessRights
float DaItem::getDeadband()
{
	float deadband = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long handles[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_DEADBAND;
		handles[0] = this->getHandle();
		long updateResult = OTCUpdateDAItemAttributes(
								m_daSubscription->getHandle(),
								1,
								handles,
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("DaItem::getDeadband::OTCUpdateDAItemAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DAITEM_DEADBAND,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		deadband = objectAttributes.m_daItemDeadband;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::getDeadband::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return deadband;
} //end getDeadband
void DaItem::setDeadband(float deadband)
{
	OTCExecutionOptions executionOptions;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long handles[1];
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	objectAttributes.m_daItemDeadband = deadband;
	long setAttrsResult = OTCSetAttributes(
							  getHandle(),
							  EnumObjectAttribute_DAITEM_DEADBAND,
							  &objectAttributes);

	if (FAILED(setAttrsResult))
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::setDeadband::OTCSetAttributes"),
			_T(" - code is %x"), setAttrsResult);
	} // end if

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_DEADBAND;
		handles[0] = this->getHandle();
		long updateResult = OTCUpdateDAItemAttributes(
								m_daSubscription->getHandle(),
								1,
								handles,
								0,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("DaItem::setDeadband::OTCUpdateAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if
} //end setDeadband
EnumEUType DaItem::getEUType()
{
	unsigned char EUType = 0;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long handles[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_EUTYPE;
		handles[0] = this->getHandle();
		long updateResult = OTCUpdateDAItemAttributes(
								m_daSubscription->getHandle(),
								1,
								handles,
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("DaItem::getEUType::OTCUpdateDAItemAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DAITEM_EUTYPE,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		EUType = objectAttributes.m_daItemEuType;
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::getEUType::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	return (EnumEUType)EUType;
} //end getEUType
Variant DaItem::getEUInfo()
{
	Variant EUInfo;
	OTCObjectAttributes objectAttributes;
	long results[1];
	unsigned long whatAttributes[1];
	unsigned long handles[1];
	OTCExecutionOptions executionOptions;
	executionOptions.m_executionContext = 0;
	executionOptions.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	memset(&objectAttributes, 0, sizeof(OTCObjectAttributes));
	objectAttributes.m_daItemEuInfo = (OTCVariant*)OTAllocateMemory(sizeof(OTCVariant));

	if (Application::Instance()->getUpdateAttributesBehavior() == EnumUpdateAttributesBehavior_IMPLICIT_UPDATE)
	{
		whatAttributes[0] = OTC_ATTRIBUTE_DAITEM_EUINFO;
		handles[0] = this->getHandle();
		long updateResult = OTCUpdateDAItemAttributes(
								m_daSubscription->getHandle(),
								1,
								handles,
								1,
								1,
								whatAttributes,
								results,
								&executionOptions);

		if (FAILED(updateResult))
		{
			Application::Instance()->trace(
				EnumTraceLevel_ERR,
				EnumTraceGroup_CLIENT,
				_T("DaItem::getEUInfo::OTCUpdateDAItemAttributes"),
				_T(" - code is %x"), updateResult);
		} // end if
	} // end if

	unsigned long getAttrResult = OTCGetAttributes(
									  getHandle(),
									  EnumObjectAttribute_DAITEM_EUINFO,
									  &objectAttributes);

	if (SUCCEEDED(getAttrResult))
	{
		EUInfo = Variant(objectAttributes.m_daItemEuInfo);
	} // end if
	else
	{
		Application::Instance()->trace(
			EnumTraceLevel_ERR,
			EnumTraceGroup_CLIENT,
			_T("DaItem::getEUInfo::OTCGetAttributes"),
			_T(" - code is %x"), getAttrResult);
	} //end else

	OTVariantClear(objectAttributes.m_daItemEuInfo);
	OTFreeMemory(objectAttributes.m_daItemEuInfo);
	return EUInfo;
} //end getEUInfo


long  DaItem::read(
	unsigned long maxAge,
	ValueQT& itemValue,
	long& result,
	ExecutionOptions* someExecutionOptions)
{
	if (m_daSubscription == NULL)
	{
		Application::Instance()->trace(
			EnumTraceLevel_INF,
			EnumTraceGroup_CLIENT,
			_T("DaItem::read"),
			_T("The item must belong to a subscription"));
		return EnumResultCode_E_INVALIDARG;
	}

	long readResult = E_FAIL;
	OTCExecutionOptions options;
	OTCValueData* pValues = new OTCValueData[1];
	long* pResults = new long[1];
	pResults[0] = E_FAIL;
	unsigned long* pItemHandles = new unsigned long[1];
	OTChar** pItemIds = new OTChar*[1];
	OTChar** pItemPaths = new OTChar*[1];

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	pValues[0].m_pValue = (OTVariant*)OTAllocateMemory(sizeof(OTVariant));
	OTVariantInit(pValues[0].m_pValue);
	pItemHandles[0] = this->getHandle();
#ifdef TBC_OS_WINDOWS
	pItemIds[0]     = _tcsdup(this->getId().c_str());
	pItemPaths[0]   = _tcsdup(this->getPath().c_str());
#endif
#ifdef TBC_OS_LINUX
	pItemIds[0]     = strdup(this->getId().c_str());
	pItemPaths[0]   = strdup(this->getPath().c_str());
#endif
	readResult = OTCRead(
					 m_daSubscription->getHandle(),
					 1,
					 pItemHandles,
					 pItemIds,
					 pItemPaths,
					 maxAge,
					 pValues,
					 pResults,
					 &options);

	if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
	{
		if (SUCCEEDED(readResult))
		{
			itemValue.setData(
				*(pValues[0].m_pValue),
				(EnumQuality)pValues[0].m_quality,
				DateTime(&pValues[0].m_timestamp));
			OTVariantClear(pValues[0].m_pValue);
		} // end if
	}//end if

	if (pResults)
	{
		result = pResults[0];
	} //end if

	OTFreeMemory(pValues[0].m_pValue);
	free(pItemIds[0]);
	free(pItemPaths[0]);
	delete[] pValues;
	delete[] pResults;
	delete[] pItemHandles;
	delete[] pItemIds;
	delete[] pItemPaths;
	return readResult;
} //end read

long  DaItem::write(
	ValueQT* itemValue,
	long& result,
	ExecutionOptions* someExecutionOptions)
{
	long writeResult = E_FAIL;
	OTCExecutionOptions options;
	OTCValueData* pValues = new OTCValueData[1];
	long* pResults = new long[1];
	unsigned long* pItemHandles = new unsigned long[1];
	OTChar** pItemIds = new OTChar*[1];
	OTChar** pItemPaths = new OTChar*[1];

	if (m_daSubscription == NULL)
	{
		Application::Instance()->trace(
			EnumTraceLevel_INF,
			EnumTraceGroup_CLIENT,
			_T("DaItem::write"),
			_T("The item must belong to a subscription"));
		return EnumResultCode_E_INVALIDARG;
	}

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType   = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

	pValues[0].m_pValue  = new OTCVariant();
	OTVariantInit(pValues[0].m_pValue);
	OTVariantCopy(pValues[0].m_pValue, (LPVARIANT)&((itemValue)->getData()));
	pValues[0].m_quality = (unsigned short)itemValue->getQuality();

#ifdef TBC_OS_WINDOWS
	pValues[0].m_timestamp.dwHighDateTime = itemValue->getTimeStamp().get().dwHighDateTime;
	pValues[0].m_timestamp.dwLowDateTime = itemValue->getTimeStamp().get().dwLowDateTime;
#endif
#ifdef TBC_OS_LINUX
	pValues[0].m_timestamp.tv_sec = 0;
	pValues[0].m_timestamp.tv_usec = 0;
#endif
	pItemHandles[0] = this->getHandle();
#ifdef TBC_OS_WINDOWS
	pItemIds[0]     = _tcsdup(this->getId().c_str());
	pItemPaths[0]   = _tcsdup(this->getPath().c_str());
#endif
#ifdef TBC_OS_LINUX
	pItemIds[0]     = strdup(this->getId().c_str());
	pItemPaths[0]   = strdup(this->getPath().c_str());
#endif
	writeResult = OTCWrite(
					  m_daSubscription->getHandle(),
					  1,
					  pItemHandles,
					  pItemIds,
					  pItemPaths,
					  pValues,
					  pResults,
					  &options);

	if (pResults)
	{
		result = pResults[0];
	} //end if

	OTVariantClear(pValues[0].m_pValue);
	delete(pValues[0].m_pValue);
	free(pItemPaths[0]);
	free(pItemIds[0]);
	delete[] pValues;
	delete[] pResults;
	delete[] pItemHandles;
	delete[] pItemIds;
	delete[] pItemPaths;
	return writeResult;
} //end write

void  DaItem::handleValueChanged(
	const ValueQT& aValueQT,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("DaItem::handleValueChanged"),
		_T(""));
} //end handleValueChanged

void  DaItem::onValueChange(
	ValueQT& aValueQT,
	long result)
{
	handleValueChanged(
		aValueQT,
		result);
} //end onValueChange

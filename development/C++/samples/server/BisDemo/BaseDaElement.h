#ifndef _BASEDAELEMENT_H_
#define _BASEDAELEMENT_H_

#include "Da/ServerDaAddressSpaceElement.h"
#include "Da/ServerDaProperty.h"

#include "BaseAeElement.h"

#include <vector>

using namespace SoftingOPCToolboxServer;



//-----------------------------------------------------------------------------
// BaseDaElement
//-----------------------------------------------------------------------------
class BaseDaElement : public DaAddressSpaceElement
{
public:
	BaseDaElement(
		tstring& anItemID,
		tstring& aName,
		unsigned int anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle) :
		DaAddressSpaceElement(anItemID, aName, anUserData, anObjectHandle, aParentHandle)
	{
	} // end ctr

	BaseDaElement(
		void) :
		DaAddressSpaceElement()
	{
	} // end ctr

	virtual ~BaseDaElement(
		void)
	{
	} // end dtor


	virtual void handleReadRequest(
		DaRequest* aRequest)
	{
		if (aRequest->getPropertyId() != 0)
		{
			DaProperty* daProperty = NULL;
			daProperty = (DaProperty*)m_properties[aRequest->getPropertyId()];

			if (daProperty != NULL)
			{
				daProperty->handleReadRequest(*aRequest);
			}
			else
			{
				aRequest->setResult(EnumResultCode_E_FAIL);
				aRequest->complete();
			} // end if .. else
		}
		else
		{
			handleItemReadRequest(aRequest);
		}   //  end if ... else
	}   //  end handleReadRequest

	virtual void handleWriteRequest(
		DaRequest* aRequest)
	{
		if (aRequest->getPropertyId() != 0)
		{
			DaProperty* daProperty = NULL;
			daProperty = (DaProperty*)m_properties[aRequest->getPropertyId()];

			if (daProperty != NULL)
			{
				daProperty->handleWriteRequest(*aRequest);
			}
			else
			{
				aRequest->setResult(EnumResultCode_E_FAIL);
				aRequest->complete();
			} // end if .. else
		}
		else
		{
			handleItemWriteRequest(aRequest);
		}   //  end if ... else
	} // end handleWriteRequest


	virtual long queryProperties(
		std::vector<DaProperty*>& aPropertyList)
	{
		if (m_properties.size() > 0)
		{
			std::map<unsigned long, DaProperty*>::iterator iter;

			for (iter = m_properties.begin(); iter != m_properties.end(); iter++)
			{
				aPropertyList.push_back(iter->second);
			} // end for
		} // end if

		return (long)EnumResultCode_S_OK;
	}   //  end queryProperties

	long addProperty(
		DaProperty* aProperty)
	{
		if (aProperty != NULL)
		{
			m_properties.insert(std::make_pair(aProperty->getId(), aProperty));
			return EnumResultCode_S_OK;
		} // end if

		return EnumResultCode_S_FALSE;
	} // end addProperty

	virtual void simulate(
		void)
	{
		if ((getHasChildren() == TRUE) == true)
		{
			std::vector<AddressSpaceElement*> children = getChildren();

			for (size_t index = 0; index < children.size(); ++index)
			{
				BaseDaElement* daElement = dynamic_cast<BaseDaElement*>(children.at(index));

				if (daElement != NULL)
				{
					daElement->simulate();
				}   //  end if
			}   //  end
		}   //  end if

		switchSimulation();
	}   //  end if

	void switchSimulation(
		void)
	{
		m_simulationOn = !m_simulationOn;
	} // end switchSimulation

	virtual unsigned int executeCommand(
		const tstring& anAddress,
		const tstring& aCommand,
		const tstring& aDescription)
	{
		return EnumResultCode_E_FAIL;
	} // end executeCommand


protected:
	virtual void handleItemReadRequest(
		DaRequest* aRequest)
	{
		ValueQT cacheValue;
		getCacheValue(cacheValue);
		aRequest->setValue(cacheValue);
		aRequest->setResult(EnumResultCode_S_OK);
		aRequest->complete();
	} // end handleItemReadRequest

	virtual void handleItemWriteRequest(
		DaRequest* aRequest)
	{
		aRequest->setResult(EnumResultCode_E_OPC_BADRIGHTS);
		aRequest->complete();
	}   //  end handleItemWriteRequest

protected:
	static bool m_simulationOn;

private:
	std::map<unsigned long, DaProperty*> m_properties;
}; // end BaseDaElement

bool BaseDaElement::m_simulationOn = true;

#endif

#ifndef _COMMANDELEMENT_H_
#define _COMMANDELEMENT_H_

#include "BaseDaElement.h"

#include "BisDaRoot.h"
#include "XmlDocument.h"



using namespace SoftingOPCToolboxServer;

static tstring COMMAND_NAME = _T("CMD");



//-----------------------------------------------------------------------------
// CommandElement
//-----------------------------------------------------------------------------
class CommandElement : public BaseDaElement
{
public:
	CommandElement(
		void)
	{
		setName(getCommandName());
		setHasChildren(false);
		setAccessRights(EnumAccessRights_READWRITEABLE);
		setIoMode(EnumIoMode_POLL);
		setDatatype(VT_BSTR);
	} // end ctr

	CommandElement(
		tstring& aName)
	{
		setName(aName);
		setHasChildren(false);
		setAccessRights(EnumAccessRights_READWRITEABLE);
		setIoMode(EnumIoMode_POLL);
		setDatatype(VT_BSTR);
	} // end ctr

	virtual ~CommandElement(
		void)
	{
	} // end dtor

protected:
	virtual void handleItemWriteRequest(
		DaRequest* aRequest)
	{
		try
		{
			//  start parsing the received command
			aRequest->setResult(EnumResultCode_E_FAIL);
			XmlDocument* doc = new XmlDocument();
			USES_CONVERSION;
			tstring xmlText = (LPCTSTR)(aRequest->getValue()->getData().bstrVal);
			doc->load(xmlText);
			tstring message = tstring(_T("Received text: ")) + xmlText;
			getApp()->trace(EnumTraceLevel_ERR, EnumTraceGroup_SERVER, _T("ComandElement::handleWriteRequest"), (TCHAR*)message.c_str());
			std::vector<Commands> availableCommands;
			doc->getCommands(availableCommands);

			for (size_t i = 0; i < availableCommands.size(); ++i)
			{
				Commands command = availableCommands.at(i);
				unsigned int executionResult = executeCommands(command.getAddress(), command.getName(), command.getDescription());
				aRequest->setResult(executionResult);

				if (SUCCEEDED(executionResult))
				{
					tstring message = command.getAddress() + tstring(_T(" ")) + command.getName() + tstring(_T(" ")) + command.getDescription();
					getApp()->trace(EnumTraceLevel_ERR, EnumTraceGroup_SERVER, _T("ComandElement::handleWriteRequest"), _T("Executed command: %s"), (TCHAR*)message.c_str());
					ValueQT* aValue = aRequest->getValue();
					aRequest->setResult(valueChanged(*aValue));
				} // end if
			} // end for

			delete doc;
		}
		catch (...)
		{
			aRequest->setResult(EnumResultCode_E_FAIL);
		} // end try catch

		aRequest->complete();
	}   //  end HandleItemWriteRequest

	unsigned int executeCommands(
		const tstring& anAddress,
		const tstring& aCommand,
		const tstring& aDescription)
	{
		//  Delegate command execution to the application
		AddressSpaceElement* targetElement;
		BisDaRoot* daRoot = (BisDaRoot*)getApp()->getDaAddressSpaceRoot();
		daRoot->findAddressSpaceElementData(anAddress, targetElement);
		BaseDaElement* element = dynamic_cast<BaseDaElement*>(targetElement);

		if (element == NULL)
		{
			//  the targeted element was not found
			return (unsigned int)EnumResultCode_E_FAIL;
		}   //  end if

		tstring message = anAddress + tstring(_T(" ")) + aCommand + tstring(_T(" ")) + aDescription;
		getApp()->trace(EnumTraceLevel_ERR, EnumTraceGroup_SERVER, _T("ComandElement::executeCommands"), (_T("Executed command: %s"), (TCHAR*)message.c_str()));
		return element->executeCommand(anAddress, aCommand, aDescription);
	} // end ExecuteCommand

	tstring& getCommandName(
		void)
	{
		return COMMAND_NAME;
	} // end getCommandName

}; // end CommandElement

#endif

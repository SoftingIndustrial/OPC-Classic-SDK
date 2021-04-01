package com.softing.opc.client.api;

import com.softing.opc.client.enums.EnumExecutionType;
import com.softing.opc.client.enums.EnumResultCode;
import com.softing.opc.client.enums.EnumTraceGroup;
import com.softing.opc.client.enums.EnumTraceLevel;
import com.softing.opc.client.enums.ResultCode;
import com.softing.opc.client.impl.jna.OTCAddressSpaceBrowseOptions;
import com.softing.opc.client.impl.jna.OTCAddressSpaceElementData;
import com.softing.opc.client.impl.jna.OTCExecutionOptions;
import com.softing.opc.client.impl.jna.OTClient;
import com.softing.opc.client.impl.jna.VARIANT;

import com.sun.jna.*;
import com.sun.jna.ptr.*;

import java.util.*;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class DaAddressSpaceElement extends AddressSpaceElement {

	final Logger logger = LoggerFactory.getLogger(DaAddressSpaceElement.class);
	private static final long serialVersionUID = 7526471155622776147L;
	
	protected String itemPath_ = null;
	protected DaSession session_ = null;
	
	public DaAddressSpaceElement(){
		itemPath_ = "";
	}

	protected DaAddressSpaceElement(String name, String itemId, String itemPath, NativeLong objectHandle, DaSession session){
		name_ = name;
		this.qualifiedName_ = itemId;
		itemPath_ = itemPath;
		objectElementHandle_ = objectHandle;
		session_ = session;
	}

	public int browse(DaAddressSpaceElementBrowseOptions browseOptions, ArrayList<DaAddressSpaceElement> addressSpaceElements, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();
		
		if(addressSpaceElements == null)
		{
			logger.error("Browse() - Parameter addressSpaceElements cannot be null");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT, 
				"DaAddressSpaceElement.Browse", "Parameter addressSpaceElements cannot be null"
				);
			*/
			return res;
		}
		
		addressSpaceElements.clear();
		OTCAddressSpaceBrowseOptions OTCBrowseOptions = new OTCAddressSpaceBrowseOptions();
        OTCBrowseOptions.m_accessRightsFilter = new NativeLong(0);
		
		
		if(this.getSession() == null){
			logger.error("Browse() - The Session property of the AddressSpaceElement cannot be null. Set the property to a value before function call");
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaAddressSpaceElement.Browse", 
				"The Session property of the AddressSpaceElement cannot be null! Set the property to a value before calling Browse!");
			*/
			return res;
		}
		try{
			OTCExecutionOptions options = new OTCExecutionOptions();

			if(executionOptions != null){
					options.m_executionType = (byte)executionOptions.getExecutionType().getInt();
					options.m_executionContext = new NativeLong(executionOptions.getExecutionContext());
			}
			else{
				options.m_executionType = (byte)EnumExecutionType.SYNCHRONOUS.getInt();
				options.m_executionContext = new NativeLong(0);
			}
			
			PointerByReference pOTCaddressSpaceElements = new PointerByReference();
			NativeLongByReference addressSpaceElementDataCount = new NativeLongByReference(new NativeLong(0));
			
			OTCBrowseOptions.m_type = (byte)browseOptions.getElementTypeFilter().getInt();
			
			if(browseOptions.getElementNameFilter() != null)
			{
				/*ByteByReference bbrElementNameFilter = new ByteByReference();
				Pointer pElementNameFilter = OTClient.INSTANCE.OTSysAllocString(new WString(browseOptions.getElementNameFilter()));
				bbrElementNameFilter.setPointer(pElementNameFilter);
				OTCBrowseOptions.m_elementNameFilter =  bbrElementNameFilter;
				*/
				Pointer pElementNameFilter = OTClient.INSTANCE.OTSysAllocString(new WString(browseOptions.getElementNameFilter()));
				OTCBrowseOptions.m_elementNameFilter =  pElementNameFilter;
			}
			if(browseOptions.getVendorFilter() != null)
			{
				/*ByteByReference bbrVendorFilter = new ByteByReference();
				Pointer pVendorFilter = OTClient.INSTANCE.OTSysAllocString(new WString(browseOptions.getVendorFilter()));
				bbrVendorFilter.setPointer(pVendorFilter);
				OTCBrowseOptions.m_vendorFilter =  bbrVendorFilter;
				*/
				Pointer pVendorFilter = OTClient.INSTANCE.OTSysAllocString(new WString(browseOptions.getVendorFilter()));
				OTCBrowseOptions.m_vendorFilter =  pVendorFilter;
			}
			
			OTCBrowseOptions.m_accessRightsFilter = new NativeLong(browseOptions.getAccessRightsFilter().toShort());               
			OTCBrowseOptions.m_datatypeFilter = VARIANT.getType(browseOptions.getDataTypeFilter());
			OTCBrowseOptions.m_maxElements = new NativeLong(browseOptions.getMaxElements());
			OTCBrowseOptions.m_retrieveItemID = (byte)(browseOptions.getIsRetrievingItemID()? 1:0);
			OTCBrowseOptions.m_retrieveProperties = (byte)(browseOptions.getIsReturningProperties()? 1:0);
			OTCBrowseOptions.m_retrievePropertyValues = (byte)(browseOptions.getIsReturningPropertyValues()? 1:0);
		
			Pointer pTemp = pOTCaddressSpaceElements.getValue();
			
			res = OTClient.INSTANCE.OTCBrowseAddressSpace(
				this.getSession().getHandle(),
				this.objectElementHandle_,
				new WString(this.getItemId()),
				new WString(this.getItemPath()),
				OTCBrowseOptions,
				addressSpaceElementDataCount,
				pOTCaddressSpaceElements,
				options).intValue();

			//addressSpaceElements = new DaAddressSpaceElement[addressSpaceElementDataCount.getValue().intValue()];
			
			if (options.m_executionType == (byte)EnumExecutionType.SYNCHRONOUS.getInt()){
				if(ResultCode.SUCCEEDED(res)){

					for (int i = 0; i< addressSpaceElementDataCount.getValue().intValue(); i++)
					{
						if(pOTCaddressSpaceElements.getValue().equals(pTemp))
						{
							//Nothing, the pointer is not valid
						}
						else
						{
							OTCAddressSpaceElementData myData = new OTCAddressSpaceElementData(pOTCaddressSpaceElements.getValue().share(i* new OTCAddressSpaceElementData().size()));
							addressSpaceElements.add(new DaAddressSpaceElement(
								myData.m_name.getString(0, true),
								myData.m_itemID.getString(0, true),
								myData.m_itemPath.getString(0, true),
								myData.m_objectHandle,
								this.getSession()));
							
							if (myData.m_itemID != null && myData.m_itemID!= null)
							{
								OTClient.INSTANCE.OTFreeMemory(myData.m_itemID);
							}
							if (myData.m_name != null && myData.m_name != null)
							{
								OTClient.INSTANCE.OTFreeMemory(myData.m_name);
							}
							if (myData.m_itemPath != null && myData.m_itemPath!=null)
							{
								OTClient.INSTANCE.OTFreeMemory(myData.m_itemPath);
							}
						}
					}

					if(pOTCaddressSpaceElements != null && pOTCaddressSpaceElements.getValue() != null){
						OTClient.INSTANCE.OTFreeMemory(pOTCaddressSpaceElements.getValue());
					}
				}
				else{
					logger.error("browse() - Browse failed. Result: " + res);
					/*
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaAddressSpaceElement.Browse", 
						"Browsing failed! Result: " + res);
					*/
				}
			}//if executionOptions Synchronous
			if(OTCBrowseOptions.m_elementNameFilter != null)
			{
				//OTClient.INSTANCE.OTFreeMemory(OTCBrowseOptions.m_elementNameFilter.getPointer());
				OTClient.INSTANCE.OTFreeMemory(OTCBrowseOptions.m_elementNameFilter); //UNICODE
			}
			if(OTCBrowseOptions.m_vendorFilter != null)
			{
				//OTClient.INSTANCE.OTFreeMemory(OTCBrowseOptions.m_vendorFilter.getPointer());
				OTClient.INSTANCE.OTFreeMemory(OTCBrowseOptions.m_vendorFilter); //UNICODE
			}
		}
		catch(Exception exc){
			logger.error("browse() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaAddressSpaceElement.Browse", 
				exc.getMessage());
			*/
		}
		return res;
	}

	public int getDaProperties(DaGetPropertiesOptions aDaGetPropertiesOptions, ArrayList<DaProperty> someDaProperties, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();	
		try{
			res = this.getSession().getDaProperties(
				this.getItemId(), 
				this.getItemPath(),
				aDaGetPropertiesOptions,
				someDaProperties,
				executionOptions);

			if (ResultCode.FAILED(res)){
				logger.error("getDaProperties() - Getting properties failed. Result: " + res);
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaAddressSpaceElement.GetDaProperties", 
					"Getting properties failed! Result: " + res);
				*/
			}
		}
		catch(Exception exc){
			logger.error("getDaProperties() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaAddressSpaceElement.GetDaProperties", 
				exc.getMessage());
			*/
			someDaProperties.clear();
		}
		return res;
	}

	public int getDaProperties(ArrayList<DaProperty> someDaProperties, ExecutionOptions executionOptions){
		int res = EnumResultCode.E_FAIL.toInt();			
		try{
			res = this.getSession().getDaProperties(
				this,
				this.getItemId(), 
				this.getItemPath(), 
				null, 
				someDaProperties,
				executionOptions);

			if (ResultCode.FAILED(res)){
				logger.error("getDaProperties() - Getting properties failed. Result: " + res);
				/*
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaAddressSpaceElement.GetDaProperties", 
					"Getting properties failed! Result: " + res);
				*/
			}
		}
		catch(Exception exc){
			logger.error("getDaProperties() - " + exc.getMessage(), exc);
			//exc.printStackTrace();
			/*
			Application.Instance.Trace(
				EnumTraceLevel.ERR,
				EnumTraceGroup.CLIENT,
				"DaAddressSpaceElement.GetDaProperties", 
				exc.getMessage());
			*/
			someDaProperties.clear();
		}
		return res;
	}


	public String getItemId(){
		return qualifiedName_;
	}

	public void setItemId(String itemId){
		qualifiedName_ = itemId;
	}

	public String getItemPath(){
		return itemPath_;
	}

	public void setItemPath(String itemPath){
		itemPath_ = itemPath;
	}

	public DaSession getSession(){
		return session_;
	}

	public void setSession(DaSession session){
		session_ = session;
	}

}
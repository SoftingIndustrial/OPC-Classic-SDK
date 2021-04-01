package com.softing.opc.client.api;

import com.softing.opc.client.enums.EnumAccessRights;


public class DaAddressSpaceElementBrowseOptions extends AddressSpaceElementBrowseOptions {

	protected int maxElements_ = 0;
	protected boolean isReturningProperties_ = false;
	protected boolean isReturningPropertyValues_ = false;
	protected boolean isRetrievingItemID_ = true;
	protected String vendorFilter_ = null;
	protected Class dataTypeFilter_ = null;
	protected EnumAccessRights accessRightsFilter_ = EnumAccessRights.READABLE;

	public DaAddressSpaceElementBrowseOptions(){

	}

	public int getMaxElements(){
		return maxElements_;
	}
	
	public void setMaxElements(int maxValue){
		maxElements_ = maxValue;
	}

	public String getVendorFilter(){
		return vendorFilter_;
	}
	
	public void setVendorFilter(String vendorFilter){
		vendorFilter_ = vendorFilter;
	}

	public EnumAccessRights getAccessRightsFilter(){
		return accessRightsFilter_;
	}
	
	public void setAccessRightsFilter(EnumAccessRights accessRights){
		accessRightsFilter_ = accessRights;
	}

	public Class getDataTypeFilter(){
		return dataTypeFilter_;
	}
	
	public void setDataTypeFilter(Class type){
		dataTypeFilter_ = type;
	}

	public boolean getIsReturningPropertyValues(){
		return isReturningPropertyValues_;
	}

	public void setIsReturningPropertyValues(boolean flag){
		isReturningPropertyValues_ = flag;
	}

	public boolean getIsReturningProperties(){
		return isReturningProperties_;
	}

	public void setIsReturningProperties(boolean flag){
		isReturningProperties_ = flag;
	}

	public boolean getIsRetrievingItemID(){
		return isRetrievingItemID_;
	}

	public void setIsRetrievingItemID(boolean flag){
		isRetrievingItemID_ = flag;
	}
}
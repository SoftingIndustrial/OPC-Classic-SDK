package com.softing.opc.client.api;

import com.softing.opc.client.enums.EnumAddressSpaceElementType;

public class AddressSpaceElementBrowseOptions {
	
	protected EnumAddressSpaceElementType elementTypeFilter_ = EnumAddressSpaceElementType.ALL;
	protected String elementNameFilter_ = null;

	public AddressSpaceElementBrowseOptions(){
	}


	public String getElementNameFilter(){
		return elementNameFilter_;
	}
	

	public void setElementNameFilter(String elementName){
		elementNameFilter_ = elementName;
	}


	public EnumAddressSpaceElementType getElementTypeFilter(){
		return elementTypeFilter_;
	}
	

	public void setElementTypeFilter(EnumAddressSpaceElementType elementType){
		elementTypeFilter_ = elementType;
	}
}
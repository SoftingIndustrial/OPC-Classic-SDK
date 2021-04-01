package com.softing.opc.client.enums;

import com.softing.opc.client.impl.jna.OTClient;

public enum EnumPropertyData {
	/**
	 * No data.
	 */
	NONE (OTClient.OTC_DAPROPERTYDATA_NONE),
	/**
	 * The value the property contains.
	 */
	VALUE (OTClient. OTC_DAPROPERTYDATA_VALUE),
	/**
	 * The ID of the item the property belongs to.
	 */
	ITEMID (OTClient.OTC_DAPROPERTYDATA_ITEMID),
	/**
	 * All the data that is related to the property.
	 */
	ALL (OTClient.OTC_DAPROPERTYDATA_ALL);
	
	private final int propertyData_;
	
	private EnumPropertyData(int pData){
		this.propertyData_ = pData;
	}
	
	public int getInt() { return propertyData_; } 
}
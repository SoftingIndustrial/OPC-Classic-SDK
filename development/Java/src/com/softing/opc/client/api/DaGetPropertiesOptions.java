package com.softing.opc.client.api;

import com.softing.opc.client.enums.EnumPropertyData;


public class DaGetPropertiesOptions {

	protected EnumPropertyData whatPropertyData_ = EnumPropertyData.NONE;
	protected int[] pPropertyIds_ = null;
	protected String[] pPropertyNames_ = null;


	public DaGetPropertiesOptions(){

	}
	
	public EnumPropertyData getWhatPropertyData(){
		return whatPropertyData_;
	}

	public void setWhatPropertyData(EnumPropertyData whatPropertyData){
		whatPropertyData_ = whatPropertyData;
	}

	public int[] getPropertyIds(){
		return pPropertyIds_;
	}

	public void setPropertyIds(int[] propertyIds){
		pPropertyIds_ = propertyIds;
	}

	public String[] getPropertyNames(){
		return pPropertyNames_;
	}

	public void setPropertyNames(String[] propertyNames){
		pPropertyNames_ = propertyNames;
	}

}
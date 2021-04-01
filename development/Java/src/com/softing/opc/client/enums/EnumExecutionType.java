package com.softing.opc.client.enums;

import com.softing.opc.client.impl.jna.OTClient;

public enum EnumExecutionType {
	/**
	 * The execution is synchronous.
	 */
	SYNCHRONOUS (OTClient.OTC_EXECUTIONTYPE_SYCHRONOUS),
	/**
	 * The execution is asynchronous.
	 */
	ASYNCHRONOUS (OTClient.OTC_EXECUTIONTYPE_ASYCHRONOUS);
	
	private final int typeId;
	
	private EnumExecutionType(int executionType){
		this.typeId = executionType;
	}
	
	public int getInt() { return typeId; }
}
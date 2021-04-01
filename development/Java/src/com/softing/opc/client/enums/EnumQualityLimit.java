package com.softing.opc.client.enums;

import com.softing.opc.client.impl.jna.OTClient;
//see enums.cs

public enum EnumQualityLimit {

	OK (OTClient.OT_QUALITY_LIMIT_OK),
	LOW (OTClient.OT_QUALITY_LIMIT_LOW),
	HIGH (OTClient.OT_QUALITY_LIMIT_HIGH),
	CONSTANT (OTClient.OT_QUALITY_LIMIT_CONST);
	
	private final short equalityLimit;
	
	EnumQualityLimit(int value)
	{
		equalityLimit = (short)value;
	}
	
	public short toShort()
	{
		return equalityLimit;
	}
	
}

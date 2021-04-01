package com.softing.opc.client.enums;

import com.softing.opc.client.impl.jna.OTClient;

//see enums.cs

public enum EnumEventType {

	SIMPLE (OTClient.OT_EVENTTYPE_SIMPLE),
	TRACKING (OTClient.OT_EVENTTYPE_TRACKING),
	CONDITION (OTClient.OT_EVENTTYPE_CONDITION);
	
	private final short eventType;
	
	EnumEventType(int value)
	{
		eventType = (short)value;
	}
	
	public short toShort()
	{
		return eventType;
	}
	
}

package com.softing.opc.client.enums;

import com.softing.opc.client.impl.jna.OTClient;
//see enums.cs
public enum EnumConditionChange {

	ACTIVE_STATE (OTClient.OT_CONDITIONCHANGE_ACTIVE_STATE),
	ACK_STATE (OTClient.OT_CONDITIONCHANGE_ACK_STATE),
	ENABLE_STATE (OTClient.OT_CONDITIONCHANGE_ENABLE_STATE),
	STATE (OTClient.OT_CONDITIONCHANGE_STATE),
	QUALITY (OTClient.OT_CONDITIONCHANGE_QUALITY),
	SEVERITY (OTClient.OT_CONDITIONCHANGE_SEVERITY),
	SUBCONDITION (OTClient.OT_CONDITIONCHANGE_SUBCONDITION),
	MESSAGE (OTClient.OT_CONDITIONCHANGE_MESSAGE),
	ATTRIBUTE (OTClient.OT_CONDITIONCHANGE_ATTRIBUTE),
	ACK_DATA (OTClient.OT_CONDITIONCHANGE_ACK_DATA),;
	
	
	private final short conditionChange;
	
	EnumConditionChange(int value)
	{
		conditionChange = (short)value;
	}
	
	public short toShort()
	{
		return conditionChange;
	}
	
}

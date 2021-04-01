package com.softing.opc.client.enums;

import com.softing.opc.client.impl.jna.OTClient;

//see enums.cs

public enum EnumConditionState {

	NO (0x00),
	ENABLED (OTClient.OT_CONDITIONSTATE_ENABLED),
	ACTIVE (OTClient.OT_CONDITIONSTATE_ACTIVE),
	ACKED (OTClient.OT_CONDITIONSTATE_ACKED);
	
	private final short conditionState;
	
	EnumConditionState(int value)
	{
		conditionState = (short)value;
	}
	
	public short toShort()
	{
		return conditionState;
	}
}

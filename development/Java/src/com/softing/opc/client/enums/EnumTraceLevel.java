package com.softing.opc.client.enums;

import com.softing.opc.client.impl.jna.OTClient;

public enum EnumTraceLevel {

	ERR (OTClient.OT_TRACELEVEL_ERR),
	WRN (OTClient.OT_TRACELEVEL_WRN),
	INF (OTClient.OT_TRACELEVEL_INF),
	DEB (OTClient.OT_TRACELEVEL_DEB);
	
	private byte traceLevel;
	
	EnumTraceLevel(int value)
	{
		traceLevel = (byte)value;
	}
	
	public byte toByte()
	{
		return traceLevel;
	}
	
}

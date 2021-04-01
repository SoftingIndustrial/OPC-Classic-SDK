package com.softing.opc.client.enums;

import com.softing.opc.client.impl.jna.OTClient;

public enum EnumTraceData {

	TRACEDATA_ALL (OTClient.OT_TRACEDATA_ALL),
	TRACEDATA_LEVEL (OTClient.OT_TRACEDATA_LEVEL),
	TRACEDATA_LEVEL_ERR (OTClient.OT_TRACEDATA_LEVEL_ERR),
	TRACEDATA_LEVEL_WRN (OTClient.OT_TRACEDATA_LEVEL_WRN),
	TRACEDATA_LEVEL_INF (OTClient.OT_TRACEDATA_LEVEL_INF),
	TRACEDATA_LEVEL_DEB (OTClient.OT_TRACEDATA_LEVEL_DEB),
	TRACEDATA_FILE (OTClient.OT_TRACEDATA_FILE),
	TRACEDATA_FILE_ENABLE (OTClient.OT_TRACEDATA_FILE_ENABLE),
	TRACEDATA_FILE_NAME (OTClient.OT_TRACEDATA_FILE_NAME),
	TRACEDATA_MAX_BACKUPS (OTClient.OT_TRACEDATA_MAX_BACKUPS),
	TRACEDATA_FILE_SIZE (OTClient.OT_TRACEDATA_FILE_SIZE);
	
	private final int traceData;
	
	EnumTraceData(int value)
	{
		traceData = (int)value;
	}
	
	public int toInt()
	{
		return traceData;
	}
	
}

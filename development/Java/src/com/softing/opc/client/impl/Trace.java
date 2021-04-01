package com.softing.opc.client.impl;


import com.softing.opc.client.enums.EnumTraceData;
import com.softing.opc.client.impl.jna.OTClient;
import com.softing.opc.client.impl.jna.OTTraceData;
import com.sun.jna.*;

public class Trace {

	private static OTTraceData traceData = new OTTraceData() ;
	private Trace(){};
	
	private static void SetTraceOptions(EnumTraceData aTraceDataMask) {
		OTClient.INSTANCE.OTEnableTracing(new NativeLong(aTraceDataMask.toInt()), traceData);
	}
	
	public static void WriteLine(byte level, int mask, String objectID, String message) {
		OTClient.INSTANCE.OTTrace(level, new NativeLong(mask), new WString(objectID), new WString(message));
	}
	
	public static void WriteLine(byte level, long mask, String objectID, String message, Object...args) {
		String formattedString = String.format(message,args);
		OTClient.INSTANCE.OTTrace(level, new NativeLong(mask), new WString(objectID), new WString(formattedString));
	}
	
	public static long getWarningLevelMask()
	{
		return traceData.m_warningLevelMask.intValue();
	}
	
	public static void setWarningLevelMask(long value)
	{
		traceData.m_warningLevelMask.setValue(value);
		SetTraceOptions(EnumTraceData.TRACEDATA_LEVEL_WRN);
	}
	
	public static long getDebugLevelMask()
	{
		return traceData.m_debugLevelMask.longValue();
	}
	
	public static void setDebugLevelMask(long value)
	{
		traceData.m_debugLevelMask.setValue(value);
		SetTraceOptions(EnumTraceData.TRACEDATA_LEVEL_DEB);
	}

	public static long getErrorLevelMask()
	{
		return traceData.m_errorLevelMask.intValue();
	}
	
	public static void setErrorLevelMask(long value)
	{
		traceData.m_errorLevelMask.setValue(value);
		SetTraceOptions(EnumTraceData.TRACEDATA_LEVEL_ERR);
	}
	
	public static long getInfoLevelMask()
	{
		return traceData.m_infoLevelMask.longValue();
	}
	
	public static void setInfoLevelMask(long value)
	{
		traceData.m_infoLevelMask.setValue(value);
		SetTraceOptions(EnumTraceData.TRACEDATA_LEVEL_INF);
	}
	
	public static boolean getEnableTraceToFile()
	{
		return (traceData.m_enableTraceToFile !=0);
	}
	
	public static void setEnableTraceToFile(boolean value)
	{
		traceData.m_enableTraceToFile = value? (byte)1:(byte)0;
		SetTraceOptions(EnumTraceData.TRACEDATA_FILE_ENABLE);
	}
	
	public static int getFileMaxSize()
	{
		return traceData.m_maxFileSize.intValue();
	}
	
	public static void setFileMaxSize(int value)
	{
		traceData.m_maxFileSize.setValue(value);
		SetTraceOptions(EnumTraceData.TRACEDATA_FILE_SIZE);
	}
	
	public static String getFileName()
	{
		return traceData.m_fileName.toString();
	}
	
	public static void setFileName(String value)
	{
		traceData.m_fileName = new WString(value);
		SetTraceOptions(EnumTraceData.TRACEDATA_FILE_NAME);
	}
	
	public static long getMaximumBackups()
	{
		return traceData.m_maxBackups.longValue();
	}
	
	public static void setMaximumBackups(long value)
	{
		traceData.m_maxBackups.setValue(value);
		SetTraceOptions(EnumTraceData.TRACEDATA_MAX_BACKUPS);
	}
	
}

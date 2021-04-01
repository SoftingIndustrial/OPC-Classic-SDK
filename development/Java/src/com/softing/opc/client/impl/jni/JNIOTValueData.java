package com.softing.opc.client.impl.jni;


public class JNIOTValueData {
	public Object m_pValue;
	public short m_quality;
	public JNIFILETIME m_timestamp;
	
	public JNIOTValueData(Object value, short quality, JNIFILETIME timestamp)
	{
		m_pValue = value;
		m_quality = quality;
		m_timestamp = timestamp;
	}
}

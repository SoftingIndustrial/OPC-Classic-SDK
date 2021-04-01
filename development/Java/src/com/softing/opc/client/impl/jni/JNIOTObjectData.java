package com.softing.opc.client.impl.jni;

public class JNIOTObjectData {
	public long m_objectHandle;
	public long m_userData;
	
	public JNIOTObjectData()
	{
	}
	
	public JNIOTObjectData(long objectHandle, long userData)
	{
		m_objectHandle = objectHandle;
		m_userData = userData;
	}
}

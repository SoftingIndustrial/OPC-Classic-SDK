package com.softing.opc.client.impl.jni;


public class JNIOTCObjectContext {
	
	public JNIOTObjectData m_objectData;
	public long m_objectType;
	public JNIOTObjectData m_sessionData;
	public JNIOTObjectData m_subscriptionData;
	
	public JNIOTCObjectContext(JNIOTObjectData objectData, long objectType, JNIOTObjectData sessionData, JNIOTObjectData subscriptionData) 
	{
		m_objectData = objectData;
		m_objectType = objectType;
		m_sessionData = sessionData;
		m_subscriptionData = subscriptionData;
	}
	
	public JNIOTCObjectContext()
	{
	}
}

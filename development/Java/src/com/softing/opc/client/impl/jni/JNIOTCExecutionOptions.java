package com.softing.opc.client.impl.jni;

public class JNIOTCExecutionOptions {
	public byte m_executionType;	
	public long m_executionContext;

	public JNIOTCExecutionOptions()
	{}

	public JNIOTCExecutionOptions(com.softing.opc.client.impl.jna.OTCExecutionOptions executionOptions)
	{
		this.m_executionType = executionOptions.m_executionType;
		this.m_executionContext = executionOptions.m_executionContext.longValue();
	}

}
package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCExecutionOptions extends Structure {
	public byte m_executionType;
	//added by TOS - pragma pack(4) problem
	public byte dummy1;
	public byte dummy2;
	public byte dummy3;
	
	public com.sun.jna.NativeLong m_executionContext;
	public OTCExecutionOptions() {
		super();
	}
	public OTCExecutionOptions(byte m_executionType, com.sun.jna.NativeLong m_executionContext) {
		super();
		this.m_executionType = m_executionType;
		this.m_executionContext = m_executionContext;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCExecutionOptions)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCExecutionOptions)clone));
	}
	
	OTCExecutionOptions setupClone (OTCExecutionOptions clone)
	{
		clone.m_executionType = this.m_executionType;
		clone.m_executionContext = this.m_executionContext;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCExecutionOptions clone() { return setupClone(new OTCExecutionOptions()); }
	public static class ByReference extends OTCExecutionOptions implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCExecutionOptions implements com.sun.jna.Structure.ByValue {}
}

package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTTraceData extends Structure {
	public com.sun.jna.NativeLong m_errorLevelMask;
	public com.sun.jna.NativeLong m_warningLevelMask;
	public com.sun.jna.NativeLong m_infoLevelMask;
	public com.sun.jna.NativeLong m_debugLevelMask;
	public byte m_enableTraceToFile;
	//Added by tos pragma pack(4) problem
	private byte dummy1;
	private byte dummy2;
	private byte dummy3;
	//
	//changed by TOS
	//public com.sun.jna.ptr.ByteByReference m_fileName;
	public WString m_fileName;
	public com.sun.jna.NativeLong m_maxFileSize;
	public com.sun.jna.NativeLong m_maxBackups;
	public OTTraceData() {
		super();
	}
	public OTTraceData(com.sun.jna.NativeLong m_errorLevelMask, com.sun.jna.NativeLong m_warningLevelMask, com.sun.jna.NativeLong m_infoLevelMask, com.sun.jna.NativeLong m_debugLevelMask, byte m_enableTraceToFile, WString m_fileName, com.sun.jna.NativeLong m_maxFileSize, com.sun.jna.NativeLong m_maxBackups) {
		super();
		this.m_errorLevelMask = m_errorLevelMask;
		this.m_warningLevelMask = m_warningLevelMask;
		this.m_infoLevelMask = m_infoLevelMask;
		this.m_debugLevelMask = m_debugLevelMask;
		this.m_enableTraceToFile = m_enableTraceToFile;
		this.m_fileName = m_fileName;
		this.m_maxFileSize = m_maxFileSize;
		this.m_maxBackups = m_maxBackups;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTTraceData)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTTraceData)clone));
	}
	
	OTTraceData setupClone (OTTraceData clone)
	{
		clone.m_errorLevelMask = this.m_errorLevelMask;
		clone.m_warningLevelMask = this.m_warningLevelMask;
		clone.m_infoLevelMask = this.m_infoLevelMask;
		clone.m_debugLevelMask = this.m_debugLevelMask;
		clone.m_enableTraceToFile = this.m_enableTraceToFile;
		clone.m_fileName = this.m_fileName;
		clone.m_maxFileSize = this.m_maxFileSize;
		clone.m_maxBackups = this.m_maxBackups;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTTraceData clone() { return setupClone(new OTTraceData()); }
	public static class ByReference extends OTTraceData implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTTraceData implements com.sun.jna.Structure.ByValue {}
}


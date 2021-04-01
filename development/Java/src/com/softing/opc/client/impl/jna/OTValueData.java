package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTValueData extends Structure {
	public com.softing.opc.client.impl.jna.VARIANT m_pValue;
	public short m_quality;
	private byte dummy1;
	private byte dummy2;
	public com.softing.opc.client.impl.jna.FILETIME m_timestamp = new com.softing.opc.client.impl.jna.FILETIME();
	
	public OTValueData() {
		super();
	}
	
	public OTValueData(Pointer p)
	{
		super();
		this.useMemory(p);
		this.read();
	}
	
	public OTValueData(com.softing.opc.client.impl.jna.VARIANT m_pValue, short m_quality, com.softing.opc.client.impl.jna.FILETIME m_timestamp) {
		super();
		this.m_pValue = m_pValue;
		this.m_quality = m_quality;
		this.m_timestamp = m_timestamp;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTValueData)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTValueData)clone));
	}
	
	OTValueData setupClone (OTValueData clone)
	{
		clone.m_pValue = this.m_pValue;
		clone.m_quality = this.m_quality;
		clone.m_timestamp = this.m_timestamp;
		return clone;
	}
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTValueData clone() { return setupClone(new OTValueData()); }
	public static class ByReference extends OTValueData implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTValueData implements com.sun.jna.Structure.ByValue {}
}
package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCDAPropertyData extends Structure {
	public com.sun.jna.NativeLong m_ID;
	public Pointer m_name = null;
	public Pointer m_itemID = null;
	public Pointer m_itemPath = null;
	public Pointer m_description = null;
	public short m_datatype;
	public byte dummy1;
	public byte dummy2;
	public com.softing.opc.client.impl.jna.OTValueData m_value;
	public com.sun.jna.NativeLong m_result;
	
	public OTCDAPropertyData() {
		super();
	}
	
	public OTCDAPropertyData(Pointer p)
	{
		super();
		this.useMemory(p);
		this.read();
	}
	
	public OTCDAPropertyData(com.sun.jna.NativeLong m_ID, Pointer m_name, Pointer m_itemID, Pointer m_itemPath, Pointer m_description, short m_datatype, com.softing.opc.client.impl.jna.OTValueData m_value, com.sun.jna.NativeLong m_result) {
		super();
		this.m_ID = m_ID;
		this.m_name = m_name;
		this.m_itemID = m_itemID;
		this.m_itemPath = m_itemPath;
		this.m_description = m_description;
		this.m_datatype = m_datatype;
		this.m_value = m_value;
		this.m_result = m_result;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCDAPropertyData)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCDAPropertyData)clone));
	}
	
	OTCDAPropertyData setupClone (OTCDAPropertyData clone)
	{
		clone.m_ID = this.m_ID;
		clone.m_name = this.m_name;
		clone.m_itemID = this.m_itemID;
		clone.m_itemPath = this.m_itemPath;
		clone.m_description = this.m_description;
		clone.m_datatype = this.m_datatype;
		clone.m_value = this.m_value;
		clone.m_result = this.m_result;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCDAPropertyData clone() { return setupClone(new OTCDAPropertyData()); }
	public static class ByReference extends OTCDAPropertyData implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCDAPropertyData implements com.sun.jna.Structure.ByValue {}
}

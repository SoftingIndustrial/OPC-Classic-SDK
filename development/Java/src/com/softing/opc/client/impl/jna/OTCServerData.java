package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCServerData extends Structure {
	//public com.sun.jna.ptr.ByteByReference m_clsid;
	public com.sun.jna.Pointer m_clsid; //UNICODE
	
	//public com.sun.jna.ptr.ByteByReference m_progId;
	public com.sun.jna.Pointer m_progId; //UNICODE
	
	//public com.sun.jna.ptr.ByteByReference m_vProgId;
	public com.sun.jna.Pointer m_vProgId; //UNICODE
	
	//public com.sun.jna.ptr.ByteByReference m_description;
	public com.sun.jna.Pointer m_description; //UNICODE
	
	//public com.sun.jna.ptr.ByteByReference m_url;
	public com.sun.jna.Pointer m_url; //UNICODE
	
	public byte m_opcSpecifications;
	public byte dummy1;
	public byte dummy2;
	public byte dummy3;
	
	public OTCServerData() {
		super();
	}
	
	/*public OTCServerData(com.sun.jna.ptr.ByteByReference m_clsid, com.sun.jna.ptr.ByteByReference m_progId, com.sun.jna.ptr.ByteByReference m_vProgId, com.sun.jna.ptr.ByteByReference m_description, com.sun.jna.ptr.ByteByReference m_url, byte m_opcSpecifications) {
		super();
		this.m_clsid = m_clsid;
		this.m_progId = m_progId;
		this.m_vProgId = m_vProgId;
		this.m_description = m_description;
		this.m_url = m_url;
		this.m_opcSpecifications = m_opcSpecifications;
	}*/
	
	public OTCServerData(Pointer m_clsid, Pointer m_progId, Pointer m_vProgId, Pointer m_description, Pointer m_url, byte m_opcSpecifications) {
		super();
		this.m_clsid = m_clsid;
		this.m_progId = m_progId;
		this.m_vProgId = m_vProgId;
		this.m_description = m_description;
		this.m_url = m_url;
		this.m_opcSpecifications = m_opcSpecifications;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCServerData)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCServerData)clone));
	}
	
	OTCServerData setupClone (OTCServerData clone)
	{
		clone.m_clsid = this.m_clsid;
		clone.m_progId = this.m_progId;
		clone.m_vProgId = this.m_vProgId;
		clone.m_description = this.m_description;
		clone.m_url = this.m_url;
		clone.m_opcSpecifications = this.m_opcSpecifications;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCServerData clone() { return setupClone(new OTCServerData()); }
	public static class ByReference extends OTCServerData implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCServerData implements com.sun.jna.Structure.ByValue {}
}
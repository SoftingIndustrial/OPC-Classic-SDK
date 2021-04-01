package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCInitData extends Structure {
	/// used OTC version
	public short m_versionOTC;
	//Added by TOS - pragma pack(push, 4) problem
	public short dummy1;
	/**
	 * server identity<br>
	 * name of service; if no service NULL
	 */
	//public com.sun.jna.ptr.ByteByReference m_serviceName;
	public com.sun.jna.Pointer m_serviceName; //UNICODE
	
	public OTCInitData() {
		super();
	}
	/**
	 * @param m_versionOTC used OTC version<br>
	 * @param m_serviceName server identity<br>
	 * name of service; if no service NULL
	 */
	public OTCInitData(short m_versionOTC, com.sun.jna.Pointer m_serviceName) {
		super();
		this.m_versionOTC = m_versionOTC;
		this.m_serviceName = m_serviceName;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCInitData)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCInitData)clone));
	}
	
	OTCInitData setupClone (OTCInitData clone)
	{
		clone.m_versionOTC = this.m_versionOTC;
		clone.m_serviceName = this.m_serviceName;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCInitData clone() { return setupClone(new OTCInitData()); }
	public static class ByReference extends OTCInitData implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCInitData implements com.sun.jna.Structure.ByValue {}
}

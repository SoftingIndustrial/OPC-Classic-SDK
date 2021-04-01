package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTObjectData extends Structure {
	
	public com.sun.jna.NativeLong m_objectHandle;
	public com.sun.jna.NativeLong m_userData;
	public OTObjectData() {
		super();
	}
	public OTObjectData(com.sun.jna.NativeLong m_objectHandle, com.sun.jna.NativeLong m_userData) {
		super();
		//setAlignType(ALIGN_NONE);
		this.m_objectHandle = m_objectHandle;
		this.m_userData = m_userData;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTObjectData)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTObjectData)clone));
	}
	
	OTObjectData setupClone (OTObjectData clone)
	{
		clone.m_objectHandle = this.m_objectHandle;
		clone.m_userData = this.m_userData;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTObjectData clone() { return setupClone(new OTObjectData()); }
	public static class ByReference extends OTObjectData implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTObjectData implements com.sun.jna.Structure.ByValue {}
}

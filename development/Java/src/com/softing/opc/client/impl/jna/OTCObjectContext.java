package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCObjectContext extends Structure {
	public com.softing.opc.client.impl.jna.OTObjectData m_objectData;
	public com.sun.jna.NativeLong m_objectType;
	public com.softing.opc.client.impl.jna.OTObjectData m_sessionData;
	public com.softing.opc.client.impl.jna.OTObjectData m_subscriptionData;
	public OTCObjectContext() {
		super();
	}
	public OTCObjectContext(com.softing.opc.client.impl.jna.OTObjectData m_objectData, com.sun.jna.NativeLong m_objectType, com.softing.opc.client.impl.jna.OTObjectData m_sessionData, com.softing.opc.client.impl.jna.OTObjectData m_subscriptionData) {
		super();
		//setAlignType(ALIGN_NONE);
		this.m_objectData = m_objectData;
		this.m_objectType = m_objectType;
		this.m_sessionData = m_sessionData;
		this.m_subscriptionData = m_subscriptionData;
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCObjectContext)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCObjectContext)clone));
	}
	
	OTCObjectContext setupClone (OTCObjectContext clone)
	{
		clone.m_objectData = this.m_objectData;
		clone.m_objectType = this.m_objectType;
		clone.m_sessionData = this.m_sessionData;
		clone.m_subscriptionData = this.m_subscriptionData;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCObjectContext clone() { return setupClone(new OTCObjectContext()); }
	public static class ByReference extends OTCObjectContext implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCObjectContext implements com.sun.jna.Structure.ByValue {}
}

package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCEventData extends Structure {
	/// simple event data
	public com.sun.jna.NativeLong m_eventType;
	/// event category id
	public com.sun.jna.NativeLong m_eventCategory;
	/// severity
	public com.sun.jna.NativeLong m_severity;
	/// fully qualified path of event source
	//public com.sun.jna.ptr.ByteByReference m_sourcePath;
	public com.sun.jna.Pointer m_sourcePath; //UNICODE
	/// event message
	//public com.sun.jna.ptr.ByteByReference m_message;
	public com.sun.jna.Pointer m_message; //UNICODE
	/// time of event occurence
	public com.softing.opc.client.impl.jna.FILETIME m_occurenceTime;
	public com.sun.jna.NativeLong m_eventAttrCount;
	/// array with all event attributes
	public com.softing.opc.client.impl.jna.VARIANT m_pEventAttrs;
	/// tracking and condition related event data
	//public com.sun.jna.ptr.ByteByReference m_actorID;
	public com.sun.jna.Pointer m_actorID; //UNICODE
	/// condition related event data
	public short m_changeMask;
	public byte m_newState;
	public byte m_ackRequired;
	public short m_quality;
	//public com.sun.jna.ptr.ByteByReference m_conditionName;
	public com.sun.jna.Pointer m_conditionName; //UNICODE
	//public com.sun.jna.ptr.ByteByReference m_subConditionName;
	public com.sun.jna.Pointer m_subConditionName; //UNICODE
	public com.softing.opc.client.impl.jna.FILETIME m_activeTime;
	public com.sun.jna.NativeLong m_cookie;
	public OTCEventData() {
		super();
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCEventData)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCEventData)clone));
	}
	
	OTCEventData setupClone (OTCEventData clone)
	{
		clone.m_eventType = this.m_eventType;
		clone.m_eventCategory = this.m_eventCategory;
		clone.m_severity = this.m_severity;
		clone.m_sourcePath = this.m_sourcePath;
		clone.m_message = this.m_message;
		clone.m_occurenceTime = this.m_occurenceTime;
		clone.m_eventAttrCount = this.m_eventAttrCount;
		clone.m_pEventAttrs = this.m_pEventAttrs;
		clone.m_actorID = this.m_actorID;
		clone.m_changeMask = this.m_changeMask;
		clone.m_newState = this.m_newState;
		clone.m_ackRequired = this.m_ackRequired;
		clone.m_quality = this.m_quality;
		clone.m_conditionName = this.m_conditionName;
		clone.m_subConditionName = this.m_subConditionName;
		clone.m_activeTime = this.m_activeTime;
		clone.m_cookie = this.m_cookie;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCEventData clone() { return setupClone(new OTCEventData()); }
	public static class ByReference extends OTCEventData implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCEventData implements com.sun.jna.Structure.ByValue {}
}


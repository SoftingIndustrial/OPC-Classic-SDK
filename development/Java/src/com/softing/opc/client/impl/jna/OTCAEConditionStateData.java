package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCAEConditionStateData extends Structure {
	/// condition state data
	public byte m_state;
	public com.sun.jna.ptr.ByteByReference m_actSubConditionName;
	public com.sun.jna.ptr.ByteByReference m_actSubConditionDefinition;
	public com.sun.jna.NativeLong m_actSubConditionSeverity;
	public com.sun.jna.ptr.ByteByReference m_actSubConditionDescription;
	public com.softing.opc.client.impl.jna.FILETIME m_conditionActiveTime;
	public com.softing.opc.client.impl.jna.FILETIME m_conditionAckTime;
	public com.softing.opc.client.impl.jna.FILETIME m_subConditionActiveTime;
	public com.softing.opc.client.impl.jna.FILETIME m_conditionInactiveTime;
	public short m_quality;
	public com.sun.jna.ptr.ByteByReference m_ackID;
	public com.sun.jna.ptr.ByteByReference m_ackComment;
	public com.sun.jna.NativeLong m_subConditionCount;
	public com.sun.jna.ptr.PointerByReference m_pSubConditionDefinition;
	public com.sun.jna.ptr.NativeLongByReference m_pSubConditionSeverity;
	public com.sun.jna.ptr.PointerByReference m_pSubConditionDescription;
	public com.sun.jna.ptr.PointerByReference m_pSubConditionName;
	public com.sun.jna.NativeLong m_eventAttrCount;
	/// array with all event attributes
	public com.softing.opc.client.impl.jna.VARIANT m_pEventAttrs;
	public com.sun.jna.ptr.NativeLongByReference m_pAttrErrors;
	public OTCAEConditionStateData() {
		super();
	}
	
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCAEConditionStateData)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCAEConditionStateData)clone));
	}
	
	OTCAEConditionStateData setupClone (OTCAEConditionStateData clone)
	{
		clone.m_state = this.m_state;
		clone.m_actSubConditionName = this.m_actSubConditionName;
		clone.m_actSubConditionDefinition = this.m_actSubConditionDefinition;
		clone.m_actSubConditionSeverity = this.m_actSubConditionSeverity;
		clone.m_actSubConditionDescription = this.m_actSubConditionDescription;
		clone.m_conditionActiveTime = this.m_conditionActiveTime;
		clone.m_conditionAckTime = this.m_conditionAckTime;
		clone.m_subConditionActiveTime = this.m_subConditionActiveTime;
		clone.m_conditionInactiveTime = this.m_conditionInactiveTime;
		clone.m_quality = this.m_quality;
		clone.m_ackID = this.m_ackID;
		clone.m_ackComment = this.m_ackComment;
		clone.m_subConditionCount = this.m_subConditionCount;
		clone.m_pSubConditionDefinition = this.m_pSubConditionDefinition;
		clone.m_pSubConditionSeverity = this.m_pSubConditionSeverity;
		clone.m_pSubConditionDescription = this.m_pSubConditionDescription;
		clone.m_pSubConditionName = this.m_pSubConditionName;
		clone.m_eventAttrCount = this.m_eventAttrCount;
		clone.m_pEventAttrs = this.m_pEventAttrs;
		clone.m_pAttrErrors = this.m_pAttrErrors;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCAEConditionStateData clone() { return setupClone(new OTCAEConditionStateData()); }
	public static class ByReference extends OTCAEConditionStateData implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCAEConditionStateData implements com.sun.jna.Structure.ByValue {}
}


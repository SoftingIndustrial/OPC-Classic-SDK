package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCObjectAttributes extends Structure {
	/// DA Session
	public com.sun.jna.NativeLong m_daSessionLCID;
	//public com.sun.jna.ptr.ByteByReference m_daSessionClientName;
	public com.sun.jna.Pointer m_daSessionClientName; //UNICODE
	
	//public com.sun.jna.ptr.ByteByReference m_daSessionURL;
	public com.sun.jna.Pointer m_daSessionURL; //UNICODE
	
	public byte m_daSessionSupportedOPCSpecification;
	public byte m_daSessionForcedOPCSpecification;
	public byte dummy1;
	public byte dummy2;
	public com.sun.jna.NativeLong m_daSessionComClsctx;
	/// DA Subscription
	//public com.sun.jna.ptr.ByteByReference m_daSubscriptionName;
	public com.sun.jna.Pointer m_daSubscriptionName; //UNICODE
	
	public com.sun.jna.NativeLong m_daSubscriptionRequestedUpdateRate;
	public com.sun.jna.NativeLong m_daSubscriptionRevisedUpdateRate;
	public float m_daSubscriptionDeadband;
	public com.sun.jna.NativeLong m_daSubscriptionTimeBias;
	public com.sun.jna.NativeLong m_daSubscriptionLCID;
	public com.sun.jna.NativeLong m_daSubscriptionRequestedKeepAliveTime;
	public com.sun.jna.NativeLong m_daSubscriptionRevisedKeepAliveTime;
	/// DA Item
	//public com.sun.jna.ptr.ByteByReference m_daItemID;
	public com.sun.jna.Pointer m_daItemID; //UNICODE
	
	//public com.sun.jna.ptr.ByteByReference m_daItemPath;
	public com.sun.jna.Pointer m_daItemPath; //UNICODE
	
	public short m_daItemNativeDatatype;
	public short m_daItemRequestedDatatype;
	public com.sun.jna.NativeLong m_daItemAccessRights;
	public float m_daItemDeadband;
	public byte m_daItemEuType;
	public byte dummy3;
	public byte dummy4;
	public byte dummy5;
	public VARIANT m_daItemEuInfo;
	/// AE Session
	public com.sun.jna.NativeLong m_aeSessionLCID;
	//public com.sun.jna.ptr.ByteByReference m_aeSessionClientName;
	public com.sun.jna.Pointer m_aeSessionClientName; //UNICODE
	
	//public com.sun.jna.ptr.ByteByReference m_aeSessionURL;
	public com.sun.jna.Pointer m_aeSessionURL; //UNICODE
	
	public com.sun.jna.NativeLong m_aeSessionComClsctx;
	/// AE Subscription
	public com.sun.jna.NativeLong m_aeSubscriptionRequestedBufferTime;
	public com.sun.jna.NativeLong m_aeSubscriptionRevisedBufferTime;
	public com.sun.jna.NativeLong m_aeSubscriptionRequestedMaxSize;
	public com.sun.jna.NativeLong m_aeSubscriptionRevisedMaxSize;
	public com.sun.jna.NativeLong m_aeSubscriptionFilterSeverityLow;
	public com.sun.jna.NativeLong m_aeSubscriptionFilterSeverityHigh;
	public com.sun.jna.NativeLong m_aeSubscriptionFilterEventTypes;
	public com.sun.jna.NativeLong m_aeSubscriptionFilterCountCategrories;
	public com.sun.jna.ptr.NativeLongByReference m_aeSubscriptionFilterCategrories;
	public com.sun.jna.NativeLong m_aeSubscriptionFilterCountAreas;
	public com.sun.jna.ptr.PointerByReference m_aeSubscriptionFilterAreas;
	public com.sun.jna.NativeLong m_aeSubscriptionFilterCountSources;
	public com.sun.jna.ptr.PointerByReference m_aeSubscriptionFilterSources;
	public com.sun.jna.NativeLong m_aeSubscriptionCountReturnedAttributes;
	public com.softing.opc.client.impl.jna.OTCAEReturnedAttributesData.ByReference m_aeSubscriptionReturnedAttributes;
	public OTCObjectAttributes() {
		super();
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCObjectAttributes)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCObjectAttributes)clone));
	}
	
	OTCObjectAttributes setupClone (OTCObjectAttributes clone)
	{
		clone.m_daSessionLCID = this.m_daSessionLCID;
		clone.m_daSessionClientName = this.m_daSessionClientName;
		clone.m_daSessionURL = this.m_daSessionURL;
		clone.m_daSessionSupportedOPCSpecification = this.m_daSessionSupportedOPCSpecification;
		clone.m_daSessionForcedOPCSpecification = this.m_daSessionForcedOPCSpecification;
		clone.m_daSessionComClsctx = this.m_daSessionComClsctx;
		clone.m_daSubscriptionName = this.m_daSubscriptionName;
		clone.m_daSubscriptionRequestedUpdateRate = this.m_daSubscriptionRequestedUpdateRate;
		clone.m_daSubscriptionRevisedUpdateRate = this.m_daSubscriptionRevisedUpdateRate;
		
		clone.m_daSubscriptionDeadband = this.m_daSubscriptionDeadband;
		clone.m_daSubscriptionTimeBias = this.m_daSubscriptionTimeBias;
		clone.m_daSubscriptionLCID = this.m_daSubscriptionLCID;
		clone.m_daSubscriptionRequestedKeepAliveTime = this.m_daSubscriptionRequestedKeepAliveTime;
		clone.m_daSubscriptionRevisedKeepAliveTime = this.m_daSubscriptionRevisedKeepAliveTime;
		clone.m_daItemID = this.m_daItemID;
		clone.m_daItemPath = this.m_daItemPath;
		clone.m_daItemNativeDatatype = this.m_daItemNativeDatatype;
		clone.m_daItemRequestedDatatype = this.m_daItemRequestedDatatype;
		clone.m_daItemAccessRights = this.m_daItemAccessRights;
		clone.m_daItemDeadband = this.m_daItemDeadband;
		clone.m_daItemEuType = this.m_daItemEuType;
		clone.m_daItemEuInfo = this.m_daItemEuInfo;
		clone.m_aeSessionLCID = this.m_aeSessionLCID;
		clone.m_aeSessionClientName = this.m_aeSessionClientName;
		clone.m_aeSessionURL = this.m_aeSessionURL;
		clone.m_aeSessionComClsctx = this.m_aeSessionComClsctx;
		clone.m_aeSubscriptionRequestedBufferTime = this.m_aeSubscriptionRequestedBufferTime;
		clone.m_aeSubscriptionRevisedBufferTime = this.m_aeSubscriptionRevisedBufferTime;
		clone.m_aeSubscriptionRequestedMaxSize = this.m_aeSubscriptionRequestedMaxSize;
		clone.m_aeSubscriptionRevisedMaxSize = this.m_aeSubscriptionRevisedMaxSize;
		clone.m_aeSubscriptionFilterSeverityLow = this.m_aeSubscriptionFilterSeverityLow;
		clone.m_aeSubscriptionFilterSeverityHigh = this.m_aeSubscriptionFilterSeverityHigh;
		clone.m_aeSubscriptionFilterEventTypes = this.m_aeSubscriptionFilterEventTypes;
		clone.m_aeSubscriptionFilterCountCategrories = this.m_aeSubscriptionFilterCountCategrories;
		clone.m_aeSubscriptionFilterCategrories = this.m_aeSubscriptionFilterCategrories;
		clone.m_aeSubscriptionFilterCountAreas = this.m_aeSubscriptionFilterCountAreas;
		clone.m_aeSubscriptionFilterAreas = this.m_aeSubscriptionFilterAreas;
		clone.m_aeSubscriptionFilterCountSources = this.m_aeSubscriptionFilterCountSources;
		clone.m_aeSubscriptionFilterSources = this.m_aeSubscriptionFilterSources;
		clone.m_aeSubscriptionCountReturnedAttributes = this.m_aeSubscriptionCountReturnedAttributes;
		clone.m_aeSubscriptionReturnedAttributes = this.m_aeSubscriptionReturnedAttributes;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCObjectAttributes clone() { return setupClone(new OTCObjectAttributes()); }
	public static class ByReference extends OTCObjectAttributes implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCObjectAttributes implements com.sun.jna.Structure.ByValue {}
}

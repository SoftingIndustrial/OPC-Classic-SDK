package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCServerStatus extends Structure {
	/// COM + XML
	public com.sun.jna.NativeLong m_state;
	/// COM + XML
	public com.softing.opc.client.impl.jna.FILETIME m_startTime;
	/// COM + XML
	public com.softing.opc.client.impl.jna.FILETIME m_currentTime;
	/// COM + XML
	//public com.sun.jna.ptr.ByteByReference m_vendorInfo;
	public com.sun.jna.Pointer m_vendorInfo; //UNICODE
	/// COM + XML
	//public com.sun.jna.ptr.ByteByReference m_productVersion;
	public com.sun.jna.Pointer m_productVersion; //UNICODE
	public com.sun.jna.NativeLong m_countLCIDs;
	/// COM + XML
	//TOS
	//changed from com.sun.jna.ptr.NativeLongByReference
	//to com.sun.jna.ptr.NativeLong[]
	public com.sun.jna.ptr.NativeLongByReference m_supportedLCIDs;
	/// COM
	public com.softing.opc.client.impl.jna.FILETIME m_lastUpdateTime;
	/// COM
	public com.sun.jna.NativeLong m_groupCount;
	/// COM
	public com.sun.jna.NativeLong m_bandwidth;
	/// XML
	//public com.sun.jna.ptr.ByteByReference m_statusInfo;
	public com.sun.jna.Pointer m_statusInfo; //UNICODE
	
	public OTCServerStatus() {
		super();
	}
	
	ByReference setupClone (ByReference clone)
	{
		((OTCServerStatus)clone).m_state = this.m_state;
		((OTCServerStatus)clone).m_startTime = this.m_startTime;
		((OTCServerStatus)clone).m_currentTime = this.m_currentTime;
		((OTCServerStatus)clone).m_vendorInfo = this.m_vendorInfo;
		((OTCServerStatus)clone).m_productVersion = this.m_productVersion;
		((OTCServerStatus)clone).m_countLCIDs = this.m_countLCIDs;
		((OTCServerStatus)clone).m_supportedLCIDs = this.m_supportedLCIDs;
		((OTCServerStatus)clone).m_lastUpdateTime = this.m_lastUpdateTime;
		((OTCServerStatus)clone).m_groupCount = this.m_groupCount;
		((OTCServerStatus)clone).m_bandwidth = this.m_bandwidth;
		((OTCServerStatus)clone).m_statusInfo = this.m_statusInfo;
		return clone;
	}
	
	ByValue setupClone (ByValue clone)
	{
		((OTCServerStatus)clone).m_state = this.m_state;
		((OTCServerStatus)clone).m_startTime = this.m_startTime;
		((OTCServerStatus)clone).m_currentTime = this.m_currentTime;
		((OTCServerStatus)clone).m_vendorInfo = this.m_vendorInfo;
		((OTCServerStatus)clone).m_productVersion = this.m_productVersion;
		((OTCServerStatus)clone).m_countLCIDs = this.m_countLCIDs;
		((OTCServerStatus)clone).m_supportedLCIDs = this.m_supportedLCIDs;
		((OTCServerStatus)clone).m_lastUpdateTime = this.m_lastUpdateTime;
		((OTCServerStatus)clone).m_groupCount = this.m_groupCount;
		((OTCServerStatus)clone).m_bandwidth = this.m_bandwidth;
		((OTCServerStatus)clone).m_statusInfo = this.m_statusInfo;
		return clone;
	}
	
	OTCServerStatus setupClone (OTCServerStatus clone)
	{
		((OTCServerStatus)clone).m_state = this.m_state;
		((OTCServerStatus)clone).m_startTime = this.m_startTime;
		((OTCServerStatus)clone).m_currentTime = this.m_currentTime;
		((OTCServerStatus)clone).m_vendorInfo = this.m_vendorInfo;
		((OTCServerStatus)clone).m_productVersion = this.m_productVersion;
		((OTCServerStatus)clone).m_countLCIDs = this.m_countLCIDs;
		((OTCServerStatus)clone).m_supportedLCIDs = this.m_supportedLCIDs;
		((OTCServerStatus)clone).m_lastUpdateTime = this.m_lastUpdateTime;
		((OTCServerStatus)clone).m_groupCount = this.m_groupCount;
		((OTCServerStatus)clone).m_bandwidth = this.m_bandwidth;
		((OTCServerStatus)clone).m_statusInfo = this.m_statusInfo;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCServerStatus clone() { return setupClone(new OTCServerStatus()); }
	public static class ByReference extends OTCServerStatus implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCServerStatus implements com.sun.jna.Structure.ByValue {}
}

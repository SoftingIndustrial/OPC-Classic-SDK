package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

public class OTCCallbackFunctions extends Structure {
	/// callbacks
	public com.softing.opc.client.impl.jna.OTClient.OTOnTrace m_OTOnTrace;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnStateChange m_OTCOnStateChange;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnDataChange m_OTCOnDataChange;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnReadComplete m_OTCOnReadComplete;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnWriteComplete m_OTCOnWriteComplete;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnPerfromStateTransition m_OTCOnPerfromStateTransition;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnServerShutdown m_OTCOnServerShutdown;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnUpdateAttributes m_OTCOnUpdateAttributes;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnUpdateDAItemAttributes m_OTCOnUpdateDAItemAttributes;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnValidateDAItems m_OTCOnValidateDAItems;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnGetServerStatus m_OTCOnGetServerStatus;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnBrowseAddressSpace m_OTCOnBrowseAddressSpace;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnGetDAProperties m_OTCOnGetDAProperties;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnBrowseServer m_OTCOnBrowseServer;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnReceivedEvents m_OTCOnReceivedEvents;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnRefreshAEConditions m_OTCOnRefreshAEConditions;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnAcknowledgeAEConditions m_OTCOnAcknowledgeAEConditions;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnQueryAvailableAEFilters m_OTCOnQueryAvailableAEFilters;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnQueryAECategories m_OTCOnQueryAECategories;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnQueryAEAttributes m_OTCOnQueryAEAttributes;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnQueryAEConditions m_OTCOnQueryAEConditions;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnQueryAESubConditions m_OTCOnQueryAESubConditions;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnQueryAESourceConditions m_OTCOnQueryAESourceConditions;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnGetAEConditionState m_OTCOnGetAEConditionState;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnEnableAEConditions m_OTCOnEnableAEConditions;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnGetErrorString m_OTCOnGetErrorString;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnLogon m_OTCOnLogon;
	public com.softing.opc.client.impl.jna.OTClient.OTCOnLogoff m_OTCOnLogoff;
	public OTCCallbackFunctions() {
		super();
	}
	
	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((OTCCallbackFunctions)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((OTCCallbackFunctions)clone));
	}
	
	OTCCallbackFunctions setupClone (OTCCallbackFunctions clone)
	{
		clone.m_OTOnTrace = this.m_OTOnTrace;
		clone.m_OTCOnStateChange = this.m_OTCOnStateChange;
		clone.m_OTCOnDataChange = this.m_OTCOnDataChange;
		clone.m_OTCOnReadComplete = this.m_OTCOnReadComplete;
		clone.m_OTCOnWriteComplete = this.m_OTCOnWriteComplete;
		clone.m_OTCOnPerfromStateTransition = this.m_OTCOnPerfromStateTransition;
		clone.m_OTCOnServerShutdown = this.m_OTCOnServerShutdown;
		clone.m_OTCOnUpdateAttributes = this.m_OTCOnUpdateAttributes;
		clone.m_OTCOnUpdateDAItemAttributes = this.m_OTCOnUpdateDAItemAttributes;
		clone.m_OTCOnValidateDAItems = this.m_OTCOnValidateDAItems;
		clone.m_OTCOnGetServerStatus = this.m_OTCOnGetServerStatus;
		clone.m_OTCOnBrowseAddressSpace = this.m_OTCOnBrowseAddressSpace;
		clone.m_OTCOnGetDAProperties = this.m_OTCOnGetDAProperties;
		clone.m_OTCOnBrowseServer = this.m_OTCOnBrowseServer;
		clone.m_OTCOnReceivedEvents = this.m_OTCOnReceivedEvents;
		clone.m_OTCOnRefreshAEConditions = this.m_OTCOnRefreshAEConditions;
		clone.m_OTCOnAcknowledgeAEConditions = this.m_OTCOnAcknowledgeAEConditions;
		clone.m_OTCOnQueryAvailableAEFilters = this.m_OTCOnQueryAvailableAEFilters;
		clone.m_OTCOnQueryAECategories = this.m_OTCOnQueryAECategories;
		clone.m_OTCOnQueryAEAttributes = this.m_OTCOnQueryAEAttributes;
		clone.m_OTCOnQueryAEConditions = this.m_OTCOnQueryAEConditions;
		clone.m_OTCOnQueryAESubConditions = this.m_OTCOnQueryAESubConditions;
		clone.m_OTCOnQueryAESourceConditions = this.m_OTCOnQueryAESourceConditions;
		clone.m_OTCOnGetAEConditionState = this.m_OTCOnGetAEConditionState;
		clone.m_OTCOnEnableAEConditions = this.m_OTCOnEnableAEConditions;
		clone.m_OTCOnGetErrorString = this.m_OTCOnGetErrorString;
		clone.m_OTCOnLogon = this.m_OTCOnLogon;
		clone.m_OTCOnLogoff = this.m_OTCOnLogoff;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public OTCCallbackFunctions clone() { return setupClone(new OTCCallbackFunctions()); }
	public static class ByReference extends OTCCallbackFunctions implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends OTCCallbackFunctions implements com.sun.jna.Structure.ByValue {}
}

package com.softing.opc.client.enums;

import com.softing.opc.client.impl.jna.OTClient;

//see enums.cs

public enum EnumFeature {

	AE_CLIENT (OTClient.OTC_FEATURE_AE_CLIENT),
	DA_CLIENT (OTClient.OTC_FEATURE_DA_CLIENT),
	TP_CLIENT (OTClient.OTC_FEATURE_TP_CLIENT),
	XMLDA_CLIENT (OTClient.OTC_FEATURE_XMLDA_CLIENT);
	
	private final short feature;
	
	EnumFeature(int value)
	{
		feature = (short)value;
	}
	
	public short toShort()
	{
		return feature;
	}
	
}

package com.softing.opc.client.api;


import java.util.*;
import java.text.*;

import com.softing.opc.client.enums.EnumQuality;
import com.softing.opc.client.impl.jna.OTClient;
import com.softing.opc.client.impl.jna.OTValueData;
import com.softing.opc.client.impl.jni.JNIOTValueData;
import com.sun.jna.Pointer;


public class ValueQT extends ValueData {
	protected EnumQuality quality_ = EnumQuality.BAD_WAITING_FOR_INITIAL_DATA;
	protected Date timeStamp_ = null;

	public static final int VARIANT_SIZE = 0x10;

	public ValueQT(Object aData, EnumQuality aQuality, Date aTimeStamp){
		super(aData);
		setTimestamp();
		quality_ = aQuality;
		timeStamp_ = aTimeStamp;
	}
	
	public ValueQT(Object aData){
		super(aData);
		quality_ = EnumQuality.QUALITY_NOT_SET;
		setTimestamp();
	}
	
	public ValueQT(OTValueData anOtValueData){
		super(OTClient.INVALID_POINTER_VARIANT.equals(anOtValueData.m_pValue) || anOtValueData.m_pValue.getPointer().equals(Pointer.NULL) ? null : anOtValueData.m_pValue.getObject());
		setTimestamp();
		quality_ = EnumQuality.getEnum(anOtValueData.m_quality);
		timeStamp_ = anOtValueData.m_timestamp.toDate();	
	}

	public ValueQT(){
		setTimestamp();
	}
	
	private void setTimestamp()
	{
		Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("UTC"));
        calendar.set(1601, Calendar.JANUARY, 1, 0, 0, 0);
        calendar.set(Calendar.MILLISECOND, 0);
		timeStamp_ = calendar.getTime();
	}

	public EnumQuality getQuality(){
		return quality_;
	}

	public Date getTimeStamp(){
		return timeStamp_;
	}

	public void setData(Object aData, EnumQuality aQuality, Date aTimeStamp){
		data_ = aData;
		quality_ = aQuality;
		timeStamp_ = aTimeStamp;
	}
	
	public void setData(OTValueData anOtValueData)
	{
		super.data_ =  anOtValueData.m_pValue.getObject();
		setTimestamp();
		quality_ = EnumQuality.getEnum(anOtValueData.m_quality);
		timeStamp_ = anOtValueData.m_timestamp.toDate();
	}

	public void setData(ValueQT anOtValueQT)
	{
		super.data_ =  anOtValueQT.data_;
		setTimestamp();
		quality_ = anOtValueQT.quality_;
		timeStamp_ = anOtValueQT.timeStamp_;
	}
	
	public void setData(JNIOTValueData anOtValueData)
	{
		super.data_ =  anOtValueData.m_pValue;
		setTimestamp();
		quality_ = EnumQuality.getEnum(anOtValueData.m_quality);
		timeStamp_ = anOtValueData.m_timestamp.toDate();
	}
	
	public Object getData()
	{
		return data_;
	}
	
	public boolean isQualitySet(){
		return quality_ != EnumQuality.BAD_WAITING_FOR_INITIAL_DATA;
	}

	public boolean isTimeStampSet(){
		Calendar cal = Calendar.getInstance(TimeZone.getTimeZone("UTC"));
		cal.setTime(timeStamp_);
		return ((cal.get(Calendar.HOUR) != 0 ) && (cal.get(Calendar.MINUTE) != 0) && (cal.get(Calendar.SECOND) != 0)&& (cal.get(Calendar.MILLISECOND)) != 0);
	}

	public String toString(){
		SimpleDateFormat formatter = new SimpleDateFormat ("dd.MM.yyyy hh:mm:ss.SSS");

		return String.format("%s (Q:%s T:%s)", super.toString(), quality_, formatter.format(timeStamp_));           
	}

}
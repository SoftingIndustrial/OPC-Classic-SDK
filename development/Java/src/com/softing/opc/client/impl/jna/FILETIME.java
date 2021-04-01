package com.softing.opc.client.impl.jna;

import com.sun.jna.*;

import java.util.*;


public class FILETIME extends Structure {
	
	
	//public long lDatetime;
	public int dwLowDateTime;
	public int dwHighDateTime;
	
	public FILETIME() {
		super();
	}
	
	public FILETIME(Pointer p)
	{
		super();
		this.useMemory(p);
		this.read();
	}
	
	private long calculateCLongValue()
	{
		long lResult = 0L;
		long lLow = checkOverflown((long)dwLowDateTime);
		long lHigh = checkOverflown((long)dwHighDateTime);
		lResult = (lHigh << 32) | lLow;
		
		return lResult;
	}
	
	private long checkOverflown(long value)
	{
		long intMinValue = Integer.MIN_VALUE;
		long result = value;
		if(result < 0)
		{
			result = intMinValue *(-2) + value;
		}
		return result;
	}
	
	public FILETIME(int dwLowDateTime, int dwHighDateTime) {
		super();
		this.dwLowDateTime = dwLowDateTime;
		this.dwHighDateTime = dwHighDateTime;
		
	}
	
	public FILETIME(long cLongDateTime) {
		super();	
		
		this.dwLowDateTime = (int) cLongDateTime & 0x00000000ffffffff;
		this.dwHighDateTime = (int)((cLongDateTime >> 32)& 0x00000000ffffffff);
		//lDatetime = cLongDateTime;
		
		
	}
	
	public static Date TimeValToDate(double d)
	{
		double tmp = d;
		
		int days = (int)d;
		tmp = (tmp - days)*24;
		int hours = (int)tmp;
		tmp = (tmp - hours)*60;
		int minutes = (int)tmp;
		tmp = (tmp - minutes)*60;
		int seconds = (int)tmp;
		tmp = (tmp - seconds)*1000;
		int millis = (int)tmp;
		 
		
		Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("GMT"));
        calendar.set(1899, Calendar.DECEMBER, 30, 0, 0, 0);
        calendar.set(Calendar.MILLISECOND, 0);
        calendar.add(Calendar.DATE, (int)days);
        calendar.add(Calendar.HOUR_OF_DAY, (int)hours);
        calendar.add(Calendar.MINUTE, (int)minutes);
        calendar.add(Calendar.SECOND, (int)seconds);
        calendar.add(Calendar.MILLISECOND, (int)millis);
		
        return calendar.getTime();
	}
	

	public FILETIME(Date dt)
	{
		Calendar cal = Calendar.getInstance();
		cal.setTime(dt);
		Calendar cal1601 = getGMTCalendar();
		cal1601.set(1601, 0, 1, 0, 0, 0);
		cal1601.set(Calendar.MILLISECOND, 0);
		long milliseconds = cal.getTimeInMillis() - cal1601.getTimeInMillis();
		long numberOf100NanosecondIntervals = 10000 * milliseconds;
		long lDatetime = numberOf100NanosecondIntervals;
		
		this.dwLowDateTime = (int) lDatetime & 0x00000000ffffffff;
		this.dwHighDateTime = (int)((lDatetime >> 32)& 0x00000000ffffffff);
	}

	ByReference setupClone (ByReference clone)
	{
		return (ByReference)(this.setupClone((FILETIME)clone));
	}
	
	ByValue setupClone (ByValue clone)
	{
		return (ByValue)(this.setupClone((FILETIME)clone));
	}
	
	FILETIME setupClone (FILETIME clone)
	{
		clone.dwHighDateTime = this.dwHighDateTime;
		clone.dwLowDateTime = this.dwLowDateTime;
		return clone;
	}
	
	public ByReference byReference() { return setupClone(new ByReference()); }
	public ByValue byValue() { return setupClone(new ByValue()); }
	public FILETIME clone() { return setupClone(new FILETIME()); }
	public static class ByReference extends FILETIME implements com.sun.jna.Structure.ByReference {}
	public static class ByValue extends FILETIME implements com.sun.jna.Structure.ByValue {}
	
    private Calendar getGMTCalendar()
	{
    	return Calendar.getInstance(TimeZone.getTimeZone("UTC"));
	}
    
    protected long getCLongDate()
    {		
		return calculateCLongValue();
    }
    
    public static Date convertToDate(FILETIME ft)
    {
    	return ft.toDate();
    }

    public Date toDate()
	{		
		final long MILLISECONDS_IN_SECOND = 1000;
		final long MILLISECONDS_IN_MINUTE = 60 * MILLISECONDS_IN_SECOND;
		final long MILLISECONDS_IN_HOUR = 60 * MILLISECONDS_IN_MINUTE;
		final long MILLISECONDS_IN_DAY = 24 * MILLISECONDS_IN_HOUR;
		
	    long numberOf100NanosecondIntervals = calculateCLongValue();
	 
	    long milliseconds = numberOf100NanosecondIntervals / 10000;
	 
        long days = milliseconds / MILLISECONDS_IN_DAY;
        milliseconds -= days * MILLISECONDS_IN_DAY;
        long hours = milliseconds / MILLISECONDS_IN_HOUR;
        milliseconds -= hours * MILLISECONDS_IN_HOUR;
        long minutes = milliseconds / MILLISECONDS_IN_MINUTE;
        milliseconds -= minutes * MILLISECONDS_IN_MINUTE;
        long seconds = milliseconds / MILLISECONDS_IN_SECOND;
        milliseconds -= seconds * MILLISECONDS_IN_SECOND;
        // since January 1, 1601
        Calendar calendar = getGMTCalendar();
        calendar.set(1601, Calendar.JANUARY, 1, 0, 0, 0);
        calendar.set(Calendar.MILLISECOND, 0);
        calendar.add(Calendar.DATE, (int)days);
        calendar.add(Calendar.HOUR_OF_DAY, (int)hours);
        calendar.add(Calendar.MINUTE, (int)minutes);
        calendar.add(Calendar.SECOND, (int)seconds);
        calendar.add(Calendar.MILLISECOND, (int)milliseconds);
 
        return calendar.getTime();
	 }

}
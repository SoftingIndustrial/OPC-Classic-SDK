package com.softing.opc.client.impl.jni;

import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

public class JNIFILETIME {

	public int dwLowDateTime;
	public int dwHighDateTime;
	public JNIFILETIME() {
		this.dwLowDateTime = 0;
		this.dwHighDateTime = 0;
	}
	
	public JNIFILETIME(int dwLowDateTime, int dwHighDateTime) {
		this.dwLowDateTime = dwLowDateTime;
		this.dwHighDateTime = dwHighDateTime;
	}
	
	public JNIFILETIME(Date dt)
	{
		Calendar cal = Calendar.getInstance();
		cal.setTime(dt);
		Calendar cal1601 = getGMTCalendar();
		cal1601.set(1601, Calendar.JANUARY, 1, 0, 0, 0);
        cal1601.set(Calendar.MILLISECOND, 0);
		long milliseconds = cal.getTimeInMillis() - cal1601.getTimeInMillis();
		long numberOf100NanosecondIntervals = 10000 * milliseconds;
		long lDatetime = numberOf100NanosecondIntervals;
		
		this.dwLowDateTime = (int) lDatetime & 0x00000000ffffffff;
		this.dwHighDateTime = (int)((lDatetime >> 32)& 0x00000000ffffffff);
	}
	
    private Calendar getGMTCalendar()
	{
    	return Calendar.getInstance(TimeZone.getTimeZone("UTC"));
	}
    
    protected long getCLongDate()
    {		
		return calculateCLongValue();
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
	
	public static Date filetimeToDate(JNIFILETIME ft)
	{
		return ft.toDate();
	}
	
	public static Date filetimeToDate(int dwLowDateTime, int dwHighDateTime)
	{
		JNIFILETIME ft = new JNIFILETIME(dwLowDateTime, dwHighDateTime);
		return ft.toDate();
	}
	
	
	public Date toDate()
	{		
		//Added by TOS
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

package com.softing.opc.client.enums;

//see enums.cs
public enum EnumPropertyId {

	ITEM_CANONICAL_DATATYPE (0x0001),
	ITEM_VALUE (0x0002),
	ITEM_QUALITY (0x0003),
	ITEM_TIMESTAMP (0x0004),
	
	ITEM_ACCESS_RIGHTS (0x0005),
	SERVER_SCAN_RATE (0x0006),
	ITEM_EU_TYPE (0x0007),
	ITEM_EU_INFO (0x0008),
	
	RESERVED_RANGE_1 (0x0009),
	EU_UNITS (0x0064),
	ITEM_DESCRIPTION (0x0065),
	HIGH_EU (0x0066),
	
	LOW_EU (0x0067),
	HIGH_INSTRUMENT_RANGE (0x0068),
	LOW_INSTRUMENT_RANGE (0x0069),
	CONTACT_CLOSE_LABEL (0x006A),
	
	CONTACT_OPEN_LABEL (0x006B),
	ITEM_TIMEZONE (0x006C),
	RESERVED_RANGE_2 (0x006D),
	CONDITION_STATUS (0x012C),
	
	ALARM_QUICK_HELP (0x012D),
	ALARM_AREA_LIST (0x012E),
	PRIMARY_ALARM_AREA (0x012F),
	CONDITION_LOGIC (0x0130),
	
	LIMIT_EXCEEDED (0x0131),
	DEADBAND (0x0132),
	HIHI_LIMIT (0x0133),
	HI_LIMIT (0x0134),
	
	LO_LIMIT (0x0135),
	LOLO_LIMIT (0x0136),
	RATE_OF_CHANGE_LIMIT (0x0137),
	DEVIATION_LIMIT (0x0138),
	
	SOUND_FILE (0x0139),
	RESERVED_RANGE_3 (0x0140),
	RESERVED_RANGE_4 (0x0190);
	
	private final short propertyId;
	
	EnumPropertyId(int value)
	{
		propertyId = (short)value;
	}
	
	public short toShort()
	{
		return propertyId;
	}
	
}

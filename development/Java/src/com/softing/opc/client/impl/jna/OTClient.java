package com.softing.opc.client.impl.jna;

import com.sun.jna.*;
import com.sun.jna.win32.*;

public interface OTClient extends StdCallLibrary {
//public interface OTClient extends Library {
	public static final com.softing.opc.client.impl.jna.OTClient INSTANCE = (com.softing.opc.client.impl.jna.OTClient)com.sun.jna.Native.loadLibrary("OTBu", com.softing.opc.client.impl.jna.OTClient.class);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_ACCESSRIGHTS_WRITEABLE = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_COMM_FAILURE = 24;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_S_OPC_INUSE = 262159;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_LIMIT_CONST = 3;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_FILTER_BY_AREA = 8;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_BADRIGHTS = 0xc0040006;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DAITEM_DEADBAND = (536870912 | 2);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_GOOD = 192;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_RANGE = 0xc004000b;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_S_OPC_DATAQUEUEOVERFLOW = 263172;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_OTSIN = 0x10000;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASUBSCRIPTION_NAME = (1073741824 | 1);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_UNKNOWNPATH = 0xc004000a;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_SERVERDATA_ALL = 255;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONCHANGE_SEVERITY = 16;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_OTBLICENSE = 0x200000;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASESSION_FORCEDOPCSPECIFICATION = (2147483648L | 8);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_WRONG_VERSION = -839974911;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_EVENTTYPE_TRACKING = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_OPCSPECIFICATION_DA10 = 3;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_FILTER_BY_EVENT = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_FEATURE_DA_CLIENT = 16;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_EXECUTIONTYPE_ASYCHRONOUS = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACEDATA_FILE_ENABLE = 16;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONCHANGE_ATTRIBUTE = 128;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_LIMIT_HIGH = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_EXECUTIONTYPE_SYCHRONOUS = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_AESUBSCRIPTION_RETURNED_ATTRIBUTES = (67108864 | 8);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_NOTLICENSED = -839974902;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_LIMIT_OK = 0;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_OPCSPECIFICATION_DA20 = 4;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_LAST_KNOWN = 20;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_CONFIG_ERROR = 4;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_OTSOUT = 0x20000;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_AESUBSCRIPTION_BUFFERTIME = (67108864 | 1);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONSTATE_ACKED = 4;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_AESESSION_LCID = (134217728 | 2);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONCHANGE_QUALITY = 8;
	/// <i>native declaration : OTClient.h</i>
	public static final int S_FALSE = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_NOT_CONNECTED = 8;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_INVALIDOBJECTTYPE = -839974910;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_TBSERVER = 0x400000;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_EVENTTYPE_CONDITION = 4;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACELEVEL_WRN = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_UNKNOWNITEMID = 0xc0040007;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_ADDRESSSPACEELEMENT_TYPE_AE = 2;
	/**
	 * define<br>
	 * SKIPPED:<br>
	 * <i>native declaration : OTClient.h</i><br>
	 * OTVariant
	 */
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_SOPC = 3;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_AESESSION_COM_CLSCTX = (134217728 | 32768);
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_COM_CLSCTX_NOT_COM = 0;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_COM_CLSCTX_INPROC_SERVER = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_FEATURE_XMLDA_CLIENT = 32;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_SERVERDATA_VERSIONINDEPENDENTPROGID = 4;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_INVALIDCONTINUATIONPOINT = 0xc0040403;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_WAITING_FOR_INITIAL_DATA = 32;
	/**
	 * define<br>
	 * SKIPPED:<br>
	 * <i>native declaration : OTClient.h</i><br>
	 * OTObjectData
	 */
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACELEVEL_DEB = 3;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_DAITEM_EUTYPE_ENUMERATED = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_COM_CLSCTX_REMOTE_SERVER = 16;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DAITEM_REQUESTED_DATATYPE = (536870912 | 1);
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_USER1 = 0x80000000;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_USER2 = 0x40000000;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_FEATURE_TP_CLIENT = 128;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_INVALIDFEATURE = -839974905;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_NOTSUPPORTED = 0xc0040406;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_FEATURE_AE_CLIENT = 64;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASUBSCRIPTION_KEEPALIVETIME = (1073741824 | 32);
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_SERVERDATA_CLSID = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_DUPLICATENAME = 0xc004000c;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_AESUBSCRIPTION_FILTER = (67108864 | 4);
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DAITEM_ACCESSRIGHTS = (536870912 | 8);
	/// <i>native declaration : OTClient.h</i>
	public static final int E_NOTIMPL = 0x80004001;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DAITEM_PATH = (536870912 | 128);
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DAITEM_NATIVE_DATATYPE = (536870912 | 4);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_ACCESSRIGHTS_READABLE = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_SERVERDATA_PROGID = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_SERVERDATA_DESCRIPTION = 8;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_OPCSPECIFICATION_DEFAULT = 0;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_DUPLICATENAME = -839974907;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_DAPROPERTYDATA_ITEMID = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_DEVICE_FAILURE = 12;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACEDATA_LEVEL_DEB = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_OPCSPECIFICATION_ALL = 255;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_STATE_ACTIVE = 3;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONCHANGE_ACK_DATA = 32768;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_INVALIDDATA = -839974908;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_STATE_DISCONNECTED = 0;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_TBCLIENT = 0x800000;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_USER = 0xff000000;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACEDATA_LEVEL = 15;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_ALL = 0xff000000;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONCHANGE_MESSAGE = 64;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_OUT_OF_SERVICE = 28;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_S_OPC_UNSUPPORTEDRATE = 262157;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_DEADBANDNOTSUPPORTED = 0xc0040401;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONCHANGE_SUBCONDITION = 32;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACEDATA_ALL = 255;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_CANTEXECUTEASYNC = -839974904;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_EGU_EXCEEDED = 84;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONCHANGE_ACTIVE_STATE = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASUBSCRIPTION_UPDATERATE = (1073741824 | 2);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_LAST_USABLE = 68;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_OPCSPECIFICATION_DA30 = 16;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_SOPCADVISE = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_DAITEM_EUTYPE_NO = 0;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_COPC = 12;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONCHANGE_ENABLE_STATE = 4;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_LIMIT_LOW = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_OBJECTTYPE_AESESSION = 134217728;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASESSION_LCID = (2147483648L | 2);
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_USER8 = 0xff000000;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_USER7 = 0x2000000;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONSTATE_ENABLED = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_USER6 = 0x2000000;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_USER5 = 0x8000000;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_USER4 = 0x10000000;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_USER3 = 0x10000000;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_EVENTTYPE_SIMPLE = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_OBJECTTYPE_DASESSION = 2147483648L;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACEDATA_MAX_BACKUPS = 64;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_COPCCALL = 4;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_INVALIDFILTER = 0xc0040009;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_INVALIDLICENSE = -839974906;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASESSION_ALL = (2147483648L | 65535);
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASUBSCRIPTION_LCID = (1073741824 | 16);
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_AESESSION_URL = (134217728 | 4);
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_AESESSION_ALL = (134217728 | 65535);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACELEVEL_ERR = 0;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DAITEM_EUTYPE = (536870912 | 16);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_RATENOTSET = 0xc0040405;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_MASK_STATUS = 252;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_AESESSION_CLIENTNAME = (134217728 | 1);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_NOT_SET = 65535;
	/**
	 * define<br>
	 * SKIPPED:<br>
	 * <i>native declaration : OTClient.h</i><br>
	 * OTObjectHandle
	 */
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_COM_CLSCTX_LOCAL_SERVER = 4;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DAITEM_ID = (536870912 | 64);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_ADDRESSSPACETYPE_BRANCH = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_DAPROPERTYDATA_ALL = 255;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_SERVERDATA_OPCSPECIFICATIONS = 32;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACEDATA_FILE_NAME = 32;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_OTCOUT = 0x80000;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_NOTFOUND = 0xc0040011;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_SOPCCALL = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_INVALIDCONFIGFILE = 0xc0040010;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_INVALIDHANDLE = 0xcdef0003;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_INVALID_PID = 0xc0040203;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_DEADBANDNOTSET = 0xc0040400;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONCHANGE_ACK_STATE = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DAITEM_ALL = (536870912 | 65535);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_ADDRESSSPACETYPE_ALL = 255;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASESSION_SUPPORTEDOPCSPECIFICATION = (2147483648L | 16);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACEDATA_FILE_SIZE = 128;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACEDATA_LEVEL_INF = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASUBSCRIPTION_DEADBAND = (1073741824 | 4);
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASESSION_URL = (2147483648L | 4);
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DAITEM_EUINFO = (536870912 | 32);
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_COM_CLSCTX_SERVER = (1 | 4 | 16);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_PUBLIC = 0xc0040005;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_OTCIN = 0x40000;
	/// <i>native declaration : OTClient.h</i>
	public static final int E_FAIL = 0x80004005;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_FILTER_BY_CATEGORY = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_ADDRESSSPACETYPE_LEAF = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASESSION_CLIENTNAME = (2147483648L | 1);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_INVALIDADDRESSSPACEELEMENTTYPE = -839974903;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASUBSCRIPTION_TIMEBIAS = (1073741824 | 8);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_SUB_NORMAL = 88;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_OPCSPECIFICATION_AE10 = 8;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONSTATE_ACTIVE = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACEDATA_LEVEL_WRN = 4;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_OBJECTTYPE_AESUBSCRIPTION = 67108864;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACELEVEL_INF = 2;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_ADDRESSSPACETYPE_ITEM = 4;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_COPCADVISE = 8;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_SENSOR_CAL = 80;
	/**
	 * define<br>
	 * SKIPPED:<br>
	 * <i>native declaration : OTClient.h</i><br>
	 * OTAPI_CALL
	 */
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_BAD = 0;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_INVALIDHANDLE = 0xc0040001;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_UNCERTAIN = 64;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_SENSOR_FAILURE = 16;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_MASK_LIMIT = 3;
	/**
	 * define<br>
	 * SKIPPED:<br>
	 * <i>native declaration : OTClient.h</i><br>
	 * OTDateTime
	 */
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_FILTER_BY_SEVERITY = 4;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_OBJECTTYPE_DASUBSCRIPTION = 1073741824;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_AESUBSCRIPTION_MAXSIZE = (67108864 | 2);
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_AESUBSCRIPTION_ALL = (67108864 | 65535);
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_STATE_CONNECTED = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACEDATA_LEVEL_ERR = 8;
	/// <i>native declaration : OTClient.h</i>
	public static final long OT_TRACEMASK_NOTHING = 0;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_OPCSPECIFICATION_XMLDA10 = 32;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_BADTYPE = 0xc0040004;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_CONDITIONCHANGE_STATE = 7;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_TRACEDATA_FILE = 240;
	/// <i>native declaration : OTClient.h</i>
	public static final int S_OK = 0;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_S_OPC_CLAMP = 262158;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_LOCAL_OVERRIDE = 216;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_INVALIDITEMID = 0xc0040008;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_E_OPC_NOBUFFERING = 0xc0040402;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_QUALITY_MASK_QUALITY = 192;
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_ADDRESSSPACEELEMENT_TYPE_DA = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_SERVERDATA_URL = 16;
	/**
	 * define<br>
	 * SKIPPED:<br>
	 * <i>native declaration : OTClient.h</i><br>
	 * OTValueData
	 */
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_DAITEM_EUTYPE_ANALOG = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_DAPROPERTYDATA_VALUE = 1;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASUBSCRIPTION_ALL = (1073741824 | 65535);
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_ATTRIBUTE_DASESSION_COM_CLSCTX = (2147483648L | 32768);
	/// <i>native declaration : OTClient.h</i>
	public static final int OT_FILTER_BY_SOURCE = 16;
	/// <i>native declaration : OTClient.h</i>
	public static final int OTC_DAPROPERTYDATA_NONE = 0;
	/// <i>native declaration : OTClient.h</i>
	public static final long OTC_OBJECTTYPE_DAITEM = 536870912;
	/// <i>native declaration : OTClient.h</i>
	public interface OTOnTrace extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.ptr.ByteByReference traceString, short level, com.sun.jna.NativeLong mask, com.sun.jna.ptr.ByteByReference objId, com.sun.jna.ptr.ByteByReference text);
	}
	/// <i>native declaration : OTClient.h</i>
	//public interface OTCOnStateChange extends com.sun.jna.Callback {
	public interface OTCOnStateChange extends com.sun.jna.win32.StdCallLibrary.StdCallCallback{
		void invoke(com.softing.opc.client.impl.jna.OTCObjectContext.ByValue objectContext, byte state);
	}
	/// <i>native declaration : OTClient.h</i>
	//modified signature by TOS
	//from softing.OTObjectData pItemData to softing.OTObjectData.ByReference pItemData
	//from softing.OTValueData pValues to softing.OTValueData.ByReference pValues
	//from com.sun.jna.ptr.NativeLongByReference to com.sun.jna.ptr.NativeLongByReference[]
	public interface OTCOnDataChange extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.softing.opc.client.impl.jna.OTCObjectContext.ByValue objectContext, com.sun.jna.NativeLong count, com.softing.opc.client.impl.jna.OTObjectData.ByReference pItemData, com.softing.opc.client.impl.jna.OTValueData.ByReference pValues, com.sun.jna.ptr.NativeLongByReference pResults);
	}
	/// <i>native declaration : OTClient.h</i>
	//modified signature by TOS
	//from softing.OTObjectData pItemData to softing.OTObjectData.ByReference pItemData
	//from com.sun.jna.ptr.PointerByReference pItemIDs to String[]
	//from com.sun.jna.ptr.PointerByReference pItemPaths to String[]
	//from softing.OTValueData pValues to softing.OTValueData.ByReference pValues
	//from com.sun.jna.ptr.NativeLongByReference pResults to Long[] pResults
	public interface OTCOnReadComplete extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTCObjectContext.ByValue objectContext, com.sun.jna.NativeLong result, com.sun.jna.NativeLong count, com.softing.opc.client.impl.jna.OTObjectData.ByReference pItemData, com.sun.jna.ptr.PointerByReference pItemIDs, com.sun.jna.ptr.PointerByReference pItemPaths, com.softing.opc.client.impl.jna.OTValueData.ByReference pValues, com.sun.jna.ptr.NativeLongByReference pResults);
	}
	/// <i>native declaration : OTClient.h</i>
	//modified signature by TOS
	//from softing.OTObjectData pItemData to softing.OTObjectData[] pItemData
	//from com.sun.jna.ptr.PointerByReference pItemIDs to String[] pItemIDs
	//from com.sun.jna.ptr.PointerByReference pItemPaths to String[] pItemPaths
	//from softing.OTValueData pValues to softing.OTValueData[] pValues
	//from softing.OTValueData pValues to softing.OTValueData.ByReference pValues
	//from com.sun.jna.ptr.NativeLongByReference pResults to Long[] pResults
	public interface OTCOnWriteComplete extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTCObjectContext.ByValue objectContext, com.sun.jna.NativeLong result, com.sun.jna.NativeLong count, com.softing.opc.client.impl.jna.OTObjectData.ByReference pItemData, com.sun.jna.ptr.PointerByReference pItemIDs, com.sun.jna.ptr.PointerByReference pItemPaths, com.softing.opc.client.impl.jna.OTValueData.ByReference pValues, com.sun.jna.ptr.NativeLongByReference pResults);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnPerfromStateTransition extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTCObjectContext.ByValue objectContext, com.sun.jna.NativeLong result);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnServerShutdown extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		byte invoke(com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.ptr.ByteByReference reason);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnUpdateAttributes extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTCObjectContext.ByValue objectContext, com.sun.jna.NativeLong result, byte fromServer, com.sun.jna.NativeLong attributesCount, com.sun.jna.ptr.NativeLongByReference pWhatAttributes, com.sun.jna.ptr.NativeLongByReference pResults);
	}
	/// <i>native declaration : OTClient.h</i>
	//FROM softing.OTObjectData pItemData to softing.OTObjectData.ByReference pItemData
	public interface OTCOnUpdateDAItemAttributes extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTCObjectContext.ByValue objectContext, com.sun.jna.NativeLong result, com.sun.jna.NativeLong itemCount, com.softing.opc.client.impl.jna.OTObjectData.ByReference pItemData, byte fromServer, com.sun.jna.NativeLong attributesCount, com.sun.jna.ptr.NativeLongByReference pWhatAttributes, com.sun.jna.ptr.NativeLongByReference pResults);
	}
	/// <i>native declaration : OTClient.h</i>
	//modified by TOS
	//FROM softing.OTObjectData pItemData to softing.OTObjectData.ByReference pItemData
	public interface OTCOnValidateDAItems extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTCObjectContext.ByValue objectContext, com.sun.jna.NativeLong result, com.sun.jna.NativeLong itemCount, com.softing.opc.client.impl.jna.OTObjectData.ByReference pItemData, com.sun.jna.ptr.NativeLongByReference pResults);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnGetServerStatus extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.softing.opc.client.impl.jna.OTCServerStatus pServerStatus);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnBrowseServer extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.sun.jna.NativeLong result, com.sun.jna.ptr.ByteByReference ipAddress, byte whatOPCSpec, byte whatServerData, com.sun.jna.NativeLong serverDataCount, com.softing.opc.client.impl.jna.OTCServerData pServerData);
	}
	/// <i>native declaration : OTClient.h</i>
	//modified by TOS
	// from softing.OTCAddressSpaceElementData pAddressSpaceElement to softing.OTCAddressSpaceElementData.ByReference pAddressSpaceElement
	public interface OTCOnBrowseAddressSpace extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.sun.jna.NativeLong addressSpaceElementHandle, com.sun.jna.ptr.ByteByReference addressSpaceElementID, com.sun.jna.ptr.ByteByReference addressSpaceElementPath, com.softing.opc.client.impl.jna.OTCAddressSpaceBrowseOptions pBrowseOptions, com.sun.jna.NativeLong addressSpaceElementDataCount, com.softing.opc.client.impl.jna.OTCAddressSpaceElementData.ByReference pAddressSpaceElement);
	}
	/// <i>native declaration : OTClient.h</i>
	//MODIFIED BY TOS
	//FROM softing.OTCDAPropertyData pPropertyData to softing.OTCDAPropertyData.ByReference pPropertyData
	public interface OTCOnGetDAProperties extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.sun.jna.NativeLong addressSpaceElementHandle, com.sun.jna.ptr.ByteByReference addressSpaceElementID, com.sun.jna.ptr.ByteByReference addressSpaceElementPath, com.softing.opc.client.impl.jna.OTCGetDAPropertiesOptions pGetPropertiesOptions, com.sun.jna.NativeLong propertyDataCount, com.softing.opc.client.impl.jna.OTCDAPropertyData.ByReference pPropertyData);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnReceivedEvents extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.softing.opc.client.impl.jna.OTCObjectContext.ByValue objectContext, com.sun.jna.NativeLong count, com.softing.opc.client.impl.jna.OTCEventData pEvents, byte refresh, byte lastRefresh);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnRefreshAEConditions extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTCObjectContext.ByValue objectContext, com.sun.jna.NativeLong result, byte cancelRefresh);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnAcknowledgeAEConditions extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.sun.jna.ptr.ByteByReference ackID, com.sun.jna.ptr.ByteByReference ackComment, com.sun.jna.NativeLong count, com.softing.opc.client.impl.jna.OTCEventData pEvents, com.sun.jna.ptr.NativeLongByReference pResults);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnQueryAvailableAEFilters extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.sun.jna.NativeLong availableFilters);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnQueryAECategories extends com.sun.jna.win32.StdCallLibrary.StdCallCallback{
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.sun.jna.NativeLong count, com.sun.jna.ptr.NativeLongByReference pEventTypes, com.sun.jna.ptr.NativeLongByReference pCategoryIds, com.sun.jna.ptr.PointerByReference pCategoryDescriptions);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnQueryAEAttributes extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.sun.jna.NativeLong categoryId, com.sun.jna.NativeLong count, com.sun.jna.ptr.NativeLongByReference pAttributeIds, com.sun.jna.ptr.PointerByReference pAttributeDescriptions, com.sun.jna.ptr.ShortByReference pAttributeDatatypes);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnQueryAEConditions extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.sun.jna.NativeLong categoryId, com.sun.jna.NativeLong count, com.sun.jna.ptr.PointerByReference pConditionsNames);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnQueryAESubConditions extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.sun.jna.ptr.ByteByReference conditionName, com.sun.jna.NativeLong count, com.sun.jna.ptr.PointerByReference pSubConditionsNames);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnQueryAESourceConditions extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.sun.jna.ptr.ByteByReference sourcePath, com.sun.jna.NativeLong count, com.sun.jna.ptr.PointerByReference pConditionsNames);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnGetAEConditionState extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.sun.jna.ptr.ByteByReference sourcePath, com.sun.jna.ptr.ByteByReference conditionName, com.sun.jna.NativeLong attributeCount, com.sun.jna.ptr.NativeLongByReference pAttributeIds, com.softing.opc.client.impl.jna.OTCAEConditionStateData pConditionState);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnEnableAEConditions extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, byte enable, byte areaOrSource, com.sun.jna.ptr.ByteByReference path);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnGetErrorString extends com.sun.jna.win32.StdCallLibrary.StdCallCallback{
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.sun.jna.NativeLong errorCode, com.sun.jna.ptr.ByteByReference errorString);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnLogon extends com.sun.jna.win32.StdCallLibrary.StdCallCallback {
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result, com.sun.jna.ptr.ByteByReference userName, com.sun.jna.ptr.ByteByReference password);
	}
	/// <i>native declaration : OTClient.h</i>
	public interface OTCOnLogoff extends com.sun.jna.win32.StdCallLibrary.StdCallCallback{
		void invoke(com.sun.jna.NativeLong executionContext, com.softing.opc.client.impl.jna.OTObjectData.ByValue sessionData, com.sun.jna.NativeLong result);
	}
	/**
	 * Original signature : <code>OTActivate(unsigned char, OTChar*, unsigned char)</code><br>
	 * <i>native declaration : OTClient.h:274</i><br>
	 * @deprecated use the safer method {@link #OTActivate(byte, java.nio.ByteBuffer, byte)} instead
	 */
	@java.lang.Deprecated
	int OTActivate(byte feature, com.sun.jna.ptr.ByteByReference key, byte forceDemo);
	/**
	 * Original signature : <code>OTActivate(unsigned char, OTChar*, unsigned char)</code><br>
	 * <i>native declaration : OTClient.h:274</i>
	 */
	//modified by TOS
	//from java.nio.ByteBuffer key to String
	int OTActivate(byte feature, WString key, byte forceDemo);
	/**
	 * Original signature : <code>OTEnableTracing(unsigned long, OTTraceData*)</code><br>
	 * <i>native declaration : OTClient.h:276</i>
	 */
	int OTEnableTracing(com.sun.jna.NativeLong whatData, com.softing.opc.client.impl.jna.OTTraceData pTraceData);
	/**
	 * Original signature : <code>OTTrace(unsigned char, unsigned long, OTChar*, OTChar*)</code><br>
	 * <i>native declaration : OTClient.h:278</i><br>
	 * @deprecated use the safer method {@link #OTTrace(byte, com.sun.jna.NativeLong, java.nio.ByteBuffer, java.nio.ByteBuffer)} instead
	 */
	@java.lang.Deprecated
	int OTTrace(byte traceLevel, com.sun.jna.NativeLong traceMask, com.sun.jna.ptr.ByteByReference objId, com.sun.jna.ptr.ByteByReference text);
	/**
	 * Original signature : <code>OTTrace(unsigned char, unsigned long, OTChar*, OTChar*)</code><br>
	 * <i>native declaration : OTClient.h:278</i>
	 */
	//Modified by TOS
	// from java.nio.ByteBuffer objId TO String objId
	// from java.nio.ByteBuffer text TO String text
	int OTTrace(byte traceLevel, com.sun.jna.NativeLong traceMask, WString objId, WString text);
	/**
	 * Original signature : <code>void* OTAllocateMemory(unsigned long)</code><br>
	 * <i>native declaration : OTClient.h:280</i>
	 */
	com.sun.jna.Pointer OTAllocateMemory(com.sun.jna.NativeLong size);
	/**
	 * Original signature : <code>void OTFreeMemory(void*)</code><br>
	 * <i>native declaration : OTClient.h:282</i>
	 */
	void OTFreeMemory(com.sun.jna.Pointer pMemory);
	/**
	 * Original signature : <code>OTChar* OTVariantToString(OTVariant*, unsigned long, OTChar*)</code><br>
	 * <i>native declaration : OTClient.h:284</i><br>
	 * @deprecated use the safer method {@link #OTVariantToString(com.softing.opc.client.impl.jna.SoftingLibrary.VARIANT, com.sun.jna.NativeLong, java.nio.ByteBuffer)} instead
	 */
	
	@java.lang.Deprecated
	com.sun.jna.ptr.ByteByReference OTVariantToString(com.softing.opc.client.impl.jna.VARIANT pValue, com.sun.jna.NativeLong stringLen, com.sun.jna.ptr.ByteByReference pString);
	/**
	 * Original signature : <code>OTChar* OTVariantToString(OTVariant*, unsigned long, OTChar*)</code><br>
	 * <i>native declaration : OTClient.h:284</i>
	 */

	com.sun.jna.ptr.ByteByReference OTVariantToString(com.softing.opc.client.impl.jna.VARIANT pValue, com.sun.jna.NativeLong stringLen, java.nio.ByteBuffer pString);
	/**
	 * Original signature : <code>OTInitLib()</code><br>
	 * <i>native declaration : OTClient.h:288</i>
	 */
	int OTInitLib();
	/**
	 * Original signature : <code>OTExitLib()</code><br>
	 * <i>native declaration : OTClient.h:290</i>
	 */
	int OTExitLib();
	/**
	 * Original signature : <code>BSTR OTSysAllocString(OTChar*)</code><br>
	 * <i>native declaration : OTClient.h:298</i><br>
	 */
	
	Pointer OTSysAllocString(com.sun.jna.ptr.ByteByReference sz);
	/**
	 * Original signature : <code>BSTR OTSysAllocString(OTChar*)</code><br>
	 * <i>native declaration : OTClient.h:298</i>
	 */
	//added by TOS
	Pointer OTSysAllocString(WString sz);
	
	/**
	 * Original signature : <code>BSTR OTSysAllocString(OTChar*)</code><br>
	 * <i>native declaration : OTClient.h:298</i>
	 * @deprecated use the safer method {@link #OTSysAllocString(java.nio.ByteByReference)} instead
	 */
	@java.lang.Deprecated
	com.sun.jna.ptr.ShortByReference OTSysAllocString(java.nio.ByteBuffer sz);
	/**
	 * Original signature : <code>BSTR OTSysAllocStringLen(OTChar*, unsigned long)</code><br>
	 * <i>native declaration : OTClient.h:299</i><br>
	 * @deprecated use the safer method {@link #OTSysAllocStringLen(java.nio.ByteBuffer, com.sun.jna.NativeLong)} instead
	 */
	@java.lang.Deprecated
	com.sun.jna.ptr.ShortByReference OTSysAllocStringLen(com.sun.jna.ptr.ByteByReference pch, com.sun.jna.NativeLong cch);
	/**
	 * Original signature : <code>BSTR OTSysAllocStringLen(OTChar*, unsigned long)</code><br>
	 * <i>native declaration : OTClient.h:299</i>
	 */

	com.sun.jna.ptr.ShortByReference OTSysAllocStringLen(java.nio.ByteBuffer pch, com.sun.jna.NativeLong cch);
	/**
	 * Original signature : <code>void OTSysFreeString(BSTR)</code><br>
	 * <i>native declaration : OTClient.h:300</i><br>
	 * @deprecated use the safer method {@link #OTSysFreeString(java.nio.ShortBuffer)} instead
	 */
	@java.lang.Deprecated
	void OTSysFreeString(com.sun.jna.ptr.ShortByReference bstr);
	/**
	 * Original signature : <code>void OTSysFreeString(BSTR)</code><br>
	 * <i>native declaration : OTClient.h:300</i>
	 */
	//MODIFIED By tos
	//FROM java.nio.ShortBuffer bstr TO Pointer bstr
	void OTSysFreeString(Pointer bstr);
	/**
	 * Original signature : <code>OTChar* OTSysBstrToString(BSTR)</code><br>
	 * <i>native declaration : OTClient.h:301</i><br>
	 * @deprecated use the safer method {@link #OTSysBstrToString(java.nio.ShortBuffer)} instead
	 */
	@java.lang.Deprecated
	com.sun.jna.ptr.ByteByReference OTSysBstrToString(com.sun.jna.ptr.ShortByReference bstr);
	/**
	 * Original signature : <code>OTChar* OTSysBstrToString(BSTR)</code><br>
	 * <i>native declaration : OTClient.h:301</i>
	 */
	//Modified by tos
	//Returned value from com.sun.jna.ptr.ByteByReference to Pointer
	Pointer OTSysBstrToString(Pointer p);
	/**
	 * Original signature : <code>OTSysStringLen(BSTR)</code><br>
	 * <i>native declaration : OTClient.h:302</i><br>
	 * @deprecated use the safer method {@link #OTSysStringLen(java.nio.ShortBuffer)} instead
	 */
	//@java.lang.Deprecated
	int OTSysStringLen(com.sun.jna.ptr.ShortByReference bstr);
	/**
	 * Original signature : <code>OTSysStringLen(BSTR)</code><br>
	 * <i>native declaration : OTClient.h:302</i>
	 */
	int OTSysStringLen(java.nio.ShortBuffer bstr);
	/**
	 * Original signature : <code>void OTVariantInit(LPVARIANT)</code><br>
	 * <i>native declaration : OTClient.h:303</i>
	 */
	void OTVariantInit(com.softing.opc.client.impl.jna.VARIANT pvarg);
	/**
	 * Original signature : <code>OTVariantClear(LPVARIANT)</code><br>
	 * <i>native declaration : OTClient.h:304</i>
	 */
	int OTVariantClear(com.softing.opc.client.impl.jna.VARIANT pvarg);
	/**
	 * Original signature : <code>OTVariantCopy(LPVARIANT, LPVARIANT)</code><br>
	 * <i>native declaration : OTClient.h:305</i>
	 */
	int OTVariantCopy(com.softing.opc.client.impl.jna.VARIANT pvargDest, com.softing.opc.client.impl.jna.VARIANT pvargSrc);
	/**
	 * Original signature : <code>OTVariantChangeType(LPVARIANT, LPVARIANT, unsigned short, VARTYPE)</code><br>
	 * <i>native declaration : OTClient.h:306</i>
	 */

	int OTVariantChangeType(com.softing.opc.client.impl.jna.VARIANT pvargDest, com.softing.opc.client.impl.jna.VARIANT pvarSrc, short wFlags, short vt);
	/**
	 * Original signature : <code>LPSAFEARRAY OTSafeArrayCreate(VARTYPE, unsigned short, LPSAFEARRAYBOUND)</code><br>
	 * <i>native declaration : OTClient.h:307</i>
	 */
	
	//tos 
	//changed return type from softing.SAFEARRAY to Pointer
	Pointer OTSafeArrayCreate(short vt, short cDims, com.softing.opc.client.impl.jna.SAFEARRAYBOUND rgsabound);
	/**
	 * Original signature : <code>LPSAFEARRAY OTSafeArrayCreateVector(VARTYPE, long, unsigned long)</code><br>
	 * <i>native declaration : OTClient.h:308</i>
	 */

	com.softing.opc.client.impl.jna.SAFEARRAY OTSafeArrayCreateVector(short vt, com.sun.jna.NativeLong lLbound, com.sun.jna.NativeLong cElements);
	/**
	 * Original signature : <code>OTSafeArrayDestroy(LPSAFEARRAY)</code><br>
	 * <i>native declaration : OTClient.h:309</i>
	 */

	int OTSafeArrayDestroy(com.softing.opc.client.impl.jna.SAFEARRAY psa);
	/**
	 * Original signature : <code>OTSafeArrayCopy(LPSAFEARRAY, LPSAFEARRAY*)</code><br>
	 * <i>native declaration : OTClient.h:310</i>
	 */

	int OTSafeArrayCopy(com.softing.opc.client.impl.jna.SAFEARRAY psa, com.sun.jna.ptr.PointerByReference ppsaOut);
	/**
	 * Original signature : <code>OTSafeArrayRedim(SAFEARRAY*, SAFEARRAYBOUND*)</code><br>
	 * <i>native declaration : OTClient.h:311</i>
	 */

	int OTSafeArrayRedim(com.softing.opc.client.impl.jna.SAFEARRAY psa, com.softing.opc.client.impl.jna.SAFEARRAYBOUND psaboundNew);
	/**
	 * Original signature : <code>OTSafeArrayGetDim(LPSAFEARRAY)</code><br>
	 * <i>native declaration : OTClient.h:312</i>
	 */
	NativeLong OTSafeArrayGetDim(com.softing.opc.client.impl.jna.SAFEARRAY psa);
	/**
	 * Original signature : <code>OTSafeArrayGetElemsize(LPSAFEARRAY)</code><br>
	 * <i>native declaration : OTClient.h:313</i>
	 */
	//tos
	//initial return type was int
	NativeLong OTSafeArrayGetElemsize(com.softing.opc.client.impl.jna.SAFEARRAY psa);
	/**
	 * Original signature : <code>OTSafeArrayGetUBound(LPSAFEARRAY, unsigned short, long*)</code><br>
	 * <i>native declaration : OTClient.h:314</i>
	 */
	
	NativeLong OTSafeArrayGetUBound(com.softing.opc.client.impl.jna.SAFEARRAY psa, short nDim, com.sun.jna.ptr.NativeLongByReference plUbound);
	/**
	 * Original signature : <code>OTSafeArrayGetLBound(LPSAFEARRAY, unsigned short, long*)</code><br>
	 * <i>native declaration : OTClient.h:315</i>
	 */
	
	NativeLong OTSafeArrayGetLBound(com.softing.opc.client.impl.jna.SAFEARRAY psa, short nDim, com.sun.jna.ptr.NativeLongByReference plLbound);
	/**
	 * Original signature : <code>OTSafeArrayAccessData(LPSAFEARRAY, void**)</code><br>
	 * <i>native declaration : OTClient.h:316</i>
	 */
	
	int OTSafeArrayAccessData(com.softing.opc.client.impl.jna.SAFEARRAY psa, com.sun.jna.ptr.PointerByReference ppvData);
	/**
	 * Original signature : <code>OTSafeArrayUnaccessData(LPSAFEARRAY)</code><br>
	 * <i>native declaration : OTClient.h:317</i>
	 */
	
	int OTSafeArrayUnaccessData(com.softing.opc.client.impl.jna.SAFEARRAY psa);
	/**
	 * Original signature : <code>OTSafeArrayGetElement(LPSAFEARRAY, long*, void*)</code><br>
	 * <i>native declaration : OTClient.h:318</i>
	 */
	
	int OTSafeArrayGetElement(com.softing.opc.client.impl.jna.SAFEARRAY psa, com.sun.jna.ptr.NativeLongByReference rgIndices, com.sun.jna.Pointer pv);
	/**
	 * Original signature : <code>OTSafeArrayPutElement(LPSAFEARRAY, long*, void*)</code><br>
	 * <i>native declaration : OTClient.h:319</i>
	 */
	//TOS changed from softing.SAFEARRAY psa to Pointer psa
	NativeLong OTSafeArrayPutElement(Pointer psa, com.sun.jna.ptr.NativeLongByReference rgIndices, com.sun.jna.Pointer pv);
	/**
	 * Original signature : <code>LONG OTCInitialize(OTCInitData*)</code><br>
	 * <i>native declaration : OTClient.h:702</i>
	 */
	
	com.sun.jna.NativeLong OTCInitialize(com.softing.opc.client.impl.jna.OTCInitData pInitData);
	/**
	 * Original signature : <code>LONG OTCTerminate()</code><br>
	 * <i>native declaration : OTClient.h:704</i>
	 */
	
	com.sun.jna.NativeLong OTCTerminate();
	/**
	 * Original signature : <code>LONG OTCProcessCommandLine(OTChar*)</code><br>
	 * <i>native declaration : OTClient.h:706</i><br>
	 * @deprecated use the safer method {@link #OTCProcessCommandLine(java.nio.ByteBuffer)} instead
	 */
	//@java.lang.Deprecated
	com.sun.jna.NativeLong OTCProcessCommandLine(com.sun.jna.ptr.ByteByReference commandLine);
	/**
	 * Original signature : <code>LONG OTCProcessCommandLine(OTChar*)</code><br>
	 * <i>native declaration : OTClient.h:706</i>
	 */
	//Modified by TOS
	//FROM java.nio.ByteBuffer commandLine TO String commandLine
	com.sun.jna.NativeLong OTCProcessCommandLine(WString commandLine);
	/**
	 * Original signature : <code>LONG OTCAdvise(OTCCallbackFunctions*)</code><br>
	 * <i>native declaration : OTClient.h:708</i>
	 */
	
	com.sun.jna.NativeLong OTCAdvise(com.softing.opc.client.impl.jna.OTCCallbackFunctions pCallbacks);
	/**
	 * Original signature : <code>LONG OTCAddDASession(OTChar*, ULONG, OTObjectHandle*)</code><br>
	 * <i>native declaration : OTClient.h:710</i><br>
	 * @deprecated use the safer method {@link #OTCAddDASession(java.nio.ByteBuffer, com.sun.jna.NativeLong, com.sun.jna.ptr.NativeLongByReference)} instead
	 */
	
	@java.lang.Deprecated
	com.sun.jna.NativeLong OTCAddDASession(com.sun.jna.ptr.ByteByReference url, com.sun.jna.NativeLong sessionUserData, com.sun.jna.ptr.NativeLongByReference pObjectHandle);
	/**
	 * Original signature : <code>LONG OTCAddDASession(OTChar*, ULONG, OTObjectHandle*)</code><br>
	 * <i>native declaration : OTClient.h:710</i>
	 */
	
	//Modified by TOS
	//from java.nio.ByteBuffer url to String url
	com.sun.jna.NativeLong OTCAddDASession(WString url, com.sun.jna.NativeLong sessionUserData, com.sun.jna.ptr.NativeLongByReference pObjectHandle);
	/**
	 * Original signature : <code>LONG OTCAddDASubscription(OTObjectHandle, ULONG, ULONG, OTObjectHandle*)</code><br>
	 * <i>native declaration : OTClient.h:712</i>
	 */
	
	com.sun.jna.NativeLong OTCAddDASubscription(com.sun.jna.NativeLong sessionHandle, com.sun.jna.NativeLong updateRate, com.sun.jna.NativeLong groupUserData, com.sun.jna.ptr.NativeLongByReference pGroupHandle);
	/**
	 * Original signature : <code>LONG OTCAddDAItem(OTObjectHandle, OTChar*, ULONG, OTObjectHandle*)</code><br>
	 * <i>native declaration : OTClient.h:714</i><br>
	 * @deprecated use the safer method {@link #OTCAddDAItem(com.sun.jna.NativeLong, java.nio.ByteBuffer, com.sun.jna.NativeLong, com.sun.jna.ptr.NativeLongByReference)} instead
	 */
	
	@java.lang.Deprecated
	com.sun.jna.NativeLong OTCAddDAItem(com.sun.jna.NativeLong groupHandle, com.sun.jna.ptr.ByteByReference itemID, com.sun.jna.NativeLong itemUserData, com.sun.jna.ptr.NativeLongByReference pItemHandle);
	/**
	 * Original signature : <code>LONG OTCAddDAItem(OTObjectHandle, OTChar*, ULONG, OTObjectHandle*)</code><br>
	 * <i>native declaration : OTClient.h:714</i>
	 */
	
	//Modified by TOS
	//From java.nio.ByteBuffer itemID to String
	com.sun.jna.NativeLong OTCAddDAItem(com.sun.jna.NativeLong groupHandle, WString itemID, com.sun.jna.NativeLong itemUserData, com.sun.jna.ptr.NativeLongByReference pItemHandle);
	/**
	 * Original signature : <code>LONG OTCRemoveDASession(OTObjectHandle)</code><br>
	 * <i>native declaration : OTClient.h:716</i>
	 */
	
	com.sun.jna.NativeLong OTCRemoveDASession(com.sun.jna.NativeLong objectHandle);
	/**
	 * Original signature : <code>LONG OTCRemoveDASubscription(OTObjectHandle)</code><br>
	 * <i>native declaration : OTClient.h:718</i>
	 */
	
	com.sun.jna.NativeLong OTCRemoveDASubscription(com.sun.jna.NativeLong objectHandle);
	/**
	 * Original signature : <code>LONG OTCRemoveDAItem(OTObjectHandle)</code><br>
	 * <i>native declaration : OTClient.h:720</i>
	 */
	
	com.sun.jna.NativeLong OTCRemoveDAItem(com.sun.jna.NativeLong objectHandle);
	/**
	 * Original signature : <code>LONG OTCGetState(OTObjectHandle, unsigned char*, unsigned char*)</code><br>
	 * <i>native declaration : OTClient.h:722</i><br>
	 * @deprecated use the safer method {@link #OTCGetState(com.sun.jna.NativeLong, java.nio.ByteBuffer, java.nio.ByteBuffer)} instead
	 */
	
	@java.lang.Deprecated
	com.sun.jna.NativeLong OTCGetState(com.sun.jna.NativeLong objectHandle, com.sun.jna.ptr.ByteByReference pCurrentState, com.sun.jna.ptr.ByteByReference pTargetState);
	/**
	 * Original signature : <code>LONG OTCGetState(OTObjectHandle, unsigned char*, unsigned char*)</code><br>
	 * <i>native declaration : OTClient.h:722</i>
	 */
	com.sun.jna.NativeLong OTCGetState(com.sun.jna.NativeLong objectHandle, java.nio.ByteBuffer pCurrentState, java.nio.ByteBuffer pTargetState);
	/**
	 * Original signature : <code>LONG OTCChangeTargetState(OTObjectHandle, unsigned char, unsigned char)</code><br>
	 * <i>native declaration : OTClient.h:724</i>
	 */
	
	com.sun.jna.NativeLong OTCChangeTargetState(com.sun.jna.NativeLong objectHandle, byte state, byte deep);
	/**
	 * Original signature : <code>LONG OTCPerformStateTransition(OTObjectHandle, unsigned char, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:726</i>
	 */
	
	com.sun.jna.NativeLong OTCPerformStateTransition(com.sun.jna.NativeLong objectHandle, byte deep, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCRead(OTObjectHandle, ULONG, OTObjectHandle*, OTChar**, OTChar**, ULONG, OTValueData*, LONG*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:728</i>
	 */
	
	//Modified by tos 
	//from com.sun.jna.ptr.PointerByReference pItemIDs to String[] pItemIDs
	//from com.sun.jna.ptr.PointerByReference pItemPaths to String[] pItemPaths
	//from softing.OTValueData pValues to softing.OTValueData.ByReference pValues
	//from com.sun.jna.ptr.NativeLongByReference pItemHandles to Pointer pItemHandles
	com.sun.jna.NativeLong OTCRead(com.sun.jna.NativeLong objectHandle, com.sun.jna.NativeLong count, Pointer pItemHandles, WString[] pItemIDs, WString[] pItemPaths, com.sun.jna.NativeLong maxAge, Pointer pValues, Pointer pResults, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCWrite(OTObjectHandle, ULONG, OTObjectHandle*, OTChar**, OTChar**, OTValueData*, LONG*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:730</i>
	 */
	//Modified by tos
	//from softing.OTValueData.ByReference pValues to Pointer pValues
	com.sun.jna.NativeLong OTCWrite(com.sun.jna.NativeLong objectHandle, com.sun.jna.NativeLong count, Pointer pItemHandles, WString[] pItemIDs, WString[] pItemPaths, Pointer pValues, Pointer pResults, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	
	//com.sun.jna.NativeLong OTCWrite(com.sun.jna.NativeLong objectHandle, com.sun.jna.NativeLong count, Pointer pItemHandles, WString[] pItemIDs, WString[] pItemPaths, softing.OTValueData.ByReference pValues, Pointer pResults, softing.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCGetAttributes(OTObjectHandle, ULONG, OTCObjectAttributes*)</code><br>
	 * <i>native declaration : OTClient.h:732</i>
	 */
	
	com.sun.jna.NativeLong OTCGetAttributes(com.sun.jna.NativeLong objectHandle, com.sun.jna.NativeLong whatAttributes, com.softing.opc.client.impl.jna.OTCObjectAttributes pObjectAttributes);
	/**
	 * Original signature : <code>LONG OTCSetAttributes(OTObjectHandle, ULONG, OTCObjectAttributes*)</code><br>
	 * <i>native declaration : OTClient.h:734</i>
	 */
	
	com.sun.jna.NativeLong OTCSetAttributes(com.sun.jna.NativeLong objectHandle, com.sun.jna.NativeLong whatAttributes, com.softing.opc.client.impl.jna.OTCObjectAttributes pObjectAttributes);
	/**
	 * Original signature : <code>LONG OTCUpdateAttributes(OTObjectHandle, unsigned char, ULONG, ULONG*, LONG*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:736</i>
	 */
	com.sun.jna.NativeLong OTCUpdateAttributes(com.sun.jna.NativeLong objectHandle, byte fromServer, com.sun.jna.NativeLong attributeCount, com.sun.jna.ptr.NativeLongByReference pWhatAttributes, com.sun.jna.ptr.NativeLongByReference pResults, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCUpdateDAItemAttributes(OTObjectHandle, ULONG, OTObjectHandle*, unsigned char, ULONG, ULONG*, LONG*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:738</i>
	 */
	
	com.sun.jna.NativeLong OTCUpdateDAItemAttributes(com.sun.jna.NativeLong subscriptionHandle, com.sun.jna.NativeLong itemCount, com.sun.jna.ptr.NativeLongByReference pItemHandles, byte fromServer, com.sun.jna.NativeLong attributeCount, com.sun.jna.ptr.NativeLongByReference pWhatAttributes, com.sun.jna.ptr.NativeLongByReference pResults, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCGetServerStatus(OTObjectHandle, OTCServerStatus*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:740</i>
	 */
	
	com.sun.jna.NativeLong OTCGetServerStatus(com.sun.jna.NativeLong sessionHandle, com.softing.opc.client.impl.jna.OTCServerStatus pServerStatus, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCValidateDAItems(OTObjectHandle, ULONG, OTObjectHandle*, LONG*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:742</i>
	 */
	//modified by tos
	//from com.sun.jna.ptr.NativeLongByReference pItemHandles to Pointer pItemHandles
	//from com.sun.jna.ptr.NativeLongByReference pResults to Pointer pResults
	com.sun.jna.NativeLong OTCValidateDAItems(com.sun.jna.NativeLong subscriptionHandle, com.sun.jna.NativeLong itemCount, Pointer pItemHandles, Pointer pResults, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCActivateConnectionMonitor(OTObjectHandle, unsigned char, ULONG, ULONG, ULONG, ULONG)</code><br>
	 * <i>native declaration : OTClient.h:744</i>
	 */
	
	com.sun.jna.NativeLong OTCActivateConnectionMonitor(com.sun.jna.NativeLong sessionHandle, byte activate, com.sun.jna.NativeLong checkCycle, com.sun.jna.NativeLong connect1Attempts, com.sun.jna.NativeLong connect1Cycle, com.sun.jna.NativeLong connect2Cycle);
	/**
	 * Original signature : <code>LONG OTCBrowseServer(OTChar*, unsigned char, unsigned char, ULONG*, OTCServerData**, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:746</i><br>
	 * @deprecated use the safer method {@link #OTCBrowseServer(java.nio.ByteBuffer, byte, byte, com.sun.jna.ptr.NativeLongByReference, com.sun.jna.ptr.PointerByReference, com.softing.opc.client.impl.jna.OTCExecutionOptions)} instead
	 */
	
	//@java.lang.Deprecated
	com.sun.jna.NativeLong OTCBrowseServer(com.sun.jna.ptr.ByteByReference ipAddress, byte whatOPCSpec, byte whatServerData, com.sun.jna.ptr.NativeLongByReference pServerDataCount, com.sun.jna.ptr.PointerByReference ppServerData, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCBrowseServer(OTChar*, unsigned char, unsigned char, ULONG*, OTCServerData**, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:746</i>
	 */
	
	com.sun.jna.NativeLong OTCBrowseServer(java.nio.ByteBuffer ipAddress, byte whatOPCSpec, byte whatServerData, com.sun.jna.ptr.NativeLongByReference pServerDataCount, com.sun.jna.ptr.PointerByReference ppServerData, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCBrowseAddressSpace(OTObjectHandle, OTObjectHandle, OTChar*, OTChar*, OTCAddressSpaceBrowseOptions*, ULONG*, OTCAddressSpaceElementData**, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:748</i><br>
	 * @deprecated use the safer method {@link #OTCBrowseAddressSpace(com.sun.jna.NativeLong, com.sun.jna.NativeLong, java.nio.ByteBuffer, java.nio.ByteBuffer, com.softing.opc.client.impl.jna.OTCAddressSpaceBrowseOptions, com.sun.jna.ptr.NativeLongByReference, com.sun.jna.ptr.PointerByReference, com.softing.opc.client.impl.jna.OTCExecutionOptions)} instead
	 */
	
	//@java.lang.Deprecated
	com.sun.jna.NativeLong OTCBrowseAddressSpace(com.sun.jna.NativeLong sessionHandle, com.sun.jna.NativeLong addressSpaceElementHandle, com.sun.jna.ptr.ByteByReference addressSpaceElementID, com.sun.jna.ptr.ByteByReference addressSpaceElementPath, com.softing.opc.client.impl.jna.OTCAddressSpaceBrowseOptions pBrowseOptions, com.sun.jna.ptr.NativeLongByReference pAddressSpaceElementDataCount, com.sun.jna.ptr.PointerByReference ppAddressSpaceElement, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCBrowseAddressSpace(OTObjectHandle, OTObjectHandle, OTChar*, OTChar*, OTCAddressSpaceBrowseOptions*, ULONG*, OTCAddressSpaceElementData**, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:748</i>
	 */
	
	//Modified by TOS
	// from PointerByReference ppAddressSpaceElement to OTCAddressSpaceElementData.ByReference[] ppAddressSpaceElement
	// from java.nio.ByteBuffer addressSpaceElementID to String addressSpaceElementID
	// from OTCAddressSpaceElementData.ByReference[] ppAddressSpaceElement to OTCAddressSpaceElementData.ByReference ppAddressSpaceElement
	// from java.nio.ByteBuffer addressSpaceElementPath to String addressSpaceElementPath
	com.sun.jna.NativeLong OTCBrowseAddressSpace(com.sun.jna.NativeLong sessionHandle, com.sun.jna.NativeLong addressSpaceElementHandle, WString addressSpaceElementID, WString addressSpaceElementPath, com.softing.opc.client.impl.jna.OTCAddressSpaceBrowseOptions pBrowseOptions, com.sun.jna.ptr.NativeLongByReference pAddressSpaceElementDataCount, com.sun.jna.ptr.PointerByReference ppAddressSpaceElement, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCReleaseAddressSpaceElement(OTObjectHandle, OTBool)</code><br>
	 * <i>native declaration : OTClient.h:750</i>
	 */
	
	com.sun.jna.NativeLong OTCReleaseAddressSpaceElement(com.sun.jna.NativeLong addressSpaceElementHandle, int deep);
	/**
	 * Original signature : <code>LONG OTCGetDAProperties(OTObjectHandle, OTObjectHandle, OTChar*, OTChar*, OTCGetDAPropertiesOptions*, ULONG*, OTCDAPropertyData**, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:752</i><br>
	 * @deprecated use the safer method {@link #OTCGetDAProperties(com.sun.jna.NativeLong, com.sun.jna.NativeLong, java.nio.ByteBuffer, java.nio.ByteBuffer, com.softing.opc.client.impl.jna.OTCGetDAPropertiesOptions, com.sun.jna.ptr.NativeLongByReference, com.sun.jna.ptr.PointerByReference, com.softing.opc.client.impl.jna.OTCExecutionOptions)} instead
	 */
	
	//@java.lang.Deprecated
	com.sun.jna.NativeLong OTCGetDAProperties(com.sun.jna.NativeLong sessionHandle, com.sun.jna.NativeLong addressSpaceElementHandle, com.sun.jna.ptr.ByteByReference addressSpaceElementID, com.sun.jna.ptr.ByteByReference addressSpaceElementPath, com.softing.opc.client.impl.jna.OTCGetDAPropertiesOptions pGetPropertiesOptions, com.sun.jna.ptr.NativeLongByReference pPropertiesDataCount, com.sun.jna.ptr.PointerByReference ppProperty, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCGetDAProperties(OTObjectHandle, OTObjectHandle, OTChar*, OTChar*, OTCGetDAPropertiesOptions*, ULONG*, OTCDAPropertyData**, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:752</i>
	 */
	//Modified by tos
	//from java.nio.ByteBuffer addressSpaceElementID to String addressSpaceElementID
	//from java.nio.ByteBuffer addressSpaceElementPath to String addressSpaceElementPath
	com.sun.jna.NativeLong OTCGetDAProperties(com.sun.jna.NativeLong sessionHandle, com.sun.jna.NativeLong addressSpaceElementHandle,WString addressSpaceElementID, WString addressSpaceElementPath, com.softing.opc.client.impl.jna.OTCGetDAPropertiesOptions pGetPropertiesOptions, com.sun.jna.ptr.NativeLongByReference pPropertiesDataCount, com.sun.jna.ptr.PointerByReference ppProperty, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCRefreshDAGroup(OTObjectHandle, ULONG, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:754</i>
	 */
	
	com.sun.jna.NativeLong OTCRefreshDAGroup(com.sun.jna.NativeLong subscriptionHandle, com.sun.jna.NativeLong maxAge, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCAddAESession(OTChar*, ULONG, OTObjectHandle*)</code><br>
	 * <i>native declaration : OTClient.h:756</i><br>
	 * @deprecated use the safer method {@link #OTCAddAESession(java.nio.ByteBuffer, com.sun.jna.NativeLong, com.sun.jna.ptr.NativeLongByReference)} instead
	 */
	
	@java.lang.Deprecated
	com.sun.jna.NativeLong OTCAddAESession(com.sun.jna.ptr.ByteByReference url, com.sun.jna.NativeLong sessionUserData, com.sun.jna.ptr.NativeLongByReference pObjectHandle);
	/**
	 * Original signature : <code>LONG OTCAddAESession(OTChar*, ULONG, OTObjectHandle*)</code><br>
	 * <i>native declaration : OTClient.h:756</i>
	 */
	
	com.sun.jna.NativeLong OTCAddAESession(java.nio.ByteBuffer url, com.sun.jna.NativeLong sessionUserData, com.sun.jna.ptr.NativeLongByReference pObjectHandle);
	/**
	 * Original signature : <code>LONG OTCAddAESubscription(OTObjectHandle, ULONG, OTObjectHandle*)</code><br>
	 * <i>native declaration : OTClient.h:758</i>
	 */
	
	com.sun.jna.NativeLong OTCAddAESubscription(com.sun.jna.NativeLong sessionHandle, com.sun.jna.NativeLong subscriptionUserData, com.sun.jna.ptr.NativeLongByReference pSubscriptionHandle);
	/**
	 * Original signature : <code>LONG OTCRemoveAESession(OTObjectHandle)</code><br>
	 * <i>native declaration : OTClient.h:760</i>
	 */
	
	com.sun.jna.NativeLong OTCRemoveAESession(com.sun.jna.NativeLong objectHandle);
	/**
	 * Original signature : <code>LONG OTCRemoveAESubscription(OTObjectHandle)</code><br>
	 * <i>native declaration : OTClient.h:762</i>
	 */
	
	com.sun.jna.NativeLong OTCRemoveAESubscription(com.sun.jna.NativeLong objectHandle);
	/**
	 * Original signature : <code>LONG OTCRefreshAEConditions(OTObjectHandle, unsigned char, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:764</i>
	 */
	
	com.sun.jna.NativeLong OTCRefreshAEConditions(com.sun.jna.NativeLong subscriptionHandle, byte cancelRefresh, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCAcknowledgeAEConditions(OTObjectHandle, OTChar*, OTChar*, ULONG, OTCEventData*, LONG*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:766</i><br>
	 * @deprecated use the safer method {@link #OTCAcknowledgeAEConditions(com.sun.jna.NativeLong, java.nio.ByteBuffer, java.nio.ByteBuffer, com.sun.jna.NativeLong, com.softing.opc.client.impl.jna.OTCEventData, com.sun.jna.ptr.NativeLongByReference, com.softing.opc.client.impl.jna.OTCExecutionOptions)} instead
	 */
	
	//@java.lang.Deprecated
	com.sun.jna.NativeLong OTCAcknowledgeAEConditions(com.sun.jna.NativeLong sessionHandle, com.sun.jna.ptr.ByteByReference ackID, com.sun.jna.ptr.ByteByReference ackComment, com.sun.jna.NativeLong count, com.softing.opc.client.impl.jna.OTCEventData pEvents, com.sun.jna.ptr.NativeLongByReference pResults, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCAcknowledgeAEConditions(OTObjectHandle, OTChar*, OTChar*, ULONG, OTCEventData*, LONG*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:766</i>
	 */
	
	com.sun.jna.NativeLong OTCAcknowledgeAEConditions(com.sun.jna.NativeLong sessionHandle, java.nio.ByteBuffer ackID, java.nio.ByteBuffer ackComment, com.sun.jna.NativeLong count, com.softing.opc.client.impl.jna.OTCEventData pEvents, com.sun.jna.ptr.NativeLongByReference pResults, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCQueryAvailableAEFilters(OTObjectHandle, ULONG*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:768</i>
	 */
	
	com.sun.jna.NativeLong OTCQueryAvailableAEFilters(com.sun.jna.NativeLong sessionHandle, com.sun.jna.ptr.NativeLongByReference pAvailableFilters, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCQueryAECategories(OTObjectHandle, ULONG*, ULONG**, ULONG**, OTChar***, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:770</i>
	 */
	
	com.sun.jna.NativeLong OTCQueryAECategories(com.sun.jna.NativeLong sessionHandle, com.sun.jna.ptr.NativeLongByReference pCount, com.sun.jna.ptr.PointerByReference ppEventTypes, com.sun.jna.ptr.PointerByReference ppCategoryIds, com.sun.jna.ptr.PointerByReference ppCategoryDescriptions, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCQueryAEAttributes(OTObjectHandle, ULONG, ULONG*, ULONG**, OTChar***, unsigned short**, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:772</i>
	 */
	
	com.sun.jna.NativeLong OTCQueryAEAttributes(com.sun.jna.NativeLong sessionHandle, com.sun.jna.NativeLong categoryId, com.sun.jna.ptr.NativeLongByReference pCount, com.sun.jna.ptr.PointerByReference ppAttributeIds, com.sun.jna.ptr.PointerByReference ppAttributeDescriptions, com.sun.jna.ptr.PointerByReference ppAttributeDatatypes, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCQueryAEConditions(OTObjectHandle, ULONG, ULONG*, OTChar***, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:774</i>
	 */
	
	com.sun.jna.NativeLong OTCQueryAEConditions(com.sun.jna.NativeLong sessionHandle, com.sun.jna.NativeLong categoryId, com.sun.jna.ptr.NativeLongByReference pCount, com.sun.jna.ptr.PointerByReference ppConditionsNames, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCQueryAESubConditions(OTObjectHandle, OTChar*, ULONG*, OTChar***, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:776</i><br>
	 * @deprecated use the safer method {@link #OTCQueryAESubConditions(com.sun.jna.NativeLong, java.nio.ByteBuffer, com.sun.jna.ptr.NativeLongByReference, com.sun.jna.ptr.PointerByReference, com.softing.opc.client.impl.jna.OTCExecutionOptions)} instead
	 */
	
	//@java.lang.Deprecated
	com.sun.jna.NativeLong OTCQueryAESubConditions(com.sun.jna.NativeLong sessionHandle, com.sun.jna.ptr.ByteByReference conditionName, com.sun.jna.ptr.NativeLongByReference pCount, com.sun.jna.ptr.PointerByReference ppSubConditionsNames, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCQueryAESubConditions(OTObjectHandle, OTChar*, ULONG*, OTChar***, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:776</i>
	 */
	
	com.sun.jna.NativeLong OTCQueryAESubConditions(com.sun.jna.NativeLong sessionHandle, java.nio.ByteBuffer conditionName, com.sun.jna.ptr.NativeLongByReference pCount, com.sun.jna.ptr.PointerByReference ppSubConditionsNames, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCQueryAESourceConditions(OTObjectHandle, OTChar*, ULONG*, OTChar***, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:778</i><br>
	 * @deprecated use the safer method {@link #OTCQueryAESourceConditions(com.sun.jna.NativeLong, java.nio.ByteBuffer, com.sun.jna.ptr.NativeLongByReference, com.sun.jna.ptr.PointerByReference, com.softing.opc.client.impl.jna.OTCExecutionOptions)} instead
	 */
	
	//@java.lang.Deprecated
	com.sun.jna.NativeLong OTCQueryAESourceConditions(com.sun.jna.NativeLong sessionHandle, com.sun.jna.ptr.ByteByReference sourcePath, com.sun.jna.ptr.NativeLongByReference pCount, com.sun.jna.ptr.PointerByReference ppConditionsNames, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCQueryAESourceConditions(OTObjectHandle, OTChar*, ULONG*, OTChar***, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:778</i>
	 */
	
	com.sun.jna.NativeLong OTCQueryAESourceConditions(com.sun.jna.NativeLong sessionHandle, java.nio.ByteBuffer sourcePath, com.sun.jna.ptr.NativeLongByReference pCount, com.sun.jna.ptr.PointerByReference ppConditionsNames, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCGetAEConditionState(OTObjectHandle, OTChar*, OTChar*, ULONG, ULONG*, OTCAEConditionStateData*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:780</i><br>
	 * @deprecated use the safer method {@link #OTCGetAEConditionState(com.sun.jna.NativeLong, java.nio.ByteBuffer, java.nio.ByteBuffer, com.sun.jna.NativeLong, com.sun.jna.ptr.NativeLongByReference, com.softing.opc.client.impl.jna.OTCAEConditionStateData, com.softing.opc.client.impl.jna.OTCExecutionOptions)} instead
	 */
	
	//@java.lang.Deprecated
	com.sun.jna.NativeLong OTCGetAEConditionState(com.sun.jna.NativeLong sessionHandle, com.sun.jna.ptr.ByteByReference sourcePath, com.sun.jna.ptr.ByteByReference conditionName, com.sun.jna.NativeLong attributeCount, com.sun.jna.ptr.NativeLongByReference pAttributeIds, com.softing.opc.client.impl.jna.OTCAEConditionStateData pConditionState, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCGetAEConditionState(OTObjectHandle, OTChar*, OTChar*, ULONG, ULONG*, OTCAEConditionStateData*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:780</i>
	 */
	
	com.sun.jna.NativeLong OTCGetAEConditionState(com.sun.jna.NativeLong sessionHandle, java.nio.ByteBuffer sourcePath, java.nio.ByteBuffer conditionName, com.sun.jna.NativeLong attributeCount, com.sun.jna.ptr.NativeLongByReference pAttributeIds, com.softing.opc.client.impl.jna.OTCAEConditionStateData pConditionState, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCEnableAEConditions(OTObjectHandle, unsigned char, unsigned char, OTChar*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:782</i><br>
	 * @deprecated use the safer method {@link #OTCEnableAEConditions(com.sun.jna.NativeLong, byte, byte, java.nio.ByteBuffer, com.softing.opc.client.impl.jna.OTCExecutionOptions)} instead
	 */
	
	//@java.lang.Deprecated
	com.sun.jna.NativeLong OTCEnableAEConditions(com.sun.jna.NativeLong sessionHandle, byte enable, byte areaOrSource, com.sun.jna.ptr.ByteByReference path, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCEnableAEConditions(OTObjectHandle, unsigned char, unsigned char, OTChar*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:782</i>
	 */
	
	com.sun.jna.NativeLong OTCEnableAEConditions(com.sun.jna.NativeLong sessionHandle, byte enable, byte areaOrSource, java.nio.ByteBuffer path, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCGetErrorString(OTObjectHandle, LONG, OTChar**, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:784</i>
	 */
	
	com.sun.jna.NativeLong OTCGetErrorString(com.sun.jna.NativeLong sessionHandle, com.sun.jna.NativeLong errorCode, com.sun.jna.ptr.PointerByReference pErrorString, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCLogon(OTObjectHandle, OTChar*, OTChar*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:786</i><br>
	 * @deprecated use the safer method {@link #OTCLogon(com.sun.jna.NativeLong, java.nio.ByteBuffer, java.nio.ByteBuffer, com.softing.opc.client.impl.jna.OTCExecutionOptions)} instead
	 */
	
	@java.lang.Deprecated
	com.sun.jna.NativeLong OTCLogon(com.sun.jna.NativeLong sessionHandle, com.sun.jna.ptr.ByteByReference userName, com.sun.jna.ptr.ByteByReference password, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCLogon(OTObjectHandle, OTChar*, OTChar*, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:786</i>
	 */
	
	//MODIFIED By TOS
	//FROM java.nio.ByteBuffer userName to String userName
	//FROM java.nio.ByteBuffer password to String password
	com.sun.jna.NativeLong OTCLogon(com.sun.jna.NativeLong sessionHandle, WString userName, WString password, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/**
	 * Original signature : <code>LONG OTCLogoff(OTObjectHandle, OTCExecutionOptions*)</code><br>
	 * <i>native declaration : OTClient.h:788</i>
	 */
		
	com.sun.jna.NativeLong OTCLogoff(com.sun.jna.NativeLong sessionHandle, com.softing.opc.client.impl.jna.OTCExecutionOptions pExecutionOptions);
	/// Pointer to unknown (opaque) type
	
	VARIANT INVALID_POINTER_VARIANT = new VARIANT() {
		{
			super.setPointer(Pointer.createConstant(-1));
		}

		public void setPointer(Pointer p) {
			throw new UnsupportedOperationException("Immutable reference");
		}
	};
	



}
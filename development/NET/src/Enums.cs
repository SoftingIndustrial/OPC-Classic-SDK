using System;

namespace Softing.OPCToolbox
{
	/// <summary>
	/// Describes the quality state for an <see cref="Softing.OPCToolbox.Client.DaItem"/>'s data value.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumQuality"]/doc/*'
	/// />
	public enum EnumQuality : ushort
	{
		/// <summary>
		/// The value is good.
		/// </summary>
		GOOD = 0xC0,

		/// <summary>
		/// The value has been overridden. Typically this means that the input has been disconnected and a manually value has been forced.
		/// </summary>
		GOOD_LOCAL_OVERRIDE = 0xD8,

		/// <summary>
		/// The value is bad but no specific reason is known.
		/// </summary>
		BAD = 0x00,

		/// <summary>
		/// There is a server specific problem with the configuration.
		/// </summary>
		BAD_CONFIG_ERROR = 0x04,

		/// <summary>
		/// The input is required to be logically connected to something but the connection couldn't be established.
		/// </summary>
		BAD_NOT_CONNECTED = 0x08,

		/// <summary>
		/// A device failure has been detected.
		/// </summary>
		BAD_DEVICE_FAILURE = 0x0C,

		/// <summary>
		/// A sensor failure has been detected .
		/// </summary>
		BAD_SENSOR_FAILURE = 0x10,

		/// <summary>
		/// Communications have failed. However, the last known value is available.
		/// </summary>
		BAD_LAST_KNOWN = 0x14,

		/// <summary>
		/// Communications have failed. No last known value is available.
		/// </summary>e
		BAD_COMM_FAILURE = 0x18,

		/// <summary>
		/// The block is off scan or otherwise locked.
		/// </summary>
		BAD_OUT_OF_SERVICE = 0x1C,

		/// <summary>
		/// The initial quality for the data items added to a subscription.
		/// </summary>
		BAD_WAITING_FOR_INITIAL_DATA = 0x20,

		/// <summary>
		/// There is no specific reason why the value is uncertain.
		/// </summary>
		UNCERTAIN = 0x40,

		/// <summary>
		/// The last usable value.
		/// </summary>
		UNCERTAIN_LAST_USABLE = 0x44,

		/// <summary>
		/// Either the value has pegged at one of the sensor limits (in which case the limit field should be set to 1 or 2)
		/// or the sensor is otherwise known to be out of calibration.
		/// </summary>
		UNCERTAIN_SENSOR_CAL = 0x50,

		/// <summary>
		/// The returned value is outside the limits defined for this parameter.
		/// Note that in this case the limit flag indicates which limit has been exceeded.
		/// </summary>
		UNCERTAIN_EGU_EXCEEDED = 0x54,

		/// <summary>
		/// The returned value is under the limit defined for this parameter.
		/// </summary>
		UNCERTAIN_EGU_EXCEEDED_LOW = 0x55,

		/// <summary>
		/// The returned value is over the limit defined for this parameter.
		/// </summary>
		UNCERTAIN_EGU_EXCEEDED_HIGH = 0x56,

		/// <summary>
		/// The value is derived from multiple sources and has less than the required number of good sources.
		/// </summary>
		UNCERTAIN_SUB_NORMAL = 0x58,

		/// <summary>
		/// The quality is not set.
		/// </summary>
		QUALITY_NOT_SET = 0xFFFF
	}

	/// <summary>
	/// Describes the toolkit application licensed features.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumFeature"]/doc/*'
	/// />
	public enum EnumFeature : byte
	{
		/// <summary>
		/// Data Access Server.
		/// </summary>
		DA_SERVER = 0x01,

		/// <summary>
		/// XML - Data Access Server.
		/// </summary>
		XMLDA_SERVER = 0x02,

		/// <summary>
		/// Alarms and Events Server.
		/// </summary>
		AE_SERVER = 0x04,

		/// <summary>
		/// OPC Tunnel Server.
		/// </summary>
		TP_SERVER = 0x08,

		/// <summary>
		/// Data Access Client.
		/// </summary>
		DA_CLIENT = 0x10,

		/// <summary>
		/// XML - Data Access Client.
		/// </summary>
		XMLDA_CLIENT = 0x20,

		/// <summary>
		/// Alarms and Events Client.
		/// </summary>
		AE_CLIENT = 0x40,

		/// <summary>
		/// OPC Tunnel Client.
		/// </summary>
		TP_CLIENT = 0x80
	} //	end EnumFeature

	/// <summary>
	/// Describes the limit exceeds of an process value.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumQualityLimit"]/doc/*'
	/// />
	public enum EnumQualityLimit : byte
	{
		/// <summary>
		/// The value is free to move up or down.
		/// </summary>
		OK = 0,

		/// <summary>
		/// The value has pegged at some lower limit.
		/// </summary>
		LOW = 1,

		/// <summary>
		/// The value has pegged at some high limit.
		/// </summary>
		HIGH = 2,

		/// <summary>
		/// The value is constant.
		/// </summary>
		CONSTANT = 3
	} //	end EnumQualityLimit

	/// <summary>
	/// Describes the engineering units information type of an item.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumEUType"]/doc/*'
	/// />
	public enum EnumEUType : byte
	{
		/// <summary>
		/// No engineering units information.
		/// </summary>
		NOENUM = 0,

		/// <summary>
		/// The limits (Hi + Low) of analog values.
		/// </summary>
		ANALOG = 1,

		/// <summary>
		/// The strings of enumerators.
		/// </summary>
		ENUMERATED = 2
	} //	end EnumEUType

	/// <summary>
	/// Describes the OPC specification. An OPC specification describes interfaces and corresponding behavior that an
	/// OPC server implements, and an OPC client application depends on.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumOPCSpecification"]/doc/*'
	/// />
	public enum EnumOPCSpecification : byte
	{
		/// <summary>
		/// The current OPC Specification.
		/// </summary>
		DEFAULT = 0x00,

		/// <summary>
		/// OPC Data Access Specification version 1.0.
		/// </summary>
		DA10 = 0x03,

		/// <summary>
		/// OPC Data Access Specification version 2.0.
		/// </summary>
		DA20 = 0x04,

		/// <summary>
		/// OPC Data Access Specification version 3.0.
		/// </summary>
		DA30 = 0x10,

		/// <summary>
		/// OPC Alarms and Events Specification version 1.10.
		/// </summary>
		AE10 = 0x08,

		/// <summary>
		/// OPC XML Data Access Specification version 1.01.
		/// </summary>
		XMLDA10 = 0x20
	}

	/// <summary>
	///Describes  the access rights to the AddressSpaceElements of the OPC DA namespace or their Properties.
	///</summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumAccessRights"]/doc/*'
	/// />
	public enum EnumAccessRights : byte
	{
		///<summary>The client can only read the data item`s value.</summary>
		READABLE = 0x01,

		///<summary>The client can only change the data item`s value.</summary>
		WRITEABLE = 0x02,

		///<summary>Item can be read and changed by the client.</summary>
		READWRITEABLE = 0x03
	} //	end EnumAccessRights

	/// <summary>
	/// Describes the possible returned error codes.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumResultCode"]/doc/*'
	/// />
	public enum EnumResultCode : int
	{
		/// <summary>0x0
		/// <br/>
		/// The operation succeeded.</summary>
		S_OK = 0, //0x00000000

		/// <summary>0x1
		/// <br/>
		/// The operation succeeded but there are one or more minor errors.<br/>
		/// The individual errors must be analyzed for more information.
		/// </summary>
		S_FALSE = 1, // 0x00000001

		/// <summary>0x80004001
		/// <br/>
		/// The function is not implemented in the base class and should be overridden.</summary>
		E_NOTIMPL = -2147467263, // 0x80004001

		/// <summary>0x8007000E
		/// <br/>
		/// Not enough memory to complete the requested operation.</summary>
		E_OUTOFMEMORY = -2147024882, //0x8007000E,

		/// <summary>0x80070057
		/// <br/>
		/// The value of one or more parameters was not valid.</summary>
		E_INVALIDARG = -2147024809, //0x80070057,

		/// <summary>0x80004002
		/// <br/>
		/// No such interface supported.</summary>
		E_NOINTERFACE = -2147467262, //0x80004002,

		/// <summary>0x80070005
		/// <br/>
		/// Execution access of current action is denied.</summary>
		E_ACCESSDENIED = -2147024891, //0x80070005,

		/// <summary>0x80004005
		/// <br/>
		/// The function could not complete the designed action. The error is not specified.</summary>
		E_FAIL = -2147467259, //0x80004005,

		/// <summary>0x8000FFFF
		/// <br/>
		/// Unexpected error not matching one of the other values.</summary>
		E_UNEXPECTED = -2147418113, //0x8000FFFF,

		/// <summary>0x80040200
		/// <br/>
		/// There is no existing connection.</summary>
		CONNECT_E_NOCONNECTION = -2147220992, //0x80040200,

		/// <summary>0x80040201
		/// <br/>
		/// Advise limit exceeded for this object.</summary>
		CONNECT_E_ADVISELIMIT = -2147220991, //0x80040201,

		/// <summary>0x80020005
		/// <br/>
		/// Type mismatch.</summary>
		DISP_E_TYPEMISMATCH = -2147352571, //0x80020005,

		/// <summary>0x8002000A
		/// <br/>
		/// Out of present range.</summary>
		DISP_E_OVERFLOW = -2147352566, //0x8002000A,

		/// <summary>0x8002000B
		/// <br/>
		/// Invalid index.</summary>
		DISP_E_BADINDEX = -2147352565, //0x8002000B

		/// <summary>0xC0040001
		/// <br/>
		/// The value of the handle is invalid.</summary>
		E_INVALIDHANDLE = -1073479679, //0xC0040001,

		/// <summary>0xC0040004
		/// <br/>
		/// The server cannot convert the data between the specified format / requested data type and the canonical data type.</summary>
		E_BADTYPE = -1073479676, //0xC0040004,

		/// <summary>0xC0040005
		/// <br/>
		/// The requested operation cannot be done on a public group.</summary>
		E_PUBLIC = -1073479675, //0xC0040005,

		/// <summary>0xC0040006
		/// <br/>
		/// The item's <see cref="Softing.OPCToolbox.Client.DaItem.AccessRights"/> do not allow the operation.</summary>
		E_BADRIGHTS = -1073479674, //0xC0040006,

		/// <summary>0xC0040007
		/// <br/>
		/// The item ID is not defined in the server address space (on add or validate) or no longer
		/// exists in the server address space (for read or write).
		/// </summary>
		E_UNKNOWNITEMID = -1073479673, //0xC0040007,

		/// <summary>0xC0040008
		/// <br/>
		/// The item ID doesn't conform to the server's syntax.</summary>
		E_INVALIDITEMID = -1073479672, //0xC0040008,

		/// <summary>0xC0040009
		/// <br/>
		/// The filter string was not valid.</summary>
		E_INVALIDFILTER = -1073479671, //0xC0040009,

		/// <summary>0xC004000A
		/// <br/>
		/// The item's access path is not known to the server.</summary>
		E_UNKNOWNPATH = -1073479670, //0xC004000A,

		/// <summary>0xC004000B
		/// <br/>
		/// The value was out of range.</summary>
		E_RANGE = -1073479669, //0xC004000B,

		/// <summary>0xC004000C
		/// <br/>
		/// Duplicate name not allowed.</summary>
		E_DUPLICATENAME = -1073479668, //0xC004000C,

		/// <summary>0x0004000D
		/// <br/>
		/// The server does not support the requested data rate but will use the closest available rate.</summary>
		S_UNSUPPORTEDRATE = 262157, //0x0004000D,

		/// <summary>0x0004000E
		/// <br/>
		/// A value passed to WRITE was accepted but the output was clamped.</summary>
		S_CLAMP = 262158, //0x0004000E,

		/// <summary>0x0004000F
		/// <br/>
		/// The operation cannot be performed because the object is being referenced.</summary>
		S_INUSE = 262159, //0x0004000F,

		/// <summary>0xC0040010
		/// <br/>
		/// The server's configuration file is an invalid format.</summary>
		E_INVALIDCONFIGFILE = -1073479664, //0xC0040010,

		/// <summary>0xC0040011
		/// <br/>
		/// The requested object (e.g. a public group) was not found.</summary>
		E_NOTFOUND = -1073479663, //0xC0040011,

		/// <summary>0xC0040203
		/// <br/>
		/// The passed property ID is not valid for the item.</summary>
		E_INVALID_PID = -1073479165, //0xC0040203,

		/// <summary>0xC0040400
		/// <br/>
		/// The item deadband has not been set for this item.</summary>
		E_DEADBANDNOTSET = -1073478656, //0xC0040400,

		/// <summary>0xC0040401
		/// <br/>
		/// The item does not support deadband.</summary>
		E_DEADBANDNOTSUPPORTED = -1073478655, //0xC0040401,

		/// <summary>0xC0040402
		/// <br/>
		/// The server does not support buffering of data items that are collected at a faster rate than the group update rate.</summary>
		E_NOBUFFERING = -1073478654, //0xC0040402,

		/// <summary>0xC0040403
		/// <br/>
		/// The continuation point is not valid.</summary>
		E_INVALIDCONTINUATIONPOINT = -1073478653, //0xC0040403,

		/// <summary>0x00040404
		/// <br/>
		/// Indicates that not every detected change has been returned for this item. Only the most recent data is provided.</summary>
		S_DATAQUEUEOVERFLOW = 263172, //0x00040404,

		/// <summary>0xC0040405
		/// <br/>
		/// Indicates that there is no sampling rate set for a particular item. In this case, the item
		/// defaults to the update rate of the group.</summary>
		E_RATENOTSET = -1073478651, //0xC0040405,

		/// <summary>0xC0040406
		/// <br/>
		/// If a client attempts to write any value, quality, timestamp combination and the server does not
		/// support the requested combination (which could be a single quantity such as just timestamp),
		///  then the server will not perform any write and will return this error code.</summary>
		E_NOTSUPPORTED = -1073478650, //0xC0040406,

		/// <summary>0x80070428
		/// <br/>
		/// The function could not complete the designed action (exception). The error is not specified.</summary>
		E_EXCEPTION = -2147023832 //0x80070428,
	}

	/// <summary>
	/// Describes the data the DaAddressSpaceRoot::queryAddressSpaceElementData
	/// callback has to provide.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//class[@name="EnumAddressSpaceElementData"]/doc/*'
	/// />
	public enum EnumAddressSpaceElementData : int
	{
		/// <summary>If set, OTSAddressSpaceElementData.m_name must be provided</summary>
		NAME = 0x001,

		/// <summary>If set, OTSAddressSpaceElementData.m_itemID must be provided</summary>
		ITEMID = 0x002,

		/// <summary>If set, OTSAddressSpaceElementData.m_accessRights must be provided</summary>
		ACCESSRIGHTS = 0x004,

		/// <summary>If set, OTSAddressSpaceElementData.m_ioMode must be provided</summary>
		IOMODE = 0x008,

		/// <summary>If set, OTSAddressSpaceElementData.m_hasChildren must be provided</summary>
		HASCHILDREN = 0x010,

		/// <summary>If set, OTSAddressSpaceElementData.m_isBrowsable must be provided</summary>
		ISBROWSABLE = 0x020,

		/// <summary>If set, OTSAddressSpaceElementData.m_userData must be provided</summary>
		USERDATA = 0x040,

		/// <summary>If set, OTSAddressSpaceElementData.m_dataType must be provided</summary>
		DATATYPE = 0x080,

		/// <summary>If set, OTSAddressSpaceElementData.m_elementType must be provided</summary>
		ELEMENTTYPE = 0x100
	}

	/// <summary>
	/// Helper class for detecting whether an operation execution has succeeded or failed.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//class[@name="ResultCode"]/doc/*'
	/// />
	public class ResultCode
	{
		/// <summary>
		/// Initializes a new instance of the <see cref="ResultCode"/> class.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ResultCode"]/constructor[@name="ResultCode"]/doc/*'
		/// />
		public ResultCode()
		{
		}

		/// <summary>
		/// The error mask.
		/// </summary>
		internal const uint ERROR_MASK = 0x80000000;

		/// <summary>
		/// The success mask.
		/// </summary>
		internal const uint SUCCESS_MASK = 0x00000000;

		/// <summary>
		/// Checks whether a result code does <b>not</b> represent an error and returns true in that case.
		/// </summary>
		/// <param name = "res">A <see cref="System.UInt32"/> that represents a result to be tested on success.</param>
		/// <returns>
		/// <i>true</i>, given result doesn't represent an error;
		/// <i>false</i>, given result represents an error.</returns>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ResultCode"]/method[@name="SUCCEEDED_1"]/doc/*'
		/// />
		public static bool SUCCEEDED(int res)
		{
			return ((res & ERROR_MASK) == SUCCESS_MASK);
		}

		/// <summary>
		/// Checks whether a result code is <b>not</b> an error and returns true in that case.
		/// </summary>
		/// <param name = "res">A <see cref="EnumResultCode"/> that represents a result to be tested on success.</param>
		/// <returns>
		/// <i>true</i>, given result doesn't represent an error;
		/// <i>false</i>, given result represents an error.</returns>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ResultCode"]/method[@name="SUCCEEDED_2"]/doc/*'
		/// />
		public static bool SUCCEEDED(EnumResultCode res)
		{
			return ((((int) res) & ERROR_MASK) == SUCCESS_MASK);
		}

		/// <summary>
		/// Checks whether a result code represents an error and returns true in case of an error.
		/// </summary>
		/// <param name = "res">A <see cref="System.UInt32"/> that represents a result to be tested on failure.</param>
		/// <returns>
		/// <i>true</i>, given result represents an error;
		/// <i>false</i>, given result doesn't represent an error.</returns>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ResultCode"]/method[@name="FAILED_1"]/doc/*'
		/// />
		public static bool FAILED(int res)
		{
			return ((res & ERROR_MASK) == ERROR_MASK);
		}

		/// <summary>
		/// Checks whether a result code represents an error and returns true in case of an error.
		/// </summary>
		/// <param name = "res">A <see cref="EnumResultCode"/> that represents a result to be tested on failure.</param>
		/// <returns>
		/// <i>true</i>, given result represents an error;
		/// <i>false</i>, given result doesn't represent an error.</returns>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ResultCode"]/method[@name="FAILED_2"]/doc/*'
		/// />
		public static bool FAILED(EnumResultCode res)
		{
			return ((((int) res) & ERROR_MASK) == ERROR_MASK);
		}
	} //	end class ResultCode

	/// <summary>
	/// Describes the possible event types.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumEventType"]/doc/*'
	/// />
	public enum EnumEventType : byte
	{
		/// <summary>
		/// This event type is used to notify the occurence of simple events (e.g. failure of a device) to the client.
		/// </summary>
		SIMPLE = 0x01,

		/// <summary>
		///The events of this type represent the occurrences which involve some
		///interference from outside the server and which result in a value change of a process parameter.
		///An example is setting of a new limit value for a process variable by a client.
		/// </summary>
		TRACKING = 0x02,

		/// <summary>
		///The events of this type are always associated with a condition. They are fired as a result of a state change of this condition
		///(e.g. active/inactive). Exceeding a limit is an example for such kind of an event.
		/// </summary>
		CONDITION = 0x04
	} //	end EnumEventType

	/// <summary>
	/// Describes the possible states of a condition.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumConditionState"]/doc/*'
	/// />
	public enum EnumConditionState : byte
	{
		/// <summary>
		/// The condition is not enabled, active or acknowledged depending on the change mask.
		/// </summary>
		NO = 0x00,

		/// <summary>
		/// The condition is currently being checked by an OPC Event Server.
		/// </summary>
		ENABLED = 0x01,

		/// <summary>
		/// The associated object is currently in the state represented by the condition.
		/// </summary>
		ACTIVE = 0x02,

		/// <summary>
		/// The condition is acknowledged.
		/// </summary>
		ACKED = 0x04
	} //	end EnumConditionState

	/// <summary>
	/// Describes which properties of the condition have been changed causing the server to send an event notification.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumConditionChange"]/doc/*'
	/// />
	public enum EnumConditionChange : ushort
	{
		/// <summary>
		/// The condition’s active state has changed.
		/// </summary>
		ACTIVE_STATE = 0x0001,

		/// <summary>
		/// The condition’s acknowledgment state has changed.
		/// </summary>
		ACK_STATE = 0x0002,

		/// <summary>
		/// The condition’s enabled state has changed.
		/// </summary>
		ENABLE_STATE = 0x0004,

		/// <summary>
		/// The condition’s <see cref="Softing.OPCToolbox.Client.AeConditionState.State">state</see> has changed.
		/// </summary>
		STATE = 0x0007,

		/// <summary>
		/// The condition's <see cref="Softing.OPCToolbox.Client.AeConditionState.Quality">quality</see> has changed.
		/// </summary>
		QUALITY = 0x0008,

		/// <summary>
		/// The severity level has changed.
		/// </summary>
		SEVERITY = 0x0010,

		/// <summary>
		/// The condition has transitioned into a new sub-condition.
		/// </summary>
		SUBCONDITION = 0x0020,

		/// <summary>
		/// The event message has changed (compared to prior event notifications related to this condition).
		/// </summary>
		MESSAGE = 0x0040,

		/// <summary>
		/// One or more event attributes have changed (compared to prior event notifications related to this condition).
		/// </summary>
		ATTRIBUTE = 0x0080,

		/// <summary>
		/// The condition’s acknowledgment data has changed.
		/// </summary>
		ACK_DATA = 0x8000
	} //	end EnumConditionChange

	/// <summary>
	/// Describes the standard property IDs.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumPropertyId"]/doc/*'
	/// />
	public enum EnumPropertyId : int
	{
		/// <summary>
		/// Item canonical datatype (VT I2).
		/// </summary>
		ITEM_CANONICAL_DATATYPE = 0x0001,

		/// <summary>
		/// Item value (variant).
		/// </summary>
		ITEM_VALUE = 0x0002,

		/// <summary>
		/// Item quality (VT I2).
		/// </summary>
		ITEM_QUALITY = 0x0003,

		/// <summary>
		/// Item timestamp (VT DATE).
		/// </summary>
		ITEM_TIMESTAMP = 0x0004,

		/// <summary>
		/// Item access rights (VT I4).
		/// </summary>
		ITEM_ACCESS_RIGHTS = 0x0005,

		/// <summary>
		/// Server scan rate in milliseconds(VT R4).
		/// </summary>
		SERVER_SCAN_RATE = 0x0006,

		/// <summary>
		/// Item EU type (VT I4).
		/// </summary>
		ITEM_EU_TYPE = 0x0007,

		/// <summary>
		/// Item EUInfo (VT BSTR|VT ARRAY). If Item 7 "Item EU Type" is "Enumerated" - EUInfo will contain a SAFEARRAY of strings
		/// (Example: "OPEN", "CLOSE", "IN", "TRANSIT" etc.) corresponding to numeric values (0,1,2,etc.)
		/// </summary>
		ITEM_EU_INFO = 0x0008,

		/// <summary>
		/// Reserved for future OPC use (9-99 dec).
		/// </summary>
		RESERVED_RANGE_1 = 0x0009,

		/// <summary>
		/// EU Units (VT BSTR). E.g. "DEGC" or "GALLONS"
		/// </summary>
		EU_UNITS = 0x0064,

		/// <summary>
		/// Item description (VT BSTR). E.g. "Evaporator 6 Coolant Temp"
		/// </summary>
		ITEM_DESCRIPTION = 0x0065,

		/// <summary>
		/// High EU (VT R8). Present only for 'analog' data. This represents the highest value likely to be obtained in normal
		/// operation and is intended for such use as automatically scaling a bargraph display e.g. 1400.0
		/// </summary>
		HIGH_EU = 0x0066,

		/// <summary>
		/// Low EU (VT R8). Present only for 'analog' data. This represents the lowest value likely to be obtained in normal
		/// operation and is intended for such use as automatically scaling a bargraph display e.g. 1400.0
		/// </summary>
		LOW_EU = 0x0067,

		/// <summary>
		/// High Instrument Range (VT R8). Present only for 'analog' data. This represents the highest value that can be returned
		/// by the instrument e.g. 9999.9
		/// </summary>
		HIGH_INSTRUMENT_RANGE = 0x0068,

		/// <summary>
		/// Low Instrument Range (VT R8). Present only for 'analog' data. This represents the lowest value that can be returned
		/// by the instrument e.g. -9999.9
		/// </summary>
		LOW_INSTRUMENT_RANGE = 0x0069,

		/// <summary>
		/// Contact close label (VT BSTR). Present only for 'discrete' data. This represents a string to be associated with
		/// this contact when it is in the closed (non-zero) state e.g. "RUN", "CLOSE", "ENABLE", "SAFE", etc.
		/// </summary>
		CONTACT_CLOSE_LABEL = 0x006A,

		/// <summary>
		/// Contact open label (VT BSTR). Present only for 'discrete' data. This represents a string to be associated with
		/// this contact when it is in the opened (non-zero) state e.g. "RUN", "CLOSE", "ENABLE", "SAFE", etc.
		/// </summary>
		CONTACT_OPEN_LABEL = 0x006B,

		/// <summary>
		/// Item timezone (VT I4). The difference in minutes between the items UTC timestamp and the local time in which
		/// the item value was obtained.
		/// </summary>
		ITEM_TIMEZONE = 0x006C,

		/// <summary>
		/// Reserved for future OPC use. (109-199 dec)
		/// </summary>
		RESERVED_RANGE_2 = 0x006D,

		/// <summary>
		/// Condition status (VT BSTR). The current alarm or condition status associated with the item e.g. "NORMAL", "ACTIVE"
		/// </summary>
		CONDITION_STATUS = 0x012C,

		/// <summary>
		/// Alarm quick help (VT BSTR). A short text string providing a brief set of instructions for the operator
		/// to follow when this alarm occurs.
		/// </summary>
		ALARM_QUICK_HELP = 0x012D,

		/// <summary>
		/// Alarm area list (VT BSTR|VT ARRAY). An array of strings indicating the plant or alarm areas which include this item ID
		/// </summary>
		ALARM_AREA_LIST = 0x012E,

		/// <summary>
		/// Primary alarm area (VT BSTR). A string indicating the primary plant or alrm area including this item ID
		/// </summary>
		PRIMARY_ALARM_AREA = 0x012F,

		/// <summary>
		/// Condition logic (VT BSTR). An aribtrary string describing the test being performed e.g. "High Limit exceeded"
		/// </summary>
		CONDITION_LOGIC = 0x0130,

		/// <summary>
		/// Limit exceeded (VT BSTR). For mulitstate alarms, the condition exceeded e.g. HIHI, HI, LO, LOLO
		/// </summary>
		LIMIT_EXCEEDED = 0x0131,

		/// <summary>
		/// Deadband (VT R8).
		/// </summary>
		DEADBAND = 0x0132,

		/// <summary>
		/// HiHi Limit (VT R8).
		/// </summary>
		HIHI_LIMIT = 0x0133,

		/// <summary>
		/// Hi Limit (VT R8).
		/// </summary>
		HI_LIMIT = 0x0134,

		/// <summary>
		/// Lo Limit (VT R8).
		/// </summary>
		LO_LIMIT = 0x0135,

		/// <summary>
		/// LoLo Limit (VT R8).
		/// </summary>
		LOLO_LIMIT = 0x0136,

		/// <summary>
		/// Rate of Change Limit (VT R8).
		/// </summary>
		RATE_OF_CHANGE_LIMIT = 0x0137,

		/// <summary>
		/// Deviation Limit (VT R8).
		/// </summary>
		DEVIATION_LIMIT = 0x0138,

		/// <summary>
		/// Sound file (VT BSTR). e.g. C:\MEDIA\FIC101.WAV, or MID
		/// </summary>
		SOUND_FILE = 0x0139,

		/// <summary>
		/// Reserved for future OPC Alarms and Events use (314-399 dec). Additional IDs may be added without revising the interface ID.
		/// </summary>
		RESERVED_RANGE_3 = 0x0140,

		/// <summary>
		/// Reserved for future OPC use (400-4999). Additional IDs may be added without revising the interface ID.
		/// </summary>
		RESERVED_RANGE_4 = 0x0190
		//  Vendor specific propery range 5000... Properties must have values 5000 or greater. They do not need to be sequential.
		//  The datatypes must be compatable with the VARIANT.
	} //	end EnumPropertyId

	/// <summary>
	/// Describes the server possible states.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumServerState"]/doc/*'
	/// />
	public enum EnumServerState : int
	{
		/// <summary>
		/// The server is running normally
		/// </summary>
		RUNNING = 1,

		/// <summary>
		/// The server is no longer functioning
		/// </summary>
		FAILED = 2,

		/// <summary>
		/// The server is running but no configuration information is loaded
		/// </summary>
		NOCONFIG = 3,

		/// <summary>
		/// The server is suspended via some vendor specific method therefore cannot receive or send data
		/// </summary>
		SUSPENDED = 4,

		/// <summary>
		/// The server is running in Test Mode
		/// </summary>
		TEST = 5,

		/// <summary>
		/// The server is running properly but is having difficulty accessing data from its data sources.
		/// </summary>
		COMM_FAULT = 6
	} //	end EnumServerState
} //	end namespace Softing.OPCToolbox
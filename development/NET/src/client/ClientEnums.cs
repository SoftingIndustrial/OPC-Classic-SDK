using System;
using System.Runtime.InteropServices;

namespace Softing.OPCToolbox.Client
{
	/// <summary>
	/// Describes the possible ways of executing operations.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//enum[@name="EnumExecutionType"]/doc/*'
	/// />
	public enum EnumExecutionType : byte
	{
		/// <summary>
		/// The execution is synchronous.
		/// </summary>
		SYNCHRONOUS = 0x01,

		/// <summary>
		/// The execution is asynchronous.
		/// </summary>
		ASYNCHRONOUS = 0x02
	} //	end EnumExecutionType

	/// <summary>
	/// Describes the current state of the connection with a server.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//enum[@name="EnumObjectState"]/doc/*'
	/// />
	public enum EnumObjectState : byte
	{
		/// <summary>
		/// There is no connection between a server and a client.
		/// </summary>
		DISCONNECTED = 0x00,

		/// <summary>
		/// Client is connected to the server but they don't comunicate with each other.
		/// </summary>
		CONNECTED = 0x01,

		/// <summary>
		/// Client and server communicate with each other via a callback connection.
		/// </summary>
		ACTIVATED = 0x03
	}

	/// <summary>
	/// Describes the data retrieval mode of DA Client subscription/group
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//enum[@name="EnumDataRetrievalMode"]/doc/*'
	/// />
	public enum EnumDataRetrievalMode : byte
	{
		/// <summary>
		/// Asynchronous advises
		/// </summary>
		EnumDataRetrievalMode_ASYNCADVISE = 0x01,

		/// <summary>
		/// Synchronous reads
		/// </summary>
		EnumDataRetrievalMode_SYNCREAD = 0x02,

		/// <summary>
		/// Polled refresh
		/// </summary>
		EnumDataRetrievalMode_POLLEDREFRESH = 0x04

	}

	/// <summary>
	/// Describes possible OPC objects type.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//enum[@name="EnumObjectType"]/doc/*'
	/// />
	internal enum EnumObjectType : uint
	{
		/// <summary>
		/// Data Access Session.
		/// </summary>
		DASESSION = 0x80000000,

		///<summary>
		///Data Access Subscription.
		///</summary>
		DASUBSCRIPTION = 0x40000000,

		///<summary>
		///Data Access Item.
		///</summary>
		DAITEM = 0x20000000,

		/// <summary>
		/// Alarms and Events Server.
		/// </summary>
		AESESSION = 0x08000000,

		/// <summary>
		/// Alarms and Events subscription.
		/// </summary>
		AESUBSCRIPTION = 0x04000000
	}

	/// <summary>
	/// Describes the possible attributes of an OPC object.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//enum[@name="EnumObjectAttribute"]/doc/*'
	/// />
	public enum EnumObjectAttribute : uint
	{
		/// <summary>
		/// All the attributes available for a <see cref="DaSession"/> object.
		/// </summary>
		DASESSION_ALL = (EnumObjectType.DASESSION | 0x0000FFFF),

		/// <summary>
		/// The name of the client for a <see cref="DaSession"/> object.
		/// </summary>
		DASESSION_CLIENTNAME = (EnumObjectType.DASESSION | 0x00000001),

		/// <summary>
		/// The locale identifier of a <see cref="DaSession"/> object.
		/// </summary>
		DASESSION_LCID = (EnumObjectType.DASESSION | 0x00000002),

		/// <summary>
		/// The url of a <see cref="DaSession"/> object.
		/// </summary>
		DASESSION_URL = (EnumObjectType.DASESSION | 0x00000004),

		/// <summary>
		/// The forced OPC specification of a <see cref="DaSession"/> object.
		/// </summary>
		DASESSION_FORCEDOPCSPECIFICATION = (EnumObjectType.DASESSION | 0x00000008),

		/// <summary>
		/// The supported Opc specification of a <see cref="DaSession"/> object.
		/// </summary>
		DASESSION_SUPPORTEDOPCSPECIFICATION = (EnumObjectType.DASESSION | 0x00000010),

		/// <summary>
		/// The HTTP request timeout of a <see cref="DaSession"/> object.
		/// </summary>
		DASESSION_REQUESTTIMEOUT = (EnumObjectType.DASESSION | 0x00000020),

		/// <summary>
		/// Whether to perform initial read or not when a <see cref="DaSubscription"/> is activated.
		/// </summary>
		DASESSION_PERFORMINITIALREAD = (EnumObjectType.DASESSION | 0x00000040),

		/// <summary>
		/// The execution context in which a Data Access server is to be run.
		/// </summary>
		DASESSION_CLASSCONTEXT = (EnumObjectType.DASESSION | 0x8000),

		/// <summary>
		/// All attributes of a <see cref="DaSubscription"/> object.
		/// </summary>
		DASUBSCRIPTION_ALL = (EnumObjectType.DASUBSCRIPTION | 0xFFFF),

		/// <summary>
		/// The name of a <see cref="DaSubscription"/> object.
		/// </summary>
		DASUBSCRIPTION_NAME = (EnumObjectType.DASUBSCRIPTION | 0x0001),

		/// <summary>
		/// The update rate of a <see cref="DaSubscription"/> object.
		/// </summary>
		DASUBSCRIPTION_UPDATERATE = (EnumObjectType.DASUBSCRIPTION | 0x0002),

		/// <summary>
		/// The deadband of a <see cref="DaSubscription"/> object.
		/// </summary>
		DASUBSCRIPTION_DEADBAND = (EnumObjectType.DASUBSCRIPTION | 0x0004),

		/// <summary>
		/// The timebias attribute  of a Data Access Subscription.
		/// </summary>
		DASUBSCRIPTION_TIMEBIAS = (EnumObjectType.DASUBSCRIPTION | 0x0008),

		/// <summary>
		/// The locale ID of a <see cref="DaSubscription"/> object.
		/// </summary>
		DASUBSCRIPTION_LCID = (EnumObjectType.DASUBSCRIPTION | 0x0010),

		/// <summary>
		/// The keep alive time of a <see cref="DaSubscription"/> object.
		/// </summary>
		DASUBSCRIPTION_KEEPALIVETIME = (EnumObjectType.DASUBSCRIPTION | 0x0020),

		/// <summary>
		/// The data retrieval mode attribute of a <see cref="DaSubscription"/> pbject.
		/// </summary>
		DASUBSCRIPTION_DATARETRIEVAL = (EnumObjectType.DASUBSCRIPTION | 0x0040),

		/// <summary>
		/// All the attributes of a <see cref="DaItem"/> object.
		/// </summary>
		DAITEM_ALL = (EnumObjectType.DAITEM | 0xFFFF),

		/// <summary>
		/// The requested datatype of a <see cref="DaItem"/> object.
		/// </summary>
		DAITEM_REQUESTED_DATATYPE = (EnumObjectType.DAITEM | 0x0001),

		/// <summary>
		/// The deadband of a <see cref="DaItem"/> object.
		/// </summary>
		DAITEM_DEADBAND = (EnumObjectType.DAITEM | 0x0002),

		/// <summary>
		/// The native datatype of a <see cref="DaItem"/> object.
		/// </summary>
		DAITEM_NATIVE_DATATYPE = (EnumObjectType.DAITEM | 0x0004),

		/// <summary>
		/// The access rights of a <see cref="DaItem"/> object.
		/// </summary>
		DAITEM_ACCESSRIGHTS = (EnumObjectType.DAITEM | 0x0008),

		/// <summary>
		/// The Engineering Unit Type of a <see cref="DaItem"/> object.
		/// </summary>
		DAITEM_EUTYPE = (EnumObjectType.DAITEM | 0x0010),

		/// <summary>
		/// The Engineering Unit Information of a <see cref="DaItem"/> object.
		/// </summary>
		DAITEM_EUINFO = (EnumObjectType.DAITEM | 0x0020),

		/// <summary>
		/// The identifier of a <see cref="DaItem"/> object.
		/// </summary>
		DAITEM_ID = (EnumObjectType.DAITEM | 0x0040),

		/// <summary>
		/// The path of a <see cref="DaItem"/> object.
		/// </summary>
		DAITEM_PATH = (EnumObjectType.DAITEM | 0x0080),

		/// <summary>
		/// All attributes of an <see cref="AeSession"/> object.
		/// </summary>
		AESESSION_ALL = (EnumObjectType.AESESSION | 0xFFFF),

		/// <summary>
		/// The client name of an <see cref="AeSession"/> object.
		/// </summary>
		AESESSION_CLIENTNAME = (EnumObjectType.AESESSION | 0x0001),

		/// <summary>
		/// The locale ID of an <see cref="AeSession"/> object.
		/// </summary>
		AESESSION_LCID = (EnumObjectType.AESESSION | 0x0002),

		/// <summary>
		/// The url of an <see cref="AeSession"/> object.
		/// </summary>
		AESESSION_URL = (EnumObjectType.AESESSION | 0x0004),

		/// <summary>
		/// The execution context in which the Alarms and Events Server is to be run.
		/// </summary>
		AESESSION_CLASSCONTEXT = (EnumObjectType.AESESSION | 0x8000),

		/// <summary>
		/// All the attributes of an <see cref="AeSubscription"/> object.
		/// </summary>
		AESUBSCRIPTION_ALL = (EnumObjectType.AESUBSCRIPTION | 0xFFFF),

		/// <summary>
		/// The buffertime of an <see cref="AeSubscription"/> object.
		/// </summary>
		AESUBSCRIPTION_BUFFERTIME = (EnumObjectType.AESUBSCRIPTION | 0x0001),

		/// <summary>
		/// The maxsize of an <see cref="AeSubscription"/> object.
		/// </summary>
		AESUBSCRIPTION_MAXSIZE = (EnumObjectType.AESUBSCRIPTION | 0x0002),

		/// <summary>
		/// The filters of an <see cref="AeSubscription"/> object.
		/// </summary>
		AESUBSCRIPTION_FILTER = (EnumObjectType.AESUBSCRIPTION | 0x0004),

		/// <summary>
		/// The returned attributtes of an <see cref="AeSubscription"/> object.
		/// </summary>
		AESUBSCRIPTION_RETURNED_ATTRIBUTES = (EnumObjectType.AESUBSCRIPTION | 0x0008)
	}

	/// <summary>
	/// Describes the data a server browser can provide.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//enum[@name="EnumServerBrowserData"]/doc/*'
	/// />
	public enum EnumServerBrowserData : byte
	{
		/// <summary>
		/// A 128-bit value which uniquely identifies a server.
		/// </summary>
		CLASSID = 0x01,

		/// <summary>
		/// The server's ProgID.
		/// </summary>
		PROGID = 0x02,

		/// <summary>
		/// The server's version independent program ID.
		/// </summary>
		PROGIDVERSIONINDEPENDENT = 0x04,

		/// <summary>
		/// The server's description.
		/// </summary>
		DESCRIPTION = 0X08,

		/// <summary>
		/// The server's url.
		/// </summary>
		URL = 0x10,

		/// <summary>
		/// The Opc specifications which the server supports.
		/// </summary>
		OPCSPECIFICATIONS = 0x20,

		/// <summary>
		/// All the above data.
		/// </summary>
		ALL = 0xFF
	}

	/// <summary>
	/// Describes the type of an <see cref="AddressSpaceElement">AddressSpaceElement</see>.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//enum[@name="EnumAddressSpaceElementType"]/doc/*'
	/// />
	public enum EnumAddressSpaceElementType : byte
	{
		/// <summary>
		///  A node in the hierarchical structure of a server's address space. It provides a logical grouping for the process variables.
		/// </summary>
		BRANCH = 0x01,

		/// <summary>
		/// A process variable which can be accessed via an OPC server.
		/// </summary>
		LEAF = 0x02,

		/// <summary>
		/// A process variable which can be accessed via an OPC Data Access Server.
		/// </summary>
		DA_ITEM = 0x04,

		/// <summary>
		/// All the types enumerated above.
		/// </summary>
		ALL = 0xFF
	}

	/// <summary>
	/// Describes the data a property can provide.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//enum[@name="EnumPropertyData"]/doc/*'
	/// />
	public enum EnumPropertyData
	{
		/// <summary>
		/// No data.
		/// </summary>
		NONE = 0x00,

		/// <summary>
		/// The value the property contains.
		/// </summary>
		VALUE = 0x01,

		/// <summary>
		/// The ID of the item the property belongs to.
		/// </summary>
		ITEMID = 0x02,

		/// <summary>
		/// All the data that is related to the property.
		/// </summary>
		ALL = 0xFF
	}

	/// <summary>
	/// Describes the way the update for the OPC object attributtes is made.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//enum[@name="EnumUpdateAttributesBehavior"]/doc/*'
	/// />
	public enum EnumUpdateAttributesBehavior
	{
		/// <summary>
		/// The attributes update is made automatically,without the user to request it explicitely.
		/// </summary>
		IMPLICIT_UPDATE = 0x01,

		/// <summary>
		/// The attributes update is made at the user request.
		/// </summary>
		EXPLICIT_UPDATE = 0x02
	}

	/// <summary>
	/// <para>Describes the filters on which is based the assignment of events to an <see cref=" Softing.OPCToolbox.Client.AeSubscription">Alarms and Events Subscription.</see></para>
	/// <para> Filters consists of information that characterizes the events(event type,category,severity) and of information describing the position of the events
	/// in the eventarea(area,source).</para>
	/// <remarks><note>Before the client can use the filter,it has to query for the filter criteria available.</note></remarks>
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//enum[@name="EnumFilterBy"]/doc/*'
	/// />
	public enum EnumFilterBy
	{
		/// <summary>
		/// The server supports filtering based on the values for the event type(condition related,simple,tracking related);
		/// </summary>
		EVENT = 0x01,

		/// <summary>
		/// The server supports filtering based on the values for the event category;
		/// </summary>
		CATEGORY = 0x02,

		/// <summary>
		/// The server supports filtering based on the values for the severity;
		/// </summary>
		SEVERITY = 0x04,

		/// <summary>
		/// The server supports filtering based on the values for the area(nodes in the eventarea);
		/// </summary>
		AREA = 0x08,

		/// <summary>
		/// The server supports filtering based on the values for the sources(leaves in the eventarea);
		/// </summary>
		SOURCE = 0x10
	}

	/// <summary>
	/// Describes the values that are used in activation calls to indicate the execution contexts in which a server is to be run.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//enum[@name="EnumClassContext"]/doc/*'
	/// />
	public enum EnumClassContext
	{
		/// <summary>
		/// Not a COM object.
		/// </summary>
		NOT_COM = 0x00000000,

		/// <summary>
		/// The code that creates and manages objects of this class is a DLL that runs in the same process
		/// as the caller of the function specifying the class context.
		/// </summary>
		INPROC_SERVER = 0x00000001,

		/// <summary>
		/// The EXE code that creates and manages objects of this class runs on same machine but is loaded in a separate process space.
		/// </summary>
		LOCAL_SERVER = 0x00000004,

		/// <summary>
		/// A remote machine context.
		/// </summary>
		REMOTE_SERVER = 0x00000010,

		/// <summary>
		/// A COM server.
		/// </summary>
		SERVER = (INPROC_SERVER | LOCAL_SERVER | REMOTE_SERVER)
	}
}
using System;

namespace Softing.OPCToolbox.Server
{
	/// <summary>
	/// Describes the possible Address space type
	/// </summary>
	/// <include
	///  file='TBNS.doc.xml'
	///  path='//enum[@name="EnumAddressSpaceType"]/doc/*'
	/// />
	public enum EnumAddressSpaceType : byte
	{
		/// <summary>The Address space is based on objects</summary>
		OBJECT_BASED = 0x01,

		/// <summary>The address space consists of strings</summary>
		STRING_BASED = 0x02,

		/// <summary>The Namespace consists of objects and strings</summary>
		OBJECT_STRING_BASED = 0x03
	} //	end EnumAddressSpaceType

	/// <summary>
	/// The type of the address space element. exclusively used in the OTB interogations.
	/// </summary>
	internal enum EnumAddressSpaceElementType : byte
	{
		DA = 0x01,
		AE = 0x02
	} //	EnumAddressSpaceElementType

	/// <summary>
	/// The type of Application.
	/// </summary>
	/// <include
	///  file='TBNS.doc.xml'
	///  path='//enum[@name="EnumApplicationType"]/doc/*'
	/// />
	public enum EnumApplicationType : byte
	{
		/// <summary>The OPC server application is an executable.</summary>
		EXECUTABLE = 1,

		/// <summary>The OPC server application is an library.</summary>
		LIBRARY = 0
	} //	end ApplicationType

	/// <summary>
	/// Special cache expiry settings for the string-based (syntax-based) namespace object data.
	/// If not using below enums, a value in milliseconds is expected.
	/// </summary>
	/// <include
	///  file='TBNS.doc.xml'
	///  path='//enum[@name="StringBasedExpiryTimeout"]/doc/*'
	/// />
	public enum StringBasedExpiryTimeout: ulong
	{
		/// <summary>The internal OPC DA Server cache for string-based object data is disabled.
		/// Every time the CORE needs any data it will invoke vendor-application callbacks.</summary>
		DISABLED = 0xFFFFFFFFUL,

		/// <summary>The internal OPC DA Server cache for string-based object data is set to never expire.
		/// Data provided once will never be requested again.</summary>
		INFINITE = 0x0UL
	} //	end StringBasedExpiryTimeout

	/// <summary>
	/// Possible the Input/Outpus modes
	/// </summary>
	/// <include
	///  file='TBNS.doc.xml'
	///  path='//enum[@name="EnumIoMode"]/doc/*'
	/// />
	public enum EnumIoMode : byte
	{
		/// <summary> No IO </summary>
		NONE = 0x00,

		/// <summary> Client driven </summary>
		POLL = 0x01,

		/// <summary> Server reports changes </summary>
		REPORT = 0x02,

		/// <summary> Polled own cache </summary>
		POLL_OWNCACHE = 0x11,

		/// <summary> Server reports changes for cyclic data</summary>
		REPORT_CYCLIC = 0x22
	} //	end IoMode

	/// <summary>
	/// The type (direction) of the transaction
	/// </summary>
	/// <include
	///  file='TBNS.doc.xml'
	///  path='//enum[@name="EnumTransactionType"]/doc/*'
	/// />
	public enum EnumTransactionType : byte
	{
		/// <summary>The client asked for some value(s)</summary>
		READ = 0x01,

		/// <summary>The client attempts to write some value(s)</summary>
		WRITE = 0x02
	} //	end TransactionType

	/// <summary>
	/// Request's possible states
	/// </summary>
	/// <include
	///  file='TBNS.doc.xml'
	///  path='//enum[@name="EnumRequestState"]/doc/*'
	/// />
	public enum EnumRequestState : byte
	{
		/// <summary> The request has just been created</summary>
		CREATED = 0x01,

		/// <summary> The request is about to be processed</summary>
		PENDING = 0x03,

		/// <summary> The request is being processed by the vendor application</summary>
		INPROGRESS = 0x04,

		/// <summary> The request was processed and is now completed</summary>
		COMPLETED = 0x05
	} //	end EnumRequestState

	/// <summary>
	/// Type of a connected session
	/// </summary>
	/// <include
	///  file='TBNS.doc.xml'
	///  path='//enum[@name="EnumSessionType"]/doc/*'
	/// />
	public enum EnumSessionType : byte
	{
		/// <summary>Data Access session </summary>
		DA = 0x01,

		/// <summary>Data Access over XML</summary>
		XMLDA = 0x06,

		/// <summary>Internal XML-DA subscription</summary>
		XMLSUBSCRIPTIONS = 0x02,

		/// <summary>Alarms and events session </summary>
		AE = 0x08
	} //	end EnumSessionType

	/// <summary>
	/// Defines the <see cref="DaSession"/> possible states
	/// </summary>
	/// <include
	///  file='TBNS.doc.xml'
	///  path='//enum[@name="EnumSessionState"]/doc/*'
	/// />
	internal enum EnumSessionState : short
	{
		/// <summary>
		/// Session created by the OPC client
		/// </summary>
		CREATE = 0,

		/// <summary>
		/// session is loging on
		/// </summary>
		LOGON = 1,

		/// <summary>
		/// session is logging off
		/// </summary>
		LOGOFF = 2,

		/// <summary>
		/// session information is changed
		/// </summary>
		MODIFY = 3,

		/// <summary>
		/// session is destroyed by the OPC client
		/// </summary>
		DESTROY = -1
	} //	end EnumSessionState
} //	end namespace Softing.OPCToolbox.Server
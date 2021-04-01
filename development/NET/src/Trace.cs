using System;
using System.Runtime.InteropServices;
using System.Collections;
using Softing.OPCToolbox.OTB;

namespace Softing.OPCToolbox
{

	#region //	Public Delegates
	//--------------------------

	/// <summary>
	/// <para>This will be triggered from within the Toolkit core when a trace message needs to be delivered to the custom application.
	/// The user now may use this Toolkit core trace messages in the propertaty logging mechanism.
	/// </para>
	/// <para><b>Unmanaged C++</b>
	/// <line>Delegate is not available for Unmanaged C++.</line>
	/// </para>
	/// <para><b>Unmanaged C++</b>
	/// <line>A function pointer definition is used for the calback receive purpose in C++</line>
	/// <line>typedef void (API_CALL *TraceEventHandler)(</line>
	///	<line>IN tstring aTraceString,</line>
	///	<line>IN unsigned short aLevel,</line>
	///	<line>IN unsigned long aMask,</line>
	///	<line>IN tstring anObjId,</line>
	///	<line>IN tstring  aText);</line>
	/// </para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.Application.TraceOutput"/>
	///  event of an <see cref="Softing.OPCToolbox.Client.Application"/>.</para>
	/// </summary>
	/// <param name="traceString">A string related to the trace.</param>
	/// <param name="traceLevel">The level of tracing.</param>
	/// <param name="traceGroup">The group to be traced.</param>
	/// <param name="objectId">An identifier for the traced object.</param>
	/// <param name="message">The tracing message.</param>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//delegate[@name="TraceEventHandler"]/doc/*'
	/// />
	public delegate void TraceEventHandler(
		string traceString,
		EnumTraceLevel traceLevel,
		EnumTraceGroup traceGroup,
		string objectId,
		string message);

	//-
	#endregion

	#region //Enumerations
	//--------------------

	/// <summary>
	///Describes what is traced.
	/// <br></br>The groups can be set for each of the four <see cref="EnumTraceLevel">trace levels</see> independently.
	/// <br></br>The groups may be OR'ed with each other.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumTraceGroup"]/doc/*'
	/// />
	public enum EnumTraceGroup : uint
	{
		/// <summary>
		/// Everything is traced.
		/// </summary>
		ALL = 0xFFFF000F,

		/// <summary>
		/// Nothing is traced.
		/// </summary>
		NOTHING = 0x00000000,

		/// <summary>
		/// Everything of the user definable part of the trace group is traced.
		/// </summary>
		USER = 0xFF000000,

		///<summary>
		/// Client internal trace.
		/// </summary>
		OPCCLIENT = 0x0000000C,

		/// <summary>
		/// Client internal method trace.
		/// </summary>
		OPCCLIENTCALL = 0x00000004,

		/// <summary>
		/// Client internal callbacks trace.
		/// </summary>
		OPCCLIENTADVISE = 0x00000008,

		/// <summary>
		/// Server internal trace.
		/// </summary>
		OPCSERVER = 0x00000003,

		/// <summary>
		/// Server internal method trace.
		/// </summary>
		OPCSERVERCALL = 0x00000001,

		/// <summary>
		/// Server internal callbacks trace.
		/// </summary>
		OPCSERVERADVISE = 0x00000002,

		/// <summary>
		/// OPC Toolkit Client trace.
		/// </summary>
		CLIENT = 0x00800000,

		/// <summary>
		/// OPC Toolkit Server trace.
		/// </summary>
		SERVER = 0x00400000,

		/// <summary>
		/// Licence concerned trace output is enabled.
		/// </summary>
		LICENSE = 0x00200000,

		/// <summary>
		/// Internal trace.
		/// </summary>
		OTSIN = 0x00010000,

		/// <summary>
		/// Internal trace.
		/// </summary>
		OTSOUT = 0x00020000,

		/// <summary>
		/// Internal trace.
		/// </summary>
		OTCIN = 0x00040000,

		/// <summary>
		/// Internal trace.
		/// </summary>
		OTCOUT = 0x00080000,

		/// <summary>
		/// User group part 1 to be OR'ed with other groups.
		/// </summary>
		USER1 = 0x80000000,

		/// <summary>
		/// User group part 2 to be OR'ed with other groups.
		/// </summary>
		USER2 = 0x40000000,

		/// <summary>
		/// User group part 3 to be OR'ed with other groups.
		/// </summary>
		USER3 = 0x20000000,

		/// <summary>
		/// User group part 4 to be OR'ed with other groups.
		/// </summary>
		USER4 = 0x10000000,

		/// <summary>
		/// User group part 5 to be OR'ed with other groups.
		/// </summary>
		USER5 = 0x08000000,

		/// <summary>
		/// User group part 6 to be OR'ed with other groups.
		/// </summary>
		USER6 = 0x04000000,

		/// <summary>
		/// User group part 7 to be OR'ed with other groups.
		/// </summary>
		USER7 = 0x02000000,

		/// <summary>
		/// User group part 8 to be OR'ed with other groups.
		/// </summary>
		USER8 = 0x01000000,
	}

	/// <summary>
	/// Describes the four trace levels.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumTraceLevel"]/doc/*'
	/// />
	public enum EnumTraceLevel : byte
	{
		/// <summary>
		/// Error level.
		/// </summary>
		ERR = 0,

		/// <summary>
		/// Warning level.
		/// </summary>
		WRN = 1,

		/// <summary>
		/// Informative level.
		/// </summary>
		INF = 2,

		/// <summary>
		/// Debugging level.
		/// </summary>
		DEB = 3
	} //	end EnumTraceLevel

	/// <summary>
	/// Describes the way the tracing is made.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//enum[@name="EnumTraceData"]/doc/*'
	/// />
	internal enum EnumTraceData : uint
	{
		/// <summary>
		/// Traces everything.
		/// </summary>
		TRACEDATA_ALL = 0x000001FF,

		/// <summary>
		/// A mask for tracing a level.
		/// </summary>
		TRACEDATA_LEVEL = 0x0000000F,

		/// <summary>
		/// Traces the error level.
		/// </summary>
		TRACEDATA_LEVEL_ERR = 0x00000008,

		/// <summary>
		/// Traces the warning level.
		/// </summary>
		TRACEDATA_LEVEL_WRN = 0x00000004,

		/// <summary>
		/// Traces the information  level.
		/// </summary>
		TRACEDATA_LEVEL_INF = 0x00000002,

		/// <summary>
		/// Traces the debug level.
		/// </summary>
		TRACEDATA_LEVEL_DEB = 0x00000001,

		/// <summary>
		/// A mask for tracing data to a file.
		/// </summary>
		TRACEDATA_FILE = 0x000000F0,

		/// <summary>
		/// The trace to a file is enabled.
		/// </summary>
		TRACEDATA_FILE_ENABLE = 0x00000010,

		/// <summary>
		/// The name of the first file where the trace is made.
		/// </summary>
		TRACEDATA_FILE_NAME = 0x00000020,

		/// <summary>
		/// The name of the first file where the trace is made.
		/// </summary>
		TRACEDATA_MAX_BACKUPS = 0x00000040,

		/// <summary>
		/// The size of the file where the trace is made.
		/// </summary>
		TRACEDATA_FILE_SIZE = 0x00000080,

		/// <summary>
		/// The TK API version.
		/// </summary>
		TRACEDATA_API_VERSION = 0x00000100
}; //	end EnumTraceData

	//-
	#endregion

	/// <summary>
	/// Provides a set of methods and properties that helps tracing the code execution.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//class[@name="Trace"]/doc/*'
	/// />
	public class Trace
	{
		#region //	Internal Ctor
		//------------------------

		/// <summary>
		/// Internal costrucotr (Singleton)
		/// </summary>
		internal Trace()
		{
		}

		//-
		#endregion

		#region //	Internal Attributes
		//----------------------------

		/// <summary>
		/// Internal sturucture used for marshalling to OTB library
		/// </summary>
		internal static OTCTraceData m_traceData = new OTCTraceData();

		//-
		#endregion

		#region //	Public Static Properties
		//----------------------------------

		/// <summary>
		/// Gets or sets the warning level for the output traced.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="Trace"]/property[@name="WarningLevelMask"]/doc/*'
		/// />
		public static uint WarningLevelMask
		{
			get { return m_traceData.m_warningLevelMask; }
			set { m_traceData.m_warningLevelMask = value; }
		} //	end WarningLevelMask

		/// <summary>
		/// Gets or sets the debug level for the output traced.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="Trace"]/property[@name="DebugLevelMask"]/doc/*'
		/// />
		public static uint DebugLevelMask
		{
			get { return m_traceData.m_debugLevelMask; }
			set { m_traceData.m_debugLevelMask = value; }
		} //	end DebugLevelMask

		/// <summary>
		/// Gets or sets the error level for the output traced.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="Trace"]/property[@name="ErrorLevelMask"]/doc/*'
		/// />
		public static uint ErrorLevelMask
		{
			get { return m_traceData.m_errorLevelMask; }
			set { m_traceData.m_errorLevelMask = value; }
		} //	end ErrorLevelMask

		/// <summary>
		/// Gets or sets the informative level for the output traced.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="Trace"]/property[@name="InfoLevelMask"]/doc/*'
		/// />
		public static uint InfoLevelMask
		{
			get { return m_traceData.m_infoLevelMask; }
			set { m_traceData.m_infoLevelMask = value; }
		} //	end InfoLevelMask

		/// <summary>
		/// Gets or sets whether the tracing to file is enabled.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="Trace"]/property[@name="EnableTraceToFile"]/doc/*'
		/// />
		public static bool EnableTraceToFile
		{
			get { return m_traceData.m_enableTraceToFile != 0; }
			set
			{
				m_traceData.m_enableTraceToFile = Convert.ToByte(value);
				SetTraceOptions(EnumTraceData.TRACEDATA_ALL);
			}
		} //	end EnableTraceToFile

		/// <summary>
		/// Gets or sets the maximum size of the file in which the tracing messages will be written.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="Trace"]/property[@name="FileMaxSize"]/doc/*'
		/// />
		public static uint FileMaxSize
		{
			get { return m_traceData.m_maxFileSize; }
			set { m_traceData.m_maxFileSize = value; }
		} //	end FileMaxSize

		/// <summary>
		/// Gets or sets the name of the file in which the tracing messages will be written.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="Trace"]/property[@name="FileName"]/doc/*'
		/// />
		public static string FileName
		{
			get { return m_traceData.m_fileName; }
			set { m_traceData.m_fileName = value; }
		} //	end FileName

		///<summary>Gets or sets the maximum number of backups made for the trace file.</summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="Trace"]/property[@name="MaximumBackups"]/doc/*'
		/// />
		public static uint MaximumBackups
		{
			get { return m_traceData.m_maxBackups; }
			set { m_traceData.m_maxBackups = value; }
		} //	end MaximumBackups
		//-
		#endregion

		#region //	Internal Static Methods
		//------------------------

		/// <summary>
		/// Forwards to the OTB the trace settings call.
		/// </summary>
		/// <param name="aTraceDataMask">
		/// The trace settings mask to be changed within OTB.
		/// </param>
		internal static void SetTraceOptions(EnumTraceData aTraceDataMask)
		{
#if !NET20 && !NET35 && !NET40 && !NET45 && !NET46
			string osVersion = Environment.OSVersion.Version.ToString();
#else
			string osVersion = Environment.OSVersion.VersionString;
#endif

			string apiData = "TBN " + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString() +
				", built with .NET " + Environment.Version.ToString() + ", running on " + osVersion;

			if (IntPtr.Size == 8)
			{
				apiData += " x64";
			}
			else
			{
				apiData += " x86";
			}

			m_traceData.m_apiVersion = apiData;

			OTBFunctions.OTEnableTracing((uint)aTraceDataMask, m_traceData);
		} //	end SetTraceOptions

		//-
		#endregion

		#region //	Public Static Methods
		//------------------------

		/// <summary>
		/// Writes a given message, of a given level, assigned to a given group in the current trace file.
		/// </summary>
		/// <param name="level">The level of tracing. It can be one of the EnumTraceLevel values.</param>
		/// <param name="mask">The group to be traced. It can be one of the EnumTraceGroup values.</param>
		/// <param name="objectID">The identifier for the traced object. (it could be the name of the method
		/// where the trace message is inserted).</param>
		/// <param name="message">The tracing message. It is a free text and denotes the reason of the trace.</param>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="Trace"]/method[@name="WriteLine_1"]/doc/*'
		/// />
		public static void WriteLine(
			byte level,
			uint mask,
			string objectID,
			string message)
		{
			string FormattedString = message;
			OTBFunctions.OTTrace(level, mask, objectID, FormattedString);
		} //	end WriteLine

		/// <summary>
		/// Writes a given message, of a given level, assigned to a given group in the current trace file.
		/// The line is written based on a given text format.
		/// </summary>
		/// <param name="level">The level of tracing. It can be one of the EnumTraceLevel values.</param>
		/// <param name="mask">The group to be traced. It can be one of the EnumTraceGroup values.</param>
		/// <param name="objectID">The identifier for the traced object. (it could be the name of the method
		/// where the trace message is inserted).</param>
		/// <param name="message">The tracing message. It is a free text and denotes the reason of the trace.</param>
		/// <param name="args">Parameter list for formatted message string.</param>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="Trace"]/method[@name="WriteLine_2"]/doc/*'
		/// />
		public static void WriteLine(
			byte level,
			uint mask,
			string objectID,
			string message,
			params object[] args)
		{
			string FormattedString = String.Format(message, args);
			OTBFunctions.OTTrace(level, mask, objectID, FormattedString);
		} //	end WriteLine

		//-
		#endregion
	} //	end Trace
} //	end namespace
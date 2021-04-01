using System;
using System.Runtime.InteropServices;
using System.Collections;

namespace Softing.OPCToolbox.Client
{
	/// <summary>
	///Represents status information for a server.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="ServerStatus"]/doc/*'
	/// />
	public class ServerStatus
	{
		#region //Protected Attributes
		//---------------------------

		///<summary>
		///The current status of the server.
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/field[@name="m_state"]/doc/*'
		/// />
		protected readonly uint m_state = 0;

		///<summary>
		///A DateTime object that represents the time the server was started.
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/field[@name="m_startTime"]/doc/*'
		/// />
		protected readonly DateTime m_startTime = new DateTime(1601, 01, 01, 0, 0, 0, 0);

		///<summary>
		///A DateTime object that represents the time in the server of the method call.
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/field[@name="m_currentTime"]/doc/*'
		/// />
		protected readonly DateTime m_currentTime = new DateTime(1601, 01, 01, 0, 0, 0, 0);

		///<summary>
		///A string that represents additional information about the server.
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/field[@name="m_vendorInfo"]/doc/*'
		/// />
		protected readonly String m_vendorInfo = String.Empty;

		///<summary>
		///A string that represents the version of the server software.
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/field[@name="m_productVersion"]/doc/*'
		/// />
		protected readonly String m_productVersion = String.Empty;

		///<summary>
		///An array of strings that represent all the culture names supported by the server.
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/field[@name="m_supportedLcIds"]/doc/*'
		/// />
		protected readonly string[] m_supportedLcIds = null;

		///<summary>
		///A DateTime object that represents the time when the last data were sent to the client by callback.
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/field[@name="m_lastUpdateTime"]/doc/*'
		/// />
		protected readonly DateTime m_lastUpdateTime = new DateTime(1601, 01, 01, 0, 0, 0, 0);

		///<summary>
		///The number of subscriptions created which clients have created in the server.
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/field[@name="m_groupCount"]/doc/*'
		/// />
		protected readonly uint m_groupCount = 0;

		///<summary>
		///The processor performance consumed by the server.
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/field[@name="m_bandwidth"]/doc/*'
		/// />
		protected readonly uint m_bandwidth = 0;

		///<summary>
		///A string that represents a textual description of the current server state.
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/field[@name="m_statusInfo"]/doc/*'
		/// />
		protected readonly String m_statusInfo = String.Empty;

		//-
		#endregion

		#region //Public Constructor
		//--------------------------

		/// <summary>
		/// Initializes a new instance of <see cref="Softing.OPCToolbox.Client.ServerStatus">ServerStatus</see> class.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/constructor[@name="ServerStatus"]/doc/*'
		/// />
		public ServerStatus()
		{
		}

		/// <summary>
		///Initializes a new instance of <see cref="Softing.OPCToolbox.Client.ServerStatus">ServerStatus</see> class with the value indicated by a server state,a server start time, a server current time,
		///a vendor information, a product version, a list with supported locale identifiers, a server last update time, a group count,
		///a bandwidth and a status information.
		/// </summary>
		/// <param name="state">An <see cref="Softing.OPCToolbox.Client.EnumObjectState "> EnumObjectState </see> that specifies the server's current state.</param>
		/// <param name="startTime">A value that contains the time the server was started.</param>
		/// <param name="currentTime">A value that indicates the time in the server when the method call was made.</param>
		/// <param name="vendorInfo">An information about the manufacturer.</param>
		/// <param name="productVersion">A version status of the server.</param>
		/// <param name="supportedLcIds">A list with all the languages supported by the server.</param>
		/// <param name="lastUpdateTime">A value that indicates the time when the last data were sent to the client by callback.</param>
		/// <param name="groupCount">A total number of <see cref="Softing.OPCToolbox.Client.DaSubscription">DaSubscription</see>
		/// or <see cref="Softing.OPCToolbox.Client.AeSubscription">AeSubscription</see> objects which the clients have created in the server.</param>
		/// <param name="bandwidth">A value that indicates the processor performance consumed by the server.</param>
		/// <param name="statusInfo">A text that gives more details about the status of the server.</param>
		internal ServerStatus(uint state, DateTime startTime,
		                      DateTime currentTime, String vendorInfo,
		                      String productVersion, string[] supportedLcIds,
		                      DateTime lastUpdateTime, uint groupCount,
		                      uint bandwidth, String statusInfo)
		{
			m_state = state;
			m_startTime = startTime;
			m_currentTime = currentTime;
			m_vendorInfo = vendorInfo;
			m_productVersion = productVersion;
			m_supportedLcIds = supportedLcIds;
			m_lastUpdateTime = lastUpdateTime;
			m_groupCount = groupCount;
			m_bandwidth = bandwidth;
			m_statusInfo = statusInfo;
		}

		//-
		#endregion

		#region //Public Properties
		//-------------------------

		/// <summary>
		/// Gets additional information about the server. (i.e. the name of the manufacturer)
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/property[@name="VendorInfo"]/doc/*'
		/// />
		public String VendorInfo
		{
			get { return m_vendorInfo; }
		}

		/// <summary>
		/// Gets the current status of a server. (i.e. running)
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/property[@name="State"]/doc/*'
		/// />
		public uint State
		{
			get { return m_state; }
		}

		/// <summary>
		/// Gets the time when the server was started.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/property[@name="StartTime"]/doc/*'
		/// />
		public DateTime StartTime
		{
			get { return m_startTime; }
		}

		/// <summary>
		/// Gets the time in the server when the <b>GetStatus</b> method of a <see cref="DaSession"/> object or of an <see cref="AeSession"/> object was called.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/property[@name="CurrentTime"]/doc/*'
		/// />
		public DateTime CurrentTime
		{
			get { return m_currentTime; }
		}

		/// <summary>
		/// Gets the version of the server software.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/property[@name="ProductVersion"]/doc/*'
		/// />
		public String ProductVersion
		{
			get { return m_productVersion; }
		}

		/// <summary>
		/// Gets the list with all the culture names supported by the server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/property[@name="SupportedLcIds"]/doc/*'
		/// />
		public string[] SupportedLcIds
		{
			get { return m_supportedLcIds; }
		}

		/// <summary>
		///  Gets the time when the last data were sent to the client by callback.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/property[@name="LastUpdateTime"]/doc/*'
		/// />
		public DateTime LastUpdateTime
		{
			get { return m_lastUpdateTime; }
		}

		/// <summary>
		/// Gets the total number of <see cref="DaSubscription"/>
		/// or <see cref="AeSubscription"/> objects
		/// which the  clients have created in the server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/property[@name="GroupCount"]/doc/*'
		/// />
		public uint GroupCount
		{
			get { return m_groupCount; }
		}

		/// <summary>
		/// Gets the processor performance consumed by the server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/property[@name="Bandwidth"]/doc/*'
		/// />
		public uint Bandwidth
		{
			get { return m_bandwidth; }
		}

		/// <summary>
		///Gets a textual description of the current server state.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerStatus"]/property[@name="StatusInfo"]/doc/*'
		/// />
		public String StatusInfo
		{
			get { return m_statusInfo; }
		}

		//-
		#endregion
	}
}
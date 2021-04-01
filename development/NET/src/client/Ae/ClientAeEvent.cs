using System;
using System.Collections;

namespace Softing.OPCToolbox.Client
{
	/// <summary>
	/// Represents a simple event.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="AeEvent"]/doc/*'
	/// />
	public class AeEvent
	{
		#region //Public Constructors
		//---------------------------

		/// <summary>
		/// Initializes a new instance of the Event class.
		/// </summary>
		/// <param name="anEventType"></param>
		/// <param name="anEventCategory"></param>
		/// <param name="aSeverity"></param>
		/// <param name="aSourcePath"></param>
		/// <param name="aMessage"></param>
		/// <param name="anOccurenceTime"></param>
		/// <param name="someEventAttributes"></param>
		/// <param name="anActorId"></param>
		internal AeEvent(EnumEventType anEventType, uint anEventCategory,
		                 uint aSeverity, string aSourcePath,
		                 string aMessage, DateTime anOccurenceTime,
		                 ArrayList someEventAttributes, string anActorId)
		{
			m_eventType = anEventType;
			m_eventCategory = anEventCategory;
			m_severity = aSeverity;
			m_sourcePath = aSourcePath;
			m_message = aMessage;
			m_occurenceTime = anOccurenceTime;
			m_eventAttributes = someEventAttributes;
			m_actorId = anActorId;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="AeEvent"/> class.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/constructor[@name="AeEvent"]/doc/*'
		/// />
		public AeEvent()
		{
		}

		//-
		#endregion

		#region //Protected Attributes
		//--------------------------

		/// <summary>
		/// The event type.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/field[@name="m_eventType"]/doc/*'
		/// />
		protected EnumEventType m_eventType = (EnumEventType) 7;

		/// <summary>
		/// The category to which the event belongs.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/field[@name="m_eventCategory"]/doc/*'
		/// />
		protected uint m_eventCategory = 0;

		/// <summary>
		/// The urgency of the event. This may be a value in the range of 1 – 1000.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/field[@name="m_severity"]/doc/*'
		/// />
		protected uint m_severity = 0;

		/// <summary>
		/// A reference path to the object which generated the event notification.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/field[@name="m_sourcePath"]/doc/*'
		/// />
		protected string m_sourcePath = String.Empty;

		/// <summary>
		/// A textual description of the event.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/field[@name="m_message"]/doc/*'
		/// />
		protected string m_message = String.Empty;

		/// <summary>
		/// The time when the event occured.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/field[@name="m_occurenceTime"]/doc/*'
		/// />
		protected DateTime m_occurenceTime = new DateTime(1601, 01, 01, 0, 0, 0, 0);

		/// <summary>
		/// A list with additional vendor-specific attributes(attributes defined by the server implementer) to be sent with the event notification.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/field[@name="m_eventAttributes"]/doc/*'
		/// />
		protected ArrayList m_eventAttributes;

		/// <summary>
		/// An identifier for the client which initiated an action resulting in an event.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/field[@name="m_actorId"]/doc/*'
		/// />
		protected string m_actorId = String.Empty;

		//-
		#endregion

		#region //Public Properties
		//-------------------------

		/// <summary>
		/// Gets or sets the event type.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/property[@name="EventType"]/doc/*'
		/// />
		public EnumEventType EventType
		{
			get { return m_eventType; }
			set { m_eventType = value; }
		}

		/// <summary>
		/// Gets or sets the identifier of the category to which the event belongs.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/property[@name="Category"]/doc/*'
		/// />
		public uint Category
		{
			get { return m_eventCategory; }
			set { m_eventCategory = value; }
		}

		/// <summary>
		/// Gets or sets the urgency of the event. This may be a value in the range of 1 – 1000.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/property[@name="Severity"]/doc/*'
		/// />
		public uint Severity
		{
			get { return m_severity; }
			set { m_severity = value; }
		}

		/// <summary>
		/// Gets or sets a reference path to the object which generated the event notification.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/property[@name="SourcePath"]/doc/*'
		/// />
		public string SourcePath
		{
			get { return m_sourcePath; }
			set { m_sourcePath = value; }
		}

		/// <summary>
		/// A textual description for the event.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/property[@name="Message"]/doc/*'
		/// />
		public string Message
		{
			get { return m_message; }
			set { m_message = value; }
		}

		/// <summary>
		/// Gets or sets the time when the event occurred.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/property[@name="OccurenceTime"]/doc/*'
		/// />
		public DateTime OccurenceTime
		{
			get { return m_occurenceTime; }
			set { m_occurenceTime = value; }
		}

		/// <summary>
		/// Gets or sets a list of vendor-specific attributes to be sent with the event notification.
		/// These attributes may be sent in addition to the standard attributes.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/property[@name="Attributes"]/doc/*'
		/// />
		public ArrayList Attributes
		{
			get { return m_eventAttributes; }
			set { m_eventAttributes = value; }
		}

		/// <summary>
		/// Gets or sets the identifier of the client which initiated an action resulting in an event.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeEvent"]/property[@name="ActorId"]/doc/*'
		/// />
		public string ActorId
		{
			get { return m_actorId; }
			set { m_actorId = value; }
		}

		//-
		#endregion
	}
}
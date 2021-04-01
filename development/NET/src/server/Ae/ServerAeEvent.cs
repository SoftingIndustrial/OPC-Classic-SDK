using System;
using System.Collections;
using System.Runtime.InteropServices;
using Softing.OPCToolbox.OTB;
using Softing.OPCToolbox.Server;

namespace Softing.OPCToolbox.Server
{
	/// <summary>
	/// An object instance of the AeEvent class represents an event which is to be sent to the OPC Clients.
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="AeEvent"]/doc/*'
	///	/>
	public class AeEvent
	{
		#region // Public Constructors
		//----------------------------

		/// <summary>
		/// Builds an event. The Type, Source and CategoryId are mandatory attributes
		/// </summary>
		/// <param name="anEventType">
		/// the type of this event
		/// </param>
		/// <param name="aSource">
		/// address space source generating the event
		/// </param>
		/// <param name="aCategoryId">
		/// event category Id
		/// </param>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			ctor[@name="AeEvent"]/doc/*'
		///	/>
		public AeEvent(
			EnumEventType anEventType,
			string aSource,
			uint aCategoryId)
		{
			this.m_type = anEventType;
			this.m_source = aSource;
			this.m_categoryId = aCategoryId;
		} //	end ctr


		/// <summary>
		/// releases the attributes variant array
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			dtor[@name="AeEvent"]/doc/*'
		///	/>
		~AeEvent()
		{
			m_attributeValueList.Clear();
		} //	end dtor

		//-
		#endregion

		#region	//	Public Properties
		//---------------------------

		/// <summary>
		/// The type of the event
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			property[@name="Type"]/doc/*'
		///	/>
		public EnumEventType Type
		{
			get { return this.m_type; }
			set { this.m_type = value; }
		} //	end Type


		/// <summary>
		/// The address space source that generates the event
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			property[@name="Source"]/doc/*'
		///	/>
		public string Source
		{
			get { return this.m_source; }
			set { this.m_source = value; }
		} //	end Source


		/// <summary>
		/// ID if the event catogory
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			property[@name="CategoryId"]/doc/*'
		///	/>
		public uint CategoryId
		{
			get { return this.m_categoryId; }
			set { this.m_categoryId = value; }
		} //	end Category


		/// <summary>
		/// Severity value of the event
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			property[@name="Severity"]/doc/*'
		///	/>
		public uint Severity
		{
			get { return this.m_severity; }
			set { this.m_severity = value; }
		} //	end Severity


		/// <summary>
		/// String representing the actor - user that generated the event
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			property[@name="ActorId"]/doc/*'
		///	/>
		public string ActorId
		{
			get { return this.m_actorId; }
			set { this.m_actorId = value; }
		} //	end ActorId


		/// <summary>
		/// Event's string message to be passed to the client
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			property[@name="Message"]/doc/*'
		///	/>
		public string Message
		{
			get { return this.m_message; }
			set { this.m_message = value; }
		} //	end Message


		/// <summary>
		/// The attributes variants owned the event and defined accordingly to the Event's category ID
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			property[@name="AttributeValueList"]/doc/*'
		///	/>
		public ArrayList AttributeValueList
		{
			get { return this.m_attributeValueList; }
			set { this.m_attributeValueList = value; }
		} //	end AttributeValueList


		/// <summary>
		/// The timestamp of the event, attached by the server
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			property[@name="OccurenceTime"]/doc/*'
		///	/>
		public DateTime OccurenceTime
		{
			get { return m_occurenceTime; }
			set { m_occurenceTime = value; }
		} //	end OccurenceTime


		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// Fires the current event to the customer
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			method[@name="Fire"]/doc/*'
		///	/>
		public int Fire()
		{
			ArrayList thisArray = new ArrayList();
			thisArray.Add(this);
			return Application.Instance.FireEvents(thisArray);
		} //	end Fire


		/// <summary>
		/// Creates an OTS event structure
		/// </summary>
		/// <returns></returns>
		internal OTSEventData CreateOTSEventData()
		{
			OTSEventData eventData = new OTSEventData();

			eventData.m_eventType = (uint) this.m_type;
			eventData.m_eventCategory = this.m_categoryId;
			eventData.m_severity = this.m_severity;
			eventData.m_sourcePath = Marshal.StringToCoTaskMemUni(this.m_source);
			eventData.m_message = Marshal.StringToCoTaskMemUni(this.m_message);
			eventData.m_occurenceTime = new OTDateTime(this.m_occurenceTime);
			eventData.m_actorID = Marshal.StringToCoTaskMemUni(this.m_actorId);
			eventData.m_eventAttrCount = (uint) this.m_attributeValueList.Count;

			if (eventData.m_eventAttrCount > 0)
			{
				eventData.m_pEventAttrs = Marshal.AllocCoTaskMem(((int) (eventData.m_eventAttrCount*ValueQT.VARIANT_SIZE)));
				IntPtr currentPointer = eventData.m_pEventAttrs;
				for (int index = 0; index < m_attributeValueList.Count; index++)
				{
					Marshal.GetNativeVariantForObject(m_attributeValueList[index], currentPointer);
					currentPointer = new IntPtr(currentPointer.ToInt64() + ValueQT.VARIANT_SIZE);
				} //	end for
			} //	end if

			return eventData;
		} //	end CreateOTSEventData


		//-
		#endregion

		#region	//	Private Attributes
		//------------------------------

		/// <summary>
		/// The type of the event
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			attribute[@name="m_type"]/doc/*'
		///	/>
		protected EnumEventType m_type = EnumEventType.SIMPLE;

		/// <summary>
		/// The address space source that generates the event
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			attribute[@name="m_source"]/doc/*'
		///	/>
		protected string m_source = string.Empty;

		/// <summary>
		/// ID if the event catogory
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			attribute[@name="m_categoryId"]/doc/*'
		///	/>
		protected uint m_categoryId = 0;

		/// <summary>
		/// Severity value of the event
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			attribute[@name="m_severity"]/doc/*'
		///	/>
		protected uint m_severity = 0;

		/// <summary>
		/// String representing the Actor - user that generated the event
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			attribute[@name="m_actorId"]/doc/*'
		///	/>
		protected string m_actorId = string.Empty;

		/// <summary>
		/// Event's string message to be passed to the client
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			attribute[@name="m_message"]/doc/*'
		///	/>
		protected string m_message = string.Empty;

		/// <summary>
		/// The attributes variants owned the event and defined accordingly to the Event's category ID
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			attribute[@name="m_attributeValueList"]/doc/*'
		///	/>
		protected ArrayList m_attributeValueList = new ArrayList();

		/// <summary>
		/// The timestamp of the event, attached by the server
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeEvent"]/
		///			attribute[@name="m_occurenceTime"]/doc/*'
		///	/>
		protected DateTime m_occurenceTime = DateTime.Now;

		//-
		#endregion
	} //	end class AeEvent
} //	end Softing.OPCToolbox.Server
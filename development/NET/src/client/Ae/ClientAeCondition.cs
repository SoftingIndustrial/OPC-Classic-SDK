using System;
using System.Collections;

namespace Softing.OPCToolbox.Client
{
	/// <summary>
	/// Represents a condition-related event.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="AeCondition"]/doc/*'
	/// />
	public class AeCondition : AeEvent
	{
		#region //Public Constructors
		//-----------------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="AeCondition"/> class.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/constructor[@name="AeCondition"]/doc/*'
		/// />
		public AeCondition()
		{
		} //	end Constructor
		internal AeCondition(EnumEventType anEventType, uint anEventCategory,
		                     uint aSeverity, string aSourcePath,
		                     string aMessage, DateTime anOccurenceTime,
		                     ArrayList someEventAttributes, string anActorId,
		                     EnumConditionChange aChangeMask, EnumConditionState aNewState, bool ackRequired,
		                     EnumQuality aQuality, string aConditionName,
		                     string aSubConditionName, DateTime anActiveTime,
		                     uint aCookie)
		{
			m_eventType = anEventType;
			m_eventCategory = anEventCategory;
			m_severity = aSeverity;
			m_sourcePath = aSourcePath;
			m_message = aMessage;
			m_occurenceTime = anOccurenceTime;
			m_eventAttributes = someEventAttributes;
			m_actorId = anActorId;
			m_changeMask = aChangeMask;
			m_newState = aNewState;
			m_ackRequired = ackRequired;
			m_quality = aQuality;
			m_conditionName = aConditionName;
			m_subConditionName = aSubConditionName;
			m_activeTime = anActiveTime;
			m_cookie = aCookie;
		}

		//-
		#endregion

		#region //Protected Attributes
		//--------------------------

		/// <summary>
		/// Indicates to the client which properties of the condition have changed.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/field[@name="m_changeMask"]/doc/*'
		/// />
		protected EnumConditionChange m_changeMask = 0;

		/// <summary>
		/// The new state of the condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/field[@name="m_newState"]/doc/*'
		/// />
		protected EnumConditionState m_newState = 0;

		/// <summary>
		/// Indicates if an acknowledgement is or not required.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/field[@name="m_ackRequired"]/doc/*'
		/// />
		protected bool m_ackRequired = false;

		/// <summary>
		/// The quality of the underlying data items upon which this condition is based.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/field[@name="m_quality"]/doc/*'
		/// />
		protected EnumQuality m_quality = EnumQuality.BAD_WAITING_FOR_INITIAL_DATA;

		/// <summary>
		/// The name of the <see cref="AeCondition"/> object associated with the event.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/field[@name="m_conditionName"]/doc/*'
		/// />
		protected string m_conditionName = String.Empty;

		/// <summary>
		/// The name of the currently active subcondition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/field[@name="m_subConditionName"]/doc/*'
		/// />
		protected string m_subConditionName = String.Empty;

		/// <summary>
		/// The time that the condition become <see cref="EnumConditionState">active</see>.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/field[@name="m_activeTime"]/doc/*'
		/// />
		protected DateTime m_activeTime = new DateTime(1601, 01, 01, 0, 0, 0, 0);

		/// <summary>
		/// A defined cookie associated with the event notification.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/field[@name="m_cookie"]/doc/*'
		/// />
		protected uint m_cookie = 0;

		/// <summary>
		/// The <see cref="AeSession"/> object that sent the notification about the occurence of the condition-related event.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/field[@name="m_aeSession"]/doc/*'
		/// />
		protected AeSession m_aeSession = null;

		//-
		#endregion

		#region //Public Properties
		//-------------------------

		/// <summary>
		/// Gets or sets the <see cref="AeSession"/> to whose eventarea the condition-related event belongs.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/property[@name="AeSession"]/doc/*'
		/// />
		public AeSession AeSession
		{
			get { return m_aeSession; }
			set { m_aeSession = value; }
		}

		/// <summary>
		/// Gets or sets which properties of the condition have changed, to have caused the server to send the event notification.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/property[@name="ChangeMask"]/doc/*'
		/// />
		public EnumConditionChange ChangeMask
		{
			get { return m_changeMask; }
			set { m_changeMask = value; }
		}

		/// <summary>
		/// Gets or sets the new state of the condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/property[@name="State"]/doc/*'
		/// />
		public EnumConditionState State
		{
			get { return m_newState; }
			set { m_newState = value; }
		}

		/// <summary>
		/// Gets or sets whether an acknowledgement is or not required.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/property[@name="AckRequired"]/doc/*'
		/// />
		public bool AckRequired
		{
			get { return m_ackRequired; }
			set { m_ackRequired = value; }
		}

		/// <summary>
		/// Gets or sets the quality of the underlying data items upon which this condition is based.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/property[@name="Quality"]/doc/*'
		/// />
		public EnumQuality Quality
		{
			get { return m_quality; }
			set { m_quality = value; }
		}

		/// <summary>
		/// Gets or sets the name of the condition associated with the event.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/property[@name="ConditionName"]/doc/*'
		/// />
		public string ConditionName
		{
			get { return m_conditionName; }
			set { m_conditionName = value; }
		}

		/// <summary>
		/// Gets or sets the name of the currently active subcondition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/property[@name="SubConditionName"]/doc/*'
		/// />
		public string SubConditionName
		{
			get { return m_subConditionName; }
			set { m_subConditionName = value; }
		}

		/// <summary>
		/// Gets or sets the time when the condition becomes <see cref="EnumConditionState">active</see>.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/property[@name="ActiveTime"]/doc/*'
		/// />
		public DateTime ActiveTime
		{
			get { return m_activeTime; }
			set { m_activeTime = value; }
		}

		/// <summary>
		/// Gets or sets a defined cookie associated with the event notification. This value is used by the client when acknowledging the condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/property[@name="Cookie"]/doc/*'
		/// />
		public uint Cookie
		{
			get { return m_cookie; }
			set { m_cookie = value; }
		}

		//-
		#endregion

		#region //Public Methods
		//------------------------

		/// <summary>
		/// Acknowledges the current condition.
		/// </summary>
		/// <param name="ackId">An identifier for the client who last acknowledged the condition.</param>
		/// <param name="ackComment">A comment string passed in by the client who acknowledged the condition.</param>
		/// <param name="executionOptions">Specifies the modality of execution for acknowledging the condition.</param>
		/// <returns>The result of acknowledging the condition.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/method[@name="Acknowledge"]/doc/*'
		/// />
		public virtual int Acknowledge(string ackId, string ackComment, ExecutionOptions executionOptions)
		{
			int[] results = new int[] {-1};
			int res = (int) EnumResultCode.E_FAIL;

			if (this.AeSession == null)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeCondition.Acknowledge",
					"The Session property of the Condition cannot be null! Set the property to a value before calling Acknowledge!");
				return res;
			}
			try
			{
				res = this.AeSession.AcknowledgeAeConditions(
					ackId,
					ackComment,
					new AeCondition[] {this},
					out results,
					executionOptions);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeCondition.Acknowledge",
						"Acknowledging condition failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeCondition.Acknowledge",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Gives specific information on the current condition.
		/// </summary>
		/// <param name="attributes">A list with the attributes related to the condition.</param>
		/// <param name="executionOptions">Specifies the modality of execution for getting the condition's state.</param>
		/// <returns>The result of acknowledging the condition.</returns>
		/// <returns>The result of getting the conditon's state.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCondition"]/method[@name="UpdateConditionState"]/doc/*'
		/// />
		public virtual int UpdateConditionState(AeAttribute[] attributes, ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			AeConditionState conditionState;

			if (this.AeSession == null)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AE.Condition.UpdateConditionState",
					"The Session property of the Condition cannot be null! Set the property to a value before calling UpdateConditionState!");
				return res;
			}
			try
			{
				res = this.AeSession.GetAeConditionState(
					this.SourcePath,
					this.ConditionName,
					attributes,
					out conditionState,
					executionOptions);

				if (ResultCode.SUCCEEDED(res))
				{
					this.ActiveTime = conditionState.ConditionActiveTime;
					this.ActorId = conditionState.AcknowledgerId;
					this.Attributes = conditionState.EventAttributes;
				}
				else
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeCondition.UpdateConditionState",
						"Updating condition state failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeCondition.UpdateConditionState",
					exc.ToString());
			}
			return res;
		}

		//-
		#endregion
	} // end class Condition

	/// <summary>
	/// Represents an OPC Condition associated with an OPC Source.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="AeConditionState"]/doc/*'
	/// />
	public class AeConditionState
	{
		#region //Public Constructor
		//--------------------------

		///<summary>
		/// Initializes a new instance of the <see cref="AeConditionState"/> class.
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/constructor[@name="AeConditionState"]/doc/*'
		/// />
		public AeConditionState()
		{
		}

		//-
		#endregion

		#region	//Protected Attributes - like OTCConditionStateData
		//------------------------------

		/// <summary>
		/// The current state of a condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_state"]/doc/*'
		/// />
		protected EnumConditionState m_state = EnumConditionState.NO;

		/// <summary>
		/// The name of the sub-condition which is currently active.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_actSubConditionName"]/doc/*'
		/// />
		protected string m_actSubConditionName = String.Empty;

		/// <summary>
		/// An expression which defines the sub-state of the sub-condition which is currently active.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_actSubConditionDefinition"]/doc/*'
		/// />
		protected string m_actSubConditionDefinition = String.Empty;

		/// <summary>
		/// The severity of any event notifications generated on behalf of the currently active sub-condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_actSubConditionSeverity"]/doc/*'
		/// />
		protected uint m_actSubConditionSeverity = 0;

		/// <summary>
		/// A text string to be included in any event notification generated on behalf of the currently active sub-condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_actSubConditionDescription"]/doc/*'
		/// />
		protected string m_actSubConditionDescription = String.Empty;

		/// <summary>
		/// The time that condition become active.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_conditionActiveTime"]/doc/*'
		/// />
		protected DateTime m_conditionActiveTime = new DateTime(1601, 01, 01, 0, 0, 0, 0);

		/// <summary>
		/// The time of the most recent acknowledgement (of any sub-condition).
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_conditionAckTime"]/doc/*'
		/// />
		protected DateTime m_conditionAckTime = new DateTime(1601, 01, 01, 0, 0, 0, 0);

		/// <summary>
		/// The time of the most recent transition into the currently active sub-condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_subConditionActiveTime"]/doc/*'
		/// />
		protected DateTime m_subConditionActiveTime = new DateTime(1601, 01, 01, 0, 0, 0, 0);

		/// <summary>
		/// The time of the most recent transition out of the condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_conditionInactiveTime"]/doc/*'
		/// />
		protected DateTime m_conditionInactiveTime = new DateTime(1601, 01, 01, 0, 0, 0, 0);

		/// <summary>
		/// The current quality of the data value(s) upon which the condition is based.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_quality"]/doc/*'
		/// />
		protected EnumQuality m_quality = EnumQuality.BAD_WAITING_FOR_INITIAL_DATA;

		/// <summary>
		/// An identifier for the client who last acknowledged the condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_ackId"]/doc/*'
		/// />
		protected string m_ackId = String.Empty;

		/// <summary>
		/// A comment string passed in by the client who last acknowledged this condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_ackComment"]/doc/*'
		/// />
		protected string m_ackComment = String.Empty;

		/// <summary>
		/// A list with the definitions of all the sub-conditions of the current condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_pSubConditionDefinition"]/doc/*'
		/// />
		protected string[] m_pSubConditionDefinition = null;

		/// <summary>
		/// A list with the severity of all the sub-conditions of the current condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_pSubConditionSeverity"]/doc/*'
		/// />
		protected uint[] m_pSubConditionSeverity = null;

		/// <summary>
		///  A list with the description of all the sub-conditions of the current condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_pSubConditionDescription"]/doc/*'
		/// />
		protected string[] m_pSubConditionDescription = null;

		/// <summary>
		/// A list with all the names associated with the sub-conditions of the current condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_pSubConditionName"]/doc/*'
		/// />
		protected string[] m_pSubConditionName = null;

		/// <summary>
		///A list of vendor-specific attributes to be sent with the event notification.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_pEventAttrs"]/doc/*'
		/// />
		protected ArrayList m_pEventAttrs = new ArrayList(); // array with all event attributes

		/// <summary>
		/// A list with the errors occured when transmitting the attributes with the event notification.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/field[@name="m_pAttrErrors"]/doc/*'
		/// />
		protected int[] m_pAttrErrors = null;

		//-
		#endregion

		#region //Public Properties - like OTCConditionStateData
		//--------------------------

		/// <summary>
		/// Get or sets the current state of a condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="State"]/doc/*'
		/// />
		public EnumConditionState State
		{
			get { return m_state; }
			set { m_state = value; }
		}

		/// <summary>
		/// Gets or sets the name of the sub-condition which is currently active.
		/// The name of the sub-condition is unique within its associated condition.
		/// <br></br>For single-state conditions, this value is the condition name.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="ActiveSubConditionName"]/doc/*'
		/// />
		public string ActiveSubConditionName
		{
			get { return m_actSubConditionName; }
			set { m_actSubConditionName = value; }
		}

		/// <summary>
		/// Gets or sets an expression which defines the sub-state represented by the sub-condition which is currently active.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="ActiveSubConditionDefinition"]/doc/*'
		/// />
		public string ActiveSubConditionDefinition
		{
			get { return m_actSubConditionDefinition; }
			set { m_actSubConditionDefinition = value; }
		}

		/// <summary>
		/// Gets or sets the severity of any event notifications generated on behalf of the currently active sub-condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="ActiveSubConditionSeverity"]/doc/*'
		/// />
		public uint ActiveSubConditionSeverity
		{
			get { return m_actSubConditionSeverity; }
			set { m_actSubConditionSeverity = value; }
		}

		/// <summary>
		/// Gets or sets a text string to be included in any event notification generated on behalf of the currently active sub-condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="ActiveSubConditionDescription"]/doc/*'
		/// />
		public string ActiveSubConditionDescription
		{
			get { return m_actSubConditionDescription; }
			set { m_actSubConditionDescription = value; }
		}

		/// <summary>
		/// Gets or sets the time that the condition become <see cref="EnumConditionState">active</see>.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="ConditionActiveTime"]/doc/*'
		/// />
		public DateTime ConditionActiveTime
		{
			get { return m_conditionActiveTime; }
			set { m_conditionActiveTime = value; }
		}

		/// <summary>
		/// Gets or sets the time of the most recent acknowledgement (of any sub-condition).
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="ConditionAckTime"]/doc/*'
		/// />
		public DateTime ConditionAckTime
		{
			get { return m_conditionAckTime; }
			set { m_conditionAckTime = value; }
		}

		/// <summary>
		/// Gets or sets the time of the most recent transition into the currently active sub-condition.
		/// This is the time value which is specified when acknowledging the condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="SubConditionActiveTime"]/doc/*'
		/// />
		public DateTime SubConditionActiveTime
		{
			get { return m_subConditionActiveTime; }
			set { m_subConditionActiveTime = value; }
		}

		/// <summary>
		/// Gets or sets the time of the most recent transition out of the condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="ConditionInactiveTime"]/doc/*'
		/// />
		public DateTime ConditionInactiveTime
		{
			get { return m_conditionInactiveTime; }
			set { m_conditionInactiveTime = value; }
		}

		/// <summary>
		/// Gets or sets the current quality of the data value(s) upon which the condition is based.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="Quality"]/doc/*'
		/// />
		public EnumQuality Quality
		{
			get { return m_quality; }
			set { m_quality = value; }
		}

		/// <summary>
		/// Gets or sets an identifier for the client who last acknowledged the condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="AcknowledgerId"]/doc/*'
		/// />
		public string AcknowledgerId
		{
			get { return m_ackId; }
			set { m_ackId = value; }
		}

		/// <summary>
		/// Gets or sets a comment string passed in by the client who last acknowledged the condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="AcknowledgerComment"]/doc/*'
		/// />
		public string AcknowledgerComment
		{
			get { return m_ackComment; }
			set { m_ackComment = value; }
		}

		/// <summary>
		/// Gets or sets a list with the definitions of all the sub-conditions of the current condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="SubConditionsDefinitions"]/doc/*'
		/// />
		public string[] SubConditionsDefinitions
		{
			get { return m_pSubConditionDefinition; }
			set { m_pSubConditionDefinition = value; }
		}

		/// <summary>
		/// Gets or sets a list with the severity of all the sub-conditions of the current condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="SubConditionsSeverities"]/doc/*'
		/// />
		public uint[] SubConditionsSeverities
		{
			get { return m_pSubConditionSeverity; }
			set { m_pSubConditionSeverity = value; }
		}

		/// <summary>
		/// Gets or sets a list with the description of all the sub-conditions of the current condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="SubConditionsDescriptions"]/doc/*'
		/// />
		public string[] SubConditionsDescriptions
		{
			get { return m_pSubConditionDescription; }
			set { m_pSubConditionDescription = value; }
		}

		/// <summary>
		/// Gets or sets a list with all the names associated with the sub-conditions of the current condition.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="SubConditionsNames"]/doc/*'
		/// />
		public string[] SubConditionsNames
		{
			get { return m_pSubConditionName; }
			set { m_pSubConditionName = value; }
		}

		/// <summary>
		/// Gets or sets a list of vendor-specific attributes to be sent with the event notification.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="EventAttributes"]/doc/*'
		/// />
		public ArrayList EventAttributes
		{
			get { return m_pEventAttrs; }
			set { m_pEventAttrs = value; }
		}

		/// <summary>
		/// Gets or sets a list with the errors occured when transmitting the attributes with the event notification.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeConditionState"]/property[@name="AttributesErrors"]/doc/*'
		/// />
		public int[] AttributesErrors
		{
			get { return m_pAttrErrors; }
			set { m_pAttrErrors = value; }
		}

		//-
		#endregion
	} // end class ConditionState
} //	end namespace
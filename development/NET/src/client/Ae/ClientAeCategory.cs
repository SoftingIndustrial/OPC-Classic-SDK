using System;
using System.Runtime.InteropServices;
using Softing.OPCToolbox.OTB;

namespace Softing.OPCToolbox.Client
{

	#region //Public Delegates
	//-----------------------

	/// <summary>
	/// <para><b>Unmanaged C++</b> This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeCategory.QueryAeAttributesCompleted"/> event of a <see cref="AeCategory"/>.</para>
	/// </summary>
	/// <param name="category">The <see cref="AeCategory"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeCategory.QueryAeAttributesCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="categoryId">An identifier for an event category.</param>
	/// <param name="attributes">A list with the vendor-specific event attributes associated with the event category.</param>
	/// <param name="result">The result of asynchronously quering the attributes.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="QueryAeAttributesEventHandler"]/doc/*'
	/// />
	public delegate void QueryAeAttributesEventHandler(
		AeCategory category,
		uint executionContext,
		uint categoryId,
		AeAttribute[] attributes,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeCategory.QueryAeConditionNamesCompleted"/> event of a <see cref="AeCategory"/>.</para>
	/// </summary>
	/// <param name="category">The <see cref="AeCategory"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeCategory.QueryAeConditionNamesCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="categoryId">An identifier of the category for which the condition names are given.</param>
	/// <param name="conditionNames">A list with the condition names for the specified event category. </param>
	/// <param name="result">The result of asynchronously quering the condition names.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="QueryAeConditionNamesEventHandler"]/doc/*'
	/// />
	public delegate void QueryAeConditionNamesEventHandler(
		AeCategory category,
		uint executionContext,
		uint categoryId,
		string[] conditionNames,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeCategory.QueryAeSubConditionNamesCompleted"/> event of a <see cref="AeCategory"/>.</para>
	/// </summary>
	/// <param name="category">The <see cref="AeCategory"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeCategory.QueryAeSubConditionNamesCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="conditionName">A condition name, as returned by the <see cref="Softing.OPCToolbox.Client.AeCategory.QueryAeConditionNames"/> method.</param>
	/// <param name="subConditionNames">A list with the sub-condition names associated with the specified condition. </param>
	/// <param name="result">The result of asynchronously quering the sub-condition names.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="QueryAeSubConditionNamesEventHandler"]/doc/*'
	/// />
	public delegate void QueryAeSubConditionNamesEventHandler(
		AeCategory category,
		uint executionContext,
		string conditionName,
		string[] subConditionNames,
		int result);

	//-
	#endregion

	/// <summary>
	/// Represents an event category. A category defines a grouping of events supported by an OPC Event server.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="AeCategory"]/doc/*'
	/// />
	public class AeCategory
	{
		#region //Protected Attributes
		//-------------------------

		/// <summary>
		/// The type of the events that belong to the category.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/field[@name="m_eventType"]/doc/*'
		/// />
		protected EnumEventType m_eventType = (EnumEventType) 7;

		/// <summary>
		/// An identifier for the category.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/field[@name="m_Id"]/doc/*'
		/// />
		protected uint m_Id = 0;

		/// <summary>
		/// A text string that describes the category.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/field[@name="m_description"]/doc/*'
		/// />
		protected string m_description = String.Empty;

		/// <summary>
		/// An <see cref="AeSession"/> object that supports the events in the category.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/field[@name="m_aeSession"]/doc/*'
		/// />
		protected AeSession m_aeSession = null;

		//-
		#endregion

		#region //Internal Attributes
		//---------------------------

		/// <summary>
		///The condition list is filled up with information when the QueryAESubConditionNames is called.
		/// </summary>
		internal string[] m_conditionNameList = null;

		/// <summary>
		/// Needed for hashing purpose.
		/// </summary>
		private int m_index = 0;

		//-
		#endregion

		#region //Internal Static Attributes
		//--------------------------------

		/// <summary>
		/// Needed for hashing.
		/// </summary>
		internal static int CategoryIndex = 0;

		//-
		#endregion

		#region //Constructor
		//-------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="AeCategory"/> class.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/constructor[@name="AeCategory"]/doc/*'
		/// />
		public AeCategory()
		{
			m_index = ++CategoryIndex;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="AeCategory"/> class.
		/// </summary>
		/// <param name="anEventType"></param>
		/// <param name="anId"></param>
		/// <param name="aDescription"></param>
		/// <param name="aSession"></param>
		internal AeCategory(
			EnumEventType anEventType,
			uint anId,
			string aDescription,
			AeSession aSession)
		{
			m_index = ++CategoryIndex;
			m_eventType = anEventType;
			m_Id = anId;
			m_description = aDescription;
			m_aeSession = aSession;
		}

		//-
		#endregion

		#region //Public Properties
		//-------------------------

		/// <summary>
		/// Gets or sets the type of the events within the category.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/property[@name="EventType"]/doc/*'
		/// />
		public EnumEventType EventType
		{
			get { return m_eventType; }
			set { m_eventType = value; }
		}

		/// <summary>
		/// Gets or sets an identifier for the category. The identifier must be unique across the server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/property[@name="Id"]/doc/*'
		/// />
		public uint Id
		{
			get { return m_Id; }
			set { m_Id = value; }
		}

		/// <summary>
		/// Gets or sets a text string that describes the category.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/property[@name="Description"]/doc/*'
		/// />
		public string Description
		{
			get { return m_description; }
			set { m_description = value; }
		}

		/// <summary>
		/// Gets or sets the <see cref="AeSession"/> object which supports the events in the category.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/property[@name="Session"]/doc/*'
		/// />
		public AeSession Session
		{
			get { return m_aeSession; }
			set { m_aeSession = value; }
		}

		//-
		#endregion

		#region //Public Events
		//---------------------

		/// <summary>
		/// Occurs when the  vendor-specific attributes of the events within the current category are given.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="QueryAeAttributes"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="QueryAeAttributes"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/event[@name="QueryAeAttributtesCompleted"]/doc/*'
		/// />
		public event QueryAeAttributesEventHandler QueryAeAttributesCompleted;

		/// <summary>
		/// Occurs when the condition names which the event server supports for the category are given.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="QueryAeConditionNames"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="QueryAeConditionNames"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/event[@name="QueryAeConditionNamesCompleted"]/doc/*'
		/// />
		public event QueryAeConditionNamesEventHandler QueryAeConditionNamesCompleted;

		/// <summary>
		/// Occurs when the sub-condition names which are associated with a specified condition name are given.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="QueryAeSubConditionNames"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="QueryAeSubConditionNames"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/event[@name="QueryAeSubConditionNamesCompleted"]/doc/*'
		/// />
		public event QueryAeSubConditionNamesEventHandler QueryAeSubConditionNamesCompleted;

		//-
		#endregion

		#region //Public Methods
		//----------------------

		#region //New methods
		//-------------------

		/// <summary>
		///		Returns an unique code for a <see cref="AeCategory"/> object, suitable for use in data structures that associate keys with values.
		/// </summary>
		/// <returns>
		///		An unique code for the category.
		///	</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/method[@name="GetHashCode"]/doc/*'
		/// />
		public override int GetHashCode()
		{
			return this.m_index;
		}

		//-
		#endregion

		/// <summary>
		/// Gives information about the vendor-specific attributes the server can provide as part of an event
		/// notification for an event within the current event category.
		/// </summary>
		/// <param name="attributes">A list with the vendor-specific event attributes associated with the current event category.</param>
		/// <param name="executionOptions">Specifies the modality of execution for quering the attributes associated with the current event category.</param>
		/// <returns>The result of quering the attributes associated with the current event category.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/method[@name="QueryAeAttributes"]/doc/*'
		/// />
		public virtual int QueryAeAttributes(
			out AeAttribute[] attributes,
			ExecutionOptions executionOptions)
		{
			attributes = new AeAttribute[0];
			int res = (int) EnumResultCode.E_FAIL;

			if (this.Session == null)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeCategory.QueryAeAttributes",
					"The Session property of the Category cannot be null! Set the property to a value before calling QueryAEAttributes!");
				return res;
			}

			try
			{
				OTCExecutionOptions options = new OTCExecutionOptions();
				if (executionOptions != null)
				{
					options.m_executionType = (byte) executionOptions.ExecutionType;
					options.m_executionContext = (uint) executionOptions.ExecutionContext;
				}
				else
				{
					options.m_executionType = (byte) EnumExecutionType.SYNCHRONOUS;
					options.m_executionContext = 0;
				}

				uint attributesCount = 0;
				IntPtr attributeIDs = IntPtr.Zero;
				IntPtr attributeDescriptions = IntPtr.Zero;
				IntPtr attributeDataTypes = IntPtr.Zero;

				res = OTBFunctions.OTCQueryAEAttributes(
					this.Session.Handle,
					this.Id,
					out attributesCount,
					out attributeIDs,
					out attributeDescriptions,
					out attributeDataTypes,
					ref options);

				if (ResultCode.SUCCEEDED(res))
				{
					if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
					{
						attributes = new AeAttribute[attributesCount];

						int uintSize = Marshal.SizeOf(typeof (uint));
						int ptrSize = Marshal.SizeOf(typeof (IntPtr));
						int ushortSize = Marshal.SizeOf(typeof (ushort));

						for (int i = 0; i < attributesCount; i++)
						{
							uint attributeID =
								(uint) Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(attributeIDs, i*uintSize), typeof (uint));
							ushort attributeDataType =
								(ushort) Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(attributeDataTypes, i*ushortSize), typeof (ushort));
							IntPtr attributeDescription =
								(IntPtr) Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(attributeDescriptions, i*ptrSize), typeof (IntPtr));
							attributes[i] = new AeAttribute(
								attributeID,
								Marshal.PtrToStringUni(attributeDescription),
								ValueQT.GetSysType(attributeDataType));

							OTBFunctions.OTFreeMemory(attributeDescription);
						} //	end for

						OTBFunctions.OTFreeMemory(attributeIDs);
						OTBFunctions.OTFreeMemory(attributeDescriptions);
						OTBFunctions.OTFreeMemory(attributeDataTypes);
					} //	end if
				}
				else
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeCategory.QueryAeAttributes",
						"Quering AeAttributes failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeCategory.QueryAeAttributes",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Gives the specific condition names which the event server supports for the current event category.
		/// </summary>
		/// <param name="conditionNames">A list with the condition names for the current event category. </param>
		/// <param name="executionOptions">Specifies the modality of execution for quering the condition names.</param>
		/// <returns>The result of quering the condition names.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/method[@name="QueryAeConditionNames"]/doc/*'
		/// />
		public virtual int QueryAeConditionNames(
			out string[] conditionNames,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			conditionNames = new string[0];

			if (this.Session == null)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeCategory.QueryAeConditionNames",
					"The Session property of the Category cannot be null! Set the property to a value before calling QueryAEConditionNames!");
				return res;
			}
			try
			{
				OTCExecutionOptions options = new OTCExecutionOptions();
				if (executionOptions != null)
				{
					options.m_executionType = (byte) executionOptions.ExecutionType;
					options.m_executionContext = (uint) executionOptions.ExecutionContext;
				}
				else
				{
					options.m_executionType = (byte) EnumExecutionType.SYNCHRONOUS;
					options.m_executionContext = 0;
				}

				uint count = 0;
				IntPtr pConditionNames = IntPtr.Zero;

				res = OTBFunctions.OTCQueryAEConditions(
					this.Session.Handle,
					m_Id,
					out count,
					out pConditionNames,
					ref options);

				//reset the conditionNameList
				if (m_conditionNameList != null)
				{
					m_conditionNameList = null;
				} //	end if

				if (ResultCode.SUCCEEDED(res))
				{
					if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
					{
						conditionNames = new string[count];
						int ptrSize = Marshal.SizeOf(typeof (IntPtr));
						for (int i = 0; i < count; i++)
						{
							IntPtr conditionName =
								(IntPtr) Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(pConditionNames, i*ptrSize), typeof (IntPtr));
							conditionNames[i] = Marshal.PtrToStringUni(conditionName);
							OTBFunctions.OTFreeMemory(conditionName);
						} //	end for
						//refresh the condition names list
						m_conditionNameList = conditionNames;

						OTBFunctions.OTFreeMemory(pConditionNames);
					} //	end if
				}
				else
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeCategory.QueryAeConditionNames",
						" Quering condition names failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeCategory.QueryAeConditionNames",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Gives the sub-condition names which are associated with a specified condition name.
		/// The condition must be supported by the event server for the current category.
		/// </summary>
		/// <param name="conditionName">A condition name, as returned by the <see cref="QueryAeConditionNames"/> method.</param>
		/// <param name="subConditionNames">A list with the sub-condition names associated with the specified condition.</param>
		/// <param name="executionOptions">Specifies the modality of execution for quering the sub-condition names.</param>
		/// <returns>The result of quering the sub-condition names.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeCategory"]/method[@name="QueryAeSubConditionNames"]/doc/*'
		/// />
		public virtual int QueryAeSubConditionNames(
			string conditionName,
			out string[] subConditionNames,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			subConditionNames = new string[0];

			if (this.Session == null)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeCategory.QueryAeSubConditionNames",
					"The Session property of the Category cannot be null! Set the property to a value before calling QueryAeSubConditionNames!");
				return res;
			}
			try
			{
				OTCExecutionOptions options = new OTCExecutionOptions();
				if (executionOptions != null)
				{
					options.m_executionType = (byte) executionOptions.ExecutionType;
					options.m_executionContext = (uint) executionOptions.ExecutionContext;
				}
				else
				{
					options.m_executionType = (byte) EnumExecutionType.SYNCHRONOUS;
					options.m_executionContext = 0;
				}

				uint count = 0;
				IntPtr pSubConditionNames = IntPtr.Zero;

				res = OTBFunctions.OTCQueryAESubConditions(
					this.Session.Handle,
					conditionName,
					out count,
					out pSubConditionNames,
					ref options);

				if (ResultCode.SUCCEEDED(res))
				{
					if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
					{
						subConditionNames = new string[count];
						int ptrSize = Marshal.SizeOf(typeof (IntPtr));
						for (int i = 0; i < count; i++)
						{
							IntPtr subConditionName =
								(IntPtr) Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(pSubConditionNames, i*ptrSize), typeof (IntPtr));
							subConditionNames[i] = Marshal.PtrToStringUni(subConditionName);
							OTBFunctions.OTFreeMemory(subConditionName);
						} //	end for
						OTBFunctions.OTFreeMemory(pSubConditionNames);
					} //	end if
				}
				else
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeCategory.QueryAeSubConditionNames",
						" Quering sub condition names failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeCategory.QueryAeSubConditionNames",
					exc.ToString());
			}
			return res;
		}

		//-
		#endregion

		#region //Internal Methods
		//----------------------

		/// <summary>
		/// Called when the condition names which the event server supports for a specified event category are asynchronously given .
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="categoryId">A specified event category for which the condition names are given.</param>
		/// <param name="conditionNames">A list with the condition names for the specified event category. </param>
		/// <param name="result">The result of asynchronously quering the condition names.</param>
		/// <remarks><note>The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.</note></remarks>
		internal void OnQueryAeConditionNames(
			uint executionContext,
			uint categoryId,
			string[] conditionNames,
			int result)
		{
			try
			{
				if (QueryAeConditionNamesCompleted != null)
				{
					QueryAeConditionNamesCompleted(this, executionContext, categoryId, conditionNames, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AE.Category.OnQueryAEAConditionNames",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the sub-condition names which are associated with a specified condition name are asynchronously given.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="conditionName">A condition name, as returned by the <see cref="QueryAeConditionNames"/> method.</param>
		/// <param name="subConditionNames">A list with the sub-condition names associated with the specified condition. </param>
		/// <param name="result">The result of asynchronously quering the sub-condition names.</param>
		/// <remarks><note>The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.</note></remarks>
		internal void OnQueryAeSubConditionNames(
			uint executionContext,
			string conditionName,
			string[] subConditionNames,
			int result)
		{
			try
			{
				if (QueryAeSubConditionNamesCompleted != null)
				{
					QueryAeSubConditionNamesCompleted(this, executionContext, conditionName, subConditionNames, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AE.Category.OnQueryAESubConditionNames",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the vendor-specific attributes the server can provide as part of an event notification
		/// for an event within an event category is asynchronously given.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="categoryId">An identifier for an event category.</param>
		/// <param name="attributes">A list with the vendor-specific event attributes associated with the event category.</param>
		/// <param name="result">The result of asynchronously quering the attributes.</param>
		/// <remarks><note>The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.</note></remarks>
		internal void OnQueryAeAttributes(
			uint executionContext,
			uint categoryId,
			AeAttribute[] attributes,
			int result)
		{
			try
			{
				if (QueryAeAttributesCompleted != null)
				{
					QueryAeAttributesCompleted(this, executionContext, categoryId, attributes, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AE.Category.OnQueryAEAttributes",
					exc.ToString());
			}
		}

		//-
		#endregion
	}
}
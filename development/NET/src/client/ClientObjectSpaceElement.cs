using System;
using System.Runtime.InteropServices;
using System.ComponentModel.Design.Serialization;
using Softing.OPCToolbox.OTB;

namespace Softing.OPCToolbox.Client
{

	#region // Public Delegates
	//-----------------

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the
	/// <see cref=" Softing.OPCToolbox.Client.ObjectSpaceElement.StateChangeCompleted">StateChangeCompleted</see> event of an
	/// <see cref=" Softing.OPCToolbox.Client.ObjectSpaceElement">ObjectSpaceElement</see>.</para>
	/// </summary>
	/// <param name="obj">The <see cref="ObjectSpaceElement"/> object to whom the <see cref="Softing.OPCToolbox.Client.ObjectSpaceElement.StateChangeCompleted">StateChangeCompleted</see> event belongs.</param>
	/// <param name="state">The new object state.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="StateChangeEventHandler"]/doc/*'
	/// />
	public delegate void StateChangeEventHandler(ObjectSpaceElement obj, EnumObjectState state);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref=" Softing.OPCToolbox.Client.ObjectSpaceElement.GetAttributesFromServerCompleted">
	/// GetAttributesFromServerCompleted</see> event of an <see cref="ObjectSpaceElement"/> object.</para>
	/// </summary>
	/// <param name="obj">The <see cref="ObjectSpaceElement"/> object to whom the <see cref="Softing.OPCToolbox.Client.ObjectSpaceElement.GetAttributesFromServerCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for the asynchronous call.</param>
	/// <param name="whatAttributes">A list with all the object attributes to be updated.</param>
	/// <param name="results">A list with the result of the update for each object attribute.</param>
	/// <param name="result">The result of updating the object attributes from the server. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="GetAttributesFromServerEventHandler"]/doc/*'
	/// />
	public delegate void GetAttributesFromServerEventHandler(
		ObjectSpaceElement obj,
		uint executionContext,
		EnumObjectAttribute[] whatAttributes,
		int[] results,
		int result);

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref=" Softing.OPCToolbox.Client.ObjectSpaceElement.SetAttributesToServerCompleted">SetAttributesToServerCompleted</see> event
	/// of an <see cref="ObjectSpaceElement"/> object.</para>
	/// </summary>
	/// <param name="obj">The <see cref="ObjectSpaceElement"/> object to whom the <see cref="Softing.OPCToolbox.Client.ObjectSpaceElement.SetAttributesToServerCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for the asynchronous call.</param>
	/// <param name="whatAttributes">A list with all the object attributes to be updated.</param>
	/// <param name="results">A list with the result of the update for each object attribute.</param>
	/// <param name="result">The result of updating attributes to the server. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="SetAttributesToServerEventHandler"]/doc/*'
	/// />
	public delegate void SetAttributesToServerEventHandler(
		ObjectSpaceElement obj,
		uint executionContext,
		EnumObjectAttribute[] whatAttributes,
		int[] results,
		int result);

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref=" Softing.OPCToolbox.Client.ObjectSpaceElement.PerformStateTransitionCompleted">PerformStateTransitionCompleted</see> event
	/// of an <see cref="ObjectSpaceElement"/> object.</para>
	/// </summary>
	/// <param name="obj">The <see cref="ObjectSpaceElement"/> object to whom the <see cref="Softing.OPCToolbox.Client.ObjectSpaceElement.PerformStateTransitionCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for the asynchronous call.</param>
	/// <param name="result">The result of performing the state transition. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="PerformStateTransitionEventHandler"]/doc/*'
	/// />
	public delegate void PerformStateTransitionEventHandler(
		ObjectSpaceElement obj,
		uint executionContext,
		int result);

	//-

	#endregion

	/// <summary>
	/// Provides the base functionality for all the objects in the toolkit which are modelling OPC objects.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="ObjectSpaceElement"]/doc/*'
	/// />
	public class ObjectSpaceElement
	{
		#region Internal static attributes
		//--------------------------------

		internal static object m_objectIndexJanitor = new object();
		internal static int m_objectIndex = 0;

		//-
		#endregion

		#region //Public Events
		//--------------

		/// <summary>
		///Occurs when the state of the <see cref="ObjectSpaceElement"/> object has changed.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="Connect"/> or <see cref="Disconnect"/> methods.</para>
		///<para><b>Unmanaged C++</b>The method is called after the methods <see cref="Connect"/> or <see cref="Disconnect"/>
		///have been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/event[@name="StateChangeCompleted"]/doc/*'
		/// />
		public event StateChangeEventHandler StateChangeCompleted;

		/// <summary>
		/// Occurs when an <see cref="ObjectSpaceElement"/>'s attributes have been updated from the server.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="GetAttributesFromServer"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="GetAttributesFromServer"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/event[@name="GetAttributesFromServerCompleted"]/doc/*'
		/// />
		public event GetAttributesFromServerEventHandler GetAttributesFromServerCompleted;

		/// <summary>
		/// Occurs when an <see cref="ObjectSpaceElement"/>'s attributes have been updated to the server.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="SetAttributesToServer"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="SetAttributesToServer"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/event[@name="SetAttributesToServerCompleted"]/doc/*'
		/// />
		public event SetAttributesToServerEventHandler SetAttributesToServerCompleted;

		/// <summary>
		/// Occurs when performing a transition between two object states has completed.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="PerformStateTransition"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="PerformStateTransition"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/event[@name="PerformStateTransitionCompleted"]/doc/*'
		/// />
		public event PerformStateTransitionEventHandler PerformStateTransitionCompleted;

		//-
		#endregion

		#region //Public Contructor
		//------------------------

		/// <summary>
		/// Initializes a new instance of the <see cref=" Softing.OPCToolbox.Client.ObjectSpaceElement">ObjectSpaceElement</see> class.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/constructor[@name="ObjectSpaceElement"]/doc/*'
		/// />
		public ObjectSpaceElement()
		{
			//this is necessary for hashing
			lock (m_objectIndexJanitor)
			{
				this.m_index = ObjectSpaceElement.m_objectIndex++;
			}
		}

		//-
		#endregion

		#region //Public Constants
		//------------------------

		/// <summary>
		/// Indicates that the process values read from the server will be taken directly from the data source(Device).
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/constant[@name="MaxAgeDevice"]/doc/*'
		/// />
		public const uint MAX_AGE_DEVICE = 0;

		/// <summary>
		/// Indicates that the process values read from the server will be taken from the server's internal cache of the process data(Cache).
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/constant[@name="MaxAgeCache"]/doc/*'
		/// />
		public const uint MAX_AGE_CACHE = 0xFFFFFFFF;

		//-
		#endregion

		#region //Private Attributes
		//-----------------------

		private uint m_objectHandle = 0;
		private object m_handleJanitor = new object();
		private readonly int m_index;

		//-
		#endregion

		#region //Internal Properties
		//---------------------------

		internal uint Handle
		{
			get
			{
				uint objectHandle = 0;
				lock (m_handleJanitor)
				{
					objectHandle = m_objectHandle;
				} //	end lock

				return objectHandle;
			} //	end get
			set
			{
				lock (m_handleJanitor)
				{
					m_objectHandle = value;
				} //	end lock
			} //	end set
		} //	end Handle


		//-
		#endregion

		#region //Public Properties
		//-------------------------

		/// <summary>
		/// Gets whether the object is a valid one (it was successfully added to his parent in the OPC hierarchy).
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		/// path='//class[@name="ObjectSpaceElement"]/property[@name="Valid"]/doc/*'
		/// />
		public bool Valid
		{
			get
			{
				if (this.Handle != 0)
				{
					return true;
				} //	end if
				return false;
			} //	end get
		} //	end Valid


		/// <summary>
		/// Gets the current state of an <see cref="ObjectSpaceElement"/> object.
		/// </summary>
		/// <value>
		/// One of the <see cref="Softing.OPCToolbox.Client.EnumObjectState">EnumObjectState </see>values.
		/// </value>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/property[@name="CurrentState"]/doc/*'
		/// />
		public EnumObjectState CurrentState
		{
			get
			{
				byte currentState = 0;
				byte targetState = 0;
				try
				{
					int res = OTBFunctions.OTCGetState(this.Handle, out currentState, out targetState);

					if (ResultCode.FAILED(res))
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"ObjectSpaceElement.CurrentState.Get",
							"result code is " + res.ToString("X"));
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"ObjectSpaceElement.CurrentState.Get",
						exc.ToString());
				}
				return (EnumObjectState) currentState;
			} //	end get
		} //	end CurrentState


		/// <summary>
		/// Gets or sets the new state of an <see cref=" ObjectSpaceElement"/> object.
		/// </summary>
		/// <value>
		/// One of the <see cref="Softing.OPCToolbox.Client.EnumObjectState">EnumObjectState </see>values.
		/// </value>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/property[@name="TargetState"]/doc/*'
		/// />
		public EnumObjectState TargetState
		{
			get
			{
				byte currentState = 0;
				byte targetState = 0;

				try
				{
					int res = OTBFunctions.OTCGetState(this.Handle, out currentState, out targetState);

					if (ResultCode.FAILED(res))
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"ObjectSpaceElement.CurrentState.Get",
							"result code is " + res.ToString("X"));
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"ObjectSpaceElement.TargetState.Get",
						exc.ToString());
				}
				return (EnumObjectState) targetState;
			}
			set
			{
				try
				{
					byte targetState = (byte) value;
					OTBFunctions.OTCChangeTargetState(this.Handle, targetState, 1);
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"ObjectSpaceElement.TargetState.Set",
						exc.ToString());
				}
			}
		}

		//-
		#endregion

		#region //Public Methods
		//----------------------

		#region //Overriden Methods
		//-------------------

		/// <summary>
		///		Returns an unique code for a <see cref="ObjectSpaceElement"/> object, suitable for use in data structures that associate keys with values.
		/// </summary>
		/// <returns>
		///		An unique code for the object.
		///	</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/method[@name="GetHashCode"]/doc/*'
		/// />
		public override int GetHashCode()
		{
			return this.m_index;
		}

		//-
		#endregion

		/// <summary>
		/// Performs the transition from the current state to the target state.
		/// </summary>
		/// <param name="deep">Indicates if changing the target state should be propagated down to the child objects or not.</param>
		/// <param name="executionOptions">Specifies the modality of execution for state transition.</param>
		/// <returns>The result of performing the state transition.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/method[@name="PerformStateTransition"]/doc/*'
		/// />
		public virtual int PerformStateTransition(bool deep, ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
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

				if (deep == false)
				{
					// performs the state transition for the object itself
					res = OTBFunctions.OTCPerformStateTransition(this.Handle, 0, ref options);
				}
				if (deep == true)
				{
					// performs the state transition for the whole subtree of the object
					res = OTBFunctions.OTCPerformStateTransition(this.Handle, 1, ref options);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ObjectSpaceElement.PerformStateTransition",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Establishes a connection to a server.
		/// </summary>
		/// <param name="deep">Indicates if connecting to the server should be propagated down to the chlid objects or not.</param>
		/// <param name="active">Indicates if the  <see cref=" ObjectSpaceElement"/> and the server communicate with each other via callbacks. </param>
		/// <param name="executionOptions">Specifies the modality of execution for connecting to a server.</param>
		/// <returns>The result of connecting to a server.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/method[@name="Connect"]/doc/*'
		/// />
		public virtual int Connect(bool deep, bool active, ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			try
			{
				if (deep == true)
				{
					if (active == true)
					{
						OTBFunctions.OTCChangeTargetState(this.Handle, (byte) EnumObjectState.ACTIVATED, 1);
						res = PerformStateTransition(deep, executionOptions);
					}
					if (active == false)
					{
						OTBFunctions.OTCChangeTargetState(this.Handle, (byte) EnumObjectState.CONNECTED, 1);
						res = PerformStateTransition(deep, executionOptions);
					}
				}
				if (deep == false)
				{
					if (active == true)
					{
						OTBFunctions.OTCChangeTargetState(this.Handle, (byte) EnumObjectState.ACTIVATED, 0);
						res = PerformStateTransition(deep, executionOptions);
					}
					if (active == false)
					{
						OTBFunctions.OTCChangeTargetState(this.Handle, (byte) EnumObjectState.CONNECTED, 0);
						res = PerformStateTransition(deep, executionOptions);
					}
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ObjectSpaceElement.Connect",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Removes the connection to a server.
		/// </summary>
		/// <param name="executionOptions">Specifies the modality of execution for disconnecting from a server.</param>
		/// <returns>The result of disconnecting from a server.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/method[@name="Disconnect"]/doc/*'
		/// />
		public virtual int Disconnect(ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
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

				EnumObjectState m_targetState = EnumObjectState.DISCONNECTED;
				OTBFunctions.OTCChangeTargetState(this.Handle, (byte) m_targetState, 1);
				res = OTBFunctions.OTCPerformStateTransition(this.Handle, 1, ref options);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ObjectSpaceElement.Disconnect",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Updates some object attributes from the server.
		/// </summary>
		/// <param name="whatAttributes">A list with all the object attributes to be updated from the server.</param>
		/// <param name="results">A list with the result of the update for each object attribute. </param>
		/// <param name="executionOptions">Specifies the modality of execution for updating attributes from the server.</param>
		/// <returns>The result of updating attributes from the server.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/method[@name="GetAttributesFromServer"]/doc/*'
		/// />
		public virtual int GetAttributesFromServer(
			EnumObjectAttribute[] whatAttributes,
			out int[] results,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			results = null;
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

				int count = whatAttributes.Length;
				uint[] whatAttributesToChange = new uint[count];
				for (int i = 0; i < whatAttributes.Length; i++)
				{
					whatAttributesToChange[i] = (uint) whatAttributes[i];
				}
				results = new int[count];
				res = OTBFunctions.OTCUpdateAttributes(
					this.Handle,
					1,
					(uint) count,
					whatAttributesToChange,
					results,
					ref options);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ObjectSpaceElement.GetAttributesFromServer",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Updates some object attributes to the server.
		/// </summary>
		/// <param name="whatAttributes">A list with all the object attributes to be updated to the server.</param>
		/// <param name="results">A list with the result of the update for each object attribute. </param>
		/// <param name="executionOptions">Specifies the modality of execution for updating attributes to a server.</param>
		/// <returns>The result of updating attributes to a server.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ObjectSpaceElement"]/method[@name="SetAttributesToServer"]/doc/*'
		/// />
		public virtual int SetAttributesToServer(
			EnumObjectAttribute[] whatAttributes,
			out int[] results,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			results = null;
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

				int count = whatAttributes.Length;
				uint[] whatAttributesToChange = new uint[count];
				for (int i = 0; i < whatAttributes.Length; i++)
				{
					whatAttributesToChange[i] = (uint) whatAttributes[i];
				}
				results = new int[count];
				res = OTBFunctions.OTCUpdateAttributes(
					this.Handle,
					0,
					(uint) count,
					whatAttributesToChange,
					results,
					ref options);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ObjectSpaceElement.SetAttributesToServer",
					exc.ToString());
			}
			return res;
		}

		//-
		#endregion

		#region //Internal Methods
		/// <summary>
		/// Called when attributes are asynchronously updated from the server.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="whatAttributes">A list with all the object attributes to be updated.</param>
		/// <param name="results">A list with the result of the update for each object attribute.</param>
		/// <param name="result">The result of updating the object attributes from a server.</param>
		///<remarks>
		/// <note>
		///All the results should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note>
		///</remarks>
		internal void OnGetAttributesFromServer(
			uint executionContext,
			EnumObjectAttribute[] whatAttributes,
			int[] results,
			int result)
		{
			try
			{
				if (GetAttributesFromServerCompleted != null)
				{
					GetAttributesFromServerCompleted(this, executionContext, whatAttributes, results, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ObjectSpaceElement.OnGetAttributesFromServer",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when attributes are asynchronously updated to the server.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="whatAttributes">A list with all the object attributes to be updated.</param>
		/// <param name="results">A list with the result of the update for each object attribute.</param>
		/// <param name="result">The result of updating attributes to a server.</param>
		///<remarks>
		/// <note>
		///All the results should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note>
		///</remarks>
		internal void OnSetAttributesToServer(
			uint executionContext,
			EnumObjectAttribute[] whatAttributes,
			int[] results,
			int result)
		{
			try
			{
				if (SetAttributesToServerCompleted != null)
				{
					SetAttributesToServerCompleted(this, executionContext, whatAttributes, results, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ObjectSpaceElement.OnSetAttributesToServer",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the is asynchronously changed.
		/// </summary>
		/// <param name="newState">A new state.</param>
		internal void OnStateChange(EnumObjectState newState)
		{
			try
			{
				if (StateChangeCompleted != null)
					StateChangeCompleted(this, newState);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ObjectSpaceElement.OnStateChange",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when a transition between two states is performed asynchronously.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="result">The result of performing the state transition.</param>
		///<remarks>
		/// <note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note>
		///</remarks>
		internal void OnPerformStateTransition(uint executionContext, int result)
		{
			try
			{
				if (PerformStateTransitionCompleted != null)
				{
					PerformStateTransitionCompleted(this, executionContext, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ObjectSpaceElement.OnPerformStateTransition",
					exc.ToString());
			}
		}

		#endregion
	}
}
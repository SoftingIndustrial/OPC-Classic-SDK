using System;
using System.Runtime.InteropServices;
using Softing.OPCToolbox.OTB;

namespace Softing.OPCToolbox.Server
{
	/// <summary>
	/// Stores data concerning an OPC read or write request.
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="DaRequest"]/doc/*'
	///	/>
	public class DaRequest
	{
		#region //	Public Constructors
		//-----------------------------

		/// <summary>
		/// Default public constructor
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaRequest"]/
		///		ctor[@name="DaRequest"]/doc/*'
		///	/>
		public DaRequest(
			EnumTransactionType aTransactionType,
			uint aSessionHandle,
			DaAddressSpaceElement anElement,
			int aPropertyId,
			uint aRequestHandle)
		{
			m_transactionType = aTransactionType;
			m_sessionHandle = aSessionHandle;
			m_requestHandle = aRequestHandle;
			m_propertyId = aPropertyId;
			m_addressSpaceElement = anElement;
			m_transactionKey = 0;
			m_result = EnumResultCode.E_NOTIMPL;
			m_requestState = EnumRequestState.CREATED;

			if (m_addressSpaceElement != null)
			{
				OTBFunctions.OTSAckRequestElement(m_addressSpaceElement.ObjectHandle);
			}
		} //	end ctr


		/// <summary>
		/// Destructor
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaRequest"]/
		///		dtor[@name="~DaRequest"]/doc/*'
		///	/>
		~DaRequest()
		{
			if (m_addressSpaceElement != null)
			{
				OTBFunctions.OTSNackRequestElement(m_addressSpaceElement.ObjectHandle);
			}
			if (m_requestState != EnumRequestState.COMPLETED)
			{
				Complete();
			}
		}

		//-

		#endregion

		#region	//	Public Properties
		//---------------------------

		/// <summary>
		/// The property ID the request is about (0 means no property request)
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaRequest"]/
		///		property[@name="PropertyId"]/doc/*'
		///	/>
		public int PropertyId
		{
			get { return m_propertyId; }
		} //	end PropertyId


		/// <summary>
		/// The namespace element the request is about
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaRequest"]/
		///		property[@name="AddressSpaceElement"]/doc/*'
		///	/>
		public DaAddressSpaceElement AddressSpaceElement
		{
			get { return m_addressSpaceElement; }
		} //	end AddressSpaceElement


		/// <summary>
		/// The result of the request
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaRequest"]/
		///		property[@name="Result"]/doc/*'
		///	/>
		public EnumResultCode Result
		{
			get { return m_result; }
			set { m_result = value; }
		} //	end Result


		/// <summary>
		/// the state of the request
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaRequest"]/
		///		property[@name="RequestState"]/doc/*'
		///	/>
		public EnumRequestState RequestState
		{
			get
			{
				lock (m_requestStateJanitor)
				{
					return m_requestState;
				}
			}
			set
			{
				lock (m_requestStateJanitor)
				{
					m_requestState = value;
				}
			}
		} //	end RequestState


		/// <summary>
		/// the transaction the request is contained in
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaRequest"]/
		///		property[@name="TransactionKey"]/doc/*'
		///	/>
		public uint TransactionKey
		{
			get { return m_transactionKey; }
			set { m_transactionKey = value; }
		} //	end TransactionKey


		/// <summary>
		/// The type of transaction to be performed
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaRequest"]/
		///		property[@name="TransactionType"]/doc/*'
		///	/>
		public EnumTransactionType TransactionType
		{
			get { return m_transactionType; }
		} //	TransactionType


		/// <summary>
		/// This requests value with quality and timestamp.
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaRequest"]/
		///		property[@name="Value"]/doc/*'
		///	/>
		public ValueQT Value
		{
			get { return m_value; }
			set { m_value = value; }
		} //	end Value


		//-
		#endregion

		#region //	Internal Properties
		//-----------------------------

		/// <summary>
		/// The handle of the request itself
		/// </summary>
		internal uint RequestHandle
		{
			get { return m_requestHandle; }
		} //	end RequestHandle


		/// <summary>
		/// Returns the handle of the owning session
		/// </summary>
		internal uint SessionHandle
		{
			get { return m_sessionHandle; }
		} //	end SessionHandle


		//-
		#endregion

		#region	//	Public Methods
		//------------------------

		/// <summary>
		/// Complete only this request
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaRequest"]/
		///		method[@name="Complete"]/doc/*'
		///	/>
		public virtual int Complete()
		{
			int result = (int) EnumResultCode.S_FALSE;

			DaTransaction transaction = Application.Instance.FindTransaction(m_transactionKey);
			if (transaction != null)
			{
				result = transaction.CompleteRequest(this);
			} //	end if

			return result;
		} //	end Complete


		/// <summary>
		/// Mark as completed only this request (was internally completed [~cancelled] inside Toolkit Core)
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaRequest"]/
		///		method[@name="CompletedInternally"]/doc/*'
		///	/>
		public virtual void CompletedInternally()
		{
			ProgressRequestState(EnumRequestState.PENDING, EnumRequestState.COMPLETED);
			ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.COMPLETED);
		} //	end CompletedInternally


		/// <summary>
		/// Advances a request state from an initial value to a final value (this operation is thread-safe).
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaRequest"]/
		///		method[@name="ProgressRequestState"]/doc/*'
		///	/>
		public bool ProgressRequestState(EnumRequestState valueFrom, EnumRequestState valueTo)
		{
			lock (m_requestStateJanitor)
			{
				if (m_requestState == valueFrom)
				{
					m_requestState = valueTo;
					return true;
				}
				return false;
			}
		} //	end ProgressRequestState


		//-
		#endregion

		#region //	Private Attributes
		//------------------------------

		private readonly uint m_sessionHandle = 0;
		private readonly uint m_requestHandle = 0;

		/// <summary>
		/// The owning transaction key
		/// </summary>
		private uint m_transactionKey = 0;

		/// <summary>
		/// The associated transaction type
		/// </summary>
		private EnumTransactionType m_transactionType;

		/// <summary>
		/// Property assigned to the request
		/// </summary>
		private int m_propertyId = 0;

		/// <summary>
		/// Address space element assigned to the request
		/// </summary>
		private DaAddressSpaceElement m_addressSpaceElement = null;

		/// <summary>
		/// Result code to be filled in when request resolved
		/// </summary>
		private EnumResultCode m_result = EnumResultCode.E_NOTIMPL;

		/// <summary>
		/// Current cempletion state
		/// </summary>
		private EnumRequestState m_requestState = EnumRequestState.CREATED;

		/// <summary>
		/// Request state janitor
		/// </summary>
		private object m_requestStateJanitor = new object();

		/// <summary>
		/// Value carried by the request as a reply or as a write request
		/// </summary>
		private ValueQT m_value = null;

		//-
		#endregion
	} //	end DaRequest
} //	end Softing.OPCToolbox.Server
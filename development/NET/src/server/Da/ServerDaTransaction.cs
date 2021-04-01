using System;
using System.Collections;
using System.Runtime.InteropServices;
using Softing.OPCToolbox.OTB;

namespace Softing.OPCToolbox.Server
{
	/// <summary>
	/// Transaction class: Used for managing the transaction collection requests
	/// </summary>
	/// <include
	///  file='TBNS.doc.xml'
	///  path='//class[@name="DaTransaction"]/doc/*'
	/// />
	public class DaTransaction
	{
		#region //	Constructors
		//----------------------

		/// <summary>
		/// Default Constructor.
		/// </summary>
		/// <param name="aTransactionType">Client wants to read or to write</param>
		/// <param name="aRequestList">Array of requests contained in the transaction</param>
		/// <param name="aSessionHandle">The session's key</param>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  ctor[@name="DaTransaction"]/doc/*'
		/// />
		public DaTransaction(
			EnumTransactionType aTransactionType,
			DaRequest[] aRequestList,
			uint aSessionHandle)
		{
			this.m_type = aTransactionType;
			this.m_requestList.AddRange(aRequestList);

			this.m_sessionHandle = aSessionHandle;
			lock (s_keyBuilderJanitor)
			{
				this.m_key = s_keyBuilder++;
			}

			foreach (DaRequest request in aRequestList)
			{
				request.TransactionKey = this.m_key;
			} //	end foreach
		} //	end public constructor


		//-
		#endregion

		#region	//	Public Virtual Methods
		//--------------------------------

		/// <summary>
		/// Called if a read transaction occurs
		/// </summary>
		/// <returns>
		/// EnumResultCode.E_NOTIMPL. Must be overridden
		///	</returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  method[@name="HandleReadRequests"]/doc/*'
		/// />
		public virtual int HandleReadRequests()
		{
			return (int) EnumResultCode.E_NOTIMPL;
		} //	end HandleReadRequests


		/// <summary>
		/// Called if a write transaction occurs
		/// </summary>
		/// <returns>
		/// EnumResultCode.E_NOTIMPL. Must be overridden
		///	</returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  method[@name="HandleWriteRequests"]/doc/*'
		/// />
		public virtual int HandleWriteRequests()
		{
			return (int) EnumResultCode.E_NOTIMPL;
		} //	end HandleWriteRequests


		//-
		#endregion

		#region	//	Protected Attributes
		//----------------------------------

		/// <summary>
		/// incremented with each transaction created
		/// </summary>
		private static uint s_keyBuilder = 1;

		/// <summary>
		/// the key builder janotir
		/// </summary>
		private static object s_keyBuilderJanitor = new object();

		/// <summary>
		/// Requests contained in this transaction
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  attribute[@name="m_requestList"]/doc/*'
		/// />
		protected ArrayList m_requestList = new ArrayList();

		/// <summary>
		/// requests list janitor
		/// </summary>
		private object m_requestListJanitor = new object();

		/// <summary>
		/// Determines whether this is a read or a write Transaction
		/// </summary>
		private readonly EnumTransactionType m_type = EnumTransactionType.READ;

		/// <summary>
		///	the session's Handle
		/// </summary>
		private readonly uint m_sessionHandle = 0;

		/// <summary>
		/// the transaction unique key
		/// </summary>
		private readonly uint m_key = 0;

		//-
		#endregion

		#region //	Public Properties
		//---------------------------

		/// <summary>
		/// the transaction associated Key
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  property[@name="Key"]/doc/*'
		/// />
		public uint Key
		{
			get { return m_key; } //	end get
		} //	end Key


		/// <summary>
		/// Determines whether this is a read or a write Transaction
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  property[@name="Type"]/doc/*'
		/// />
		public EnumTransactionType Type
		{
			get { return m_type; } //	end get
		} //	end Type


		/// <summary>
		/// Requests contained in this transaction
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  property[@name="Requests"]/doc/*'
		/// />
		public ArrayList Requests
		{
			get { return m_requestList; } //	end get
		} //	end Requests


		/// <summary>
		/// Is transaction empty?
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  property[@name="IsEmpty"]/doc/*'
		/// />
		public bool IsEmpty
		{
			get
			{
				lock (m_requestListJanitor)
				{
					return (m_requestList.Count == 0);
				} //	end lock
			} //	end get
		} //	end IsEmpty


		/// <summary>
		/// Retrieves the session object owning this transaction
		/// </summary>
		/// <returns>
		/// null - Some error occurred
		/// DaSession the session that generated this transaction
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  property[@name="Session"]/doc/*'
		/// />
		public DaSession Session
		{
			get
			{
				if (m_sessionHandle != 0)
				{
					return (DaSession) Application.Instance.GetSession(m_sessionHandle);
				} //	end if
				else
				{
					return null;
				} //	end if ... else
			} //	end get
		} //	end Session


		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// Completes all requests must be called while or after "handling" requests
		/// </summary>
		/// <returns>
		/// S_OK - Everything was OK
		/// S_FALSE - Everything was OK
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  method[@name="CompleteRequests"]/doc/*'
		/// />
		public int CompleteRequests()
		{
			int result = (int) EnumResultCode.E_FAIL;

			int count = 0;
			OTValueData[] values = null;
			OTSRequestData[] requests = null;
			int[] results = null;

			try
			{
				lock (m_requestListJanitor)
				{
					count = m_requestList.Count;

					if (count == 0)
					{
						return (int) EnumResultCode.S_FALSE;
					} //	end if

					values = new OTValueData[count];
					requests = new OTSRequestData[count];
					results = new int[count];

					for (int i = 0; i < count; i++)
					{
						DaRequest request = m_requestList[i] as DaRequest;
						if (request != null)
						{
							requests[i].m_sessionHandle = request.SessionHandle;
							requests[i].m_propertyID = request.PropertyId;
							requests[i].m_requestHandle = request.RequestHandle;

							if (request.AddressSpaceElement != null)
							{
								requests[i].m_object.m_userData = (uint) request.AddressSpaceElement.UserData;
								requests[i].m_object.m_objectHandle = request.AddressSpaceElement.ObjectHandle;
							}
							else
							{
								requests[i].m_object.m_userData = 0;
								requests[i].m_object.m_objectHandle = 0;
							} //	end if ... else

							values[i] = new OTValueData();
							if (request.Value != null)
							{
								values[i].m_timestamp = new OTDateTime(request.Value.TimeStamp);
								values[i].m_quality = (ushort) request.Value.Quality;
								values[i].m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
								Marshal.GetNativeVariantForObject(request.Value.Data, values[i].m_value);

								results[i] = (int) request.Result;
							}
							else
							{
								values[i].m_quality = (ushort) EnumQuality.BAD;
								values[i].m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
								Marshal.GetNativeVariantForObject(null, values[i].m_value);
								results[i] = (int) EnumResultCode.E_UNEXPECTED;
							} //	end if ... else
						}
						else
						{
							requests[i].m_sessionHandle = 0;
							requests[i].m_propertyID = 0;
							requests[i].m_requestHandle = 0;
							requests[i].m_object.m_userData = 0;
							requests[i].m_object.m_objectHandle = 0;

							values[i] = new OTValueData();
							values[i].m_quality = (ushort) EnumQuality.BAD;
							values[i].m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
							Marshal.GetNativeVariantForObject(null, values[i].m_value);
							results[i] = (int) EnumResultCode.E_UNEXPECTED;
						} //	end if ... else

						request.RequestState = EnumRequestState.COMPLETED;
					} //	end for

					m_requestList.Clear();
				} //	end lock

				result = OTBFunctions.OTSCompleteRequests(count, requests, results, values);
			}
			catch (Exception e)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR, EnumTraceGroup.OPCSERVER,
					"DaTransaction.CompleteRequests", "Exception caught: " + e.ToString());

				result = (int) EnumResultCode.E_FAIL;
			}
			finally
			{
				Application.Instance.ReleaseTransaction(m_key);

				for (int i = 0; i < count; i++)
				{
					OTBFunctions.OTVariantClear(values[i].m_value);
					Marshal.FreeCoTaskMem(values[i].m_value);
				} //	end for
			} //	end try ... catch ... finally

			return result;
		} //	end CompleteRequests


		/// <summary>
		/// Marks the request identified by "requestHandle" completed, as it was internally handled (~cancelled) inside Toolkit Core
		/// </summary>
		/// <returns>
		/// S_OK - Request was found and marked completed
		/// E_NOTFOUND - Request was not found
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  method[@name="requestCompletedInternally"]/doc/*'
		/// />
		public int findRequestByHandle(uint requestHandle, out DaRequest request)
		{
			request = null;
			DaRequest req;
			lock (m_requestListJanitor)
			{
				int count = m_requestList.Count;
				for (int i = 0; i < count; ++i)
				{
					req = m_requestList[i] as DaRequest;
					if ((req != null) && (req.RequestHandle == requestHandle))
					{
						request = req;
						return (int) EnumResultCode.S_OK;
					}
				}
			} //	end lock
			return (int) EnumResultCode.E_NOTFOUND;
		}

		/// <summary>
		/// Completes a single request and removes it from the transaction
		/// </summary>
		/// <returns>
		/// S_OK - Everything was OK
		/// S_FALSE - Everything was OK
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  method[@name="CompleteRequest"]/doc/*'
		/// />
		public int CompleteRequest(DaRequest aRequest)
		{
			int result = (int) EnumResultCode.E_FAIL;

			OTValueData[] values = new OTValueData[1];
			OTSRequestData[] requests = new OTSRequestData[1];
			int[] results = new int[1];

			try
			{
				DaRequest request = aRequest as DaRequest;

				if (request != null)
				{
					request.RequestState = EnumRequestState.COMPLETED;

					//	remove the request from the list
					RemoveRequest(request);

					requests[0].m_sessionHandle = request.SessionHandle;
					requests[0].m_propertyID = request.PropertyId;
					requests[0].m_requestHandle = request.RequestHandle;

					if (request.AddressSpaceElement != null)
					{
						requests[0].m_object.m_userData = (uint) request.AddressSpaceElement.UserData;
						requests[0].m_object.m_objectHandle = request.AddressSpaceElement.ObjectHandle;
					}
					else
					{
						requests[0].m_object.m_userData = 0;
						requests[0].m_object.m_objectHandle = 0;
					} //	end if ... else

					values[0] = new OTValueData();
					if (request.Value != null)
					{
						values[0].m_timestamp = new OTDateTime(request.Value.TimeStamp);
						values[0].m_quality = (ushort) request.Value.Quality;
						values[0].m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
						Marshal.GetNativeVariantForObject(request.Value.Data, values[0].m_value);

						results[0] = (int) request.Result;
					}
					else
					{
						values[0].m_quality = (ushort) EnumQuality.BAD;
						values[0].m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
						Marshal.GetNativeVariantForObject(null, values[0].m_value);
						results[0] = (int) EnumResultCode.E_UNEXPECTED;
					} //	end if ... else
				}
				else
				{
					requests[0].m_sessionHandle = 0;
					requests[0].m_propertyID = 0;
					requests[0].m_requestHandle = 0;
					requests[0].m_object.m_userData = 0;
					requests[0].m_object.m_objectHandle = 0;

					values[0] = new OTValueData();
					values[0].m_quality = (ushort) EnumQuality.BAD;
					values[0].m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
					Marshal.GetNativeVariantForObject(null, values[0].m_value);
					results[0] = (int) EnumResultCode.E_UNEXPECTED;
				} //	end if ... else

				result = OTBFunctions.OTSCompleteRequests(1, requests, results, values);
			}
			catch (Exception e)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR, EnumTraceGroup.OPCSERVER,
					"DaTransaction.CompleteRequest", "Exception caught: " + e.ToString());

				result = (int) EnumResultCode.E_FAIL;
			}
			finally
			{
				if (this.IsEmpty)
				{
					Application.Instance.ReleaseTransaction(m_key);
				}
				OTBFunctions.OTVariantClear(values[0].m_value);
				Marshal.FreeCoTaskMem(values[0].m_value);
			} //	end try ... catch ... finally

			return result;
		} //	end CompleteRequest


		/// <summary>
		/// Changes the cache value for the Address space elements provided
		/// </summary>
		/// <returns>
		/// E_INVALIDARG - Invalid valueQT was passed
		/// S_OK - Value changed
		/// OTS_E_EXCEPTION - Unexpected error occurred
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  method[@name="ValuesChanged"]/doc/*'
		/// />
		public int ValuesChanged()
		{
			int result;
			int count = 0;
			OTValueData[] values = null;
			uint[] handles = null;

			try
			{
				lock (m_requestListJanitor)
				{
					count = m_requestList.Count;

					values = new OTValueData[count];
					handles = new uint[count];

					for (int i = 0; i < count; i++)
					{
						OTValueData valueData = new OTValueData();
						DaRequest request = m_requestList[i] as DaRequest;

						if (request != null)
						{
							valueData.m_quality = (ushort) request.Value.Quality;
							valueData.m_timestamp = new OTDateTime(request.Value.TimeStamp);
							valueData.m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);

							Marshal.GetNativeVariantForObject(request.Value.Data, valueData.m_value);
							handles[i] = request.AddressSpaceElement.ObjectHandle;

							request.Result = EnumResultCode.S_OK;
						}
						else
						{
							valueData.m_quality = (ushort) EnumQuality.BAD;
							valueData.m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
							Marshal.GetNativeVariantForObject(null, valueData.m_value);
							handles[i] = 0;
						} //	end if ... else

						values[i] = valueData;
					} //	end for
					result = OTBFunctions.OTSValuesChanged(count, handles, values);
				} //	end lock
			}
			catch (Exception e)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR, EnumTraceGroup.OPCSERVER,
					"DaTransaction.ValuesChanged", "Exception caught: " + e.ToString());
				result = (int) EnumResultCode.E_FAIL;
			}
			finally
			{
				for (int i = 0; i < count; i++)
				{
					OTBFunctions.OTVariantClear(values[i].m_value);
					Marshal.FreeCoTaskMem(values[i].m_value);
				} //	end for
			} //	end try ... catch ... finally

			return result;
		} //	end ValuesChanged


		/// <summary>
		/// removes a specific request from the internal requests list
		/// </summary>
		/// <param name="aRequest">
		/// the request to be removed from the transactrion
		/// </param>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="DaTransaction"]/
		///  method[@name="RemoveRequest"]/doc/*'
		/// />
		public void RemoveRequest(DaRequest aRequest)
		{
			if (aRequest == null)
			{
				return;
			} //	end if

			lock (m_requestListJanitor)
			{
				m_requestList.Remove(aRequest);
			} //	end lock
		} //	end removeRequest


		//-
		#endregion
	} //	end class Transaction
} //	end namespace Softing.OPCToolbox.Server
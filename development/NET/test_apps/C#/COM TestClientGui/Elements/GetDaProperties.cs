using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using Softing.OPCToolbox.Client;
using System.ComponentModel;
using Softing.OPCToolbox;

namespace TestClientGui.Elements
{
	public class GetDaProperties : Element
	{
		#region Public Properties
		[XmlElement(ElementName = "Session")]
		public string SessionProperty { get; set; }

		public string RootID { get; set; }

		public string RootPath { get; set; }

		public DaGetPropertiesOptions DaGetPropertiesOptions { get; set; }

		[DefaultValue(EnumExecutionType.SYNCHRONOUS)]
		public EnumExecutionType ExecutionType { get; set; }

		public List<TestClientGui.Support.DaProperty> ExpectedElements { get; set; }
		public string Property { get; set; }
		#endregion

		public GetDaProperties()
		{
			ExecutionType = EnumExecutionType.SYNCHRONOUS;
		}

		public override TestResult Execute()
		{
			if (string.IsNullOrEmpty(SessionProperty))
				return TestResult.ParameterMissing("Session");

			DaSession session = GetProperty(SessionProperty) as DaSession;
			if (session == null)
				return TestResult.ParameterInvalid("Session");

			try
			{
				Softing.OPCToolbox.Client.DaProperty[] elements;

				int result = int.MaxValue;
				if (ExecutionType == EnumExecutionType.SYNCHRONOUS)
				{
					result = session.GetDaProperties(RootID, RootPath, DaGetPropertiesOptions, out elements, new ExecutionOptions(EnumExecutionType.SYNCHRONOUS, m_asyncExecutionContext));
				}
				else
				{
					m_asyncExecutionContext = DaExecutionContext.Next;
					session.GetDaPropertiesCompleted += session_GetDaPropertiesCompleted;
					m_waitHandle = new System.Threading.AutoResetEvent(false);
					result = session.GetDaProperties(RootID, RootPath, DaGetPropertiesOptions, out elements, new ExecutionOptions(EnumExecutionType.ASYNCHRONOUS, m_asyncExecutionContext));

					if (m_waitHandle.WaitOne(3000))
					{
						result = m_browseAsyncResult;
						elements = m_browseAsyncResults;
					}
					session.GetDaPropertiesCompleted -= session_GetDaPropertiesCompleted;
				}

				if (ResultCode.SUCCEEDED(result))
				{
					if (!string.IsNullOrEmpty(Property))
						SetProperty(Property, elements);

					if (ExpectedElements.Count != 0 && ExpectedElements.Count != elements.Length)
						return TestResult.Failed(string.Format("Unexpected number of results, expected {0}, actual {1}", ExpectedElements.Count, elements.Length));

					for (int i = 0; i < ExpectedElements.Count; i++)
					{
						Softing.OPCToolbox.Client.DaProperty receivedElement = elements[i];
						TestClientGui.Support.DaProperty expectedElement = ExpectedElements[i];

						if ((expectedElement.ItemId != null && expectedElement.ItemId != receivedElement.ItemId)
							|| (expectedElement.ItemPath != null && expectedElement.ItemPath != receivedElement.ItemPath)
							|| (!string.IsNullOrEmpty(expectedElement.Name) && expectedElement.Name != receivedElement.Name)
							|| (expectedElement.DataType != null && expectedElement.DataType != receivedElement.DataType)
							|| (!string.IsNullOrEmpty(expectedElement.Description) && expectedElement.Description != receivedElement.Description)
							|| (expectedElement.ValueQT != null && !expectedElement.ValueQT.Data.Equals(receivedElement.ValueQT.Data))
							|| (expectedElement.ValueQT != null && expectedElement.ValueQT.Quality != receivedElement.ValueQT.Quality))
							return TestResult.Failed(string.Format("Unexpected result for item #{0}", i));

						if (DaGetPropertiesOptions.WhatPropertyData == EnumPropertyData.ITEMID || DaGetPropertiesOptions.WhatPropertyData == EnumPropertyData.NONE)
							if (receivedElement.ValueQT.Data != null)
								return TestResult.Failed(string.Format("Value was not NULL for element #{0}", i));
						if (DaGetPropertiesOptions.WhatPropertyData == EnumPropertyData.VALUE)
							if (!string.IsNullOrEmpty(receivedElement.ItemId))
								return TestResult.Failed(string.Format("ItemId was not NULL for element #{0}", i));
					}

					return TestResult.Success();
				}
				else
				{
					EnumResultCode resCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), result.ToString());

					return TestResult.Failed(string.Format("Call returned: {0}\t({1})", resCode.ToString(), result));
				}
			}
			catch (Exception ex)
			{
				return new TestResult(ex);
			}
		}

		void session_GetDaPropertiesCompleted(DaSession aDaSession, uint executionContext, uint addressSpaceElementHandle, string addressSpaceElementID, string addressSpaceElementPath, DaProperty[] aDaProperty, int result)
		{
			if (executionContext == m_asyncExecutionContext)
			{
				m_browseAsyncResults = aDaProperty;
				m_browseAsyncResult = result;

				m_waitHandle.Set();
			}
		}

		#region Private Fields
		System.Threading.AutoResetEvent m_waitHandle;
		private uint m_asyncExecutionContext;
		Softing.OPCToolbox.Client.DaProperty[] m_browseAsyncResults = null;
		int m_browseAsyncResult;
		#endregion
	}
}

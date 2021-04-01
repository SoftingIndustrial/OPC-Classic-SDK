using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using System.Collections.Generic;
using Softing.OPCToolbox;

namespace TestClientGui.Elements
{
	public class DaSessionBrowse : Element
	{
		#region Public Properties
		public string Session { get; set; }

		public string RootID { get; set; }

		public string RootPath { get; set; }

		[TypeConverter( typeof( ExpandableObjectConverter ) )]
		public DaAddressSpaceElementBrowseOptions BrowseOptions
		{
			get
			{
				return m_browseOptions;
			}

			set
			{
				m_browseOptions = value;
			}
		}

		public List<DaAddressSpaceElement> ExpectedElements { get; set; }

		[DefaultValue(EnumExecutionType.SYNCHRONOUS)]
		public EnumExecutionType ExecutionType { get; set; }

		public string ExpectedServiceResult { get; set; }

		public string Property { get; set; }
		#endregion

		public DaSessionBrowse()
		{
			ExecutionType = EnumExecutionType.SYNCHRONOUS;
		}

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( Session ) )
				return TestResult.ParameterMissing( "Session" );

			DaSession session = GetProperty( Session ) as DaSession;
			if( session == null )
				return TestResult.ParameterInvalid( "Session" );

			try
			{
				Softing.OPCToolbox.Client.DaAddressSpaceElement[] elements;
				int result = Int16.MaxValue;

				if( BrowseOptions == null )
					BrowseOptions = new DaAddressSpaceElementBrowseOptions();

				if (ExecutionType == EnumExecutionType.SYNCHRONOUS)
				{
					result = session.Browse(RootID, RootPath, BrowseOptions.Wrapped, out elements, new ExecutionOptions(EnumExecutionType.SYNCHRONOUS, m_asyncExecutionContext));
				}
				else
				{
					m_asyncExecutionContext = DaExecutionContext.Next;
					session.BrowseCompleted += session_BrowseCompleted;
					m_waitHandle = new System.Threading.AutoResetEvent(false);
					result = session.Browse(RootID, RootPath, BrowseOptions.Wrapped, out elements, new ExecutionOptions(EnumExecutionType.ASYNCHRONOUS, m_asyncExecutionContext));

					if (m_waitHandle.WaitOne(3000))
					{
						result = m_browseAsyncResult;
						elements = m_browseAsyncResults;
					}
					session.BrowseCompleted -= session_BrowseCompleted;
				}

				return ValidateBrowsedElements(elements, result);
			}
			catch( Exception ex )
			{
				return new TestResult( ex );
			}
		}

		private TestResult ValidateBrowsedElements(Softing.OPCToolbox.Client.DaAddressSpaceElement[] elements, int result)
		{
			if (ResultCode.SUCCEEDED(result))
			{
				if (!string.IsNullOrEmpty(Property))
					SetProperty(Property, elements);

				if (ExpectedElements.Count != 0 && ExpectedElements.Count != elements.Length)
					return new DaBrowseResult(result, elements, "Unexpected number of results");

				for (int i = 0; i < ExpectedElements.Count; i++)
				{
					Softing.OPCToolbox.Client.DaAddressSpaceElement receivedElement = elements[i];
					DaAddressSpaceElement expectedElement = ExpectedElements[i];

					if ((expectedElement.ItemId != null && expectedElement.ItemId != receivedElement.ItemId)
						|| (expectedElement.ItemPath != null && expectedElement.ItemPath != receivedElement.ItemPath)
						|| (expectedElement.Name != null && expectedElement.Name != receivedElement.Name)
						|| (expectedElement.IsItem.HasValue && expectedElement.IsItem.Value != receivedElement.IsItem)
						|| (expectedElement.IsBranch.HasValue && expectedElement.IsBranch.Value != receivedElement.IsBranch)
						|| (expectedElement.IsLeaf.HasValue && expectedElement.IsLeaf.Value != receivedElement.IsLeaf))
						return new DaBrowseResult(result, elements, string.Format("Unexpected result for item #{0}", i));
				}

				return new DaBrowseResult(result, elements);
			}
			else
			{
				EnumResultCode resCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), result.ToString());
				if (!string.IsNullOrEmpty(ExpectedServiceResult))
				{
					EnumResultCode expectedResCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), ExpectedServiceResult.ToString());
					if (expectedResCode != resCode)
					{
						return TestResult.Failed(string.Format("Unexpected service result: expected {0}, actual {1}", resCode, expectedResCode));
					}
					return TestResult.Success();
				}

				return TestResult.Failed(string.Format("Call returned: {0}  ({1})", resCode.ToString(), result));
			}
		}

		void session_BrowseCompleted(DaSession aDaSession, uint executionContext, uint addressSpaceElementHandle, string addressSpaceElementID, string addressSpaceElementPath, Softing.OPCToolbox.Client.DaAddressSpaceElement[] addressSpaceElements, int result)
		{
			if (executionContext == m_asyncExecutionContext)
			{
				m_browseAsyncResults = addressSpaceElements;
				m_browseAsyncResult = result;

				m_waitHandle.Set();
			}
		}

		#region Private Fields
		DaAddressSpaceElementBrowseOptions m_browseOptions;
		System.Threading.AutoResetEvent m_waitHandle;
		private uint m_asyncExecutionContext;
		Softing.OPCToolbox.Client.DaAddressSpaceElement[] m_browseAsyncResults = null;
		int m_browseAsyncResult;
		#endregion
	}


	public class DaBrowseResult : TestResult
	{
		[XmlIgnore]
		public Softing.OPCToolbox.Client.DaAddressSpaceElement[] Elements
		{
			get
			{
				if( m_values == null )
					return DaAddressSpaceElement.CreateSDKCollection( m_valuesWrapper );
				else
					return m_values;
			}

			protected set
			{
				m_values = value;
			}
		}

		//[System.ComponentModel.Browsable( false )]
		[XmlArray( ElementName = "Values" )]
		public List<DaAddressSpaceElement> ElementsWrapper
		{
			get
			{
				if( m_values == null )
					return m_valuesWrapper;
				else
					return DaAddressSpaceElement.CreateList( m_values );
			}

			set
			{
				m_valuesWrapper = value;
			}
		}

		public int Result
		{
			get;
			set;
		}

		public DaBrowseResult()
		{
		}

		public DaBrowseResult( int result, Softing.OPCToolbox.Client.DaAddressSpaceElement[] values )
			: base( TestResults.Success )
		{
			Result = result;
			m_values = values;
		}

		public DaBrowseResult( int result, Softing.OPCToolbox.Client.DaAddressSpaceElement[] values, string details )
			: base( TestResults.Failed )
		{
			Result = result;
			m_values = values;
			Details = details;
		}

		private Softing.OPCToolbox.Client.DaAddressSpaceElement[] m_values;
		private List<DaAddressSpaceElement> m_valuesWrapper;
	}
}

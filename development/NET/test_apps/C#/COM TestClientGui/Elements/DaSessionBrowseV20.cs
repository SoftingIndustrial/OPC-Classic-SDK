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
	public class DaSessionBrowseV20 : Element
	{
		#region Public Properties
		[XmlElement( ElementName = "Session" )]
		public string SessionProperty { get; set; }

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

		[DefaultValue( EnumExecutionType.SYNCHRONOUS )]
		public EnumExecutionType ExecutionType { get; set; }

		public string Property { get; set; }
		#endregion

		public DaSessionBrowseV20()
		{
			ExecutionType = EnumExecutionType.SYNCHRONOUS;
		}

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( SessionProperty ) )
				return TestResult.ParameterMissing( "Session" );

			DaSession session = GetProperty( SessionProperty ) as DaSession;
			if( session == null )
				return TestResult.ParameterInvalid( "Session" );

			try
			{
				Softing.OPCToolbox.Client.DaAddressSpaceElement[] elements;

				Softing.OPCToolbox.Client.DaAddressSpaceElement rootDaAddressSpaceElement = new Softing.OPCToolbox.Client.DaAddressSpaceElement(
					EnumAddressSpaceElementType.BRANCH, String.Empty, string.Empty, string.Empty, 0, null );
				rootDaAddressSpaceElement.Session = session;

				int result = rootDaAddressSpaceElement.Browse( new Softing.OPCToolbox.Client.DaAddressSpaceElementBrowseOptions(), out elements, new ExecutionOptions( EnumExecutionType.SYNCHRONOUS, m_asyncExecutionContext ) );

				if( !ResultCode.SUCCEEDED( result ) )
				{
					EnumResultCode resCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), result.ToString() );

					return TestResult.Failed( string.Format( "Call returned: {0}  ({1})", resCode.ToString(), result ) );
				}

				Softing.OPCToolbox.Client.DaAddressSpaceElement addressSpaceElement = null;
				for( int i = 0; i < elements.Length; i++ )
				{
					if( elements[i].ItemId == RootID )
					{
						addressSpaceElement = elements[i];
						break;
					}
				}

				if( addressSpaceElement == null )
					return TestResult.Failed( string.Format( "Address space element {0} not found!", RootID ) );

				if( BrowseOptions == null )
					BrowseOptions = new DaAddressSpaceElementBrowseOptions();

				if( ExecutionType == EnumExecutionType.SYNCHRONOUS )
				{
					//browse again => this time the specified item id
					result = addressSpaceElement.Browse( BrowseOptions.Wrapped, out elements, new ExecutionOptions( ExecutionType, 0 ) );
				}
				else
				{
					m_asyncExecutionContext = DaExecutionContext.Next;
					session.BrowseCompleted += session_BrowseCompleted;
					m_waitHandle = new System.Threading.AutoResetEvent( false );
					result = addressSpaceElement.Browse( BrowseOptions.Wrapped, out elements, new ExecutionOptions( EnumExecutionType.ASYNCHRONOUS, m_asyncExecutionContext ) );

					if( m_waitHandle.WaitOne( 3000 ) )
					{
						result = m_browseAsyncResult;
						elements = m_browseAsyncResults;
					}
					session.BrowseCompleted -= session_BrowseCompleted;
				}

				if( ResultCode.SUCCEEDED( result ) )
				{
					if( !string.IsNullOrEmpty( Property ) )
						SetProperty( Property, elements );

					if( ExpectedElements.Count != 0 && ExpectedElements.Count != elements.Length )
						return new DaBrowseResult( result, elements, "Unexpected number of results" );

					for( int i = 0; i < ExpectedElements.Count; i++ )
					{
						Softing.OPCToolbox.Client.DaAddressSpaceElement receivedElement = elements[i];
						DaAddressSpaceElement expectedElement = ExpectedElements[i];

						if( ( expectedElement.ItemId != null && expectedElement.ItemId != receivedElement.ItemId )
							|| ( expectedElement.ItemPath != null && expectedElement.ItemPath != receivedElement.ItemPath )
							|| ( expectedElement.Name != null && expectedElement.Name != receivedElement.Name )
							|| ( expectedElement.IsItem.HasValue && expectedElement.IsItem.Value != receivedElement.IsItem )
							|| ( expectedElement.IsBranch.HasValue && expectedElement.IsBranch.Value != receivedElement.IsBranch )
							|| ( expectedElement.IsLeaf.HasValue && expectedElement.IsLeaf.Value != receivedElement.IsLeaf ) )
							return new DaBrowseResult( result, elements, string.Format( "Unexpected result for item #{0}", i ) );
					}

					return new DaBrowseResult( result, elements );
				}
				else
				{
					EnumResultCode resCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), result.ToString() );

					return TestResult.Failed( string.Format( "Call returned: {0}  ({1})", resCode.ToString(), result ) );
				}
			}
			catch( Exception ex )
			{
				return new TestResult( ex );
			}
		}

		void session_BrowseCompleted( DaSession aDaSession, uint executionContext, uint addressSpaceElementHandle, string addressSpaceElementID, string addressSpaceElementPath, Softing.OPCToolbox.Client.DaAddressSpaceElement[] addressSpaceElements, int result )
		{
			if( executionContext == m_asyncExecutionContext )
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
}

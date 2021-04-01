using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using System.Xml.Serialization;
using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class DaSubscribeChildItems : Element
	{
		#region Public Properties

		public string Session { get; set; }

		public string RootID { get; set; }

		public string RootPath { get; set; }

		[TypeConverter(typeof(ExpandableObjectConverter))]
		public DaAddressSpaceElementBrowseOptions BrowseOptions { get; set; }

		[DefaultValue(1000)]
		public uint updateRate { get; set; }

		[DefaultValue(true)]
		public bool ExpectedSuccess { get; set; }
		#endregion

		public DaSubscribeChildItems()
		{
			ExpectedSuccess = true;
		//	Connect = true;
		//	Active = true;
		}

		public override TestResult Execute()
		{
			if (string.IsNullOrEmpty(Session))
				return TestResult.ParameterMissing("Session");
			if (string.IsNullOrEmpty(RootID))
				return TestResult.ParameterMissing("RootID");
		//	if (string.IsNullOrEmpty(RootPath))
			//	return TestResult.ParameterMissing("RootPath");


			Softing.OPCToolbox.Client.DaSession session = GetProperty( Session ) as DaSession;
			if (session == null)

				return TestResult.Failed( "Session non existent" );

			try
			{
				ExecutionOptions executionOptions = new ExecutionOptions(EnumExecutionType.SYNCHRONOUS, 0);
				session.Connect(false, false, executionOptions);
				int result=1;

				Softing.OPCToolbox.Client.DaAddressSpaceElementBrowseOptions browseOptions = new Softing.OPCToolbox.Client.DaAddressSpaceElementBrowseOptions();
				browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.LEAF;

				Softing.OPCToolbox.Client.DaAddressSpaceElement[] DaElements;

				//get the nodes children
				if (ResultCode.SUCCEEDED(session.Browse(RootID, RootPath, browseOptions, out DaElements, executionOptions)))
				{
					//Softing.OPCToolbox.Client.DaAddressSpaceElement daElement;
					Softing.OPCToolbox.Client.DaSubscription workerSubscription = new Softing.OPCToolbox.Client.DaSubscription(updateRate, session);

					foreach (Softing.OPCToolbox.Client.DaAddressSpaceElement daElement in DaElements)
					{
						DaItem newItem = new DaItem(daElement.ItemId, workerSubscription);
					//	result =  Connect(true, true, executionOptions);
					}
					result = workerSubscription.Connect(true, true, executionOptions);
						if (ResultCode.FAILED(result))
						{
							EnumResultCode resCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), result.ToString());

							return TestResult.Failed(string.Format("Subscribe Call returned: {0}  ({1})", resCode.ToString(), result));
						}



				}//end if
				else
				{
					EnumResultCode resCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), result.ToString());

					return TestResult.Failed(string.Format(" Browse Call returned: {0}  ({1})", resCode.ToString(), result));
				}


			}
			catch (Exception ex)
			{
				if (ExpectedSuccess)
					return new TestResult(ex);
			}

			return TestResult.Success();
		}
	}
}

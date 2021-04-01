using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using System.Xml.Serialization;
using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class DaReadChildItems : Element
	{
		#region Public Properties
		public string RootID { get; set; }
		public string RootPath {get; set; }

		public string Session { get; set; }

		[DefaultValue(1000)]
		public uint updateRate { get; set; }

		[DefaultValue(true)]
		public bool ExpectedSuccess { get; set; }

		#endregion



		public DaReadChildItems()
		{
			ExpectedSuccess = true;
		}

		public override TestResult Execute()
		{
			Softing.OPCToolbox.Client.DaSession session = GetProperty(Session) as DaSession;
			if (session == null)
				//	session = new Softing.OPCToolbox.Client.DaSession("opcda:///Softing.COM TestServer.DA.1/{E4D228D9-1442-4DC9-BFB6-026215D5D302}");
				session = new Softing.OPCToolbox.Client.DaSession("opcda:///OpcTestLab.OpcAnalyzer.1/{B8AADBA5-382F-45DA-AA0A-D7BB21E4AE97}");
			try
			{
				if (string.IsNullOrEmpty(Session))
					return TestResult.ParameterMissing("Session");
				if (string.IsNullOrEmpty(RootID))
					return TestResult.ParameterMissing("RootID");

				ExecutionOptions executionOptions = new ExecutionOptions(EnumExecutionType.SYNCHRONOUS, 0);
				session.Connect(true, true, new ExecutionOptions());
				int result = 1;

				Softing.OPCToolbox.Client.DaAddressSpaceElementBrowseOptions browseOptions = new Softing.OPCToolbox.Client.DaAddressSpaceElementBrowseOptions();
				browseOptions.ElementTypeFilter = EnumAddressSpaceElementType.ALL;

				Softing.OPCToolbox.Client.DaAddressSpaceElement[] DaElements;
				ValueQT[] values;
				int[] results;
				Softing.OPCToolbox.Client.DaSubscription workerSubscription = new Softing.OPCToolbox.Client.DaSubscription(1000, session);

				if (ResultCode.SUCCEEDED(session.Browse(RootID, RootPath, browseOptions, out DaElements, executionOptions)))
				{
					foreach (Softing.OPCToolbox.Client.DaAddressSpaceElement daElement in DaElements)
					{
						if (daElement.ItemId.Contains("TV"))
						{
							Softing.OPCToolbox.Client.DaItem myItem = new DaItem(daElement.ItemId, workerSubscription);
						}
					}
					workerSubscription.Connect(true,true, new ExecutionOptions());
					result = workerSubscription.Read(1000, workerSubscription.ItemList, out values, out results, new ExecutionOptions());

				}
			}
			catch (System.Exception ex)
			{
				if (ExpectedSuccess)
					return new TestResult(ex);
			}


			return TestResult.Success();
		}
	}
}

using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;
using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class DaVariableNodesSubscribeProxy :Element
	{
		#region Private Fields

		private const string m_variableNodeIdPrefix = "/TV_";

		#endregion

		#region Public Properties
		public string Session { get; set; }

		public string NodeSetId { get; set; }

		[DefaultValue((uint)10)]
		public uint Count { get; set; }

		[DefaultValue(1000)]
		public uint UpdateRate { get; set; }

		public string Property { get; set; }
		public string SubscriptionProperty { get; set; }
		#endregion

		public override TestResult Execute()
		{
			if (string.IsNullOrEmpty(Session))
				return TestResult.ParameterMissing("Session");

			DaSession session = GetProperty(Session) as DaSession;
			if (session == null)
				return TestResult.ParameterInvalid("Session");

			if (string.IsNullOrEmpty(NodeSetId))
				return TestResult.ParameterMissing("NodeSetId");

			string nodeSetId = GetProperty(NodeSetId) as string;
			if (nodeSetId == null)
				return TestResult.ParameterInvalid("NodeSetId");

			try
			{
				DaSubscription subscription = new DaSubscription(UpdateRate, session);

				if (!subscription.Valid)
					return TestResult.Failed("DaSubscription not Valid");

				int result = subscription.Connect(true, true, null);

				if (ResultCode.SUCCEEDED(result))
				{
					// subscription created
					ExecutionOptions executionOptions = new ExecutionOptions(EnumExecutionType.SYNCHRONOUS, 0);
					List<DaItem> items = new List<DaItem>();

					// create items
					for (uint i = 0; i < Count; i++)
					{
						string itemId = String.Format("{0}, {1}, {2}", nodeSetId, m_variableNodeIdPrefix, i);

						DaItem item = new DaItem(itemId, subscription);

						if (!item.Valid)
							return TestResult.Failed("Item not Valid");

						result = item.Connect(false, true, executionOptions);

						if (ResultCode.FAILED(result))
						{
							EnumResultCode resCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), result.ToString());

							return TestResult.Failed(string.Format("Call returned: {0}  ({1})", resCode.ToString(), result));
						}

						items.Add(item);
					}

					SetProperty(Property, items.ToArray());
					SetProperty(SubscriptionProperty, subscription);

					return TestResult.Success();
				}
				else
				{
					EnumResultCode resCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), result.ToString());

					return TestResult.Failed(string.Format("Call returned: {0}  ({1})", resCode.ToString(), result));
				}
			}
			catch (Exception ex)
			{
				return new TestResult(ex);
			}
		}

	}
}

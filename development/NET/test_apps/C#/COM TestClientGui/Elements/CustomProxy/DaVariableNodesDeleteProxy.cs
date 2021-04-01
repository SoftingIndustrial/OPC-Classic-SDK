using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;
using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class DaVariableNodesDeleteProxy :Element
	{
		#region Private Fields

		private string m_deleteVariablesCommand = "/DeleteItemsCommand";

		#endregion

		#region Public Properties
		public string Session { get; set; }

		public string NodeSetId { get; set; }
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
				int result = (int)EnumResultCode.S_FALSE;
				int[] results;
				string[] itemIDs;
				ValueQT[] values;

				// set stop simulation params
				itemIDs = new string[] { nodeSetId + m_deleteVariablesCommand };
				values = new ValueQT[] { new ValueQT(true, EnumQuality.GOOD, DateTime.Now) };

				// send write request
				result = session.Write(itemIDs, null, values, out results, null);

				if (ResultCode.SUCCEEDED(result))
				{
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

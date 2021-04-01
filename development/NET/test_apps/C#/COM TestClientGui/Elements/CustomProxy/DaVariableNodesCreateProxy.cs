using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;
using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class DaVariableNodesCreateProxy : Element
	{
		#region Private Fields
		private string m_ItemCountNodeId = "ns*2;s*Test Folder/ItemCount";
		private string m_ItemTypeNodeId = "ns*2;s*Test Folder/ItemType";
		private string m_ItemIsAnalogNodeId = "ns*2;s*Test Folder/ItemIsAnalog";
		private string m_CreateItemsNodeId = "ns*2;s*Test Folder/CreateItemsCommand";

		private string ItemSetIDNodeId = "ns*2;s*Test Folder/ItemSetID";

		private const string m_nodeIdSufix = ".TV_";
		private const string m_nodeIdPrefix = "0:";
		private const string m_nodeSetPrefix = "ns*2;s*";

		private string guid = string.Empty;
		#endregion

		#region Public Properties
		public string Session { get; set; }

		public string Property { get; set; }

		[DefaultValue((uint)10)]
		public uint Count { get; set; }

		[DefaultValue(VarType.Double)]
		public VarType NodeType { get; set; }

		[DefaultValue(false)]
		public bool IsAnalog { get; set; }
		#endregion

		public override TestResult Execute()
		{
			if (string.IsNullOrEmpty(Session))
				return TestResult.ParameterMissing("Session");

			DaSession session = GetProperty(Session) as DaSession;
			if (session == null)
				return TestResult.ParameterInvalid("Session");


			if (Count <= 0)
				return TestResult.ParameterInvalid("Count");

			try
			{
				// prepare values to write
				List<string> itemToWrite = new List<string>();
				itemToWrite.Add(m_ItemCountNodeId);
				itemToWrite.Add(m_ItemTypeNodeId);
				itemToWrite.Add(m_ItemIsAnalogNodeId);
				itemToWrite.Add(m_CreateItemsNodeId);

				string[] itemIDs = itemToWrite.ToArray();

				List<ValueQT> valuesToWrite = new List<ValueQT>();
				valuesToWrite.Add(new ValueQT(Count, EnumQuality.GOOD, DateTime.Now));
				valuesToWrite.Add(new ValueQT((uint)NodeType, EnumQuality.GOOD, DateTime.Now));
				valuesToWrite.Add(new ValueQT(IsAnalog, EnumQuality.GOOD, DateTime.Now));
				guid = Guid.NewGuid().ToString();
				valuesToWrite.Add(new ValueQT(guid, EnumQuality.GOOD, DateTime.Now));

				ValueQT[] values = valuesToWrite.ToArray();

				int result = (int)EnumResultCode.S_FALSE;
				int[] results;

				// send CreateItemsRequest
				result = session.Write(itemIDs, null, values, out results, null);

				if (ResultCode.SUCCEEDED(result))
				{
					// wait for Variables to be created
					System.Threading.Thread.Sleep(1000);

					string[] itemsToRead = new string[] { ItemSetIDNodeId };
					ValueQT[] readValues;

					result = session.Read(0, itemsToRead, null, out readValues, out results, null);

					if (ResultCode.SUCCEEDED(result))
					{
						string itemSetIdValue = string.Empty;
						string[] stringArray = readValues[0].Data as string[];
						string item = string.Empty;
						for (int i = 0; i < stringArray.Length; i++)
						{
							item = stringArray[i];
							if (item.Split('#')[1] == guid)
							{
								itemSetIdValue = item.Split('#')[0];
								break;
							}
						}
						if (itemSetIdValue == string.Empty)
						{
							return TestResult.Failed(string.Format("ItemSetId value was not found! {0}", guid));
						}

						itemSetIdValue = m_nodeSetPrefix + itemSetIdValue;

						SetProperty(Property, itemSetIdValue);

						return TestResult.Success();
					}
					else
					{
						EnumResultCode resCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), result.ToString());

						return TestResult.Failed(string.Format("Call returned: {0}  ({1})", resCode.ToString(), result));
					}
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

	public enum VarType
	{
		Unknown = 0,
		Uint8 = 1,
		Int8 = 2,
		Uint16 = 3,
		Int16 = 4,
		Uint32 = 5,
		Int32 = 6,
		Uint64 = 7,
		Int64 = 8,
		Float = 9,
		Double = 10,
	}
}

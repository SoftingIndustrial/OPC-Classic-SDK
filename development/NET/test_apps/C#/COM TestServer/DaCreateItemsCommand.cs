using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;


namespace TestServer
{
	/// <summary>
	/// Used to manage a CreateItems command
	/// </summary>
	public class DaCreateItemsCommand : DaAddressSpaceElement
	{
		#region Constructors

		public DaCreateItemsCommand(
			string anItemID,
			string aName,
			uint anUserData,
			uint anObjectHandle,
            uint aParentHandle)
			:
			base(
			anItemID,
			aName,
			anUserData,
			anObjectHandle,
			aParentHandle)
		{
		}

		public DaCreateItemsCommand()
		{
			Datatype = typeof(string);
			AccessRights = EnumAccessRights.READWRITEABLE;
			IoMode = EnumIoMode.POLL;
		}
		#endregion

		#region Public Properties

		public const string ItemCountParam = "ItemCount";
		public const string ItemTypeParam  = "ItemType";
		public const string ItemSetIdParam = "ItemSetID";
		public const string ItemIsAnalogParam = "ItemIsAnalog";

		public static Type ItemCountParamType = typeof(uint);
		public static Type ItemTypeParamType = typeof(uint);
		public static Type ItemSetIdParamType = typeof(string[]);
		public static Type ItemIsAnalogParamType = typeof(bool);
		#endregion

		#region Public Methods

		public EnumResultCode CreateItems(ValueQT valueQT)
		{
			//lock (m_lock)
			{
				try
				{
					// Retreive parent folder
					AddressSpaceElement parentFolder = GetParent();
					ArrayList children = GetChildren();

					uint itemCount = 0;
					VarType itemType = VarType.Unknown;
					string itemSet = String.Empty;
					bool isAnalogItem = false;
					DaAddressSpaceElement outParam = null;

					// get parameter values
					foreach (DaAddressSpaceElement param in children)
					{
						if (param == null)
						{
							continue;
						}

						ValueQT paramValue = null;
						param.GetCacheValue(ref paramValue);

						if (param.Name == ItemCountParam)
						{
							itemCount = (uint)paramValue.Data;
						}
						else if (param.Name == ItemTypeParam)
						{
							itemType = (VarType)((uint)paramValue.Data);
						}
						else if (param.Name == ItemSetIdParam)
						{
							outParam = param;
						}
						else if (param.Name == ItemIsAnalogParam)
						{
							isAnalogItem = (bool)paramValue.Data;
						}
					}

					// Check if all parameters are set
					if (itemCount != 0 && itemType != VarType.Unknown && outParam != null)
					{
						Type varType;

						switch (itemType)
						{
							case VarType.Uint8:
								varType = typeof(Byte);
								break;
							case VarType.Int8:
								varType = typeof(SByte);
								break;
							case VarType.Uint16:
								varType = typeof(UInt16);
								break;
							case VarType.Int16:
								varType = typeof(Int16);
								break;
							case VarType.Uint32:
								varType = typeof(UInt32);
								break;
							case VarType.Int32:
								varType = typeof(Int32);
								break;
							case VarType.Uint64:
								varType = typeof(UInt64);
								break;
							case VarType.Int64:
								varType = typeof(Int64);
								break;
							case VarType.Float:
								varType = typeof(Single);
								break;
							case VarType.Double:
								varType = typeof(Double);
								break;
							default:
								varType = typeof(Byte);
								break;
						}

						// Create the item set
						CreateItems(parentFolder, outParam, itemCount, varType, false, valueQT.Data.ToString(), isAnalogItem);
					}

					else
					{
						// Parameters not set correctly
						return EnumResultCode.E_FAIL;
					}

					return EnumResultCode.S_OK;
				}
				catch
				{
					return EnumResultCode.E_FAIL;
				}
			}
		}

		#endregion

		#region Private Methods

		/// <summary>
		/// Create a list o items in the AddressSpace
		/// </summary>
		/// <param name="parentFolder"></param>
		/// <param name="itemCount"></param>
		/// <param name="itemType"></param>
		/// <param name="itemSetName"></param>
		private EnumResultCode CreateItems(
			AddressSpaceElement parentFolder,
			DaAddressSpaceElement resultItem,
			uint itemCount,
			Type itemType,
			bool generateRandomValues,
			string guid,
			bool isAnalogItem)
		{
			// Create item folder
			MyDaAddressSpaceElement itemFolder = new MyDaAddressSpaceElement();
			itemFolder.Name = String.Format("TestItems_{0}", m_createdFolders );
			itemFolder.HasChildren = true;
			itemFolder.AccessRights = EnumAccessRights.READABLE;
			if (!parentFolder.AddChild(itemFolder))
			{
				return EnumResultCode.E_FAIL;
			}

			m_createdFolders++;

			//add guid to ItemSetId node
			ValueQT cacheValue = null;
			resultItem.GetCacheValue(ref cacheValue);
			string[] currentValue = cacheValue.Data as string[];
			string[] newValue = new string[currentValue.Length + 1];
			currentValue.CopyTo(newValue, 0);
			newValue[currentValue.Length] = itemFolder.ItemId + "#" + guid;
			resultItem.ValueChanged(new ValueQT(newValue, EnumQuality.GOOD, DateTime.Now));


			// Add Start/Stop simulation command
			DaStartStopSimulationCommand simulationCmd = new DaStartStopSimulationCommand();
			simulationCmd.Name = "StartStopSimulationCommand";
			simulationCmd.HasChildren = true;
			if (!itemFolder.AddChild(simulationCmd))
			{
				return EnumResultCode.E_FAIL;
			}
			simulationCmd.ValueChanged(new ValueQT(false, EnumQuality.GOOD, DateTime.Now));

			MyDaAddressSpaceElement param;

			// Add command parameters

			// item count parameter
			param = new MyDaAddressSpaceElement(DaStartStopSimulationCommand.ChangeIntervalParam, DaStartStopSimulationCommand.ChangeIntervalParamType);
			if (!simulationCmd.AddChild(param))
			{
				return EnumResultCode.E_FAIL;
			}
			param.ValueChanged(new ValueQT((uint)0, EnumQuality.GOOD, DateTime.Now));

			// repeat count parameter
			param = new MyDaAddressSpaceElement(DaStartStopSimulationCommand.RepeatCountParam, DaStartStopSimulationCommand.RepeatCountParamType);
			if (!simulationCmd.AddChild(param))
			{
				return EnumResultCode.E_FAIL;
			}
			param.ValueChanged(new ValueQT((uint)0, EnumQuality.GOOD, DateTime.Now));

			// incrementt parameter
			param = new MyDaAddressSpaceElement(DaStartStopSimulationCommand.IncrementParam, DaStartStopSimulationCommand.IncrementParamType);
			if (!simulationCmd.AddChild(param))
			{
				return EnumResultCode.E_FAIL;
			}
			param.ValueChanged(new ValueQT((uint)0, EnumQuality.GOOD, DateTime.Now));

			// Add Delete Items command
			DaDeleteItemsCommand deleteItemsCmd = new DaDeleteItemsCommand();
			deleteItemsCmd.Name = "DeleteItemsCommand";
			deleteItemsCmd.HasChildren = true;
			if (!itemFolder.AddChild(deleteItemsCmd))
			{
				return EnumResultCode.E_FAIL;
			}
			deleteItemsCmd.ValueChanged(new ValueQT(false, EnumQuality.GOOD, DateTime.Now));

			// Add items to list
			for (uint i = 0; i < itemCount; i++)
			{
				MyDaAddressSpaceElement item = new MyDaAddressSpaceElement(String.Format("TV_{0}", i), itemType);
				if (!itemFolder.AddChild(item))
				{
					return EnumResultCode.E_FAIL;
				}

				if (isAnalogItem)
				{
					DaConstantProperty euType = new DaConstantProperty(
						(int)EnumPropertyId.ITEM_EU_TYPE,
						"EUType",
						"7",
						new ValueQT(EnumEUType.ANALOG, EnumQuality.GOOD, DateTime.Now));
					euType.AccessRights = EnumAccessRights.READABLE;
					item.AddProperty(euType);

					DaProperty euHigh = new DaProperty();
					euHigh.Id = (int)EnumPropertyId.HIGH_EU;
					euHigh.Name = "High EU";
					euHigh.Description = "High EU";
					euHigh.ItemId = euHigh.Id.ToString();
					euHigh.Datatype = typeof(System.Double);
					euHigh.AccessRights = EnumAccessRights.READABLE;
					item.AddProperty(euHigh);

					DaProperty euLow = new DaProperty();
					euLow.Id = (int)EnumPropertyId.LOW_EU;
					euLow.Name = "Low EU";
					euLow.Description = "Low EU";
					euLow.ItemId = euLow.Id.ToString();
					euLow.Datatype = typeof(System.Double);
					euLow.AccessRights = EnumAccessRights.READABLE;
					item.AddProperty(euLow);
				}

				if (generateRandomValues)
				{
					// item.ValueChanged(m_generator.GetRandomValueQT(item.Datatype));
				}
				else
				{
					item.ValueChanged(new ValueQT(0, EnumQuality.GOOD, DateTime.Now));
				}
			}



			return EnumResultCode.S_OK;
		}

		#endregion

		#region Private Fields

		private int m_createdFolders = 0;
		private object m_lock = new object();
		#endregion
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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace TestServer
{
	/// <summary>
	/// Used to manage a CreateItems command
	/// </summary>
	class DaDeleteItemsCommand : DaAddressSpaceElement
	{
		#region Constructors

		public DaDeleteItemsCommand(
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

		public DaDeleteItemsCommand()
		{
			Datatype = typeof(bool);
			AccessRights = EnumAccessRights.READWRITEABLE;
			IoMode = EnumIoMode.POLL;
		}
		#endregion

		#region Public Properties



		#endregion

		#region Public Methods

		public EnumResultCode DeleteItems()
		{
			try
			{
				// Retreive items folder
				AddressSpaceElement itemsFolder = GetParent();

				// Get children
				ArrayList simulationItems = itemsFolder.GetChildren();

				// Search stopSimulation command item
				foreach (DaAddressSpaceElement item in simulationItems)
				{
					// stop simulation
					if (item is DaStartStopSimulationCommand)
					{
						((DaStartStopSimulationCommand)item).StopSimulation();
						break;
					}
				}

				AddressSpaceElement parentFolder = itemsFolder.GetParent();

				if (parentFolder.RemoveChild(itemsFolder))
				{
					return EnumResultCode.S_OK;
				}
				else
				{
					// Delete error
					return EnumResultCode.E_FAIL;
				}
			}
			catch
			{
				return EnumResultCode.E_FAIL;
			}
		}

		#endregion

		#region Private Methods

		#endregion

		#region Private Fields

		#endregion
	}
}

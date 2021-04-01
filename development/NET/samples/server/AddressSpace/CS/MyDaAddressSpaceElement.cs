using System;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace AddressSpace
{
	public class MyDaAddressSpaceElement : DaAddressSpaceElement
	{

		public MyDaAddressSpaceElement(
			string anItemID,
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle) :
			base(anItemID, aName, anUserData,
			anObjectHandle, aParentHandle)
		{
			this.AddedToAddressSpace += new AddedToAddressSpace(HandleAddedToAddressSpace);
			this.RemovedFromAddressSpace += new RemovedFromAddressSpace(HandleRemovedFromAddressSpace);
		}   //	end ctr

		public MyDaAddressSpaceElement()
		{
			this.AddedToAddressSpace += new AddedToAddressSpace(HandleAddedToAddressSpace);
			this.RemovedFromAddressSpace += new RemovedFromAddressSpace(HandleRemovedFromAddressSpace);
		}   //	end ctr

		public override int QueryProperties(out ArrayList aPropertyList)
		{

			aPropertyList = new ArrayList();

			DaProperty prop = new DaProperty();
			prop.Id = 5555;
			prop.AccessRights = EnumAccessRights.READABLE;
			prop.Datatype = typeof(UInt16);
			prop.Name = "type";
			prop.Description = "Object Type";
			if (ItemId == null)
			{
				prop.ItemId = "objectType";
			}
			else
			{
				prop.ItemId = ItemId + Application.Instance.PropertyDelimiter + "objectType";
			}   //	end if ... else

			DaProperty prop2 = new DaProperty();
			prop2.Id = 5556;
			prop2.AccessRights = EnumAccessRights.READABLE;
			prop2.Datatype = typeof(UInt16);
			prop2.Name = "type2";
			prop2.Description = "Object Type2";
			if (ItemId == null)
			{
				prop2.ItemId = "objectType2";
			}
			else
			{
				prop2.ItemId = ItemId + Application.Instance.PropertyDelimiter + "objectType2";
			}   //	end if ... else

			aPropertyList.Add(prop);
			aPropertyList.Add(prop2);

			return (int)EnumResultCode.S_OK;

		}   //	end QueryProperties

		public void HandleAddedToAddressSpace()
		{

			if (ItemId == "stringBased")
			{
				ObjectType = 20;
			}
			else if ((ItemId == "stringBased.N1") || (ItemId == "stringBased.N2"))
			{
				ObjectType = 21;
			}
			else if (ItemId == "stringBased.T1")
			{
				ObjectType = 22;
			}
			else if ((ItemId == "stringBased.N1.T1") ||
				(ItemId == "stringBased.N2.T1") ||
				(ItemId == "stringBased.N2.T2"))
			{
				ObjectType = 23;
			}
			else if (ItemId == "syntaxBased")
			{
				ObjectType = 40;
			}
			else if (ItemId.StartsWith("syntaxBased"))
			{
				Int32 index;
				index = ItemId.LastIndexOf(".T");
				if (index > 0)
				{
					ObjectType = 42; // T
				}
				else
				{
					ObjectType = 41; // N
				}   //	end if ... else
			}   //	end if ... else



			if (this.IoMode != EnumIoMode.NONE)
			{
				ValueQT initialValue = new ValueQT(0, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(initialValue);
			}

			OpcServer.AddressSpaceChanged.Set();

		}   //	end HandleAddedToAddressSpace

		public void HandleRemovedFromAddressSpace()
		{
			OpcServer.AddressSpaceChanged.Set();
		}   //	end RemovedFromAddressSpace

		private byte m_objectType;

		public byte ObjectType
		{
			get { return m_objectType; }
			set { m_objectType = value; }
		}   //	end ObjectType

	}   //	end class MyDaAddressSpaceElement

}   //	end namespace

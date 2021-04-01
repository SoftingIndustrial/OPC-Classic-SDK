using System;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace AddressSpace
{

	public class MyDaAddressSpaceRoot : DaAddressSpaceRoot
	{

		public override int QueryAddressSpaceElementData(string anElementID, out AddressSpaceElement anElement)
		{

			DaAddressSpaceElement element = new DaAddressSpaceElement();

			if (anElementID.StartsWith("stringBased"))
			{

				if (anElementID == "stringBased")
				{
					element.Name = anElementID;
					element.IoMode = EnumIoMode.NONE;
					element.HasChildren = true;
				}
				else if (anElementID == "stringBased.N1" ||
					anElementID == "stringBased.N2")
				{

					element.Name = anElementID.Substring(anElementID.LastIndexOf(".") + 1);
					element.AccessRights = EnumAccessRights.READWRITEABLE;
					element.Datatype = typeof(byte);
					element.HasChildren = true;
					element.IoMode = EnumIoMode.REPORT;
				}
				else if (anElementID == "stringBased.T1")
				{

					element.Name = anElementID.Substring(anElementID.LastIndexOf(".") + 1);
					element.AccessRights = EnumAccessRights.READWRITEABLE;
					element.Datatype = typeof(byte);
					element.HasChildren = false;
					element.IoMode = EnumIoMode.REPORT;
				}
				else if (anElementID.LastIndexOf("N1") > 0)
				{

					if (anElementID.Substring(anElementID.LastIndexOf("N1")).EndsWith("T1"))
					{

						element.Name = anElementID.Substring(anElementID.LastIndexOf(".") + 1);
						element.AccessRights = EnumAccessRights.READWRITEABLE;
						element.Datatype = typeof(byte);
						element.HasChildren = false;
						element.IoMode = EnumIoMode.REPORT;
					}
				}
				else if (anElementID.LastIndexOf("N2") > 0)
				{

					if (anElementID.Substring(anElementID.LastIndexOf("N2")).EndsWith("T1") ||
						anElementID.Substring(anElementID.LastIndexOf("N2")).EndsWith("T2"))
					{

						element.Name = anElementID.Substring(anElementID.LastIndexOf(".") + 1);
						element.AccessRights = EnumAccessRights.READWRITEABLE;
						element.Datatype = typeof(byte);
						element.HasChildren = false;
						element.IoMode = EnumIoMode.REPORT;
					}
				}
			}
			else if (anElementID.StartsWith("syntaxBased"))
			{

				if (anElementID == "syntaxBased")
				{
					element.Name = anElementID;
					element.IoMode = EnumIoMode.NONE;
					element.HasChildren = true;
				}

				Int32 index = anElementID.LastIndexOf(".T");
				if (index > 0)
				{

					element.Name = anElementID.Substring(index + 1);
					if (element.Name.Length == 2)
					{

						element.HasChildren = false;
						element.IoMode = EnumIoMode.POLL;
						element.AccessRights = EnumAccessRights.READWRITEABLE;
						element.Datatype = typeof(Int32);
					}
					else
					{
						element.Name = string.Empty;
					}
				}
				else
				{
					index = anElementID.LastIndexOf(".N");
					if (index > 0)
					{

						element.Name = anElementID.Substring(index + 1);
						if (element.Name.Length == 2)
						{
							element.HasChildren = true;
							element.IoMode = EnumIoMode.NONE;
						}
						else
						{
							element.Name = string.Empty;
						}
					}
				}
			}

			//	check if an element was succeeded
			if (element.Name == string.Empty)
			{

				anElement = null;
				return (int)EnumResultCode.E_BADTYPE;
			}   //	end if

			anElement = element;
			return (int)EnumResultCode.S_OK;

		}   //	end QueryAddressSpaceElementData


		public override int QueryAddressSpaceElementChildren(string anElementID, ArrayList aChildrenList)
		{

			MyDaAddressSpaceElement child = null;

			if (anElementID.Length == 0)
			{
				child = new MyDaAddressSpaceElement();
				child.Name = "stringBased";
				child.IoMode = EnumIoMode.NONE;
				child.ItemId = "stringBased";
				child.HasChildren = true;
				aChildrenList.Add(child);
			}
			else if (anElementID == "stringBased")
			{
				child = new MyDaAddressSpaceElement();
				child.Name = "N1";
				child.ItemId = anElementID + Application.Instance.AddressSpaceDelimiter + child.Name;
				child.AccessRights = EnumAccessRights.READWRITEABLE;
				child.Datatype = typeof(byte[]);
				child.IoMode = EnumIoMode.REPORT;
				child.HasChildren = true;
				aChildrenList.Add(child);

				child = new MyDaAddressSpaceElement();
				child.Name = "N2";
				child.ItemId = anElementID + Application.Instance.AddressSpaceDelimiter + child.Name;
				child.AccessRights = EnumAccessRights.READWRITEABLE;
				child.Datatype = typeof(byte[]);
				child.IoMode = EnumIoMode.REPORT;
				child.HasChildren = true;
				aChildrenList.Add(child);

				child = new MyDaAddressSpaceElement();
				child.Name = "T1";
				child.ItemId = anElementID + Application.Instance.AddressSpaceDelimiter + child.Name;
				child.AccessRights = EnumAccessRights.READWRITEABLE;
				child.Datatype = typeof(byte);
				child.IoMode = EnumIoMode.REPORT;
				child.HasChildren = false;
				aChildrenList.Add(child);
			}
			else if (anElementID == "stringBased.N1")
			{
				child = new MyDaAddressSpaceElement();
				child.Name = "T1";
				child.ItemId = anElementID + Application.Instance.AddressSpaceDelimiter + child.Name;
				child.AccessRights = EnumAccessRights.READWRITEABLE;
				child.Datatype = typeof(byte);
				child.IoMode = EnumIoMode.REPORT;
				child.HasChildren = false;
				aChildrenList.Add(child);
			}
			else if (anElementID == "stringBased.N2")
			{

				child = new MyDaAddressSpaceElement();
				child.Name = "T1";
				child.ItemId = anElementID + Application.Instance.AddressSpaceDelimiter + child.Name;
				child.AccessRights = EnumAccessRights.READWRITEABLE;
				child.Datatype = typeof(byte);
				child.IoMode = EnumIoMode.REPORT;
				child.HasChildren = false;
				aChildrenList.Add(child);

				child = new MyDaAddressSpaceElement();
				child.Name = "T2";
				child.ItemId = anElementID + Application.Instance.AddressSpaceDelimiter + child.Name;
				child.AccessRights = EnumAccessRights.READWRITEABLE;
				child.Datatype = typeof(byte);
				child.IoMode = EnumIoMode.REPORT;
				child.HasChildren = false;
				aChildrenList.Add(child);
			}   //	end if ... else

			return (int)EnumResultCode.S_OK;
		}

		/// <summary>
		/// QueryAddressSpaceElementProperties
		/// </summary>		
		public override int QueryAddressSpaceElementProperties(string anElementID, out ArrayList aPopertyList)
		{

			aPopertyList = null;

			if (anElementID.StartsWith("stringBased"))
			{

				aPopertyList = new ArrayList();

				DaProperty prop = new DaProperty();
				prop.Id = 5555;
				prop.AccessRights = EnumAccessRights.READABLE;
				prop.Datatype = typeof(short);
				prop.Name = "type";
				prop.ItemId = anElementID + Application.Instance.PropertyDelimiter + "objectType";
				prop.Description = "Object Type";

				DaProperty prop2 = new DaProperty();
				prop2.Id = 5556;
				prop2.AccessRights = EnumAccessRights.READABLE;
				prop2.Datatype = typeof(short);
				prop2.Name = "type2";
				prop2.ItemId = anElementID + Application.Instance.PropertyDelimiter + "objectType2";
				prop2.Description = "Object Type2";

				aPopertyList.Add(prop);
				aPopertyList.Add(prop2);

			}   //	end if

			return (int)EnumResultCode.S_OK;

		}   //	end QueryAddressSpaceElementProperties

	}   //	end class MyAddressSpaceRoot

}   //	end namespace

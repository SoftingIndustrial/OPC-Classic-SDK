using System;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.ComponentModel;

namespace TestClientGui.Elements
{
	[TypeConverter( typeof( ExpandableObjectConverter ) )]
	public class DaAddressSpaceElement
	{
		public string ItemId { get; set; }

		public string ItemPath { get; set; }

		public string Name { get; set; }

		public bool? IsItem { get; set; }

		public bool? IsBranch { get; set; }

		public bool? IsLeaf { get; set; }

		public static Softing.OPCToolbox.Client.DaAddressSpaceElement[] CreateSDKCollection( List<DaAddressSpaceElement> list )
		{
			Softing.OPCToolbox.Client.DaAddressSpaceElement[] l = new Softing.OPCToolbox.Client.DaAddressSpaceElement[list.Count];

			for( int i = 0; i < list.Count; i++ )
			{
				l[i] = list[i].GetWrapped();
			}

			return l;
		}

		public static List<DaAddressSpaceElement> CreateList( Softing.OPCToolbox.Client.DaAddressSpaceElement[] collection )
		{
			List<DaAddressSpaceElement> l = new List<DaAddressSpaceElement>( collection.Length );

			for( int i = 0; i < collection.Length; i++ )
			{
				l.Add( new DaAddressSpaceElement( collection[i] ) );
			}

			return l;
		}

		public DaAddressSpaceElement()
		{
		}

		public DaAddressSpaceElement( Softing.OPCToolbox.Client.DaAddressSpaceElement value )
		{
			ItemId = value.ItemId;
			ItemPath = value.ItemPath;
			Name = value.Name;
			IsItem = value.IsItem;
			IsBranch = value.IsBranch;
			IsLeaf = value.IsLeaf;
		}

		public Softing.OPCToolbox.Client.DaAddressSpaceElement GetWrapped()
		{
			Softing.OPCToolbox.Client.EnumAddressSpaceElementType type = Softing.OPCToolbox.Client.EnumAddressSpaceElementType.ALL;

			if( IsBranch.HasValue && IsBranch.Value )
				type = Softing.OPCToolbox.Client.EnumAddressSpaceElementType.BRANCH;
			else if( IsLeaf.HasValue && IsLeaf.Value )
				type = Softing.OPCToolbox.Client.EnumAddressSpaceElementType.LEAF;
			else if( IsItem.HasValue && IsItem.Value )
				type = Softing.OPCToolbox.Client.EnumAddressSpaceElementType.DA_ITEM;

			return new Softing.OPCToolbox.Client.DaAddressSpaceElement( type,
				Name, ItemId, ItemPath, 0, null );
		}
	}
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using Softing.OPCToolbox;

namespace TestClientGui.Support
{
	[TypeConverter(typeof(ExpandableObjectConverter))]
	public class DaProperty
	{
		public Type DataType { get; set; }
		public string Description { get; set; }
		public uint Id { get; set; }
		public string ItemId { get; set; }
		public string ItemPath { get; set; }
		public string Name { get; set; }
		public int Result { get; set; }
		public ValueQT ValueQT { get; set; }

		public DaProperty()
		{ }

		public DaProperty(Softing.OPCToolbox.Client.DaProperty daProperty)
		{
			DataType = daProperty.DataType;
			Description = daProperty.Description;
			Id = daProperty.Id;
			ItemId = daProperty.ItemId;
			ItemPath = daProperty.ItemPath;
			Name = daProperty.Name;
			Result = daProperty.Result;
			ValueQT = new ValueQT(daProperty.ValueQT);
		}
	}

	public class ValueQT
	{
		public EnumQuality Quality { get; set; }
		public DateTime TimeStamp { get; set; }
		public object Data { get; set; }

		public ValueQT()
		{
			Quality = EnumQuality.GOOD;
		}

		public ValueQT(Softing.OPCToolbox.ValueQT valueQT)
		{
			Quality = valueQT.Quality;
			TimeStamp = valueQT.TimeStamp;
			Data = valueQT.Data;
		}
	}
}

using System;
using System.Xml.Serialization;
using System.ComponentModel;

namespace TestClientGui.Elements
{
	public enum EnumAccessRights
	{
		NOTSET = 0,
		READABLE = Softing.OPCToolbox.EnumAccessRights.READABLE,
		WRITEABLE = Softing.OPCToolbox.EnumAccessRights.WRITEABLE,
		READWRITEABLE = Softing.OPCToolbox.EnumAccessRights.READWRITEABLE
	}

	public class DaAddressSpaceElementBrowseOptions
	{
		#region Public Properties
		[DefaultValue( EnumAccessRights.NOTSET )]
		public EnumAccessRights AccessRightsFilter
		{
			get
			{
				return (EnumAccessRights) Wrapped.AccessRightsFilter;
			}

			set
			{
				Wrapped.AccessRightsFilter = (Softing.OPCToolbox.EnumAccessRights) value;
			}
		}

		public string DataTypeFilter
		{
			get
			{
				return Wrapped.DataTypeFilter != null ? Wrapped.DataTypeFilter.FullName : null;
			}
			set
			{
				Wrapped.DataTypeFilter = value != null ? Type.GetType( value ) : null;
			}
		}

		public uint? MaxElements
		{
			get
			{
				return Wrapped.MaxElements;
			}
			set
			{
				if( value.HasValue )
					Wrapped.MaxElements = value.Value;
				else
					Wrapped.MaxElements = default( uint );
			}
		}

		public bool? RetrieveItemId
		{
			get
			{
				return Wrapped.RetrieveItemId;
			}
			set
			{
				if( value.HasValue )
					Wrapped.RetrieveItemId = value.Value;
				else
					Wrapped.RetrieveItemId = default( bool );
			}
		}

		[DefaultValue( false )]
		public bool? ReturnProperties
		{
			get
			{
				return Wrapped.ReturnProperties;
			}
			set
			{
				if( value.HasValue )
					Wrapped.ReturnProperties = value.Value;
				else
					Wrapped.ReturnProperties = default( bool );
			}
		}

		[DefaultValue( false )]
		public bool? ReturnPropertyValues
		{
			get
			{
				return Wrapped.ReturnPropertyValues;
			}
			set
			{
				if( value.HasValue )
					Wrapped.ReturnPropertyValues = value.Value;
				else
					Wrapped.ReturnPropertyValues = default( bool );
			}
		}

		public string VendorFilter
		{
			get
			{
				return Wrapped.VendorFilter;
			}
			set
			{
				Wrapped.VendorFilter = value;
			}
		}

		public string ElementNameFilter
		{
			get
			{
				return Wrapped.ElementNameFilter;
			}
			set
			{
				Wrapped.ElementNameFilter = value;
			}
		}

		[DefaultValue( Softing.OPCToolbox.Client.EnumAddressSpaceElementType.ALL )]
		public Softing.OPCToolbox.Client.EnumAddressSpaceElementType ElementTypeFilter
		{
			get
			{
				return Wrapped.ElementTypeFilter;
			}

			set
			{
				Wrapped.ElementTypeFilter = value == 0 ? Softing.OPCToolbox.Client.EnumAddressSpaceElementType.ALL : value;
			}
		}

		[DefaultValue( false )]
		public bool? ForceBrowseUp
		{
			get
			{
				return Wrapped.ForceBrowseUp;
			}
			set
			{
				if( value.HasValue )
					Wrapped.ForceBrowseUp = value.Value;
				else
					Wrapped.ForceBrowseUp = default( bool );
			}
		}
		#endregion

		public DaAddressSpaceElementBrowseOptions()
		{
			Wrapped = new Softing.OPCToolbox.Client.DaAddressSpaceElementBrowseOptions();
		}

		public DaAddressSpaceElementBrowseOptions( Softing.OPCToolbox.Client.DaAddressSpaceElementBrowseOptions value )
		{
			Wrapped = value;
		}

		[XmlIgnore]
		public Softing.OPCToolbox.Client.DaAddressSpaceElementBrowseOptions Wrapped;
	}
}

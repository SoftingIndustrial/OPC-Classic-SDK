using System;
using System.Collections;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Security.Permissions;
using Softing.OPCToolbox.Client;

namespace Softing.OPCToolbox.Client.Control
{
	/// <summary>
	/// A <b>DaSession</b> object used with a <see cref="DataControl"/>.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="ControlDaSession"]/doc/*'
	/// />
	[SerializableAttribute]
	public class ControlDaSession : DaSession
	{
		#region	//	Public Constructors
		//-----------------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="ControlDaSession"/> class with
		/// the value indicated by a session url.
		/// </summary>
		/// <param name="url">The session url.</param>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSession"]/constructor[@name="ControlDaSession"]/doc/*'
		/// />
		public ControlDaSession(string url) : base(url)
		{
			m_url = url;
		}

		//-
		#endregion

		#region //	Protected Attributes
		//---------------------------

		/// <summary>
		/// The server's url.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSession"]/field[@name="m_url"]/doc/*'
		/// />
		protected string m_url = string.Empty;

		/// <summary>
		/// A DA binary license.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSession"]/field[@name="m_binaryLicenseDa"]/doc/*'
		/// />
		internal string m_binaryLicenseDa = string.Empty;

		/// <summary>
		/// A XML-DA binary license.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSession"]/field[@name="m_binaryLicenseXmlDa"]/doc/*'
		/// />
		internal string m_binaryLicenseXmlDa = string.Empty;

		//-
		#endregion

		#region //	Public Properties
		//---------------------------

		/// <summary>
		/// Gets or sets the url for the <see cref="ControlDaSession"/> object.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSession"]/property[@name="StoredUrl"]/doc/*'
		/// />
		public string StoredUrl
		{
			set { m_url = value; } //	end set
			get { return m_url; } //	end get
		} //	StoredUrl


		//-
		#endregion

		#region ISerializable
		/// <summary>
		/// Contructs a <b>ControlDaSession</b> object by de-serializing it from the stream.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSession"]/method[@name="ControlDaSession"]/doc/*'
		/// />
		protected ControlDaSession(SerializationInfo info, StreamingContext context) :
			base(info, context)
		{
			m_url = info.GetString("m_url");
			m_binaryLicenseDa = info.GetString("m_binaryLicenseDa");
			m_binaryLicenseXmlDa = info.GetString("m_binaryLicenseXmlDa");
		}

		/// <summary>
		/// Serializes a <b>ControlDaSession</b> into a stream.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSession"]/method[@name="GetObjectData"]/doc/*'
		/// />
		[SecurityPermission(SecurityAction.Demand, SerializationFormatter = true)]
		public override void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			base.GetObjectData(info, context);
			info.AddValue("m_url", m_url);
			info.AddValue("m_binaryLicenseDa", m_binaryLicenseDa);
			info.AddValue("m_binaryLicenseXmlDa", m_binaryLicenseXmlDa);
		}

		#endregion
	} // end class Session

	/// <summary>
	/// A <b>DaSubscription</b> object used with a <see cref="DataControl"/>.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="ControlDaSubscription"]/doc/*'
	/// />
	[SerializableAttribute]
	public class ControlDaSubscription : DaSubscription
	{
		#region	//	Public Constructors
		//-----------------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="ControlDaSubscription"/> class with
		/// the value indicated by an update rate and a <see cref="ControlDaSession"/> object to whom to be added.
		/// </summary>
		/// <param name="updateRate">The cyclic rate that the  active <see cref="ControlDaItem"/> objects have to be read.</param>
		/// <param name="parentSession">The session to whom the subscription is to be added.</param>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSubscription"]/constructor[@name="ControlDaSubscription"]/doc/*'
		/// />
		public ControlDaSubscription(
			uint updateRate,
			ControlDaSession parentSession)
			:
				base(
				updateRate,
				parentSession)
		{
			m_updateRate = updateRate;
		} //	end ctr


		//-
		#endregion

		#region //	Protected Attributes
		//---------------------------

		/// <summary>
		/// A symbolic name for the <see cref="ControlDaSubscription"/> object that describes its semantic significance.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSubscription"]/field[@name="m_name"]/doc/*'
		/// />
		protected string m_name = String.Empty;

		/// <summary>
		///The speed (in milliseconds)requested for scanning the active item objects.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSubscription"]/field[@name="m_updateRate"]/doc/*'
		/// />
		protected uint m_updateRate = 0;

		/// <summary>
		/// Shows whether the subscription is activated or not.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSubscription"]/field[@name="m_isActivated"]/doc/*'
		/// />
		protected bool m_isActivated = false;

		//-
		#endregion

		#region //	Public Properties
		//---------------------------

		/// <summary>
		/// Gets or sets a symbolic name for the <see cref="ControlDaSubscription"/> object that describes its semantic significance.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSubscription"]/property[@name="StoredName"]/doc/*'
		/// />
		public string StoredName
		{
			get { return this.m_name; } //	end get
			set { m_name = value; } //	end set
		} //	end StoredName


		/// <summary>
		/// Gets or sets the speed (in milliseconds)requested for scanning the active item objects.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSubscription"]/property[@name="StoredUpdateRate"]/doc/*'
		/// />
		public uint StoredUpdateRate
		{
			get { return this.m_updateRate; } //	end get
			set { m_updateRate = value; } //	end set
		} //	end StoredUpdateRate


		/// <summary>
		/// Gets or sets whether the subscription is active or not.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSubscription"]/property[@name="IsActivated"]/doc/*'
		/// />
		public bool IsActivated
		{
			get { return m_isActivated; } //	end get
			set { m_isActivated = value; } //	end set
		} //	end IsActivated
		//-
		#endregion

		#region ISerializable
		/// <summary>
		///  Contructs a <b>ControlDaSubscription</b> object by de-serializing it from the stream.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSubscription"]/method[@name="ControlDaSubscription"]/doc/*'
		/// />
		protected ControlDaSubscription(SerializationInfo info, StreamingContext context) :
			base(info, context)
		{
			m_name = info.GetString("m_name");
			m_updateRate = info.GetUInt32("m_updateRate");
			m_isActivated = info.GetBoolean("m_isActivated");
		}

		/// <summary>
		/// Serializes a <b>ControlDaSubscription</b> object into a stream.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaSubscription"]/method[@name="GetObjectData"]/doc/*'
		/// />
		[SecurityPermission(SecurityAction.Demand, SerializationFormatter = true)]
		public override void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			base.GetObjectData(info, context);
			info.AddValue("m_name", m_name);
			info.AddValue("m_updateRate", m_updateRate);
			info.AddValue("m_isActivated", m_isActivated);
		}

		#endregion
	} // end class Subscription

	/// <summary>
	/// A <b>DaItem</b> object used with a <see cref="DataControl"/>.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="ControlDaItem"]/doc/*'
	/// />
	[SerializableAttribute]
	public class ControlDaItem : DaItem, IEditableObject
	{
		#region // Public Constructors
		//----------------------------

		/// <summary>
		/// Initializes a new instance of <see cref="ControlDaItem"/> class with
		/// the value indicated by an item identifier and a <see cref="ControlDaSubscription"/> object to whom to be added.
		/// </summary>
		/// <param name="itemId">An unique identifier for the item in the server's address space.</param>
		/// <param name="parentSubscription">The subscription to whom the item is to be added.</param>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/constructor[@name="ControlDaItem"]/doc/*'
		/// />
		public ControlDaItem(
			string itemId,
			ControlDaSubscription parentSubscription)
			:
				base(
				itemId,
				parentSubscription)
		{
			m_ID = itemId;
		} //	end ctr


		internal ControlDaItem()
		{
		} //	end ctr

		//-
		#endregion

		#region // Protected Attributes
		//----------------------------

		/// <summary>
		/// An unique identifier for the <see cref="ControlDaItem"/> object in the server's address space.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/field[@name="m_ID"]/doc/*'
		/// />
		protected string m_ID = string.Empty;

		//-
		#endregion

		#region //	Public Properties
		//---------------------------

		/// <summary>
		/// Gets or sets an unique identifier for the <see cref="ControlDaItem"/> object in the server's address space.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/property[@name="StoredId"]/doc/*'
		/// />
		public string StoredId
		{
			get { return this.m_ID; } //	end get
			set { m_ID = value; } //	end set
		} //	end StoredName


		//-
		#endregion

		#region ISerializable
		/// <summary>
		///  Constructs an <b>ControlDaItem</b> object by de-serializing it from the stream.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/method[@name="ControlDaItem"]/doc/*'
		/// />
		public ControlDaItem(SerializationInfo info, StreamingContext context) :
			base(info, context)
		{
			this.m_ID = info.GetString("m_ID");
		} //	end ctr

		/// <summary>
		/// Serializes an <b>ControlDaItem</b> object into a stream.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/method[@name="GetObjectData"]/doc/*'
		/// />
		[SecurityPermission(SecurityAction.Demand, SerializationFormatter = true)]
		public override void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			base.GetObjectData(info, context);
			info.AddValue("m_ID", m_ID);
		}

		#endregion

		#region IEditableObject Interface
		private Hashtable m_OriginalData;
		private bool m_Editing = false;
		private bool m_IsAddNew = false;

		/// <summary>
		/// Represents a method that will handle the <b>CancelAddNew</b> event of an <b>ControlDaItem</b>.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/delegate[@name="CancelAddNewEventHandler"]/doc/*'
		/// />
		public delegate void CancelAddNewEventHandler(ControlDaItem sender, bool Remove);

		/// <summary>
		/// Signals to the collection that the item should be removed from the list.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/event[@name="CancelAddNew"]/doc/*'
		/// />
		public event CancelAddNewEventHandler CancelAddNew;

		/// <summary>
		/// Initializes a new instance of the <b>ControlDaItem</b> class.
		/// This constructor will be called when the user chooses to add an item to the collection via the IBindingList.AddNew method.
		/// The boolean value tells the instance of the <b>ControlDaItem</b> class that it was created at the user's request.
		/// </summary>
		/// <param name="IsAddNew">Specifies is the item was created at the user's request.</param>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/method[@name="ControlDaItem"]/doc/*'
		/// />
		public ControlDaItem(bool IsAddNew)
		{
			m_IsAddNew = IsAddNew;
		}

		/// <summary>
		/// Begins an edit on an object.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/method[@name="BeginEdit"]/doc/*'
		/// />
		public void BeginEdit()
		{
			if (!(m_Editing))
			{
				m_Editing = true;
				m_OriginalData = new Hashtable();
				PropertyDescriptorCollection Properties = TypeDescriptor.GetProperties(this);
				foreach (System.ComponentModel.PropertyDescriptor prop in Properties)
				{
					m_OriginalData.Add(prop, prop.GetValue(this));
				}
			}
		}

		/// <summary>
		/// Called when the user navigates to an item within the control, but only when editing is allowed by the collection.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/method[@name="BeginEdit"]/doc/*'
		/// />
		void System.ComponentModel.IEditableObject.BeginEdit()
		{
			BeginEdit();
		}

		/// <summary>
		/// Pushes changes since the last BeginEdit or IBindingList.AddNew call into the underlying object.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/method[@name="EndEdit"]/doc/*'
		/// />
		public void EndEdit()
		{
			if (m_Editing)
			{
				if ((m_IsAddNew))
				{
					if (null != CancelAddNew) CancelAddNew(this, false);
				}
				m_OriginalData = null;
				m_Editing = false;
			}
		}

		/// <summary>
		/// Called when the user navigates away from an item and any changes the user made should be saved.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/method[@name="EndEdit"]/doc/*'
		/// />
		void System.ComponentModel.IEditableObject.EndEdit()
		{
			EndEdit();
		}

		/// <summary>
		/// Discards changes since the last BeginEdit call.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/method[@name="CancelEdit"]/doc/*'
		/// />
		public void CancelEdit()
		{
			if (m_Editing)
			{
				if ((m_IsAddNew))
				{
					if (null != CancelAddNew) CancelAddNew(this, true);
				}
				else
				{
					PropertyDescriptor prop = null;
					foreach (System.Collections.DictionaryEntry entry in m_OriginalData)
					{
						prop = ((PropertyDescriptor) (entry.Key));
						prop.SetValue(this, entry.Value);
					}
					m_OriginalData = null;
					m_Editing = false;
				}
			}
		}

		/// <summary>
		/// Called when the user navigates away and the changes should be discarded.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ControlDaItem"]/method[@name="CancelEdit"]/doc/*'
		/// />
		void System.ComponentModel.IEditableObject.CancelEdit()
		{
			CancelEdit();
		}

		#endregion
	} // end class ControlItem
} //	end namespace
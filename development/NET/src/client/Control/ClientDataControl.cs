using System.Collections;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;
using System;
using System.Data;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace Softing.OPCToolbox.Client.Control
{
	/// <summary>
	/// Provides a control for collecting data on different items.
	/// It can be bound to other Windows Forms Controls at run time.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="DataControl"]/doc/*'
	/// />
	public class DataControl :
		IList,
		ICollection,
		IEnumerable,
		IComponent,
		IBindingList,
		ITypedList
	{
		#region Public Constructor
		//-------------------------

		/// <summary>
		/// Creates a new instance of the <see cref="DataControl"/> class.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DataControl"]/constructor[@name="DataControl"]/doc/*'
		/// />
		public DataControl()
		{
			this.m_list = new ArrayList();
		} //  end constructor

		//--

		#endregion

		#region Public Events
		//-------------------

		/// <summary>
		/// Occurs when the client is notified about the value changes for the active <see cref="ControlDaItem">items</see> of the
		/// <see cref="DataControl">DataControl.</see>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DataControl"]/event[@name="DataChanged"]/doc/*'
		/// />
		public event ControlDataChangedEventHandler DataChanged;

		//-
		#endregion

		#region Private Attributes
		//--------------------------

		private ControlDaSession m_session = new ControlDaSession(string.Empty);
		private ControlDaSession m_newSession = null;
		private Application m_instance = Application.Instance;
		private ArrayList m_list = new ArrayList();
		//-
		#endregion

		#region Private Methods
		//-----------------------

		private static bool HandleSessionShutdownWithReconnection(ObjectSpaceElement obj, string reason)
		{
			MessageBox.Show("Server shutdown " + reason + "!");

			return true; // reconnect automatically
		}

		private static bool HandleSessionShutdownWithoutReconnection(ObjectSpaceElement obj, string reason)
		{
			MessageBox.Show("Server shutdown " + reason);

			return false; // do not reconnect automatically
		}

		private void ControlSubscription_DataChange(DaSubscription aDaSubscription, DaItem[] changedItems, ValueQT[] newValues,
		                                            int[] results)
		{
			try
			{
				int count = changedItems.Length;
				ControlDaItem[] controlItems = new ControlDaItem[count];
				for (int i = 0; i < count; i++)
				{
					if (this.Contains(changedItems[i]))
					{
						int index = this.IndexOf(changedItems[i]);
						((DaItem) this[index]).ValueQT = newValues[i];
						if (null != ListChanged)
						{
							ListChanged(this, new ListChangedEventArgs(ListChangedType.ItemChanged, i));
						} //	end if
					} //	end if
					controlItems[i] = (ControlDaItem) changedItems[i];
				} //	end for
				this.OnDataChange((ControlDaSubscription) aDaSubscription, controlItems, newValues, results);
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "DataControl.DataControl_DataChange", exc.Message);
			}
		}

		//-
		#endregion

		#region Public Methods
		//----------------------

		/// <summary>
		/// Starts the <see cref="DataControl">DataControl.</see>
		/// </summary>
		/// <remarks>
		/// After starting the <B>DataControl</B> the client will be notified about the <see cref="ValueQT">value</see> changes for all the
		/// <see cref="ControlDaItem">items</see> selected in the configuration phase. If an item is bound to a
		///	Windows Forms Control then the new values will be displayed in this one.
		/// </remarks>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DataControl"]/method[@name="Start"]/doc/*'
		/// />
		public void Start()
		{
			try
			{
				if (m_session == null)
					return;
				this.Clear();

				if (this.BinaryLicenseDa != string.Empty)
				{
					m_instance.Activate(EnumFeature.DA_CLIENT, this.BinaryLicenseDa);
				}

				if (this.BinaryLicenseXmlDa != string.Empty)
				{
					m_instance.Activate(EnumFeature.DA_CLIENT, this.BinaryLicenseXmlDa);
				}

				m_instance.Initialize();

				m_newSession = new ControlDaSession(m_session.StoredUrl);
				m_newSession.Connect(false, true, new ExecutionOptions());

				m_newSession.ShutdownRequest += new ShutdownEventHandler(HandleSessionShutdownWithReconnection);

				if (m_session.SubscriptionList.Length == 0)
				{
					return;
				}

				foreach (ControlDaSubscription currentSubscription in m_session.SubscriptionList)
				{
					ControlDaSubscription controlSubscription = new ControlDaSubscription(
						currentSubscription.StoredUpdateRate,
						m_newSession);

					if (controlSubscription.Valid)
					{
						controlSubscription.Name = currentSubscription.StoredName;
						controlSubscription.StoredName = controlSubscription.Name;
						controlSubscription.StoredUpdateRate = controlSubscription.RequestedUpdateRate;
						controlSubscription.IsActivated = currentSubscription.IsActivated;

						controlSubscription.DataChanged += new DataChangedEventHandler(ControlSubscription_DataChange);
						DaItem[] items = currentSubscription.ItemList;
						for (int i = 0; i < items.Length; i++)
						{
							ControlDaItem controlItem = new ControlDaItem(
								((ControlDaItem) items[i]).StoredId,
								controlSubscription);

							this.Add(controlItem);
						} //	end for

						if (controlSubscription.IsActivated)
						{
							controlSubscription.Connect(true, true, new ExecutionOptions());
						} //	end if
					} //	end if
				} //	end foreach

				m_session.Handle = m_newSession.Handle;
			}
			catch (Exception exc)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DataControl.Start",
					exc.ToString());
			}
		} //	end Start


		/// <summary>
		/// Stops the <see cref="DataControl">DataControl.</see>
		/// </summary>
		/// <remarks>
		/// The connection with the server is closing down. No values will be read for the items
		///	that belong to the <b>DataControl.</b>
		/// </remarks>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DataControl"]/method[@name="Stop"]/doc/*'
		/// />
		public void Stop()
		{
			try
			{
				if (m_newSession == null)
					return;
				foreach (ControlDaSubscription currentSubscription in m_session.SubscriptionList)
				{
					currentSubscription.DataChanged -= new DataChangedEventHandler(ControlSubscription_DataChange);
				}

				m_newSession.ShutdownRequest -= new ShutdownEventHandler(HandleSessionShutdownWithoutReconnection);
				m_newSession.Disconnect(new ExecutionOptions());
				m_session.Disconnect(new ExecutionOptions());

				foreach (ControlDaSubscription subscription in m_newSession.SubscriptionList)
				{
					foreach (ControlDaItem item in subscription.ItemList)
					{
						subscription.RemoveDaItem(item);
					} //	end foreach
					m_newSession.RemoveDaSubscription(subscription);
				} //	end foreach

				m_instance.RemoveDaSession(m_newSession);
				m_instance.Terminate();

				m_newSession = null;
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "DataControl.Stop", exc.ToString());
			}
		}

		///<summary>
		///Gets the <see cref="ControlDaItem">item</see> that belongs to a given <see cref="ControlDaSubscription">subscription.</see> The
		///item is uniquely identified by its ID and the subscription by its name.
		///</summary>
		///<param name="subscriptionName">The name of the subscription to which the item belongs.</param>
		///<param name="itemID">The item's unique identifier.</param>
		///<return>A <b>ControlDaItem</b> that has the given ID and belongs to the given subscription.</return>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DataControl"]/method[@name="GetItem"]/doc/*'
		/// />
		public ControlDaItem GetItem(string subscriptionName, string itemID)
		{
			try
			{
				for (int i = 0; i < this.Count; i++)
				{
					if ((this[i].StoredId == itemID) && (this[i].DaSubscription.Name == subscriptionName))
					{
						return this[i];
					}
				}
			}
			catch (Exception exc)
			{
				m_instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "DataControl.GetItem", exc.ToString());
			}
			return null;
		}

		//-
		#endregion

		#region Internal Methods
		//----------------------

		/// <summary>
		/// Called when the active <see cref="ControlDaItem">items</see> of a <see cref="ControlDaSubscription">subscription</see> change their values.
		/// </summary>
		/// <param name="aDaSubscription"></param>
		/// <param name="items">A list of items bellonging to the subscription.</param>
		/// <param name="values">A list with the new item values.</param>
		/// <param name="results">A list with the result of value change for each item.</param>
		internal void OnDataChange(ControlDaSubscription aDaSubscription, ControlDaItem[] items, ValueQT[] values,
		                           int[] results)
		{
			try
			{
				if (DataChanged != null)
				{
					DataChanged(aDaSubscription, items, values, results);
				}
			}
			catch (Exception exc)
			{
				m_instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DataControl.OnDataChange",
					exc.ToString());
			}
		}

		/// <summary>
		/// Validates the given parameter. If it is null an execption is thrown.
		/// </summary>
		/// <param name="value"></param>
		internal void OnValidate(object value)
		{
			if (value == null)
			{
				throw new ArgumentNullException("value");
			} //	end if
		} //  end OnValidate

		/// <summary>
		/// Adds a <see cref="ControlDaItem">ControlDaItem</see> to the end of the collection.
		/// </summary>
		/// <param name="item">The <see cref="ControlDaItem">ControlDaItem</see> to be added to the end of the collection.</param>
		internal void Add(ControlDaItem item)
		{
			this.List.Add(item);
		} //  end Add

		/// <summary>
		/// Determines whether the collection contains a specific element.
		/// </summary>
		/// <param name="item">The <see cref="ControlDaItem">ControlDaItem</see> to locate in the collection.</param>
		/// <returns><b>true</b> if the collection contains the specified value; otherwise, <b>false</b>.</returns>
		internal bool Contains(DaItem item)
		{
			return this.List.Contains(item);
		}

		/// <summary>
		/// Searches for the specified <see cref="ControlDaItem">ControlDaItem</see> and returns the zero-based index of the first occurrence within the entire collection.
		/// </summary>
		/// <param name="item">The <b>ControlDaItem</b> to locate in the collection.</param>
		/// <returns>The zero-based index of the first occurrence of value within the entire collection, if found; otherwise, -1.</returns>
		internal int IndexOf(DaItem item)
		{
			return this.List.IndexOf(item);
		}

		/// <summary>
		/// Inserts a <see cref="ControlDaItem">ControlDaItem</see> into the collection at the specified index.
		/// </summary>
		/// <param name="index">The zero-based index at which the item should be inserted. </param>
		/// <param name="item">The <b>ControlDaItem</b> to insert.</param>
		internal void Insert(int index, ControlDaItem item)
		{
			this.List.Insert(index, item);
		}

		/// <summary>
		/// Gets and sets the <see cref="ControlDaItem">ControlDaItem</see> at the specified index.
		/// </summary>
		internal ControlDaItem this[int index]
		{
			get { return ((ControlDaItem) (this.List[index])); }
			set { this.List[index] = value; }
		}

		/// <summary>
		/// Removes the first occurrence of a specific <see cref="ControlDaItem">ControlDaItem</see> from the collection.
		/// </summary>
		/// <param name="item"></param>
		internal void Remove(ControlDaItem item)
		{
			this.List.Remove(item);
		}

		internal IList List
		{
			get { return this; }
		}

		//-
		#endregion

		#region	Public Properties
		//----------------------------------

		/// <summary>
		/// A client - Data Access server session. The server is the one chosen in the configuration phase.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DataControl"]/property[@name="Session"]/doc/*'
		/// />
		[Bindable(true)]
		[BrowsableAttribute(true)]
		[EditorAttribute(typeof (ValueEditor), typeof (System.Drawing.Design.UITypeEditor))]
		[DesignerSerializationVisibility(System.ComponentModel.DesignerSerializationVisibility.Visible)]
		public ControlDaSession Session
		{
			get { return m_session; }
			set { m_session = value; }
		}

		/// <summary>
		/// Gets or sets a binary license key for the DA.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DataControl"]/property[@name="BinaryLicenseDa"]/doc/*'
		/// />
		public string BinaryLicenseDa
		{
			get { return m_session.m_binaryLicenseDa; }
			set { m_session.m_binaryLicenseDa = value; }
		} //  BinaryLicenseDa

		/// <summary>
		/// Gets or sets a binary license key for the XML-DA.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DataControl"]/property[@name="BinaryLicenseXmlDa"]/doc/*'
		/// />
		public string BinaryLicenseXmlDa
		{
			get { return m_session.m_binaryLicenseXmlDa; }
			set { m_session.m_binaryLicenseXmlDa = value; }
		} //  BinaryLicenseXmlDa

		/// <summary>
		/// Gets the collection's inner list.
		/// </summary>
		protected ArrayList InnerList
		{
			get
			{
				if (this.m_list == null)
				{
					return new ArrayList();
				} //	end if
				return this.m_list;
			} //	end get
		} //  end InnerList

		/// <summary>
		/// Gets or sets the number of elements that the collection can contain.
		/// </summary>
		public int Capacity
		{
			get { return this.InnerList.Capacity; }
			set { this.InnerList.Capacity = value; }
		} //  end Capacity

		//-
		#endregion

		#region IList Members
		//-------------------

		int IList.Add(object value)
		{
			OnValidate(value);
			return this.InnerList.Add(value);
		}

		/// <summary>
		/// Removes all the elements from the collection.
		/// </summary>
		public void Clear()
		{
			this.InnerList.Clear();
		} //  end Clear

		bool IList.Contains(object value)
		{
			return this.InnerList.Contains(value);
		} //  end Contains

		int IList.IndexOf(object value)
		{
			return this.InnerList.IndexOf(value);
		} //  end IndexOf

		void IList.Insert(int index, object value)
		{
			if ((index < 0) || (index > this.InnerList.Count))
			{
				throw new ArgumentOutOfRangeException("index", index, "Index not valid!");
			}
			this.OnValidate(value);
			this.InnerList.Insert(index, value);
		} //  end Insert

		bool IList.IsFixedSize
		{
			get { return this.InnerList.IsFixedSize; }
		}

		bool IList.IsReadOnly
		{
			get { return this.InnerList.IsReadOnly; }
		}

		void IList.Remove(object value)
		{
			this.OnValidate(value);
			int index = this.InnerList.IndexOf(value);

			if (index < 0)
			{
				throw new ArgumentException("Element to be removed not found!", "value");
			}
			this.InnerList.RemoveAt(index);
		}

		/// <summary>
		/// Removes the element at the specified index of the collection.
		/// </summary>
		/// <param name="index">The zero-based index of the element to remove.</param>
		public void RemoveAt(int index)
		{
			if ((index < 0) || (index > this.InnerList.Count))
			{
				throw new ArgumentOutOfRangeException("index", index, "Index not valid!");
			}

			object obj = this.InnerList[index];
			OnValidate(obj);
			this.InnerList.RemoveAt(index);
		}

		object IList.this[int index]
		{
			get
			{
				if ((index < 0) || (index > this.InnerList.Count))
				{
					throw new ArgumentOutOfRangeException("index", index, "Index not valid!");
				}
				return this.InnerList[index];
			}
			set
			{
				if ((index < 0) || (index > this.InnerList.Count))
				{
					throw new ArgumentOutOfRangeException("index", index, "Index not valid!");
				}
				OnValidate(value);
				this.InnerList[index] = value;
			}
		}

		//--

		#endregion

		#region ICollection Members
		//-------------------------

		void ICollection.CopyTo(Array array, int index)
		{
			this.InnerList.CopyTo(array, index);
		}

		/// <summary>
		/// Gets the number of elements actually contained in the collection.
		/// </summary>
		public int Count
		{
			get
			{
				if (this.m_list != null)
				{
					return m_list.Count;
				}
				return 0;
			}
		}

		bool ICollection.IsSynchronized
		{
			get { return InnerList.IsSynchronized; }
		}

		/// <summary>
		/// Gets an object that can be used to synchronize access to the collection.
		/// </summary>
		public object SyncRoot
		{
			get { return this.InnerList.SyncRoot; }
		}

		//--

		#endregion

		#region IEnumerable Members
		//-------------------------

		/// <summary>
		/// Returns an enumerator for the entire collection.
		/// </summary>
		/// <returns></returns>
		public IEnumerator GetEnumerator()
		{
			return this.InnerList.GetEnumerator();
		}

		//-
		#endregion

		#region IComponent Implementation
		private ISite m_Site = null;

		/// <summary>
		/// Represents the method that handles the <b>Disposed</b> event of a component.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DataControl"]/event[@name="Disposed"]/doc/*'
		/// />
		public event EventHandler Disposed;

		/// <summary>
		/// Gets or sets the ISite associated with the IComponent.
		/// </summary>
		internal System.ComponentModel.ISite Site
		{
			get { return m_Site; }
			set { m_Site = value; }
		}

		/// <summary>
		/// Gets or sets the ISite associated with the IComponent.
		/// An object that implements ISite contains a reference to a
		/// component and the container it is in and is used by the Designer to
		/// keep track of object hierarchies.
		///</summary>
		System.ComponentModel.ISite System.ComponentModel.IComponent.Site
		{
			get { return Site; }
			set { Site = value; }
		}

		/// <summary>
		/// Allows the component to free up any resources currently being used when the object is no longer needed.
		/// </summary>
		internal void Dispose()
		{
			this.Clear();
			if (null != Disposed) Disposed(this, System.EventArgs.Empty);
		}

		/// <summary>
		/// Allows the component to free up any resources currently being used when the object is no longer needed.
		/// </summary>
		void System.IDisposable.Dispose()
		{
			Dispose();
		}

		#endregion

		#region IBindingList Implementation
		#region IBindingList Editing Properties
		/// <summary>
		/// Occurs when the list changes or an item in the list changes.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DataControl"]/event[@name="ListChanged"]/doc/*'
		/// />
		public event ListChangedEventHandler ListChanged;

		private bool m_AllowEditItems = true;
		private bool m_AllowNewItems = true;
		private bool m_AllowRemoveItems = true;

		/// <summary>
		/// Gets whether you can update items in the list.
		/// </summary>
		internal bool AllowEdit
		{
			get { return m_AllowEditItems; }
		}

		bool System.ComponentModel.IBindingList.AllowEdit
		{
			get { return AllowEdit; }
		}

		/// <summary>
		/// Gets whether you can add items to the list using AddNew.
		/// </summary>
		internal bool AllowNew
		{
			get { return m_AllowNewItems; }
		}

		bool System.ComponentModel.IBindingList.AllowNew
		{
			get { return AllowNew; }
		}

		/// <summary>
		/// Gets whether you can remove items from the list, using Remove or RemoveAt.
		/// </summary>
		internal bool AllowRemove
		{
			get { return m_AllowRemoveItems; }
		}

		bool System.ComponentModel.IBindingList.AllowRemove
		{
			get { return AllowRemove; }
		}

		#endregion

		#region IBindingList Features Properties
		private bool m_SupportsChangeNotification = true;
		private bool m_SupportsSearching = true;
		private bool m_SupportsSorting = true;

		/// <summary>
		/// Gets whether a ListChanged event is raised when the list changes or an item in the list changes.
		/// </summary>
		internal bool SupportsChangeNotification
		{
			get { return m_SupportsChangeNotification; }
		}

		bool System.ComponentModel.IBindingList.SupportsChangeNotification
		{
			get { return SupportsChangeNotification; }
		}

		/// <summary>
		/// Gets whether the list supports searching using the Find method.
		/// </summary>
		internal bool SupportsSearching
		{
			get { return m_SupportsSearching; }
		}

		bool System.ComponentModel.IBindingList.SupportsSearching
		{
			get { return SupportsSearching; }
		}

		/// <summary>
		/// Gets whether the list supports sorting.
		/// </summary>
		internal bool SupportsSorting
		{
			get { return m_SupportsSorting; }
		}

		bool System.ComponentModel.IBindingList.SupportsSorting
		{
			get { return SupportsSorting; }
		}

		#endregion

		#region IBindingList Editing Methods
		/// <summary>
		/// Adds a new item to the list.
		/// </summary>
		/// <returns>The item added to the list.</returns>
		internal object AddNew()
		{
			ControlDaItem item = new ControlDaItem(true);
			this.Add(item);
			item.CancelAddNew += new ControlDaItem.CancelAddNewEventHandler(this.CancelAddNew);
			return item;
		}

		object System.ComponentModel.IBindingList.AddNew()
		{
			return AddNew();
		}

		internal void CancelAddNew(ControlDaItem item, bool Remove)
		{
			item.CancelAddNew -= new ControlDaItem.CancelAddNewEventHandler(this.CancelAddNew);
			if ((Remove))
			{
				this.Remove(item);
			}
		}

		/// <summary>
		/// Performs additional custom processes after inserting a new element into the collection.
		/// </summary>
		/// <param name="index">The zero-based index at which to insert the value. </param>
		/// <param name="value">The new value of the element at index. </param>
		///protected override void OnInsertComplete( int index, object value ) {
		internal void OnInsertComplete(int index, object value)
		{
			//base.OnInsertComplete( index, value );
			if (null != ListChanged)
				ListChanged(this, new ListChangedEventArgs(ListChangedType.ItemAdded, index));
		}

		/// <summary>
		/// Performs additional custom processes after removing an element from the collection.
		/// </summary>
		/// <param name="index">The zero-based index at which value can be found.</param>
		/// <param name="value">The value of the element to remove from index.</param>
		///protected override void OnRemoveComplete( int index, object value ) {
		internal void OnRemoveComplete(int index, object value)
		{
			//base.OnRemoveComplete( index, value );
			if (null != ListChanged)
				ListChanged(this, new ListChangedEventArgs(ListChangedType.ItemDeleted, index));
		}

		#endregion

		#region IBindingList Sorting Features
		private PropertyDescriptor m_SortProperty;
		private ListSortDirection m_SortDirection;
		private ArrayList m_OriginalList;

		/// <summary>
		/// Gets the direction of the sort.
		/// </summary>
		internal System.ComponentModel.ListSortDirection SortDirection
		{
			get { return m_SortDirection; }
		}

		System.ComponentModel.ListSortDirection System.ComponentModel.IBindingList.SortDirection
		{
			get { return SortDirection; }
		}

		/// <summary>
		/// Gets the PropertyDescriptor that is being used for sorting.
		/// </summary>
		internal System.ComponentModel.PropertyDescriptor SortProperty
		{
			get { return m_SortProperty; }
		}

		System.ComponentModel.PropertyDescriptor System.ComponentModel.IBindingList.SortProperty
		{
			get { return SortProperty; }
		}

		/// <summary>
		/// Gets whether the items in the list are sorted.
		/// </summary>
		internal bool IsSorted
		{
			get { return m_SortProperty == null; }
		}

		bool System.ComponentModel.IBindingList.IsSorted
		{
			get { return IsSorted; }
		}

		private void SaveList()
		{
			m_OriginalList = new ArrayList(this);
		}

		private void ResetList(ArrayList NewList)
		{
			this.Clear();
			foreach (ControlDaItem m_item in NewList)
			{
				this.Add(m_item);
			}
		}

		private void DoSort()
		{
			this.InnerList.Sort(new ValueComparer(m_SortProperty, m_SortDirection));
		}

		/// <summary>
		/// Sorts the list based on a PropertyDescriptor and a ListSortDirection.
		/// </summary>
		/// <param name="property">The PropertyDescriptor to sort by. </param>
		/// <param name="direction">One of the ListSortDirection values. </param>
		internal void ApplySort(System.ComponentModel.PropertyDescriptor property,
		                        System.ComponentModel.ListSortDirection direction)
		{
			m_SortProperty = property;
			m_SortDirection = direction;
			if ((m_OriginalList == null))
			{
				SaveList();
			}
			DoSort();
			if (null != ListChanged) ListChanged(this, new ListChangedEventArgs(ListChangedType.Reset, 0));
		}

		void System.ComponentModel.IBindingList.ApplySort(System.ComponentModel.PropertyDescriptor property,
		                                                  System.ComponentModel.ListSortDirection direction)
		{
			ApplySort(property, direction);
		}

		/// <summary>
		/// Removes any sort applied using ApplySort.
		/// </summary>
		internal void RemoveSort()
		{
			ResetList(m_OriginalList);
			m_SortDirection = ((System.ComponentModel.ListSortDirection) (0));
			m_SortProperty = null;
			if (null != ListChanged) ListChanged(this, new ListChangedEventArgs(ListChangedType.Reset, 0));
		}

		void System.ComponentModel.IBindingList.RemoveSort()
		{
			RemoveSort();
		}

		#endregion

		#region IBindingList Other Features
		/// <summary>
		/// Returns the index of the row that has the given PropertyDescriptor.
		/// </summary>
		/// <param name="property">Returns the index of the row that has the given PropertyDescriptor.</param>
		/// <param name="key">The value of the property parameter to search for. </param>
		/// <returns>The index of the row that has the given PropertyDescriptor.</returns>
		internal int Find(System.ComponentModel.PropertyDescriptor property, object key)
		{
			int idx = 0;
			int result = -1;
			for (idx = 0; idx <= this.Count - 1; idx++)
			{
				object value = property.GetValue(this[idx]);
				switch (property.Name)
				{
					case "ValueToString":
						if ((value == key))
						{
							result = idx;
						}
						break;
					default:
						if ((value == key))
						{
							result = idx;
						}
						break;
				}

				if ((result != -1))
				{
					return result;
				}
			}
			return 0;
		}

		int System.ComponentModel.IBindingList.Find(System.ComponentModel.PropertyDescriptor property, object key)
		{
			return Find(property, key);
		}

		/// <summary>
		/// Adds the PropertyDescriptor to the indexes used for searching.
		/// </summary>
		/// <param name="property">Adds the PropertyDescriptor to the indexes used for searching.</param>
		internal void AddIndex(System.ComponentModel.PropertyDescriptor property)
		{
		}

		void System.ComponentModel.IBindingList.AddIndex(System.ComponentModel.PropertyDescriptor property)
		{
			AddIndex(property);
		}

		/// <summary>
		/// Removes the PropertyDescriptor from the indexes used for searching.
		/// </summary>
		/// <param name="property">The PropertyDescriptor to remove from the indexes used for searching. </param>
		internal void RemoveIndex(System.ComponentModel.PropertyDescriptor property)
		{
		}

		void System.ComponentModel.IBindingList.RemoveIndex(System.ComponentModel.PropertyDescriptor property)
		{
			RemoveIndex(property);
		}

		#endregion

		#endregion

		#region ITypedList Implementation
		/// <summary>
		/// Returns the PropertyDescriptorCollection that represents the properties on each item used to bind data.
		/// </summary>
		/// <param name="listAccessors">An array of PropertyDescriptor objects to find in the collection as bindable. This can be a null reference.</param>
		/// <returns>The PropertyDescriptorCollection that represents the properties on each item used to bind data.</returns>
		internal PropertyDescriptorCollection GetItemProperties(PropertyDescriptor[] listAccessors)
		{
			PropertyDescriptorCollection m_OriginalList = TypeDescriptor.GetProperties(typeof (DaItem));
			PropertyDescriptorCollection m_SortedList = m_OriginalList.Sort(new string[] {});
			return m_SortedList;
		}

		PropertyDescriptorCollection System.ComponentModel.ITypedList.GetItemProperties(PropertyDescriptor[] listAccessors)
		{
			return GetItemProperties(listAccessors);
		}

		/// <summary>
		/// Returns the name of the list.
		/// </summary>
		/// <param name="listAccessors">An array of PropertyDescriptor objects, the list name for which is returned. This can be a null reference .</param>
		/// <returns>The name of the list.</returns>
		internal string GetListName(PropertyDescriptor[] listAccessors)
		{
			return "Softing Data Control";
		}

		string System.ComponentModel.ITypedList.GetListName(PropertyDescriptor[] listAccessors)
		{
			return GetListName(listAccessors);
		}

		#endregion
	} // end class DataControl

	#region //Public Delegates
	//-------------------

	/// <summary>
	/// Represents a method that will handle the <see cref="Softing.OPCToolbox.Client.Control.DataControl.DataChanged"/> event of a <see cref="DataControl"/>.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="DataControl"]/delegate[@name="ControlDataChangedEventHandler"]/doc/*'
	/// />
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="ControlDataChangedEventHandler"]/doc/*'
	/// />
	public delegate void ControlDataChangedEventHandler(
		ControlDaSubscription aControlSubscription, ControlDaItem[] items, ValueQT[] values, int[] results);

	#endregion
}
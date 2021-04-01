using System;
using System.Collections;
using System.Runtime.InteropServices;
using Softing.OPCToolbox.OTB;

namespace Softing.OPCToolbox.Server
{

	#region // Public Delegates
	//-------------------------

	/// <summary>
	/// <para><b>Unmanaged C++</b>
	/// <line>Delegate is not available for Unmanaged C++.</line>
	/// </para>
	/// <para><b>Unmanaged C++</b>
	/// <line>This functionality is available overriding the AddressSpaceElement::addedToAddressSpace() virtual method.</line>
	/// </para>
	/// <para><b>C#, Visual Basic, C++</b> Delegate to be called when a new element was added to the address space.</para>
	/// </summary>
	/// <remarks>
	/// <para><b>C#, Visual Basic, C++</b> For the the default behavior no delegate is associated to the address space element event
	/// <see cref="AddressSpaceElement.AddedToAddressSpace"/> therefore it will not be triggered</para>
	/// </remarks>
	public delegate void AddedToAddressSpace();

	/// <summary>
	/// <para><b>Unmanaged C++</b>
	/// <line>Delegate is not available for Unmanaged C++.</line>
	/// </para>
	/// <para><b>Unmanaged C++</b>
	/// <line>This functionality is available overriding the AddressSpaceElement::removedFromAddressSpace() virtual method.</line>
	/// </para>
	/// <para><b>C#, Visual Basic, C++</b> Delegate to be called when a new element was removed from the address space.</para>
	/// </summary>
	/// <remarks>
	/// <para><b>C#, Visual Basic, C++</b> For the the default behavior no delegate is associated to the address space element event
	/// <see cref="AddressSpaceElement.RemovedFromAddressSpace"/> therefore it will not be triggered</para>
	/// </remarks>
	public delegate void RemovedFromAddressSpace();

	//-

	#endregion

	#region //	Public Interfaces
	//---------------------------

	/// <summary>
	/// Interface for AddressSpaceElement and AddresssapceRoot classes
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="IAddressSpaceElement"]/doc/*'
	///	/>
	public interface IAddressSpaceElement
	{
		#region	//	Interface Methods
		//---------------------------

		/// <summary>
		/// Adds an address space element provided as parameter to the children list of this instance of the address space element
		/// </summary>
		/// <param name="aChild">the address sapce element to be added</param>
		/// <returns>
		/// the operation success or failure status
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="IAddressSpaceElement"]/
		///		method[@name="AddChild"]/doc/*'
		///	/>
		bool AddChild(AddressSpaceElement aChild);

		/// <summary>
		/// Removes the specified child from the children list of this instance of address space element.
		/// <note>
		/// This will remove completely from the address space the entire branch represented by aChild.
		/// </note>
		/// </summary>
		/// <param name="aChild">the children to be removed</param>
		/// <returns>
		/// the opperation success or failure status
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="IAddressSpaceElement"]/
		///		method[@name="RemoveChild"]/doc/*'
		///	/>
		bool RemoveChild(AddressSpaceElement aChild);

		/// <summary>
		/// Retrieves the children collection of this instance of the address space element
		/// </summary>
		/// <returns>
		/// collection - with the references of the children of this address space element.
		/// empty collection  - if no children or invalid parameter provided
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="IAddressSpaceElement"]/
		///		method[@name="GetChildren"]/doc/*'
		///	/>
		ArrayList GetChildren();

		/// <summary>
		/// retrieves the parent object of instance of the address space element
		/// </summary>
		/// <returns>
		///	a reference to the parent
		///	null - if the instance is the root object
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="IAddressSpaceElement"]/
		///		method[@name="GetParent"]/doc/*'
		///	/>
		AddressSpaceElement GetParent();

		#endregion
	} //	end IAddressSpaceElement

	//-

	#endregion

	#region //	Public Classes
	//------------------------

	/// <summary>
	/// An Address Space Element represent the base component of the OPC DA and AE namespaces.
	/// Specific AE and DA object types are derived from this abstract class
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="AddressSpaceElement"]/doc/*'
	///	/>
	public abstract class AddressSpaceElement : IAddressSpaceElement
	{
		#region	//	Protected Constructors
		//--------------------------------

		/// <summary>
		///	Proteced constructor used for string based address element creation.
		///	This constructor is only allowed to be called from within the server's <see cref="Creator"/> instance.
		/// </summary>
		/// <param name="aName"> the name of the element</param>
		/// <param name="anUserData"> application's ID of the element. If 0, this is automatically generated</param>
		/// <param name="anObjectHandle">Toolkit internal identifier of the element</param>
		/// <param name="aParentHandle">Toolkit internal identifier of the element's parent</param>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="AddressSpaceElement"]/
		///		ctor[@name="AddressSpaceElement.all"]/doc/*'
		///	/>
		protected AddressSpaceElement(
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle)
		{
			this.m_name = aName;

			//	check if an userData was provided
			if (anUserData > 0)
			{
				this.m_userData = anUserData;
			}
			else
			{
				lock (m_currentHashJanitor)
				{
					m_userData = m_currentHash++;
				}
			} //	end if ... ekse

			this.m_objectHandle = anObjectHandle;
			this.m_parentHandle = aParentHandle;
			this.m_isBrowsable = true;
		} //	end constructor


		/// <summary>
		/// Default constructor
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="AddressSpaceElement"]/
		///		ctor[@name="AddressSpaceElement.def"]/doc/*'
		///	/>
		protected AddressSpaceElement()
		{
			lock (m_currentHashJanitor)
			{
				m_userData = m_currentHash++;
			}
			this.m_isBrowsable = true;
		} //	end constructor


		//-
		#endregion

		#region //	Public Destructors
		//----------------------------

		/// <summary>
		/// public virtual destructor
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="AddressSpaceElement"]/
		///		dtor[@name="AddressSpaceElement"]/doc/*'
		///	/>
		~AddressSpaceElement()
		{
		} //	end dtor


		//-
		#endregion

		#region //	Public Properties
		//---------------------------

		/// <summary>
		/// The name of this namespace element
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceElement"]/
		///			property[@name="Name"]/doc/*'
		///	/>
		public string Name
		{
			get { return m_name; }
			set { m_name = value; }
		} //	end Name


		/// <summary>
		/// Determines whether this address space element may have children i.e. it is a node
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceElement"]/
		///			property[@name="HasChildren"]/doc/*'
		///	/>
		public bool HasChildren
		{
			get { return m_hasChildren; }
			set { m_hasChildren = value; }
		} //	end HasChildren


		/// <summary>
		///  Determines whether a an AddressSpaceElement is shown in the hierarchie during browsing.
		///  It is a valid I/O element though.
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceElement"]/
		///			property[@name="IsBrowsable"]/doc/*'
		///	/>
		public bool IsBrowsable
		{
			get { return m_isBrowsable; }
			set { m_isBrowsable = value; }
		} //	end IsBrowsable


		/// <summary>
		/// Numeric (unique )identifier for the address space element.
		/// The user may override this method but he must assure the unique IDs for the address space then.
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceElement"]/
		///			property[@name="UserData"]/doc/*'
		///	/>
		public virtual uint UserData
		{
			get { return this.m_userData; }
			set { this.m_userData = value; }
		} //	end UserData


		/// <summary>
		/// OTB internal ID for the instance
		/// </summary>
		internal uint ObjectHandle
		{
			get { return this.m_objectHandle; } //	end get
			set { this.m_objectHandle = value; } //	end set
		} //	end ObjectHandle


		/// <summary>
		/// OTB internal ID for the parent
		/// </summary>
		internal uint ParentHandle
		{
			get { return this.m_parentHandle; } //	end get
			set { this.m_parentHandle = value; } //	end set
		} //	end ParentHandle


		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		///	Implements the AddChild Interface. Must be overriden in the children classes
		/// </summary>
		/// <param name="aChild"> an address space element to be added as child</param>
		/// <returns>
		/// success or failure status
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceElement"]/
		///			method[@name="AddChild"]/doc/*'
		///	/>
		public virtual bool AddChild(AddressSpaceElement aChild)
		{
			return aChild.TriggerAddedToAddressSpace();
		} //	end AddChild


		/// <summary>
		///	Implements the RemoveChild Interface. Must be overriden in the children classes
		/// </summary>
		/// <param name="aChild"> an address space element to be added as child</param>
		/// <returns>
		/// success or failure status
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceElement"]/
		///			method[@name="RemoveChild"]/doc/*'
		///	/>
		public virtual bool RemoveChild(AddressSpaceElement aChild)
		{
			return aChild.TriggerRemovedFromAddressSpace();
		} //	end RemoveChild


		/// <summary>
		///	GetChildren interface. Derived classes must implement this method.
		/// </summary>
		/// <returns>
		/// a collection of children
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceElement"]/
		///			method[@name="GetChildren"]/doc/*'
		///	/>
		public abstract ArrayList GetChildren();

		/// <summary>
		///	GetParent intrface. Derived classes must implement this method.
		/// </summary>
		/// <returns>
		/// the parent element.
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceElement"]/
		///			method[@name="GetParent"]/doc/*'
		///	/>
		public abstract AddressSpaceElement GetParent();

		/// <summary>
		/// triggers the event
		/// </summary>
		/// <returns></returns>
		internal virtual bool TriggerRemovedFromAddressSpace()
		{
			if (RemovedFromAddressSpace != null)
			{
				RemovedFromAddressSpace();
			} //	end RemoveChild
			return true;
		} //	end TriggerRemovedFromAddressSpace


		/// <summary>
		/// triggers the event
		/// </summary>
		/// <returns></returns>
		internal virtual bool TriggerAddedToAddressSpace()
		{
			if (AddedToAddressSpace != null)
			{
				AddedToAddressSpace();
			} //	end if
			return true;
		} //	end TriggerRemovedFromAddressSpace


		//-
		#endregion

		#region //	Public Events
		//-----------------------

		/// <summary>
		///
		/// </summary>
		public event AddedToAddressSpace AddedToAddressSpace = null;

		/// <summary>
		///
		/// </summary>
		public event RemovedFromAddressSpace RemovedFromAddressSpace = null;

		//-
		#endregion

		#region //	Private Attrtibutes
		//-----------------------------

		/// <summary>
		///
		/// </summary>
		protected string m_name = string.Empty;

		/// <summary>
		///
		/// </summary>
		protected bool m_hasChildren = false;

		/// <summary>
		///
		/// </summary>
		protected bool m_isBrowsable = true;

		/// <summary>
		///
		/// </summary>
		private uint m_userData = 0;

		/// <summary>
		/// OTB handler
		/// </summary>
		protected internal uint m_objectHandle = 0;

		/// <summary>
		/// OTB parent handler
		/// </summary>
		protected internal uint m_parentHandle = 0;

		//-
		#endregion

		#region	//	Private Class Attributes
		//----------------------------------

		private static uint m_currentHash = 0;

		private static object m_currentHashJanitor = new object();

		//-
		#endregion
	} //	end class AddressSpaceElement

	/// <summary>
	/// Represents the address space entry point
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="AddressSpaceElement"]/doc/*'
	///	/>
	public abstract class AddressSpaceRoot : IAddressSpaceElement
	{
		#region	//	Protected Constructors
		//--------------------------------

		/// <summary>
		/// Used to build an address space toor both object and string based
		/// </summary>
		/// <param name="aRoot">An address space element to impersonate the root. This element is not visible in the namespace</param>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="AddressSpaceRoot"]/
		///		ctor[@name="AddressSpaceRoot.default"]/doc/*'
		///	/>
		protected AddressSpaceRoot(AddressSpaceElement aRoot) :
			this(EnumAddressSpaceType.OBJECT_STRING_BASED, aRoot)
		{
		} //	end ctr


		/// <summary>
		/// AddressSpaceRoot constructor:  Used to build a specialised root type
		/// </summary>
		/// <param name="anAddressSpaceType">
		/// The type of address space
		/// </param>
		/// <param name="aRoot">
		/// An address space element to impersonate the root. This element is not visible in the namespace
		/// </param>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="AddressSpaceRoot"]/
		///		ctor[@name="AddressSpaceRoot.typed"]/doc/*'
		///	/>
		protected AddressSpaceRoot(
			EnumAddressSpaceType anAddressSpaceType,
			AddressSpaceElement aRoot)
		{
			this.m_root = aRoot;
			m_root.HasChildren = true;
			m_namespaceType = anAddressSpaceType;
			int result = OTBFunctions.OTSInitAddressSpace((byte) this.m_namespaceType);
		} //	end ctr


		//-
		#endregion

		#region //	Public Members
		//------------------------

		/// <summary>
		///  Adds a namespace element as child to the root element
		/// </summary>
		/// <param name="aChild">Child to be added</param>
		/// <returns>
		/// true	- Child added
		/// false	- Child was not added
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceRoot"]/
		///			method[@name="AddChild"]/doc/*'
		///	/>
		public virtual bool AddChild(AddressSpaceElement aChild)
		{
			return m_root.AddChild(aChild);
		} //	end AddChild


		/// <summary>
		/// Removes Child and its children if there are some
		/// </summary>
		/// <param name="aChild">AddressSpaceElement to be removed</param>
		/// <returns>
		/// true	- Child removed
		/// false	- Child was not removed</returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceRoot"]/
		///			method[@name="RemoveChild"]/doc/*'
		///	/>
		public virtual bool RemoveChild(AddressSpaceElement aChild)
		{
			return m_root.RemoveChild(aChild);
		} //	end RemoveChild


		/// <summary>
		/// Returns the collection of root children
		/// </summary>
		/// <returns>
		/// collection with all first level elements of the address space
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceRoot"]/
		///			method[@name="GetChildren"]/doc/*'
		///	/>
		public virtual ArrayList GetChildren()
		{
			return m_root.GetChildren();
		} //	end GetChildren


		/// <summary>
		/// returns the parent of the root - this is not usefull,
		/// but kept for compatibility with the element interface
		/// </summary>
		/// <returns>
		/// returns a null reference
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceRoot"]/
		///			method[@name="GetParent"]/doc/*'
		///	/>
		public virtual AddressSpaceElement GetParent()
		{
			return m_root.GetParent();
		} //	end GetParent


		/// <summary>
		/// Returns the corresponding address space element to the element ID provided.
		/// This interface needs to be implemented in the string based object space case.
		/// </summary>
		/// <param name="anElementID">Address space path of the element</param>
		/// <param name="anElement">Out parameter reference to the element matched in case of success. Null reference if not supported or invalid path provided.</param>
		/// <returns>
		/// Success or failure result code
		/// Default behavior returns EnumResultCode.E_NOTIMPL
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceRoot"]/
		///			method[@name="QueryAddressSpaceElementData"]/doc/*'
		///	/>
		public abstract int QueryAddressSpaceElementData(string anElementID, out AddressSpaceElement anElement);

		/// <summary>
		/// Returns the corresponding address space element to the element ID provided.
		/// <br/>This interface needs to be implemented in the string based object space case.
		/// <br/>This is the extended version of QueryAddressSpaceElementData method.
		/// </summary>
		/// <param name="anElementID">Address space path of the element</param>
		/// <param name="elementDataFilter">Type of the element data to be filled in</param>
		/// <param name="anElement">Out parameter reference to the element matched in case of success. Null reference if not supported or invalid path provided.</param>
		/// <returns>
		/// Success or failure result code.
		/// <br/>Default behavior returns EnumResultCode.E_NOTIMPL
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceRoot"]/
		///			method[@name="QueryAddressSpaceElementDataEx"]/doc/*'
		///	/>
		public abstract int QueryAddressSpaceElementDataEx(string anElementID, ref EnumAddressSpaceElementData elementDataFilter, out AddressSpaceElement anElement);

		/// <summary>
		/// Returns the collection of children of the address space element to the element ID provided.
		/// This interface needs to be implemented in the string based object space case
		/// </summary>
		/// <param name="anElementID">Address space path of the element.</param>
		/// <param name="aChildrenList">out parameter - collection of children of the element ID.</param>
		/// <returns>
		/// Success or failure result code
		/// Default behavior returns EnumResultCode.E_NOTIMPL
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AddressSpaceRoot"]/
		///			method[@name="QueryAddressSpaceElementChildren"]/doc/*'
		///	/>
		public abstract int QueryAddressSpaceElementChildren(string anElementID, ArrayList aChildrenList);

		/// <summary>
		/// retrieves the parent from a handle
		/// </summary>
		internal AddressSpaceElement GetParent(uint aHandle)
		{
			if (aHandle == 0)
			{
				return m_root;
			} //	end if

			OTObjectData parent = new OTObjectData();
			IntPtr pParent = Marshal.AllocCoTaskMem(Marshal.SizeOf(parent));
			Marshal.StructureToPtr(parent, pParent, false);

			if ((uint) EnumResultCode.S_OK != OTBFunctions.OTSGetParent(aHandle, pParent))
			{
				return null;
			} //	end if

			parent = (OTObjectData) Marshal.PtrToStructure(pParent, typeof (OTObjectData));
			Marshal.FreeCoTaskMem(pParent);

			AddressSpaceElement elementParent = GetElementFromArray(parent.m_userData);
			if (elementParent == null)
			{
				return m_root;
			} //	end if

			return elementParent;
		} //	end getParent


		/// <summary>
		/// adds an element to the internal address space hashtable
		/// </summary>
		/// <param name="anElement"></param>
		/// <returns></returns>
		internal virtual uint AddElementToArray(AddressSpaceElement anElement)
		{
			lock (m_elements.SyncRoot) // modifying content
			{
				m_elements.Add(anElement.UserData, anElement);
			}
			return anElement.UserData;
		} //	end AddElementToArray


		/// <summary>
		/// retreives an address space element from the internal address space hashtable
		/// </summary>
		/// <param name="anElementUserData"></param>
		/// <returns></returns>
		internal virtual AddressSpaceElement GetElementFromArray(uint anElementUserData)
		{
			Hashtable synchronizedElements = Hashtable.Synchronized(m_elements);
			return synchronizedElements[anElementUserData] as AddressSpaceElement;
		} //	end GetElementFromArray


		/// <summary>
		/// removes an element from the internal hashtable
		/// </summary>
		/// <param name="anElement"></param>
		/// <returns></returns>
		internal virtual int RemoveElementFromArray(AddressSpaceElement anElement)
		{
			return RemoveElementFromArray(anElement.UserData);
		} //	end RemoveElementFromArray


		/// <summary>
		/// removes an element from the internal hashtable
		/// </summary>
		/// <param name="anElementUserData"></param>
		/// <returns></returns>
		internal virtual int RemoveElementFromArray(uint anElementUserData)
		{
			try
			{
				lock (m_elements.SyncRoot) // modifying content
				{
					m_elements.Remove(anElementUserData);
				}
				return (int) EnumResultCode.S_OK;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.INF,
				                           EnumTraceGroup.OPCSERVER,
				                           "AddressSpaceElement.RemoveElementFromArray",
				                           "Exception caught:" + e.ToString());

				return (int) EnumResultCode.E_FAIL;
			} //	end try catch
		} //	end RemoveElementFromArray


		//-
		#endregion

		#region //	Protected Attributes
		//----------------------------

		/// <summary>
		/// the type of the address space
		/// </summary>
		protected readonly EnumAddressSpaceType m_namespaceType = EnumAddressSpaceType.OBJECT_STRING_BASED;

		/// <summary>
		/// Storage place for all for the address space elements assigned to this root
		/// the key for this table is the UserData field of the Address space element
		/// </summary>
		protected readonly Hashtable m_elements = new Hashtable();

		/// <summary>
		/// An address space element representing the root element
		/// </summary>
		protected readonly AddressSpaceElement m_root = null;

		//-
		#endregion

		#region //Internal Properties
		//----------------------------

		public object ElementSyncRoot
		{
			get { return m_elements.SyncRoot; }
		}

		//-
		#endregion
	} //	end class AddressSpaceRoot

	//-

	#endregion
} //	end namespace Softing.OPCToolbox.Server
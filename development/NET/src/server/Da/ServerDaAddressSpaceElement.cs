using System;
using System.Collections;
using System.Runtime.InteropServices;
using Softing.OPCToolbox.OTB;

namespace Softing.OPCToolbox.Server
{
	/// <summary>
	/// <para><b>Unmanaged C++</b>
	/// <line>Delegate is not available for Unmanaged C++.</line>
	/// </para>
	/// <para><b>Unmanaged C++</b>
	/// <line>This is handled by overriding the DaAddressSpaceRoot::activationStateUpdated() method</line>
	/// </para>
	/// <para><b>C#, Visual Basic, C++</b>
	/// Delegate will be triggered by the DaRoot when when the active state of an <see cref="DaAddressSpaceElement"/> has been changed.
	/// </para>
	/// </summary>
	public delegate void ActivationStateUpdated();

	/// <summary>
	/// Base class element definition for an construction of the OPC-DA address space tree.
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="DaAddressSpaceElement"]/doc/*'
	///	/>
	public class DaAddressSpaceElement : AddressSpaceElement
	{
		#region //	Public Constructors
		//-----------------------------

		/// <summary>
		/// Default Da Address space element constructor
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		ctor[@name="DaAddressSpaceElement.default"]/doc/*'
		///	/>
		public DaAddressSpaceElement()
		{
		} //	end ctr


		/// <summary>
		/// Constructor to be used internally within the the <see cref="Creator"/> instance.
		/// </summary>
		/// <param name="anItemId">Item Id provided by internals of the toolkit.</param>
		/// <param name="aName"> Element name provided by internals of the toolkit.</param>
		/// <param name="anUserData">associated user data provided by internals of the toolkit</param>
		/// <param name="anObjectHandle">internal toolkit handle</param>
		/// <param name="aParentHandle">internal toolkit handle</param>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		ctor[@name="DaAddressSpaceElement.internal"]/doc/*'
		///	/>
		public DaAddressSpaceElement(
			string anItemId,
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle) :
				base(aName, anUserData, anObjectHandle, aParentHandle)
		{
			m_itemId = anItemId;
		} //	end ctr


		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		///  Adds a namespace element as child to a namespace element
		/// </summary>
		/// <param name="aChild">Child to be added. Checks if the child provided is a <see cref="DaAddressSpaceElement"/></param>
		/// <returns>
		/// <line>true	- Child successfully. </line>
		/// <line>false	- Child was not added. If fails, the Child should be released by the user.</line>
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		method[@name="AddChild"]/doc/*'
		///	/>
		public override bool AddChild(AddressSpaceElement aChild)
		{
			if (!HasChildren)
			{
				Application.Instance.Trace(
					EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER,
					"DaAddressSpaceElement.AddChild", "This element does not allow childred");
				return false;
			} //	end if

			DaAddressSpaceElement child = aChild as DaAddressSpaceElement;

			if (child == null)
			{
				Application.Instance.Trace(
					EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER,
					"DaAddressSpaceElement.AddChild", "Invalid child provided");
				return false;
			} //	end if

			//	only add the child if no handle assigned
			if (child.ObjectHandle == 0)
			{
				OTSAddressSpaceElementData data = new OTSAddressSpaceElementData();

				data.m_name = Marshal.StringToCoTaskMemUni(child.Name);
				data.m_itemID = Marshal.StringToCoTaskMemUni(
					Name + Application.Instance.AddressSpaceDelimiter + child.Name);

				data.m_accessRights = (byte) child.AccessRights;
				data.m_ioMode = (byte) child.IoMode;
				data.m_datatype = ValueQT.GetVartype(child.Datatype);
				data.m_hasChildren = Convert.ToByte(child.HasChildren);
				data.m_isBrowsable = Convert.ToByte(child.IsBrowsable);
				data.m_elementType = (byte) EnumAddressSpaceElementType.DA;
				data.m_userData = child.UserData;

				Application.Instance.DaAddressSpaceRoot.AddElementToArray(child);

				int result = OTBFunctions.OTSAddAddressSpaceElement(m_objectHandle, data, out child.m_objectHandle);
				Marshal.FreeCoTaskMem(data.m_name);
				Marshal.FreeCoTaskMem(data.m_itemID);

				if (!ResultCode.SUCCEEDED(result))
				{
					result = Application.Instance.DaAddressSpaceRoot.RemoveElementFromArray(child);
					System.Diagnostics.Debug.Assert(ResultCode.SUCCEEDED(result),
					                                "RemoveElementFromArray has failed");
					return false;
				} //	end if ... else
			}
			else
			{
				Application.Instance.DaAddressSpaceRoot.AddElementToArray(child);
			} //	end if ... else

			//	end if

			return base.AddChild(aChild);
		} //	end AddChild


		/// <summary>
		/// Removes the child if there is any. The entire branch of the aChild will be removed as well from the address space.
		/// </summary>
		/// <param name="aChild">AddressSpaceElement child to be removed from this instance</param>
		/// <returns>
		/// true	- aChild removed
		/// false	- aChild was not removed
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		method[@name="RemoveChild"]/doc/*'
		///	/>
		public override bool RemoveChild(AddressSpaceElement aChild)
		{
			if (!HasChildren)
			{
				//	just log a warning, but proceed then with the removal attempt
				Application.Instance.Trace(
					EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER,
					"DaAddressSpaceElement.RemoveChild", "This element does not allow childred");
			} //	end if

			DaAddressSpaceElement child = aChild as DaAddressSpaceElement;

			if (child == null)
			{
				Application.Instance.Trace(EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER,
				                           "DaAddressSpaceElement.RemoveChild", "Invalid child provided");
				return false;
			} //	end if

			if (child.HasChildren)
			{
				//	Remove the clildren of the child
				ArrayList otherChildren = child.GetChildren();

				foreach (AddressSpaceElement element in otherChildren)
				{
					child.RemoveChild(element);
				} //	end for
			} //	end if

			int result = OTBFunctions.OTSRemoveAddressSpaceElement(child.ObjectHandle);
			if (!ResultCode.SUCCEEDED(result))
			{
				Application.Instance.Trace(EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER,
				                           "DaAddressSpaceElement.RemoveChild",
				                           "OTSRemoveAddressSpaceElement has failed with code" + result.ToString());
				return false;
			} //	end if

			result = Application.Instance.DaAddressSpaceRoot.RemoveElementFromArray(child);
			if (!ResultCode.SUCCEEDED(result))
			{
				Application.Instance.Trace(EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER,
				                           "DaAddressSpaceElement.RemoveChild",
				                           "RemoveElementFromArray has failed with code" + result.ToString());
				return false;
			} //	end if

			return base.RemoveChild(aChild);
		} //	end RemoveChild


		/// <summary>
		/// Gets the current value in the element's cache
		/// </summary>
		/// <param name="aValue">out parameter to be filled in with the <see cref="ValueQT"/> instance of the cache value</param>
		/// <returns>
		/// returns a result code that should be checked for success. If S_OK, the value provided is valid.
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		method[@name="GetCacheValue"]/doc/*'
		///	/>
		public virtual int GetCacheValue(ref ValueQT aValue)
		{
			aValue = null;
			int result = (int) EnumResultCode.E_FAIL;

			if (m_objectHandle != 0 && IoMode != EnumIoMode.NONE)
			{
				OTValueData valueData = new OTValueData();

				//	Allocate space and reset the reserved space
				valueData.m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
				Marshal.GetNativeVariantForObject(null, valueData.m_value);

				result = OTBFunctions.OTSGetCacheValue(this.m_objectHandle, ref valueData);

				if (ResultCode.SUCCEEDED(result))
				{
					aValue = new ValueQT(ref valueData);
				} //	end if

				OTBFunctions.OTVariantClear(valueData.m_value);
				Marshal.FreeCoTaskMem(valueData.m_value);
			} //	end if

			return result;
		} //	end GetCacheValue


		/// <summary>
		/// Querries the element's current cache value, providing the session that queried this request.
		/// </summary>
		/// <param name="aSession">The session that generated the cache value</param>
		/// <param name="aCacheValue">out parameter to be filled in with the <see cref="ValueQT"/> instance of the cache value</param>
		/// <returns>
		/// returns a result code that should be checked for success. If S_OK, the value provided is valid.
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		method[@name="QueryCacheValue"]/doc/*'
		///	/>
		public virtual int QueryCacheValue(
			DaSession aSession,
			ref ValueQT aCacheValue)
		{
			return GetCacheValue(ref aCacheValue);
		} //	end QueryCacheValue


		/// <summary>
		/// Returns the address space element DA properties
		/// </summary>
		/// <param name="aPropertyList">A list of Properties</param>
		/// <returns>
		/// default value returns E_NOTIMPL - Method must be overridden if needed
		///	</returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		method[@name="QueryProperties"]/doc/*'
		///	/>
		public virtual int QueryProperties(out ArrayList aPropertyList)
		{
			aPropertyList = null;
			return (int) EnumResultCode.E_NOTIMPL;
		} //	end QueryProperties


		/// <summary>
		/// Changes value cache value of this element instance
		/// </summary>
		/// <param name="aNewValue"></param>
		/// <returns>
		/// E_INVALIDARG - Invalid valueQT was passed
		/// S_OK - Value changed
		/// OTS_E_EXCEPTION - Unexpected error occurred
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		method[@name="ValueChanged"]/doc/*'
		///	/>
		public virtual int ValueChanged(ValueQT aNewValue)
		{
			if (this.ObjectHandle == 0)
			{
				return (int) EnumResultCode.E_FAIL;
			} //	end if

			OTValueData valueData = new OTValueData();
			valueData.m_quality = (ushort) aNewValue.Quality;
			valueData.m_timestamp = new OTDateTime(aNewValue.TimeStamp);

			valueData.m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
			Marshal.GetNativeVariantForObject(aNewValue.Data, valueData.m_value);

			int result = OTBFunctions.OTSValuesChanged(1, new uint[1] {ObjectHandle}, new OTValueData[1] {valueData});

			OTBFunctions.OTVariantClear(valueData.m_value);
			Marshal.FreeCoTaskMem(valueData.m_value);
			return result;
		} //	end ValueChanged


		/// <summary>
		/// Returns the element`s parent
		/// </summary>
		/// <returns>
		/// the parent of this element
		/// null - the element is the root </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		method[@name="GetParent"]/doc/*'
		///	/>
		public override AddressSpaceElement GetParent()
		{
			return Application.Instance.DaAddressSpaceRoot.GetParent(ObjectHandle);
		} //	end GetParent


		/// <summary>
		/// Returns a list of children. This list is empty in case of an error or if no children are there to be delivered
		/// </summary>
		/// <returns>
		/// List of this element's children. If the element has no children, the list is empty
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		method[@name="GetChildren"]/doc/*'
		///	/>
		public override ArrayList GetChildren()
		{
			ArrayList children = new ArrayList();
			IntPtr ptrData = IntPtr.Zero;
			int count = 0;

			int res = OTBFunctions.OTSGetChildren(m_objectHandle, (byte) EnumAddressSpaceElementType.DA, out count, out ptrData);

			if (ResultCode.SUCCEEDED(res))
			{
				if (count > 0)
				{
					int size = Marshal.SizeOf(typeof (OTObjectData));

					lock (Application.Instance.DaAddressSpaceRoot.ElementSyncRoot)
					{
						for (int i = 0; i < count; i++)
						{
							IntPtr currentPtr = new IntPtr(ptrData.ToInt64() + size*i);
							OTObjectData myData = (OTObjectData) Marshal.PtrToStructure(currentPtr, typeof (OTObjectData));
							children.Add(
								Application.Instance.DaAddressSpaceRoot.GetElementFromArray(myData.m_userData) as DaAddressSpaceElement);
						} //	end for
					}
				} //	end if

				OTBFunctions.OTFreeMemory(ptrData);
			} //	end if

			return children;
		} //	end GetChildren


		/// <summary>
		/// Changes the active state of this element. This method is trigger internally.
		/// User should never call this method, but override it for own custom purpose.
		/// </summary>
		/// <param name="isActive"> element is active </param>
		/// <param name="anUpdateRate"> the update rate that the element should be polled with </param>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		method[@name="Change"]/doc/*'
		///	/>
		public virtual void Change(bool isActive, int anUpdateRate)
		{
			m_active = isActive;
			m_updateRate = anUpdateRate;
		} //	end Change


		/// <summary>
		/// Used to add the Engineering Units related properties as Analog EU type
		/// </summary>
		/// <param name="lowEULimit">Low value for the property 102</param>
		/// <param name="highEULimit">High value for the property 103</param>
		/// <returns>result code</returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		method[@name="SetEUInfoAnalog"]/doc/*'
		///	/>
		public int SetEUInfoAnalog(double lowEULimit, double highEULimit)
		{
			return OTBFunctions.OTSSetEUInfo(m_objectHandle, false, 0, null, lowEULimit, highEULimit);
		} //	end SetEUInfoAnalog


		/// <summary>
		///
		/// </summary>
		/// <param name="enumeratedValues"></param>
		/// <returns></returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		method[@name="SetEUInfoEnumerated"]/doc/*'
		///	/>
		public int SetEUInfoEnumerated(string[] enumeratedValues)
		{
			return OTBFunctions.OTSSetEUInfo(m_objectHandle, true, enumeratedValues.Length, enumeratedValues, 0, 0);
		} //	end SetEUInfoEnumerated


		//-
		#endregion

		#region	//	Public Properties
		//---------------------------

		/// <summary>
		/// Determines whether clients can read and/or write
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		property[@name="AccessRights"]/doc/*'
		///	/>
		public EnumAccessRights AccessRights
		{
			get { return m_accessRights; }
			set { m_accessRights = value; }
		}

		/// <summary>
		/// Determines the IO mode of this namespace element
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		property[@name="IoMode"]/doc/*'
		///	/>
		public EnumIoMode IoMode
		{
			get { return m_ioMode; }
			set { m_ioMode = value; }
		}

		/// <summary>
		/// Determines the datatype of this namespace element
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		property[@name="Datatype"]/doc/*'
		///	/>
		public Type Datatype
		{
			get { return m_datatype; }
			set { m_datatype = value; }
		}

		/// <summary>
		/// Determines the update rate for the namespace element
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		property[@name="UpdateRate"]/doc/*'
		///	/>
		public virtual int UpdateRate
		{
			get { return m_updateRate; }
		} //	end UpdateRate


		/// <summary>
		/// Determines wheter the namespace element is active(true) or not(false)
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		property[@name="Active"]/doc/*'
		///	/>
		public virtual bool Active
		{
			get { return m_active; }
		} //	end Active


		/// <summary>
		/// Returns the item ID (address space path) of this address space element
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceElement"]/
		///		property[@name="ItemId"]/doc/*'
		///	/>
		public string ItemId
		{
			get { return m_itemId; }
			set { m_itemId = value; }
		} //	end ItemId


		//-
		#endregion

		#region	//	Private Attributes
		//----------------------------

		private string m_itemId = string.Empty;
		private EnumAccessRights m_accessRights = EnumAccessRights.READABLE;
		private EnumIoMode m_ioMode = EnumIoMode.NONE;
		private Type m_datatype = null;
		private bool m_active = false;
		private int m_updateRate = -1;

		//-
		#endregion
	} //	end class DaAddressSpaceElement

	/// <summary>
	/// AddressSpaceRoot: Represents the OPC DA address space root
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="DaAddressSpaceRoot"]/doc/*'
	///	/>
	public class DaAddressSpaceRoot : AddressSpaceRoot
	{
		#region //	Public Constructors
		//-----------------------------

		/// <summary>
		/// DaAddressSpaceRoot constructor:  Creates the namespace's root node.
		/// </summary>
		/// <param name="anAddressSpaceType">object- and/or string-based namespace
		/// </param>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceRoot"]/
		///		ctor[@name="DaAddressSpaceRoot.typed"]/doc/*'
		///	/>
		public DaAddressSpaceRoot(EnumAddressSpaceType anAddressSpaceType) : base(
			anAddressSpaceType,
			new DaAddressSpaceElement("DaRoot", "DaRoot", 0, 0, 0))
		{
		} //	end constructor


		/// <summary>
		/// Constructs a OPC-DA address space entry point which is based on objects and strings EnumNamespaceType.OBJECT_STRING
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceRoot"]/
		///		ctor[@name="DaAddressSpaceRoot.default"]/doc/*'
		///	/>
		public DaAddressSpaceRoot() : this(EnumAddressSpaceType.OBJECT_STRING_BASED)
		{
		} //	end constructor


		//-
		#endregion

		#region	//	Public Methods
		//------------------------

		/// <summary>
		/// Returns namespace element`s properties
		/// </summary>
		/// <param name="anElementID">Fully qualified item ID</param>
		/// <param name="aPropertyList">Array of Properties</param>
		/// <returns>
		/// EnumResultCode.E_NOTIMPL - Must be overridden
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceRoot"]/
		///		method[@name="QueryAddressSpaceElementProperties"]/doc/*'
		///	/>
		public virtual int QueryAddressSpaceElementProperties(string anElementID, out ArrayList aPropertyList)
		{
			aPropertyList = null;
			return (int) EnumResultCode.E_NOTIMPL;
		} //	end QueryAddressSpaceElementProperties


		/// <summary>
		/// Returns the string based address space element
		/// </summary>
		/// <param name="anElementId">Address space path of the element</param>
		/// <param name="anElement">returned element</param>
		/// <returns>
		/// EnumResultCode.E_NOTIMPL
		/// Must be overridden to support string based address space
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceRoot"]/
		///		method[@name="QueryAddressSpaceElementData"]/doc/*'
		///	/>
		public override int QueryAddressSpaceElementData(string anElementId, out AddressSpaceElement anElement)
		{
			anElement = null;
			return (int) EnumResultCode.E_NOTIMPL;
		} //	end QueryAddressSpaceElementData

		/// <summary>
		/// Returns the string based address space element (extended version)
		/// </summary>
		/// <param name="anElementId">Address space path of the element</param>
		/// <param name="elementDataFilter">Type of the element data to be filled in</param>
		/// <param name="anElement">returned element</param>
		/// <returns>
		/// EnumResultCode.E_NOTIMPL
		/// Must be overridden to support string based address space
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceRoot"]/
		///		method[@name="QueryAddressSpaceElementDataEx"]/doc/*'
		///	/>
		public override int QueryAddressSpaceElementDataEx(string anElementId, ref EnumAddressSpaceElementData elementDataFilter, out AddressSpaceElement anElement)
		{
			anElement = null;
			return (int) EnumResultCode.E_NOTIMPL;
		} //	end QueryAddressSpaceElementDataEx


		/// <summary>
		/// Returns the list of children of the string based address space element.
		/// </summary>
		/// <param name="anElementId">Address space path of the element</param>
		/// <param name="aChildrenDetailsList">List of Children</param>
		/// <returns>
		/// EnumResultCode.E_NOTIMPL.
		/// Must be overridden to support string based address space.
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceRoot"]/
		///		method[@name="QueryAddressSpaceElementChildren"]/doc/*'
		///	/>
		public override int QueryAddressSpaceElementChildren(string anElementId, ArrayList aChildrenDetailsList)
		{
			aChildrenDetailsList = null;
			return (int) EnumResultCode.E_NOTIMPL;
		} //	end QueryAddressSpaceElementChildren


		/// <summary>
		/// Changes the cache value for the Address space elements provided
		/// </summary>
		/// <param name="anElementList">the namespace elements</param>
		/// <param name="aValueList">the corresponding values</param>
		/// <returns>
		/// E_INVALIDARG - Invalid valueQT was passed
		/// S_OK - Value changed
		/// OTS_E_EXCEPTION - Unexpected error occurred
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceRoot"]/
		///		method[@name="ValuesChanged.array"]/doc/*'
		///	/>
		public int ValuesChanged(DaAddressSpaceElement[] anElementList, ValueQT[] aValueList)
		{
			if (anElementList.Length != aValueList.Length)
			{
				return (int) EnumResultCode.E_INVALIDARG;
			} //	end if

			int count = anElementList.Length;

			if (count == 0)
			{
				return (int) EnumResultCode.S_FALSE;
			} //	end if

			OTValueData[] aoValues = new OTValueData[count];
			uint[] ahObjects = new uint[count];

			for (int i = 0; i < count; i++)
			{
				ahObjects[i] = anElementList[i].ObjectHandle;
				aoValues[i] = new OTValueData();
				aoValues[i].m_quality = (ushort) aValueList[i].Quality;
				aoValues[i].m_timestamp = new OTDateTime(aValueList[i].TimeStamp);
				aoValues[i].m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
				Marshal.GetNativeVariantForObject(aValueList[i].Data, aoValues[i].m_value);
			} //	end for

			int result = OTBFunctions.OTSValuesChanged(count, ahObjects, aoValues);

			for (int i = 0; i < count; i++)
			{
				OTBFunctions.OTVariantClear(aoValues[i].m_value);
				Marshal.FreeCoTaskMem(aoValues[i].m_value);
			} //	end for

			return result;
		} //	end ValuesChanged


		/// <summary>
		/// Set a new set of ValueQT values to the Elements provided.
		/// </summary>
		/// <param name="anElementsList">the list of elements.</param>
		/// <param name="aValueList">the ValueQT for each element in the list.</param>
		/// <returns>
		/// E_INVALIDARG - Invalid valueQT was passed
		/// S_OK - Value changed
		/// OTS_E_EXCEPTION - Unexpected error occurred
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceRoot"]/
		///		method[@name="ValuesChanged.list"]/doc/*'
		///	/>
		public virtual int ValuesChanged(ArrayList anElementsList, ArrayList aValueList)
		{
			int count = anElementsList.Count;
			if (count == 0)
			{
				return (int) EnumResultCode.S_FALSE;
			} //	end if

			if (count != aValueList.Count)
			{
				return (int) EnumResultCode.S_FALSE;
			} //	end if

			DaAddressSpaceElement[] elementList = new DaAddressSpaceElement[count];
			ValueQT[] valueList = new ValueQT[count];

			for (int index = 0; index < count; index++)
			{
				elementList[index] = anElementsList[index] as DaAddressSpaceElement;
				valueList[index] = aValueList[index] as ValueQT;
			} //	end foreach

			return ValuesChanged(elementList, valueList);
		} //	end ValuesChanged


		//-
		#endregion

		#region //	Public Events
		//-----------------------

		/// <summary>
		/// This event is triggered when the state of at least one item has been changed.
		/// The changes refers to the activation state which can be true or false and the update rate.
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaAddressSpaceRoot"]/
		///		event[@name="ActivationStateUpdated"]/doc/*'
		///	/>
		public event ActivationStateUpdated ActivationStateUpdated = null;

		//-
		#endregion

		#region //	Internal Methods
		//--------------------------

		/// <summary>
		/// called to change the items structure
		/// </summary>
		internal uint ChangeItems(uint aCount, IntPtr anItemDataPointer)
		{
			bool errorFound = false;

			if (anItemDataPointer == IntPtr.Zero || aCount == 0)
			{
				return (uint) EnumResultCode.S_FALSE;
			} //	end if

			IntPtr currentPointer = anItemDataPointer;

			lock (m_elements.SyncRoot) // enumerating
			{
				for (int i = 0; i < aCount; i++)
				{
					OTSItemData itemData = (OTSItemData) Marshal.PtrToStructure(currentPointer, typeof (OTSItemData));
					currentPointer = new IntPtr(currentPointer.ToInt64() + Marshal.SizeOf(typeof (OTSItemData)));

					DaAddressSpaceElement element = GetElementFromArray(itemData.m_object.m_userData) as DaAddressSpaceElement;

					if (element != null)
					{
						bool active = itemData.m_active != 0;
						element.Change(active, itemData.m_sampleRate);
					}
					else
					{
						errorFound = true;
					} //	end if ... else
				} //	end for
			}

			if (this.ActivationStateUpdated != null)
			{
				this.ActivationStateUpdated();
			} //	end if

			return (uint) (errorFound ? EnumResultCode.E_FAIL : EnumResultCode.S_OK);
		} //	end ChangeItems


		//-
		#endregion
	} //	end class DaAddressSpaceRoot
} //	end namespace Softing.OPCToolbox.Server
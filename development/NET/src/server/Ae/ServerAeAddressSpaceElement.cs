using System;
using System.Collections;
using System.Runtime.InteropServices;
using Softing.OPCToolbox.OTB;
using Softing.OPCToolbox.Server;

namespace Softing.OPCToolbox.Server
{
	/// <summary>
	/// AeAddressSpaceElement is the base class for defining the OPC server's
	/// address space element.
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="AeAddressSpaceElement"]/doc/*'
	///	/>
	public class AeAddressSpaceElement : AddressSpaceElement
	{
		#region //	Public Constructors
		//--------------------------------

		/// <summary>
		/// Internal constructor for the AeAddressSpace Element
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAddressSpaceElement"]/
		///			ctor[@name="AeAddressSpaceElement" or @name="AeAddressSpaceElement.default"]/doc/*'
		///	/>
		public AeAddressSpaceElement()
		{
		} //	end ctr


		/// <summary>
		/// Constructor for the AeAddressSpaceElement. For dynamically usage only
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAddressSpaceElement"]/
		///			ctor[@name="AeAddressSpaceElement" or @name="AeAddressSpaceElement.all"]/doc/*'
		///	/>
		public AeAddressSpaceElement(
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle) :
				base(aName, anUserData, anObjectHandle, aParentHandle)
		{
		} //	end ctr


		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		///  Adds a namespace element as child to a namespace element
		/// </summary>
		/// <param name="aChild">Child to be added</param>
		/// <returns>
		/// true	- Child successfully added
		/// false	- Child was not added
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="AeAddressSpaceElement"]/
		///		method[@name="AddChild"]/doc/*'
		///	/>
		public override bool AddChild(AddressSpaceElement aChild)
		{
			if (!HasChildren)
			{
				Application.Instance.Trace(
					EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER,
					"AeAddressSpaceElement.AddChild", "this element does not allow childred");
				return false;
			} //	end if

			AeAddressSpaceElement child = aChild as AeAddressSpaceElement;
			if (child == null)
			{
				Application.Instance.Trace(
					EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER,
					"AeAddressSpaceElement.AddChild", "Invalid child provided");
				return false;
			} //	end if

			//	only add the child if no handle assigned
			if (child.ObjectHandle == 0)
			{
				//	Prepare for the add addressSpace element call
				OTSAddressSpaceElementData data = new OTSAddressSpaceElementData();

				data.m_name = Marshal.StringToCoTaskMemUni(child.Name);
				data.m_itemID = Marshal.StringToCoTaskMemUni(
					Name + Application.Instance.AddressSpaceDelimiter + child.Name);

				data.m_hasChildren = Convert.ToByte(child.HasChildren);
				data.m_isBrowsable = Convert.ToByte(child.IsBrowsable);
				data.m_elementType = (byte) EnumAddressSpaceElementType.AE;
				data.m_userData = child.UserData;

				Application.Instance.AeAddressSpaceRoot.AddElementToArray(child);

				int result = OTBFunctions.OTSAddAddressSpaceElement(m_objectHandle, data, out child.m_objectHandle);

				Marshal.FreeCoTaskMem(data.m_name);
				Marshal.FreeCoTaskMem(data.m_itemID);

				if (!ResultCode.SUCCEEDED(result))
				{
					result = Application.Instance.AeAddressSpaceRoot.RemoveElementFromArray(child);
					System.Diagnostics.Debug.Assert(ResultCode.SUCCEEDED(result),
					                                "RemoveElementFromArray has failed");
					return false;
				} //	end if
			}
			else
			{
				Application.Instance.AeAddressSpaceRoot.AddElementToArray(child);
			} //	end if ... else

			return base.AddChild(aChild);
		} //	end AddChild


		/// <summary>
		/// Removes Child and its children if there are some
		/// </summary>
		/// <param name="aChild">AddressSpaceElement to be removed</param>
		/// <returns>
		/// true	- Child removed
		/// false	- Child was not removed
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAddressSpaceElement"]/
		///			method[@name="RemoveChild"]/doc/*'
		///	/>
		public override bool RemoveChild(AddressSpaceElement aChild)
		{
			AeAddressSpaceElement child = aChild as AeAddressSpaceElement;

			if (child == null)
			{
				Application.Instance.Trace(EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER,
				                           "AeAddressSpaceElement.RemoveChild", "Invalid child provided");
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
				                           "AeAddressSpaceElement.RemoveChild",
				                           "OTSRemoveAddressSpaceElement has failed with code" + result.ToString());
				return false;
			} //	end if

			result = Application.Instance.AeAddressSpaceRoot.RemoveElementFromArray(child);
			if (!ResultCode.SUCCEEDED(result))
			{
				Application.Instance.Trace(EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER,
				                           "AeAddressSpaceElement.RemoveChild",
				                           "OTSRemoveAddressSpaceElement has failed with code" + result.ToString());
				return false;
			} //	end if

			return base.RemoveChild(aChild);
		} //	end RemoveChild


		/// <summary>
		/// Returns the element`s parent
		/// </summary>
		/// <returns>
		/// The parent of this element.
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAddressSpaceElement"]/
		///			method[@name="GetParent"]/doc/*'
		///	/>
		public override AddressSpaceElement GetParent()
		{
			return Application.Instance.AeAddressSpaceRoot.GetParent(this.ObjectHandle);
		} //	end GetParent


		/// <summary>
		/// Returns a list of children. This list is empty in case of an error or if no children are there to be delivered
		/// </summary>
		/// <returns>
		/// List of this element's children
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAddressSpaceElement"]/
		///			method[@name="GetChildren"]/doc/*'
		///	/>
		public override ArrayList GetChildren()
		{
			ArrayList children = new ArrayList();
			IntPtr ptrData = IntPtr.Zero;
			int count = 0;

			int res = OTBFunctions.OTSGetChildren(m_objectHandle, (byte) EnumAddressSpaceElementType.AE, out count, out ptrData);

			if (ResultCode.SUCCEEDED(res))
			{
				if (count > 0)
				{
					OTObjectData TypeOfObjData = new OTObjectData();
					int size = Marshal.SizeOf(TypeOfObjData);

					lock (Application.Instance.AeAddressSpaceRoot.ElementSyncRoot)
					{
						for (int i = 0; i < count; i++)
						{
							IntPtr currentPtr = new IntPtr(ptrData.ToInt64() + size*i);
							OTObjectData myData = (OTObjectData) Marshal.PtrToStructure(currentPtr, typeof (OTObjectData));
							children.Add(
								Application.Instance.AeAddressSpaceRoot.GetElementFromArray(myData.m_userData) as AeAddressSpaceElement);
						} //	end for
					}
				} //	end if

				OTBFunctions.OTFreeMemory(ptrData);
			} //	end if

			return children;
		} //	end GetChildren


		/// <summary>
		/// Provides the available conditions for this address space element.
		/// This virtual function has no default implementation. The user should override it with the appropriate implementation
		/// </summary>
		/// <param name="aSourcePath">
		/// the AE address space source path
		/// </param>
		/// <param name="aConditionNames">
		/// out parameter; filed by the method with the list of strings
		/// </param>
		/// <returns>
		/// EnumResultCode.E_NOTIMPL - this must be implemented if
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAddressSpaceElement"]/
		///			method[@name="QueryConditions"]/doc/*'
		///	/>
		public virtual int QueryConditions(
			string aSourcePath,
			out string[] aConditionNames)
		{
			aConditionNames = null;
			return (int) EnumResultCode.E_NOTIMPL;
		} //	end QueryConditions


		//-
		#endregion
	} //	end class AeAddressSpaceElement

	/// <summary>
	/// Implementation of the OPC-AE Address Space Root node
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="AeAddressSpaceRoot"]/doc/*'
	///	/>
	public class AeAddressSpaceRoot : AddressSpaceRoot
	{
		#region //	Public Constructors
		//-----------------------------

		/// <summary>
		/// Creates a specified type fo address space space root node.
		/// </summary>
		/// <param name="anAddressSpaceType">object, string or both - based address space type
		/// </param>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAddressSpaceRoot"]/
		///			ctor[@name="AeAddressSpaceRoot" or @name="AeAddressSpaceRoot.type"]/doc/*'
		///	/>
		public AeAddressSpaceRoot(EnumAddressSpaceType anAddressSpaceType) :
			base(anAddressSpaceType,
			     new AeAddressSpaceElement("AeRoot", 0, 0, 0))
		{
		} //	end constructor


		/// <summary>
		/// Creates an object space root node.
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAddressSpaceRoot"]/
		///			ctor[@name="AeAddressSpaceRoot" or @name="AeAddressSpaceRoot.default"]/doc/*'
		///	/>
		public AeAddressSpaceRoot() : this(EnumAddressSpaceType.OBJECT_STRING_BASED)
		{
		} //	end constructor


		//-
		#endregion

		#region	//	Public Methods
		//------------------------

		/// <summary>
		/// Queries for a string based address space element
		/// </summary>
		/// <param name="anElementId">Address space path of the element</param>
		/// <param name="anElement">Out parameter - the element</param>
		/// <returns>
		/// EnumResultCode.E_NOTIMPL - Must be overridden to support string based address space
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAddressSpaceRoot"]/
		///			method[@name="QueryAddressSpaceElementData"]/doc/*'
		///	/>
		public override int QueryAddressSpaceElementData(string anElementId, out AddressSpaceElement anElement)
		{
			anElement = null;
			return (int) EnumResultCode.E_NOTIMPL;
		} //	end QueryAddressSpaceElementData


		/// <summary>
		/// Queries for a string based address space element (extended version)
		/// </summary>
		/// <param name="anElementId">Address space path of the element</param>
		/// <param name="elementDataFilter">Type of the element data to be filled in</param>
		/// <param name="anElement">Out parameter - the element</param>
		/// <returns>
		/// EnumResultCode.E_NOTIMPL - Must be overridden to support string based address space
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAddressSpaceRoot"]/
		///			method[@name="QueryAddressSpaceElementDataEx"]/doc/*'
		///	/>
		public override int QueryAddressSpaceElementDataEx(string anElementId, ref EnumAddressSpaceElementData elementDataFilter, out AddressSpaceElement anElement)
		{
			anElement = null;
			return (int) EnumResultCode.E_NOTIMPL;
		} //	end QueryAddressSpaceElementDataEx


		/// <summary>
		/// Returns the list of children of the string based addess space element.
		/// </summary>
		/// <param name="anElementId">Address space path of the element</param>
		/// <param name="aChildrenList">List to be filled with the children elements</param>
		/// <returns>
		/// EnumResultCode.E_NOTIMPL - Must be overridden to support string based addess space
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAddressSpaceRoot"]/
		///			method[@name="QueryAddressSpaceElementChildren"]/doc/*'
		///	/>
		public override int QueryAddressSpaceElementChildren(string anElementId, ArrayList aChildrenList)
		{
			aChildrenList.Clear();
			return (int) EnumResultCode.E_NOTIMPL;
		} //	end QueryAddressSpaceElementChildren


		/// <summary>
		/// retireves the conditions strings for a specified <see cref="AeAddressSpaceElement"/>
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAddressSpaceRoot"]/
		///			method[@name="QueryConditions"]/doc/*'
		///	/>
		public virtual int QueryConditions(
			uint anUserData,
			string aSourcePath,
			out string[] aConditionNames)
		{
			AeAddressSpaceElement element = (AeAddressSpaceElement) GetElementFromArray(anUserData);

			if (element != null)
			{
				return element.QueryConditions(aSourcePath, out aConditionNames);
			}
			else
			{
				aConditionNames = null;
				return (int) EnumResultCode.S_FALSE;
			} //	end if ... else
		} //	end QueryConditions


		//-
		#endregion
	} //	end class AeAddressSpaceRoot
} //	end namespace Softing.OPCToolbox.Server
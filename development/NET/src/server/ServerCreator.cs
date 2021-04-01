using System;
using System.Runtime.InteropServices;

namespace Softing.OPCToolbox.Server
{
	/// <summary>
	/// Helper class used as factory for various object instances of
	/// the OPC server components. This class is used by the Toolkit internal
	/// mechanisms to instantiate user specific objects. To enable this,
	/// the user must extend the Creator class and override the create methods.
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="Creator"]/doc/*'
	///	/>
	public class Creator
	{
		#region //	Public overridable methods
		//------------------------------------

		/// <summary>
		/// Creates the DaAddressSpaceRoot instance. This method should be overriden when a custom root is wanted.
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="Creator"]/
		///		method[@name="CreateDaAddressSpaceRoot"]/doc/*'
		///	/>
		public virtual DaAddressSpaceRoot CreateDaAddressSpaceRoot()
		{
			return new DaAddressSpaceRoot();
		} //	end CreateDaAddressSpaceRoot


		/// <summary>
		/// Creates the AeAddressSpaceRoot instance
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="Creator"]/
		///		method[@name="CreateAeAddressSpaceRoot"]/doc/*'
		///	/>
		public virtual AeAddressSpaceRoot CreateAeAddressSpaceRoot()
		{
			return new AeAddressSpaceRoot();
		} //	end CreateAeAddressSpaceRoot


		/// <summary>
		/// Creates the DaAddressSpaceElement instance. This method should be overriden when a custom root is wanted.
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="Creator"]/
		///		method[@name="CreateDaAddressSpaceElement"]/doc/*'
		///	/>
		public virtual DaAddressSpaceElement CreateDaAddressSpaceElement()
		{
			return new DaAddressSpaceElement();
		} //	end CreateDaAddressSpaceRoot


		/// <summary>
		/// Creates the AeAddressSpaceElement instance. This method should be overriden when a custom root is wanted.
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="Creator"]/
		///		method[@name="CreateAeAddressSpaceElement"]/doc/*'
		///	/>
		public virtual AeAddressSpaceElement CreateAeAddressSpaceElement()
		{
			return new AeAddressSpaceElement();
		} //	end CreateAeAddressSpaceRoot


		/// <summary>
		/// Creates a new instance of DaAddressSpaceElement for string based address space only
		/// </summary>
		/// <param name="anItemId">Item ID of the element to be created</param>
		/// <param name="aName">Iten name of the element to be created</param>
		/// <param name="anUserData">User data for the element to be created</param>
		/// <param name="anObjectHandle">Toolkit CORE object handle of the element to be created</param>
		/// <param name="aParentHandle">Toolkit CORE object handle of the parent of the element to be created</param>
		/// <returns></returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="Creator"]/
		///		method[@name="CreateInternalDaAddressSpaceElement"]/doc/*'
		///	/>
		public virtual DaAddressSpaceElement CreateInternalDaAddressSpaceElement(
			string anItemId,
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle)
		{
			return new DaAddressSpaceElement(anItemId, aName, anUserData, anObjectHandle, aParentHandle);
		} //	end CreateInternalDaAddressSpaceElement


		/// <summary>
		/// Creates a new instance of DaAddressSpaceElement for string based address space only (extended version).
		/// <br/>Must return a valid <see cref="DaAddressSpaceElement"/> object in order to have the Toolkit CORE use this extended method.
		/// <br/>If <see cref="CreateInternalDaAddressSpaceElementEx"/> method is correctly implemented, <see cref="CreateInternalDaAddressSpaceElement"/>
		/// will be ignored (will never be called unless <see cref="CreateInternalDaAddressSpaceElementEx"/> fails.
		/// </summary>
		/// <param name="anItemId">Item ID of the element to be created</param>
		/// <param name="aName">Iten name of the element to be created</param>
		/// <param name="anUserData">User data for the element to be created</param>
		/// <param name="anObjectHandle">Toolkit CORE object handle of the element to be created</param>
		/// <param name="aParentHandle">Toolkit CORE object handle of the parent of the element to be created</param>
		/// <param name="elementDataFilter">Type of element data which is not set in the parameters below</param>
		/// <param name="accessRights">Element`s access rights, valid if <see cref="EnumAddressSpaceElementData.ACCESSRIGHTS"/> flag is not set in elementDataFilter</param>
		/// <param name="ioMode">Element`s IO mode, valid if <see cref="EnumAddressSpaceElementData.IOMODE"/> is not set in elementDataFilter</param>
		/// <param name="hasChildren">Whether the element has children or not (is node or tag), valid if <see cref="EnumAddressSpaceElementData.HASCHILDREN"/> is not set in elementDataFilter</param>
		/// <param name="isBrowsable">Whether the elementis browsable, valid if <see cref="EnumAddressSpaceElementData.ISBROWSABLE"/> is not set in elementDataFilter</param>
		/// <param name="dataType">Element`s data type, valid if <see cref="EnumAddressSpaceElementData.DATATYPE"/> flag is not set in elementDataFilter</param>
		/// <returns></returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="Creator"]/
		///		method[@name="CreateInternalDaAddressSpaceElementEx"]/doc/*'
		///	/>
		public virtual DaAddressSpaceElement CreateInternalDaAddressSpaceElementEx(
			string anItemId,
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle,
			EnumAddressSpaceElementData elementDataFilter,
			EnumAccessRights accessRights,
			EnumIoMode ioMode,
			bool hasChildren, // aka isNode
			bool isBrowsable,
			Type dataType)
		{
			return null;
		} //	end CreateInternalDaAddressSpaceElement


        /// <summary>
		/// Creates a new instance of AddressSpaceElement for string based address space only
		/// </summary>
		/// <param name="aName"></param>
		/// <param name="anUserData"></param>
		/// <param name="anObjectHandle"></param>
		/// <param name="aParentHandle"></param>
		/// <returns></returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="Creator"]/
		///		method[@name="CreateInternalAeAddressSpaceElement"]/doc/*'
		///	/>
		public virtual AeAddressSpaceElement CreateInternalAeAddressSpaceElement(
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle)
		{
			return new AeAddressSpaceElement(aName, anUserData, anObjectHandle, aParentHandle);
		} //	end CreateInternalAeAddressSpaceElement


		/// <summary>
		/// Toolkit internal creator of a <see cref="DaRequest"/>. By overloading this method, the user can determine creation of custom Request objects.
		/// </summary>
		/// <returns></returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="Creator"]/
		///		method[@name="CreateRequest"]/doc/*'
		///	/>
		public virtual DaRequest CreateRequest(
			EnumTransactionType aTransactionType,
			uint aSessionHandle,
			DaAddressSpaceElement anElement,
			int aPropertyId,
			uint aRequestHandle)
		{
			return new DaRequest(aTransactionType, aSessionHandle, anElement, aPropertyId, aRequestHandle);
		} //	end CreateRequest


		/// <summary>
		/// Toolkit internal creator of a <see cref="DaTransaction"/>. By overloading this method, the user can determine creation of custom Transaction objects.
		/// All parameters should be forwarded directly to the custom Transaction class.
		/// </summary>
		/// <param name="aTransactionType"></param>
		/// <param name="aRequests"></param>
		/// <param name="aSessionKey"></param>
		/// <returns></returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="Creator"]/
		///		method[@name="CreateTransaction"]/doc/*'
		///	/>
		public virtual DaTransaction CreateTransaction(
			EnumTransactionType aTransactionType,
			DaRequest[] aRequests,
			uint aSessionKey)
		{
			return new DaTransaction(aTransactionType, aRequests, aSessionKey);
		} //	end CreateTransaction


		/// <summary>
		/// Toolkit internal creator of a <see cref="DaSession"/>. By overloading this method, the user can determine creation of custom Session objects.
		/// The number of connected clients can be limited. This can be achieved by returning null instead of a new instance of the DaSession class if the
		/// number of clients exceeds a desired number.
		/// </summary>
		/// <param name="aType"></param>
		/// <param name="aHandle"></param>
		/// <returns></returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="Creator"]/
		///		method[@name="CreateSession"]/doc/*'
		///	/>
		public virtual DaSession CreateSession(
			EnumSessionType aType,
			uint aHandle)
		{
			return new DaSession(aType, aHandle);
		} //	end CreateSession


		/// <summary>
		/// Toolkit internal creator of a <see cref="WebTemplate"/>. By overloading this method, the user can determine creation of a custom WebTemplate handler instance.
		///	If web sever is used, this method shuld be overloaded
		/// </summary>
		/// <returns></returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="Creator"]/
		///		method[@name="CreateWebTemplate"]/doc/*'
		///	/>
		public virtual WebTemplate CreateWebTemplate()
		{
			return new WebTemplate();
		} //	end CreateWebTemplate


		//-
		#endregion
	} //	end  class Creator
} //	end namespace Softing.OPCToolbon.Server
using System;
using System.Runtime.InteropServices;

#if !NET35 && !NET40 && !NET45 && !NET46 && !NET461

namespace Softing.OPCToolbox.OTB
{

	#region OTB functions
	internal class OTBFunctions
	{
#if DEBUG
		//const string  OTB_DLL = "OTBuD.dll";
		const string  OTB_DLL = "OTBu.dll";
#elif NDEBUG
		const string  OTB_DLL = "OTBu.dll";
#endif

		// common
		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTActivate(
			byte feature,
			String key,
			byte forceDemo);

		[DllImport(OTB_DLL)]
		public static extern int OTTrace(
			byte traceLevel,
			uint traceMask,
			[MarshalAs(UnmanagedType.LPWStr)]string ObjID,
			[MarshalAs(UnmanagedType.LPWStr)]string Message);

		[DllImport(OTB_DLL)]
		public static extern int OTEnableTracing(
			uint aTraceDataMask,
			OTCTraceData aTraceData);

		[DllImport(OTB_DLL)]
		public static extern int OTFreeMemory(IntPtr Handle);

		[DllImport(OTB_DLL)]
		public static extern IntPtr OTAllocateMemory(int aSize);

		[DllImport(OTB_DLL, CharSet=CharSet.Unicode)]
		public static extern IntPtr OTVariantToString(
								IntPtr pValue, //OTVariant*
								uint stringLength,
								IntPtr input);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTVariantClear(IntPtr input);

		// client
		[DllImport(OTB_DLL)]
		public static extern int OTCInitialize(OTCInitData initData);

		[DllImport(OTB_DLL)]
		public static extern int OTCProcessCommandLine([MarshalAs(UnmanagedType.LPWStr)]string commandLine);

		[DllImport(OTB_DLL)]
		public static extern int OTCTerminate();

		[DllImport(OTB_DLL)]
		public static extern int OTCAdvise(
			[MarshalAs(UnmanagedType.LPStruct)]OTCCallbackFunctions callbacks);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCAddDASession(
			string url,
			uint sessionUserData,
			ref uint objectHandle);

		[DllImport(OTB_DLL)]
		public static extern int OTCAddDASubscription(
			uint sessionHandle,
			uint updateRate ,
			uint groupUserData,
			ref uint objectHandle);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCAddDAItem(
			uint groupHandle,string itemID,
			uint itemUserData,
			ref uint objectHandle);

		[DllImport(OTB_DLL)]
		public static extern int OTCRemoveDASession(uint objectHandle);

		[DllImport(OTB_DLL)]
		public static extern int OTCRemoveDASubscription(uint objectHandle);

		[DllImport(OTB_DLL)]
		public static extern int OTCRemoveDAItem(uint objectHandle);

		[DllImport(OTB_DLL)]
		public static extern int OTCGetAttributes(
			uint objectHandle,
			uint whatAttributes,
			[In,Out]OTCObjectAttributes pObjectAttributes);

		[DllImport(OTB_DLL)]
		public static extern int OTCSetAttributes(
			uint objectHandle,
			uint whatAttributes,
			[In,Out]OTCObjectAttributes pObjectAttributes);

		[DllImport(OTB_DLL)]
		public static extern int OTCUpdateAttributes(
			uint objectHandle,
			byte fromServer,
			uint attributeCount,
			[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=2)]uint[] pWhatAttributes,
			[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=2)][In,Out] int[] pResults,
			ref OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL)]
		public static extern int OTCUpdateDAItemAttributes(
			uint subscriptionHandle,
			int itemCount,
			uint[] itemHandles,
			byte fromServer,
			uint attributeCount,
			[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=6)] uint[] whatAttributes,
			[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=6)][In,Out]int[] results,
			ref OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL)]
		public static extern int OTCPerformStateTransition(
			uint objectHandle,
			byte deep,
			ref OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCChangeTargetState(
			uint objectHandle,
			byte state,
			byte deep);

		[DllImport(OTB_DLL)]
		public static extern int OTCGetState(
			uint objectHandle,
			out byte currentState,
			out byte targetState);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCRead(
			uint objectHandle,
			int count,
			[MarshalAs(UnmanagedType.LPArray)]uint[] itemHandles,
			[MarshalAs(UnmanagedType.LPArray)]string[] itemIDs,
			[MarshalAs(UnmanagedType.LPArray)]string[] itemPaths,
			uint maxAge,
			[MarshalAs(UnmanagedType.LPArray)][In,Out]OTValueData[] values,
			[In,Out]int[] results,
			ref OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCWrite(
			uint objectHandle,
			int count,
			[MarshalAs(UnmanagedType.LPArray)] uint[] itemHandles,
			string[] itemIDs,
			string[] itemPaths,
			[MarshalAs(UnmanagedType.LPArray)]OTValueData[] values,
			[In,Out]int[] results,
			ref OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL)]
		public static extern int OTCGetServerStatus(
			uint sessionHandle,
			[In,Out]OTCServerStatus serverStatus,
			ref OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL)]
		public static extern int OTCActivateConnectionMonitor(
			uint sessionHandle,
			byte activate,
			uint checkCycle,
			uint connect1Attempts,
			uint connect1Cycle,
			uint connect2Cycle);

		[DllImport(OTB_DLL)]
		public static extern int OTCValidateDAItems(
			uint subscriptionHandle,
			int itemCount,
			uint[] itemHandles,
			int[] results,
			ref OTCExecutionOptions ExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCBrowseServer(
			string ipAddress,
			byte whatOPCSpec,
			byte whatServerData,
			ref uint serverDataCount,
			out IntPtr ppServerData,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCBrowseAddressSpace(
			uint sessionHandle,
			uint addressSpaceElementHandle,
			string addressSpaceElementID,
			string addressSpaceElementPath,
			ref OTCAddressSpaceBrowseOptions pBrowseOptions,
			ref uint pAddressSpaceElementDataCount,
			out IntPtr ppAddressSpaceElement,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCReleaseAddressSpaceElement(
			uint addressSpaceElementHandle,
			int deep);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCGetDAProperties(
			uint aSessionHandle,
			uint anAddressSpaceElementHandle,
			string addressSpaceElementID,
			string addressSpaceElementPath,
			OTCGetDAPropertiesOptions pGetPropertiesOptions,
			out uint pPropertiesDataCount,
			out IntPtr ppProperty,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCAddAESession(
			string url,
			uint sessionUserData,
			ref uint pObjectHandle);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCAddAESubscription(
			uint sessionHandle,
			uint subscriptionUserData,
			ref uint pSubscriptionHandle);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCRefreshDAGroup(
			uint subscriptionHandle,
			uint maxAge,
			ref OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCRemoveAESession(uint objectHandle);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCRemoveAESubscription(uint objectHandle);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCRefreshAEConditions(
			uint subscriptionHandle,
			byte cancelRefresh,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCAcknowledgeAEConditions(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string ackID,
			[MarshalAs(UnmanagedType.LPWStr)] string ackComment,
			uint count,
			[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3)] OTCEventData[] pEvents,
			[In,Out]int[] results,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCQueryAvailableAEFilters(
			uint sessionHandle,
			out uint availableFilters,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCQueryAECategories(
			uint sessionHandle,
			out uint categoryCount,
			out IntPtr ppEventTypes,
			out IntPtr ppCategoryIds,
			out IntPtr ppCategoryDescriptions,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCQueryAEAttributes(
			uint sessionHandle,
			uint categoryId,
			out uint pCount,
			out IntPtr ppAttributeIds,
			out IntPtr ppAttributeDescriptions,
			out IntPtr ppAttributeDatatypes,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCQueryAEConditions(
			uint sessionHandle,
			uint categoryId,
			out uint pCount,
			out IntPtr ppConditionsNames,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCQueryAESubConditions(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)]string conditionName,
			out uint pCount,
			out IntPtr ppSubConditionsNames,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCQueryAESourceConditions(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)]string sourcePath,
			out uint pCount,
			out IntPtr ppConditionsNames,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCGetAEConditionState(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)]string sourcePath,
			[MarshalAs(UnmanagedType.LPWStr)]string conditionName,
			uint  attributeCount,
			[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3)] uint[] pAttributeIds,
			[In,Out] OTCAEConditionStateData pConditionState,
			//out IntPtr pConditionState,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCEnableAEConditions(
			uint sessionHandle,
			byte enable,
			byte areaOrSource,
			[MarshalAs(UnmanagedType.LPWStr)]string path,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCGetErrorString(
			uint sessionHandle,
			int errorCode,
			out IntPtr errorString,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCLogon(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)]string userName,
			[MarshalAs(UnmanagedType.LPWStr)]string password,
			ref OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL,CharSet=CharSet.Unicode)]
		public static extern int OTCLogoff(
			uint sessionHandle,
			ref OTCExecutionOptions pExecutionOptions);

		// server
		[DllImport(OTB_DLL)]
		public static extern int OTSInitialize(OTSInitData pInitData);

		[DllImport(OTB_DLL)]
		public static extern int OTSTerminate();

		[DllImport(OTB_DLL)]
		public static extern int OTSProcessCommandLine([MarshalAs(UnmanagedType.LPWStr)]string commandLine);

		[DllImport(OTB_DLL)]
		public static extern int OTSSetServerState(uint daServerState, uint aeServerState);

		[DllImport(OTB_DLL)]
		public static extern int OTSAdvise(OTSCallbackFunctions callbacks);

		[DllImport(OTB_DLL)]
		public static extern int OTSStart();

		[DllImport(OTB_DLL)]
		public static extern int OTSReady();

		[DllImport(OTB_DLL)]
		public static extern int OTSStop([MarshalAs(UnmanagedType.LPWStr)]string reason);

		[DllImport(OTB_DLL)]
		public static extern int OTSAddAddressSpaceElement(
				uint hParent,
				OTSAddressSpaceElementData elementData,
				out uint hObject);

		[DllImport(OTB_DLL)]
		public static extern int OTSChangeUserdata(IntPtr hObject, int iUserData);

		[DllImport(OTB_DLL)]
		public static extern int OTSRemoveAddressSpaceElement(uint hObject);

		[DllImport(OTB_DLL)]
		public static extern int OTSCompleteRequests(
				int count,
				[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=0)]OTSRequestData[] aRequests,
				[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=0)]int[] aResult,
				[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=0)]OTValueData[] aValues);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAckRequestElement(
				uint hRequest);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSNackRequestElement(
				uint hRequest);

		[DllImport(OTB_DLL)]
		public static extern int OTSGetCacheValue(
				uint hObject,
				ref OTValueData val);

		[DllImport(OTB_DLL)]
		public static extern int OTSValuesChanged(
				int count,
				[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=0)]uint[] aObjects,
				[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=0)]OTValueData[] aValues);

		[DllImport(OTB_DLL)]
		public static extern int OTSGetParent(
				uint hObject,
				IntPtr pParent);

		[DllImport(OTB_DLL)]
		public static extern int OTSGetAddressSpaceElementData(
				IntPtr hObject,
				out IntPtr pData);

		[DllImport(OTB_DLL)]
		public static extern int OTSInitAddressSpace(byte anAddressSpaceType);

		[DllImport(OTB_DLL)]
		public static extern int OTSGetChildren(
				uint hObject,
				byte elementType,
				out int pCount,
				out System.IntPtr ppChildren);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSSetEUInfo(
				uint hObject,
				bool isEnumerated,
				int count,
				[MarshalAs(UnmanagedType.LPArray)]string[] itemIDs,
				double lowEU,
				double highEU);

		[DllImport(OTB_DLL)]
		public static extern int OTSAddEventCategory(
				uint aCategoryID,
				[MarshalAs(UnmanagedType.LPWStr)]string aDescription,
				uint eventType,
				out uint pCatHandle);

		[DllImport(OTB_DLL)]
		public static extern int OTSAddEventAttribute(
				uint hCategory,
				uint anAttributeID,
				[MarshalAs(UnmanagedType.LPWStr)]string aDescription,
				ushort aDataType);

		[DllImport(OTB_DLL)]
		public static extern int OTSFireEvents(
				uint anEventCount,
				IntPtr pEventData);

		[DllImport(OTB_DLL)]
		public static extern int OTSAddCondition(
				uint hCategory,
				[MarshalAs(UnmanagedType.LPWStr)]string aConditionName);

		[DllImport(OTB_DLL)]
		public static extern int OTSAddSubCondition(
				uint hCategory,
				[MarshalAs(UnmanagedType.LPWStr)]string aConditionName,
				[MarshalAs(UnmanagedType.LPWStr)]string aSubConditionName);

		[DllImport(OTB_DLL)]
		public static extern int OTSConditionsChanged(
				uint aCount,
				[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=0)]uint[] pObjects,
				IntPtr pConditionData,
				[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=0)][In,Out]uint[] pObjectsOut);

		[DllImport(OTB_DLL)]
		public static extern int OTSGetConditionData(
				IntPtr hObject,
				[MarshalAs(UnmanagedType.LPStruct)] out OTSConditionData pData);

		public static IntPtr AllocateOTBString(string aSource){

			if (aSource == null || aSource == string.Empty){
				return IntPtr.Zero;
			} //	end if

			IntPtr result = OTBFunctions.OTAllocateMemory((aSource.Length + 1) * Marshal.SizeOf(typeof(short)));
			if (result == IntPtr.Zero){
				return IntPtr.Zero;
			} //	end if

			for (int index = 0; index < aSource.Length; index++){
				Marshal.WriteInt16(result, index*Marshal.SizeOf(typeof(short)), (short)aSource[index]);
			} //	end for

			return result;

		} //	end AllocateOTBString

		public static IntPtr GetIntPtrOffset(IntPtr source, Int32 offset)
		{
			return (IntPtr)(source.ToInt32() + offset);
		} //	end GetIntPtrOffset
	}

	#endregion // OTB functions

	#region OTB callbacks
	//common

	internal delegate void OTOnTrace(
		[MarshalAs(UnmanagedType.LPWStr)]string traceString,
		ushort level,
		uint mask,
		[MarshalAs(UnmanagedType.LPWStr)]string objId,
		[MarshalAs(UnmanagedType.LPWStr)]string text);

	internal delegate void OTCOnStateChange(
		OTCObjectContext objectData,
		byte state);

	internal delegate void OTCOnDataChange(
		uint executionContext,
		OTCObjectContext objectContext,
		uint count ,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=2)]OTObjectData[] itemData,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=2)]OTValueData[] values,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=2)]int[] results);

	internal delegate void OTCOnReadComplete(
		uint executionContext,
		OTCObjectContext objectContext,
		int result,
		uint count,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3)]OTObjectData[] itemData,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3, ArraySubType=UnmanagedType.LPWStr)]  String[] itemIDs,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3, ArraySubType=UnmanagedType.LPWStr)] String[] itemPaths,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3)]OTValueData[] values,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3)] int[] results);

	internal delegate void OTCOnWriteComplete(
		uint executionContext,
		OTCObjectContext objectContext,
		int result,
		uint count,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3)]OTObjectData[] itemData,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3, ArraySubType=UnmanagedType.LPWStr)] String[] itemIDs,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3, ArraySubType=UnmanagedType.LPWStr)] String[] itemPaths,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3)]OTValueData[] values,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3)] int[]results);

	internal delegate void OTCOnPerfromStateTransition(
		uint executionContext,
		OTCObjectContext objectContext,
		int result);

	internal delegate bool OTCOnServerShutdown(
		OTObjectData sessionData,
		[MarshalAs(UnmanagedType.LPWStr)]string reason);

	internal delegate void OTCOnUpdateAttributes(
		uint executionContext,
		OTCObjectContext objectContext,
		int result,
		byte fromServer,
		uint attributesCount,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=4)] uint[] whatAttributes,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=4)] int[] results);

	internal delegate void OTCOnUpdateDAItemAttributes(
		uint executionContext,
		OTCObjectContext objectContext,
		int result,
		uint itemCount,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 3)] OTObjectData[] itemData,
		byte fromServer,
		uint attributesCount,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=6)] uint[] whatAttributes,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3)]int[] results);

	internal delegate void OTCOnValidateDAItems(
		uint executionContext,
		OTCObjectContext objectContext,
		int result,
		uint itemCount,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3)]OTObjectData[] objectDataArray,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=3)]int[] results);

	internal delegate void OTCOnGetServerStatus(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		[In,Out] OTCServerStatus serverStatus);

	internal delegate void OTCOnBrowseServer(
		uint executionContext,
		int result,
		[MarshalAs(UnmanagedType.LPWStr)]string ipAddress,
		byte whatOPCSpec,
		byte whatServerData,
		uint serverDataCount,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=5)]OTCServerData[] serverData);

	internal delegate void OTCOnBrowseAddressSpace(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		uint addressSpaceElementHandle,
		[MarshalAs(UnmanagedType.LPWStr)]String addressSpaceElementID,
		[MarshalAs(UnmanagedType.LPWStr)]String addressSpaceElementPath,
		ref OTCAddressSpaceBrowseOptions browseOptions,
		uint addressSpaceElementDataCount,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=7)]OTCAddressSpaceElementData[] addressSpaceElement);

	internal delegate void OTCOnGetDAProperties(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		uint addressSpaceElementHandle,
		[MarshalAs(UnmanagedType.LPWStr)]String addressSpaceElementID,
		[MarshalAs(UnmanagedType.LPWStr)]String addressSpaceElementPath,
		OTCGetDAPropertiesOptions getPropertiesOptions,
		uint propertyDataCount,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex=7)] OTCDAPropertyData[] propertyData);

	internal delegate void OTCOnReceivedEvents (
		OTCObjectContext objectContext,
		uint count,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 1)]OTCEventData[] pEvents,
		byte refresh,
		byte lastRefresh);

	internal delegate void OTCOnRefreshAEConditions(
		uint executionContext,
		OTCObjectContext objectContext,
		int result,
		byte cancelRefresh);

	internal delegate void OTCOnAcknowledgeAEConditions(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		[MarshalAs(UnmanagedType.LPWStr)]string  ackID,
		[MarshalAs(UnmanagedType.LPWStr)]string ackComment,
		uint count,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 5)] OTCEventData[] pEvents,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 5)] int[] pResults);

	internal delegate void OTCOnQueryAvailableAEFilters(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		uint availableFilters);

	internal delegate void OTCOnQueryAECategories(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		uint count,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 3)]uint[] pEventTypes,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 3)]uint[] pCategoryIds,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 3, ArraySubType=UnmanagedType.LPWStr)] string[] pCategoryDescriptions);

	internal delegate void OTCOnQueryAEAttributes(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		uint categoryId,
		uint count,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 4)]uint[] pAttributeIds,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 4,ArraySubType=UnmanagedType.LPWStr)]string[] pAttributeDescriptions,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 4)]ushort[] pAttributeDatatypes);

	internal delegate void OTCOnQueryAEConditions(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		uint categoryId,
		uint count,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 4, ArraySubType=UnmanagedType.LPWStr)] string[] pConditionsNames);

	internal delegate void OTCOnQueryAESubConditions(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		[MarshalAs(UnmanagedType.LPWStr)]string conditionName,
		uint count,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 4, ArraySubType=UnmanagedType.LPWStr)] string[] pSubConditionsNames);

	internal delegate void OTCOnQueryAESourceConditions(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		[MarshalAs(UnmanagedType.LPWStr)]string sourcePath,
		uint count,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 4, ArraySubType=UnmanagedType.LPWStr)] string[] pConditionsNames);

	internal delegate void OTCOnGetAEConditionState(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		[MarshalAs(UnmanagedType.LPWStr)]string sourcePath,
		[MarshalAs(UnmanagedType.LPWStr)]string conditionName,
		uint attributeCount,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 5)]uint[] pAttributeIds,
		OTCAEConditionStateData pConditionState);

	internal delegate void OTCOnEnableAEConditions(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		byte enable,
		byte areaOrSource,
		[MarshalAs(UnmanagedType.LPWStr)]string path);

	internal delegate void OTCOnGetErrorString(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		int errorCode,
		[MarshalAs(UnmanagedType.LPWStr)] string errorString);

	internal delegate void OTCOnLogon(
		uint executionContext,
		OTObjectData sessionData,
		int result,
		[MarshalAs(UnmanagedType.LPWStr)]string userName,
		[MarshalAs(UnmanagedType.LPWStr)]string password);

	internal delegate void OTCOnLogoff(
		uint executionContext,
		OTObjectData sessionData,
		int result);

	//server
	internal delegate int OTSShutdown();

	internal delegate int OTSHandleReadRequests(
		int count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)]OTSRequestData[] aRequests);

	internal delegate int OTSHandleWriteRequests(
		int count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)]OTSRequestData[] aRequests,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)]OTValueData[] aValues);

	internal delegate int OTSRequestCompletedInternally(
		uint pRequestData);

	internal delegate int OTSQueryProperties(
		IntPtr pObjectData,
		[MarshalAs(UnmanagedType.LPWStr)]string anItemId,
		int aPropertyId,
		out uint aPropertiesCount,
		out IntPtr pProperties);

	internal delegate int OTSQueryAddressSpaceElementData(
		[MarshalAs(UnmanagedType.LPWStr)]string path,
		byte elementType,
		IntPtr pData);

	internal delegate int OTSQueryAddressSpaceElementDataEx(
		[MarshalAs(UnmanagedType.LPWStr)]string path,
		byte elementType,
		ref ushort elementDataFilter,
		IntPtr pData);

	internal delegate int OTSQueryAddressSpaceElementChildren(
		[MarshalAs(UnmanagedType.LPWStr)]string path,
		byte elementType,
		out uint pCount,
		out IntPtr pElementData);

	internal delegate int OTSQueryAddressSpaceElementProperties(
		int aPropertyID,
		IntPtr aPropertyList);

	internal delegate int OTSChangeSessionState(
		uint aSessionHandle,
		OTSSessionData pSessionStateData);

	internal delegate int OTSQueryCacheValue(
		uint aSessionHandle,
		OTObjectData anObjectData,
		IntPtr pOutValue);

	internal delegate int OTSChangeItems(
		uint itemCnt,
		IntPtr pItemData);

	internal delegate int OTSCreateAddressSpaceElement(
		uint hParent,
		uint hObject,
		OTSAddressSpaceElementData pDataIn,
		IntPtr pDataOut);

	internal delegate int OTSCreateAddressSpaceElementEx(
		uint hParent,
		uint hObject,
		Softing.OPCToolbox.OTB.OTSAddressSpaceElementData pDataIn,
		ushort elementDataFilter,
		IntPtr pDataOut);

	internal delegate int OTSDestroyAddressSpaceElement(OTObjectData objectData);

	internal delegate int OTSQueryConditions(
		IntPtr pObjectData,
		[MarshalAs(UnmanagedType.LPWStr)]string aSourcePath,
		out uint aConditionCount,
		out IntPtr pConditionNames);

	internal delegate int OTSAcknowledgeCondition(
		OTObjectData pConditionData,
		[MarshalAs(UnmanagedType.LPWStr)]string anAckId,
		[MarshalAs(UnmanagedType.LPWStr)]string anAckComment);

	internal delegate int OTSQueryConditionDefinition(
		OTObjectData pConditionData,
		IntPtr aConditionDefinitionData);

	internal delegate int OTSEnableConditions(
		byte isEnable,
		[MarshalAs(UnmanagedType.LPWStr)]string anAddressSpaceElementPath);

	internal delegate int OTSWebHandleTemplate(
		[MarshalAs(UnmanagedType.LPWStr)]string aTemplateName,
		uint numArgs,
		[MarshalAs(UnmanagedType.LPArray,SizeParamIndex = 1, ArraySubType=UnmanagedType.LPWStr)] string[] pArgs,
		out IntPtr pResult);

	#endregion // OTB callbacks

} //	end namespace Softing.OPCToolbox.OTB

#else
// !NET35 && !NET40 && !NET45 && !NET46

namespace Softing.OPCToolbox.OTB_x86
{

	#region OTB functions
	internal class OTBFunctions
	{
#if DEBUG
		//const string  OTB_DLL = "OTBuD.dll";
		const string OTB_DLL = "OTBu.dll";
#elif NDEBUG
		private const string OTB_DLL = "OTBu.dll";
#endif

		// common
		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTActivate(
			byte feature,
			String key,
			byte forceDemo);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTTrace(
			byte traceLevel,
			uint traceMask,
			[MarshalAs(UnmanagedType.LPWStr)] string ObjID,
			[MarshalAs(UnmanagedType.LPWStr)] string Message);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTEnableTracing(
			uint aTraceDataMask,
			Softing.OPCToolbox.OTB.OTCTraceData aTraceData);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTFreeMemory(IntPtr Handle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern IntPtr OTAllocateMemory(int aSize);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern IntPtr OTVariantToString(
			IntPtr pValue, //OTVariant*
			uint stringLength,
			IntPtr input);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTVariantClear(IntPtr input);

		// client
		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCInitialize(Softing.OPCToolbox.OTB.OTCInitData initData);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCProcessCommandLine([MarshalAs(UnmanagedType.LPWStr)] string commandLine);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCTerminate();

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAdvise(
			[MarshalAs(UnmanagedType.LPStruct)] Softing.OPCToolbox.OTB.OTCCallbackFunctions callbacks);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAddDASession(
			string url,
			uint sessionUserData,
			ref uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAddDASubscription(
			uint sessionHandle,
			uint updateRate,
			uint groupUserData,
			ref uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAddDAItem(
			uint groupHandle, string itemID,
			uint itemUserData,
			ref uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRemoveDASession(uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRemoveDASubscription(uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRemoveDAItem(uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCGetAttributes(
			uint objectHandle,
			uint whatAttributes,
			[In, Out] Softing.OPCToolbox.OTB.OTCObjectAttributes pObjectAttributes);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCSetAttributes(
			uint objectHandle,
			uint whatAttributes,
			[In, Out] Softing.OPCToolbox.OTB.OTCObjectAttributes pObjectAttributes);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCUpdateAttributes(
			uint objectHandle,
			byte fromServer,
			uint attributeCount,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] uint[] pWhatAttributes,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] [In, Out] int[] pResults,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCUpdateDAItemAttributes(
			uint subscriptionHandle,
			int itemCount,
			uint[] itemHandles,
			byte fromServer,
			uint attributeCount,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 6)] uint[] whatAttributes,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 6)] [In, Out] int[] results,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCPerformStateTransition(
			uint objectHandle,
			byte deep,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCChangeTargetState(
			uint objectHandle,
			byte state,
			byte deep);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCGetState(
			uint objectHandle,
			out byte currentState,
			out byte targetState);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRead(
			uint objectHandle,
			int count,
			[MarshalAs(UnmanagedType.LPArray)] uint[] itemHandles,
			[MarshalAs(UnmanagedType.LPArray)] string[] itemIDs,
			[MarshalAs(UnmanagedType.LPArray)] string[] itemPaths,
			uint maxAge,
			[MarshalAs(UnmanagedType.LPArray)] [In, Out] Softing.OPCToolbox.OTB.OTValueData[] values,
			[In, Out] int[] results,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCWrite(
			uint objectHandle,
			int count,
			[MarshalAs(UnmanagedType.LPArray)] uint[] itemHandles,
			string[] itemIDs,
			string[] itemPaths,
			[MarshalAs(UnmanagedType.LPArray)] Softing.OPCToolbox.OTB.OTValueData[] values,
			[In, Out] int[] results,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCGetServerStatus(
			uint sessionHandle,
			[In, Out] Softing.OPCToolbox.OTB.OTCServerStatus serverStatus,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCActivateConnectionMonitor(
			uint sessionHandle,
			byte activate,
			uint checkCycle,
			uint connect1Attempts,
			uint connect1Cycle,
			uint connect2Cycle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCValidateDAItems(
			uint subscriptionHandle,
			int itemCount,
			uint[] itemHandles,
			int[] results,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions ExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCBrowseServer(
			string ipAddress,
			byte whatOPCSpec,
			byte whatServerData,
			ref uint serverDataCount,
			out IntPtr ppServerData,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCBrowseAddressSpace(
			uint sessionHandle,
			uint addressSpaceElementHandle,
			string addressSpaceElementID,
			string addressSpaceElementPath,
			ref Softing.OPCToolbox.OTB.OTCAddressSpaceBrowseOptions pBrowseOptions,
			ref uint pAddressSpaceElementDataCount,
			out IntPtr ppAddressSpaceElement,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCReleaseAddressSpaceElement(
			uint addressSpaceElementHandle,
			int deep);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCGetDAProperties(
			uint aSessionHandle,
			uint anAddressSpaceElementHandle,
			string addressSpaceElementID,
			string addressSpaceElementPath,
			Softing.OPCToolbox.OTB.OTCGetDAPropertiesOptions pGetPropertiesOptions,
			out uint pPropertiesDataCount,
			out IntPtr ppProperty,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAddAESession(
			string url,
			uint sessionUserData,
			ref uint pObjectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAddAESubscription(
			uint sessionHandle,
			uint subscriptionUserData,
			ref uint pSubscriptionHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRefreshDAGroup(
			uint subscriptionHandle,
			uint maxAge,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRemoveAESession(uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRemoveAESubscription(uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRefreshAEConditions(
			uint subscriptionHandle,
			byte cancelRefresh,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAcknowledgeAEConditions(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string ackID,
			[MarshalAs(UnmanagedType.LPWStr)] string ackComment,
			uint count,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] Softing.OPCToolbox.OTB.OTCEventData[] pEvents,
			[In, Out] int[] results,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCQueryAvailableAEFilters(
			uint sessionHandle,
			out uint availableFilters,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCQueryAECategories(
			uint sessionHandle,
			out uint categoryCount,
			out IntPtr ppEventTypes,
			out IntPtr ppCategoryIds,
			out IntPtr ppCategoryDescriptions,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCQueryAEAttributes(
			uint sessionHandle,
			uint categoryId,
			out uint pCount,
			out IntPtr ppAttributeIds,
			out IntPtr ppAttributeDescriptions,
			out IntPtr ppAttributeDatatypes,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCQueryAEConditions(
			uint sessionHandle,
			uint categoryId,
			out uint pCount,
			out IntPtr ppConditionsNames,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCQueryAESubConditions(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string conditionName,
			out uint pCount,
			out IntPtr ppSubConditionsNames,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCQueryAESourceConditions(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string sourcePath,
			out uint pCount,
			out IntPtr ppConditionsNames,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCGetAEConditionState(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string sourcePath,
			[MarshalAs(UnmanagedType.LPWStr)] string conditionName,
			uint attributeCount,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] uint[] pAttributeIds,
			[In, Out] Softing.OPCToolbox.OTB.OTCAEConditionStateData pConditionState,
			//out IntPtr pConditionState,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCEnableAEConditions(
			uint sessionHandle,
			byte enable,
			byte areaOrSource,
			[MarshalAs(UnmanagedType.LPWStr)] string path,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCGetErrorString(
			uint sessionHandle,
			int errorCode,
			out IntPtr errorString,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCLogon(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string userName,
			[MarshalAs(UnmanagedType.LPWStr)] string password,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCLogoff(
			uint sessionHandle,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		// server
		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSInitialize(Softing.OPCToolbox.OTB.OTSInitData pInitData);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSTerminate();

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSProcessCommandLine([MarshalAs(UnmanagedType.LPWStr)] string commandLine);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSSetServerState(uint daServerState, uint aeServerState);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAdvise(Softing.OPCToolbox.OTB.OTSCallbackFunctions callbacks);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSStart();

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSReady();

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSStop([MarshalAs(UnmanagedType.LPWStr)]string reason);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAddAddressSpaceElement(
			uint hParent,
			Softing.OPCToolbox.OTB.OTSAddressSpaceElementData elementData,
			out uint hObject);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSChangeUserdata(uint hObject, int iUserData);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSRemoveAddressSpaceElement(uint hObject);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSCompleteRequests(
			int count,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] Softing.OPCToolbox.OTB.OTSRequestData[] aRequests,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] int[] aResult,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] Softing.OPCToolbox.OTB.OTValueData[] aValues);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAckRequestElement(
			uint hRequest);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSNackRequestElement(
			uint hRequest);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSGetCacheValue(
			uint hObject,
			ref Softing.OPCToolbox.OTB.OTValueData val);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSValuesChanged(
			int count,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] uint[] aObjects,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] Softing.OPCToolbox.OTB.OTValueData[] aValues);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSGetParent(
			uint hObject,
			IntPtr pParent);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSGetAddressSpaceElementData(
			uint hObject,
			out IntPtr pData);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSInitAddressSpace(byte anAddressSpaceType);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSGetChildren(
			uint hObject,
			byte elementType,
			out int pCount,
			out IntPtr ppChildren);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSSetEUInfo(
			uint hObject,
			bool isEnumerated,
			int count,
			[MarshalAs(UnmanagedType.LPArray)] string[] itemIDs,
			double lowEU,
			double highEU);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAddEventCategory(
			uint aCategoryID,
			[MarshalAs(UnmanagedType.LPWStr)] string aDescription,
			uint eventType,
			out uint pCatHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAddEventAttribute(
			uint hCategory,
			uint anAttributeID,
			[MarshalAs(UnmanagedType.LPWStr)] string aDescription,
			ushort aDataType);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSFireEvents(
			uint anEventCount,
			IntPtr pEventData);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAddCondition(
			uint hCategory,
			[MarshalAs(UnmanagedType.LPWStr)] string aConditionName);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAddSubCondition(
			uint hCategory,
			[MarshalAs(UnmanagedType.LPWStr)] string aConditionName,
			[MarshalAs(UnmanagedType.LPWStr)] string aSubConditionName);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSConditionsChanged(
			uint aCount,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] uint[] pObjects,
			IntPtr pConditionData,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] [In, Out] uint[] pObjectsOut);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSGetConditionData(
			IntPtr hObject,
			[MarshalAs(UnmanagedType.LPStruct)] out Softing.OPCToolbox.OTB.OTSConditionData pData);

		public static IntPtr AllocateOTBString(string aSource)
		{
			if (aSource == null || aSource == string.Empty)
			{
				return IntPtr.Zero;
			} //	end if

			IntPtr result = OTBFunctions.OTAllocateMemory((aSource.Length + 1)*Marshal.SizeOf(typeof (short)));
			if (result == IntPtr.Zero)
			{
				return IntPtr.Zero;
			} //	end if

			for (int index = 0; index < aSource.Length; index++)
			{
				Marshal.WriteInt16(result, index*Marshal.SizeOf(typeof (short)), (short) aSource[index]);
			} //	end for

			return result;
		} //	end AllocateOTBString

	}

	#endregion // OTB functions
} //	end namespace Softing.OPCToolbox.OTB_x86

namespace Softing.OPCToolbox.OTB_x64
{

	#region OTB functions
	internal class OTBFunctions
	{
#if DEBUG
		//const string OTB_DLL = "OTBuDx64.dll";
		const string OTB_DLL = "OTBux64.dll";
#elif NDEBUG
		private const string OTB_DLL = "OTBux64.dll";
#endif
		// common
		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTActivate(
			byte feature,
			String key,
			byte forceDemo);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTTrace(
			byte traceLevel,
			uint traceMask,
			[MarshalAs(UnmanagedType.LPWStr)] string ObjID,
			[MarshalAs(UnmanagedType.LPWStr)] string Message);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTEnableTracing(
			uint aTraceDataMask,
			Softing.OPCToolbox.OTB.OTCTraceData aTraceData);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTFreeMemory(IntPtr Handle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern IntPtr OTAllocateMemory(int aSize);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern IntPtr OTVariantToString(
			IntPtr pValue, //OTVariant*
			uint stringLength,
			IntPtr input);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTVariantClear(IntPtr input);

		// client
		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCInitialize(Softing.OPCToolbox.OTB.OTCInitData initData);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCProcessCommandLine([MarshalAs(UnmanagedType.LPWStr)] string commandLine);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCTerminate();

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAdvise(
			[MarshalAs(UnmanagedType.LPStruct)] Softing.OPCToolbox.OTB.OTCCallbackFunctions callbacks);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAddDASession(
			string url,
			uint sessionUserData,
			ref uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAddDASubscription(
			uint sessionHandle,
			uint updateRate,
			uint groupUserData,
			ref uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAddDAItem(
			uint groupHandle, string itemID,
			uint itemUserData,
			ref uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRemoveDASession(uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRemoveDASubscription(uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRemoveDAItem(uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCGetAttributes(
			uint objectHandle,
			uint whatAttributes,
			[In, Out] Softing.OPCToolbox.OTB.OTCObjectAttributes pObjectAttributes);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCSetAttributes(
			uint objectHandle,
			uint whatAttributes,
			[In, Out] Softing.OPCToolbox.OTB.OTCObjectAttributes pObjectAttributes);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCUpdateAttributes(
			uint objectHandle,
			byte fromServer,
			uint attributeCount,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] uint[] pWhatAttributes,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] [In, Out] int[] pResults,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCUpdateDAItemAttributes(
			uint subscriptionHandle,
			int itemCount,
			uint[] itemHandles,
			byte fromServer,
			uint attributeCount,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 6)] uint[] whatAttributes,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 6)] [In, Out] int[] results,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCPerformStateTransition(
			uint objectHandle,
			byte deep,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCChangeTargetState(
			uint objectHandle,
			byte state,
			byte deep);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCGetState(
			uint objectHandle,
			out byte currentState,
			out byte targetState);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRead(
			uint objectHandle,
			int count,
			[MarshalAs(UnmanagedType.LPArray)] uint[] itemHandles,
			[MarshalAs(UnmanagedType.LPArray)] string[] itemIDs,
			[MarshalAs(UnmanagedType.LPArray)] string[] itemPaths,
			uint maxAge,
			[MarshalAs(UnmanagedType.LPArray)] [In, Out] Softing.OPCToolbox.OTB.OTValueData[] values,
			[In, Out] int[] results,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCWrite(
			uint objectHandle,
			int count,
			[MarshalAs(UnmanagedType.LPArray)] uint[] itemHandles,
			string[] itemIDs,
			string[] itemPaths,
			[MarshalAs(UnmanagedType.LPArray)] Softing.OPCToolbox.OTB.OTValueData[] values,
			[In, Out] int[] results,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCGetServerStatus(
			uint sessionHandle,
			[In, Out] Softing.OPCToolbox.OTB.OTCServerStatus serverStatus,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCActivateConnectionMonitor(
			uint sessionHandle,
			byte activate,
			uint checkCycle,
			uint connect1Attempts,
			uint connect1Cycle,
			uint connect2Cycle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCValidateDAItems(
			uint subscriptionHandle,
			int itemCount,
			uint[] itemHandles,
			int[] results,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions ExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCBrowseServer(
			string ipAddress,
			byte whatOPCSpec,
			byte whatServerData,
			ref uint serverDataCount,
			out IntPtr ppServerData,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCBrowseAddressSpace(
			uint sessionHandle,
			uint addressSpaceElementHandle,
			string addressSpaceElementID,
			string addressSpaceElementPath,
			ref Softing.OPCToolbox.OTB.OTCAddressSpaceBrowseOptions pBrowseOptions,
			ref uint pAddressSpaceElementDataCount,
			out IntPtr ppAddressSpaceElement,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCReleaseAddressSpaceElement(
			uint addressSpaceElementHandle,
			int deep);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCGetDAProperties(
			uint aSessionHandle,
			uint anAddressSpaceElementHandle,
			string addressSpaceElementID,
			string addressSpaceElementPath,
			Softing.OPCToolbox.OTB.OTCGetDAPropertiesOptions pGetPropertiesOptions,
			out uint pPropertiesDataCount,
			out IntPtr ppProperty,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAddAESession(
			string url,
			uint sessionUserData,
			ref uint pObjectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAddAESubscription(
			uint sessionHandle,
			uint subscriptionUserData,
			ref uint pSubscriptionHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRefreshDAGroup(
			uint subscriptionHandle,
			uint maxAge,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions executionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRemoveAESession(uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRemoveAESubscription(uint objectHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCRefreshAEConditions(
			uint subscriptionHandle,
			byte cancelRefresh,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCAcknowledgeAEConditions(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string ackID,
			[MarshalAs(UnmanagedType.LPWStr)] string ackComment,
			uint count,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] Softing.OPCToolbox.OTB.OTCEventData[] pEvents,
			[In, Out] int[] results,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCQueryAvailableAEFilters(
			uint sessionHandle,
			out uint availableFilters,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCQueryAECategories(
			uint sessionHandle,
			out uint categoryCount,
			out IntPtr ppEventTypes,
			out IntPtr ppCategoryIds,
			out IntPtr ppCategoryDescriptions,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCQueryAEAttributes(
			uint sessionHandle,
			uint categoryId,
			out uint pCount,
			out IntPtr ppAttributeIds,
			out IntPtr ppAttributeDescriptions,
			out IntPtr ppAttributeDatatypes,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCQueryAEConditions(
			uint sessionHandle,
			uint categoryId,
			out uint pCount,
			out IntPtr ppConditionsNames,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCQueryAESubConditions(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string conditionName,
			out uint pCount,
			out IntPtr ppSubConditionsNames,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCQueryAESourceConditions(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string sourcePath,
			out uint pCount,
			out IntPtr ppConditionsNames,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCGetAEConditionState(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string sourcePath,
			[MarshalAs(UnmanagedType.LPWStr)] string conditionName,
			uint attributeCount,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] uint[] pAttributeIds,
			[In, Out] Softing.OPCToolbox.OTB.OTCAEConditionStateData pConditionState,
			//out IntPtr pConditionState,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCEnableAEConditions(
			uint sessionHandle,
			byte enable,
			byte areaOrSource,
			[MarshalAs(UnmanagedType.LPWStr)] string path,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCGetErrorString(
			uint sessionHandle,
			int errorCode,
			out IntPtr errorString,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCLogon(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string userName,
			[MarshalAs(UnmanagedType.LPWStr)] string password,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTCLogoff(
			uint sessionHandle,
			ref Softing.OPCToolbox.OTB.OTCExecutionOptions pExecutionOptions);

		// server
		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSInitialize(Softing.OPCToolbox.OTB.OTSInitData pInitData);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSTerminate();

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSProcessCommandLine([MarshalAs(UnmanagedType.LPWStr)] string commandLine);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSSetServerState(uint daServerState, uint aeServerState);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAdvise(Softing.OPCToolbox.OTB.OTSCallbackFunctions callbacks);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSStart();

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSReady();

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSStop([MarshalAs(UnmanagedType.LPWStr)]string reason);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAddAddressSpaceElement(
			uint hParent,
			Softing.OPCToolbox.OTB.OTSAddressSpaceElementData elementData,
			out uint hObject);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSChangeUserdata(uint hObject, int iUserData);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSRemoveAddressSpaceElement(uint hObject);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSCompleteRequests(
			int count,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] Softing.OPCToolbox.OTB.OTSRequestData[] aRequests,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] int[] aResult,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] Softing.OPCToolbox.OTB.OTValueData[] aValues);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAckRequestElement(
			uint hRequest);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSNackRequestElement(
			uint hRequest);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSGetCacheValue(
			uint hObject,
			ref Softing.OPCToolbox.OTB.OTValueData val);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSValuesChanged(
			int count,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] uint[] aObjects,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] Softing.OPCToolbox.OTB.OTValueData[] aValues);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSGetParent(
			uint hObject,
			IntPtr pParent);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSGetAddressSpaceElementData(
			uint hObject,
			out IntPtr pData);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSInitAddressSpace(byte anAddressSpaceType);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSGetChildren(
			uint hObject,
			byte elementType,
			out int pCount,
			out IntPtr ppChildren);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSSetEUInfo(
			uint hObject,
			bool isEnumerated,
			int count,
			[MarshalAs(UnmanagedType.LPArray)] string[] itemIDs,
			double lowEU,
			double highEU);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAddEventCategory(
			uint aCategoryID,
			[MarshalAs(UnmanagedType.LPWStr)] string aDescription,
			uint eventType,
			out uint pCatHandle);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAddEventAttribute(
			uint hCategory,
			uint anAttributeID,
			[MarshalAs(UnmanagedType.LPWStr)] string aDescription,
			ushort aDataType);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSFireEvents(
			uint anEventCount,
			IntPtr pEventData);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAddCondition(
			uint hCategory,
			[MarshalAs(UnmanagedType.LPWStr)] string aConditionName);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSAddSubCondition(
			uint hCategory,
			[MarshalAs(UnmanagedType.LPWStr)] string aConditionName,
			[MarshalAs(UnmanagedType.LPWStr)] string aSubConditionName);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSConditionsChanged(
			uint aCount,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] uint[] pObjects,
			IntPtr pConditionData,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] [In, Out] uint[] pObjectsOut);

		[DllImport(OTB_DLL, CharSet = CharSet.Unicode)]
		public static extern int OTSGetConditionData(
			IntPtr hObject,
			[MarshalAs(UnmanagedType.LPStruct)] out Softing.OPCToolbox.OTB.OTSConditionData pData);

		public static IntPtr AllocateOTBString(string aSource)
		{
			if (aSource == null || aSource == string.Empty)
			{
				return IntPtr.Zero;
			} //	end if

			IntPtr result = OTBFunctions.OTAllocateMemory((aSource.Length + 1)*Marshal.SizeOf(typeof (short)));
			if (result == IntPtr.Zero)
			{
				return IntPtr.Zero;
			} //	end if

			for (int index = 0; index < aSource.Length; index++)
			{
				Marshal.WriteInt16(result, index*Marshal.SizeOf(typeof (short)), (short) aSource[index]);
			} //	end for

			return result;
		} //	end AllocateOTBString

	}

	#endregion // OTB functions
} //	end namespace Softing.OPCToolbox.OTB_x64

namespace Softing.OPCToolbox.OTB
{

	#region OTB functions
	internal class OTBFunctions
	{
		// common
		public static int OTActivate(
			byte feature,
			String key,
			byte forceDemo)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTActivate(feature, key, forceDemo);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTActivate(feature, key, forceDemo);
			}
		}

		public static int OTTrace(
			byte traceLevel,
			uint traceMask,
			[MarshalAs(UnmanagedType.LPWStr)] string ObjID,
			[MarshalAs(UnmanagedType.LPWStr)] string Message)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTTrace(traceLevel, traceMask, ObjID, Message);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTTrace(traceLevel, traceMask, ObjID, Message);
			}
		}

		public static int OTEnableTracing(
			uint aTraceDataMask,
			Softing.OPCToolbox.OTB.OTCTraceData aTraceData)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTEnableTracing(aTraceDataMask, aTraceData);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTEnableTracing(aTraceDataMask, aTraceData);
			}
		}

		public static int OTFreeMemory(IntPtr Handle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTFreeMemory(Handle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTFreeMemory(Handle);
			}
		}

		public static IntPtr OTAllocateMemory(int aSize)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTAllocateMemory(aSize);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTAllocateMemory(aSize);
			}
		}

		public static int OTVariantClear(IntPtr pValue)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTVariantClear(pValue);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTVariantClear(pValue);
			}
		}

		public static IntPtr OTVariantToString(
			IntPtr pValue, //OTVariant*
			uint stringLength,
			IntPtr input)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTVariantToString(pValue, stringLength, input);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTVariantToString(pValue, stringLength, input);
			}
		} //	client

		public static int OTCInitialize(OTCInitData initData)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCInitialize(initData);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCInitialize(initData);
			}
		}

		public static int OTCProcessCommandLine([MarshalAs(UnmanagedType.LPWStr)] string commandLine)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCProcessCommandLine(commandLine);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCProcessCommandLine(commandLine);
			}
		}

		public static int OTCTerminate()
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCTerminate();
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCTerminate();
			}
		}

		public static int OTCAdvise(
			[MarshalAs(UnmanagedType.LPStruct)] OTCCallbackFunctions callbacks)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCAdvise(callbacks);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCAdvise(callbacks);
			}
		}

		public static int OTCAddDASession(
			string url,
			uint sessionUserData,
			ref uint objectHandle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCAddDASession(url, sessionUserData, ref objectHandle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCAddDASession(url, sessionUserData, ref objectHandle);
			}
		}

		public static int OTCAddDASubscription(
			uint sessionHandle,
			uint updateRate,
			uint groupUserData,
			ref uint objectHandle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCAddDASubscription(sessionHandle, updateRate, groupUserData, ref objectHandle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCAddDASubscription(sessionHandle, updateRate, groupUserData, ref objectHandle);
			}
		}

		public static int OTCAddDAItem(
			uint groupHandle,
			string itemID,
			uint itemUserData,
			ref uint objectHandle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCAddDAItem(groupHandle, itemID, itemUserData, ref objectHandle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCAddDAItem(groupHandle, itemID, itemUserData, ref objectHandle);
			}
		}

		public static int OTCRemoveDASession(uint objectHandle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCRemoveDASession(objectHandle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCRemoveDASession(objectHandle);
			}
		}

		public static int OTCRemoveDASubscription(uint objectHandle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCRemoveDASubscription(objectHandle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCRemoveDASubscription(objectHandle);
			}
		}

		public static int OTCRemoveDAItem(uint objectHandle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCRemoveDAItem(objectHandle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCRemoveDAItem(objectHandle);
			}
		}

		public static int OTCGetAttributes(
			uint objectHandle,
			uint whatAttributes,
			[In, Out] OTCObjectAttributes pObjectAttributes)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCGetAttributes(objectHandle, whatAttributes, pObjectAttributes);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCGetAttributes(objectHandle, whatAttributes, pObjectAttributes);
			}
		}

		public static int OTCSetAttributes(
			uint objectHandle,
			uint whatAttributes,
			[In, Out] OTCObjectAttributes pObjectAttributes)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCSetAttributes(objectHandle, whatAttributes, pObjectAttributes);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCSetAttributes(objectHandle, whatAttributes, pObjectAttributes);
			}
		}

		public static int OTCUpdateAttributes(
			uint objectHandle,
			byte fromServer,
			uint attributeCount,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] uint[] pWhatAttributes,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] [In, Out] int[] pResults,
			ref OTCExecutionOptions executionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCUpdateAttributes(objectHandle, fromServer, attributeCount, pWhatAttributes, pResults,
				                                                ref executionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCUpdateAttributes(objectHandle, fromServer, attributeCount, pWhatAttributes, pResults,
				                                                ref executionOptions);
			}
		}

		public static int OTCUpdateDAItemAttributes(
			uint subscriptionHandle,
			int itemCount,
			uint[] itemHandles,
			byte fromServer,
			uint attributeCount,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 6)] uint[] whatAttributes,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 6)] [In, Out] int[] results,
			ref OTCExecutionOptions executionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCUpdateDAItemAttributes(subscriptionHandle, itemCount, itemHandles, fromServer,
				                                                      attributeCount, whatAttributes, results, ref executionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCUpdateDAItemAttributes(subscriptionHandle, itemCount, itemHandles, fromServer,
				                                                      attributeCount, whatAttributes, results, ref executionOptions);
			}
		}

		public static int OTCPerformStateTransition(
			uint objectHandle,
			byte deep,
			ref OTCExecutionOptions executionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCPerformStateTransition(objectHandle, deep, ref executionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCPerformStateTransition(objectHandle, deep, ref executionOptions);
			}
		}

		public static int OTCChangeTargetState(
			uint objectHandle,
			byte state,
			byte deep)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCChangeTargetState(objectHandle, state, deep);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCChangeTargetState(objectHandle, state, deep);
			}
		}

		public static int OTCGetState(
			uint objectHandle,
			out byte currentState,
			out byte targetState)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCGetState(objectHandle, out currentState, out targetState);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCGetState(objectHandle, out currentState, out targetState);
			}
		}

		public static int OTCRead(
			uint objectHandle,
			int count,
			[MarshalAs(UnmanagedType.LPArray)] uint[] itemHandles,
			[MarshalAs(UnmanagedType.LPArray)] string[] itemIDs,
			[MarshalAs(UnmanagedType.LPArray)] string[] itemPaths,
			uint maxAge,
			[MarshalAs(UnmanagedType.LPArray)] [In, Out] OTValueData[] values,
			[In, Out] int[] results,
			ref OTCExecutionOptions executionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCRead(objectHandle, count, itemHandles, itemIDs, itemPaths, maxAge, values, results,
				                                    ref executionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCRead(objectHandle, count, itemHandles, itemIDs, itemPaths, maxAge, values, results,
				                                    ref executionOptions);
			}
		}

		public static int OTCWrite(
			uint objectHandle,
			int count,
			[MarshalAs(UnmanagedType.LPArray)] uint[] itemHandles,
			string[] itemIDs,
			string[] itemPaths,
			[MarshalAs(UnmanagedType.LPArray)] OTValueData[] values,
			[In, Out] int[] results,
			ref OTCExecutionOptions executionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCWrite(objectHandle, count, itemHandles, itemIDs, itemPaths, values, results,
				                                     ref executionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCWrite(objectHandle, count, itemHandles, itemIDs, itemPaths, values, results,
				                                     ref executionOptions);
			}
		}

		public static int OTCGetServerStatus(
			uint sessionHandle,
			[In, Out] OTCServerStatus serverStatus,
			ref OTCExecutionOptions executionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCGetServerStatus(sessionHandle, serverStatus, ref executionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCGetServerStatus(sessionHandle, serverStatus, ref executionOptions);
			}
		}

		public static int OTCActivateConnectionMonitor(
			uint sessionHandle,
			byte activate,
			uint checkCycle,
			uint connect1Attempts,
			uint connect1Cycle,
			uint connect2Cycle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCActivateConnectionMonitor(sessionHandle, activate, checkCycle, connect1Attempts,
				                                                         connect1Cycle, connect2Cycle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCActivateConnectionMonitor(sessionHandle, activate, checkCycle, connect1Attempts,
				                                                         connect1Cycle, connect2Cycle);
			}
		}

		public static int OTCValidateDAItems(
			uint subscriptionHandle,
			int itemCount,
			uint[] itemHandles,
			int[] results,
			ref OTCExecutionOptions ExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCValidateDAItems(subscriptionHandle, itemCount, itemHandles, results,
				                                               ref ExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCValidateDAItems(subscriptionHandle, itemCount, itemHandles, results,
				                                               ref ExecutionOptions);
			}
		}

		public static int OTCBrowseServer(
			string ipAddress,
			byte whatOPCSpec,
			byte whatServerData,
			ref uint serverDataCount,
			out IntPtr ppServerData,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCBrowseServer(ipAddress, whatOPCSpec, whatServerData, ref serverDataCount,
				                                            out ppServerData, ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCBrowseServer(ipAddress, whatOPCSpec, whatServerData, ref serverDataCount,
				                                            out ppServerData, ref pExecutionOptions);
			}
		}

		public static int OTCBrowseAddressSpace(
			uint sessionHandle,
			uint addressSpaceElementHandle,
			string addressSpaceElementID,
			string addressSpaceElementPath,
			ref OTCAddressSpaceBrowseOptions pBrowseOptions,
			ref uint pAddressSpaceElementDataCount,
			out IntPtr ppAddressSpaceElement,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCBrowseAddressSpace(sessionHandle, addressSpaceElementHandle, addressSpaceElementID,
				                                                  addressSpaceElementPath, ref pBrowseOptions,
				                                                  ref pAddressSpaceElementDataCount, out ppAddressSpaceElement,
				                                                  ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCBrowseAddressSpace(sessionHandle, addressSpaceElementHandle, addressSpaceElementID,
				                                                  addressSpaceElementPath, ref pBrowseOptions,
				                                                  ref pAddressSpaceElementDataCount, out ppAddressSpaceElement,
				                                                  ref pExecutionOptions);
			}
		}

		public static int OTCReleaseAddressSpaceElement(
			uint addressSpaceElementHandle,
			int deep)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCReleaseAddressSpaceElement(addressSpaceElementHandle, deep);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCReleaseAddressSpaceElement(addressSpaceElementHandle, deep);
			}
		}

		public static int OTCGetDAProperties(
			uint aSessionHandle,
			uint anAddressSpaceElementHandle,
			string addressSpaceElementID,
			string addressSpaceElementPath,
			OTCGetDAPropertiesOptions pGetPropertiesOptions,
			out uint pPropertiesDataCount,
			out IntPtr ppProperty,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCGetDAProperties(aSessionHandle, anAddressSpaceElementHandle, addressSpaceElementID,
				                                               addressSpaceElementPath, pGetPropertiesOptions,
				                                               out pPropertiesDataCount, out ppProperty, ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCGetDAProperties(aSessionHandle, anAddressSpaceElementHandle, addressSpaceElementID,
				                                               addressSpaceElementPath, pGetPropertiesOptions,
				                                               out pPropertiesDataCount, out ppProperty, ref pExecutionOptions);
			}
		}

		public static int OTCAddAESession(
			string url,
			uint sessionUserData,
			ref uint pObjectHandle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCAddAESession(url, sessionUserData, ref pObjectHandle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCAddAESession(url, sessionUserData, ref pObjectHandle);
			}
		}

		public static int OTCAddAESubscription(
			uint sessionHandle,
			uint subscriptionUserData,
			ref uint pSubscriptionHandle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCAddAESubscription(sessionHandle, subscriptionUserData, ref pSubscriptionHandle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCAddAESubscription(sessionHandle, subscriptionUserData, ref pSubscriptionHandle);
			}
		}

		public static int OTCRefreshDAGroup(
			uint subscriptionHandle,
			uint maxAge,
			ref OTCExecutionOptions executionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCRefreshDAGroup(subscriptionHandle, maxAge, ref executionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCRefreshDAGroup(subscriptionHandle, maxAge, ref executionOptions);
			}
		}

		public static int OTCRemoveAESession(uint objectHandle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCRemoveAESession(objectHandle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCRemoveAESession(objectHandle);
			}
		}

		public static int OTCRemoveAESubscription(uint objectHandle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCRemoveAESubscription(objectHandle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCRemoveAESubscription(objectHandle);
			}
		}

		public static int OTCRefreshAEConditions(
			uint subscriptionHandle,
			byte cancelRefresh,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCRefreshAEConditions(subscriptionHandle, cancelRefresh, ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCRefreshAEConditions(subscriptionHandle, cancelRefresh, ref pExecutionOptions);
			}
		}

		public static int OTCAcknowledgeAEConditions(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string ackID,
			[MarshalAs(UnmanagedType.LPWStr)] string ackComment,
			uint count,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] OTCEventData[] pEvents,
			[In, Out] int[] results,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCAcknowledgeAEConditions(sessionHandle, ackID, ackComment, count, pEvents, results,
				                                                       ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCAcknowledgeAEConditions(sessionHandle, ackID, ackComment, count, pEvents, results,
				                                                       ref pExecutionOptions);
			}
		}

		public static int OTCQueryAvailableAEFilters(
			uint sessionHandle,
			out uint availableFilters,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCQueryAvailableAEFilters(sessionHandle, out availableFilters, ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCQueryAvailableAEFilters(sessionHandle, out availableFilters, ref pExecutionOptions);
			}
		}

		public static int OTCQueryAECategories(
			uint sessionHandle,
			out uint categoryCount,
			out IntPtr ppEventTypes,
			out IntPtr ppCategoryIds,
			out IntPtr ppCategoryDescriptions,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCQueryAECategories(sessionHandle, out categoryCount, out ppEventTypes,
				                                                 out ppCategoryIds, out ppCategoryDescriptions,
				                                                 ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCQueryAECategories(sessionHandle, out categoryCount, out ppEventTypes,
				                                                 out ppCategoryIds, out ppCategoryDescriptions,
				                                                 ref pExecutionOptions);
			}
		}

		public static int OTCQueryAEAttributes(
			uint sessionHandle,
			uint categoryId,
			out uint pCount,
			out IntPtr ppAttributeIds,
			out IntPtr ppAttributeDescriptions,
			out IntPtr ppAttributeDatatypes,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCQueryAEAttributes(sessionHandle, categoryId, out pCount, out ppAttributeIds,
				                                                 out ppAttributeDescriptions, out ppAttributeDatatypes,
				                                                 ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCQueryAEAttributes(sessionHandle, categoryId, out pCount, out ppAttributeIds,
				                                                 out ppAttributeDescriptions, out ppAttributeDatatypes,
				                                                 ref pExecutionOptions);
			}
		}

		public static int OTCQueryAEConditions(
			uint sessionHandle,
			uint categoryId,
			out uint pCount,
			out IntPtr ppConditionsNames,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCQueryAEConditions(sessionHandle, categoryId, out pCount, out ppConditionsNames,
				                                                 ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCQueryAEConditions(sessionHandle, categoryId, out pCount, out ppConditionsNames,
				                                                 ref pExecutionOptions);
			}
		}

		public static int OTCQueryAESubConditions(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string conditionName,
			out uint pCount,
			out IntPtr ppSubConditionsNames,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCQueryAESubConditions(sessionHandle, conditionName, out pCount,
				                                                    out ppSubConditionsNames, ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCQueryAESubConditions(sessionHandle, conditionName, out pCount,
				                                                    out ppSubConditionsNames, ref pExecutionOptions);
			}
		}

		public static int OTCQueryAESourceConditions(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string sourcePath,
			out uint pCount,
			out IntPtr ppConditionsNames,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCQueryAESourceConditions(sessionHandle, sourcePath, out pCount, out ppConditionsNames,
				                                                       ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCQueryAESourceConditions(sessionHandle, sourcePath, out pCount, out ppConditionsNames,
				                                                       ref pExecutionOptions);
			}
		}

		public static int OTCGetAEConditionState(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string sourcePath,
			[MarshalAs(UnmanagedType.LPWStr)] string conditionName,
			uint attributeCount,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] uint[] pAttributeIds,
			[In, Out] OTCAEConditionStateData pConditionState,
			//out IntPtr pConditionState,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCGetAEConditionState(sessionHandle, sourcePath, conditionName, attributeCount,
				                                                   pAttributeIds, pConditionState, ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCGetAEConditionState(sessionHandle, sourcePath, conditionName, attributeCount,
				                                                   pAttributeIds, pConditionState, ref pExecutionOptions);
			}
		}

		public static int OTCEnableAEConditions(
			uint sessionHandle,
			byte enable,
			byte areaOrSource,
			[MarshalAs(UnmanagedType.LPWStr)] string path,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCEnableAEConditions(sessionHandle, enable, areaOrSource, path, ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCEnableAEConditions(sessionHandle, enable, areaOrSource, path, ref pExecutionOptions);
			}
		}

		public static int OTCGetErrorString(
			uint sessionHandle,
			int errorCode,
			out IntPtr errorString,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCGetErrorString(sessionHandle, errorCode, out errorString, ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCGetErrorString(sessionHandle, errorCode, out errorString, ref pExecutionOptions);
			}
		}

		public static int OTCLogon(
			uint sessionHandle,
			[MarshalAs(UnmanagedType.LPWStr)] string userName,
			[MarshalAs(UnmanagedType.LPWStr)] string password,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCLogon(sessionHandle, userName, password, ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCLogon(sessionHandle, userName, password, ref pExecutionOptions);
			}
		}

		public static int OTCLogoff(
			uint sessionHandle,
			ref OTCExecutionOptions pExecutionOptions)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTCLogoff(sessionHandle, ref pExecutionOptions);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTCLogoff(sessionHandle, ref pExecutionOptions);
			}
		} //	server

		public static int OTSInitialize(OTSInitData pInitData)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSInitialize(pInitData);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSInitialize(pInitData);
			}
		}

		public static int OTSTerminate()
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSTerminate();
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSTerminate();
			}
		}

		public static int OTSProcessCommandLine([MarshalAs(UnmanagedType.LPWStr)] string commandLine)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSProcessCommandLine(commandLine);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSProcessCommandLine(commandLine);
			}
		}

		public static int OTSSetServerState(uint daServerState, uint aeServerState)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSSetServerState(daServerState, aeServerState);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSSetServerState(daServerState, aeServerState);
			}
		}

		public static int OTSAdvise(OTSCallbackFunctions callbacks)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSAdvise(callbacks);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSAdvise(callbacks);
			}
		}

		public static int OTSStart()
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSStart();
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSStart();
			}
		}

		public static int OTSReady()
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSReady();
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSReady();
			}
		}

		public static int OTSStop([MarshalAs(UnmanagedType.LPWStr)]string reason)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSStop(reason);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSStop(reason);
			}
		}

		public static int OTSAddAddressSpaceElement(
			uint hParent,
			OTSAddressSpaceElementData elementData,
			out uint hObject)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSAddAddressSpaceElement(hParent, elementData, out hObject);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSAddAddressSpaceElement(hParent, elementData, out hObject);
			}
		}

		public static int OTSChangeUserdata(uint hObject, int iUserData)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSChangeUserdata(hObject, iUserData);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSChangeUserdata(hObject, iUserData);
			}
		}

		public static int OTSRemoveAddressSpaceElement(uint hObject)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSRemoveAddressSpaceElement(hObject);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSRemoveAddressSpaceElement(hObject);
			}
		}

		public static int OTSCompleteRequests(
			int count,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] OTSRequestData[] aRequests,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] int[] aResult,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] OTValueData[] aValues)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSCompleteRequests(count, aRequests, aResult, aValues);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSCompleteRequests(count, aRequests, aResult, aValues);
			}
		}

		public static int OTSAckRequestElement(
			uint hRequest)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSAckRequestElement(hRequest);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSAckRequestElement(hRequest);
			}
		}

		public static int OTSNackRequestElement(
			uint hRequest)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSNackRequestElement(hRequest);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSNackRequestElement(hRequest);
			}
		}

		public static int OTSGetCacheValue(
			uint hObject,
			ref OTValueData val)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSGetCacheValue(hObject, ref val);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSGetCacheValue(hObject, ref val);
			}
		}

		public static int OTSValuesChanged(
			int count,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] uint[] aObjects,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] OTValueData[] aValues)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSValuesChanged(count, aObjects, aValues);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSValuesChanged(count, aObjects, aValues);
			}
		}

		public static int OTSGetParent(
			uint hObject,
			IntPtr pParent)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSGetParent(hObject, pParent);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSGetParent(hObject, pParent);
			}
		}

		public static int OTSGetAddressSpaceElementData(
			uint hObject,
			out IntPtr pData)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSGetAddressSpaceElementData(hObject, out pData);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSGetAddressSpaceElementData(hObject, out pData);
			}
		}

		public static int OTSInitAddressSpace(byte anAddressSpaceType)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSInitAddressSpace(anAddressSpaceType);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSInitAddressSpace(anAddressSpaceType);
			}
		}

		public static int OTSGetChildren(
			uint hObject,
			byte elementType,
			out int pCount,
			out IntPtr ppChildren)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSGetChildren(hObject, elementType, out pCount, out ppChildren);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSGetChildren(hObject, elementType, out pCount, out ppChildren);
			}
		}

		public static int OTSSetEUInfo(
			uint hObject,
			bool isEnumerated,
			int count,
			[MarshalAs(UnmanagedType.LPArray)] string[] itemIDs,
			double lowEU,
			double highEU)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSSetEUInfo(hObject, isEnumerated, count, itemIDs, lowEU, highEU);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSSetEUInfo(hObject, isEnumerated, count, itemIDs, lowEU, highEU);
			}
		}

		public static int OTSAddEventCategory(
			uint aCategoryID,
			[MarshalAs(UnmanagedType.LPWStr)] string aDescription,
			uint eventType,
			out uint pCatHandle)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSAddEventCategory(aCategoryID, aDescription, eventType, out pCatHandle);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSAddEventCategory(aCategoryID, aDescription, eventType, out pCatHandle);
			}
		}

		public static int OTSAddEventAttribute(
			uint hCategory,
			uint anAttributeID,
			[MarshalAs(UnmanagedType.LPWStr)] string aDescription,
			ushort aDataType)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSAddEventAttribute(hCategory, anAttributeID, aDescription, aDataType);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSAddEventAttribute(hCategory, anAttributeID, aDescription, aDataType);
			}
		}

		public static int OTSFireEvents(
			uint anEventCount,
			IntPtr pEventData)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSFireEvents(anEventCount, pEventData);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSFireEvents(anEventCount, pEventData);
			}
		}

		public static int OTSAddCondition(
			uint hCategory,
			[MarshalAs(UnmanagedType.LPWStr)] string aConditionName)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSAddCondition(hCategory, aConditionName);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSAddCondition(hCategory, aConditionName);
			}
		}

		public static int OTSAddSubCondition(
			uint hCategory,
			[MarshalAs(UnmanagedType.LPWStr)] string aConditionName,
			[MarshalAs(UnmanagedType.LPWStr)] string aSubConditionName)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSAddSubCondition(hCategory, aConditionName, aSubConditionName);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSAddSubCondition(hCategory, aConditionName, aSubConditionName);
			}
		}

		public static int OTSConditionsChanged(
			uint aCount,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] uint[] pObjects,
			IntPtr pConditionData,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] [In, Out] uint[] pObjectsOut)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSConditionsChanged(aCount, pObjects, pConditionData, pObjectsOut);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSConditionsChanged(aCount, pObjects, pConditionData, pObjectsOut);
			}
		}

		public static int OTSGetConditionData(
			IntPtr hObject,
			[MarshalAs(UnmanagedType.LPStruct)] out OTSConditionData pData)
		{
			if (IntPtr.Size == 8)
			{
				return OTB_x64.OTBFunctions.OTSGetConditionData(hObject, out pData);
			}
			else
			{
				return OTB_x86.OTBFunctions.OTSGetConditionData(hObject, out pData);
			}
		}

		public static IntPtr AllocateOTBString(string aSource)
		{
			if (aSource == null || aSource == string.Empty)
			{
				return IntPtr.Zero;
			} //	end if

			IntPtr result = OTBFunctions.OTAllocateMemory((aSource.Length + 1)*Marshal.SizeOf(typeof (short)));
			if (result == IntPtr.Zero)
			{
				return IntPtr.Zero;
			} //	end if

			for (int index = 0; index < aSource.Length; index++)
			{
				Marshal.WriteInt16(result, index*Marshal.SizeOf(typeof (short)), (short) aSource[index]);
			} //	end for

			return result;
		} //	end AllocateOTBString


		public static IntPtr GetIntPtrOffset(IntPtr source, Int64 offset)
		{
			if (IntPtr.Size == 8)
			{
				return (IntPtr) (source.ToInt64() + offset);
			}
			else
			{
				return (IntPtr) (source.ToInt32() + offset);
			}
		} //	end GetIntPtrOffset

	}

	#endregion // OTB functions

	#region OTB callbacks
	//common

	internal delegate void OTOnTrace(
		[MarshalAs(UnmanagedType.LPWStr)] string traceString,
		ushort level,
		uint mask,
		[MarshalAs(UnmanagedType.LPWStr)] string objId,
		[MarshalAs(UnmanagedType.LPWStr)] string text);

	internal delegate void OTCOnStateChange(
		Softing.OPCToolbox.OTB.OTCObjectContext objectData,
		byte state);

	internal delegate void OTCOnDataChange(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTCObjectContext objectContext,
		uint count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] Softing.OPCToolbox.OTB.OTObjectData[] itemData,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] Softing.OPCToolbox.OTB.OTValueData[] values,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] int[] results);

	internal delegate void OTCOnReadComplete(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTCObjectContext objectContext,
		int result,
		uint count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] Softing.OPCToolbox.OTB.OTObjectData[] itemData,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3, ArraySubType = UnmanagedType.LPWStr)] String[] itemIDs,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3, ArraySubType = UnmanagedType.LPWStr)] String[] itemPaths,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] Softing.OPCToolbox.OTB.OTValueData[] values,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] int[] results);

	internal delegate void OTCOnWriteComplete(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTCObjectContext objectContext,
		int result,
		uint count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] Softing.OPCToolbox.OTB.OTObjectData[] itemData,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3, ArraySubType = UnmanagedType.LPWStr)] String[] itemIDs,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3, ArraySubType = UnmanagedType.LPWStr)] String[] itemPaths,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] Softing.OPCToolbox.OTB.OTValueData[] values,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] int[] results);

	internal delegate void OTCOnPerfromStateTransition(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTCObjectContext objectContext,
		int result);

	internal delegate bool OTCOnServerShutdown(
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		[MarshalAs(UnmanagedType.LPWStr)] string reason);

	internal delegate void OTCOnUpdateAttributes(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTCObjectContext objectContext,
		int result,
		byte fromServer,
		uint attributesCount,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4)] uint[] whatAttributes,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4)] int[] results);

	internal delegate void OTCOnUpdateDAItemAttributes(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTCObjectContext objectContext,
		int result,
		uint itemCount,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] Softing.OPCToolbox.OTB.OTObjectData[] itemData,
		byte fromServer,
		uint attributesCount,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 6)] uint[] whatAttributes,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] int[] results);

	internal delegate void OTCOnValidateDAItems(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTCObjectContext objectContext,
		int result,
		uint itemCount,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] Softing.OPCToolbox.OTB.OTObjectData[] objectDataArray,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] int[] results);

	internal delegate void OTCOnGetServerStatus(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		[In, Out] Softing.OPCToolbox.OTB.OTCServerStatus serverStatus);

	internal delegate void OTCOnBrowseServer(
		uint executionContext,
		int result,
		[MarshalAs(UnmanagedType.LPWStr)] string ipAddress,
		byte whatOPCSpec,
		byte whatServerData,
		uint serverDataCount,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 5)] Softing.OPCToolbox.OTB.OTCServerData[] serverData);

	internal delegate void OTCOnBrowseAddressSpace(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		uint addressSpaceElementHandle,
		[MarshalAs(UnmanagedType.LPWStr)] String addressSpaceElementID,
		[MarshalAs(UnmanagedType.LPWStr)] String addressSpaceElementPath,
		ref Softing.OPCToolbox.OTB.OTCAddressSpaceBrowseOptions browseOptions,
		uint addressSpaceElementDataCount,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 7)] Softing.OPCToolbox.OTB.OTCAddressSpaceElementData[]
			addressSpaceElement);

	internal delegate void OTCOnGetDAProperties(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		uint addressSpaceElementHandle,
		[MarshalAs(UnmanagedType.LPWStr)] String addressSpaceElementID,
		[MarshalAs(UnmanagedType.LPWStr)] String addressSpaceElementPath,
		Softing.OPCToolbox.OTB.OTCGetDAPropertiesOptions getPropertiesOptions,
		uint propertyDataCount,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 7)] Softing.OPCToolbox.OTB.OTCDAPropertyData[] propertyData);

	internal delegate void OTCOnReceivedEvents(
		Softing.OPCToolbox.OTB.OTCObjectContext objectContext,
		uint count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Softing.OPCToolbox.OTB.OTCEventData[] pEvents,
		byte refresh,
		byte lastRefresh);

	internal delegate void OTCOnRefreshAEConditions(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTCObjectContext objectContext,
		int result,
		byte cancelRefresh);

	internal delegate void OTCOnAcknowledgeAEConditions(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		[MarshalAs(UnmanagedType.LPWStr)] string ackID,
		[MarshalAs(UnmanagedType.LPWStr)] string ackComment,
		uint count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 5)] Softing.OPCToolbox.OTB.OTCEventData[] pEvents,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 5)] int[] pResults);

	internal delegate void OTCOnQueryAvailableAEFilters(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		uint availableFilters);

	internal delegate void OTCOnQueryAECategories(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		uint count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] uint[] pEventTypes,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] uint[] pCategoryIds,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3, ArraySubType = UnmanagedType.LPWStr)] string[]
			pCategoryDescriptions);

	internal delegate void OTCOnQueryAEAttributes(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		uint categoryId,
		uint count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4)] uint[] pAttributeIds,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4, ArraySubType = UnmanagedType.LPWStr)] string[]
			pAttributeDescriptions,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4)] ushort[] pAttributeDatatypes);

	internal delegate void OTCOnQueryAEConditions(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		uint categoryId,
		uint count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4, ArraySubType = UnmanagedType.LPWStr)] string[] pConditionsNames);

	internal delegate void OTCOnQueryAESubConditions(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		[MarshalAs(UnmanagedType.LPWStr)] string conditionName,
		uint count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4, ArraySubType = UnmanagedType.LPWStr)] string[]
			pSubConditionsNames);

	internal delegate void OTCOnQueryAESourceConditions(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		[MarshalAs(UnmanagedType.LPWStr)] string sourcePath,
		uint count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4, ArraySubType = UnmanagedType.LPWStr)] string[] pConditionsNames);

	internal delegate void OTCOnGetAEConditionState(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		[MarshalAs(UnmanagedType.LPWStr)] string sourcePath,
		[MarshalAs(UnmanagedType.LPWStr)] string conditionName,
		uint attributeCount,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 5)] uint[] pAttributeIds,
		Softing.OPCToolbox.OTB.OTCAEConditionStateData pConditionState);

	internal delegate void OTCOnEnableAEConditions(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		byte enable,
		byte areaOrSource,
		[MarshalAs(UnmanagedType.LPWStr)] string path);

	internal delegate void OTCOnGetErrorString(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		int errorCode,
		[MarshalAs(UnmanagedType.LPWStr)] string errorString);

	internal delegate void OTCOnLogon(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result,
		[MarshalAs(UnmanagedType.LPWStr)] string userName,
		[MarshalAs(UnmanagedType.LPWStr)] string password);

	internal delegate void OTCOnLogoff(
		uint executionContext,
		Softing.OPCToolbox.OTB.OTObjectData sessionData,
		int result);

	//server
	internal delegate int OTSShutdown();

	internal delegate int OTSHandleReadRequests(
		int count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] Softing.OPCToolbox.OTB.OTSRequestData[] aRequests);

	internal delegate int OTSHandleWriteRequests(
		int count,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] Softing.OPCToolbox.OTB.OTSRequestData[] aRequests,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] Softing.OPCToolbox.OTB.OTValueData[] aValues);

	internal delegate int OTSRequestCompletedInternally(
		uint pRequestData);

	internal delegate int OTSQueryProperties(
		IntPtr pObjectData,
		[MarshalAs(UnmanagedType.LPWStr)] string anItemId,
		int aPropertyId,
		out uint aPropertiesCount,
		out IntPtr pProperties);

	internal delegate int OTSQueryAddressSpaceElementData(
		[MarshalAs(UnmanagedType.LPWStr)] string path,
		byte elementType,
		IntPtr pData);

	internal delegate int OTSQueryAddressSpaceElementDataEx(
		[MarshalAs(UnmanagedType.LPWStr)] string path,
		byte elementType,
		ref ushort elementDataFilter,
		IntPtr pData);

	internal delegate int OTSQueryAddressSpaceElementChildren(
		[MarshalAs(UnmanagedType.LPWStr)] string path,
		byte elementType,
		out uint pCount,
		out IntPtr pElementData);

	internal delegate int OTSQueryAddressSpaceElementProperties(
		int aPropertyID,
		IntPtr aPropertyList);

	internal delegate int OTSChangeSessionState(
		uint aSessionHandle,
		Softing.OPCToolbox.OTB.OTSSessionData pSessionStateData);

	internal delegate int OTSQueryCacheValue(
		uint aSessionHandle,
		Softing.OPCToolbox.OTB.OTObjectData anObjectData,
		IntPtr pOutValue);

	internal delegate int OTSChangeItems(
		uint itemCnt,
		IntPtr pItemData);

	internal delegate int OTSCreateAddressSpaceElement(
		uint hParent,
		uint hObject,
		Softing.OPCToolbox.OTB.OTSAddressSpaceElementData pDataIn,
		IntPtr pDataOut);

	internal delegate int OTSCreateAddressSpaceElementEx(
		uint hParent,
		uint hObject,
		Softing.OPCToolbox.OTB.OTSAddressSpaceElementData pDataIn,
		ushort elementDataFilter,
		IntPtr pDataOut);

	internal delegate int OTSDestroyAddressSpaceElement(Softing.OPCToolbox.OTB.OTObjectData objectData);

	internal delegate int OTSQueryConditions(
		IntPtr pObjectData,
		[MarshalAs(UnmanagedType.LPWStr)] string aSourcePath,
		out uint aConditionCount,
		out IntPtr pConditionNames);

	internal delegate int OTSAcknowledgeCondition(
		Softing.OPCToolbox.OTB.OTObjectData pConditionData,
		[MarshalAs(UnmanagedType.LPWStr)] string anAckId,
		[MarshalAs(UnmanagedType.LPWStr)] string anAckComment);

	internal delegate int OTSQueryConditionDefinition(
		Softing.OPCToolbox.OTB.OTObjectData pConditionData,
		IntPtr aConditionDefinitionData);

	internal delegate int OTSEnableConditions(
		byte isEnable,
		[MarshalAs(UnmanagedType.LPWStr)] string anAddressSpaceElementPath);

	internal delegate int OTSWebHandleTemplate(
		[MarshalAs(UnmanagedType.LPWStr)] string aTemplateName,
		uint numArgs,
		[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1, ArraySubType = UnmanagedType.LPWStr)] string[] pArgs,
		out IntPtr pResult);

	#endregion // OTB callbacks
} //	end namespace Softing.OPCToolbox.OTB

#endif // !NET35 && !NET40 && !NET45 && !NET46
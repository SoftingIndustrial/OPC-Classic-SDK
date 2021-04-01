

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 6.00.0361 */
/* at Wed Nov 15 15:10:37 2006
 */
/* Compiler settings for SOProxy.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data
    VC __declspec() decoration level:
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __SOProxy_h__
#define __SOProxy_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */

#ifndef __ISoftingOPCWatch_FWD_DEFINED__
#define __ISoftingOPCWatch_FWD_DEFINED__
typedef interface ISoftingOPCWatch ISoftingOPCWatch;
#endif  /* __ISoftingOPCWatch_FWD_DEFINED__ */


#ifndef __ISoftingOPCTrace_FWD_DEFINED__
#define __ISoftingOPCTrace_FWD_DEFINED__
typedef interface ISoftingOPCTrace ISoftingOPCTrace;
#endif  /* __ISoftingOPCTrace_FWD_DEFINED__ */


#ifndef __ISoftingOPCWatch_FWD_DEFINED__
#define __ISoftingOPCWatch_FWD_DEFINED__
typedef interface ISoftingOPCWatch ISoftingOPCWatch;
#endif  /* __ISoftingOPCWatch_FWD_DEFINED__ */


#ifndef __ISoftingOPCTrace_FWD_DEFINED__
#define __ISoftingOPCTrace_FWD_DEFINED__
typedef interface ISoftingOPCTrace ISoftingOPCTrace;
#endif  /* __ISoftingOPCTrace_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C" {
#endif

	void* __RPC_USER MIDL_user_allocate(size_t);
	void __RPC_USER MIDL_user_free(void*);

#ifndef __ISoftingOPCWatch_INTERFACE_DEFINED__
#define __ISoftingOPCWatch_INTERFACE_DEFINED__

	/* interface ISoftingOPCWatch */
	/* [unique][uuid][object] */


	EXTERN_C const IID IID_ISoftingOPCWatch;

#if defined(__cplusplus) && !defined(CINTERFACE)

	MIDL_INTERFACE("8C727A71-AA9D-4415-B49F-87EFD8504DBC")
ISoftingOPCWatch :
	public IUnknown
	{
public:
		virtual HRESULT STDMETHODCALLTYPE GetChildren(
			/* [string][in] */ LPCWSTR parent,
			/* [in] */ BOOL withAttributes,
			/* [string][out] */ LPWSTR* children) = 0;

		virtual HRESULT STDMETHODCALLTYPE CollectChanges(
			/* [string][in] */ LPCWSTR objects,
			/* [in] */ DWORD doCollect) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetChanges(
			/* [string][out] */ LPWSTR* changes) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetStatistics(
			/* [string][out] */ LPWSTR* statistics) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetObject(
			/* [string][in] */ LPCWSTR objectDescription,
			/* [string][out] */ LPWSTR* obj) = 0;

	};

#else   /* C style interface */

	typedef struct ISoftingOPCWatchVtbl
	{
		BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			ISoftingOPCWatch* This,
			/* [in] */ REFIID riid,
			/* [iid_is][out] */ void** ppvObject);

		ULONG(STDMETHODCALLTYPE* AddRef)(
			ISoftingOPCWatch* This);

		ULONG(STDMETHODCALLTYPE* Release)(
			ISoftingOPCWatch* This);

		HRESULT(STDMETHODCALLTYPE* GetChildren)(
			ISoftingOPCWatch* This,
			/* [string][in] */ LPCWSTR parent,
			/* [in] */ BOOL withAttributes,
			/* [string][out] */ LPWSTR* children);

		HRESULT(STDMETHODCALLTYPE* CollectChanges)(
			ISoftingOPCWatch* This,
			/* [string][in] */ LPCWSTR objects,
			/* [in] */ DWORD doCollect);

		HRESULT(STDMETHODCALLTYPE* GetChanges)(
			ISoftingOPCWatch* This,
			/* [string][out] */ LPWSTR* changes);

		HRESULT(STDMETHODCALLTYPE* GetStatistics)(
			ISoftingOPCWatch* This,
			/* [string][out] */ LPWSTR* statistics);

		HRESULT(STDMETHODCALLTYPE* GetObject)(
			ISoftingOPCWatch* This,
			/* [string][in] */ LPCWSTR objectDescription,
			/* [string][out] */ LPWSTR* obj);

		END_INTERFACE
	} ISoftingOPCWatchVtbl;

	interface ISoftingOPCWatch
	{
		CONST_VTBL struct ISoftingOPCWatchVtbl* lpVtbl;
	};



#ifdef COBJMACROS


#define ISoftingOPCWatch_QueryInterface(This,riid,ppvObject)    \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISoftingOPCWatch_AddRef(This)   \
    (This)->lpVtbl -> AddRef(This)

#define ISoftingOPCWatch_Release(This)  \
    (This)->lpVtbl -> Release(This)


#define ISoftingOPCWatch_GetChildren(This,parent,withAttributes,children)   \
    (This)->lpVtbl -> GetChildren(This,parent,withAttributes,children)

#define ISoftingOPCWatch_CollectChanges(This,objects,doCollect) \
    (This)->lpVtbl -> CollectChanges(This,objects,doCollect)

#define ISoftingOPCWatch_GetChanges(This,changes)   \
    (This)->lpVtbl -> GetChanges(This,changes)

#define ISoftingOPCWatch_GetStatistics(This,statistics) \
    (This)->lpVtbl -> GetStatistics(This,statistics)

#define ISoftingOPCWatch_GetObject(This,objectDescription,obj)  \
    (This)->lpVtbl -> GetObject(This,objectDescription,obj)

#endif /* COBJMACROS */


#endif  /* C style interface */



	HRESULT STDMETHODCALLTYPE ISoftingOPCWatch_GetChildren_Proxy(
		ISoftingOPCWatch* This,
		/* [string][in] */ LPCWSTR parent,
		/* [in] */ BOOL withAttributes,
		/* [string][out] */ LPWSTR* children);


	void __RPC_STUB ISoftingOPCWatch_GetChildren_Stub(
		IRpcStubBuffer* This,
		IRpcChannelBuffer* _pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD* _pdwStubPhase);


	HRESULT STDMETHODCALLTYPE ISoftingOPCWatch_CollectChanges_Proxy(
		ISoftingOPCWatch* This,
		/* [string][in] */ LPCWSTR objects,
		/* [in] */ DWORD doCollect);


	void __RPC_STUB ISoftingOPCWatch_CollectChanges_Stub(
		IRpcStubBuffer* This,
		IRpcChannelBuffer* _pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD* _pdwStubPhase);


	HRESULT STDMETHODCALLTYPE ISoftingOPCWatch_GetChanges_Proxy(
		ISoftingOPCWatch* This,
		/* [string][out] */ LPWSTR* changes);


	void __RPC_STUB ISoftingOPCWatch_GetChanges_Stub(
		IRpcStubBuffer* This,
		IRpcChannelBuffer* _pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD* _pdwStubPhase);


	HRESULT STDMETHODCALLTYPE ISoftingOPCWatch_GetStatistics_Proxy(
		ISoftingOPCWatch* This,
		/* [string][out] */ LPWSTR* statistics);


	void __RPC_STUB ISoftingOPCWatch_GetStatistics_Stub(
		IRpcStubBuffer* This,
		IRpcChannelBuffer* _pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD* _pdwStubPhase);


	HRESULT STDMETHODCALLTYPE ISoftingOPCWatch_GetObject_Proxy(
		ISoftingOPCWatch* This,
		/* [string][in] */ LPCWSTR objectDescription,
		/* [string][out] */ LPWSTR* obj);


	void __RPC_STUB ISoftingOPCWatch_GetObject_Stub(
		IRpcStubBuffer* This,
		IRpcChannelBuffer* _pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD* _pdwStubPhase);



#endif  /* __ISoftingOPCWatch_INTERFACE_DEFINED__ */


#ifndef __ISoftingOPCTrace_INTERFACE_DEFINED__
#define __ISoftingOPCTrace_INTERFACE_DEFINED__

	/* interface ISoftingOPCTrace */
	/* [unique][uuid][object] */


	EXTERN_C const IID IID_ISoftingOPCTrace;

#if defined(__cplusplus) && !defined(CINTERFACE)

	MIDL_INTERFACE("B5C68CC6-E339-44ce-82DF-43293D9708ED")
ISoftingOPCTrace :
	public IUnknown
	{
public:
		virtual HRESULT STDMETHODCALLTYPE SetTraceOptions(
			/* [string][in] */ LPCWSTR options) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetTraceOptions(
			/* [string][out] */ LPWSTR* options) = 0;

		virtual HRESULT STDMETHODCALLTYPE SaveTraceOptions(void) = 0;

		virtual HRESULT STDMETHODCALLTYPE GetTrace(
			/* [string][out] */ LPWSTR* trace) = 0;

	};

#else   /* C style interface */

	typedef struct ISoftingOPCTraceVtbl
	{
		BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			ISoftingOPCTrace* This,
			/* [in] */ REFIID riid,
			/* [iid_is][out] */ void** ppvObject);

		ULONG(STDMETHODCALLTYPE* AddRef)(
			ISoftingOPCTrace* This);

		ULONG(STDMETHODCALLTYPE* Release)(
			ISoftingOPCTrace* This);

		HRESULT(STDMETHODCALLTYPE* SetTraceOptions)(
			ISoftingOPCTrace* This,
			/* [string][in] */ LPCWSTR options);

		HRESULT(STDMETHODCALLTYPE* GetTraceOptions)(
			ISoftingOPCTrace* This,
			/* [string][out] */ LPWSTR* options);

		HRESULT(STDMETHODCALLTYPE* SaveTraceOptions)(
			ISoftingOPCTrace* This);

		HRESULT(STDMETHODCALLTYPE* GetTrace)(
			ISoftingOPCTrace* This,
			/* [string][out] */ LPWSTR* trace);

		END_INTERFACE
	} ISoftingOPCTraceVtbl;

	interface ISoftingOPCTrace
	{
		CONST_VTBL struct ISoftingOPCTraceVtbl* lpVtbl;
	};



#ifdef COBJMACROS


#define ISoftingOPCTrace_QueryInterface(This,riid,ppvObject)    \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISoftingOPCTrace_AddRef(This)   \
    (This)->lpVtbl -> AddRef(This)

#define ISoftingOPCTrace_Release(This)  \
    (This)->lpVtbl -> Release(This)


#define ISoftingOPCTrace_SetTraceOptions(This,options)  \
    (This)->lpVtbl -> SetTraceOptions(This,options)

#define ISoftingOPCTrace_GetTraceOptions(This,options)  \
    (This)->lpVtbl -> GetTraceOptions(This,options)

#define ISoftingOPCTrace_SaveTraceOptions(This) \
    (This)->lpVtbl -> SaveTraceOptions(This)

#define ISoftingOPCTrace_GetTrace(This,trace)   \
    (This)->lpVtbl -> GetTrace(This,trace)

#endif /* COBJMACROS */


#endif  /* C style interface */



	HRESULT STDMETHODCALLTYPE ISoftingOPCTrace_SetTraceOptions_Proxy(
		ISoftingOPCTrace* This,
		/* [string][in] */ LPCWSTR options);


	void __RPC_STUB ISoftingOPCTrace_SetTraceOptions_Stub(
		IRpcStubBuffer* This,
		IRpcChannelBuffer* _pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD* _pdwStubPhase);


	HRESULT STDMETHODCALLTYPE ISoftingOPCTrace_GetTraceOptions_Proxy(
		ISoftingOPCTrace* This,
		/* [string][out] */ LPWSTR* options);


	void __RPC_STUB ISoftingOPCTrace_GetTraceOptions_Stub(
		IRpcStubBuffer* This,
		IRpcChannelBuffer* _pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD* _pdwStubPhase);


	HRESULT STDMETHODCALLTYPE ISoftingOPCTrace_SaveTraceOptions_Proxy(
		ISoftingOPCTrace* This);


	void __RPC_STUB ISoftingOPCTrace_SaveTraceOptions_Stub(
		IRpcStubBuffer* This,
		IRpcChannelBuffer* _pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD* _pdwStubPhase);


	HRESULT STDMETHODCALLTYPE ISoftingOPCTrace_GetTrace_Proxy(
		ISoftingOPCTrace* This,
		/* [string][out] */ LPWSTR* trace);


	void __RPC_STUB ISoftingOPCTrace_GetTrace_Stub(
		IRpcStubBuffer* This,
		IRpcChannelBuffer* _pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD* _pdwStubPhase);



#endif  /* __ISoftingOPCTrace_INTERFACE_DEFINED__ */



#ifndef __SOProxy_LIBRARY_DEFINED__
#define __SOProxy_LIBRARY_DEFINED__

	/* library SOProxy */
	/* [helpstring][version][uuid] */




	EXTERN_C const IID LIBID_SOProxy;
#endif /* __SOProxy_LIBRARY_DEFINED__ */

	/* Additional Prototypes for ALL interfaces */

	/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



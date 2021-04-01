//-----------------------------------------------------------------------------
// Supported defines:
//
//  Operating systems:
//   SOOS_WINDOWS            : Microsoft Windows operating systems
//    SOOS_WINDOWS_CE        : Microsoft Windows CE and Windows CE .NET
//                             operating systems.
//   SOOS_LINUX              : Linux operating systems
//
//  Development environments:
//   SODEVENV_CYGWIN         : Cygwin compiler
//
//  Library features:
//  Main features:
//   SOFEATURE_DCOM          : Support of the DCOM OPC interfaces
//   SOFEATURE_SOAP          : Support of the SOAP OPC interfaces
//
//  Sub features:
//   SOFEATURE_XML_METHOD    : Support parsing of XML encoded function calls
//   SOFEATURE_DA10          : Support the OPC DA 1.0 interfaces
//   SOFEATURE_COMBINED_SERVER : Create a combined OPC DA and AE server
//   SOFEATURE_WATCH         : Support the Watch interfaces
//   SOFEATURE_COMIF_HOOKS   : Provide begin and end hooks for COM interface calls
//
//  SOCmn features:
//   SOFEATURE_CMN_OPCSERVER : Include all to SOCmn needed for an OPC server
//   SOFEATURE_CMN_OPCCLIENT : Include all to SOCmn needed for an OPC client
//   SOFEATURE_CMN_XML       : Support for eXpat XML Parser
//   SOFEATURE_CMN_SOCKET    : Support for TCP/IP sockets
//    SOFEATURE_CMN_SOCKET_SRV    : Classes for TCP/IP servers
//    SOFEATURE_CMN_SOCKET_CLIENT : Classes for TCP/IP clients
//   SOFEATURE_CMN_TRACE     : Include tracing functionality
//   SOFEATURE_CMN_VARIANT   : Include VARIANT class
//   SOFEATURE_CMN_HTTP      : Include HTTP parsing class
//   SOFEATURE_CMN_ELEMENT   : Include base class for tree objects
//   SOFEATURE_CMN_OPC       : Add OPC specific parts to SOCmn
//   SOFEATURE_CMN_STRINGEX  : Enhanced string class
//   SOFEATURE_CMN_TIME      : Include DateTime class
//  only for linux:
//   SOFEATURE_CMN_TRACE_GETTID : use SYS_gettid in traces (new Kernels)
//   SOFEATURE_CMN_TRACE_GETPID : use getpid in traces (old Kernels)
//        otherwise use pthread_self, which is nost portable
//
// Compilation flags:
//  SOFLAG_NO_DEFAULT_FEATURES : Don't set the default features of the
//                               operating system
//  SOFLAG_NO_DLLS           : Don't use the shared libraries (DLLs) of the
//                             OPC Toolkit. This flag has to be set if you
//                             include the source of the OPC Toolkit to your
//                             application (only Windows).
//  SOFLAG_NO_FORCE_LIBS     : Don't import the libraries of the OPC Toolkit
//                             by default (only Windows).
//  SOFLAG_NO_FORCE_HEADERS  : Don't import the headers of the OPC Toolkit
//                             by default.
//  SOFLAG_DEMO_VERSION      : Use the demo libraries of the OPC Toolkit.
//
//-----------------------------------------------------------------------------

#ifndef SOFLAG_NO_CFG
#define SOFLAG_NO_CFG

//-----------------------------------------------------------------------------
// Operating systems and default features                                     |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Windows
#if defined(WIN32) || defined(WIN64)

#ifndef SOOS_WINDOWS
#define SOOS_WINDOWS
#endif

#ifndef SOFLAG_NO_DEFAULT_FEATURES

#ifndef SOFEATURE_DCOM
#define SOFEATURE_DCOM
#endif

#ifndef SOFEATURE_SOAP
#define SOFEATURE_SOAP
#endif

#endif

#endif // Windows
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Windows CE
#ifdef _WIN32_WCE

#ifndef SOOS_WINDOWS
#define SOOS_WINDOWS
#endif

#ifndef SOOS_WINDOWS_CE
#define SOOS_WINDOWS_CE
#endif

#ifndef SOFLAG_NO_DEFAULT_FEATURES

#ifndef SOFEATURE_DCOM
#define SOFEATURE_DCOM
#endif

#ifndef SOFEATURE_SOAP
#define SOFEATURE_SOAP
#endif

#endif

#endif // Windows CE
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Linux
#ifdef __linux__

#ifndef SOOS_LINUX
#define SOOS_LINUX
#endif

#define SOFEATURE_CMN_TRACE_GETPID

#ifndef SOFLAG_NO_DEFAULT_FEATURES

#ifndef SOFEATURE_SOAP
#define SOFEATURE_SOAP
#endif

#endif

#endif // Linux
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Cygwin
#ifdef __CYGWIN__

#ifndef SOOS_LINUX
#define SOOS_LINUX
#endif

#ifndef SODEVENV_CYGWIN
#define SODEVENV_CYGWIN
#endif

#ifndef SOFLAG_NO_DEFAULT_FEATURES

#ifndef SOFEATURE_SOAP
#define SOFEATURE_SOAP
#endif

#endif

#endif // Cygwin
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Libraries                                                                  |
//-----------------------------------------------------------------------------

#ifdef SOCMN
#ifndef SOFEATURE_CMN_OPCSERVER
#define SOFEATURE_CMN_OPCSERVER
#endif
#ifndef SOFEATURE_CMN_OPCCLIENT
#define SOFEATURE_CMN_OPCCLIENT
#endif
#endif

#ifdef SOSRV
#ifndef SOFEATURE_CMN_OPCSERVER
#define SOFEATURE_CMN_OPCSERVER
#endif
#ifndef SOFEATURE_CMN_OPCCLIENT
#define SOFEATURE_CMN_OPCCLIENT
#endif
#endif

#ifdef SODAS
#ifndef SOFEATURE_CMN_OPCSERVER
#define SOFEATURE_CMN_OPCSERVER
#endif
#ifndef SOFEATURE_CMN_OPCCLIENT
#define SOFEATURE_CMN_OPCCLIENT
#endif
#endif

#ifdef SOAES
#ifndef SOFEATURE_CMN_OPCSERVER
#define SOFEATURE_CMN_OPCSERVER
#endif
#ifndef SOFEATURE_CMN_OPCCLIENT
#define SOFEATURE_CMN_OPCCLIENT
#endif
#endif

#ifdef SOASS
#ifndef SOFEATURE_CMN_OPCSERVER
#define SOFEATURE_CMN_OPCSERVER
#endif
#ifndef SOFEATURE_CMN_OPCCLIENT
#define SOFEATURE_CMN_OPCCLIENT
#endif
#endif

#ifdef SOCLT
#ifndef SOFEATURE_CMN_OPCSERVER
#define SOFEATURE_CMN_OPCSERVER
#endif
#ifndef SOFEATURE_CMN_OPCCLIENT
#define SOFEATURE_CMN_OPCCLIENT
#endif
#endif

#ifdef SODAC
#ifndef SOFEATURE_CMN_OPCSERVER
#define SOFEATURE_CMN_OPCSERVER
#endif
#ifndef SOFEATURE_CMN_OPCCLIENT
#define SOFEATURE_CMN_OPCCLIENT
#endif
#endif

#ifdef SOAEC
#ifndef SOFEATURE_CMN_OPCSERVER
#define SOFEATURE_CMN_OPCSERVER
#endif
#ifndef SOFEATURE_CMN_OPCCLIENT
#define SOFEATURE_CMN_OPCCLIENT
#endif
#endif



//-----------------------------------------------------------------------------
// Main features                                                              |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef SOFEATURE_SOAP

#ifdef SOFEATURE_CMN_OPCSERVER

#ifndef SOFEATURE_XML_METHOD
#define SOFEATURE_XML_METHOD
#endif

#ifndef SOFEATURE_CMN_SOCKET_SRV
#define SOFEATURE_CMN_SOCKET_SRV
#endif

#endif // SOFEATURE_CMN_OPCSERVER

#ifdef SOFEATURE_CMN_OPCCLIENT

#ifndef SOFEATURE_XML_METHOD
#define SOFEATURE_XML_METHOD
#endif

#ifndef SOFEATURE_CMN_SOCKET_CLIENT
#define SOFEATURE_CMN_SOCKET_CLIENT
#endif

#endif // SOFEATURE_CMN_OPCCLIENT

#endif // SOFEATURE_SOAP
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef SOFEATURE_DCOM

#ifdef SOFEATURE_CMN_OPCSERVER

#ifndef SOFEATURE_WATCH
#define SOFEATURE_WATCH
#endif

#endif // SOFEATURE_CMN_OPCSERVER

#ifndef SOOS_WINDOWS_CE

#ifndef SOFEATURE_DA10
#define SOFEATURE_DA10
#endif

#endif // SOOS_WINDOWS_CE

#endif // SOFEATURE_DCOM
//-----------------------------------------------------------------------------

#ifdef SOFEATURE_TUNNEL
#ifdef SOFEATURE_CMN_OPCSERVER

#ifndef SOFEATURE_CMN_SOCKET_SRV
#define SOFEATURE_CMN_SOCKET_SRV
#endif

#endif // SOFEATURE_CMN_OPCSERVER

#ifdef SOFEATURE_CMN_OPCCLIENT

#ifndef SOFEATURE_CMN_SOCKET_CLIENT
#define SOFEATURE_CMN_SOCKET_CLIENT
#endif

#endif // SOFEATURE_CMN_OPCCLIENT
#endif //SOFEATURE_TUNNEL

//-----------------------------------------------------------------------------
// Sub features                                                               |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef SOFEATURE_WATCH

#ifndef SOFEATURE_XML_METHOD
#define SOFEATURE_XML_METHOD
#endif

#endif // SOFEATURE_XML_METHOD
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef SOFEATURE_XML_METHOD

#ifndef SOFEATURE_CMN_XML
#define SOFEATURE_CMN_XML
#endif

#endif // SOFEATURE_XML_METHOD
//-----------------------------------------------------------------------------


#ifdef SOFEATURE_CMN_SOCKET_SRV
#ifndef SOFEATURE_CMN_SOCKET
#define SOFEATURE_CMN_SOCKET
#endif
#endif

#ifdef SOFEATURE_CMN_SOCKET_CLIENT
#ifndef SOFEATURE_CMN_SOCKET
#define SOFEATURE_CMN_SOCKET
#endif
#endif

//-----------------------------------------------------------------------------
// SOCmn features                                                             |
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef SOFEATURE_CMN_OPCSERVER

#ifndef SOFEATURE_CMN_TRACE
#define SOFEATURE_CMN_TRACE
#endif

#ifndef SOFEATURE_CMN_VARIANT
#define SOFEATURE_CMN_VARIANT
#endif

#ifndef SOFEATURE_CMN_ELEMENT
#define SOFEATURE_CMN_ELEMENT
#endif

#ifndef SOFEATURE_CMN_HTTP
#define SOFEATURE_CMN_HTTP
#endif

#ifndef SOFEATURE_CMN_OPC
#define SOFEATURE_CMN_OPC
#endif

#ifndef SOFEATURE_CMN_STRINGEX
#define SOFEATURE_CMN_STRINGEX
#endif

#ifndef SOFEATURE_CMN_TIME
#define SOFEATURE_CMN_TIME
#endif

#ifndef SOFEATURE_CMN_XML
#define SOFEATURE_CMN_XML
#endif

#endif // SOFEATURE_CMN_OPCSERVER
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef SOFEATURE_CMN_OPCCLIENT

#ifndef SOFEATURE_CMN_TRACE
#define SOFEATURE_CMN_TRACE
#endif

#ifndef SOFEATURE_CMN_VARIANT
#define SOFEATURE_CMN_VARIANT
#endif

#ifndef SOFEATURE_CMN_ELEMENT
#define SOFEATURE_CMN_ELEMENT
#endif

#ifndef SOFEATURE_CMN_HTTP
#define SOFEATURE_CMN_HTTP
#endif

#ifndef SOFEATURE_CMN_OPC
#define SOFEATURE_CMN_OPC
#endif

#ifndef SOFEATURE_CMN_STRINGEX
#define SOFEATURE_CMN_STRINGEX
#endif

#ifndef SOFEATURE_CMN_TIME
#define SOFEATURE_CMN_TIME
#endif

#endif // SOFEATURE_CMN_OPCCLIENT
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Undefs                                                                     |
//-----------------------------------------------------------------------------

// Insert your custom undefs below e.g.
// #ifdef SOFEATURE_CMN_TRACE
// #undef SOFEATURE_CMN_TRACE
// #endif

#endif // SOFLAG_NO_CFG

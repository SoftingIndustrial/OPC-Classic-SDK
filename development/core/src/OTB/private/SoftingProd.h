#ifndef __SOFTINGPROD_H
#define __SOFTINGPROD_H

//procuct number 0 is reserved

#define SPROD_4C_20_WB                      1
#define SPROD_4C_20_PC_EL                   2
#define SPROD_4C_20_EC_EL                   3
#define SPROD_4C_20_CL                      4
#define SPROD_4C_20_PC_L_DB                 5
#define SPROD_4C_20_PC_L_CL                 6
#define SPROD_OPC_SRV_TOOL_XPRESS           7
#define SPROD_OPC_DA_SRV_DEV_KIT            8
#define SPROD_OPC_DA_CLIENT_DEV_KIT         9
#define SPROD_OPC_AE_SRV_DEV_KIT            10
#define SPROD_OPC_AE_CLIENT_DEV_KIT         11
#define SPROD_OPC_DEV_KIT_EXT_CE            12
#define SPROD_OPC_DA_CLIENT_CTRL            13
#define SPROD_OPC_DA_CLIENT_CTRL_SE         14
#define SPROD_OPC_AE_CLIENT_CTRL            15
#define SPROD_PROFIBUS_OPC_SERVER           16
#define SPROD_PROFIBUS_OPC_CONFIG           17
#define SPROD_PROFIBUS_DTM                  18
#define SPROD_OPC_AE_CLIENT_CTRL_RUNTIME_V314    19 
#define SPROD_4C_DRESDEN_ELEKTRONIK_MST32   20
#define SPROD_4C_DRESDEN_ELEKTRONIK_PEM05   21
#define SPROD_S7_OPC_SERVER                 22
#define SPROD_4C_20_FC_EL                   23
#define SPROD_FF_HSE_CLIENT                 24
#define SPROD_OPC_DASRV_TOOL_XPRESS         25
#define SPROD_OPC_DA_CLIENT_V32             26
#define SPROD_OPC_DA_SERVER_V32             27
#define SPROD_OPC_AE_CLIENT_V32             28
#define SPROD_OPC_AE_SERVER_V32             29
#define SPROD_OPC_WINCE_V32                 30
#define SPROD_OPC_DOTNET_DA_CLIENT_V32      31
#define SPROD_OPC_XPRESS_DA_SERVER_V32      32
#define SPROD_OPC_DAAE_SERVER_V32           33
#define SPROD_4C_CMZ_FCT241                 34
#define SPROD_4C_OSAI_CM                    35
#define SPROD_4C_SKM_KMDIREKT               36
#define SPROD_4C_SKM_KSYS                   37
#define SPROD_OPC_DA_CLIENT_V40             38
#define SPROD_OPC_DA_SERVER_V40             39
#define SPROD_OPC_AE_CLIENT_V40             40
#define SPROD_OPC_AE_SERVER_V40             41
#define SPROD_OPC_WINCE_V40                 42
#define SPROD_OPC_DOTNET_DA_CLIENT_V40      43
#define SPROD_OPC_XPRESS_DA_SERVER_V40      44
#define SPROD_OPC_DAAE_SERVER_V40           45
#define SPROD_OPC_XMLDA_SERVER_V40          46
#define SPROD_OPC_DA_CLIENT_CTRL_RUNTIME_V314    47
#define SPROD_OPC_XMLDA_LINUX_SERVER_V40    48
#define SPROD_OPC_DA_CLIENT_CTRL_SE_RUNTIME_V314 49
#define SPROD_4C_OSAI_CE                    50
#define SPROD_FF_CIT                        51
#define SPROD_4C_20_EM_SDK                  52
#define SPROD_OCT_STORAGE_DA                53
//#define SPROD_OCT_STORAGE_XMLDA             54
#define SPROD_GENVFD_FF						54
#define SPROD_OCT_OPTIMIZER                 55
#define SPROD_OCT_TUNNEL                    56
#define SPROD_OCT_BRIDGE                    57
#define SPROD_GENVFD_PA						58
#define SPROD_OCT_CONCENTRATOR              59
#define SPROD_MODBUS_TCP					60	

// new 4C product versions
#define SPROD_4C_21_WB                      61
#define SPROD_4C_21_PC_EL                   62
#define SPROD_4C_21_EC_EL                   63
#define SPROD_4C_21_CL                      64
#define SPROD_4C_21_PC_L_DB                 65
#define SPROD_4C_21_PC_L_CL                 66
#define SPROD_4C_21_DRESDEN_ELEKTRONIK_MST32   67
#define SPROD_4C_21_DRESDEN_ELEKTRONIK_PEM05   68
#define SPROD_4C_21_FC_EL                   69
#define SPROD_4C_21_CMZ_FCT241				70
#define SPROD_4C_21_OSAI_CM					71
#define SPROD_4C_21_SKM_KMDIREKT			72
#define SPROD_4C_21_SKM_KSYS				73
#define SPROD_4C_21_OSAI_CE					74
#define SPROD_4C_21_EM_SDK					75
#define SPROD_OPC_DA_CLIENT_CTRL_V314		76
#define SPROD_OPC_DA_CLIENT_CTRL_SE_V314	77
#define SPROD_OPC_AE_CLIENT_CTRL_V314		78
#define SPROD_4C_21_BC_EL					79

#define SPROD_OTS_DA_DT_V40                 80
#define SPROD_OTS_XMLDA_DT_V40              81
#define SPROD_OTS_DA_RT_V40                 82
#define SPROD_OTS_XMLDA_RT_V40              83
#define SPROD_OPC_DOTNET_SERVER_V40         84

#define SPROD_4C_21_WEBVISER                85
#define SPROD_4C_SWB_DIS_UL_01              86

// OPC Toolbox C++ / .NET V4.1x
#define SPROD_OPC_DA_CLIENT_V41             87
#define SPROD_OPC_DA_SERVER_V41             88
#define SPROD_OPC_AE_CLIENT_V41             89
#define SPROD_OPC_AE_SERVER_V41             90
#define SPROD_OPC_WINCE_V41					91                 
#define SPROD_OPC_XMLDA_CLIENT_V41          92 //_T("OPC Toolbox C++ XML-DA Client"),  
#define SPROD_OPC_XMLDA_LINUX_CLIENT_V41    93 //_T("OPC Toolbox C++ XML-DA Client - Linux")
#define SPROD_OPC_XMLDA_SERVER_V41          94
#define SPROD_OPC_XMLDA_LINUX_SERVER_V41    95
#define SPROD_OPC_DOTNET_DA_CLIENT_V41      96
#define SPROD_OPC_DOTNET_SERVER_V41         97
#define SPROD_FDT_ET200MDTM	                98	
#define SPROD_FDT_LDDTM			            99	// FF HSE/LD DTMs

// 4C Building Controller Devices
#define SPROD_4C_21_CF_PROFI_DP				100
#define SPROD_4C_21_CF_BACNET				101
#define SPROD_4C_21_CF_MOD_RTU				102
#define SPROD_4C_21_CF_MOD_IP				103
#define SPROD_4C_21_CF_MBUS					104
#define SPROD_4C_21_CF_WEBVIS				105
#define SPROD_4C_21_CF_PROFI_FMS			106
#define SPROD_4C_21_CF_FIELDCONTROLLER		107
#define SPROD_4C_21_CF_BUILDINGCONTROLLER	108
#define SPROD_4C_21_CF_DELETE_ALL			109
#define SPROD_4C_21_CF_GA_NODES				110
//NOTE: 100..149 are reserved product numbers for 4C building controllers!
#define FIRST_4C_BUILDINGCONTROLLER			100
#define LAST_4C_BUILDINGCONTROLLER			110

#define SPROD_4C_21_CE_EL					150
#define SPROD_PROFINET_IO_CONTROLLER		151
#define SPROD_PROFINET_IO_DEVICE			152
#define SPROD_PROFINET_IO_SUPERVISOR		153

#define SPROD_FDT_PALINKDTM					154
#define SPROD_PTOOL							155

#define SPROD_OTC_DA_DT_V40					156                 
#define SPROD_OTC_XMLDA_DT_V40              157
#define SPROD_OTC_DA_RT_V40                 158
#define SPROD_OTC_XMLDA_RT_V40              159
#define SPROD_OPC_DA_CLIENT_CTRL_V320		160
#define SPROD_OPC_DA_CLIENT_CTRL_RT_V320	161
#define SPROD_OPC_AE_CLIENT_CTRL_V320		162
#define SPROD_OPC_AE_CLIENT_CTRL_RT_V320	163 
#define SPROD_SNMP_OPC_SERVER				164 
#define SPROD_OPC_AE_CLIENT_V40X			165
#define SPROD_OPC_AE_SERVER_V40X			166
#define SPROD_OPC_AE_CLIENT_RT_V40X			167
#define SPROD_OPC_AE_SERVER_RT_V40X			168

#define SPROD_4C_20_EM_FCT200				169
#define SPROD_4C_20_EM_FCT241				170

#define SPROD_4C_EKON_DIS_01                171
// rename SPROD_OCT_COLLECTOR to SPROD_OCT_DBADDON
//#define SPROD_OCT_DBADDON                   171	// REMOVED!
//#define SPROD_OCT_EXCHANGE                  172	// REMOVED!
#define SPROD_OCT_BASE						172

#define SPROD_4C_21_GA_EL					173

#define SPROD_OCT_SERVER_CONNECTIONS		174
#define SPROD_OCT_COLLECTOR_ITEMS			175

// OPC Toolbox Source Code Licenses V4.20
#define SPROD_OTS_DA_SOURCE_V42				176
#define SPROD_OTS_XMLDA_SOURCE_V42          177
#define SPROD_OTC_DA_SOURCE_V42				178
#define SPROD_OTC_XMLDA_SOURCE_V42          179
#define SPROD_OTC_AE_SOURCE_V42				180
#define SPROD_OTS_AE_SOURCE_V42				181

#define SPROD_PROFINET_IO_OPC				182
#define SPROD_MODBUS_TCP_OEM				183	// HYDRA Modbus/TCP OPC Server (OEM-Variant)
#define SPROD_INDUSOL_PBINSPEKTOR			184

#define SPROD_OTS_XMLDA_LINUX_SOURCE_V42    185
#define SPROD_OTC_XMLDA_LINUX_SOURCE_V42    186
#define SPROD_OTS_XMLDA_LINUX_OBJECT_V42    187
#define SPROD_OTC_XMLDA_LINUX_OBJECT_V42    188

#define SPROD_FF_OPC_SERVER_BASE			189 
#define SPROD_FF_OPC_SERVER_DEVICES			190 

#define SPROD_OTC_TP_OBJECT					191 
#define SPROD_OTS_TP_OBJECT					192 

#define SPROD_FF_CIT_SINGLEUSER				193 
#define SPROD_DTM_H1_FFUSB_FL				194 
#define SPROD_FF_CONF_STD_F					195

#define SPROD_OPCTB_UA_CLIENT_WINDOWS_DT_V50	196
#define SPROD_OPCTB_UA_SERVER_WINDOWS_DT_V50	197
#define SPROD_OPCTB_UA_CLIENT_VXWORKS_DT_V50	198
#define SPROD_OPCTB_UA_SERVER_VXWORKS_DT_V50	199
#define SPROD_OPCTB_UA_CLIENT_LINUX_DT_V50		200
#define SPROD_OPCTB_UA_SERVER_LINUX_DT_V50		201
#define SPROD_OPCTB_UA_CLIENT_WINDOWSCE_DT_V50	202
#define SPROD_OPCTB_UA_SERVER_WINDOWSCE_DT_V50	203

#define SPROD_FFCIT_DT_V40						204

#define SPROD_INDUSOL_PNINSPEKTOR				205

// OPC Toolbox DesignTime Licenses V4.30
#define SPROD_OTC_DA_DT_V430                    206
#define SPROD_OTC_AE_DT_V430                    207
#define SPROD_OTC_XMLDA_DT_WINDOWS_V430         208
#define SPROD_OTC_XMLDA_DT_LINUX_V430           209

#define SPROD_OTS_DA_DT_V430                    210
#define SPROD_OTS_AE_DT_V430                    211
#define SPROD_OTS_XMLDA_DT_WINDOWS_V430         212
#define SPROD_OTS_XMLDA_DT_LINUX_V430           213

// OPC Toolbox Source Code Licenses V4.30
#define SPROD_OTC_DA_SOURCE_V430                214
#define SPROD_OTC_XMLDA_SOURCE_WINDOWS_V430     215
#define SPROD_OTC_XMLDA_SOURCE_LINUX_V430       216
#define SPROD_OTC_AE_SOURCE_V430                217

#define SPROD_OTS_DA_SOURCE_V430                218
#define SPROD_OTS_XMLDA_SOURCE_WINDOWS_V430     219
#define SPROD_OTS_XMLDA_SOURCE_LINUX_V430       220
#define SPROD_OTS_AE_SOURCE_V430                221

#define SPROD_UA_NET_SDK_CLIENT_DT_V10          222
#define SPROD_UA_NET_SDK_SERVER_DT_V10          223
#define SPROD_UA_NET_SDK_CLIENT_SOURCE_V10      224
#define SPROD_UA_NET_SDK_SERVER_SOURCE_V10      225

#define SPROD_PNIO_STACK_CONTROLLER_DT			226
#define SPROD_PNIO_STACK_DEVICE_DT				227
#define SPROD_PNIO_STACK_SUPERVISOR_DT			228

#define SPROD_UA_NET_SERVER_GATEWAY_DT_V1X		229
#define SPROD_UA_NET_CLIENT_GATEWAY_DT_V1X		230


#define SPROD_OPCTB_UA_CLT_WINDOWS_SRC_V50		231
#define SPROD_OPCTB_UA_CLT_VXWORKS_SRC_V50		232
#define SPROD_OPCTB_UA_CLT_LINUX_SRC_V50		233

#define SPROD_OPCTB_UA_SRV_WINDOWS_SRC_V50		234
#define SPROD_OPCTB_UA_SRV_VXWORKS_SRC_V50		235
#define SPROD_OPCTB_UA_SRV_LINUX_SRC_V50		236


// OPC Toolbox DesignTime Licenses V4.40
#define SPROD_OTC_DA_DT_V440                    237
#define SPROD_OTC_AE_DT_V440                    238
#define SPROD_OTC_XMLDA_DT_WINDOWS_V440         239

#define SPROD_OTS_DA_DT_V440                    240
#define SPROD_OTS_AE_DT_V440                    241
#define SPROD_OTS_XMLDA_DT_WINDOWS_V440         242


// OPC Toolbox Source Code Licenses V4.40
#define SPROD_OTC_DA_SOURCE_V440                243
#define SPROD_OTC_XMLDA_SOURCE_WINDOWS_V440     244
#define SPROD_OTC_AE_SOURCE_V440                245

#define SPROD_OTS_DA_SOURCE_V440                246
#define SPROD_OTS_XMLDA_SOURCE_WINDOWS_V440     247
#define SPROD_OTS_AE_SOURCE_V440                248

#define SPROD_OPCTB_UA_XML_IMPORT_V530          249

#define SPROD_UA_NET_SDK_CLIENT_DT_V14          250
#define SPROD_UA_NET_SDK_SERVER_DT_V14          251
#define SPROD_UA_NET_SDK_CLIENT_SOURCE_V14      252
#define SPROD_UA_NET_SDK_SERVER_SOURCE_V14      253

#define SPROD_OPCTB_UA_CLIENT_WINDOWS_DT_V55	254
#define SPROD_OPCTB_UA_SERVER_WINDOWS_DT_V55	255
#define SPROD_OPCTB_UA_CLIENT_VXWORKS_DT_V55	256
#define SPROD_OPCTB_UA_SERVER_VXWORKS_DT_V55	257
#define SPROD_OPCTB_UA_CLIENT_LINUX_DT_V55		258
#define SPROD_OPCTB_UA_SERVER_LINUX_DT_V55		259

// OPC UA Embedded Toolkit
#define SPROD_OPCUA_EMBEDDED_TOOLKIT            260

#define SPROD_UA_NET_SDK_CLIENT_DT_V143         261
#define SPROD_UA_NET_SDK_SERVER_DT_V143         262
#define SPROD_UA_NET_SDK_CLIENT_SOURCE_V143     263
#define SPROD_UA_NET_SDK_SERVER_SOURCE_V143     264

#define SPROD_UA_NET_STD_CLIENT_DT_V10          265
#define SPROD_UA_NET_STD_SERVER_DT_V10          266
#define SPROD_UA_NET_STD_CLIENT_SOURCE_V10      267
#define SPROD_UA_NET_STD_SERVER_SOURCE_V10      268

// OPC Toolbox DesignTime Licenses V4.47
#define SPROD_OTC_DA_DT_V447                    269
#define SPROD_OTC_AE_DT_V447                    270
#define SPROD_OTC_XMLDA_DT_WINDOWS_V447         271
#define SPROD_OTS_DA_DT_V447                    272
#define SPROD_OTS_AE_DT_V447                    273
#define SPROD_OTS_XMLDA_DT_WINDOWS_V447         274

// OPC Toolbox Source Code Licenses V4.47
#define SPROD_OTC_DA_SOURCE_V447                275
#define SPROD_OTC_AE_SOURCE_V447                276
#define SPROD_OTC_XMLDA_SOURCE_WINDOWS_V447     277
#define SPROD_OTS_DA_SOURCE_V447                278
#define SPROD_OTS_AE_SOURCE_V447                279
#define SPROD_OTS_XMLDA_SOURCE_WINDOWS_V447     280

#define LAST_PROD_NUMBER                        280 // last product number
#define MAX_PROD_NAME_LEN                       64

//to be continued up to max product number 4095

#ifdef SOFTINGLM

#define NO_PB_INTERFACES	9
#define INTERFACE_OFFSET	5000

#ifdef __cplusplus
extern "C"
{
extern TCHAR g_softingProductNames[LAST_PROD_NUMBER + 1][MAX_PROD_NAME_LEN];
extern TCHAR g_softingOrderNumbers[LAST_PROD_NUMBER + 1][MAX_PROD_NAME_LEN];
extern TCHAR g_profibusInterfaceNames[NO_PB_INTERFACES + 1][MAX_PROD_NAME_LEN]; 
}
#else
extern TCHAR g_softingProductNames[LAST_PROD_NUMBER + 1][MAX_PROD_NAME_LEN];
extern TCHAR g_softingOrderNumbers[LAST_PROD_NUMBER + 1][MAX_PROD_NAME_LEN];
extern TCHAR g_profibusInterfaceNames[NO_PB_INTERFACES + 1][MAX_PROD_NAME_LEN]; 
#endif

#endif // SOFTINGLM

#ifdef FC_SLBASE

#define SPROD_TYPE_RUNTIME          0x00000001
#define SPROD_TYPE_DESIGNTIME       0x80000000
#define SPROD_TYPE_SOURCE           0x00000002
#define SPROD_TYPE_CLIENTCONTROLS   0x00000004

#ifdef __cplusplus
extern "C"
{
extern unsigned long g_softingProductType[LAST_PROD_NUMBER + 1];
}
#else
extern unsigned long g_softingProductType[LAST_PROD_NUMBER + 1];
#endif

#endif // FC_SLBASE

#endif //__SOFTINGPROD_H
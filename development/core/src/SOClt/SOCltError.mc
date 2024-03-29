;//-----------------------------------------------------------------------------
;//                                                                            |
;//                   Softing Industrial Automation GmbH                       |
;//                        Richard-Reitzner-Allee 6                            |
;//                           85540 Haar, Germany                              |
;//                                                                            |
;//                 This is a part of the Softing OPC Toolkit                  |
;//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
;//                           All Rights Reserved                              |
;//                                                                            |
;//-----------------------------------------------------------------------------
;//-----------------------------------------------------------------------------
;//                           OPC Toolkit - SOClt                              |
;//                                                                            |
;//  Filename    : SOCltError.mc                                               |
;//  Version     : 4.47.1                                                      |
;//  Date        : 27-July-2020                                                |
;//                                                                            |
;//  Description : This file contains client toolkit specific exit texts       |
;//                                                                            |
;//-----------------------------------------------------------------------------

;
;#ifndef _SOCLT_ERROR_H_
;#define _SOCLT_ERROR_H_
;

SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )

LanguageNames=( English=0x409:MSG00409
                German=0x407:MSG00407
              )

MessageId=1000 
Severity=Error 
SymbolicName=SOCLT_ERR_COM_INIT

Language=English
Failed to initialize DCOM runtime library.
.
Language=German
DCOM Laufzeitbibliothek konnte nicht initialisiert werden.
.

MessageId=1001
Severity=Error 
SymbolicName=SOCLT_ERR_CREATE_SERVER_BROWSER
Language=English
Failed to create server browser object.
.
Language=German
Server Browser Objekt konnte nicht erzeugt werden.
.

MessageId=1002
Severity=Error 
SymbolicName=SOCLT_ERR_ENUM_SERVERS
Language=English
Failed to enumerate OPC servers.
.
Language=German
Die Aufzählung von OPC Servern ist fehlgeschlagen.
.

MessageId=1003
Severity=Error 
SymbolicName=SOCLT_ERR_GET_CLASS_DETAILS
Language=English
Failed to get class details.
.
Language=German
Die Klassendetails konnten nicht ermittelt werden.
.

MessageId=1004
Severity=Error 
SymbolicName=SOCLT_ERR_GET_CLSID
Language=English
Failed to get the CLSID of the server.
.
Language=German
Die CLSID des Servers konnte nicht ermittelt werden.
.

MessageId=1010
Severity=Error 
SymbolicName=SOCLT_ERR_ADVISE_SHUTDOWN
Language=English
Failed to advise IOPCShutdown interface.
.
Language=German
Die Rückrufstelle IOPCShutdown konnte nicht registriert werden.
.

MessageId=1011
Severity=Error 
SymbolicName=SOCLT_ERR_UNADVISE_SHUTDOWN
Language=English
Failed to unadvise IOPCShutdown interface.
.
Language=German
Die Rückrufstelle IOPCShutdown konnte nicht deregistriert werden.
.

MessageId=1012
Severity=Error 
SymbolicName=SOCLT_ERR_EXCEPTION
Language=English
Failed to unadvise IOPCShutdown interface.
.
Language=German
Die Rückrufstelle IOPCShutdown konnte nicht deregistriert werden.
.

MessageId=1013
Severity=Error 
SymbolicName=SOCLT_ERR_OUT_OF_MEMORY
Language=English
Out of memory.
.
Language=German
Es steht nicht genügend Speicher zur Verfügung.
.

MessageId=1014
Severity=Error 
SymbolicName=SOCLT_ERR_CREATE_SERVER
Language=English
Failed to create OPC Server object.
.
Language=German
Das OPC Server Objekt konnte nicht erzeugt werden.
.

MessageId=1015
Severity=Error
SymbolicName=SOCLT_ERR_COM_QI
Language=English
No interface.
.
Language=German
COM-Schnittstelle nicht gefunden.
.

MessageId=1016
Severity=Error
SymbolicName=SOCLT_ERR_SET_CLIENT_NAME
Language=English
Failed to set client name.
.
Language=German
Der Client-Name konnte nicht gesetzt werde.
.

MessageId=1017
Severity=Error
SymbolicName=SOCLT_ERR_SET_LOCALE_ID
Language=English
Failed to set locale id.
.
Language=German
Das gewünste Lokale ID konnte nicht gesetzt werde.
.

MessageId=1018
Severity=Error
SymbolicName=SOCLT_ERR_GET_LOCALE_ID
Language=English
Failed to retrieve locale id.
.
Language=German
Das Lokale ID konnte nicht ermittelt werde.
.

MessageId=1020
Severity=Error
SymbolicName=SOCLT_ERR_NO_PARENT_OBJECT
Language=English
No reference to the parent object.
.
Language=German
Das Objekt hat keine Referenz zum übergeordneten Objekt.
.

MessageId=1021
Severity=Error
SymbolicName=SOCLT_ERR_PARENT_NOT_CONNECTED
Language=English
Parent object not connected.
.
Language=German
Das übergeordnete Objekt ist nicht verbunden.
.

MessageId=1022
Severity=Error
SymbolicName=SOCLT_ERR_DISCONNECT
Language=English
Error disconnecting object.
.
Language=German
Beim Verbindungsabbau ist ein Fehler aufgetreten.
.

MessageId=1023
Severity=Error
SymbolicName=SOCLT_ERR_PAR_INPUT
Language=English
Invalid input parameter.
.
Language=German
Ungültiges Input-Parameter.
.

MessageId=1024
Severity=Error
SymbolicName=SOCLT_ERR_PAR_OUTPUT
Language=English
Invalid output parameter.
.
Language=German
Ungültiges Output-Parameter.
.
MessageId=1025
Severity=Error
SymbolicName=SOCLT_ERR_SRV_GET_STATUS
Language=English
Failed to retrieve server's status.
.
Language=German
Der Serverstatus konnte nicht ermittelt werden.
.
MessageId=1026
Severity=Error
SymbolicName=SODAC_ERR_SRV_QUERY_ORG
Language=English
Cannot query namespace organization type.
.
Language=German
Namensraum-Organizationstyp konnte nicht abgefragt werden.
.

MessageId=1027
Severity=Error
SymbolicName=SOCLT_ERR_BROWSE_UNEXPECTED
Language=English
An unexpected error occured while browsing.
.
Language=German
Beim Browsen ist ein unerwarteter Fehler aufgetreten.
.

MessageId=1028
Severity=Error
SymbolicName=SOCLT_ERR_ITEM_PROPS_NOT_SUPPORTED
Language=English
Item properties are not supported.
.
Language=German
Item Properties werden nicht unterstützt.
.

MessageId=1029
Severity=Error
SymbolicName=SOCLT_ERR_SRV_QUERY_LCIDS
Language=English
Faied to query available locale ids.
.
Language=German
Ünterschtützte Sprachkennungen konnten nicht ermittelt werden.
.

MessageId=1030
Severity=Error
SymbolicName=SOCLT_ERR_GET_ERROR_STRING
Language=English
Server was not able to deliver error message.
.
Language=German
Der Server konnte keine Fehlerbeschreibung liefern.
.

MessageId=1031
Severity=Error
SymbolicName=SOCLT_ERR_LOGON
Language=English
Could not logon to the server.
.
Language=German
Fehler beim Einloggen in den Server.
.

MessageId=1032
Severity=Error
SymbolicName=SOCLT_ERR_LOGOFF
Language=English
Could not logoff from the server.
.
Language=German
Fehler beim Abmelden vom Server.
.

MessageId=1033
Severity=Error
SymbolicName=SOCLT_ERR_ISAVAILABLEPRIV
Language=English
Could not query for available security features.
.
Language=German
Die verfügbare Sicherheitsfunktionalität konnte nicht abgefragt werden.
.

MessageId=1034
Severity=Error
SymbolicName=SOCLT_ERR_BROWSE_NOTSUPPORTED
Language=English
The called browse functionality is not supported by the OPC server.
.
Language=German
Die aufgerufene Browsefunktionalität wird vom OPC Server nicht unterstützt.
.

;//////////////////////////////////////////////////////////////////////////////
;//
;// AEC - Errors
;//
;//////////////////////////////////////////////////////////////////////////////

MessageId=1100
Severity=Error
SymbolicName=SOAEC_ERR_ADD_SUBSCRIPTION
Language=English
Failed to create event subscription.
.
Language=German
Die Event Subscription konnte nicht erzeugt werden.
.
MessageId=1101
Severity=Error
SymbolicName=SOAEC_ERR_GET_FILTER
Language=English
Failed to retrieve filter information.
.
Language=German
Die Filter-Angaben konnten nicht ermittelt werden.
.

MessageId=1102
Severity=Error
SymbolicName=SOAEC_ERR_SET_FILTER
Language=English
Failed to set filter information.
.
Language=German
Die Filter-Angaben konnten nicht gesetzt werden.
.

MessageId=1103
Severity=Error
SymbolicName=SOAEC_ERR_SUBS_GET_ATTR
Language=English
Failed to retrieve returned attributes for subscription.
.
Language=German
Die Subscription-Attribute konnten nicht ermittelt werden.
.

MessageId=1104
Severity=Error
SymbolicName=SOAEC_ERR_SUBS_SET_ATTR
Language=English
Failed to select returned attributes for subscription.
.
Language=German
Die Subscription-Attribute konnten nicht festgelegt werden.
.

MessageId=1105
Severity=Error
SymbolicName=SOAEC_ERR_SUBS_GET_STATE
Language=English
Failed to retrieve subscription state.
.
Language=German
Der Subscription-Status konnte nicht ermittelt werden.
.

MessageId=1106
Severity=Error
SymbolicName=SOAEC_ERR_SUBS_SET_STATE
Language=English
Failed to set subscription state.
.
Language=German
Der Subscription-Status konnte nicht gesetzt werden.
.

MessageId=1107
Severity=Error
SymbolicName=SOAEC_ERR_TRANSLATE_TO_ITEM_IDS
Language=English
Failed to translate to item ids.
.
Language=German
Die Übersetzung zu Item IDs ist fehlgeschlagen.
.

MessageId=1108
Severity=Error
SymbolicName=SOAEC_ERR_GET_CONDITION_STATE
Language=English
Failed to get condition state.
.
Language=German
Der Condition-Zustand konnte nich abgefragt werden.
.

MessageId=1109
Severity=Error
SymbolicName=SOAEC_ERR_CREATE_BROWSE_OBJECT
Language=English
Failed to create browser object.
.
Language=German
Das Browse-Objekt konnte nicht erzeugt werden.
.

MessageId=1110
Severity=Error
SymbolicName=SOAEC_ERR_REFRESH
Language=English
Failed to refresh.
.
Language=German
Die 'Refresh' Operation konnte nich durchgeführt werden.
.

MessageId=1111
Severity=Error
SymbolicName=SOAEC_ERR_CANCEL_REFRESH
Language=English
Failed to cancel refresh operation.
.
Language=German
Die 'Refresh' Operation konnte nich abgebrochen werden.
.

MessageId=1112
Severity=Error
SymbolicName=SOAEC_ERR_ENABLE_CONDITIONS
Language=English
Enabling/Disabling condition failed.
.
Language=German
Bedingungen konnte nicht ein- oder ausgeschaltet werden.
.

MessageId=1113
Severity=Error
SymbolicName=SOAEC_ERR_REMOVE_SUBSCRIPTION
Language=English
Failed to remove the event subscription.
.
Language=German
Die Event Subscription konnte nicht entfernt werden.
.

;//////////////////////////////////////////////////////////////////////////////
;//
;// DAC - Errors
;//
;//////////////////////////////////////////////////////////////////////////////

MessageId=1200
Severity=Error
SymbolicName=SODAC_ERR_ADVISE_DATAOBJECT_READ
Language=English
Failed to advise IDataObject read connection.
.
Language=German
Die Rückrufstelle IDataObject konnte nicht zum Lesen registriert werden.
.

MessageId=1201
Severity=Error
SymbolicName=SODAC_ERR_ADVISE_DATAOBJECT_WRITE
Language=English
Failed to advise IDataObject write connection.
.
Language=German
Die Rückrufstelle IDataObject konnte nicht zum Schreiben registriert werden.
.


MessageId=1202
Severity=Error
SymbolicName=SODAC_ERR_UNADVISE_DATAOBJECT
Language=English
Failed to unadvise IDataObject connection.
.
Language=German
Die Rückrufstelle IDataObject konnte nicht deregistriert werden.
.

MessageId=1203
Severity=Error
SymbolicName=SODAC_ERR_ADVISE_DATACALLBACK
Language=English
Failed to advise IOPCDataCallback connection.
.
Language=German
Die Rückrufstelle IOPCDataCallback konnte nicht registriert werden.
.

MessageId=1204
Severity=Error
SymbolicName=SODAC_ERR_UNADVISE_DATACALLBACK
Language=English
Failed to unadvise IOPCDataCallback connection.
.
Language=German
Die Rückrufstelle IOPCDataCallback konnte nicht deregistriert werden.
.

MessageId=1205
Severity=Error
SymbolicName=SODAC_ERR_REMOVE_GROUP
Language=English
Failed to remove group object.
.
Language=German
Das Group-Objekt konnte nicht zerstört werden.
.

MessageId=1206
Severity=Error
SymbolicName=SODAC_ERR_ADD_GROUP
Language=English
Failed to create group object.
.
Language=German
Das Group-Objekt konnte nicht erzeugt werden.
.

MessageId=1207
Severity=Error
SymbolicName=SODAC_ERR_GET_PUBGROUPBYNAME
Language=English
Failed to get public group object by name.
.
Language=German
Das Public Group-Objekt mit disem Namen konnte nicht erzeugt werden.
.

MessageId=1208
Severity=Error
SymbolicName=SODAC_BAD_GROUP_CONNECTION_TYPE
Language=English
Bad group connection type.
.
Language=German
Unzulässiger Verbindungstyp für diese Gruppe.
.

MessageId=1209
Severity=Error
SymbolicName=SODAC_ERR_GRP_ADD_ITEMS
Language=English
Cannot add items.
.
Language=German
Items konnten nicht hinzugefügt werden.
.

;#define SODAC_ERR_ITM_ADD_ITEM									SODAC_ERR_GRP_ADD_ITEMS


MessageId=1210
Severity=Error
SymbolicName=SODAC_ERR_GRP_SET_CLIENTHANDLES_ITEMS
Language=English
Items client handle cannot be set.
.
Language=German
Das Client Handle vom Item kann nicht gesetzt werden.
.

;#define SODAC_ERR_ITM_SET_CLIENTHANDLE				 SODAC_ERR_GRP_SET_CLIENTHANDLES_ITEMS

MessageId=1211
Severity=Error
SymbolicName=SODAC_ERR_GRP_REMOVE_ITEMS
Language=English
Cannot remove items.
.
Language=German
Items konnten nicht gelöscht werden.
.

;#define SODAC_ERR_ITM_REMOVE_ITEM						SODAC_ERR_GRP_REMOVE_ITEMS

MessageId=1212
Severity=Error
SymbolicName=SODAC_ERR_GRP_ACTIVATE_ITEMS
Language=English
Cannot activate items.
.
Language=German
Items konnten nicht aktiviert werden.
.

;#define SODAC_ERR_ITM_ACTIVATE_ITEM					SODAC_ERR_GRP_ACTIVATE_ITEMS

MessageId=1213
Severity=Error
SymbolicName=SODAC_ERR_GRP_DEACTIVATE_ITEMS
Language=English
Cannot deactivate items.
.
Language=German
Items konnten nicht deaktiviert werden.
.

;#define SODAC_ERR_ITM_DEACTIVATE_ITEM				SODAC_ERR_GRP_DEACTIVATE_ITEMS


MessageId=1214
Severity=Error
SymbolicName=SODAC_ERR_GRP_CREATE_ITEMENUM
Language=English
Cannot create item enumerator object.
.
Language=German
Das Itemaufzählungsobjekt konnte nicht erzeugt werden.
.


MessageId=1220
Severity=Error
SymbolicName=SOCLT_ERR_DEMO_TIME
Language=English
This is a demo version with a limited runtime.
.
Language=German
Dies ist eine Demo-Version mit beschränker Laufzeit.
.

MessageId=1221
Severity=Error
SymbolicName=SODAC_ERR_OPCDA_QI_REQ
Language=English
Query for an obligatory OPC DA interface failed.
.
Language=German
Eine der obligatorischen OPC DA Schnittstellen wurde nicht gefunden.
.

MessageId=1222
Severity=Error
SymbolicName=SODAC_ERR_OPCDA_QI_REQ_V20
Language=English
Query for an obligatory OPC DA 2.0 interface failed.
.
Language=German
Eine der obligatorischen OPC DA 2.0 Schnittstellen wurde nicht gefunden.
.

MessageId=1223
Severity=Error
SymbolicName=SODAC_ERR_OPCDA_QI_REQ_V10
Language=English
Query for an obligatory OPC DA 1.0 interface failed.
.
Language=German
Eine der obligatorischen OPC DA 1.0 Schnittstellen wurde nicht gefunden.
.

MessageId=1224
Severity=Error
SymbolicName=SODAC_ERR_OPCDA_QI_OPT
Language=English
Query for an optional OPC DA interface failed.
.
Language=German
Eine der optionalen OPC DA Schnittstellen wurde nicht gefunden.
.

MessageId=1225
Severity=Error
SymbolicName=SODAC_ERR_GRP_GET_STATE
Language=English
Failed to retrieve group state.
.
Language=German
Der Status des Group-Objekts konnte nicht ermittelt werden.
.

MessageId=1226
Severity=Error
SymbolicName=SODAC_ERR_GRP_SET_STATE
Language=English
Failed to set group state.
.
Language=German
Der Status des Group-Objekts konnte nicht gesetzt werden.
.

MessageId=1227
Severity=Error
SymbolicName=SODAC_ERR_OPCDA_QI_REQ_V30
Language=English
Query for an obligatory OPC DA 3.0 interface failed.
.
Language=German
Eine der obligatorischen OPC DA 3.0 Schnittstellen wurde nicht gefunden.
.

MessageId=1228
Severity=Error
SymbolicName=SODAC_ERR_GRP_GET_KEEPALIVE
Language=English
Failed to get group keep-alive time.
.
Language=German
Die Keep-Alive Zeit der Gruppe kann nicht geholt werden.
.

MessageId=1229
Severity=Error
SymbolicName=SODAC_ERR_GRP_SET_KEEPALIVE
Language=English
Failed to set group keep-alive time.
.
Language=German
Die Keep-Alive Zeit der Gruppe kann nicht gesetzt werden.
.

MessageId=1230
Severity=Error
SymbolicName=SODAC_ERR_GRP_BAD_CLIENTHANDLE
Language=English
OPC server returned bad client handle.
.
Language=German
OPC Server lieferte einen ungültigen Client Handle zurück.
.

MessageId=1231
Severity=Error
SymbolicName=SODAC_ERR_GRP_WRONG_ACTMODE
Language=English
OPC server has wrong activation mode.
.
Language=German
Falscher Aktivierungsmodus beim OPC Server Objekt.
.

MessageId=1233
Severity=Error
SymbolicName=SODAC_ERR_GRP_GET_STATE_PUB
Language=English
Failed to retrieve public group state.
.
Language=German
Der Status des Public Group-Objekts konnte nicht ermittelt werden.
.

MessageId=1234
Severity=Error
SymbolicName=SODAC_ERR_GRP_SET_STATE_PUB
Language=English
Failed to set public group state.
.
Language=German
Der Status des Public Group-Objekts konnte nicht gesetzt werden.
.

MessageId=1236
Severity=Error
SymbolicName=SODAC_ERR_GRP_SET_NAME
Language=English
Failed to set group name.
.
Language=German
Der Name des Group-Objekts konnte nicht gesetzt werden.
.

MessageId=1237
Severity=Error
SymbolicName=SODAC_ERR_GRP_MOVE_TO_PUB
Language=English
Cannot move group to public.
.
Language=German
Das Group-Objekts konnte nicht "public" gemacht werden.
.
MessageId=1240
Severity=Error
SymbolicName=SODAC_ERR_GRP_SYNC_READ
Language=English
Synchronous read of item(s) failed.
.
Language=German
Synchrones Lesen von Items fehlgeschlagen.
.

MessageId=1241
Severity=Error
SymbolicName=SODAC_ERR_GRP_SYNC_WRITE
Language=English
Synchronous writing items failed.
.
Language=German
Synchrones Schreiben von Items fehlgeschlagen.
.

MessageId=1242
Severity=Error
SymbolicName=SODAC_ERR_GRP_NO_ASYNC_CON
Language=English
No connection advised for asynchronous operations.
.
Language=German
Keine Rückrufstelle fürs asynchrone Lesen/Schreiben ist registriert.
.

MessageId=1243
Severity=Error
SymbolicName=SODAC_ERR_GRP_ASYNC_READ
Language=English
Asynchronous reading items failed.
.
Language=German
Asynchrones Lesen von Items fehlgeschlagen.
.

MessageId=1245
Severity=Error
SymbolicName=SODAC_ERR_GRP_ASYNC_WRITE
Language=English
Asynchronous writing items failed.
.
Language=German
Asynchrones Schreiben von Items fehlgeschlagen.
.

MessageId=1246
Severity=Error
SymbolicName=SODAC_ERR_GRP_ASYNC_REFRESH
Language=English
Asynchronous refreshing items failed.
.
Language=German
Asynchrones Aktualisieren von Items fehlgeschlagen.
.

MessageId=1247
Severity=Error
SymbolicName=SODAC_ERR_GRP_ASYNC_CANCEL
Language=English
Asynchronous operation could not be canceled.
.
Language=German
Die asynchrone Operation konnte nicht abgebrochen werden.
.

MessageId=1248
Severity=Error
SymbolicName=SODAC_ERR_GRP_ITM_GET_DEADBAND
Language=English
Error get item deadband.
.
Language=German
Das Item Deadband konnte nicht gelesen werden.
.

MessageId=1249
Severity=Error
SymbolicName=SODAC_ERR_GRP_ITM_SET_DEADBAND
Language=English
Error set item deadband.
.
Language=German
Das Item Deadband konnte nicht gesetzt werden.
.

MessageId=1250
Severity=Error
SymbolicName=SODAC_ERR_GRP_ITM_CLEAR_DEADBAND
Language=English
Error clear item deadband.
.
Language=German
Das Item Deadband konnte nicht gelöscht werden.
.


MessageId=1251
Severity=Error
SymbolicName=SODAC_ERR_GRP_SET_ITEM_DATATYPES
Language=English
Cannot set item(s) datatypes.
.
Language=German
Itemtyp konnte nicht gesetzt werden.
.

;#define SODAC_ERR_ITM_SET_DATATYPE					SODAC_ERR_GRP_SET_ITEM_DATATYPES

MessageId=1252
Severity=Error
SymbolicName=SODAC_ERR_GRP_VALIDATE_ITEMS
Language=English
Item validation failed.
.
Language=German
Itemvalidierung konnte nicht durchgeführt werden.
.
;#define SODAC_ERR_ITM_VALIDATE_ITEM				SODAC_ERR_GRP_VALIDATE_ITEMS

MessageId=1253
Severity=Error
SymbolicName=SODAC_ERR_GRP_GET_DEADBAND
Language=English
Error get group deadband.
.
Language=German
Das Group Deadband konnte nicht gelesen werden.
.


MessageId=1252
Severity=Informational
SymbolicName=SODAC_INF_SRV_VERSION_10A
Language=English
OPC server version 1.0A.
.
Language=German
OPC Server Version 1.0A.
.

MessageId=1254
Severity=Informational
SymbolicName=SODAC_INF_SRV_VERSION_20
Language=English
OPC server version 2.0.
.
Language=German
OPC Server Version 2.0.
.

MessageId=1255
Severity=Informational
SymbolicName=SODAC_INF_SRV_VERSION_30
Language=English
OPC server version 3.0.
.
Language=German
OPC Server Version 3.0.
.

MessageId=1259
Severity=Error
SymbolicName=SODAC_ERR_SRV_CHANGE_BRS_POS
Language=English
Can't change namespace browse position.
.
Language=German
Die Browse-Position kann nicht geändert werden.
.

MessageId=1260
Severity=Error
SymbolicName=SODAC_ERR_SRV_BRS_ITEMIDS
Language=English
Can't browse OPC item identifiers.
.
Language=German
Item IDs können nicht ermittelt werden.
.

MessageId=1261
Severity=Error
SymbolicName=SODAC_ERR_SRV_GET_ITEMID
Language=English
Can't get fully qualified item name.
.
Language=German
Der volständige Name des Items kann nicht ermittelt werden.
.

MessageId=1262
Severity=Error
SymbolicName=SODAC_ERR_SRV_BRS_ACCESS_PATHS
Language=English
Can't browse item's access path.
.
Language=German
Der Zugrifsphad des Items kann nicht ermittelt werden.
.

MessageId=1263
Severity=Error
SymbolicName=SODAC_ERR_SRV_QUERY_PROPERTIES
Language=English
Failed to retrieve item's available properties.
.
Language=German
Die Itemseigenschaften konnten nicht ermittelt werden.
.

MessageId=1264
Severity=Error
SymbolicName=SODAC_ERR_SRV_GET_PROP_VALUE
Language=English
Failed to retrieve property values.
.
Language=German
Die Werte von Eigenschaften konnten nicht ermittelt werden.
.

MessageId=1265
Severity=Error
SymbolicName=SODAC_ERR_SRV_GET_PROP_ITEMID
Language=English
Failed to retrieve the property ids for item.
.
Language=German
Die IDs von Eigenschaften konnten nicht ermittelt werden.
.

MessageId=1266
Severity=Error
SymbolicName=SODAC_ERR_SRV_LOAD
Language=English
Server's configuration file could not be loaded.
.
Language=German
Servers Konfigurationsdatei kann nicht geladen werden.
.

MessageId=1267
Severity=Error
SymbolicName=SODAC_ERR_SRV_SAVE
Language=English
Server's configuration file could not be saved.
.
Language=German
Servers Konfigurationsdatei kann nicht gespeichert werden.
.

MessageId=1268
Severity=Error
SymbolicName=SODAC_ERR_SRV_READ_ITEM_VALUE
Language=English
Failed to read at least one item value.
.
Language=German
Mindestens ein Itemwert konnte nicht gelesen werden.
.

MessageId=1269
Severity=Error
SymbolicName=SODAC_ERR_SRV_WRITE_ITEM_VALUE
Language=English
Failed to write at least one item value
.
Language=German
Mindestens ein Itemwert konnte nicht geschrieben werden.
.

MessageId=1270
Severity=Error
SymbolicName=SODAC_ERR_SRV_SAVE_COMPLETED
Language=English
Saving server's configuration file could not be completed.
.
Language=German
Das Speichern der Konfigurationsdatei konnte nicht abgeschlossen werden.
.

MessageId=1271
Severity=Error
SymbolicName=SODAC_ERR_SRV_GET_CUR_FILE
Language=English
Failed to retrieve the path of the current configuration file.
.
Language=German
Der Pfad der aktuellen Konfigurationsdatei konnte nicht ermittelt werden.
.

MessageId=1272
Severity=Warning
SymbolicName=SODAC_ERR_NO_MAPPING_FOR_PROPERTIES
Language=English
Mapping to item is not supported for this property.
.
Language=German
Diese Eigenschaft kann nicht auf ein Item abgebildet werden.
.

MessageId=1273
Severity=Error
SymbolicName=SODAC_ERR_SRV_GET_PROPERTIES
Language=English
Failed to get properties.
.
Language=German
Fehler beim Erfragen der Properties.
.

MessageId=1274
Severity=Error
SymbolicName=SODAC_ERR_SRV_BROWSE
Language=English
Failed to browse.
.
Language=German
Fehler beim Browsen.
.

MessageId=1275
Severity=Error
SymbolicName=SODAC_ERR_READANDDATACHANGE_CALLBACK
Language=English
Error in the OnReadComplete or OnDataChange callback.
.
Language=German
Fehler im OnReadComplete oder OnDataChange Callback.
.

MessageId=1276
Severity=Error
SymbolicName=SODAC_ERR_WRITECOMPLETE_CALLBACK
Language=English
Error in the OnWriteComplete callback.
.
Language=German
Fehler im OnWriteComplete Callback.
.

MessageId=1277
Severity=Error
SymbolicName=SODAC_ERR_CANCELCOMPLETE_CALLBACK
Language=English
Error in the OnCancelComplete callback.
.
Language=German
Fehler im OnCancelComplete Callback.
.


;#endif  // _SOCLT_ERROR_H_
;

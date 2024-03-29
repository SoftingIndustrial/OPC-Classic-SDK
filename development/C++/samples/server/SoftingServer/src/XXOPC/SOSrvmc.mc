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
;//                           OPC Toolkit - SOSrv                              |
;//                                                                            |
;//  Filename    : SOSrvmc.mc                                                  |
;//  Version     : 4.47.1                                                      |
;//  Date        : 27-July-2020                                                |
;//                                                                            |
;//  Description : OPC error texts                                             |
;//                                                                            |
;//-----------------------------------------------------------------------------

;#ifndef __SOSRVMC_H
;#define __SOSRVMC_H
;

MessageIdTypedef=HRESULT
SeverityNames=(
               Success=0x0
               Informational=0x1
               Warning=0x2
               Error=0x3
              )
FacilityNames=(
               System=0x0FF
               DA=0x1FF;
               AE=0x2FF;
              )
LanguageNames=(
              English=0x409:SOSrv409
              German=0x407:SOSrv407
              Swedish=0x41D:SOSrv41D
              )
OutputBase=16



;// OPC AE
MessageID=0x0200
Severity=Success
Facility=AE
SymbolicName=SOSRV_S_ALREADYACKED
Language=English
The condition has already been acknowledged.%0
.
Language=German
Die Bedingung wurde bereits bestätigt.%0
.
Language=Swedish
Tillståndet har redan kvitterats.%0
.

MessageID=
SymbolicName=SOSRV_S_INVALIDBUFFERTIME
Language=English
The buffer time parameter was invalid.%0
.
Language=German
Der Parameter für die Puffer Zeit ist ungültig.%0
.
Language=Swedish
Ogiltig parameter för tidsbuffert.%0
.

MessageID=
SymbolicName=SOSRV_S_INVALIDMAXSIZE
Language=English
The max size parameter was invalid.%0
.
Language=German
Der Parameter für die maximale Größe ist ungültig.%0
.
Language=Swedish
Ogiltig parameter för maximal storlek.%0
.

;// error codes

MessageID=
Severity=Error
SymbolicName=SOSRV_E_INVALIDBRANCHNAME
Language=English
The string was not recognized as an area name.%0
.
Language=German
Der String ist kein gültiger Area Name.%0
.
Language=Swedish
Strängen är inte ett giltigt areanamn.%0
.

MessageID=
SymbolicName=SOSRV_E_INVALIDTIME
Language=English
The time does not match the latest active time.%0
.
Language=German
Die Zeit stimmt nicht mit der letzten Aktivierungszeit überein.%0
.
Language=Swedish
Tiden överenstämmer inte med senaste aktiveringstid.%0
.

MessageID=
SymbolicName=SOSRV_E_BUSY
Language=English
A refresh is currently in progress.%0
.
Language=German
Eine Aktualisierung wird gegenwärtig vorgenommen.%0
.
Language=Swedish
Uppdatering pågår.%0
.

MessageID=
SymbolicName=SOSRV_E_NOINFO
Language=English
Information is not available.%0
.
Language=German
Die Information ist nicht verfügbar.%0
.
Language=Swedish
Informationen är inte tillgänglig.%0
.


;// OPC DA

MessageID=1
Severity=Error
Facility=DA
SymbolicName=SOSRV_E_INVALIDHANDLE
Language=English
The value of the handle is invalid.%0
.
Language=German
Ein ungültiges OPC-Handle wurde übergeben.%0
.
Language=Swedish
Handtagets värde är ogiltigt.%0
.

MessageID=4
SymbolicName=SOSRV_E_BADTYPE
Language=English
The server cannot convert the data between the specified format and/or requested data type and the canonical data type.%0
.
Language=German
Der Server kann den Wert nicht zwischen dem geforderten und dem ursprünglichen Datentyp konvertieren.%0
.
Language=Swedish
Servern kan inte konvertera data mellan den begärda datatypen och den vedertagna datatypen.%0
.

MessageID=
SymbolicName=SOSRV_E_PUBLIC
Language=English
The requested operation cannot be done on a public group.%0
.
Language=German
Die gewünschte Operation kann nicht in einer öffentlichen Gruppe ausgeführt werden.%0
.
Language=Swedish
Den begärda operationen går inte att utföra på en publik grupp.%0
.

MessageID=
SymbolicName=SOSRV_E_BADRIGHTS
Language=English
The item's access rights do not allow the operation.%0
.
Language=German
Die Zugriffsrechte des Items erlauben die Operation nicht.%0
.
Language=Swedish
Objektets åtkomsträttigheter tillåter inte att operationen utförs.%0
.

MessageID=
SymbolicName=SOSRV_E_UNKNOWNITEMID
Language=English
The item ID is not defined in the server address space or no longer exists in the server address space.%0
.
Language=German
Das Item ist nicht definiert oder nicht länger im Adressraum des Servers verfügbar.%0
.
Language=Swedish
Objektet är inte längre tillgängligt i serverns adressutrymme.%0
.

MessageID=
SymbolicName=SOSRV_E_INVALIDITEMID
Language=English
The item definition does not conform to the server syntax.%0
.
Language=German
Die Item Definition entspricht nicht der Syntax des Servers.%0
.
Language=Swedish
Objektdefinitionen överensstämmer inte med serverns syntax.%0
.

MessageID=
SymbolicName=SOSRV_E_INVALIDFILTER
Language=English
The filter string was not valid.%0
.
Language=German
Der Filter String war nicht gültig.%0
.
Language=Swedish
Ogiltig filtersträng.%0
.

MessageID=
SymbolicName=SOSRV_E_UNKNOWNPATH
Language=English
The item's access path is not known to the server.%0
.
Language=German
Der Zugriffspfad des Items ist dem Server nicht bekannt.%0
.
Language=Swedish
Accessvägen till objektet är inte känd för servern.%0
.

MessageID=
SymbolicName=SOSRV_E_RANGE
Language=English
The value was out of range.%0
.
Language=German
Der Wert war ausserhalb des Wertebereiches.%0
.
Language=Swedish
Värdet var utanför tillåtet område.%0
.

MessageID=
SymbolicName=SOSRV_E_DUPLICATENAME
Language=English
Duplicate name not allowed.%0
.
Language=German
Ein doppelter Name ist nicht erlaubt.%0
.
Language=Swedish
Det är inte tillåtet att dubblera namn.%0
.

MessageID=
Severity=Success
SymbolicName=SOSRV_S_UNSUPPORTEDRATE
Language=English
The server does not support the requested data rate but will use the closest available rate.%0
.
Language=German
Der Server unterstützt die angeforderte Datenrate nicht. Er wird aber die näheste verfügbare Rate benutzen.%0
.
Language=Swedish
Servern stödjer inte den begärda datahastigheten men kommer att använda närmaste tillgängliga hastighet.%0
.

MessageID=
SymbolicName=SOSRV_S_CLAMP
Language=English
A value passed to write was accepted but the output was clamped.%0
.
Language=German
Der an write übergebene Wert wurde akzeptiert, aber die Ausgabe wurde gehalten.%0
.
Language=Swedish
Ett värde sänt till write togs emot, men utsignalen stoppades.%0
.

MessageID=
SymbolicName=SOSRV_S_INUSE
Language=English
The operation cannot be performed because the object is bering referenced.%0
.
Language=German
Die Funktion konnte nicht beendet werden, da noch Referenzen auf das Objekt existieren.%0
.
Language=Swedish
Operationen går inte att slutföra, eftersom objektet fortfarande har existerande referenser.%0
.

MessageID=
Severity=Error
SymbolicName=SOSRV_E_INVALIDCONFIGFILE
Language=English
The server's configuration file has an invalid format.%0
.
Language=German
Das Konfigurationsfile des Servers hat ein ungültiges Format.%0
.
Language=Swedish
Ogiltigt format för serverns konfigurationsfil.%0
.

MessageID=
SymbolicName=SOSRV_E_NOTFOUND
Language=English
The requested object (e.g. a public group) was not found.%0
.
Language=German
Der Server kann das angeforderte Objekt nicht finden.%0
.
Language=Swedish
Servern kunde inte hitta begärt objekt.%0
.

MessageID=0x203
SymbolicName=SOSRV_E_INVALID_PID
Language=English
The specified property ID is not valid for the item.%0
.
Language=German
Die übergebene Eigenschaftskennung ist nicht gültig für das Item.%0
.
Language=Swedish
Servern känner inte igen angivet egenskaps-ID.%0
.

MessageID=0x400
SymbolicName=SOSRV_E_DEADBANDNOTSET
Language=English
The item deadband has not been set for this item.%0
.
Language=German
Das Schwankungsbreite für den Itemwert wurde für dieses Item nicht gesetzt.%0
.
Language=Swedish
The item deadband has not been set for this item.%0
.

MessageID=0x401
SymbolicName=SOSRV_E_DEADBANDNOTSUPPORTED
Language=English
The item does not support deadband.%0
.
Language=German
Das Item unterstützt nicht das Setzen einer Schwankungsbreite.%0
.
Language=Swedish
The item does not support deadband.%0
.

MessageID=0x402
SymbolicName=SOSRV_E_NOBUFFERING
Language=English
The server does not support buffering of data items that are collected at a faster rate than the group update rate.%0
.
Language=German
Der Server unterstützt nicht das Speichern von Itemwerten, die schneller als die Aktualisierungsrate gesammelt werden.%0
.
Language=Swedish
The server does not support buffering of data items that are collected at a faster rate than the group update rate.%0
.

MessageID=0x403
SymbolicName=SOSRV_E_INVALIDCONTINUATIONPOINT
Language=English
The continuation point is not valid.%0
.
Language=German
Der Fortsetzungspunkt ist nicht gültig.%0
.
Language=Swedish
The continuation point is not valid.%0
.

MessageID=0x404
SymbolicName=SOSRV_S_DATAQUEUEOVERFLOW
Language=English
Not every detected change has been returned since the server's buffer reached its limit and had to purge out the oldest data.%0
.
Language=German
Nicht jede Datenänderung wurde geliefert, da der Speicher des Servers seine Obergrenze erreicht hat und die ältesten Werte verworfen wurden.%0
.
Language=Swedish
Not every detected change has been returned since the server's buffer reached its limit and had to purge out the oldest data.%0
.

MessageID=0x405
SymbolicName=SOSRV_E_RATENOTSET
Language=English
There is no sampling rate set for the specified item.%0
.
Language=German
Für das Item ist keine Samplingrate gesetzt.%0
.
Language=Swedish
There is no sampling rate set for the specified item.%0
.

MessageID=0x406
SymbolicName=SOSRV_E_NOTSUPPORTED
Language=English
The server does not support writing of quality and/or timestamp.%0
.
Language=German
Der Server unterstützt nicht das Schreiben der Qualität und/oder des Zeitstempels.%0
.
Language=Swedish
The server does not support writing of quality and/or timestamp.%0
.

;// OPC XML DA

MessageID=0x5600
SymbolicName=SOSRV_E_INVALIDHOLDTIME
Language=English
The hold time is too long (determined by server).%0
.
Language=German
Die Haltezeit ist zu lang (vom Server festgestellt).%0
.
Language=Swedish
The hold time is too long (determined by server).%0
.

MessageID=0x5601
SymbolicName=SOSRV_E_INVALIDITEMNAME
Language=English
The item name does not conform the server’s syntax.%0
.
Language=German
Der Item Name entspricht nicht der Syntax des Servers.%0
.
Language=Swedish
The item name does not conform the server’s syntax.%0
.

MessageID=0x5602
SymbolicName=SOSRV_E_INVALIDITEMPATH
Language=English
The item path does not conform the server’s syntax.%0
.
Language=German
Der Item Pfad entspricht nicht der Syntax des Servers.%0
.
Language=Swedish
The item path does not conform the server’s syntax.%0
.

MessageID=0x5603
SymbolicName=SOSRV_E_NOSUBSCRIPTION
Language=English
An invalid set of subscription handles was passed to the request.%0
.
Language=German
Ein ungülter Satz von Subscription Handles wurde mit der Anfrage übergeben.%0
.
Language=Swedish
An invalid set of subscription handles was passed to the request.%0
.

MessageID=0x5604
SymbolicName=SOSRV_E_READONLY
Language=English
The value is read only and may not be written to.%0
.
Language=German
Der Wert ist nur lesbar und kann nicht geschrieben werden.%0
.
Language=Swedish
The value is read only and may not be written to.%0
.

MessageID=0x5605
SymbolicName=SOSRV_E_WRITEONLY
Language=English
The value is write-only and may not be read from or returned as part of a write response.%0
.
Language=German
Der Wert ist nur schreibar und kann nicht gelesen oder als Teil einer Schreibantwort zurückgegeben werden.%0
.
Language=Swedish
The value is write-only and may not be read from or returned as part of a write response.%0
.

MessageID=0x5606
SymbolicName=SOSRV_E_UNKNOWNITEMNAME
Language=English
The item name is no longer available in the server address space.%0
.
Language=German
Der Item Name nicht definiert oder nicht länger im Adressraum des Servers verfügbar.%0
.
Language=Swedish
The item name is no longer available in the server address space.%0
.

MessageID=0x5607
SymbolicName=SOSRV_E_UNKNOWNITEMPATH
Language=English
The item path is no longer available in the server address space.%0
.
Language=German
Der Item Pfad nicht definiert oder nicht länger im Adressraum des Servers verfügbar.%0
.
Language=Swedish
The item path is no longer available in the server address space.%0
.

MessageID=0x5608
SymbolicName=SOSRV_E_TIMEDOUT
Language=English
The operation took too long to complete (determined by server).
.
Language=German
Die Anfrage hat zu viel Zeit benötigt (vom Server festgestellt).%0
.
Language=Swedish
The operation took too long to complete (determined by server).
.

MessageID=0x5609
SymbolicName=SOSRV_E_SERVERSTATE
Language=English
The operation could not complete due to an abnormal server state.%0
.
Language=German
Die Anfrage kann wegen eines nicht normalen Serverzustandes nicht bearbeitet werden.%0
.
Language=Swedish
The operation could not complete due to an abnormal server state.%0
.

MessageID=0x5610
SymbolicName=SOSRV_E_ACCESS_DENIED
Language=English
The server denies access (read and/or write) to the specified item. This error is typically caused by Web Service security (e.g. globally disabled write capabilities).%0
.
Language=German
Der Server verbietet den Zugriff (lesen und/oder schreiben) auf das spezifizierte Item (read and/or write) to the specified item.%0
.
Language=Swedish
The server denies access (read and/or write) to the specified item. This error is typically caused by Web Service security (e.g. globally disabled write capabilities).%0
.

MessageID=0x5611
SymbolicName=SOSRV_E_BUSY
Language=English
The server is currenly processing another polled refresh for one or more of the subscriptions.%0
.
Language=German
Der Server bearbeitet gerade einen anderen polled Refresh für eine oder mehrere Subscriptions.%0
.
Language=Swedish
The server is currenly processing another polled refresh for one or more of the subscriptions.%0
.

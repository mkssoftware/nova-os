# NPSPEC-STORAGE-REMOTE-0001 – Nova Remote Storage

## Status

Angenommen

## Kategorie

Storage / Remote

## Zweck

NovaOS integriert entfernte Speicherressourcen in das gemeinsame Storage-Modell, ohne Anwendungen unnötig an Netzwerkprotokolle oder physische Standorte zu koppeln.

```text
Remote Storage
      ↓
Storage Provider
      ↓
Nova Storage Model
      ↓
Filesystem / Namespace
```

## Grundprinzipien

```text
Remote ≠ Local
Remote Location ≠ Object Identity
Connectivity ≠ Availability
Discovery ≠ Authority
Location Transparency ≠ Authority Transparency
```

Entfernte Ressourcen verwenden dieselben grundlegenden Identitäts-, Capability- und Namespace-Konzepte wie lokale Ressourcen.

## Remote-Storage-Modell

```text
RemoteStorage
├── ProviderID
├── EndpointID
├── State
├── Capabilities
└── RemoteLocation
```

Je nach Provider können daraus Devices, Volumes oder direkt Storage-Objekte bereitgestellt werden.

## Provider

Protokoll- und dienstspezifische Details werden durch Storage Provider gekapselt.

Beispiele:

```text
Network Filesystem
Remote Block Storage
Object Storage
NovaOS Storage Node
Cloud Storage
```

Höhere Schichten sollen nicht direkt von einem bestimmten Netzwerkprotokoll abhängig sein.

## Identität

Remote Storage darf bestehende stabile Identitäten verwenden.

```text
ObjectID
VolumeID
DeviceID
```

Eine Änderung von Serveradresse, Route oder Netzwerkverbindung darf die logische Identität nicht automatisch verändern.

## Location Transparency

Remote gespeicherte Objekte können über dieselben logischen Referenzen wie lokale Objekte angesprochen werden.

```text
ObjectID
   ↓
Location Resolution
   ↓
Local / Remote Location
```

Ob ein Zugriff lokal oder remote ausgeführt wird, darf transparent sein, solange dadurch keine Sicherheits- oder Ausführungsanforderungen verletzt werden.

## Verbindungszustände

Mindestens folgende Zustände müssen darstellbar sein:

```text
Available
Connecting
Degraded
Offline
Unavailable
Unknown
```

`Offline` oder `Unknown` dürfen nicht als aktuelle Remote-Verfügbarkeit interpretiert werden.

## Caching

Remote Storage darf lokale Caches verwenden.

```text
Remote Object
    ↓
Local Cache
```

Cache und autoritative Remote-Ressource bleiben unterscheidbar.

Konflikte und veraltete Daten müssen erkannt werden können.

## Sicherheit

Remote-Zugriffe benötigen explizite Authority.

```text
Object / Volume
      ↓
Capability Check
      ↓
Remote Provider
```

Netzwerkzugriff allein gewährt keine Storage-Berechtigung.

Authentifizierung, Verschlüsselung, Trust- und Sovereignty-Regeln müssen vom Remote-Storage-Modell berücksichtigt werden können.

## Fehlerverhalten

Netzwerkfehler dürfen nicht mit lokalen Storage-Fehlern gleichgesetzt werden.

NovaOS muss mindestens unterscheiden können zwischen:

```text
Storage Failure
Network Failure
Authentication Failure
Remote Unavailable
Conflict
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Provider
Remote State
Location
Capabilities
Connectivity
Cache State
```

## Normative Anforderungen

1. NovaOS MUSS Remote Storage in das gemeinsame Storage-Modell integrieren können.
2. Protokollspezifische Details SOLLEN durch Storage Provider gekapselt werden.
3. Remote Location und Storage Identity MÜSSEN getrennt bleiben.
4. Netzwerkänderungen DÜRFEN stabile Storage-Identitäten nicht automatisch verändern.
5. Remote Storage MUSS capability-basiert autorisierbar sein.
6. Netzwerkzugriff DARF keine Storage-Authority implizieren.
7. Remote Storage MUSS definierte Verbindungs- und Fehlerzustände besitzen.
8. Lokale Caches DÜRFEN die autoritative Ressource nicht ersetzen, sofern dies nicht ausdrücklich vorgesehen ist.
9. Security-, Trust- und Sovereignty-Regeln MÜSSEN bei Remote-Zugriffen berücksichtigt werden.
10. Remote Storage MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-IDENTITY-0001`
- `NPSPEC-STORAGE-LOCATION-0001`
- `NPSPEC-STORAGE-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-STORAGE-DISCOVERY-0001`
- `NPSPEC-DISTRIBUTED-LOCATION-0001`
- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`

## Ergebnis

NovaOS kann lokale und entfernte Speicherressourcen über ein gemeinsames Storage-Modell verwenden. Netzwerkstandort und Protokoll bleiben von der stabilen Storage-Identität getrennt, während Sicherheit, Verfügbarkeit und Sovereignty weiterhin explizit kontrolliert werden.
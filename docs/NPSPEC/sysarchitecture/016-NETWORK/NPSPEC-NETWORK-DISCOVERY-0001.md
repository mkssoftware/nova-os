# NPSPEC-NETWORK-DISCOVERY-0001 – Nova Network Discovery

## Status

Angenommen

## Kategorie

Network / Discovery / Service Discovery

## Zweck

NovaOS definiert eine gemeinsame Infrastruktur zur Erkennung von Geräten, Diensten und Netzwerkressourcen innerhalb erreichbarer Netzwerke.

```text
Network
   ↓
Discovery Providers
   ↓
Nova Discovery
   ↓
Services / Devices
```

Discovery stellt gefundene Ressourcen bereit, erzeugt jedoch weder Vertrauen noch Zugriffsberechtigungen.

## Grundprinzip

```text
Discovered ≠ Trusted
Visible ≠ Accessible
Service Name ≠ Identity
Discovery Result ≠ Permission
```

Alle über das Netzwerk gefundenen Informationen müssen als externe, potenziell nicht vertrauenswürdige Daten behandelt werden.

## Discovery-Modell

Eine gefundene Ressource wird als logisches Discovery-Objekt beschrieben.

```text
DiscoveryObject
├── DiscoveryID
├── Type
├── Name
├── Endpoints
├── Provider
└── State
```

Optional:

```text
Capabilities
Metadata
Network Context
Interface
Security Information
Lifetime
```

Die `DiscoveryID` darf nicht als dauerhafte Identität der tatsächlichen Ressource verwendet werden.

## Discovery Provider

NovaOS soll unterschiedliche Discovery-Verfahren über eine gemeinsame Schnittstelle integrieren.

```text
Nova Discovery
├── mDNS
├── DNS-SD
├── Local Discovery
└── Future Providers
```

Provider-spezifische Protokolle bleiben voneinander getrennt.

Anwendungen sollen Discovery nicht für jedes Protokoll selbst implementieren müssen.

## Discovery Scope

Discovery muss auf definierte Netzwerkbereiche begrenzbar sein.

```text
Discovery Request
      ↓
Scope
├── Interface
├── Network
├── Protocol
└── Security Context
```

Eine Suche über ein lokales Interface darf nicht automatisch auf andere Interfaces, VPNs oder Netzwerkpfade ausgeweitet werden.

## Service Discovery

Dienste dürfen anhand logischer Servicetypen gesucht werden.

```text
Service Type
     ↓
Discovery
     ↓
Service Instances
     ↓
Endpoints
```

Ein Ergebnis darf mehrere mögliche Endpoints enthalten.

Die Auswahl des tatsächlich verwendeten Endpoints erfolgt durch die zuständigen Routing-, Multipath- und Sicherheitsmechanismen.

## Geräteerkennung

Discovery darf Netzwerkgeräte erkennen und deren angekündigte Eigenschaften bereitstellen.

Beispiele:

```text
Printer
Media Device
Storage Service
Development Device
NovaOS Node
```

Vom Gerät selbst gelieferte Namen oder Eigenschaften dürfen nicht automatisch als verifiziert gelten.

## Lebensdauer

Discovery-Ergebnisse sind temporärer Netzwerkzustand.

```text
Discovered
    ↓
Available
    ↓
Stale
    ↓
Expired
```

Einträge müssen anhand von TTL, Provider-Zustand oder Netzwerkänderungen invalidiert werden können.

Veraltete Ergebnisse dürfen nicht unbegrenzt als verfügbar dargestellt werden.

## Netzwerkwechsel

Discovery muss Änderungen der Netzwerktopologie berücksichtigen.

```text
Network Change
     ↓
Revalidate Discovery State
     ↓
Keep / Refresh / Remove
```

Ein Wechsel zwischen Ethernet, Wi-Fi, VPN oder anderen Pfaden kann unterschiedliche sichtbare Ressourcen ergeben.

## DNS-Integration

DNS-basierte Discovery-Verfahren sollen die gemeinsame DNS-Infrastruktur verwenden können.

```text
Nova Discovery
      ↓
DNS-SD / mDNS
      ↓
DNS Infrastructure
```

Discovery darf den DNS-Resolver nicht durch parallele, inkompatible Namensauflösungsmechanismen ersetzen.

## Multipath

Discovery-Ergebnisse müssen dem Netzwerkpfad oder Interface zugeordnet werden können, über den sie gefunden wurden.

```text
Service
├── Path A
└── Path B
```

Mehrere Pfade zu derselben logischen Ressource dürfen erkannt werden.

Die Gleichheit zweier Discovery-Ergebnisse darf jedoch nicht allein anhand eines Anzeigenamens angenommen werden.

## Sicherheit

Discovery darf keine automatische Vertrauensbeziehung herstellen.

Vor der tatsächlichen Nutzung eines gefundenen Dienstes können weitere Prüfungen erforderlich sein:

```text
Identity
Authentication
Trust
Capability
Firewall Policy
```

Discovery und Autorisierung bleiben getrennte Mechanismen.

## Datenschutz

Discovery kann Informationen über das lokale System oder vorhandene Dienste offenlegen.

NovaOS muss deshalb kontrollieren können:

```text
Which Services Are Advertised
Which Interfaces Advertise
Who May Discover
Which Metadata Is Exposed
```

Anwendungen dürfen nicht automatisch beliebige Systemdienste veröffentlichen.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Discovery Requests
Known Objects
Provider State
Cached Results
Announcements
Network Traffic
```

Manipulierte Discovery-Antworten dürfen keinen unbegrenzten Speicher-, CPU- oder Netzwerkverbrauch verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
DiscoveryID
Type
Name
Provider
Endpoints
Interface
State
Lifetime
Last Seen
Security State
```

Sensible Discovery-Informationen dürfen nur entsprechend der geltenden Berechtigungen sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame Network-Discovery-Infrastruktur bereitstellen.
2. Discovery und Autorisierung MÜSSEN getrennte Mechanismen bleiben.
3. Discovery-Ergebnisse MÜSSEN als nicht vertrauenswürdige Netzwerkdaten behandelt werden.
4. Discovery MUSS auf Interfaces oder Netzwerkkontexte begrenzbar sein.
5. NovaOS SOLL mehrere Discovery Provider unterstützen können.
6. mDNS und DNS-SD SOLLEN integrierbar sein.
7. Discovery-Ergebnisse MÜSSEN eine begrenzte Lebensdauer besitzen können.
8. Netzwerkänderungen MÜSSEN eine Neubewertung betroffener Discovery-Ergebnisse ermöglichen.
9. Anzeigenamen DÜRFEN NICHT allein als sichere Ressourcenidentität verwendet werden.
10. Discovery DARF Firewall- oder Capability-Regeln NICHT umgehen.
11. Discovery-Zustände und Netzwerkverkehr MÜSSEN ressourcenbegrenzt sein.
12. Gefundene Ressourcen und deren Herkunft SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-IPV4-0001`
- `NPSPEC-NETWORK-IPV6-0001`
- `NPSPEC-NETWORK-UDP-0001`
- `NPSPEC-NETWORK-DNS-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-FIREWALL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0015`

## Ergebnis

```text
Network
   ↓
Discovery Providers
   ↓
Validated Discovery Data
   ↓
Nova Discovery
   ↓
Devices + Services + Endpoints
```

NovaOS erhält damit eine gemeinsame und kontrollierte Discovery-Infrastruktur, über die Netzwerkgeräte und Dienste gefunden werden können, ohne Sichtbarkeit mit Identität, Vertrauen oder Zugriffsberechtigung gleichzusetzen.
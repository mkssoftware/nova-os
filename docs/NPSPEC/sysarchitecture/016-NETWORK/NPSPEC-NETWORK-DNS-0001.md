# NPSPEC-NETWORK-DNS-0001 – Nova Domain Name System

## Status

Angenommen

## Kategorie

Network / DNS / Name Resolution

## Zweck

NovaOS definiert eine gemeinsame DNS-Infrastruktur zur Auflösung von Domainnamen und anderen DNS-Ressourcen.

```text
Application
    ↓
Nova Resolver
    ↓
DNS Query
    ↓
Configured Resolver
    ↓
DNS Response
```

DNS-Auflösung wird als Systemdienst bereitgestellt und von Anwendungen nicht unnötig mehrfach implementiert.

## Grundprinzip

```text
Name ≠ Identity
DNS Response ≠ Trusted Data
Resolved Address ≠ Permission
DNS Cache ≠ Permanent Truth
```

Eine erfolgreiche Namensauflösung erzeugt weder Vertrauen noch Netzwerkberechtigung.

## Resolver-Modell

Der Nova Resolver verarbeitet mindestens:

```text
DNSRequest
├── RequestID
├── Name
├── Record Type
├── Security Context
└── State
```

Optional:

```text
Deadline
Resolver Policy
Network Context
ExecutionContract
```

Der Resolver darf mehrere DNS-Server und Transportwege verwenden.

## DNS Records

Mindestens folgende Record-Typen müssen verarbeitet werden können:

```text
A
AAAA
CNAME
NS
MX
TXT
PTR
SRV
SOA
```

Die Architektur muss für weitere Record-Typen erweiterbar bleiben.

Unbekannte Record-Typen dürfen nicht zu fehlerhafter Interpretation führen.

## Namensauflösung

Eine typische Auflösung erfolgt über:

```text
Name
 ↓
Resolver Cache
 ├── Hit → Result
 └── Miss
       ↓
   DNS Query
       ↓
   DNS Server
       ↓
   Validation
       ↓
   Cache
       ↓
   Result
```

Anwendungen sollen nicht wissen müssen, welcher konkrete Resolver oder Netzwerkpfad verwendet wurde.

## IPv4 und IPv6

DNS muss IPv4- und IPv6-Auflösung unterstützen.

```text
Domain
 ├→ A    → IPv4
 └→ AAAA → IPv6
```

Die Auswahl zwischen erhaltenen IPv4- und IPv6-Adressen erfolgt nicht durch DNS allein, sondern gemeinsam mit Routing und Netzwerkpolicy.

## Cache

NovaOS soll einen gemeinsamen DNS-Cache bereitstellen.

```text
DNS Response
     ↓
TTL
     ↓
Resolver Cache
```

Cache-Einträge müssen ihre TTL berücksichtigen.

Abgelaufene Einträge dürfen nicht unbegrenzt als gültige Antworten behandelt werden.

Negative Antworten dürfen entsprechend ihrer definierten Gültigkeit gecacht werden.

## Resolver-Auswahl

Mehrere Resolver dürfen gleichzeitig konfiguriert sein.

```text
Nova Resolver
 ├→ Resolver A
 ├→ Resolver B
 └→ Resolver C
```

Die Auswahl darf berücksichtigen:

```text
Network Interface
Network Path
VPN
Security Policy
Trust Policy
Resolver Availability
```

DNS-Anfragen eines VPN-Kontexts dürfen beispielsweise gezielt über den zugehörigen Resolver geleitet werden.

## Transport

DNS muss mindestens klassisches DNS über UDP und TCP unterstützen können.

```text
DNS
├→ UDP
└→ TCP
```

Zusätzliche verschlüsselte Transportmechanismen dürfen ergänzt werden.

Der DNS-Kern darf nicht dauerhaft an einen einzelnen Transportmechanismus gekoppelt sein.

## DNSSEC

NovaOS soll DNSSEC-validierte Namensauflösung unterstützen können.

```text
DNS Response
     ↓
DNSSEC Validation
     ↓
Secure / Insecure / Bogus / Indeterminate
```

Ein nicht validiertes Ergebnis darf nicht als kryptografisch bestätigt dargestellt werden.

DNSSEC erzeugt keine Anwendungs- oder Netzwerkberechtigung.

## Asynchrones I/O

DNS-Auflösung soll asynchron erfolgen können.

```text
Resolve Request
      ↓
Pending
      ↓
DNS Processing
      ↓
Completion
```

Cancellation und Deadlines sollen unterstützt werden.

Ein langsamer DNS-Server darf nicht unbegrenzt eine Anwendung oder den Resolver blockieren.

## Multipath und Netzwerkwechsel

DNS muss mit mehreren Netzwerkpfaden umgehen können.

```text
Wi-Fi Resolver
VPN Resolver
Ethernet Resolver
```

Ändert sich der aktive Netzwerkpfad, müssen Resolver-Konfiguration und Cache-Gültigkeit entsprechend der zugehörigen Policy neu bewertet werden können.

## Sicherheit

DNS-Antworten sind grundsätzlich als nicht vertrauenswürdige Netzwerkdaten zu behandeln.

Validiert werden müssen insbesondere:

```text
Message Length
Transaction State
Record Structure
Names
Record Count
Compression References
Response Association
```

Fehlerhafte DNS-Nachrichten dürfen keine Speicher- oder Ressourcenverletzungen verursachen.

## Datenschutz

DNS-Anfragen können Informationen über Nutzeraktivitäten offenlegen.

NovaOS muss deshalb ermöglichen, Resolver und Transport anhand von Sicherheits- und Datenschutzrichtlinien auszuwählen.

Anwendungen dürfen nicht automatisch systemweite DNS-Aktivitäten anderer Sicherheitskontexte einsehen.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Pending Queries
Cache Entries
Response Size
Record Count
Retries
Resolver State
```

Manipulierte Antworten dürfen keinen unbegrenzten Speicher- oder CPU-Verbrauch verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Configured Resolvers
Resolver State
Cache Usage
Query Statistics
Cache Hits
Cache Misses
Validation Errors
Timeouts
DNSSEC State
```

Konkrete DNS-Anfragen anderer Anwendungen dürfen nur mit entsprechender Berechtigung sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS einen gemeinsamen DNS-Resolver bereitstellen.
2. DNS MUSS IPv4- und IPv6-Adressauflösung unterstützen.
3. DNS-Antworten MÜSSEN als nicht vertrauenswürdige Netzwerkdaten behandelt werden.
4. DNS-Ergebnisse DÜRFEN NICHT als Netzwerkberechtigung interpretiert werden.
5. DNS-Caching MUSS TTL-basierte Gültigkeit berücksichtigen.
6. NovaOS MUSS mehrere DNS-Resolver verwalten können.
7. Resolver-Auswahl SOLL Netzwerk- und Sicherheitskontexte berücksichtigen können.
8. DNS MUSS über UDP und TCP funktionieren können.
9. DNSSEC SOLL unterstützt werden.
10. DNS-Auflösung SOLL asynchron und abbrechbar sein.
11. DNS-Zustände und Cache-Ressourcen MÜSSEN begrenzt sein.
12. Resolver-, Cache- und Validierungszustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-IPV4-0001`
- `NPSPEC-NETWORK-IPV6-0001`
- `NPSPEC-NETWORK-TCP-0001`
- `NPSPEC-NETWORK-UDP-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-FIREWALL-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0012`

## Ergebnis

```text
Application
    ↓
Nova Resolver
    ↓
Cache + Policy + Validation
    ↓
DNS Transport
    ↓
Network Stack
```

NovaOS erhält damit eine zentrale, asynchrone und sicherheitsbewusste DNS-Infrastruktur, die IPv4, IPv6, mehrere Resolver, Caching und DNSSEC in das gemeinsame Netzwerkmodell integriert.
# NPSPEC-NETWORK-NAT-0001 – Nova Network Address Translation

## Status

Angenommen

## Kategorie

Network / NAT / Address Translation

## Zweck

NovaOS definiert Network Address Translation als optionalen Mechanismus zur kontrollierten Übersetzung von Netzwerkadressen und Ports.

```text
Internal Flow
     ↓
NAT Translation
     ↓
External Flow
```

NAT ist kein grundlegender Bestandteil der Netzwerkidentität und darf Sicherheits-, Routing- oder Capability-Mechanismen nicht ersetzen.

## Grundprinzip

```text
NAT ≠ Firewall
NAT ≠ Authorization
Translation ≠ Routing
Address Mapping ≠ Identity
```

NAT verändert Netzwerkrepräsentationen, nicht die logische Identität oder Berechtigung eines Kommunikationspartners.

## NAT-Modell

Eine aktive Übersetzung besitzt mindestens:

```text
NATMapping
├── MappingID
├── Protocol
├── Internal Endpoint
├── External Endpoint
├── Direction
└── State
```

Optional:

```text
Remote Endpoint
Lifetime
FlowID
Interface
Security Context
Policy
```

Mappings müssen eindeutig einem kontrollierten Netzwerkzustand zugeordnet sein.

## NAT-Typen

NovaOS soll mindestens folgende Mechanismen unterstützen können:

```text
Source NAT
Destination NAT
Port Translation
Static Mapping
Dynamic Mapping
```

Source NAT darf beispielsweise für gemeinsam genutzte externe Adressen verwendet werden.

Destination NAT darf eingehenden Verkehr kontrolliert auf interne Ziele abbilden.

## Flow-Integration

NAT arbeitet mit dem Nova Network Flow Model zusammen.

```text
Original Flow
     ↓
NAT Mapping
     ↓
Translated Flow
```

Die Beziehung zwischen ursprünglichem und übersetztem Flow muss nachvollziehbar bleiben.

Eine NAT-Übersetzung darf keine neue Netzwerkberechtigung erzeugen.

## Routing

Routing und NAT bleiben getrennte Mechanismen.

```text
Flow
 ↓
Routing / NAT Policy
 ↓
Translation
 ↓
Network Path
```

Die genaue Verarbeitungsreihenfolge muss für den jeweiligen Netzwerkpfad eindeutig definiert sein.

Routingänderungen dürfen bestehende NAT-Mappings nur weiterverwenden, wenn deren Gültigkeit erhalten bleibt.

## Stateful Translation

Dynamisches NAT darf zustandsbehaftete Mappings erzeugen.

```text
Internal Endpoint
       ↓
Dynamic Mapping
       ↓
External Endpoint
```

Mappings müssen zeitlich und ressourcenseitig begrenzt sein.

Inaktive dynamische Mappings müssen automatisch entfernt werden können.

## Port Translation

Mehrere interne Endpunkte dürfen über Portübersetzung eine externe Adresse gemeinsam verwenden.

```text
10.0.0.2:5000 ─┐
10.0.0.3:5000 ─┼→ External Address
10.0.0.4:5000 ─┘
```

Externe Ports müssen konfliktfrei zugeordnet werden.

Portbereiche und verfügbare Mapping-Ressourcen müssen begrenzbar sein.

## Protokollbehandlung

NAT muss mindestens mit folgenden Transportprotokollen arbeiten können:

```text
TCP
UDP
```

ICMP-bezogene Fehler müssen soweit erforderlich auf bestehende Übersetzungen zurückgeführt werden können.

Protokolle, die Adressen innerhalb ihrer Payload transportieren, dürfen nicht durch generische NAT-Logik blind verändert werden.

## IPv4 und IPv6

Klassisches NAT ist primär für IPv4 vorgesehen.

IPv6 darf grundsätzlich ohne NAT betrieben werden.

```text
IPv4 → NAT optional
IPv6 → Native Routing bevorzugt
```

Spezielle Übersetzungsmechanismen zwischen IPv4 und IPv6 müssen als eigene klar definierte Funktionen behandelt werden und dürfen nicht implizit durch generisches NAT entstehen.

## Firewall-Integration

NAT und Firewall müssen unabhängig bleiben.

```text
NAT Mapping
     ≠
Firewall Permission
```

Ein vorhandenes Mapping darf eingehenden Verkehr nicht automatisch autorisieren.

Firewall-Regeln müssen auf eindeutig definierten Pre- oder Post-Translation-Zuständen arbeiten können.

## Multipath

NAT muss mit mehreren Netzwerkpfaden umgehen können.

Ein Mapping darf an ein Interface oder einen externen Netzwerkpfad gebunden sein.

```text
Mapping
   ↓
External Path
```

Ein Pfadwechsel darf nicht stillschweigend ungültige oder sicherheitswidrige Übersetzungen weiterverwenden.

## Sicherheit

NAT-Konfiguration und statische Mappings müssen autorisiert werden.

Mindestens kontrollierbar sind:

```text
Create Mapping
Remove Mapping
Create Static Forward
Change Translation Policy
Inspect Mapping State
```

Normale Anwendungen dürfen NAT nicht verwenden, um ihre Netzwerkberechtigungen zu erweitern.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Mappings
Port Allocations
Pending Translations
Protocol State
Timeout State
```

Externer Netzwerkverkehr darf keine unbegrenzte Anzahl von NAT-Zuständen erzeugen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
MappingID
Protocol
Internal Endpoint
External Endpoint
Remote Endpoint
FlowID
State
Lifetime
Packet Count
Byte Count
```

Sensible Mapping-Informationen dürfen nur mit entsprechender Berechtigung sichtbar sein.

## Normative Anforderungen

1. NovaOS SOLL NAT als optionalen Netzwerkmechanismus unterstützen.
2. NAT und Firewall MÜSSEN getrennte Mechanismen bleiben.
3. NAT und Routing MÜSSEN logisch getrennt bleiben.
4. NAT DARF KEINE Netzwerkberechtigung erzeugen.
5. Source NAT und Destination NAT SOLLEN unterstützt werden können.
6. Port Translation SOLL für IPv4 unterstützt werden.
7. Dynamische NAT-Mappings MÜSSEN zeitlich und ressourcenseitig begrenzt sein.
8. NAT-Mappings MÜSSEN einem ursprünglichen Netzwerkflow zugeordnet werden können.
9. Vorhandene NAT-Mappings DÜRFEN Firewall-Regeln NICHT umgehen.
10. NAT-Zustände MÜSSEN gegen unkontrolliertes Wachstum geschützt werden.
11. IPv6 SOLL grundsätzlich natives Routing ohne verpflichtendes NAT ermöglichen.
12. NAT-Mappings und Übersetzungsentscheidungen SOLLEN introspektierbar sein.

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
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0011`

## Ergebnis

```text
Original Flow
     ↓
Routing + Policy
     ↓
NAT Translation
     ↓
Firewall Enforcement
     ↓
External / Internal Network
```

NovaOS erhält damit einen optionalen und kontrollierten NAT-Mechanismus für Adress- und Portübersetzung, ohne NAT mit Routing, Firewall, Identität oder Netzwerkberechtigung gleichzusetzen.
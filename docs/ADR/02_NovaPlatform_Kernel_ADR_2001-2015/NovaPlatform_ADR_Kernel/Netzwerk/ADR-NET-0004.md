# ADR-NET-0004 – IPv6

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-NET-0004 |
| Titel | IPv6 |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Netzwerkprotokolle |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-NET-0001, ADR-NET-0002, ADR-NET-0003, ADR-DEV-0002, ADR-DEV-0010 |
| Nachfolgende Spezifikation | NPSPEC-NET-IPV6 |

---

## 1. Kontext

IPv6 ist das langfristige Internetprotokoll für moderne Netzwerke und beseitigt zentrale Einschränkungen des IPv4-Adressraums.

NovaOS benötigt eine native IPv6-Implementierung für:

- globale und lokale Adressierung
- automatische Adresskonfiguration
- Routing
- Multicast
- Neighbor Discovery
- ICMPv6
- Path MTU Discovery
- TCP- und UDP-Kommunikation
- Datenschutzadressen
- Dual-Stack-Betrieb

IPv6 wird nicht als spätere Ergänzung zu IPv4 betrachtet, sondern von Beginn an gleichwertig in die Netzwerkarchitektur integriert.

---

## 2. Problemstellung

IPv6 vereinfacht den Basisheader, führt jedoch zusätzliche Komplexität durch Extension Header, Neighbor Discovery und automatische Konfiguration ein.

Mögliche Risiken sind:

- manipulierte Router Advertisements
- Neighbor-Cache-Erschöpfung
- Extension-Header-Ketten
- Fragmentierungsangriffe
- gefälschte Neighbor Advertisements
- unkontrollierte automatische Routen
- Datenschutzprobleme durch stabile Interface-Identifier
- Multicast-Überlastung

NovaOS benötigt deshalb eine streng validierende und ressourcenbegrenzte IPv6-Implementierung.

---

## 3. Entscheidung

NovaOS implementiert IPv6 als eigenständiges, natives Protokollmodul.

Die Implementierung unterstützt mindestens:

- Unicast
- Anycast entsprechend der Routingkonfiguration
- Multicast
- Link-Local-Adressen
- SLAAC
- statische Adressen
- DHCPv6-Integration
- Neighbor Discovery
- Duplicate Address Detection
- ICMPv6
- Path MTU Discovery
- senderseitige Fragmentierung
- Datenschutzadressen

IPv4 und IPv6 verwenden gemeinsame Socket-, Routing- und Paketfilter-Schnittstellen, bleiben intern jedoch getrennte Parser und Zustandsmaschinen.

---

## 4. IPv6-Basisheader

```c
typedef struct ipv6_header {
    uint32_t version_traffic_flow;
    uint16_t payload_length;
    uint8_t next_header;
    uint8_t hop_limit;
    uint8_t source_address[16];
    uint8_t destination_address[16];
} ipv6_header_t;
```

Der Basisheader besitzt eine feste Länge von 40 Byte.

Netzwerkdaten werden feldweise gelesen. Die Implementierung darf keine Annahmen über Alignment, Padding oder native CPU-Endianness treffen.

---

## 5. Eingangsvalidierung

Vor der weiteren Verarbeitung prüft NovaOS mindestens:

1. IP-Version ist 6.
2. Basisheader ist vollständig vorhanden.
3. Payload Length passt zum Packet Buffer.
4. Quell- und Zieladresse sind für den Pfad zulässig.
5. Extension Header sind syntaktisch gültig.
6. Anzahl und Gesamtlänge der Extension Header bleiben innerhalb der Limits.
7. Transportheader liegt im validierten Paketbereich.
8. Fragmentinformationen sind konsistent.
9. Hop Limit ist für die Verarbeitung ausreichend.
10. Paketfilter erlauben die weitere Verarbeitung.

Ungültige Pakete werden verworfen, bevor TCP, UDP oder andere obere Protokolle sie erhalten.

---

## 6. Adressdarstellung

Eine IPv6-Adresse besteht aus 128 Bit:

```c
typedef struct ipv6_address {
    uint8_t bytes[16];
} ipv6_address_t;
```

Textdarstellungen verwenden die standardisierte Hexadezimalnotation:

```text
2001:db8::42
fe80::1
::1
```

Parser akzeptieren genau eine `::`-Komprimierung und validieren alle Gruppen vollständig.

IPv4-eingebettete Darstellungen werden nur in ausdrücklich unterstützten Kontexten zugelassen.

---

## 7. Adressarten

NovaOS unterscheidet mindestens:

| Adresstyp | Bedeutung |
|---|---|
| Unspecified | `::` |
| Loopback | `::1` |
| Link Local | `fe80::/10` |
| Global Unicast | global routbare Adresse |
| Unique Local | `fc00::/7` |
| Multicast | `ff00::/8` |
| IPv4-mapped | IPv4-Adresse in IPv6-Darstellung |
| Anycast | durch Routingsemantik bestimmte Adresse |

IPv6 besitzt keinen Broadcast. Multicast übernimmt die entsprechenden Aufgaben.

---

## 8. Interface-Adressen

Ein Interface kann mehrere IPv6-Adressen gleichzeitig besitzen.

```c
typedef struct ipv6_interface_address {
    ipv6_address_t address;
    uint8_t prefix_length;
    uint32_t scope_id;
    uint32_t flags;
    uint64_t preferred_until;
    uint64_t valid_until;
} ipv6_interface_address_t;
```

Mögliche Zustände:

- tentative
- preferred
- deprecated
- duplicate
- invalid
- temporary
- stable

Eine tentative Adresse darf nicht für gewöhnliche Verbindungen verwendet werden.

---

## 9. Link-Local-Adressen

Jedes IPv6-fähige Interface erhält grundsätzlich eine Link-Local-Adresse.

Sie wird benötigt für:

- Neighbor Discovery
- Router Discovery
- lokale Kommunikation
- automatische Konfiguration
- bestimmte Routingprotokolle

Link-Local-Adressen benötigen bei mehrdeutiger Verwendung eine `scope_id`, die das zugehörige Interface bestimmt.

Sie dürfen nicht über Router hinweg weitergeleitet werden.

---

## 10. Duplicate Address Detection

Vor der regulären Verwendung einer neuen IPv6-Adresse führt NovaOS Duplicate Address Detection aus.

Der Ablauf verwendet Neighbor Solicitation und Neighbor Advertisement.

Wird ein Konflikt erkannt:

- wechselt die Adresse in den Zustand `duplicate`,
- wird sie nicht für gewöhnlichen Verkehr aktiviert,
- wird der Konfigurationsdienst informiert,
- kann eine neue temporäre oder stabile Adresse erzeugt werden.

DAD wird zeitlich und in der Anzahl paralleler Prüfungen begrenzt.

---

## 11. SLAAC

Stateless Address Autoconfiguration verwendet Router Advertisements und angekündigte Präfixe.

NovaOS verarbeitet dabei:

- Präfixinformationen
- Standardrouter
- Preferred Lifetime
- Valid Lifetime
- MTU
- On-Link-Status
- Managed- und Other-Configuration-Flags

Router Advertisements gelten als nicht vertrauenswürdig und werden vollständig validiert.

Netzwerkprofile können SLAAC deaktivieren oder auf bestimmte Interfaces beschränken.

---

## 12. DHCPv6

DHCPv6 wird durch einen privilegierten Userspace-Netzwerkdienst implementiert.

Es kann verwendet werden für:

- zustandsbehaftete Adressvergabe
- DNS-Server
- zusätzliche Netzwerkoptionen
- Präfixdelegation
- administrativ verwaltete Netzwerke

Der Kernel übernimmt nur validierte Konfigurationen über eine capability-geschützte Schnittstelle.

SLAAC und DHCPv6 können abhängig vom Netzwerkprofil gleichzeitig verwendet werden.

---

## 13. Datenschutzadressen

NovaOS unterstützt temporäre IPv6-Adressen für ausgehende Clientverbindungen.

Ziele:

- Reduzierung langfristiger Geräteverfolgung
- regelmäßiger Wechsel des Interface-Identifier
- Vermeidung hardwarebasierter Adressableitung

NovaOS erzeugt keine globale IPv6-Adresse direkt aus einer öffentlich erkennbaren MAC-Adresse.

Stabile lokale Adressen werden aus einem geschützten, systemspezifischen Geheimnis und dem Netzwerkkontext abgeleitet.

---

## 14. Neighbor Discovery

Neighbor Discovery Protocol ersetzt unter IPv6 unter anderem ARP.

Es übernimmt:

- Auflösung von Link-Layer-Adressen
- Router Discovery
- Präfixerkennung
- Erreichbarkeitsprüfung
- Duplicate Address Detection
- Redirects

Der Neighbor Cache besitzt begrenzte Zustände und Ressourcen.

Ungeprüfte Neighbor-Nachrichten dürfen bestehende gültige Einträge nicht beliebig überschreiben.

---

## 15. Neighbor-Cache-Zustände

NovaOS verwendet mindestens:

| Zustand | Bedeutung |
|---|---|
| `INCOMPLETE` | Link-Layer-Adresse wird ermittelt |
| `REACHABLE` | Nachbar wurde kürzlich bestätigt |
| `STALE` | Eintrag ist vorhanden, aber nicht aktuell bestätigt |
| `DELAY` | erneute Prüfung wird vorbereitet |
| `PROBE` | Erreichbarkeit wird aktiv geprüft |
| `FAILED` | Nachbar ist nicht erreichbar |

Anzahl, Lebensdauer und Probe-Versuche werden begrenzt.

Unbestätigte Einträge dürfen keine unbegrenzten Paketwarteschlangen erzeugen.

---

## 16. Router Advertisements

Router Advertisements werden nur akzeptiert, wenn:

- sie über ein geeignetes Interface eintreffen,
- ihre Quelle eine gültige Link-Local-Adresse ist,
- Hop Limit und ICMPv6-Prüfungen korrekt sind,
- Optionen und Längen gültig sind,
- die Netzwerk- und Sicherheitsrichtlinie sie erlaubt.

NovaOS kann optional Schutzmechanismen wie RA Guard auf verwalteten oder virtuellen Interfaces anwenden.

Ein Router Advertisement darf keine sicherheitskritische Systemrichtlinie überschreiben.

---

## 17. Extension Header

NovaOS unterstützt definierte IPv6 Extension Header, darunter:

- Hop-by-Hop Options
- Destination Options
- Routing Header
- Fragment Header
- Authentication Header, sofern ein Sicherheitsmodul ihn unterstützt

Für Extension Header gelten feste Grenzen:

- maximale Anzahl
- maximale Gesamtlänge
- zulässige Reihenfolge
- maximale Optionszahl
- begrenzte Verarbeitungszeit

Unbekannte oder gefährliche Routing-Header-Typen werden nach Protokollregel verworfen.

---

## 18. Fragmentierung

IPv6-Router fragmentieren keine Pakete.

Nur der Absender darf ein Paket über einen Fragment Header aufteilen.

NovaOS vermeidet Fragmentierung nach Möglichkeit durch Path MTU Discovery und passende Transportsegmentgrößen.

Eingehende Fragmentfolgen werden anhand von Quelle, Ziel, Fragment-ID und Next Header zusammengesetzt.

Überlappende, widersprüchliche oder übergroße Fragmente werden vollständig verworfen.

---

## 19. Path MTU Discovery

ICMPv6 Packet-Too-Big-Nachrichten werden zur Bestimmung der Path MTU verwendet.

Der Stack prüft:

- Bezug zu einem gesendeten Paket oder einer Verbindung
- Plausibilität der gemeldeten MTU
- zulässige Mindest-MTU
- Netzwerkkontext und Interface
- Gültigkeitsdauer des Cacheeintrags

Die minimale IPv6-Link-MTU von 1280 Byte wird entsprechend dem Standard berücksichtigt.

Path-MTU-Einträge laufen aus und können erneut geprüft werden.

---

## 20. ICMPv6

ICMPv6 ist ein zwingender Bestandteil von IPv6 und darf nicht global blockiert werden.

Unterstützt werden mindestens:

- Destination Unreachable
- Packet Too Big
- Time Exceeded
- Parameter Problem
- Echo Request und Reply
- Neighbor Solicitation und Advertisement
- Router Solicitation und Advertisement
- Redirect unter restriktiven Regeln

ICMPv6-Antworten und Fehlerverarbeitung unterliegen Rate Limits.

Firewallregeln müssen notwendige ICMPv6-Typen von optionalen Diagnosemeldungen unterscheiden.

---

## 21. Multicast

IPv6 verwendet Multicast für zahlreiche Kernfunktionen.

Der Stack verwaltet:

- erforderliche Solicited-Node-Gruppen
- Interfacegruppen
- Socket-Mitgliedschaften
- Scope
- Multicast Loopback
- Quellfilterung
- MLD, sofern benötigt

Multicastpakete dürfen ihren zulässigen Scope nicht überschreiten.

Mitgliedschaften werden beim Entfernen einer Adresse, eines Interfaces oder des letzten Sockets beendet.

---

## 22. Routing

IPv6 verwendet die gemeinsame NovaOS-Routinginfrastruktur mit Longest Prefix Match.

Eine Route enthält mindestens:

```c
typedef struct ipv6_route {
    ipv6_address_t prefix;
    uint8_t prefix_length;
    ipv6_address_t next_hop;
    uint64_t interface_id;
    uint32_t metric;
    uint32_t flags;
} ipv6_route_t;
```

Zusätzlich berücksichtigt die Routenauswahl:

- Adressscope
- Quelladressauswahl
- Interfacezustand
- Policy Routing
- Netzwerk-Namespace
- Router-Lebenszeit

Routingänderungen erfolgen atomar.

---

## 23. Quelladressauswahl

Bei mehreren gültigen Adressen wählt NovaOS eine Quelladresse anhand von:

- Adressscope
- Präfixübereinstimmung
- Preferred- oder Deprecated-Status
- temporärer oder stabiler Adressrichtlinie
- expliziter Socketbindung
- Ausgangsinterface
- Sicherheits- und Netzwerkprofil

Normale Anwendungen dürfen keine beliebigen fremden IPv6-Quelladressen verwenden.

---

## 24. Weiterleitung

IPv6-Forwarding ist standardmäßig deaktiviert.

Die Aktivierung benötigt:

- administrative Netzwerk-Capability
- explizit freigegebene Interfaces
- Paketfilterrichtlinie
- Routingkonfiguration
- Hop-Limit-Verarbeitung
- ICMPv6-Fehlerbehandlung

Ein System kann gleichzeitig auf einem Interface als Host und auf einem anderen als Router arbeiten, sofern die Richtlinien dies ausdrücklich vorsehen.

---

## 25. Socket-Integration

IPv6 unterstützt mindestens:

- TCP Stream
- UDP Datagram
- Raw IPv6
- Multicast-Sockets

Socketoptionen können festlegen:

- IPv6-only
- Dual-Stack-Verhalten
- Hop Limit
- Traffic Class
- Multicast-Interface
- Multicast-Scope
- temporäre oder stabile Quelladresspräferenz

Raw IPv6-Sockets benötigen besondere Capabilities und dürfen Paketfilterregeln nicht umgehen.

---

## 26. Dual-Stack

IPv4 und IPv6 werden parallel betrieben.

NovaOS bevorzugt IPv6 nur, wenn:

- eine geeignete Route vorhanden ist,
- die Zieladresse erreichbar erscheint,
- die Sicherheitsrichtlinie dies erlaubt,
- keine bekannte fehlerhafte Konfiguration vorliegt.

Resolver und Verbindungsbibliothek können mehrere Adressen parallel oder zeitlich versetzt testen.

IPv4-mapped IPv6-Sockets sind standardmäßig deaktiviert und müssen ausdrücklich aktiviert werden.

---

## 27. Ressourcenbegrenzung

Begrenzt werden mindestens:

- Neighbor-Cache-Einträge
- unaufgelöste Pakete pro Nachbar
- Fragment-Reassembly
- Extension-Header-Verarbeitung
- Router- und Präfixeinträge
- temporäre Adressen
- Multicast-Mitgliedschaften
- ICMPv6-Antwortrate
- Path-MTU-Cache
- DAD-Vorgänge

Limits gelten pro Interface, Namespace, Prozess und systemweit.

---

## 28. Sicherheit

Für IPv6 gelten folgende Regeln:

- alle Pakete und Extension Header werden grenzgeprüft,
- Router- und Neighbor-Nachrichten gelten als nicht vertrauenswürdig,
- überlappende Fragmente werden verworfen,
- Redirects werden restriktiv behandelt,
- Forwarding ist standardmäßig deaktiviert,
- stabile Adressen enthalten keine direkt abgeleitete MAC-Adresse,
- Neighbor- und Reassembly-Caches sind begrenzt,
- notwendiges ICMPv6 wird nicht pauschal blockiert,
- Raw Sockets benötigen besondere Capabilities,
- Paketfilter gelten auch für automatisch konfigurierte Adressen.

---

## 29. Datenschutz und Diagnose

Lokale Diagnose darf enthalten:

- aktive IPv6-Adressen und Zustände
- Präfixe und Router
- Neighbor-Cache-Status
- DAD-Fehler
- Path-MTU-Einträge
- ICMPv6-Fehler
- verworfene Pakete nach Fehlerklasse
- Fragmentierungsstatistiken

Temporäre Adressen und Neighbor-Daten werden nicht ohne Notwendigkeit dauerhaft gespeichert.

Paketinhalte werden nicht standardmäßig protokolliert.

---

## 30. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_IPV6_HEADER_INVALID` | IPv6-Header ist ungültig |
| `STATUS_EXTENSION_HEADER_INVALID` | Extension Header ist ungültig |
| `STATUS_ADDRESS_NOT_AVAILABLE` | IPv6-Adresse ist nicht verwendbar |
| `STATUS_DUPLICATE_ADDRESS` | DAD hat einen Konflikt erkannt |
| `STATUS_NEIGHBOR_UNREACHABLE` | Nachbar ist nicht erreichbar |
| `STATUS_ROUTE_NOT_FOUND` | keine passende Route vorhanden |
| `STATUS_MESSAGE_TOO_LARGE` | Paket überschreitet die Path MTU |
| `STATUS_REASSEMBLY_FAILED` | Fragmente konnten nicht zusammengesetzt werden |
| `STATUS_HOP_LIMIT_EXPIRED` | Hop Limit wurde aufgebraucht |
| `STATUS_FILTERED` | Paket wurde durch Richtlinie blockiert |
| `STATUS_LIMIT_EXCEEDED` | IPv6-Ressourcenlimit überschritten |

---

## 31. Alternative Ansätze

### 31.1 IPv6 erst nach vollständigem IPv4 implementieren

**Vorteile:**

- geringerer anfänglicher Entwicklungsaufwand

**Nachteile:**

- spätere Architekturänderungen
- unvollständige moderne Netzwerkunterstützung

**Entscheidung:** Abgelehnt.

### 31.2 Automatische globale Adresse aus MAC-Adresse

**Vorteile:**

- einfache stabile Adresserzeugung

**Nachteile:**

- langfristige Geräteverfolgung
- Offenlegung einer Hardwarekennung

**Entscheidung:** Abgelehnt. NovaOS verwendet geschützte stabile und temporäre Adressen.

### 31.3 ICMPv6 vollständig blockieren

**Vorteile:**

- scheinbar kleinere Angriffsfläche

**Nachteile:**

- Neighbor Discovery und Path MTU Discovery funktionieren nicht
- IPv6 wird unzuverlässig

**Entscheidung:** Abgelehnt. ICMPv6 wird typ- und richtlinienbezogen gefiltert.

---

## 32. Konsequenzen

### Positive Konsequenzen

- vollständige moderne Netzwerkadressierung
- native SLAAC- und Neighbor-Discovery-Unterstützung
- Datenschutzadressen
- keine Broadcast-Abhängigkeit
- gleichwertige Integration mit IPv4
- skalierbare Routing- und Socket-Infrastruktur

### Negative Konsequenzen

- komplexe Neighbor- und Routerzustände
- Extension Header erhöhen den Parseraufwand
- Dual-Stack verdoppelt Teile des Testumfangs
- fehlerhafte Netzwerke können automatische Konfiguration beeinflussen

### Risiken

- manipulierte Router Advertisements
- Neighbor-Cache-Erschöpfung
- Fragmentierungsangriffe
- überlange Extension-Header-Ketten
- fehlerhafte Quelladressauswahl
- unzureichend gefilterte Multicastpakete

---

## 33. Implementierungsrichtlinien

- IPv6 ist ein natives und von IPv4 getrenntes Parsermodul.
- Der Basisheader wird vor allen Extension Headern vollständig geprüft.
- Extension Header besitzen feste Anzahl- und Längenlimits.
- Link-Local-Adressen werden pro Interface und Scope behandelt.
- DAD erfolgt vor der regulären Adressnutzung.
- Globale Adressen werden nicht direkt aus MAC-Adressen erzeugt.
- Router Advertisements werden als nicht vertrauenswürdig behandelt.
- Nur Absender fragmentieren IPv6-Pakete.
- Überlappende Fragmente werden vollständig verworfen.
- ICMPv6 wird nicht pauschal blockiert.
- IPv4-mapped IPv6-Sockets sind standardmäßig deaktiviert.

---

## 34. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- IPv6-Basisheader
- Text- und Binäradressdarstellung
- Link-Local-, Global- und Multicast-Adressen
- DAD
- SLAAC und DHCPv6-Integration
- Neighbor Discovery
- Router Advertisements
- Extension Header
- Fragmentierung und Reassembly
- Path MTU Discovery
- ICMPv6
- Dual-Stack
- Quelladressauswahl
- Weiterleitung
- Fuzzing aller IPv6-Parser und Zustandsmaschinen

---

## 35. Verbindliche Invarianten

1. IPv6 ist ein natives, von IPv4 getrenntes Protokollmodul.
2. IPv6 wird architektonisch gleichwertig zu IPv4 unterstützt.
3. Jede Adresse wird vor der regulären Aktivierung auf Konflikte geprüft.
4. Router- und Neighbor-Nachrichten gelten als nicht vertrauenswürdig.
5. Stabile globale Adressen werden nicht direkt aus MAC-Adressen abgeleitet.
6. Extension-Header-Ketten sind streng begrenzt.
7. IPv6-Router fragmentieren keine Pakete.
8. Überlappende Fragmente werden nicht akzeptiert.
9. Notwendiges ICMPv6 darf nicht pauschal blockiert werden.
10. IPv6-Paketinhalte werden nicht standardmäßig protokolliert.

---

## 36. Zusammenfassung

NovaOS implementiert IPv6 als natives und gleichwertiges Netzwerkprotokoll neben IPv4.

Die Implementierung unterstützt SLAAC, DHCPv6-Integration, Neighbor Discovery, ICMPv6, Multicast, Path MTU Discovery und datenschutzfreundliche Adressbildung. Extension Header und Fragment-Reassembly werden streng validiert und begrenzt.

Damit erhält NovaOS eine moderne IPv6-Grundlage, ohne Datenschutz, Ressourcenbegrenzung oder die Sicherheit automatisch gelernter Netzwerkinformationen zu vernachlässigen.
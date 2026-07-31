# ADR-NET-0003 – IPv4

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-NET-0003 |
| Titel | IPv4 |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Netzwerkprotokolle |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-NET-0001, ADR-NET-0002, ADR-DEV-0002, ADR-DEV-0010 |
| Nachfolgende Spezifikation | NPSPEC-NET-IPV4 |

---

## 1. Kontext

IPv4 bleibt für lokale Netzwerke, Internetzugänge, virtuelle Umgebungen und ältere Geräte unverzichtbar.

NovaOS benötigt eine eigene IPv4-Implementierung für:

- Adressierung
- Paketübertragung
- Routing
- Fragmentierung und Reassembly
- ICMP-Fehlerverarbeitung
- Multicast und Broadcast
- automatische und statische Konfiguration
- Integration in TCP, UDP und Socket API

IPv4 wird parallel zu IPv6 unterstützt, aber nicht als bevorzugte Grundlage für alle zukünftigen Netzwerkfunktionen behandelt.

---

## 2. Problemstellung

IPv4 verarbeitet vollständig fremdgesteuerte Pakete und besitzt mehrere historisch gewachsene Sonderfälle.

Zu den Risiken gehören:

- ungültige Headerlängen
- manipulierte Prüfsummen
- Fragmentierungsangriffe
- Adress-Spoofing
- Broadcast-Verstärkung
- Ressourcenerschöpfung
- überlappende Fragmente
- unsichere oder veraltete IP-Optionen

NovaOS benötigt daher eine streng validierende und ressourcenbegrenzte Implementierung.

---

## 3. Entscheidung

NovaOS implementiert IPv4 als eigenständiges, versioniertes Netzwerkprotokollmodul innerhalb des NovaOS Netzwerk-Stacks.

Die Implementierung unterstützt:

- Unicast
- Broadcast
- Multicast
- Routing
- ICMPv4
- Path MTU Discovery
- kontrollierte Fragmentierung
- begrenzte Reassembly
- mehrere Interfaces und Adressen

IPv4 und IPv6 verwenden gemeinsame abstrakte Routing-, Socket- und Filter-Schnittstellen, bleiben intern jedoch getrennte Protokollmodule.

---

## 4. IPv4-Paketheader

Der IPv4-Header wird in Netzwerk-Byte-Reihenfolge verarbeitet.

```c
typedef struct ipv4_header {
    uint8_t version_ihl;
    uint8_t dscp_ecn;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t header_checksum;
    uint32_t source_address;
    uint32_t destination_address;
} ipv4_header_t;
```

Native C-Strukturen dürfen nur als interne Darstellung verwendet werden. Netzwerkdaten werden feldweise und ohne Annahmen über Padding oder Alignment gelesen.

---

## 5. Eingangsvalidierung

Vor jeder weiteren Verarbeitung prüft NovaOS mindestens:

1. IP-Version ist 4.
2. Headerlänge beträgt mindestens 20 Byte.
3. Headerlänge liegt innerhalb des Packet Buffers.
4. Gesamtlänge ist nicht kleiner als die Headerlänge.
5. Gesamtlänge überschreitet den empfangenen Puffer nicht.
6. Headerprüfsumme ist gültig oder verifiziertes Hardware-Offload liegt vor.
7. Fragmentfelder sind konsistent.
8. Optionen sind syntaktisch gültig.
9. Quell- und Zieladresse sind für den Verarbeitungspfad zulässig.
10. Transportheader liegt innerhalb der validierten Paketgrenzen.

Ungültige Pakete werden verworfen, bevor Transportprotokolle sie erhalten.

---

## 6. Adressdarstellung

Eine IPv4-Adresse wird als 32-Bit-Netzwerkadresse dargestellt.

```c
typedef struct ipv4_address {
    uint8_t octets[4];
} ipv4_address_t;
```

Textdarstellungen verwenden die bekannte Punktnotation:

```text
192.168.1.10
```

Parser müssen alle vier Dezimalkomponenten verlangen. Historische Kurzformen, Oktalnotationen und übergroße Zahlen werden in der nativen API nicht akzeptiert.

---

## 7. Adressarten

NovaOS unterscheidet mindestens:

| Adresstyp | Bedeutung |
|---|---|
| Unspecified | `0.0.0.0` |
| Unicast | Adresse eines einzelnen Interfaces oder Hosts |
| Limited Broadcast | `255.255.255.255` |
| Directed Broadcast | Broadcast eines bestimmten Subnetzes |
| Multicast | `224.0.0.0/4` |
| Loopback | `127.0.0.0/8` |
| Link Local | `169.254.0.0/16` |
| Private | RFC1918-Adressbereiche |

Adressklassen werden nicht als Routingmodell verwendet. NovaOS verwendet ausschließlich Präfixlängen und CIDR.

---

## 8. Interface-Adressen

Ein Netzwerkinterface kann mehrere IPv4-Adressen besitzen.

```c
typedef struct ipv4_interface_address {
    ipv4_address_t address;
    uint8_t prefix_length;
    uint32_t flags;
    uint64_t valid_until;
} ipv4_interface_address_t;
```

Eine Adresse kann folgende Zustände besitzen:

- tentative
- active
- deprecated
- duplicate
- expired

Pakete dürfen erst mit einer Adresse gesendet werden, nachdem notwendige Konfliktprüfungen abgeschlossen wurden.

---

## 9. Adresskonfiguration

IPv4-Adressen können konfiguriert werden durch:

- statische Administration
- DHCPv4
- Link-Local-Autokonfiguration
- Recovery- oder Installationsprofile
- virtuelle Netzwerkumgebungen

DHCP wird durch einen Userspace-Netzwerkdienst ausgeführt.

Der Kernel übernimmt ausschließlich validierte Adressen, Präfixe, Routen und Laufzeiten über capability-geschützte Konfigurationsschnittstellen.

---

## 10. Routing

IPv4 verwendet die gemeinsame NovaOS-Routinginfrastruktur.

Die Routenauswahl berücksichtigt:

1. Sicherheits- und Policy-Regeln,
2. Longest Prefix Match,
3. Routingmetrik,
4. Interfacezustand,
5. Quelladressauswahl,
6. optionalen Netzwerk-Namespace.

Eine Route enthält mindestens:

```c
typedef struct ipv4_route {
    ipv4_address_t prefix;
    uint8_t prefix_length;
    ipv4_address_t gateway;
    uint64_t interface_id;
    uint32_t metric;
    uint32_t flags;
} ipv4_route_t;
```

Routingänderungen erfolgen atomar.

---

## 11. Quelladressauswahl

Für ausgehende Pakete wählt NovaOS eine Quelladresse anhand von:

- expliziter Socketbindung
- gewählter Route
- Ausgangsinterface
- Präfixübereinstimmung
- Adresszustand
- Anwendungs- und Sicherheitsrichtlinie

Eine Anwendung darf nur dann eine fremde Quelladresse verwenden, wenn sie eine besondere Raw- oder Spoofing-Capability besitzt.

Normale Sockets dürfen keine beliebigen Absenderadressen erzeugen.

---

## 12. Time to Live

Der TTL-Wert wird bei jedem Routinghop verringert.

Erreicht TTL null, wird das Paket verworfen. Falls zulässig, wird eine ICMP-Time-Exceeded-Nachricht erzeugt.

NovaOS verwendet einen konfigurierbaren sicheren Standardwert für ausgehende Pakete.

Anwendungen dürfen TTL nur innerhalb der durch ihre Socket- und Netzwerkrechte erlaubten Grenzen ändern.

---

## 13. Headerprüfsumme

IPv4 schützt ausschließlich den Header durch eine Internet-Prüfsumme.

Beim Empfang:

- wird die Prüfsumme in Software geprüft oder
- ein vertrauenswürdig markiertes Hardware-Offload-Ergebnis verwendet.

Beim Senden:

- berechnet NovaOS die Prüfsumme oder
- beauftragt ein geeignetes Hardware-Offload.

Offload-Metadaten werden nur von autorisierten und korrekt initialisierten Treibern akzeptiert.

---

## 14. IPv4-Optionen

IPv4-Optionen werden grundsätzlich restriktiv behandelt.

Unterstützt werden nur ausdrücklich freigegebene Optionen.

Gefährliche oder veraltete Optionen wie Source Routing werden standardmäßig verworfen oder ignoriert.

Für jede Optionsliste gelten:

- begrenzte Gesamtlänge
- korrekte Ausrichtung
- vollständige Längenprüfung
- Schleifen- und Überlaufschutz
- keine unkontrollierte Weitergabe an Anwendungen

Raw Sockets dürfen Sicherheitsregeln für Optionen nicht umgehen.

---

## 15. Fragmentierung

NovaOS versucht ausgehende IPv4-Fragmentierung durch Path MTU Discovery zu vermeiden.

Fragmentierung erfolgt nur, wenn:

- das Paket größer als die Ausgangs-MTU ist,
- das `Don't Fragment`-Flag nicht gesetzt ist,
- der Übertragungspfad Fragmentierung erlaubt,
- Ressourcenlimits eingehalten werden.

Transportprotokolle sollen ihre Segmentgröße bevorzugt an die ermittelte Path MTU anpassen.

---

## 16. Fragment-Reassembly

Eingehende Fragmente werden nur innerhalb fester Grenzen zusammengesetzt.

Ein Reassembly-Kontext wird identifiziert durch:

```text
Quelladresse
Zieladresse
Protokoll
Identification
Netzwerkkontext
```

Begrenzt werden:

- parallele Reassembly-Kontexte
- Fragmente pro Paket
- Speicher pro Quelle
- Gesamtspeicher
- Lebenszeit
- maximale rekonstruierte Paketgröße

Überlappende oder widersprüchliche Fragmente werden vollständig verworfen.

---

## 17. Path MTU Discovery

NovaOS verwendet ICMP-Fehlermeldungen und lokale Interfaceinformationen zur Ermittlung der Path MTU.

Path-MTU-Einträge besitzen:

- Zielpräfix
- Interfacebezug
- ermittelte MTU
- Gültigkeitsdauer
- Vertrauensstatus

Unplausible ICMP-MTU-Werte werden begrenzt und validiert.

Ein veralteter Path-MTU-Eintrag darf Verbindungen nicht dauerhaft blockieren.

---

## 18. ICMPv4

ICMPv4 ist Bestandteil der IPv4-Implementierung.

Unterstützt werden mindestens:

- Echo Request und Echo Reply
- Destination Unreachable
- Time Exceeded
- Parameter Problem
- Fragmentation Needed
- Redirect nur unter restriktiven Richtlinien

ICMP-Fehler werden nur akzeptiert, wenn sie plausibel zu einer bestehenden Verbindung oder einem gesendeten Paket gehören.

Antworten unterliegen Rate Limits.

---

## 19. Broadcast

Broadcast-Verkehr wird restriktiv behandelt.

Standardregeln:

- Directed Broadcast Forwarding ist deaktiviert.
- Anwendungen benötigen eine Socketoption und Berechtigung zum Senden von Broadcasts.
- Broadcast-Antworten werden rate-limitiert.
- ungültige Broadcast-Quelladressen werden verworfen.
- Broadcast darf keine Firewall- oder Namespace-Regeln umgehen.

Damit werden Verstärkungs- und Spoofing-Angriffe reduziert.

---

## 20. Multicast

IPv4-Multicast verwendet den Adressbereich `224.0.0.0/4`.

Sockets können kontrolliert Multicastgruppen beitreten oder sie verlassen.

Der Stack verwaltet:

- Gruppenmitgliedschaft pro Interface
- Socketzuordnung
- TTL
- Loopback-Verhalten
- Quellfilterung
- IGMP, sofern unterstützt

Multicast-Mitgliedschaften werden beim Schließen des letzten zugehörigen Sockets entfernt.

---

## 21. Weiterleitung

IPv4-Forwarding ist standardmäßig deaktiviert.

Es kann für Router-, Gateway- oder virtuelle Netzwerkfunktionen aktiviert werden.

Voraussetzungen:

- administrative Netzwerk-Capability
- aktive Paketfilterrichtlinie
- begrenzte Routingressourcen
- TTL-Verarbeitung
- ICMP-Fehlerbehandlung
- explizite Interfacefreigabe

Die Aktivierung einer einzelnen Route aktiviert nicht automatisch globales Forwarding.

---

## 22. Socket-Integration

IPv4 kann mit mindestens folgenden Socket-Typen verwendet werden:

- TCP Stream
- UDP Datagram
- Raw IPv4

Sockets können an folgende Werte gebunden werden:

- konkrete lokale Adresse
- alle lokalen IPv4-Adressen
- bestimmtes Interface
- bestimmter Port

Der Stack muss eingehende Pakete deterministisch einem oder mehreren berechtigten Sockets zuordnen.

Raw Sockets bleiben besonders privilegiert.

---

## 23. Netzwerk-Byte-Reihenfolge

Alle IPv4-Protokollfelder verwenden Netzwerk-Byte-Reihenfolge.

NovaOS stellt explizite Konvertierungsfunktionen bereit:

```c
uint16_t net_to_host_u16(uint16_t value);
uint32_t net_to_host_u32(uint32_t value);
uint16_t host_to_net_u16(uint16_t value);
uint32_t host_to_net_u32(uint32_t value);
```

Parser dürfen keine unausgerichteten Mehrbytezugriffe voraussetzen.

Feldwerte werden erst nach Längenprüfung gelesen.

---

## 24. Ressourcenbegrenzung

IPv4 begrenzt mindestens:

- Fragment-Reassembly-Speicher
- Path-MTU-Cache
- Routingtabellen
- Multicastgruppen
- ICMP-Antwortrate
- ausstehende Neighbor-Auflösungen
- Paketwarteschlangen
- Pakete pro Quelle
- Raw-Socket-Verarbeitung

Limits werden pro Interface, Netzwerk-Namespace, Prozess und systemweit angewendet.

---

## 25. Sicherheit

Für IPv4 gelten folgende Regeln:

- alle Pakete werden vor der Nutzung vollständig validiert,
- überlappende Fragmente werden verworfen,
- Source Routing ist standardmäßig deaktiviert,
- Quelladress-Spoofing wird bei normalen Sockets verhindert,
- Broadcast- und ICMP-Antworten werden begrenzt,
- ungültige Adressarten werden entsprechend dem Verarbeitungspfad abgelehnt,
- Forwarding ist standardmäßig deaktiviert,
- Paketfilter werden vor lokaler Zustellung und Weiterleitung angewendet,
- Raw Sockets benötigen besondere Capabilities.

---

## 26. Datenschutz und Diagnose

Lokale IPv4-Diagnose kann enthalten:

- empfangene und gesendete Pakete
- verworfene Pakete nach Fehlerklasse
- Fragmentierungsstatistiken
- ICMP-Fehler
- Routing- und MTU-Probleme
- Interface-Adressen und Zustände

Paketinhalte und vollständige Verbindungsverläufe werden nicht standardmäßig gespeichert.

Paketmitschnitte benötigen die in ADR-NET-0001 definierte Capture-Capability.

---

## 27. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_IPV4_HEADER_INVALID` | Header ist ungültig |
| `STATUS_IPV4_CHECKSUM_INVALID` | Headerprüfsumme ist falsch |
| `STATUS_ADDRESS_NOT_AVAILABLE` | Adresse ist nicht nutzbar |
| `STATUS_ROUTE_NOT_FOUND` | keine passende Route vorhanden |
| `STATUS_HOST_UNREACHABLE` | Zielhost ist nicht erreichbar |
| `STATUS_MESSAGE_TOO_LARGE` | Paket überschreitet MTU und darf nicht fragmentiert werden |
| `STATUS_REASSEMBLY_FAILED` | Fragmente konnten nicht zusammengesetzt werden |
| `STATUS_TTL_EXPIRED` | TTL ist abgelaufen |
| `STATUS_FILTERED` | Paket wurde durch eine Richtlinie blockiert |
| `STATUS_LIMIT_EXCEEDED` | IPv4-Ressourcenlimit überschritten |

---

## 28. Alternative Ansätze

### 28.1 IPv4 nur über eine Kompatibilitätsschicht

**Vorteile:**

- Fokus auf IPv6
- geringerer nativer Protokollumfang

**Nachteile:**

- viele Netze und Geräte wären nicht direkt erreichbar
- Übersetzungsdienste würden zwingend benötigt

**Entscheidung:** Abgelehnt. IPv4 bleibt ein natives Protokollmodul.

### 28.2 IPv4 und IPv6 in einer gemeinsamen Parserimplementierung

**Vorteile:**

- teilweise weniger doppelter Code

**Nachteile:**

- stark unterschiedliche Header- und Fragmentierungssemantik
- komplexere Sicherheitsprüfung

**Entscheidung:** Gemeinsame Abstraktionen, aber getrennte Protokollparser.

### 28.3 Großzügige Unterstützung historischer IPv4-Optionen

**Vorteile:**

- maximale Altkompatibilität

**Nachteile:**

- größere Angriffsfläche
- geringer praktischer Nutzen

**Entscheidung:** Nur ausdrücklich benötigte und sichere Optionen werden unterstützt.

---

## 29. Konsequenzen

### Positive Konsequenzen

- vollständige Unterstützung bestehender IPv4-Netze
- native Integration in Routing und Socket API
- kontrollierte Fragmentierung und Reassembly
- sichere ICMP- und Broadcast-Verarbeitung
- Unterstützung mehrerer Interfaces und Adressen
- klare Parallelität zu IPv6

### Negative Konsequenzen

- zusätzlicher Protokoll- und Testaufwand neben IPv6
- Fragmentierung erhöht die Komplexität
- IPv4-Altlasten erfordern Sonderregeln
- NAT und private Adressräume bleiben in vielen Netzen notwendig

### Risiken

- Fragmentierungsangriffe
- Adress-Spoofing
- ICMP-Manipulation
- Reassembly-Ressourcenerschöpfung
- fehlerhafte Header-Offloads
- falsche Routing- oder Broadcastkonfiguration

---

## 30. Implementierungsrichtlinien

- IPv4-Header werden feldweise und grenzgeprüft gelesen.
- CIDR ersetzt historische Netzklassen.
- IPv4 und IPv6 bleiben getrennte Parsermodule.
- Source Routing ist standardmäßig deaktiviert.
- Ausgehende Fragmentierung wird möglichst vermieden.
- Überlappende Fragmente werden vollständig verworfen.
- Reassembly und ICMP-Antworten sind rate- und speicherbegrenzt.
- Forwarding ist standardmäßig deaktiviert.
- Raw Sockets dürfen Sicherheitsrichtlinien nicht umgehen.
- Hardwareprüfsummen werden nur von autorisierten Treibern akzeptiert.

---

## 31. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Headerlängen und Gesamtgrößen
- Prüfsummen
- Unicast, Broadcast und Multicast
- mehrere Interface-Adressen
- CIDR-Routing
- TTL
- IPv4-Optionen
- Fragmentierung und Reassembly
- überlappende und fehlende Fragmente
- Path MTU Discovery
- ICMPv4
- Forwarding
- Paketfilterung
- Hardware-Offload
- Fuzzing des vollständigen IPv4-Parsers

---

## 32. Verbindliche Invarianten

1. IPv4 ist ein natives, von IPv6 getrenntes Protokollmodul.
2. Alle Headerfelder werden erst nach vollständiger Längenprüfung gelesen.
3. IPv4 verwendet ausschließlich CIDR-basierte Präfixe.
4. Normale Sockets dürfen keine beliebigen Quelladressen erzeugen.
5. Source Routing ist standardmäßig deaktiviert.
6. Überlappende Fragmente werden nicht akzeptiert.
7. Fragment-Reassembly ist zeitlich und räumlich begrenzt.
8. Forwarding ist standardmäßig deaktiviert.
9. Raw Sockets benötigen besondere Capabilities.
10. IPv4-Paketinhalte werden nicht standardmäßig protokolliert.

---

## 33. Zusammenfassung

NovaOS implementiert IPv4 als natives und streng validiertes Protokollmodul innerhalb des modularen Netzwerk-Stacks.

Die Implementierung unterstützt Unicast, Broadcast, Multicast, CIDR-Routing, ICMPv4, Path MTU Discovery und kontrollierte Fragmentierung. Historische und riskante Funktionen wie Source Routing bleiben standardmäßig deaktiviert.

Feste Ressourcenlimits, frühe Paketvalidierung und capability-geschützte Verwaltungs- und Raw-Socket-Zugriffe reduzieren die Risiken des historisch komplexen IPv4-Protokolls.
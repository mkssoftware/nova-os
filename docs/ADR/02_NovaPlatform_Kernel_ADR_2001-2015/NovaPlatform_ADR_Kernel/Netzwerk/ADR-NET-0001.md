# ADR-NET-0001 – Netzwerk-Stack

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-NET-0001 |
| Titel | Netzwerk-Stack |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Netzwerkarchitektur |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-IPC-0008, ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0009, ADR-DEV-0010 |
| Nachfolgende Spezifikation | NPSPEC-NET-NETWORK-STACK |

---

## 1. Kontext

NovaOS benötigt einen eigenen Netzwerk-Stack für lokale und entfernte Kommunikation.

Der Stack muss mindestens folgende Aufgaben abdecken:

- Verwaltung von Netzwerkschnittstellen
- Ethernet-Kommunikation
- IPv4 und IPv6
- Routing
- TCP und UDP
- ICMP und ICMPv6
- Paketfilterung
- Netzwerkdiagnose
- Integration in den Socket Layer
- Unterstützung physischer und virtueller Netzwerkgeräte

Die Architektur soll sowohl auf älteren Einzelkernsystemen als auch auf modernen Mehrkernrechnern funktionieren.

---

## 2. Problemstellung

Netzwerkprotokolle verarbeiten vollständig fremdgesteuerte Daten. Fehler in Paketparsern können Speicherfehler, Denial-of-Service oder Rechteerweiterungen verursachen.

Gleichzeitig benötigt der Netzwerkpfad:

- geringe Latenz
- hohen Durchsatz
- begrenzte Kopieroperationen
- skalierbare Parallelität
- kontrollierte Ressourcen
- eindeutige Fehler- und Sicherheitsgrenzen

NovaOS muss daher Performance, Isolation und Wartbarkeit miteinander verbinden.

---

## 3. Entscheidung

NovaOS implementiert einen modularen, schichtorientierten Netzwerk-Stack hinter dem capability-basierten Socket Layer.

Die erste Implementierung verwendet ein hybrides Modell:

- zeitkritische Paketannahme und grundlegende Datenpfade laufen kernelnah,
- komplexe Richtlinien-, Konfigurations- und Namensdienste laufen im Userspace,
- Protokollmodule werden über versionierte Schnittstellen getrennt,
- später isolierbare Protokollteile können in Userspace-Dienste verschoben werden.

Anwendungen greifen ausschließlich über Sockets oder ausdrücklich freigegebene Netzwerk-Capabilities auf den Stack zu.

---

## 4. Architektur

```text
Anwendungen
     |
     v
Socket Layer
     |
     v
TCP / UDP / ICMP
     |
     v
IPv4 / IPv6
     |
     v
Routing und Paketfilter
     |
     v
Ethernet und Netzwerkschnittstelle
     |
     v
Netzwerktreiber
```

Konfigurationsdienste wie DHCP, DNS und Netzwerkprofile verwenden RPC und sind nicht Teil des unmittelbaren Kernel-Datenpfads.

---

## 5. Unterstützte Protokolle

Die erste vollständige Netzwerkversion unterstützt mindestens:

| Schicht | Protokolle |
|---|---|
| Link Layer | Ethernet II, ARP |
| Network Layer | IPv4, IPv6, ICMP, ICMPv6 |
| Transport Layer | TCP, UDP |
| Adresskonfiguration | DHCPv4, IPv6 SLAAC |
| Namensauflösung | DNS über separaten Resolverdienst |
| Diagnose | Echo, Fehler- und Erreichbarkeitsmeldungen |

Weitere Protokolle werden über versionierte Module ergänzt.

Veraltete oder unsichere Protokolle werden nicht allein aus Kompatibilitätsgründen im Basissystem aktiviert.

---

## 6. Netzwerkschnittstellen

Jede Netzwerkschnittstelle wird als Device Interface und Netzwerkobjekt repräsentiert.

```c
typedef struct network_interface_info {
    uint64_t interface_id;
    uint32_t interface_type;
    uint32_t state;
    uint32_t mtu;
    uint32_t flags;
    uint64_t link_speed;
} network_interface_info_t;
```

Eine Schnittstelle besitzt mindestens:

- stabile Laufzeit-ID
- benutzerfreundlichen Namen
- Gerätetyp
- Linkzustand
- MTU
- Hardwareadresse, sofern vorhanden
- zugewiesene Netzwerkadressen
- Routing- und Sicherheitskontext
- Statistikzähler

Die Interface-ID stellt keine Zugriffsberechtigung dar.

---

## 7. Schnittstellentypen

NovaOS unterstützt grundsätzlich:

- Ethernet
- WLAN
- Loopback
- virtuelle Adapter
- Tunnel
- Bridge
- VPN-Schnittstellen
- Point-to-Point-Verbindungen

Die erste Implementierung priorisiert Ethernet und Loopback.

WLAN benötigt zusätzlich einen gesonderten, sicherheitskritischen Verwaltungsdienst für Authentifizierung und Verbindungsaufbau.

---

## 8. Paketobjekte

Eingehende und ausgehende Netzwerkdaten werden intern als Packet Buffer dargestellt.

```c
typedef struct packet_buffer_info {
    uint64_t packet_id;
    uint32_t total_length;
    uint32_t protocol;
    uint32_t flags;
    uint32_t segment_count;
} packet_buffer_info_t;
```

Packet Buffer unterstützen:

- mehrere Speichersegmente
- Header-Offsets
- Prüfsummenstatus
- Interface-Zuordnung
- Routing-Metadaten
- Referenzzählung
- kontrollierte DMA-Integration

Paketdaten dürfen nicht durch unkontrollierte Zeiger zwischen Treibern und Protokollmodulen weitergegeben werden.

---

## 9. Empfangspfad

Ein eingehendes Paket durchläuft grundsätzlich:

1. Netzwerktreiber bestätigt den Empfang.
2. DMA-Puffer wird dem Netzwerk-Stack übergeben.
3. grundlegende Länge und Hardwaremetadaten werden geprüft.
4. Link-Layer-Protokoll wird erkannt.
5. Paketfilter und Sicherheitsrichtlinien werden angewendet.
6. Netzwerk- und Transportheader werden validiert.
7. Paket wird einem Socket oder Systemdienst zugeordnet.
8. Puffer wird freigegeben oder erneut verwendet.

Fehlerhafte Pakete werden frühzeitig verworfen.

---

## 10. Sendepfad

Ein ausgehendes Paket durchläuft:

1. Anwendung sendet über einen Socket.
2. Transportprotokoll erzeugt Segment oder Datagramm.
3. Routing bestimmt Ziel und Schnittstelle.
4. Paketfilter prüft die Übertragung.
5. Network- und Link-Layer-Header werden erzeugt.
6. Paket wird an die Sendewarteschlange übergeben.
7. Treiber erzeugt kontrollierte DMA-Mappings.
8. Abschluss wird an den Stack zurückgemeldet.

Teilweise Sendefehler und Verbindungsabbrüche müssen an den Socket Layer weitergegeben werden.

---

## 11. IPv4 und IPv6

IPv4 und IPv6 werden als gleichwertige Netzwerkprotokolle unterstützt.

IPv6 ist kein optionaler späterer Zusatz, sondern Bestandteil der grundlegenden Architektur.

Gemeinsame Funktionen wie:

- Routing
- Firewallregeln
- Socketzuordnung
- Statistik
- Interfaceauswahl

verwenden protokollunabhängige interne Schnittstellen.

Protokollspezifische Besonderheiten bleiben getrennt implementiert.

---

## 12. Routing

NovaOS verwendet versionierte Routingtabellen mit Unterstützung für:

- IPv4- und IPv6-Präfixe
- direkte und Gateway-Routen
- Interfacebindung
- Metriken
- mehrere Routingtabellen
- Policy Routing
- lokale und Broadcast-Routen
- Blackhole- und Reject-Routen

Die Routenauswahl verwendet Longest Prefix Match und anschließend Richtlinie und Metrik.

Routingänderungen erfolgen atomar und benötigen administrative Netzwerk-Capabilities.

---

## 13. TCP

Die TCP-Implementierung unterstützt mindestens:

- Verbindungsaufbau und -abbau
- zuverlässige geordnete Byteübertragung
- Sequenz- und Bestätigungsnummern
- Wiederholungsübertragung
- Flusskontrolle
- Überlastkontrolle
- Selective Acknowledgements
- Path-MTU-Verarbeitung
- Keepalive
- Schutz gegen ungültige Zustandsübergänge

TCP-Zustände und Timer unterliegen festen Ressourcenlimits.

Ein Timeout darf nicht als Garantie interpretiert werden, dass die Gegenstelle keine Daten erhalten hat.

---

## 14. UDP

UDP stellt verbindungslose Datagramme bereit.

Der Stack erhält:

- Datagrammgrenzen
- Quell- und Zieladressen
- Portnummern
- Prüfsummenstatus
- Interfaceinformationen

Datagramme oberhalb zulässiger Größen werden abgelehnt oder nach protokollspezifischen Regeln fragmentiert.

Anwendungen müssen Paketverlust, Duplikate und veränderte Reihenfolge selbst behandeln.

---

## 15. Fragmentierung

IPv4-Fragmentierung wird nur soweit erforderlich unterstützt. Ausgehende Fragmentierung soll durch Path MTU Discovery möglichst vermieden werden.

IPv6-Router fragmentieren keine Pakete. Fragmentierung erfolgt dort ausschließlich durch den Absender.

Der Stack begrenzt:

- Anzahl paralleler Reassemblies
- Speicher pro Quelle
- Fragmentanzahl
- Lebenszeit
- überlappende Fragmente

Mehrdeutige oder überlappende Fragmentfolgen werden verworfen.

---

## 16. Paketfilterung

Der Netzwerk-Stack enthält definierte Filterpunkte für:

- eingehende Pakete
- lokale Zustellung
- Weiterleitung
- ausgehende Pakete
- Socketbindung
- Verbindungsaufbau

Paketfilterregeln können berücksichtigen:

- Protokoll
- Quell- und Zieladresse
- Port
- Interface
- Verbindungstatus
- Anwendung oder Sicherheitsdomäne
- Benutzer- und Sessionkontext

Standardmäßig eingehende Dienste müssen ausdrücklich freigegeben werden.

---

## 17. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `NET_RIGHT_SOCKET_CREATE` | gewöhnliche Sockets erzeugen |
| `NET_RIGHT_CONNECT` | ausgehende Verbindung aufbauen |
| `NET_RIGHT_LISTEN` | eingehende Verbindungen annehmen |
| `NET_RIGHT_BIND_PRIVILEGED` | geschützte Ports binden |
| `NET_RIGHT_RAW` | Raw Sockets verwenden |
| `NET_RIGHT_CONFIGURE` | Interfaces und Adressen konfigurieren |
| `NET_RIGHT_ROUTE` | Routingtabellen verändern |
| `NET_RIGHT_FILTER` | Paketfilter verwalten |
| `NET_RIGHT_INSPECT` | Netzwerkstatus und Statistiken lesen |
| `NET_RIGHT_CAPTURE` | Paketmitschnitt durchführen |

Raw-Zugriff und Paketmitschnitt benötigen besonders geschützte Capabilities.

---

## 18. Konfiguration

Netzwerkkonfiguration wird durch einen privilegierten Userspace-Dienst verwaltet.

Dieser koordiniert:

- statische Adressen
- DHCP
- IPv6 SLAAC
- Standardrouten
- DNS-Konfiguration
- Netzwerkprofile
- Interfaceprioritäten
- Benutzer- und Anwendungsrichtlinien

Der Kernel prüft weiterhin jede Konfigurationsänderung gegen die Capability-Rechte des Dienstes.

Eine fehlerhafte Konfiguration darf den Kernel-Netzwerkpfad nicht beschädigen.

---

## 19. DNS

DNS ist kein Bestandteil des Kernel-Netzwerk-Stacks.

Ein separater Resolverdienst übernimmt:

- DNS-Anfragen
- lokalen Cache
- Suchdomänen
- Hosts-Konfiguration
- DNSSEC-Validierung, sofern aktiviert
- Datenschutzrichtlinien
- Split-DNS und VPN-Kontexte

Anwendungen verwenden eine versionierte Resolver-RPC-Schnittstelle.

DNS-Ergebnisse gelten nicht als dauerhafte oder kryptografisch bestätigte Identität, sofern keine entsprechende Verifikation erfolgt.

---

## 20. Parallelität und Skalierung

Der Stack unterstützt mehrere Empfangs- und Sendewarteschlangen.

Die Verarbeitung kann verteilt werden nach:

- Netzwerkschnittstelle
- Hardwarequeue
- CPU-Affinität
- Protokollfluss
- NUMA-Knoten

Gemeinsame globale Sperren im Paketpfad sind zu vermeiden.

Die Reihenfolge innerhalb eines TCP-Flusses muss erhalten bleiben, während unabhängige Flüsse parallel verarbeitet werden dürfen.

---

## 21. Zero-Copy und Offloading

Der Stack darf kontrollierte Optimierungen verwenden:

- Scatter/Gather-I/O
- Checksum Offload
- TCP Segmentation Offload
- Receive Side Scaling
- Shared-Memory-Puffer
- Zero-Copy-Socketübertragung

Hardware-Offload-Ergebnisse gelten nicht ungeprüft als korrekt.

Bei bekannten Geräte- oder Treiberfehlern muss ein Software-Fallback möglich sein.

Zero-Copy darf Capability-, Speicher- und DMA-Isolation nicht umgehen.

---

## 22. Ressourcenbegrenzung

Begrenzt werden mindestens:

- offene Sockets
- TCP-Verbindungen
- halboffene Verbindungen
- Paketwarteschlangen
- Fragment-Reassembly
- Routingtabelleneinträge
- Neighbor-Cache
- Socketpuffer
- Filterregeln
- Packet Buffer
- Netzwerkressourcen pro Prozess und Job

Überlastung muss kontrolliert zu Paketverlust oder klaren Fehlern führen und nicht zu unbegrenztem Kernelwachstum.

---

## 23. Sicherheit

Für den Netzwerk-Stack gelten folgende Regeln:

- alle externen Pakete gelten als nicht vertrauenswürdig,
- Headerlängen und Offsets werden vollständig validiert,
- Parser arbeiten mit expliziten Puffergrenzen,
- eingehende Verbindungen benötigen Freigaben,
- Raw Sockets und Paketmitschnitt sind privilegiert,
- Netzwerkzugriff kann pro Anwendung beschränkt werden,
- DMA erfolgt ausschließlich über das DMA Framework,
- Protokollzustände und Timer sind begrenzt,
- zufällige Protokollwerte verwenden kryptografisch geeignete Zufallsquellen.

Netzwerkparser müssen regelmäßig fuzz-getestet werden.

---

## 24. Datenschutz und Diagnose

NovaOS sammelt keine externe Netzwerk-Telemetrie als Standardverhalten.

Lokale Diagnose kann enthalten:

- übertragene Byte- und Paketanzahlen
- Fehler und Drops
- Schnittstellenzustände
- Verbindungsstatus
- Protokollstatistiken
- Routing- und Filterentscheidungen

Paketinhalte, DNS-Anfragen und vollständige Verbindungsverläufe werden nicht standardmäßig gespeichert.

Paketmitschnitte benötigen eine ausdrückliche Capture-Capability und sichtbare Benutzer- oder Administratoraktion.

---

## 25. Fehlerbehandlung

Mögliche Netzwerkfehler sind:

| Fehler | Bedeutung |
|---|---|
| `STATUS_NETWORK_UNAVAILABLE` | kein nutzbares Netzwerk vorhanden |
| `STATUS_HOST_UNREACHABLE` | Zielhost nicht erreichbar |
| `STATUS_ROUTE_NOT_FOUND` | keine passende Route |
| `STATUS_CONNECTION_REFUSED` | Gegenstelle lehnt Verbindung ab |
| `STATUS_CONNECTION_RESET` | Verbindung wurde zurückgesetzt |
| `STATUS_NETWORK_TIMEOUT` | Operation überschritt das Zeitlimit |
| `STATUS_PACKET_INVALID` | Paketstruktur ist ungültig |
| `STATUS_MESSAGE_TOO_LARGE` | Nutzlast überschreitet die zulässige Größe |
| `STATUS_FILTERED` | Sicherheitsrichtlinie blockiert den Verkehr |
| `STATUS_LIMIT_EXCEEDED` | Netzwerkressourcenlimit überschritten |

Fehler müssen bis zum betroffenen Socket oder Systemdienst weitergegeben werden.

---

## 26. Alternative Ansätze

### 26.1 Vollständiger monolithischer Kernel-Stack

**Vorteile:**

- kurzer Datenpfad
- klassische Implementierung

**Nachteile:**

- große Kernel-Angriffsfläche
- schwierige Isolation von Protokollfehlern

**Entscheidung:** Abgelehnt. NovaOS verwendet eine modulare hybride Architektur.

### 26.2 Vollständiger Netzwerk-Stack im Userspace

**Vorteile:**

- starke Fehlerisolation
- einfacher aktualisierbar

**Nachteile:**

- höherer IPC- und Kopieraufwand
- komplexerer früher Systemstart

**Entscheidung:** Für einzelne Protokolle und Dienste möglich, aber nicht für die erste vollständige Implementierung verpflichtend.

### 26.3 IPv4 zuerst, IPv6 später

**Vorteile:**

- geringerer Anfangsaufwand

**Nachteile:**

- spätere Architekturumbauten
- unvollständige moderne Netzwerkunterstützung

**Entscheidung:** IPv4 und IPv6 werden von Beginn an architektonisch berücksichtigt.

---

## 27. Konsequenzen

### Positive Konsequenzen

- einheitlicher modularer Netzwerk-Stack
- native Unterstützung für IPv4 und IPv6
- capability-basierte Netzwerkrechte
- skalierbare Mehrkernverarbeitung
- getrennte DNS- und Konfigurationsdienste
- Unterstützung physischer und virtueller Adapter
- kontrollierte Offload- und Zero-Copy-Optimierungen

### Negative Konsequenzen

- hybride Kernel-/Userspace-Architektur ist komplex
- hoher Testaufwand für Protokollzustände
- parallele Verarbeitung benötigt sorgfältige Synchronisation
- ältere Hardware erfordert Software-Fallbacks

### Risiken

- Parserfehler durch manipulierte Pakete
- Ressourcenerschöpfung durch Verbindungs- oder Paketfluten
- Deadlocks im parallelen Datenpfad
- fehlerhafte Hardware-Offloads
- unklare Fehler bei dynamischen Netzwerkwechseln
- Leistungsprobleme durch unnötige Kopieroperationen

---

## 28. Implementierungsrichtlinien

- Anwendungen greifen ausschließlich über Sockets oder freigegebene Netzwerk-Capabilities zu.
- IPv4 und IPv6 verwenden gemeinsame abstrakte Schnittstellen, bleiben aber protokollspezifisch getrennt.
- Alle Paketparser arbeiten mit expliziten Längen und Grenzen.
- DNS und Netzwerkprofile bleiben Userspace-Dienste.
- Globale Sperren im schnellen Paketpfad sind zu vermeiden.
- Ressourcen werden pro Prozess, Job und System begrenzt.
- Hardware-Offloads benötigen einen Software-Fallback.
- Zero-Copy darf keine Speicher- oder Capability-Grenzen umgehen.
- Fehlerhafte Pakete werden möglichst früh verworfen.
- Netzwerk-Nutzdaten werden nicht standardmäßig protokolliert.

---

## 29. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Ethernet und Loopback
- IPv4 und IPv6
- TCP und UDP
- ICMP und ICMPv6
- Routing und mehrere Interfaces
- DHCP und SLAAC
- Paketfilterung
- Fragmentierung und Reassembly
- ungültige und übergroße Pakete
- Ressourcenlimits
- Verbindungsfluten
- Mehrkern- und NUMA-Verarbeitung
- DMA und Netzwerk-Offloads
- Interface-Hotplug
- Fuzzing aller Paketparser

---

## 30. Verbindliche Invarianten

1. Der Socket Layer ist die primäre Anwendungsschnittstelle zum Netzwerk-Stack.
2. IPv4 und IPv6 werden architektonisch gleichwertig unterstützt.
3. Alle externen Netzwerkdaten gelten als nicht vertrauenswürdig.
4. Paketparser verwenden explizite Puffergrenzen.
5. Netzwerkressourcen und Warteschlangen sind immer begrenzt.
6. DNS und Netzwerkprofile werden außerhalb des Kernel-Datenpfads verwaltet.
7. Raw Sockets und Paketmitschnitt benötigen besondere Capabilities.
8. DMA wird ausschließlich über das DMA Framework ausgeführt.
9. Hardware-Offloads dürfen Sicherheitsprüfungen nicht ersetzen.
10. Netzwerk-Nutzdaten werden nicht standardmäßig gespeichert oder übertragen.

---

## 31. Zusammenfassung

NovaOS implementiert einen modularen, hybriden Netzwerk-Stack hinter dem capability-basierten Socket Layer.

Der Stack unterstützt Ethernet, IPv4, IPv6, TCP, UDP, ICMP, Routing und Paketfilterung. Zeitkritische Datenpfade bleiben zunächst kernelnah, während Konfiguration, DHCP, DNS und komplexe Richtlinien durch isolierte Userspace-Dienste verwaltet werden.

Begrenzte Ressourcen, streng validierte Paketparser und kontrollierte DMA- sowie Offload-Pfade bilden die Sicherheitsgrundlage der NovaOS-Netzwerkarchitektur.
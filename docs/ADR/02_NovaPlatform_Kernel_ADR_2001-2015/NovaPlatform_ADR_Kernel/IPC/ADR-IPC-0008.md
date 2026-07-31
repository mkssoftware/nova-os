# ADR-IPC-0008 – Socket Layer

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-IPC-0008 |
| Titel | Socket Layer |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Interprozess- und Netzwerkkommunikation |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-IPC-0001, ADR-IPC-0005, ADR-IPC-0007, ADR-KERNEL-0102, ADR-KERNEL-0103 |
| Nachfolgende Spezifikation | NPSPEC-IPC-SOCKET-LAYER |

---

## 1. Kontext

NovaOS benötigt eine einheitliche Schnittstelle für lokale und netzwerkbasierte Kommunikation.

Anwendungen sollen Daten über unterschiedliche Transportarten austauschen können, ohne direkt mit Netzwerkgeräten, Protokollimplementierungen oder internen Kernelstrukturen arbeiten zu müssen.

Der Socket Layer bildet die gemeinsame Schnittstelle zwischen:

- Anwendungen und Systemdiensten
- lokaler Interprozesskommunikation
- TCP/IP-Protokollen
- verbindungsorientierten Transporten
- datagrammorientierten Transporten
- zukünftigen Netzwerkprotokollen

---

## 2. Problemstellung

Ohne eine gemeinsame Socket-Abstraktion würden unterschiedliche Transportprotokolle jeweils eigene APIs benötigen.

NovaOS muss dabei festlegen:

- welche Socket-Typen unterstützt werden,
- wie lokale und entfernte Adressen dargestellt werden,
- wie Sockets erstellt und verbunden werden,
- wie blockierende Operationen funktionieren,
- wie Sockets in das Capability-Modell integriert werden,
- wie Protokolle erweiterbar bleiben,
- welche Zuständigkeit Kernel und Netzwerkdienste besitzen.

---

## 3. Entscheidung

NovaOS implementiert einen capability-basierten Socket Layer mit einer stabilen, protokollunabhängigen API.

Sockets sind native Kernelobjekte. Die tatsächliche Protokollverarbeitung kann abhängig von Protokoll und Systemkonfiguration im Kernel oder in autorisierten Userspace-Netzwerkdiensten erfolgen.

Für Anwendungen bleibt diese interne Verteilung transparent. Sicherheits- und Fehlergrenzen bleiben jedoch ausdrücklich sichtbar.

---

## 4. Architekturmodell

```text
Anwendung
    |
    v
Socket API
    |
    v
Socket-Kernelobjekt
    |
    +--> Lokaler Transport
    |
    +--> Kernel-Netzwerkstack
    |
    +--> Userspace-Netzwerkdienst
```

Der Socket Layer übernimmt:

- Socket-Lebenszyklus
- Capability-Prüfung
- Adressvalidierung
- Zustandsverwaltung
- Pufferverwaltung
- blockierende Warteoperationen
- Übergabe an die zuständige Protokollimplementierung

---

## 5. Adressfamilien

NovaOS definiert zunächst folgende Adressfamilien:

| Adressfamilie | Bedeutung |
|---|---|
| `SOCKET_AF_LOCAL` | Lokale Kommunikation innerhalb des Systems |
| `SOCKET_AF_IPV4` | IPv4-Kommunikation |
| `SOCKET_AF_IPV6` | IPv6-Kommunikation |
| `SOCKET_AF_UNSPECIFIED` | Noch nicht festgelegte oder automatische Auswahl |

Weitere Adressfamilien können über versionierte Erweiterungen ergänzt werden.

Anwendungen dürfen numerische Adressfamilien nicht mit einer bestimmten internen Protokollimplementierung gleichsetzen.

---

## 6. Socket-Typen

Die erste Implementierung unterstützt:

| Socket-Typ | Verhalten |
|---|---|
| `SOCKET_TYPE_STREAM` | Verbindungsorientierter Bytestrom |
| `SOCKET_TYPE_DATAGRAM` | Verbindungslose, nachrichtenorientierte Datagramme |
| `SOCKET_TYPE_SEQPACKET` | Verbindungsorientiert mit Nachrichtengrenzen |
| `SOCKET_TYPE_RAW` | Direkter Protokollzugriff mit besonderen Rechten |

Raw Sockets sind standardmäßig nicht für gewöhnliche Anwendungen verfügbar.

---

## 7. Protokollauswahl

Ein Socket wird durch Adressfamilie, Typ und Protokoll bestimmt.

Beispiele:

| Adressfamilie | Typ | Protokoll |
|---|---|---|
| IPv4 | Stream | TCP |
| IPv6 | Stream | TCP |
| IPv4 | Datagram | UDP |
| IPv6 | Datagram | UDP |
| Local | Stream | lokaler Streamtransport |
| Local | Seqpacket | lokaler Nachrichtentransport |

Wird kein konkretes Protokoll angegeben, wählt NovaOS das Standardprotokoll für die Kombination aus Adressfamilie und Socket-Typ.

---

## 8. Socket-Erzeugung

```c
status_t socket_create(
    uint32_t address_family,
    uint32_t socket_type,
    uint32_t protocol,
    uint32_t flags,
    cap_handle_t *socket_handle
);
```

Mögliche Flags:

```c
#define SOCKET_FLAG_NONBLOCK  (1u << 0)
#define SOCKET_FLAG_CLOEXEC   (1u << 1)
```

Die Operation erzeugt ein noch nicht gebundenes und nicht verbundenes Socket-Objekt.

---

## 9. Adressdarstellung

Socket-Adressen verwenden einen versionierten, längencodierten Aufbau:

```c
typedef struct socket_address {
    uint16_t length;
    uint16_t family;
    uint8_t data[];
} socket_address_t;
```

Die konkrete Struktur von `data` hängt von der Adressfamilie ab.

Alle Adresslängen müssen vor der protokollspezifischen Verarbeitung validiert werden.

Native C-Pointer und prozessabhängige Daten dürfen nicht Teil gespeicherter Socket-Adressen sein.

---

## 10. Lokale Socket-Adressen

Lokale Sockets werden über capability-geschützte Namespaces veröffentlicht.

Beispiel:

```text
/session/sockets/notification-service
/user/sockets/application-control
```

Ein lokaler Socket-Name ist kein Dateisystempfad und erzeugt keinen gewöhnlichen Dateieintrag.

Die Registrierung und Auflösung erfolgen über die Named-Object-Infrastruktur.

Die Kenntnis des Namens allein erteilt keine Verbindungsberechtigung.

---

## 11. Bind und Listen

Ein Socket kann an eine lokale Adresse gebunden werden:

```c
status_t socket_bind(
    cap_handle_t socket_handle,
    const socket_address_t *address
);
```

Ein verbindungsorientierter Server aktiviert anschließend den Listen-Zustand:

```c
status_t socket_listen(
    cap_handle_t socket_handle,
    uint32_t backlog
);
```

Der Backlog ist begrenzt und darf keine unbegrenzte Anzahl wartender Verbindungen erzeugen.

Das Binden geschützter Ports oder Systemnamen benötigt zusätzliche Rechte.

---

## 12. Verbindung und Annahme

Ein Client baut eine Verbindung auf:

```c
status_t socket_connect(
    cap_handle_t socket_handle,
    const socket_address_t *address,
    uint64_t timeout_ns
);
```

Ein Server nimmt eine Verbindung an:

```c
status_t socket_accept(
    cap_handle_t listening_socket,
    uint32_t flags,
    cap_handle_t *connected_socket,
    socket_address_t *peer_address
);
```

Jede angenommene Verbindung erzeugt ein neues Socket-Objekt mit eigener Capability.

Der Listening Socket bleibt für weitere Verbindungen aktiv.

---

## 13. Datenübertragung

### 13.1 Streams

```c
status_t socket_send(
    cap_handle_t socket_handle,
    const void *buffer,
    uint64_t size,
    uint32_t flags,
    uint64_t *bytes_sent
);

status_t socket_receive(
    cap_handle_t socket_handle,
    void *buffer,
    uint64_t size,
    uint32_t flags,
    uint64_t *bytes_received
);
```

Stream-Sockets erhalten keine Nachrichtengrenzen. Teilweise Lese- und Schreibvorgänge sind zulässig.

### 13.2 Datagramme

```c
status_t socket_send_to(
    cap_handle_t socket_handle,
    const void *buffer,
    uint64_t size,
    const socket_address_t *destination,
    uint32_t flags
);

status_t socket_receive_from(
    cap_handle_t socket_handle,
    void *buffer,
    uint64_t capacity,
    socket_address_t *source,
    uint32_t flags,
    uint64_t *bytes_received
);
```

Datagramm-Sockets erhalten Nachrichtengrenzen.

Ist der Empfangspuffer zu klein, muss die API eindeutig melden, ob das Datagramm abgeschnitten oder vollständig verworfen wurde.

---

## 14. Socket-Zustände

Ein verbindungsorientierter Socket durchläuft definierte Zustände:

```text
CREATED
   |
   +--> BOUND --> LISTENING
   |
   +--> CONNECTING --> CONNECTED
                           |
                           v
                        CLOSING
                           |
                           v
                         CLOSED
```

Ungültige Operationen für den aktuellen Zustand werden mit einem eindeutigen Fehler abgelehnt.

Netzwerkspezifische Unterzustände bleiben innerhalb der Protokollimplementierung.

---

## 15. Blockierendes Verhalten

Socket-Operationen können blockieren bei:

- Verbindungsaufbau
- Annahme einer Verbindung
- leerem Empfangspuffer
- vollem Sendepuffer
- Namens- oder Adressauflösung, sofern ausdrücklich verwendet
- ordnungsgemäßem Schließen einer Verbindung

Nicht blockierende Operationen liefern `STATUS_WOULD_BLOCK`, wenn sie nicht unmittelbar fortgesetzt werden können.

Signals können unterbrechbare Socket-Operationen mit `STATUS_INTERRUPTED` beenden.

---

## 16. Integration in die Wait-API

Sockets sind wartbare Kernelobjekte.

Ein Socket kann unter anderem folgende Zustände signalisieren:

| Zustand | Bedeutung |
|---|---|
| `SOCKET_WAIT_READABLE` | Daten oder neue Verbindung verfügbar |
| `SOCKET_WAIT_WRITABLE` | Daten können gesendet werden |
| `SOCKET_WAIT_CONNECTED` | Verbindungsaufbau abgeschlossen |
| `SOCKET_WAIT_CLOSED` | Gegenstelle oder lokaler Socket geschlossen |
| `SOCKET_WAIT_ERROR` | Asynchroner Fehler vorhanden |

Damit können Anwendungen mehrere Sockets gemeinsam mit Events, Pipes und Message Channels überwachen.

---

## 17. Socket-Optionen

Optionen werden über eine versionierte API gesetzt und gelesen:

```c
status_t socket_set_option(
    cap_handle_t socket_handle,
    uint32_t option_level,
    uint32_t option_name,
    const void *value,
    uint32_t value_size
);

status_t socket_get_option(
    cap_handle_t socket_handle,
    uint32_t option_level,
    uint32_t option_name,
    void *value,
    uint32_t *value_size
);
```

Mögliche Optionen sind:

- Sendepuffergröße
- Empfangspuffergröße
- Verbindungs-Timeout
- Keepalive
- Adresswiederverwendung
- Broadcast-Berechtigung
- IPv6-spezifische Einstellungen
- Protokolloptionen

Unbekannte Optionen müssen mit `STATUS_NOT_SUPPORTED` abgelehnt werden.

---

## 18. Zugriffsrechte

Socket-Capabilities besitzen mindestens folgende Rechte:

| Recht | Bedeutung |
|---|---|
| `SOCKET_RIGHT_CONNECT` | Verbindung aufbauen |
| `SOCKET_RIGHT_BIND` | Socket an Adresse binden |
| `SOCKET_RIGHT_LISTEN` | Listening Socket aktivieren |
| `SOCKET_RIGHT_ACCEPT` | Verbindungen annehmen |
| `SOCKET_RIGHT_SEND` | Daten senden |
| `SOCKET_RIGHT_RECEIVE` | Daten empfangen |
| `SOCKET_RIGHT_CONFIGURE` | Optionen verändern |
| `SOCKET_RIGHT_INSPECT` | Zustand und Metadaten abfragen |
| `SOCKET_RIGHT_TRANSFER` | Socket-Capability weitergeben |

Besondere Operationen wie Raw Sockets, Broadcast oder geschützte Ports benötigen zusätzliche Rechte.

---

## 19. Netzwerkberechtigungen

Der Besitz einer gewöhnlichen Socket-Capability erlaubt nicht automatisch uneingeschränkten Netzwerkzugriff.

NovaOS kann Netzwerkrechte nach folgenden Kriterien begrenzen:

- Adressfamilie
- Protokoll
- lokale und entfernte Adresse
- Portbereich
- eingehende oder ausgehende Verbindung
- Netzwerkschnittstelle
- Benutzer, Session oder Anwendung
- Sicherheitsprofil

Diese Regeln werden bei Socket-Erzeugung, Bind und Connect geprüft.

---

## 20. DNS und Namensauflösung

DNS-Auflösung ist kein Bestandteil des Socket-Kernelobjekts.

Hostnamen werden durch einen gesonderten Resolver-Systemdienst aufgelöst. Dieser verwendet versionierte RPC-Schnittstellen und berücksichtigt:

- lokale Hosts-Konfiguration
- DNS
- Datenschutzrichtlinien
- Cache-Regeln
- Netzwerkkontext
- DNSSEC oder zukünftige Vertrauensmechanismen

Die Socket API arbeitet nach der Auflösung mit konkreten Socket-Adressen.

---

## 21. Lebenszeit und Schließen

Ein Socket wird geschlossen, wenn die letzte gültige Referenz freigegeben wird oder eine ausdrückliche Close-Operation erfolgt.

```c
status_t socket_shutdown(
    cap_handle_t socket_handle,
    uint32_t direction
);
```

Unterstützte Richtungen:

| Wert | Bedeutung |
|---|---|
| `SOCKET_SHUTDOWN_READ` | Weitere Empfänge beenden |
| `SOCKET_SHUTDOWN_WRITE` | Weitere Sendungen beenden |
| `SOCKET_SHUTDOWN_BOTH` | Beide Richtungen beenden |

`shutdown` beendet Kommunikationsrichtungen, zerstört aber nicht zwingend sofort das Socket-Objekt.

---

## 22. Ressourcenbegrenzung

Socket-Ressourcen werden begrenzt:

- Anzahl der Sockets pro Prozess
- Anzahl offener Verbindungen
- Größe der Sende- und Empfangspuffer
- Anzahl wartender Verbindungen
- Anzahl unvollständiger Verbindungsaufbauten
- Datagrammwarteschlangen
- systemweite Netzwerkressourcen

Limits werden zusätzlich über Job Objects und Sicherheitsprofile durchgesetzt.

---

## 23. Sicherheit

Für den Socket Layer gelten folgende Regeln:

- Jeder Zugriff erfolgt über Capabilities.
- Eingehende Adressen und Paketmetadaten werden validiert.
- Userspace-Puffer werden vor dem Zugriff geprüft.
- Größenberechnungen müssen gegen Überläufe geschützt sein.
- Raw Sockets benötigen besondere Berechtigungen.
- Geschützte Ports und Systemnamen dürfen nicht unautorisiert gebunden werden.
- Netzwerkdienste behandeln alle externen Daten als nicht vertrauenswürdig.
- Protokollparser müssen fuzz-getestet werden.
- Socket-Puffer werden vor Wiederverwendung bereinigt.

Lokale Socket-Verbindungen dürfen nicht automatisch als vertrauenswürdig gelten.

---

## 24. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_WOULD_BLOCK` | Operation würde blockieren |
| `STATUS_TIMEOUT` | Zeitlimit überschritten |
| `STATUS_INTERRUPTED` | Operation wurde unterbrochen |
| `STATUS_INVALID_HANDLE` | Ungültige Socket-Capability |
| `STATUS_ACCESS_DENIED` | Erforderliches Recht fehlt |
| `STATUS_ADDRESS_IN_USE` | Adresse ist bereits gebunden |
| `STATUS_ADDRESS_UNREACHABLE` | Zieladresse ist nicht erreichbar |
| `STATUS_CONNECTION_REFUSED` | Verbindung wurde abgelehnt |
| `STATUS_CONNECTION_RESET` | Verbindung wurde zurückgesetzt |
| `STATUS_NOT_CONNECTED` | Socket ist nicht verbunden |
| `STATUS_ALREADY_CONNECTED` | Socket ist bereits verbunden |
| `STATUS_PROTOCOL_NOT_SUPPORTED` | Protokoll wird nicht unterstützt |
| `STATUS_MESSAGE_TOO_LARGE` | Datagramm ist zu groß |
| `STATUS_NETWORK_UNAVAILABLE` | Netzwerk ist nicht verfügbar |
| `STATUS_OBJECT_CLOSED` | Socket wurde geschlossen |

---

## 25. Audit und Datenschutz

Audit-Ereignisse können erzeugt werden bei:

- verweigerten Netzwerkverbindungen
- Verwendung von Raw Sockets
- Binden geschützter Ports
- wiederholten Protokollverletzungen
- Netzwerkzugriff geschützter Systemdienste
- Überschreitung von Ressourcenlimits
- Änderungen administrativer Netzwerkoptionen

Nutzdaten werden nicht standardmäßig protokolliert.

Lokale Telemetrie darf nur notwendige Metadaten wie Protokoll, Ergebnis, Datenmenge und grobe Zeitangaben erfassen. Personenbezogene Netzwerkdaten müssen minimiert und geschützt werden.

---

## 26. Alternative Ansätze

### 26.1 Vollständig POSIX-kompatible Socket-API

**Vorteile:**

- bekannte Programmierschnittstelle
- einfache Portierung vieler Anwendungen

**Nachteile:**

- dateideskriptorzentriertes Sicherheitsmodell
- historisch gewachsene Sonderfälle
- unzureichende Integration in Capabilities

**Entscheidung:** NovaOS bietet eine eigene native Socket-API. Eine POSIX-Kompatibilitätsschicht kann darauf aufbauen.

### 26.2 Gesamter Netzwerkstack im Kernel

**Vorteile:**

- kurze Kommunikationswege
- klassische Implementierung

**Nachteile:**

- große Angriffsfläche im Kernel
- Protokollfehler gefährden das Gesamtsystem

**Entscheidung:** Nicht verbindlich vorgeschrieben. Der Socket Layer unterstützt Kernel- und Userspace-Protokollkomponenten.

### 26.3 Gesamter Netzwerkstack im Userspace

**Vorteile:**

- stärkere Fehlerisolation
- Dienste können separat aktualisiert werden

**Nachteile:**

- höherer IPC-Aufwand
- komplexere Datenpfade

**Entscheidung:** Für geeignete Protokolle bevorzugt, aber nicht für jede Hardware und Startphase verpflichtend.

---

## 27. Konsequenzen

### Positive Konsequenzen

- einheitliche API für lokale und entfernte Kommunikation
- capability-basierte Zugriffskontrolle
- Unterstützung für Stream- und Datagrammprotokolle
- Integration in die allgemeine Wait-Infrastruktur
- Möglichkeit zur Isolation von Netzwerkprotokollen im Userspace
- Grundlage für POSIX-Kompatibilität

### Negative Konsequenzen

- komplexe Zustands- und Fehlerverwaltung
- zusätzlicher Aufwand für Kernel-/Userspace-Protokollgrenzen
- Netzwerkberechtigungen erfordern eine eigene Richtlinienverwaltung
- teilweise Lese- und Schreibvorgänge müssen korrekt behandelt werden

### Risiken

- Fehler in Protokollparsern
- Ressourcenerschöpfung durch Verbindungsfluten
- Race Conditions beim Schließen
- unzureichende Rechteprüfung
- Leistungsprobleme bei zu vielen Kopieroperationen

---

## 28. Implementierungsrichtlinien

- Adressfamilie, Socket-Typ und Protokoll müssen getrennt validiert werden.
- Socket-Zustandsübergänge müssen atomar sein.
- Lokale Namen dürfen kein globales Zugriffsrecht darstellen.
- Netzwerkberechtigungen müssen vor Bind und Connect geprüft werden.
- Teilweise Send- und Empfangsoperationen sind ausdrücklich zu unterstützen.
- Datagrammgrenzen müssen erhalten bleiben.
- Anwendungen dürfen keine internen Protokollobjekte sehen.
- Protokollfehler dürfen keine ungültigen Zwischenzustände hinterlassen.
- Socket-Optionen müssen versioniert und größenvalidiert werden.

---

## 29. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- lokale Stream- und Datagramm-Sockets
- IPv4- und IPv6-Sockets
- Bind, Listen, Connect und Accept
- blockierende und nicht blockierende Operationen
- Teilübertragungen
- Datagrammgrenzen
- Timeout und Signalunterbrechung
- Shutdown und Close
- Netzwerkberechtigungen
- geschützte Ports
- Ressourcenlimits
- Integration in die Wait-API
- Server- und Clientabstürze
- parallele Verbindungen
- Fuzzing von Adressen und Protokollnachrichten

---

## 30. Verbindliche Invarianten

1. Sockets sind capability-geschützte Kernelobjekte.
2. Adressfamilie, Socket-Typ und Protokoll sind voneinander getrennte Eigenschaften.
3. Stream-Sockets erhalten keine Nachrichtengrenzen.
4. Datagramm-Sockets erhalten Nachrichtengrenzen.
5. Lokale Socket-Namen erteilen keine Zugriffsberechtigung.
6. Raw Sockets benötigen besondere Rechte.
7. Socket-Puffer und Verbindungswarteschlangen sind begrenzt.
8. DNS-Auflösung erfolgt außerhalb des Socket-Kernelobjekts.
9. Netzwerk- und lokale Daten gelten grundsätzlich als nicht vertrauenswürdig.
10. Socket-Nutzdaten werden nicht standardmäßig protokolliert.

---

## 31. Zusammenfassung

NovaOS implementiert einen capability-basierten Socket Layer als gemeinsame Schnittstelle für lokale und netzwerkbasierte Kommunikation.

Die Architektur unterstützt Stream-, Datagramm- und nachrichtenorientierte Sockets sowie IPv4, IPv6 und lokale Adressen. Anwendungen verwenden eine stabile Socket-API, unabhängig davon, ob ein Protokoll im Kernel oder durch einen isolierten Userspace-Dienst verarbeitet wird.

Netzwerkberechtigungen, begrenzte Ressourcen und die Integration in die Wait-Infrastruktur sorgen für ein kontrolliertes und erweiterbares Kommunikationsmodell.
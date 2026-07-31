# ADR-NET-0002 – Socket API

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-NET-0002 |
| Titel | Socket API |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Netzwerk-API und Anwendungs-ABI |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-NET-0001, ADR-IPC-0008, ADR-KERNEL-0103, ADR-KERNEL-0105, ADR-SEC-0008 |
| Nachfolgende Spezifikation | NPSPEC-NET-SOCKET-API |

---

## 1. Kontext

Der NovaOS Netzwerk-Stack benötigt eine stabile Programmierschnittstelle für Anwendungen und Systemdienste.

Die Socket API soll unter anderem ermöglichen:

- TCP-Verbindungen
- UDP-Datagramme
- lokale Socket-Kommunikation
- IPv4 und IPv6
- Server-Sockets
- blockierende und nicht blockierende I/O
- asynchrone Netzwerkoperationen
- Integration in die allgemeine Wait-API

ADR-IPC-0008 definiert die interne Socket-Abstraktion. Diese ADR legt die native, öffentlich verwendbare Netzwerk-API fest.

---

## 2. Problemstellung

Die API muss einfach verwendbar sein, darf aber Netzwerkfehler und Prozessgrenzen nicht verbergen.

Zusätzlich muss sie:

- mit dem Capability-Modell arbeiten,
- 32-Bit- und 64-Bit-Anwendungen unterstützen,
- versionierbar bleiben,
- Adressfamilien erweiterbar darstellen,
- Timeouts und Abbruch unterstützen,
- teilweise Übertragungen eindeutig melden,
- eine POSIX-Kompatibilitätsschicht ermöglichen.

---

## 3. Entscheidung

NovaOS stellt eine native, capability-basierte Socket API bereit.

Jeder Socket wird durch ein Kernelobjekt und eine Capability repräsentiert. Numerische POSIX-Dateideskriptoren sind nicht Bestandteil der nativen API, können aber durch eine Kompatibilitätsschicht auf Socket-Capabilities abgebildet werden.

Die API verwendet explizite Größen, Versionen, Flags und Statuscodes.

---

## 4. Grundmodell

```text
Anwendung
    |
    v
Native Socket API
    |
    v
Socket-Capability
    |
    v
Socket Layer
    |
    v
Netzwerk-Stack oder lokaler Transport
```

Anwendungen erhalten keinen direkten Zugriff auf interne Protokollobjekte, Paketpuffer oder Netzwerktreiber.

---

## 5. Socket-Erzeugung

```c
status_t net_socket_create(
    uint32_t address_family,
    uint32_t socket_type,
    uint32_t protocol,
    uint32_t flags,
    cap_handle_t *socket_handle
);
```

Adressfamilien:

```c
#define NET_AF_UNSPECIFIED  0
#define NET_AF_LOCAL        1
#define NET_AF_IPV4         2
#define NET_AF_IPV6         3
```

Socket-Typen:

```c
#define NET_SOCKET_STREAM     1
#define NET_SOCKET_DATAGRAM   2
#define NET_SOCKET_SEQPACKET  3
#define NET_SOCKET_RAW        4
```

Raw Sockets benötigen eine besondere Netzwerk-Capability.

---

## 6. Erzeugungsflags

```c
#define NET_SOCKET_NONBLOCK  (1u << 0)
#define NET_SOCKET_CLOEXEC   (1u << 1)
#define NET_SOCKET_ASYNC     (1u << 2)
```

`CLOEXEC` ist für nicht ausdrücklich vererbbare Sockets der sichere Standard.

Flags dürfen nach der Erzeugung nur über definierte Konfigurationsoperationen verändert werden.

---

## 7. Adressdarstellung

Socket-Adressen verwenden eine gemeinsame, längencodierte Struktur:

```c
typedef struct net_socket_address {
    uint16_t structure_size;
    uint16_t family;
    uint32_t scope_id;
    uint8_t address_data[];
} net_socket_address_t;
```

Das Format von `address_data` hängt von der Adressfamilie ab.

Alle Strukturen sind unabhängig von nativen Zeigergrößen und Padding-Regeln.

---

## 8. IPv4-Adresse

```c
typedef struct net_ipv4_address {
    uint16_t structure_size;
    uint16_t family;
    uint16_t port;
    uint16_t reserved;
    uint8_t address[4];
} net_ipv4_address_t;
```

Portnummern und Netzwerkadressen werden in einer eindeutig dokumentierten Byte-Reihenfolge gespeichert.

Anwendungen dürfen nicht von der nativen CPU-Endianness ausgehen.

---

## 9. IPv6-Adresse

```c
typedef struct net_ipv6_address {
    uint16_t structure_size;
    uint16_t family;
    uint16_t port;
    uint16_t flags;
    uint32_t flow_label;
    uint32_t scope_id;
    uint8_t address[16];
} net_ipv6_address_t;
```

`scope_id` wird insbesondere für linklokale IPv6-Adressen verwendet.

IPv4-mapped IPv6-Adressen dürfen nur verwendet werden, wenn die Socket-Konfiguration dies ausdrücklich erlaubt.

---

## 10. Bind

```c
status_t net_socket_bind(
    cap_handle_t socket_handle,
    const net_socket_address_t *local_address
);
```

`bind` ordnet dem Socket eine lokale Adresse und gegebenenfalls einen Port zu.

Der Kernel prüft:

- Adressfamilie
- lokale Interfacezuordnung
- Portberechtigung
- Adressverfügbarkeit
- Netzwerk- und Anwendungsrichtlinie
- aktuellen Socket-Zustand

Das Binden geschützter Ports benötigt `NET_RIGHT_BIND_PRIVILEGED`.

---

## 11. Listen

```c
status_t net_socket_listen(
    cap_handle_t socket_handle,
    uint32_t backlog
);
```

Ein Listening Socket nimmt keine regulären Streamdaten entgegen. Er verwaltet ausschließlich ankommende Verbindungsanfragen.

Der Backlog wird durch System- und Prozesslimits begrenzt.

Ein übergroßer Wert wird reduziert oder mit `STATUS_LIMIT_EXCEEDED` abgelehnt.

---

## 12. Connect

```c
status_t net_socket_connect(
    cap_handle_t socket_handle,
    const net_socket_address_t *remote_address,
    uint64_t timeout_ns
);
```

Im blockierenden Modus wartet die Operation auf Erfolg, Fehler oder Timeout.

Im nicht blockierenden Modus kann sie `STATUS_IN_PROGRESS` liefern. Der Abschluss wird anschließend über die Wait-API oder den asynchronen Fehlerstatus abgefragt.

Ein Timeout beweist nicht, dass die Gegenstelle keine Daten oder Verbindungsanfrage erhalten hat.

---

## 13. Accept

```c
status_t net_socket_accept(
    cap_handle_t listening_socket,
    uint32_t flags,
    cap_handle_t *connected_socket,
    net_socket_address_t *peer_address,
    uint32_t *peer_address_size
);
```

Jede angenommene Verbindung erzeugt eine neue Socket-Capability.

Die neue Capability erhält nur die für eine verbundene Sitzung erforderlichen Rechte.

Der Listening Socket bleibt für weitere Verbindungen geöffnet.

---

## 14. Streamübertragung

```c
status_t net_socket_send(
    cap_handle_t socket_handle,
    const void *buffer,
    uint64_t size,
    uint32_t flags,
    uint64_t *bytes_sent
);

status_t net_socket_receive(
    cap_handle_t socket_handle,
    void *buffer,
    uint64_t capacity,
    uint32_t flags,
    uint64_t *bytes_received
);
```

Bei Stream-Sockets gilt:

- Nachrichtengrenzen bleiben nicht erhalten.
- Teilübertragungen sind zulässig.
- Ein erfolgreicher Send-Aufruf bestätigt nicht den Empfang durch die Gegenstelle.
- Null empfangene Bytes können ein geordnetes Verbindungsende anzeigen.

---

## 15. Datagrammübertragung

```c
status_t net_socket_send_to(
    cap_handle_t socket_handle,
    const void *buffer,
    uint64_t size,
    const net_socket_address_t *destination,
    uint32_t flags
);

status_t net_socket_receive_from(
    cap_handle_t socket_handle,
    void *buffer,
    uint64_t capacity,
    net_socket_address_t *source,
    uint32_t *source_size,
    uint32_t flags,
    uint64_t *bytes_received
);
```

Datagrammgrenzen bleiben erhalten.

Ist der Zielpuffer zu klein, liefert die API die tatsächlich verfügbare Datagrammgröße und einen eindeutigen Truncation-Status.

Ein Datagramm darf nicht stillschweigend als vollständige Nachricht dargestellt werden, wenn es gekürzt wurde.

---

## 16. Scatter/Gather-I/O

Für effiziente Übertragungen unterstützt NovaOS mehrere Buffersegmente:

```c
typedef struct net_io_vector {
    void *base;
    uint64_t length;
} net_io_vector_t;
```

```c
status_t net_socket_send_vector(
    cap_handle_t socket_handle,
    const net_io_vector_t *vectors,
    uint32_t vector_count,
    uint32_t flags,
    uint64_t *bytes_sent
);
```

Anzahl, Gesamtgröße und Adressbereiche der Segmente werden vollständig validiert.

Scatter/Gather erteilt keinen direkten DMA-Zugriff auf Userspace-Speicher.

---

## 17. Flags für Send und Receive

Mögliche Operationsflags:

```c
#define NET_MSG_DONT_WAIT       (1u << 0)
#define NET_MSG_PEEK            (1u << 1)
#define NET_MSG_WAIT_ALL        (1u << 2)
#define NET_MSG_MORE            (1u << 3)
#define NET_MSG_NO_SIGNAL       (1u << 4)
#define NET_MSG_TRUNCATE_REPORT (1u << 5)
```

`WAIT_ALL` garantiert nicht, dass die angeforderte Größe bei Verbindungsende oder Fehler vollständig erreicht wird.

`PEEK` liest Daten, ohne sie aus der Empfangswarteschlange zu entfernen.

---

## 18. Shutdown und Close

```c
status_t net_socket_shutdown(
    cap_handle_t socket_handle,
    uint32_t direction
);
```

Richtungen:

```c
#define NET_SHUTDOWN_READ   1
#define NET_SHUTDOWN_WRITE  2
#define NET_SHUTDOWN_BOTH   3
```

`shutdown` beendet eine oder beide Kommunikationsrichtungen.

Das Schließen der letzten Socket-Capability gibt das Socket-Objekt frei, sobald keine internen oder asynchronen Referenzen mehr bestehen.

Ein geordneter Protokollabbau und die Freigabe des Kernelobjekts sind getrennte Vorgänge.

---

## 19. Socket-Optionen

```c
status_t net_socket_set_option(
    cap_handle_t socket_handle,
    uint32_t option_level,
    uint32_t option_name,
    const void *value,
    uint32_t value_size
);

status_t net_socket_get_option(
    cap_handle_t socket_handle,
    uint32_t option_level,
    uint32_t option_name,
    void *value,
    uint32_t *value_size
);
```

Unterstützte Optionen können umfassen:

- Sendepuffergröße
- Empfangspuffergröße
- Timeouts
- Keepalive
- Adresswiederverwendung
- Broadcast
- IPv6-only
- TCP-spezifische Optionen
- Interfacebindung
- Traffic Class

Optionen dürfen Sicherheitsrichtlinien nicht umgehen.

---

## 20. Nicht blockierender Betrieb

Ein Socket im nicht blockierenden Modus wartet nicht auf:

- eingehende Daten
- freien Sendepuffer
- Verbindungsannahme
- Abschluss eines Connect
- Protokollabschluss

Stattdessen liefert die Operation:

```text
STATUS_WOULD_BLOCK
```

oder bei einem laufenden Verbindungsaufbau:

```text
STATUS_IN_PROGRESS
```

Die Anwendung verwendet anschließend die allgemeine Wait-API.

---

## 21. Wait-Integration

Sockets sind wartbare Kernelobjekte.

Unterstützte Zustände sind mindestens:

| Zustand | Bedeutung |
|---|---|
| `NET_WAIT_READABLE` | Daten oder Verbindungsende vorhanden |
| `NET_WAIT_WRITABLE` | Senden ist möglich |
| `NET_WAIT_ACCEPTABLE` | Verbindung kann angenommen werden |
| `NET_WAIT_CONNECTED` | Connect ist abgeschlossen |
| `NET_WAIT_ERROR` | asynchroner Fehler vorhanden |
| `NET_WAIT_CLOSED` | Socket oder Gegenstelle wurde geschlossen |

Readiness bedeutet nicht, dass eine nachfolgende Operation unter konkurrierenden Zugriffen garantiert erfolgreich bleibt.

---

## 22. Asynchrone Operationen

NovaOS unterstützt asynchrone Netzwerk-I/O über Completion-Objekte.

```c
status_t net_socket_send_async(
    cap_handle_t socket_handle,
    const net_async_buffer_t *buffer,
    uint32_t flags,
    cap_handle_t *completion_handle
);
```

Eine asynchrone Operation besitzt:

- eigene Bufferreferenzen
- Deadline
- Abbruchstatus
- Ergebnis
- übertragene Byteanzahl
- Socketreferenz

Das Schließen der aufrufenden Socket-Capability bricht bereits angenommene Operationen nicht automatisch ab.

---

## 23. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `SOCKET_RIGHT_BIND` | lokale Adresse binden |
| `SOCKET_RIGHT_CONNECT` | Verbindung aufbauen |
| `SOCKET_RIGHT_LISTEN` | Socket in Listen-Zustand versetzen |
| `SOCKET_RIGHT_ACCEPT` | Verbindungen annehmen |
| `SOCKET_RIGHT_SEND` | Daten senden |
| `SOCKET_RIGHT_RECEIVE` | Daten empfangen |
| `SOCKET_RIGHT_CONFIGURE` | erlaubte Optionen ändern |
| `SOCKET_RIGHT_INSPECT` | Zustand und Adressen lesen |
| `SOCKET_RIGHT_DUPLICATE` | Capability duplizieren |
| `SOCKET_RIGHT_TRANSFER` | Capability übertragen |

Besondere Netzwerkrechte wie Raw Access oder geschützte Ports werden zusätzlich durch Netzwerk-Capabilities kontrolliert.

---

## 24. Duplizierung und Übertragung

Socket-Capabilities können mit reduzierten Rechten dupliziert und übertragen werden.

Mehrere Capabilities desselben Socket-Objekts teilen:

- Verbindungszustand
- Sende- und Empfangspuffer
- Socket-Optionen
- Fehlerstatus
- Shutdown-Zustand

Das Schließen einer Capability schließt nicht automatisch das Socket-Objekt, solange weitere Referenzen bestehen.

Die Übertragung an einen anderen Prozess ist eine ausdrückliche Delegation.

---

## 25. Netzwerkberechtigungen

Die Socket API prüft neben Socket-Rechten auch Netzwerkprofile.

Diese können beschränken:

- erlaubte Adressfamilien
- lokale und entfernte Adressen
- Portbereiche
- eingehende und ausgehende Verbindungen
- Netzwerkschnittstellen
- Broadcast und Multicast
- Raw Sockets
- Hintergrundnetzwerkzugriff

Anwendungen dürfen durch das Übertragen einer Socket-Capability keine übergeordneten Systemrichtlinien umgehen.

---

## 26. Namensauflösung

Hostnamen werden nicht direkt durch `connect` aufgelöst.

Anwendungen verwenden den Resolverdienst:

```c
status_t net_resolve_host(
    cap_handle_t resolver,
    const char *hostname,
    const char *service,
    const net_resolve_hints_t *hints,
    net_resolve_result_t *result
);
```

Die Trennung ermöglicht:

- asynchrone Auflösung
- DNSSEC
- VPN- und Sessionkontexte
- Datenschutzrichtlinien
- lokale Namensquellen
- getrennte Fehlerbehandlung

Die Socket API selbst arbeitet mit konkreten Netzwerkadressen.

---

## 27. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_WOULD_BLOCK` | Operation würde blockieren |
| `STATUS_IN_PROGRESS` | asynchrone Zustandsänderung läuft |
| `STATUS_TIMEOUT` | Zeitlimit überschritten |
| `STATUS_INTERRUPTED` | Operation wurde unterbrochen |
| `STATUS_ACCESS_DENIED` | erforderliches Recht fehlt |
| `STATUS_ADDRESS_IN_USE` | lokale Adresse ist belegt |
| `STATUS_ROUTE_NOT_FOUND` | keine Route zum Ziel |
| `STATUS_CONNECTION_REFUSED` | Verbindung wurde abgelehnt |
| `STATUS_CONNECTION_RESET` | Verbindung wurde zurückgesetzt |
| `STATUS_NOT_CONNECTED` | Socket ist nicht verbunden |
| `STATUS_ALREADY_CONNECTED` | Socket ist bereits verbunden |
| `STATUS_MESSAGE_TOO_LARGE` | Datagramm ist zu groß |
| `STATUS_MESSAGE_TRUNCATED` | Datagramm wurde gekürzt |
| `STATUS_NETWORK_UNAVAILABLE` | Netzwerk ist nicht verfügbar |
| `STATUS_SOCKET_CLOSED` | Socket wurde geschlossen |

---

## 28. Sicherheit und Datenschutz

Für die Socket API gelten folgende Regeln:

- Jeder Socket-Zugriff benötigt eine Capability.
- Adressen und Buffer werden vollständig validiert.
- Socket-IDs und Portnummern erteilen keine Rechte.
- Raw Sockets und Paketmitschnitt bleiben privilegiert.
- Optionen können Systemrichtlinien nicht abschwächen.
- Userspace-Pointer werden nicht über den Systemaufruf hinaus ungeprüft gespeichert.
- Asynchrone Buffer werden kontrolliert referenziert oder gepinnt.
- Netzwerkdaten werden nicht standardmäßig protokolliert.
- Close-on-Execute verhindert unbeabsichtigte Vererbung.

---

## 29. POSIX-Kompatibilität

Eine POSIX-Schicht darf native Socket-Capabilities auf numerische File Descriptors abbilden.

Sie übersetzt unter anderem:

- `socket`
- `bind`
- `listen`
- `accept`
- `connect`
- `send`
- `recv`
- `select`
- `poll`
- `close`

Die native NovaOS-API bleibt jedoch unabhängig von POSIX-Datentypen, globalen Fehlernummern und historisch bedingten Strukturgrößen.

POSIX-Kompatibilität darf die nativen Capability-Prüfungen nicht umgehen.

---

## 30. Alternative Ansätze

### 30.1 POSIX-Sockets als native API

**Vorteile:**

- einfache Portierung bestehender Software
- weit verbreitetes Modell

**Nachteile:**

- dateideskriptorzentriert
- historisch gewachsene ABI
- schwächere Capability-Integration

**Entscheidung:** POSIX wird als Kompatibilitätsschicht angeboten.

### 30.2 Unterschiedliche APIs für TCP, UDP und lokale Sockets

**Vorteile:**

- protokollspezifisch klare Schnittstellen

**Nachteile:**

- mehr APIs und doppelte Mechanismen
- schlechter erweiterbar

**Entscheidung:** Eine gemeinsame Socket API mit sichtbarer Protokollsemantik wird verwendet.

### 30.3 Ausschließlich asynchrone API

**Vorteile:**

- gute Skalierbarkeit

**Nachteile:**

- unnötig komplex für einfache Anwendungen

**Entscheidung:** Blockierende, nicht blockierende und asynchrone Modi werden unterstützt.

---

## 31. Konsequenzen

### Positive Konsequenzen

- stabile native Netzwerk-ABI
- capability-basierte Socket-Rechte
- Unterstützung für IPv4, IPv6 und lokale Sockets
- blockierender, nicht blockierender und asynchroner Betrieb
- Integration in die allgemeine Wait-API
- klare Datagramm- und Streamsemantik
- Grundlage für POSIX-Kompatibilität

### Negative Konsequenzen

- zusätzliche native API neben der POSIX-Schicht
- asynchrone Bufferlebenszeiten sind komplex
- Capability-Übertragung benötigt sorgfältige Richtlinien
- verschiedene Socket-Modi erhöhen den Testaufwand

### Risiken

- unbeabsichtigte Socket-Vererbung
- Race Conditions bei Close und asynchroner I/O
- falsch behandelte Teilübertragungen
- Datagrammtrunkierung
- Umgehung von Netzwerkprofilen durch Delegation
- ABI-Inkompatibilität bei unzureichender Versionierung

---

## 32. Implementierungsrichtlinien

- Native Sockets werden durch Capabilities repräsentiert.
- Alle öffentlichen Strukturen besitzen Größen- oder Versionsinformationen.
- Stream- und Datagrammsemantik bleiben eindeutig getrennt.
- Teilübertragungen werden immer über die tatsächliche Byteanzahl gemeldet.
- Nicht blockierende Operationen verwenden eindeutige Statuscodes.
- Socketoptionen dürfen Sicherheitsrichtlinien nicht umgehen.
- Hostnamen werden durch den Resolverdienst aufgelöst.
- Close-on-Execute ist für nicht ausdrücklich vererbte Sockets Standard.
- Asynchrone Operationen halten sichere Objekt- und Bufferreferenzen.
- POSIX-Kompatibilität wird auf die native API abgebildet.

---

## 33. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- IPv4-, IPv6- und lokale Sockets,
- Stream- und Datagrammkommunikation,
- Bind, Listen, Connect und Accept,
- blockierende und nicht blockierende Operationen,
- asynchrone I/O,
- Teilübertragungen,
- Datagrammtrunkierung,
- Wait-Zustände,
- Shutdown und Close,
- Capability-Duplizierung und -Übertragung,
- Close-on-Execute,
- Netzwerkprofile,
- ungültige Adressen und Buffer,
- 32-Bit-/64-Bit-Kompatibilität,
- POSIX-Abbildung.

---

## 34. Verbindliche Invarianten

1. Native NovaOS-Sockets werden durch Capabilities repräsentiert.
2. Numerische File Descriptors gehören ausschließlich zur Kompatibilitätsschicht.
3. Stream-Sockets erhalten keine Nachrichtengrenzen.
4. Datagramm-Sockets erhalten Nachrichtengrenzen.
5. Teilübertragungen müssen eindeutig gemeldet werden.
6. Ein erfolgreicher Send-Aufruf bestätigt nicht den Empfang durch die Gegenstelle.
7. Socketoptionen dürfen Netzwerk- und Sicherheitsrichtlinien nicht abschwächen.
8. Namensauflösung erfolgt außerhalb der Socket-Kernel-API.
9. Socket-Übertragung ist eine ausdrückliche Capability-Delegation.
10. Netzwerkdaten werden nicht standardmäßig protokolliert.

---

## 35. Zusammenfassung

NovaOS stellt eine native, capability-basierte Socket API für IPv4, IPv6 und lokale Kommunikation bereit.

Die API unterstützt Stream- und Datagramm-Sockets, blockierende, nicht blockierende und asynchrone Operationen sowie die allgemeine Wait-Infrastruktur. Explizite Größen, Statuscodes und Capability-Rechte bilden eine stabile 32-Bit-/64-Bit-ABI.

Eine POSIX-Kompatibilitätsschicht kann bekannte File-Descriptor-Sockets anbieten, ohne das native Sicherheits- und Objektmodell von NovaOS zu verändern.
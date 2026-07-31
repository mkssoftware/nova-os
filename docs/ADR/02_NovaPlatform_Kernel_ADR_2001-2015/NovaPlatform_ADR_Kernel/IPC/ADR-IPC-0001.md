# ADR-IPC-0001 – Message Passing

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-IPC-0001 |
| Titel | Message Passing |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Interprozesskommunikation |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-PROC-0009, ADR-PROC-0010, ADR-SEC-0006, ADR-SEC-0008 |
| Nachfolgende Spezifikation | NPSPEC-IPC-MESSAGE-PASSING |

---

## 1. Kontext

NovaOS benötigt einen einheitlichen Mechanismus für den strukturierten Nachrichtenaustausch zwischen Prozessen, Systemdiensten und ausgewählten Kernelkomponenten.

Message Passing soll insbesondere für folgende Aufgaben verwendet werden:

- Kommunikation zwischen Anwendungen und Systemdiensten
- Implementierung von Client-Server-Protokollen
- Übertragung kleiner Steuer- und Statusnachrichten
- Ereignisbenachrichtigungen
- Aufbau höherer IPC-Abstraktionen
- kontrollierte Kommunikation über Sicherheitsgrenzen hinweg

Der Mechanismus muss mit dem Capability-Modell und der Prozessisolation von NovaOS vereinbar sein.

---

## 2. Problemstellung

Direkter Speicherzugriff zwischen Prozessen würde die Isolation schwächen und komplexe Synchronisationsprobleme verursachen.

Ein ausschließlich streambasierter IPC-Mechanismus wäre für strukturierte Betriebssystemnachrichten ebenfalls unzureichend, weil Nachrichtengrenzen, Typinformationen und die kontrollierte Übergabe von Kernelobjekten benötigt werden.

NovaOS benötigt daher einen Mechanismus, der:

- Nachrichtengrenzen erhält,
- Absender und Empfänger eindeutig bestimmt,
- Zugriffsrechte erzwingt,
- blockierende und nicht blockierende Operationen unterstützt,
- begrenzte Nachrichtenwarteschlangen verwendet,
- Capability-Übergaben kontrolliert ermöglicht.

---

## 3. Entscheidung

NovaOS implementiert Message Passing über kernelverwaltete, bidirektionale Message Channels.

Ein Channel besteht aus zwei verbundenen Endpunkten. Jeder Endpunkt ist ein Kernelobjekt und wird ausschließlich über eine gültige Capability angesprochen.

Nachrichten werden atomar übertragen. Entweder wird eine vollständige Nachricht in die Warteschlange des Empfängers eingestellt oder die Operation schlägt fehl beziehungsweise blockiert.

---

## 4. Ziele

Die Architektur verfolgt folgende Ziele:

- sichere Kommunikation zwischen isolierten Prozessen
- Erhaltung von Nachrichtengrenzen
- vorhersehbares Verhalten bei Überlastung
- kontrollierte Übertragung von Capabilities
- Unterstützung synchroner und asynchroner Kommunikationsmuster
- Vermeidung unbegrenzter Kernelpuffer
- einheitliche Schnittstelle für Anwendungen und Systemdienste

---

## 5. Nichtziele

Diese ADR definiert nicht:

- gemeinsam genutzten Speicher
- Netzwerkkommunikation
- persistente Nachrichtenwarteschlangen
- verteilte Nachrichtenvermittlung
- Remote Procedure Calls auf Protokollebene
- die Serialisierung komplexer Anwendungsobjekte

RPC und Service Discovery können später auf Message Channels aufbauen.

---

## 6. Channel-Modell

Ein Message Channel besitzt zwei Endpunkte:

```text
Prozess A                    Prozess B
+------------+              +------------+
| Endpoint A | <----------> | Endpoint B |
+------------+              +------------+
```

Nachrichten, die über Endpoint A gesendet werden, erscheinen in der Empfangswarteschlange von Endpoint B und umgekehrt.

Eigenschaften eines Endpunkts:

- eindeutige Kernelobjekt-ID
- begrenzte Empfangswarteschlange
- definierte Zugriffsrechte
- Referenz auf den verbundenen Endpunkt
- Zustand `OPEN`, `CLOSING` oder `CLOSED`
- optionale Zuordnung zu einem Audit-Kontext

---

## 7. Nachrichtenformat

Jede Nachricht enthält einen festen Header und eine variable Nutzlast.

```c
typedef struct ipc_message_header {
    uint32_t protocol_id;
    uint32_t message_type;
    uint32_t flags;
    uint32_t payload_size;
    uint64_t correlation_id;
} ipc_message_header_t;
```

| Feld | Bedeutung |
|---|---|
| `protocol_id` | Identifiziert das verwendete Protokoll |
| `message_type` | Protokollspezifischer Nachrichtentyp |
| `flags` | Steuer- und Verarbeitungsoptionen |
| `payload_size` | Größe der Nutzlast |
| `correlation_id` | Zuordnung von Antworten zu Anfragen |

Der Kernel interpretiert die Nutzlast grundsätzlich nicht. Die Validierung protokollspezifischer Inhalte liegt beim Empfänger.

---

## 8. Nachrichtengröße

Die maximale Inline-Nachrichtengröße wird begrenzt.

Für die erste NovaOS-Implementierung gelten folgende Standardwerte:

| Eigenschaft | Standardwert |
|---|---:|
| Maximale Inline-Nutzlast | 16 KiB |
| Maximale Capability-Anhänge | 8 |
| Standardtiefe einer Warteschlange | 64 Nachrichten |
| Maximale konfigurierbare Tiefe | 1024 Nachrichten |

Größere Datenmengen sollen später über Shared Memory oder andere geeignete Transfermechanismen übertragen werden. Die Nachricht enthält dann nur Metadaten und die erforderliche Capability.

---

## 9. Capability-Übergabe

Nachrichten dürfen optional Capabilities enthalten.

Bei erfolgreicher Übertragung erzeugt der Kernel im Capability-Space des Empfängers neue Capability-Einträge. Die übertragenen Rechte dürfen die Rechte des Absenders nicht überschreiten.

Unterstützte Modi:

- `COPY`: Der Absender behält seine Capability.
- `MOVE`: Die Capability wird beim Absender entfernt.
- `REDUCE`: Der Empfänger erhält eine Capability mit reduzierten Rechten.

Beispiel:

```c
typedef struct ipc_cap_transfer {
    cap_handle_t source;
    uint32_t transfer_mode;
    uint32_t requested_rights;
} ipc_cap_transfer_t;
```

Die Capability-Übergabe ist Teil der atomaren Sendeoperation. Schlägt ein Teil der Übertragung fehl, werden weder Nachricht noch Capabilities zugestellt.

---

## 10. Zugriffsrechte

Ein Channel-Endpunkt unterstützt mindestens folgende Rechte:

| Recht | Bedeutung |
|---|---|
| `IPC_RIGHT_SEND` | Nachrichten senden |
| `IPC_RIGHT_RECEIVE` | Nachrichten empfangen |
| `IPC_RIGHT_TRANSFER` | Capabilities anhängen |
| `IPC_RIGHT_INSPECT` | Metadaten des Endpunkts abfragen |
| `IPC_RIGHT_CLOSE` | Endpunkt schließen |
| `IPC_RIGHT_DUPLICATE` | Capability für den Endpunkt duplizieren |

Der Kernel prüft die Rechte bei jeder Operation.

---

## 11. API

Die grundlegende Kernel-API umfasst:

```c
status_t ipc_channel_create(
    cap_handle_t *endpoint_a,
    cap_handle_t *endpoint_b
);

status_t ipc_send(
    cap_handle_t endpoint,
    const ipc_message_t *message,
    uint32_t flags,
    uint64_t timeout_ns
);

status_t ipc_receive(
    cap_handle_t endpoint,
    ipc_message_t *message,
    uint32_t flags,
    uint64_t timeout_ns
);

status_t ipc_channel_close(
    cap_handle_t endpoint
);
```

Optionale Flags:

```c
#define IPC_FLAG_NONBLOCK       (1u << 0)
#define IPC_FLAG_PEEK           (1u << 1)
#define IPC_FLAG_INTERRUPTIBLE  (1u << 2)
```

`IPC_FLAG_PEEK` darf keine angehängten Capabilities in den Empfängerprozess übertragen.

---

## 12. Blockierendes Verhalten

### 12.1 Senden

Ist die Empfangswarteschlange des verbundenen Endpunkts voll, kann der Absender:

- blockieren,
- mit Timeout warten,
- im nicht blockierenden Modus sofort einen Fehler erhalten.

### 12.2 Empfangen

Ist keine Nachricht vorhanden, kann der Empfänger:

- blockieren,
- mit Timeout warten,
- im nicht blockierenden Modus sofort zurückkehren.

Blockierte Threads werden über den NovaOS-Scheduler verwaltet. Priority Inheritance ist für gewöhnliche Message Channels nicht automatisch aktiviert.

---

## 13. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_WOULD_BLOCK` | Operation würde blockieren |
| `STATUS_TIMEOUT` | Zeitlimit überschritten |
| `STATUS_ACCESS_DENIED` | Erforderliches Recht fehlt |
| `STATUS_INVALID_HANDLE` | Ungültige Capability |
| `STATUS_MESSAGE_TOO_LARGE` | Nachricht überschreitet die Größenbegrenzung |
| `STATUS_QUEUE_FULL` | Warteschlange ist voll |
| `STATUS_PEER_CLOSED` | Gegenstelle wurde geschlossen |
| `STATUS_INVALID_TRANSFER` | Capability-Übergabe ist ungültig |
| `STATUS_INTERRUPTED` | Warteoperation wurde unterbrochen |

---

## 14. Schließen eines Endpunkts

Wird ein Endpunkt geschlossen:

1. werden neue Sendeoperationen über diesen Endpunkt abgelehnt,
2. wird der verbundene Endpunkt über das Schließen informiert,
3. dürfen bereits zugestellte Nachrichten noch gelesen werden,
4. liefern spätere Operationen nach Leerung der Warteschlange `STATUS_PEER_CLOSED`.

Das Schließen eines Prozesses schließt automatisch alle Endpunkte, für die keine weiteren gültigen Referenzen existieren.

---

## 15. Überlastung und Ressourcenbegrenzung

Alle Nachrichtenwarteschlangen sind begrenzt. Unbegrenztes Wachstum von Kernelpuffern ist nicht zulässig.

Der Kernel berücksichtigt bei der Channel-Erstellung und beim Nachrichtentransfer:

- Prozesslimits
- Job-Object-Limits
- globale Kernelgrenzen
- Größe der Nachricht
- Anzahl angehängter Capabilities

Ein Prozess darf durch das Erzeugen vieler Channels oder großer Warteschlangen nicht unbegrenzt Kernelspeicher reservieren.

---

## 16. Sicherheit

Der Message-Passing-Mechanismus folgt diesen Sicherheitsregeln:

- Kommunikation ist nur mit gültigen Capabilities möglich.
- Der Kernel vertraut keinen Längenangaben aus dem Userspace.
- Nachrichten werden vor der Verarbeitung vollständig validiert und kopiert.
- Kernelzeiger werden niemals an den Empfänger weitergegeben.
- Capability-Rechte dürfen bei einer Übertragung nicht erweitert werden.
- Nachrichtenpuffer werden vor einer erneuten Verwendung bereinigt.
- Geschlossene oder widerrufene Endpunkte dürfen nicht weiterverwendet werden.

Systemdienste müssen alle empfangenen Nutzdaten als nicht vertrauenswürdig behandeln.

---

## 17. Audit

Normale Nachrichteninhalte werden nicht protokolliert.

Audit-Ereignisse können für sicherheitsrelevante Vorgänge erzeugt werden:

- fehlgeschlagene Capability-Übertragungen
- wiederholte Zugriffsverletzungen
- Überschreitung von Ressourcenlimits
- Kommunikation mit besonders geschützten Systemdiensten
- administrative Inspektion eines Channels

Audit-Datensätze enthalten nur notwendige Metadaten wie Prozess-ID, Endpunkt-ID, Operation, Ergebnis und Zeitstempel.

---

## 18. Performance

Für kleine Nachrichten wird die Nutzlast über kernelverwaltete Puffer kopiert.

Der typische Datenpfad lautet:

1. Nutzlast aus dem Absenderadressraum validieren und kopieren.
2. Nachricht in die Zielwarteschlange einfügen.
3. wartenden Empfänger aufwecken.
4. Nutzlast beim Empfang in den Empfängeradressraum kopieren.

Eine spätere Optimierung darf direkte Übergaben an bereits wartende Empfänger oder speziell kontrollierte Zero-Copy-Verfahren einführen, sofern die Sicherheits- und Atomaritätsgarantien erhalten bleiben.

---

## 19. Alternative Ansätze

### 19.1 Nur Shared Memory

**Vorteile:**

- effizient bei großen Datenmengen
- wenige Kopieroperationen

**Nachteile:**

- komplexe Synchronisation
- schwächere Isolation bei fehlerhafter Nutzung
- keine natürlichen Nachrichtengrenzen
- ungeeignet als alleiniger IPC-Mechanismus

**Entscheidung:** Nicht als grundlegender Message-Passing-Mechanismus gewählt.

### 19.2 Globale benannte Mailboxen

**Vorteile:**

- einfache Service-Adressierung
- lose Kopplung

**Nachteile:**

- zusätzlicher globaler Namensraum
- komplexere Rechteverwaltung
- höheres Risiko für Namenskonflikte und Spoofing

**Entscheidung:** Service Discovery wird getrennt definiert.

### 19.3 Unbegrenzte Warteschlangen

**Vorteile:**

- Absender blockieren seltener

**Nachteile:**

- unkontrollierter Speicherverbrauch
- einfache Denial-of-Service-Möglichkeit

**Entscheidung:** Abgelehnt.

---

## 20. Konsequenzen

### Positive Konsequenzen

- einheitliche und sichere IPC-Grundlage
- klare Nachrichtengrenzen
- kontrollierbare Ressourcenbelegung
- Capability-basierte Autorisierung
- geeignet für Client-Server-Protokolle
- erweiterbar für RPC und Service Discovery

### Negative Konsequenzen

- mindestens eine Kopieroperation pro Übertragungsrichtung
- zusätzlicher Kernelaufwand für Warteschlangen
- begrenzte Inline-Nachrichtengröße
- Anwendungen müssen Überlastung und Timeouts behandeln

### Risiken

- Deadlocks bei zyklisch blockierenden Protokollen
- Priority Inversion zwischen Client und Server
- Protokollfehler durch unzureichende Eingabevalidierung
- Ressourcenerschöpfung durch viele wartende Nachrichten

Diese Risiken müssen durch Protokolldesign, Timeouts, Ressourcenlimits und Tests reduziert werden.

---

## 21. Implementierungsrichtlinien

- Alle Größen- und Offsetberechnungen müssen auf Überläufe geprüft werden.
- Nachrichten dürfen erst nach vollständiger Validierung sichtbar werden.
- Capability-Übertragungen müssen transaktional ausgeführt werden.
- Warteschlangen müssen gegen konkurrierende Zugriffe geschützt sein.
- Das Aufwecken wartender Threads darf nicht zu verlorenen Signalen führen.
- Fehlerpfade müssen alle temporären Referenzen und Puffer freigeben.
- Nachrichteninhalte dürfen nicht ohne ausdrückliche Sicherheitsentscheidung geloggt werden.

---

## 22. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Channel-Erstellung und -Schließung
- bidirektionales Senden und Empfangen
- Erhaltung der Nachrichtenreihenfolge
- blockierende und nicht blockierende Operationen
- Timeouts und Unterbrechungen
- volle Warteschlangen
- maximale Nachrichtengrößen
- Capability-Übertragung mit `COPY`, `MOVE` und `REDUCE`
- Rechteverletzungen
- Schließen der Gegenstelle
- Prozessbeendigung während einer IPC-Operation
- konkurrierende Sender und Empfänger
- Ressourcenlimitierung
- Fuzzing von Nachrichtenheadern und Übergabedeskriptoren

---

## 23. Verbindliche Invarianten

1. Nachrichten werden niemals teilweise zugestellt.
2. Nachrichtengrenzen bleiben erhalten.
3. Empfangswarteschlangen sind immer begrenzt.
4. Ohne `IPC_RIGHT_SEND` darf keine Nachricht gesendet werden.
5. Ohne `IPC_RIGHT_RECEIVE` darf keine Nachricht empfangen werden.
6. Capability-Rechte dürfen bei einer Übertragung nicht erweitert werden.
7. Nachricht und Capability-Anhänge werden gemeinsam oder gar nicht übertragen.
8. Der Kernel interpretiert keine protokollspezifische Nutzlast.
9. Das Schließen eines Endpunkts wird für die Gegenstelle eindeutig erkennbar.
10. Nachrichteninhalte werden standardmäßig nicht protokolliert.

---

## 24. Zusammenfassung

NovaOS verwendet kernelverwaltete, bidirektionale Message Channels als grundlegenden Mechanismus für strukturiertes Message Passing.

Jeder Channel besteht aus zwei capability-geschützten Endpunkten mit begrenzten Empfangswarteschlangen. Nachrichten werden atomar übertragen, behalten ihre Grenzen und können kontrolliert Capabilities transportieren.

Der Mechanismus bildet die Grundlage für sichere Systemdienste, Ereignisübertragung, Serviceprotokolle und spätere RPC-Abstraktionen.
# ADR-IPC-0007 – Pipes

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-IPC-0007 |
| Titel | Pipes |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Interprozesskommunikation und Datenströme |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-IPC-0001, ADR-IPC-0005, ADR-PROC-0001, ADR-KERNEL-0102, ADR-KERNEL-0103 |
| Nachfolgende Spezifikation | NPSPEC-IPC-PIPES |

---

## 1. Kontext

NovaOS benötigt neben nachrichtenorientierter IPC einen einfachen Mechanismus für unstrukturierte, sequenzielle Datenströme zwischen Prozessen.

Typische Anwendungsfälle sind:

- Verkettung von Kommandozeilenprogrammen
- Weiterleitung von Standardausgabe
- Übergabe von Standardeingabe
- Kommunikation zwischen Eltern- und Kindprozessen
- Streaming von Text- und Binärdaten
- Kompatibilität mit Unix-ähnlichen Anwendungen

Pipes stellen einen geordneten Bytestrom bereit. Sie erhalten keine logischen Nachrichtengrenzen.

---

## 2. Problemstellung

NovaOS muss festlegen:

- ob Pipes uni- oder bidirektional sind,
- wie Lese- und Schreibenden behandelt werden,
- wann Operationen blockieren,
- wie End-of-File signalisiert wird,
- wie parallele Schreibzugriffe behandelt werden,
- wie Puffer und Ressourcen begrenzt werden,
- wie Pipe-Endpunkte an neue Prozesse übergeben werden.

---

## 3. Entscheidung

NovaOS implementiert anonyme, unidirektionale Pipes als native Kernelobjekte.

Jede Pipe besitzt zwei getrennte Endpunkte:

- Read End
- Write End

```text
Schreibender Prozess                 Lesender Prozess
+-----------+      Bytestrom        +----------+
| Write End | --------------------> | Read End |
+-----------+                       +----------+
```

Bidirektionale Kommunikation verwendet zwei Pipes oder einen Message Channel.

Benannte Pipes werden bei Bedarf als Service über Named Objects aufgebaut und nicht als gesonderter globaler Pipe-Namensraum implementiert.

---

## 4. Datenmodell

Eine Pipe transportiert eine geordnete Folge von Bytes.

Eigenschaften:

- Bytes werden in Schreibreihenfolge gelesen.
- Nachrichtengrenzen bleiben nicht erhalten.
- Lesen darf weniger Bytes liefern als angefordert.
- Ein erfolgreicher Schreibvorgang kann teilweise ausgeführt werden.
- Daten werden nach erfolgreichem Lesen aus dem Puffer entfernt.
- Mehrere Leser teilen sich denselben Datenstrom.

Pipes interpretieren weder Textkodierung noch Anwendungsprotokolle.

---

## 5. Pipe-Objekt

Ein Pipe-Objekt enthält mindestens:

```c
typedef struct pipe_info {
    uint64_t object_id;
    uint64_t capacity;
    uint64_t buffered_bytes;
    uint32_t reader_count;
    uint32_t writer_count;
    uint32_t flags;
} pipe_info_t;
```

Zusätzlich verwaltet der Kernel:

- einen begrenzten Ringpuffer,
- eine Warteliste lesender Threads,
- eine Warteliste schreibender Threads,
- Referenzen auf Read- und Write-Endpunkte,
- Ressourcen- und Eigentümerinformationen.

---

## 6. Erzeugung

Eine Pipe wird über folgende API erzeugt:

```c
status_t pipe_create(
    uint32_t flags,
    cap_handle_t *read_handle,
    cap_handle_t *write_handle
);
```

Mögliche Flags:

```c
#define PIPE_FLAG_NONBLOCK_READ   (1u << 0)
#define PIPE_FLAG_NONBLOCK_WRITE  (1u << 1)
#define PIPE_FLAG_CLOEXEC         (1u << 2)
```

Der Aufrufer erhält zwei getrennte Capabilities mit unterschiedlichen Rechten.

---

## 7. Lesen

Die Leseoperation lautet:

```c
status_t pipe_read(
    cap_handle_t read_handle,
    void *buffer,
    uint64_t requested_size,
    uint64_t *bytes_read,
    uint64_t timeout_ns
);
```

Das Verhalten ist:

| Zustand | Ergebnis |
|---|---|
| Daten vorhanden | Bis zu `requested_size` Bytes lesen |
| Keine Daten, Writer vorhanden | Blockieren oder `STATUS_WOULD_BLOCK` |
| Keine Daten, kein Writer vorhanden | Erfolg mit `bytes_read = 0` |
| Ungültiger Zielpuffer | Operation ablehnen |

`bytes_read = 0` bei geschlossener Schreibseite signalisiert End-of-File.

---

## 8. Schreiben

Die Schreiboperation lautet:

```c
status_t pipe_write(
    cap_handle_t write_handle,
    const void *buffer,
    uint64_t size,
    uint64_t *bytes_written,
    uint64_t timeout_ns
);
```

Das Verhalten ist:

| Zustand | Ergebnis |
|---|---|
| Ausreichend Pufferplatz | Daten schreiben |
| Begrenzter Pufferplatz | Teilweise schreiben oder blockieren |
| Kein Pufferplatz | Blockieren oder `STATUS_WOULD_BLOCK` |
| Kein Reader vorhanden | `STATUS_BROKEN_PIPE` |
| Ungültiger Quellpuffer | Operation ablehnen |

Ein Schreibversuch ohne vorhandenen Leser darf optional zusätzlich ein `SIGNAL_BROKEN_PIPE` auslösen. Die Signalzustellung muss pro Prozess konfigurierbar sein.

---

## 9. Atomare Schreibvorgänge

Kleine Schreibvorgänge werden atomar in den Datenstrom eingefügt.

NovaOS definiert dafür:

```text
PIPE_ATOMIC_WRITE_MAX = mindestens 4096 Byte
```

Ein Schreibvorgang bis zu dieser Größe wird nicht mit den Bytes anderer gleichzeitig schreibender Threads vermischt.

Für größere Schreibvorgänge besteht diese Garantie nicht. Sie dürfen aufgeteilt oder mit anderen Schreibvorgängen verschachtelt werden.

Atomar bedeutet nicht, dass die Operation im nicht blockierenden Modus immer erfolgreich ist.

---

## 10. Puffergröße

Pipes verwenden begrenzte Kernelpuffer.

Empfohlener Standardwert:

```text
Standardkapazität: 64 KiB
```

Die tatsächliche Größe kann abhängig sein von:

- verfügbarer Speichermenge,
- Prozesslimit,
- Job-Object-Limit,
- Systemkonfiguration,
- Hardwareplattform.

Eine optionale Größenänderung darf nur innerhalb definierter Grenzen erfolgen:

```c
status_t pipe_set_capacity(
    cap_handle_t pipe_handle,
    uint64_t requested_capacity
);
```

Das Vergrößern einer Pipe benötigt ein entsprechendes Verwaltungsrecht.

---

## 11. Blockierendes Verhalten

### 11.1 Blockierendes Lesen

Ein Leser blockiert, wenn:

- der Puffer leer ist,
- mindestens ein Write End existiert,
- kein Timeout oder Abbruch eingetreten ist.

### 11.2 Blockierendes Schreiben

Ein Schreiber blockiert, wenn:

- nicht genügend Pufferplatz vorhanden ist,
- mindestens ein Read End existiert,
- kein Timeout oder Abbruch eingetreten ist.

Pipe-Warteoperationen müssen durch Signals unterbrechbar sein.

---

## 12. Nicht blockierender Modus

Im nicht blockierenden Modus gilt:

- Leere Pipe mit vorhandenem Writer: `STATUS_WOULD_BLOCK`
- Volle Pipe mit vorhandenem Reader: `STATUS_WOULD_BLOCK`
- Leere Pipe ohne Writer: End-of-File
- Schreiben ohne Reader: `STATUS_BROKEN_PIPE`

Der nicht blockierende Modus kann als Eigenschaft der Capability oder der einzelnen Operation umgesetzt werden.

---

## 13. Zugriffsrechte

Pipe-Capabilities verwenden mindestens folgende Rechte:

| Recht | Bedeutung |
|---|---|
| `PIPE_RIGHT_READ` | Daten lesen |
| `PIPE_RIGHT_WRITE` | Daten schreiben |
| `PIPE_RIGHT_INSPECT` | Pipe-Zustand abfragen |
| `PIPE_RIGHT_RESIZE` | Pufferkapazität verändern |
| `PIPE_RIGHT_TRANSFER` | Endpunkt weitergeben |
| `PIPE_RIGHT_DUPLICATE` | Capability duplizieren |

Ein Read End erhält standardmäßig nur `PIPE_RIGHT_READ`, ein Write End nur `PIPE_RIGHT_WRITE`.

Ein Prozess darf keine zusätzlichen Rechte durch Duplizierung oder Übertragung erzeugen.

---

## 14. Übergabe an Prozesse

Pipe-Endpunkte können:

- bei der Prozesserzeugung vererbt,
- als Standard-Streams zugewiesen,
- über Message Passing übertragen,
- durch Capability-Duplikation weitergegeben werden.

Beispiel einer Shell-Pipeline:

```text
programm_a | programm_b
```

Die Shell:

1. erzeugt eine Pipe,
2. übergibt das Write End als Standardausgabe an `programm_a`,
3. übergibt das Read End als Standardeingabe an `programm_b`,
4. schließt ihre nicht mehr benötigten Kopien.

Nicht benötigte Endpunkte müssen geschlossen werden, damit End-of-File korrekt erkannt werden kann.

---

## 15. Standard-Streams

NovaOS-Prozesse können folgende vordefinierte Stream-Slots erhalten:

| Slot | Bedeutung |
|---|---|
| `STDIN` | Standardeingabe |
| `STDOUT` | Standardausgabe |
| `STDERR` | Standardfehlerausgabe |

Diese Slots enthalten Capabilities für Pipe-, Datei-, Terminal- oder andere Streamobjekte.

Anwendungen sollen nicht voraussetzen, dass Standard-Streams immer mit einem Terminal verbunden sind.

---

## 16. Lebenszeit und Schließen

Die Pipe bleibt bestehen, solange mindestens eine Referenz auf einen Endpunkt existiert.

### Alle Write Ends geschlossen

- Gepufferte Daten bleiben lesbar.
- Nach dem vollständigen Lesen wird End-of-File gemeldet.

### Alle Read Ends geschlossen

- Weitere Schreibvorgänge schlagen mit `STATUS_BROKEN_PIPE` fehl.
- Nicht mehr benötigte gepufferte Daten können verworfen werden.
- Blockierte Writer werden aufgeweckt.

Das Beenden eines Prozesses schließt automatisch seine Pipe-Capabilities.

---

## 17. Integration in die Wait-API

Pipe-Endpunkte sind wartbare Kernelobjekte.

Ein Read End gilt als lesebereit, wenn:

- mindestens ein Byte verfügbar ist oder
- kein Write End mehr existiert.

Ein Write End gilt als schreibbereit, wenn:

- Pufferplatz verfügbar ist oder
- kein Read End mehr existiert.

Dadurch können Pipes gemeinsam mit Events, Message Channels und anderen Objekten über eine allgemeine Wait-API überwacht werden.

---

## 18. Fairness

Bei mehreren wartenden Lesern oder Schreibern muss der Kernel Verhungern einzelner Threads vermeiden.

Grundsätzlich gilt:

- wartende Threads werden in stabiler Reihenfolge berücksichtigt,
- Scheduler-Prioritäten dürfen einbezogen werden,
- kleine Schreibvorgänge dürfen große Schreibvorgänge nicht dauerhaft verdrängen,
- eine strikte globale FIFO-Garantie zwischen verschiedenen Threads wird nicht zugesichert.

Die Bytereihenfolge bereits angenommener Daten bleibt immer erhalten.

---

## 19. Sicherheit

Für Pipes gelten folgende Sicherheitsregeln:

- Zugriff erfolgt ausschließlich über Capabilities.
- Prozess-IDs oder Pipe-Objekt-IDs erteilen keine Rechte.
- Kernelpuffer werden vor einer erneuten Verwendung bereinigt.
- Userspace-Puffer werden vor dem Kopieren validiert.
- Größenberechnungen werden auf Überläufe geprüft.
- Nicht autorisierte Prozesse können eine Pipe nicht global suchen.
- Pipe-Inhalte werden nicht standardmäßig protokolliert.
- Pufferkapazitäten unterliegen festen Ressourcenlimits.

Pipes bieten keine Vertraulichkeit gegenüber allen Prozessen, die eine gültige Read-Capability besitzen.

---

## 20. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_WOULD_BLOCK` | Operation würde blockieren |
| `STATUS_TIMEOUT` | Zeitlimit überschritten |
| `STATUS_INTERRUPTED` | Operation wurde unterbrochen |
| `STATUS_BROKEN_PIPE` | Kein lesender Endpunkt vorhanden |
| `STATUS_INVALID_HANDLE` | Ungültige Capability |
| `STATUS_ACCESS_DENIED` | Erforderliches Recht fehlt |
| `STATUS_INVALID_BUFFER` | Userspace-Puffer ist ungültig |
| `STATUS_LIMIT_EXCEEDED` | Ressourcenlimit wurde überschritten |
| `STATUS_OBJECT_CLOSED` | Endpunkt wurde geschlossen |

End-of-File ist kein Fehler und wird als erfolgreicher Lesevorgang mit null gelesenen Bytes dargestellt.

---

## 21. Audit und Diagnose

Normale Pipe-Inhalte werden weder auditiert noch diagnostisch gespeichert.

Audit-Ereignisse können erzeugt werden bei:

- verweigerten Zugriffsversuchen,
- ungewöhnlich vielen erzeugten Pipes,
- wiederholter Überschreitung von Pufferlimits,
- administrativer Inspektion fremder Prozesse,
- Weitergabe an geschützte Systemdienste.

Diagnoseschnittstellen dürfen Metadaten wie Kapazität, Füllstand sowie Anzahl der Reader und Writer anzeigen.

---

## 22. Alternative Ansätze

### 22.1 Bidirektionale Pipes

**Vorteile:**

- Kommunikation in beide Richtungen über ein Objekt

**Nachteile:**

- komplexere Endpunktsemantik
- stärkere Überschneidung mit Message Channels

**Entscheidung:** Abgelehnt. Zwei Pipes oder ein Message Channel werden verwendet.

### 22.2 Nachrichtenorientierte Pipes

**Vorteile:**

- Nachrichtengrenzen bleiben erhalten

**Nachteile:**

- entspricht weitgehend Message Passing
- ungeeignet für gewöhnliche Stream-APIs

**Entscheidung:** Pipes bleiben reine Byte-Streams.

### 22.3 Unbegrenzte Puffer

**Vorteile:**

- Writer blockieren seltener

**Nachteile:**

- unkontrollierter Kernelspeicherverbrauch
- Denial-of-Service-Risiko

**Entscheidung:** Abgelehnt.

---

## 23. Konsequenzen

### Positive Konsequenzen

- einfache Verkettung von Programmen
- natürliche Unterstützung für Standard-Streams
- effiziente Übertragung sequenzieller Daten
- bekannte Streamsemantik
- Integration in Capability- und Wait-System
- gute Grundlage für Unix-Kompatibilität

### Negative Konsequenzen

- keine Nachrichtengrenzen
- Kernelkopien für übertragene Daten
- begrenzter Puffer kann Writer blockieren
- mehrere Leser oder Schreiber benötigen sorgfältiges Protokolldesign

### Risiken

- Deadlocks bei gegenseitig blockierenden Pipes
- fehlendes End-of-File durch nicht geschlossene Endpunkte
- vermischte Daten bei großen parallelen Schreibvorgängen
- Ressourcenerschöpfung durch viele Pipes
- unerwartete Teiloperationen

---

## 24. Implementierungsrichtlinien

- Read- und Write-Endpunkte müssen getrennte Rechte besitzen.
- Pipe-Puffer dürfen niemals unbegrenzt wachsen.
- End-of-File darf erst nach Schließen aller Write Ends und Leerung des Puffers auftreten.
- Writer müssen nach Schließen des letzten Read Ends aufgeweckt werden.
- Kleine Schreibvorgänge bis `PIPE_ATOMIC_WRITE_MAX` dürfen nicht vermischt werden.
- Teilweise Lese- und Schreibvorgänge müssen eindeutig gemeldet werden.
- Wartelistenoperationen und Pufferzustände müssen atomar koordiniert werden.
- Nicht benötigte Endpunkte sind bei Prozesserzeugung zu schließen.

---

## 25. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Erzeugung und Endpunktrechte
- blockierendes und nicht blockierendes Lesen
- blockierendes und nicht blockierendes Schreiben
- End-of-File
- Broken-Pipe-Verhalten
- Teiloperationen
- atomare kleine Schreibvorgänge
- parallele Reader und Writer
- Timeout und Signalunterbrechung
- Prozessbeendigung
- Weitergabe an Kindprozesse
- Standard-Stream-Umleitung
- Ressourcenlimits
- Integration in die Wait-API
- Mehrkernbetrieb

---

## 26. Verbindliche Invarianten

1. NovaOS-Pipes sind unidirektionale Byte-Streams.
2. Jede Pipe besitzt getrennte Read- und Write-Endpunkte.
3. Pipes erhalten keine Nachrichtengrenzen.
4. Der Zugriff erfolgt ausschließlich über Capabilities.
5. Pipe-Puffer sind immer begrenzt.
6. Kleine Schreibvorgänge bis zur atomaren Grenze werden nicht vermischt.
7. End-of-File tritt erst ein, wenn alle Writer geschlossen und alle Daten gelesen wurden.
8. Schreiben ohne vorhandenen Reader liefert `STATUS_BROKEN_PIPE`.
9. Nicht benötigte Pipe-Endpunkte werden bei Prozessstart oder -ende geschlossen.
10. Pipe-Inhalte werden nicht standardmäßig protokolliert.

---

## 27. Zusammenfassung

NovaOS implementiert anonyme, unidirektionale Pipes als capability-geschützte Byte-Streams.

Pipes dienen vor allem der Verkettung von Programmen, der Umleitung von Standard-Streams und der Übertragung sequenzieller Daten. Sie besitzen begrenzte Kernelpuffer, unterstützen blockierende sowie nicht blockierende Operationen und integrieren sich in die allgemeine Wait-Infrastruktur.

Für bidirektionale oder strukturierte Kommunikation werden zwei Pipes beziehungsweise Message Channels verwendet.
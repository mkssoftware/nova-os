# ADR-IPC-0003 – Events

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-IPC-0003 |
| Titel | Events |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Interprozesskommunikation und Synchronisation |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-IPC-0001, ADR-IPC-0002, ADR-PROC-0002, ADR-KERNEL-0101, ADR-KERNEL-0102 |
| Nachfolgende Spezifikation | NPSPEC-IPC-EVENTS |

---

## 1. Kontext

NovaOS benötigt einen einfachen Mechanismus, mit dem Threads und Prozesse auf Zustandsänderungen warten können.

Typische Anwendungsfälle sind:

- Abschluss asynchroner Operationen
- Verfügbarkeit neuer Daten
- Zustandsänderungen eines Systemdienstes
- Synchronisation von Shared Memory
- Benachrichtigung wartender Threads
- Signalisierung durch Gerätetreiber
- Integration in ereignisgesteuerte Systemkomponenten

Events übertragen keine umfangreichen Nutzdaten. Sie signalisieren lediglich, dass ein bestimmter Zustand eingetreten ist.

---

## 2. Problemstellung

Polling verschwendet CPU-Zeit und erhöht den Energieverbrauch. Reines Message Passing ist für einfache Zustandsbenachrichtigungen teilweise unnötig aufwendig.

NovaOS benötigt deshalb ein Kernelobjekt, das:

- signalisiert und zurückgesetzt werden kann,
- Threads effizient blockiert,
- mehrere wartende Threads unterstützt,
- über Capabilities geschützt ist,
- für Kernel- und Userspace-Komponenten nutzbar ist,
- eindeutig definierte Wake-up-Regeln besitzt.

---

## 3. Entscheidung

NovaOS führt capability-geschützte Event-Objekte als native Kernelobjekte ein.

Ein Event besitzt einen binären Zustand:

```text
UNSIGNALED
SIGNALED
```

Threads können auf ein Event warten. Der Kernel blockiert den Thread, solange das Event nicht signalisiert ist.

NovaOS unterstützt zwei Event-Modi:

- Auto-Reset Event
- Manual-Reset Event

---

## 4. Event-Modi

### 4.1 Auto-Reset Event

Ein Auto-Reset Event weckt beim Signalisieren höchstens einen wartenden Thread.

Nach erfolgreicher Freigabe eines wartenden Threads wechselt das Event automatisch zurück in den Zustand `UNSIGNALED`.

Wenn kein Thread wartet, bleibt das Event signalisiert, bis ein Thread erfolgreich darauf wartet.

### 4.2 Manual-Reset Event

Ein Manual-Reset Event bleibt nach dem Signalisieren im Zustand `SIGNALED`.

Alle aktuell wartenden Threads werden freigegeben. Spätere Warteoperationen sind ebenfalls sofort erfolgreich, bis das Event ausdrücklich zurückgesetzt wird.

---

## 5. Zustandsmodell

```text
set()
UNSIGNALED ---------> SIGNALED
     ^                    |
     |                    |
     +--------------------+
       reset() oder
       Auto-Reset-Verbrauch
```

Mehrere `set()`-Operationen werden nicht gezählt. Ein bereits signalisiertes Event bleibt signalisiert.

Events sind daher keine Semaphore und speichern keine Anzahl eingetretener Ereignisse.

---

## 6. Kernelobjekt

Ein Event enthält mindestens:

```c
typedef struct event_info {
    uint64_t object_id;
    uint32_t mode;
    uint32_t state;
    uint32_t waiter_count;
    process_id_t owner;
} event_info_t;
```

| Eigenschaft | Bedeutung |
|---|---|
| Objekt-ID | Eindeutige Kernelkennung |
| Modus | Auto-Reset oder Manual-Reset |
| Zustand | Signalisiert oder nicht signalisiert |
| Warteliste | Blockierte Threads |
| Eigentümer | Ressourcenzuordnung und Audit |
| Referenzanzahl | Verwaltung der Lebenszeit |

---

## 7. API

### 7.1 Event erzeugen

```c
status_t event_create(
    uint32_t mode,
    bool initial_state,
    cap_handle_t *event_handle
);
```

Unterstützte Modi:

```c
#define EVENT_MODE_AUTO_RESET    1
#define EVENT_MODE_MANUAL_RESET  2
```

### 7.2 Event signalisieren

```c
status_t event_set(
    cap_handle_t event_handle
);
```

### 7.3 Event zurücksetzen

```c
status_t event_reset(
    cap_handle_t event_handle
);
```

### 7.4 Auf Event warten

```c
status_t event_wait(
    cap_handle_t event_handle,
    uint64_t timeout_ns,
    uint32_t flags
);
```

Mögliche Warteflags:

```c
#define EVENT_WAIT_NONBLOCK       (1u << 0)
#define EVENT_WAIT_INTERRUPTIBLE  (1u << 1)
```

---

## 8. Zugriffsrechte

Event-Capabilities unterstützen mindestens folgende Rechte:

| Recht | Bedeutung |
|---|---|
| `EVENT_RIGHT_WAIT` | Auf das Event warten |
| `EVENT_RIGHT_SET` | Event signalisieren |
| `EVENT_RIGHT_RESET` | Event zurücksetzen |
| `EVENT_RIGHT_INSPECT` | Zustand und Metadaten abfragen |
| `EVENT_RIGHT_TRANSFER` | Capability weitergeben |
| `EVENT_RIGHT_DUPLICATE` | Capability duplizieren |

Ein Produzent kann beispielsweise nur `EVENT_RIGHT_SET` erhalten, während ein Verbraucher ausschließlich `EVENT_RIGHT_WAIT` besitzt.

---

## 9. Warteverhalten

Ist ein Event nicht signalisiert, wird der wartende Thread in den Zustand `BLOCKED` versetzt.

Der Scheduler muss den Thread wieder lauffähig machen, wenn:

- das Event signalisiert wird,
- das Zeitlimit abläuft,
- die Operation unterbrochen wird,
- das Eventobjekt geschlossen oder widerrufen wird.

Die Prüfung des Eventzustands und das Einfügen in die Warteliste müssen atomar erfolgen. Dadurch dürfen keine Wake-ups verloren gehen.

---

## 10. Warten auf mehrere Objekte

NovaOS soll eine allgemeine Wait-API bereitstellen, mit der ein Thread auf mehrere wartbare Kernelobjekte reagieren kann.

```c
status_t object_wait_many(
    const cap_handle_t *handles,
    uint32_t handle_count,
    uint32_t wait_mode,
    uint64_t timeout_ns,
    uint32_t *signaled_index
);
```

Unterstützte Modi:

| Modus | Bedeutung |
|---|---|
| `WAIT_ANY` | Erfolgreich, sobald mindestens ein Objekt signalisiert ist |
| `WAIT_ALL` | Erfolgreich, sobald alle Objekte signalisiert sind |

Die erste Implementierung darf `WAIT_ALL` zurückstellen, wenn dessen atomare Semantik noch nicht sicher umgesetzt werden kann.

---

## 11. Verwendung mit Shared Memory

Events können Änderungen in Shared Memory signalisieren.

Ein typischer Ablauf ist:

1. Ein Produzent schreibt Daten in Shared Memory.
2. Der Produzent führt erforderliche Memory Barriers aus.
3. Der Produzent signalisiert das Event.
4. Der Verbraucher wird aufgeweckt.
5. Der Verbraucher liest die gemeinsamen Daten.

Das Event schützt die Daten nicht automatisch vor konkurrierenden Zugriffen. Für komplexe Datenstrukturen sind zusätzliche Synchronisationsmechanismen erforderlich.

---

## 12. Verwendung durch Gerätetreiber

Gerätetreiber dürfen Event-Objekte signalisieren, wenn beispielsweise:

- eine I/O-Operation abgeschlossen wurde,
- neue Daten verfügbar sind,
- ein Gerät seinen Zustand geändert hat,
- ein Fehler eingetreten ist.

Interrupt-Service-Routinen dürfen keine blockierenden Eventoperationen ausführen.

Das eigentliche Aufwecken und Scheduling kann an einen nachgelagerten Kernelpfad delegiert werden.

---

## 13. Lebenszeit

Ein Event bleibt bestehen, solange mindestens eine gültige Referenz existiert.

Referenzen können bestehen durch:

- Event-Capabilities
- aktive Warteoperationen
- interne Kernelkomponenten
- registrierte asynchrone Operationen

Wird die letzte externe Capability geschlossen, darf das Event erst zerstört werden, nachdem alle internen Referenzen freigegeben wurden.

Wartende Threads müssen beim endgültigen Schließen mit einem eindeutigen Fehler aufgeweckt werden.

---

## 14. Ressourcenbegrenzung

Die Anzahl der Event-Objekte und wartenden Threads wird begrenzt:

- pro Prozess
- pro Job Object
- pro Sicherheitskontext
- systemweit

Ein Prozess darf durch massenhaft erzeugte Events oder Warteoperationen keinen unbegrenzten Kernelspeicher belegen.

---

## 15. Sicherheit

Für Events gelten folgende Sicherheitsregeln:

- Jede Operation benötigt eine gültige Capability.
- Rechte zum Warten, Signalisieren und Zurücksetzen sind getrennt.
- Ein Prozess darf fremde Events nicht ohne übertragene Capability erkennen.
- Event-Objekt-IDs dürfen nicht als Zugriffsnachweis verwendet werden.
- Widerrufene Capabilities müssen zuverlässig abgelehnt werden.
- Der Kernel darf keine verloren gegangenen oder mehrfach verbrauchten Wake-ups erzeugen.

Events übertragen keine Identität des Signalisierenden an wartende Threads. Falls diese Information benötigt wird, muss sie über Message Passing übertragen werden.

---

## 16. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Event war oder wurde signalisiert |
| `STATUS_TIMEOUT` | Zeitlimit wurde überschritten |
| `STATUS_WOULD_BLOCK` | Nicht blockierende Operation würde warten |
| `STATUS_INTERRUPTED` | Warteoperation wurde unterbrochen |
| `STATUS_INVALID_HANDLE` | Ungültige Capability |
| `STATUS_ACCESS_DENIED` | Erforderliches Recht fehlt |
| `STATUS_OBJECT_CLOSED` | Event wurde geschlossen |
| `STATUS_LIMIT_EXCEEDED` | Ressourcenlimit wurde überschritten |
| `STATUS_INVALID_ARGUMENT` | Modus oder Parameter ist ungültig |

---

## 17. Alternative Ansätze

### 17.1 Ausschließlich Message Passing

**Vorteile:**

- Nachrichten können zusätzliche Daten enthalten.
- Absender und Nachrichtentyp sind erkennbar.

**Nachteile:**

- unnötiger Overhead bei einfachen Zustandsänderungen
- zusätzliche Nachrichtenpuffer erforderlich

**Entscheidung:** Message Passing bleibt für strukturierte Kommunikation bestehen, ersetzt aber keine Events.

### 17.2 Polling

**Vorteile:**

- einfache Implementierung
- keine Eventobjekte erforderlich

**Nachteile:**

- hohe CPU-Last
- schlechter Energieverbrauch
- unvorhersehbare Reaktionszeit

**Entscheidung:** Als allgemeiner Synchronisationsmechanismus abgelehnt.

### 17.3 Ereigniszähler statt binärer Events

**Vorteile:**

- mehrere Signalisierungen gehen nicht verloren

**Nachteile:**

- andere Semantik
- entspricht eher einer Semaphore

**Entscheidung:** Wird getrennt als Semaphore- oder Counter-Objekt definiert.

---

## 18. Konsequenzen

### Positive Konsequenzen

- effizientes Blockieren ohne Polling
- einfache Signalisierung von Zustandsänderungen
- geeignet für asynchrone I/O
- kombinierbar mit Shared Memory
- in das Capability-Modell integriert
- Grundlage für eine einheitliche Wait-Infrastruktur

### Negative Konsequenzen

- Events transportieren keine Nutzdaten
- binäre Events zählen keine mehrfachen Signalisierungen
- fehlerhafte Moduswahl kann Wake-ups zusammenfassen
- Manual-Reset Events müssen korrekt zurückgesetzt werden

### Risiken

- verlorene Wake-ups durch Implementierungsfehler
- Race Conditions zwischen `set()`, `reset()` und `wait()`
- Thundering-Herd-Effekt bei Manual-Reset Events
- Deadlocks durch fehlerhafte Anwendungslogik

---

## 19. Implementierungsrichtlinien

- Zustandsänderung und Wartelistenoperation müssen atomar sein.
- Auto-Reset Events dürfen pro Signal höchstens einen wartenden Thread freigeben.
- Manual-Reset Events müssen alle wartenden Threads freigeben.
- Ein signalisiertes Event darf weitere `set()`-Operationen sicher ignorieren.
- Interrupt-Kontexte dürfen nicht blockieren.
- Warteoperationen müssen Timeouts und Thread-Abbrüche korrekt behandeln.
- Priority-Regeln des Schedulers müssen beim Aufwecken berücksichtigt werden.
- Fehlerpfade müssen Wartelisteneinträge vollständig entfernen.

---

## 20. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Auto-Reset Events
- Manual-Reset Events
- initial signalisierten Zustand
- Signalisierung ohne wartenden Thread
- mehrere gleichzeitig wartende Threads
- wiederholte `set()`-Operationen
- `reset()` während konkurrierender Warteoperationen
- nicht blockierendes Warten
- Timeouts und Unterbrechungen
- Capability-Rechte und Übertragung
- Schließen mit wartenden Threads
- Signalisierung aus Treiberkontexten
- konkurrierende Operationen auf Mehrkernsystemen

---

## 21. Verbindliche Invarianten

1. Events besitzen ausschließlich die Zustände `SIGNALED` und `UNSIGNALED`.
2. Mehrere Signalisierungen werden nicht gezählt.
3. Auto-Reset Events geben pro Signal höchstens einen wartenden Thread frei.
4. Manual-Reset Events bleiben bis zum Zurücksetzen signalisiert.
5. Warten und Einfügen in die Warteliste erfolgen ohne Wake-up-Verlust.
6. Jede Operation benötigt die entsprechenden Capability-Rechte.
7. Eventoperationen übertragen keine Anwendungsnutzdaten.
8. Ein Prozessende entfernt alle zugehörigen Warteoperationen.
9. Interrupt-Kontexte dürfen durch Eventoperationen nicht blockieren.
10. Events ersetzen keine Synchronisation gemeinsam veränderter Datenstrukturen.

---

## 22. Zusammenfassung

NovaOS verwendet capability-geschützte Event-Objekte zur effizienten Signalisierung von Zustandsänderungen.

Auto-Reset Events wecken höchstens einen wartenden Thread, während Manual-Reset Events alle wartenden Threads freigeben und bis zum ausdrücklichen Zurücksetzen signalisiert bleiben.

Events ergänzen Message Passing und Shared Memory und bilden eine zentrale Grundlage für asynchrone I/O, Gerätetreiber und ereignisgesteuerte Systemdienste.
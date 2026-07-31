# ADR-IPC-0004 – Signals

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-IPC-0004 |
| Titel | Signals |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Interprozesskommunikation und Prozesssteuerung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-IPC-0001, ADR-IPC-0003, ADR-PROC-0001, ADR-PROC-0002, ADR-SEC-0008 |
| Nachfolgende Spezifikation | NPSPEC-IPC-SIGNALS |

---

## 1. Kontext

NovaOS benötigt einen Mechanismus für dringende, kompakte Benachrichtigungen an Prozesse und Threads.

Typische Anwendungsfälle sind:

- Aufforderung zur Prozessbeendigung
- erzwungener Prozessabbruch
- Meldung schwerer Ausführungsfehler
- Unterbrechung blockierender Operationen
- Debugging und Prozesskontrolle
- Benachrichtigung über Terminalereignisse
- Meldung benutzerdefinierter Zustände

Signals übertragen keine großen Nutzdaten und ersetzen weder Message Passing noch Events.

---

## 2. Problemstellung

Signals verändern den normalen Kontrollfluss eines Threads. Eine unkontrollierte Implementierung kann dadurch Race Conditions, Sicherheitslücken und schwer reproduzierbare Fehler verursachen.

NovaOS muss insbesondere festlegen:

- welche Signals existieren,
- wer sie senden darf,
- ob sie behandelt oder ignoriert werden dürfen,
- welchem Thread ein Signal zugestellt wird,
- wie blockierte Threads reagieren,
- welche Signals synchron oder asynchron entstehen,
- wie Signalüberflutung verhindert wird.

---

## 3. Entscheidung

NovaOS führt capability-geschützte Signals als begrenzten Prozess- und Thread-Benachrichtigungsmechanismus ein.

Signals bestehen mindestens aus:

- Signalnummer
- Herkunft
- Zielprozess oder Zielthread
- Ursache
- optionalem kleinen Integerwert
- optionaler zugehöriger Objekt-ID

Signals werden in zwei Kategorien unterteilt:

- synchrone Signals
- asynchrone Signals

---

## 4. Abgrenzung zu Events und Nachrichten

| Mechanismus | Hauptzweck |
|---|---|
| Message Passing | Strukturierte Datenübertragung |
| Shared Memory | Gemeinsame Nutzung großer Datenmengen |
| Events | Zustandsbasierte Synchronisation |
| Signals | Dringende Kontroll- oder Fehlerbenachrichtigung |

Signals dürfen nicht für gewöhnliche Anwendungsprotokolle verwendet werden.

Wenn eine Benachrichtigung komplexe Daten benötigt, muss stattdessen Message Passing eingesetzt werden.

---

## 5. Signaltypen

NovaOS definiert einen stabilen Satz von System-Signals:

| Signal | Bedeutung |
|---|---|
| `SIGNAL_TERMINATE` | Geordnete Beendigung anfordern |
| `SIGNAL_KILL` | Prozess unmittelbar beenden |
| `SIGNAL_INTERRUPT` | Aktuelle Operation unterbrechen |
| `SIGNAL_ABORT` | Prozess wegen schwerem Fehler abbrechen |
| `SIGNAL_ILLEGAL_INSTRUCTION` | Ungültige CPU-Instruktion |
| `SIGNAL_ARITHMETIC_ERROR` | Arithmetischer Ausführungsfehler |
| `SIGNAL_MEMORY_FAULT` | Ungültiger Speicherzugriff |
| `SIGNAL_BREAKPOINT` | Debug-Breakpoint erreicht |
| `SIGNAL_CHILD_STATE` | Zustandsänderung eines Kindprozesses |
| `SIGNAL_USER_1` | Anwendungsdefiniertes Signal |
| `SIGNAL_USER_2` | Anwendungsdefiniertes Signal |

Weitere Signaltypen benötigen eine versionierte Erweiterung der ABI.

---

## 6. Synchrone Signals

Synchrone Signals entstehen unmittelbar durch die Ausführung eines bestimmten Threads.

Beispiele:

- Division durch null
- ungültige Instruktion
- Zugriff auf eine nicht erlaubte Speicherseite
- nicht behandelbarer Stackfehler
- Breakpoint oder Debug-Trap

Ein synchrones Signal wird immer dem verursachenden Thread zugestellt.

Es darf nicht an einen beliebigen anderen Thread des Prozesses umgeleitet werden.

---

## 7. Asynchrone Signals

Asynchrone Signals werden unabhängig von der aktuellen Instruktion erzeugt.

Mögliche Quellen sind:

- andere Prozesse
- Systemdienste
- Terminal- oder Sitzungsverwaltung
- Debugger
- Kernelkomponenten
- administrative Prozesskontrolle

Ein asynchrones Signal kann an einen Prozess oder gezielt an einen Thread adressiert werden.

---

## 8. Signalinformationen

Der Kernel stellt dem Empfänger einen Signal-Datensatz bereit:

```c
typedef struct signal_info {
    uint32_t signal_number;
    uint32_t flags;
    process_id_t sender_process;
    thread_id_t sender_thread;
    uint64_t cause;
    uint64_t value;
    uint64_t related_object_id;
} signal_info_t;
```

Nicht verfügbare oder nicht freigegebene Herkunftsinformationen werden mit null belegt.

Der Kernel darf keine internen Zeiger oder vertraulichen Kernelinformationen offenlegen.

---

## 9. Zustellungsmodell

Signals können sich in einem der folgenden Zustände befinden:

```text
PENDING -> DELIVERED -> HANDLED
```

Ein Signal wird zunächst als ausstehend markiert. Die eigentliche Zustellung erfolgt an einem sicheren Übergangspunkt, beispielsweise:

- vor der Rückkehr in den Userspace,
- nach einer unterbrechbaren Kerneloperation,
- beim Aufwecken eines blockierten Threads,
- nach einer Scheduler-Unterbrechung.

Userspace-Signalhandler werden nicht direkt innerhalb einer Interrupt-Service-Routine ausgeführt.

---

## 10. Prozess- und Threadzustellung

### 10.1 Threadgerichtetes Signal

Ein threadgerichtetes Signal wird nur dem angegebenen Thread zugestellt.

Dies gilt insbesondere für synchrone Ausführungsfehler.

### 10.2 Prozessgerichtetes Signal

Ein prozessgerichtetes Signal wird einem geeigneten Thread des Zielprozesses zugestellt.

Der Kernel bevorzugt einen Thread, der:

- das Signal nicht blockiert,
- sich in einem zustellbaren Zustand befindet,
- nicht bereits beendet wird.

Ist aktuell kein Thread geeignet, bleibt das Signal ausstehend.

---

## 11. Signalhandler

Ein Prozess kann für behandelbare Signals einen Userspace-Handler registrieren:

```c
typedef void (*signal_handler_t)(
    uint32_t signal_number,
    const signal_info_t *info,
    void *user_context
);

status_t signal_set_handler(
    uint32_t signal_number,
    signal_handler_t handler,
    uint32_t flags
);
```

Mögliche Behandlungsarten:

```c
#define SIGNAL_ACTION_DEFAULT  0
#define SIGNAL_ACTION_IGNORE   1
#define SIGNAL_ACTION_HANDLER  2
```

Der Kernel validiert die Handleradresse vor der Registrierung und erneut vor der Zustellung.

---

## 12. Nicht behandelbare Signals

Folgende Signals dürfen weder ignoriert noch durch einen eigenen Handler ersetzt werden:

- `SIGNAL_KILL`
- interne Kernel-Signals zur endgültigen Prozessbeendigung
- nicht wiederherstellbare Integritäts- oder Sicherheitsverletzungen

`SIGNAL_MEMORY_FAULT` und vergleichbare synchrone Fehler dürfen behandelt werden. Kehrt der Handler jedoch ohne Behebung der Ursache zur fehlerhaften Instruktion zurück, kann das Signal erneut ausgelöst werden.

---

## 13. Signalmaske

Threads können behandelbare Signals vorübergehend blockieren:

```c
status_t signal_set_mask(
    uint64_t blocked_signals,
    uint64_t *previous_mask
);
```

Blockierte Signals bleiben ausstehend, bis sie wieder freigegeben werden.

Nicht behandelbare Signals dürfen nicht blockiert werden.

Synchrone Signals, die durch den aktuellen Thread verursacht wurden, dürfen nicht dauerhaft maskiert werden. Ist keine zulässige Behandlung möglich, wird die Standardaktion ausgeführt.

---

## 14. Standardaktionen

Jeder Signaltyp besitzt eine definierte Standardaktion:

| Aktion | Bedeutung |
|---|---|
| `IGNORE` | Signal ohne weitere Wirkung verwerfen |
| `CONTINUE` | Unterbrochene Ausführung fortsetzen |
| `INTERRUPT` | Blockierende Operation unterbrechen |
| `TERMINATE` | Prozess geordnet beenden |
| `ABORT` | Prozess abbrechen und Diagnose erfassen |
| `STOP` | Thread oder Prozess anhalten |

System-Signals müssen plattformübergreifend dieselbe logische Standardaktion besitzen.

---

## 15. Senden eines Signals

Ein Signal kann über folgende API gesendet werden:

```c
status_t signal_send_process(
    cap_handle_t process_handle,
    uint32_t signal_number,
    uint64_t value
);

status_t signal_send_thread(
    cap_handle_t thread_handle,
    uint32_t signal_number,
    uint64_t value
);
```

Die Prozess- oder Thread-Capability muss das Recht zum Senden von Signals enthalten.

Die Kenntnis einer Prozess-ID oder Thread-ID allein stellt keine Berechtigung dar.

---

## 16. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `SIGNAL_RIGHT_SEND` | Behandelbare Signals senden |
| `SIGNAL_RIGHT_INTERRUPT` | Unterbrechung anfordern |
| `SIGNAL_RIGHT_TERMINATE` | Prozessbeendigung anfordern |
| `SIGNAL_RIGHT_KILL` | Prozess unmittelbar beenden |
| `SIGNAL_RIGHT_INSPECT` | Signalzustand untersuchen |
| `SIGNAL_RIGHT_DEBUG` | Debug-Signals senden und empfangen |

Das Recht `SIGNAL_RIGHT_KILL` wird nur besonders autorisierten Systemdiensten und administrativen Komponenten gewährt.

---

## 17. Signalwarteschlange

Ausstehende Signals werden kernelintern begrenzt gespeichert.

Für gewöhnliche Signals gilt:

- identische Signals dürfen zusammengefasst werden,
- mindestens der ausstehende Zustand bleibt erhalten,
- wiederholtes Senden erzeugt nicht zwingend mehrere Zustellungen.

Signals mit expliziter Warteschlangen-Semantik können einzeln gespeichert werden, unterliegen aber einem festen Prozesslimit.

Bei Überschreitung des Limits schlägt die Sendeoperation mit `STATUS_SIGNAL_QUEUE_FULL` fehl.

Nicht behandelbare System-Signals dürfen nicht durch eine volle Warteschlange verhindert werden.

---

## 18. Unterbrechung blockierender Operationen

Ein Signal kann eine als unterbrechbar markierte Kerneloperation beenden.

Die Operation liefert dann:

```text
STATUS_INTERRUPTED
```

Je nach API und Handler-Flags darf die Operation automatisch neu gestartet werden.

Nicht unterbrechbare Kernelabschnitte werden vollständig abgeschlossen, bevor das Signal zugestellt wird.

Ein Signal darf keine halb abgeschlossene Kerneloperation sichtbar hinterlassen.

---

## 19. Signal-Stack

Signalhandler werden standardmäßig auf dem normalen User-Stack des Zielthreads ausgeführt.

Ein Prozess kann optional einen alternativen Signal-Stack registrieren:

```c
status_t signal_set_alt_stack(
    void *base,
    uint64_t size,
    uint32_t flags
);
```

Ein alternativer Signal-Stack wird insbesondere für folgende Fälle empfohlen:

- Stacküberlauf
- Guard-Page-Verletzung
- Crash-Diagnose
- Debugging

Der Kernel muss Adresse, Größe und Zugriffsrechte des Signal-Stacks prüfen.

---

## 20. Rückkehr aus einem Signalhandler

Der Kernel erzeugt vor der Handlerausführung einen geschützten Signalrahmen mit dem vorherigen Threadkontext.

Die Rückkehr erfolgt über eine kontrollierte Kerneloperation:

```c
status_t signal_return(
    const user_context_t *restored_context
);
```

Der Kernel validiert den wiederherzustellenden Kontext vollständig.

Insbesondere dürfen dadurch keine privilegierten CPU-Zustände, Kerneladressen oder unzulässigen Segment- und Registerwerte gesetzt werden.

---

## 21. Sicherheit und Audit

Für Signals gelten folgende Sicherheitsregeln:

- Jede externe Signaloperation benötigt eine gültige Capability.
- Kritische Signalrechte sind getrennt zu vergeben.
- Prozess-IDs ersetzen keine Autorisierung.
- Signalrahmen und Rückkehrkontext werden vollständig validiert.
- Signals dürfen keine Kernelzeiger offenlegen.
- Signalüberflutung wird durch Limits begrenzt.
- Systemprozesse dürfen nicht ohne administrative Berechtigung beendet werden.

Sicherheitsrelevante Audit-Ereignisse umfassen:

- verweigerte Signalversuche
- Verwendung von `SIGNAL_KILL`
- Signals an geschützte Systemprozesse
- ungültige Signalrückkehrkontexte
- ungewöhnlich hohe Signalraten
- Debug-Signals an fremde Prozesse

Normale benutzerdefinierte Signals werden nicht standardmäßig protokolliert.

---

## 22. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Signal wurde angenommen |
| `STATUS_INVALID_HANDLE` | Ungültige Capability |
| `STATUS_INVALID_SIGNAL` | Unbekannte Signalnummer |
| `STATUS_ACCESS_DENIED` | Erforderliches Recht fehlt |
| `STATUS_PROCESS_EXITED` | Zielprozess ist bereits beendet |
| `STATUS_THREAD_EXITED` | Zielthread ist bereits beendet |
| `STATUS_SIGNAL_QUEUE_FULL` | Signalwarteschlange ist voll |
| `STATUS_SIGNAL_BLOCKED` | Signal kann momentan nicht zugestellt werden |
| `STATUS_INVALID_CONTEXT` | Ungültiger Signal- oder Rückkehrkontext |
| `STATUS_NOT_SUPPORTED` | Angeforderte Aktion wird nicht unterstützt |

Ein blockiertes Signal gilt beim Senden grundsätzlich als angenommen und bleibt ausstehend. `STATUS_SIGNAL_BLOCKED` wird nur für APIs verwendet, die eine sofortige Zustellung verlangen.

---

## 23. Alternative Ansätze

### 23.1 Vollständige POSIX-Signal-Kompatibilität

**Vorteile:**

- bekannte Programmierschnittstelle
- einfachere Portierung von Unix-Software

**Nachteile:**

- historisch gewachsene Komplexität
- teilweise mehrdeutige Semantik
- unpassend zum Capability-Modell

**Entscheidung:** NovaOS übernimmt bewährte Konzepte, definiert jedoch eine eigene eindeutige Signal-ABI.

### 23.2 Signals vollständig durch Message Passing ersetzen

**Vorteile:**

- einheitlicher IPC-Mechanismus
- strukturierte Daten möglich

**Nachteile:**

- synchrone CPU-Fehler schwer abbildbar
- ungeeignet für unmittelbare Prozesssteuerung
- höherer Aufwand für einfache Unterbrechungen

**Entscheidung:** Abgelehnt.

### 23.3 Direkte Handlerausführung im Interrupt-Kontext

**Vorteile:**

- geringe Zustellungsverzögerung

**Nachteile:**

- unsicherer Ausführungskontext
- hohe Race- und Deadlock-Gefahr
- Verletzung der Prozessisolation

**Entscheidung:** Abgelehnt.

---

## 24. Konsequenzen

### Positive Konsequenzen

- klarer Mechanismus für Prozesssteuerung
- kontrollierte Behandlung synchroner CPU-Fehler
- Unterbrechung blockierender Operationen
- capability-basierte Autorisierung
- Unterstützung für Debugger und Terminaldienste
- eindeutige Trennung von Nachrichten und Kontrollsignalen

### Negative Konsequenzen

- asynchrone Handler erhöhen die Anwendungskomplexität
- nicht alle Funktionen dürfen sicher in Handlern verwendet werden
- Signalzustellung benötigt zusätzliche Kontextverwaltung
- Portierung von POSIX-Anwendungen erfordert eine Kompatibilitätsschicht

### Risiken

- Race Conditions zwischen Hauptcode und Handler
- beschädigte Signal-Stacks
- wiederholte synchrone Fehler
- Signalüberflutung
- fehlerhafte Wiederherstellung des Threadkontexts

---

## 25. Implementierungsrichtlinien

- Synchrone Signals müssen dem verursachenden Thread zugestellt werden.
- Handler dürfen niemals direkt im Hardware-Interrupt-Kontext laufen.
- Signalrahmen müssen vollständig innerhalb gültiger User-Memory-Bereiche liegen.
- Nicht behandelbare Signals dürfen nicht maskiert werden.
- Signalzustellung darf keine unvollständigen Kerneloperationen sichtbar machen.
- Änderungen an Signalmaske und Warteschlange müssen atomar erfolgen.
- Signalrückkehrkontexte müssen gegen Privilegienerweiterung geprüft werden.
- Standardaktionen müssen auch ohne funktionsfähigen Userspace-Handler ausführbar bleiben.

---

## 26. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- prozess- und threadgerichtete Signals
- synchrone CPU-Fehler
- registrierte Handler
- Standardaktionen
- blockierte und ausstehende Signals
- nicht behandelbare Signals
- Unterbrechung wartender Threads
- alternativen Signal-Stack
- volle Signalwarteschlangen
- Capability-Rechte
- ungültige Handleradressen
- manipulierte Signalrückkehrkontexte
- Prozessbeendigung während der Zustellung
- gleichzeitige Signalzustellung auf Mehrkernsystemen

---

## 27. Verbindliche Invarianten

1. Synchrone Signals werden dem verursachenden Thread zugestellt.
2. Extern gesendete Signals benötigen eine gültige Capability.
3. Prozess- und Thread-IDs stellen keine Zugriffsberechtigung dar.
4. `SIGNAL_KILL` darf nicht behandelt, ignoriert oder blockiert werden.
5. Signalhandler werden nicht im Hardware-Interrupt-Kontext ausgeführt.
6. Blockierte Signals bleiben ausstehend.
7. Signalwarteschlangen sind begrenzt.
8. Kritische System-Signals dürfen nicht durch Warteschlangenüberlastung verloren gehen.
9. Der Kernel validiert jeden Signalrückkehrkontext.
10. Signals werden nicht für die Übertragung großer oder strukturierter Daten verwendet.

---

## 28. Zusammenfassung

NovaOS verwendet capability-geschützte Signals für dringende Kontroll-, Fehler- und Unterbrechungsbenachrichtigungen.

Synchrone Signals werden dem verursachenden Thread zugestellt. Asynchrone Signals können kontrolliert an Prozesse oder einzelne Threads gesendet werden.

Die eigene NovaOS-Signal-ABI übernimmt bewährte Betriebssystemkonzepte, vermeidet jedoch eine unkontrollierte Übernahme historischer POSIX-Komplexität und integriert Signals konsequent in das Capability- und Sicherheitsmodell.
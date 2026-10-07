# NPSPEC-POWER-WAKE-0001 – Nova Wake Management

## Status

Angenommen

## Kategorie

Power / Wake

## Zweck

NovaOS definiert ein einheitliches Modell für das Aufwecken von Systemen, Geräten und Power Domains aus energiesparenden Zuständen.

Wake beschreibt ausschließlich den Auslöser und Beginn einer Reaktivierung. Ein Wake Event bedeutet nicht automatisch, dass das gesamte System vollständig aktiviert oder ein Resume erfolgreich abgeschlossen wurde.

## Grundprinzipien

```text
Wake ≠ Resume
Wake Capability ≠ Enabled Wake Source
Wake Event ≠ User Activity
Wake ≠ Full System Activation
Wake Source ≠ Authority
Wake Request ≠ Successful Transition
```

## Modell

```text
WakeSource
├── WakeSourceID
├── Type
├── Target
├── SupportedStates[]
├── Enabled
├── Scope
├── Constraints
└── State
```

Ein Wake Target kann sein:

```text
Device
Power Domain
CPU
Low Power Idle
Suspend
System
```

## Wake Sources

NovaOS unterstützt unter anderem:

```text
Power Button
Keyboard
Pointer
Touch
Lid
RTC / Timer
Network
USB
Sensor
Device Event
Platform Event
```

Weitere Wake Sources dürfen durch registrierte Provider bereitgestellt werden.

## Aktivierung

Die Fähigkeit eines Geräts, das System aufzuwecken, aktiviert diese Funktion nicht automatisch.

```text
Wake Capability
      +
Policy
      +
User Configuration
      +
Platform Support
        ↓
Enabled Wake Source
```

Nur explizit aktivierte Wake Sources dürfen für den jeweiligen Power State verwendet werden.

## Wake Scope

Ein Wake Event muss nicht das gesamte System aktivieren.

```text
Wake Event
   ↓
Determine Required Scope
   ↓
Device / Domain / Partial System / Full System
```

NovaOS soll nur die Komponenten aktivieren, die für die Verarbeitung des Ereignisses tatsächlich benötigt werden.

## Wake-Ablauf

```text
Wake Event
    ↓
Validate Source
    ↓
Record Wake Reason
    ↓
Activate Required Domains
    ↓
Resume Required Components
    ↓
Verify
    ↓
Target State
```

Der tatsächlich erreichte Zustand muss nach der Transition verifiziert werden.

## Low Power Idle

Während Low Power Idle dürfen Hintergrundereignisse einen begrenzten partiellen Wake auslösen.

```text
Network Event
     ↓
Partial Wake
     ↓
Bounded Work
     ↓
Return to Low Power Idle
```

Nicht jedes Ereignis muss dadurch eine sichtbare Benutzeraktivierung verursachen.

## Suspend

Bei System Suspend kann ein autorisiertes Wake Event den vollständigen Resume-Prozess starten.

```text
Wake Source
    ↓
Platform Wake
    ↓
System Resume
```

Wake und Resume bleiben getrennte Phasen.

## Wake Reason

NovaOS muss den Auslöser einer Reaktivierung erfassen können:

```text
WakeSourceID
Timestamp
Previous Power State
Wake Type
Target
Resume Result
```

Dies dient Diagnose, Power-Optimierung und Introspection.

## Sicherheit

Wake Events erzeugen keine zusätzliche Authority.

Ein Wake darf insbesondere nicht automatisch:

```text
Session entsperren
Authentifizierung umgehen
Capabilities erweitern
Security Policy deaktivieren
```

Ein System darf aufwachen und trotzdem gesperrt bleiben.

## Fehlerverhalten

Ungültige, deaktivierte oder nicht unterstützte Wake Sources dürfen keinen normalen Wake auslösen.

Bei fehlerhaften Wake Sources darf NovaOS diese gezielt deaktivieren oder einschränken.

Wiederholte unerwünschte Wake Events müssen diagnostizierbar sein.

## Normative Anforderungen

1. NovaOS MUSS Wake und Resume als getrennte Konzepte behandeln.
2. Wake Sources MÜSSEN eindeutig identifizierbar sein.
3. Wake Capability und aktivierte Wake Source MÜSSEN getrennt bleiben.
4. Wake Sources MÜSSEN abhängig vom Power State konfigurierbar sein.
5. Wake Events DÜRFEN partielle Reaktivierungen auslösen können.
6. Ein Wake Event DARF nicht automatisch eine vollständige Systemaktivierung erzwingen.
7. Der tatsächlich erreichte Zustand MUSS verifiziert werden.
8. Der Wake Reason MUSS nachvollziehbar sein.
9. Wake Events DÜRFEN keine Authority erzeugen oder erweitern.
10. Wake DARF Session Lock oder Authentifizierung nicht automatisch umgehen.
11. Fehlerhafte Wake Sources MÜSSEN gezielt deaktivierbar sein.
12. WakeSourceID, Wake Reason, vorheriger Zustand, Zielzustand und Resume-Ergebnis MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-POWER-RUNTIME-0001`
- `NPSPEC-POWER-SUSPEND-0001`
- `NPSPEC-POWER-HIBERNATE-0001`
- `NPSPEC-POWER-HYBRIDSLEEP-0001`
- `NPSPEC-POWER-LOWPOWERIDLE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein einheitliches Wake-Modell für Geräte, Power Domains und systemweite Energiesparzustände. Wake Sources werden explizit verwaltet, Reaktivierungen können auf den tatsächlich benötigten Umfang begrenzt werden und jeder Wake bleibt von Resume, Benutzeraktivität und Sicherheitszustand getrennt.
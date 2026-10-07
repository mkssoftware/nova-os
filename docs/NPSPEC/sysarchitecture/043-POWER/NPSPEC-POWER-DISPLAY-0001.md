# NPSPEC-POWER-DISPLAY-0001 – Nova Display Power Management

## Status

Angenommen

## Kategorie

Power / Display

## Zweck

NovaOS definiert die energieeffiziente Steuerung von Displays und zugehörigen Display-Komponenten.

Display Power Management koordiniert Helligkeit, Panel-Zustand, Bildwiederholrate und Energiesparzustände anhand von Benutzeraktivität, Inhalt, Energiequelle, Execution Contracts und Hardwarefähigkeiten.

## Grundprinzipien

```text
Display Off ≠ System Suspend
Brightness ≠ Display Power State
Display Idle ≠ Device Removal
Refresh Rate ≠ Frame Rate
Power Saving ≠ Loss of Display State
Requested State ≠ Effective State
```

## Modell

```text
DisplayPowerContext
├── DisplayID
├── PowerState
├── Brightness
├── RefreshRate
├── Capabilities[]
├── IdleState
├── PowerSource
├── Constraints
└── Policy
```

Mehrere Displays besitzen jeweils einen eigenen Power Context.

## Power States

NovaOS abstrahiert mindestens:

```text
Active
Dimmed
LowPower
Standby
Off
Unavailable
```

Der Display Provider bildet diese Zustände auf die tatsächlichen Hardwaremechanismen ab.

## Helligkeit

Helligkeit wird unabhängig vom Power State behandelt.

```text
User Brightness
      +
Ambient Policy
      +
Power Policy
      +
Hardware Limits
        ↓
Effective Brightness
```

Explizite Benutzerwerte dürfen nur durch höhere Hard Constraints begrenzt werden.

## Idle-Verhalten

Bei fehlender Benutzeraktivität kann NovaOS stufenweise reagieren:

```text
Active
  ↓
Dimmed
  ↓
LowPower
  ↓
Display Off
```

Benutzeraktivität kann das Display wieder aktivieren.

Ein ausgeschaltetes Display darf keinen System-Suspend voraussetzen.

## Bildwiederholrate

Unterstützte Displays dürfen dynamische Bildwiederholraten verwenden.

```text
Content Demand
     +
Latency Requirement
     +
Power Policy
     ↓
Refresh Rate Selection
```

Statische Inhalte dürfen niedrigere Bildwiederholraten ermöglichen, während interaktive oder bewegte Inhalte höhere Raten verwenden können.

## Adaptive Display-Techniken

Hardwareabhängig dürfen unter anderem verwendet werden:

```text
Variable Refresh Rate
Panel Self Refresh
Adaptive Sync
Content-Adaptive Refresh
Display Link Power Saving
Backlight Power Saving
```

Diese Mechanismen bleiben hinter der Display-Power-Abstraktion verborgen.

## Multi-Display

Jedes Display darf unabhängig gesteuert werden:

```text
Display A → Active
Display B → Dimmed
Display C → Off
```

Gemeinsame GPU-, Link- oder Power-Domain-Abhängigkeiten müssen berücksichtigt werden.

## Energiequelle

Die Display Policy darf abhängig sein von:

```text
AC
Battery
UPS
External Power
```

Im Batteriebetrieb dürfen beispielsweise aggressivere Energiesparstrategien verwendet werden.

Dies bleibt Policy und keine feste Hardwareannahme.

## Benutzeraktivität

Display Power Management darf Aktivität berücksichtigen aus:

```text
Keyboard
Pointer
Touch
Pen
Media
Presentation
Accessibility
Explicit Keep-Awake Request
```

Nicht jede Hintergrundaktivität darf automatisch das Display aktiv halten.

## Sicherheit

Das Abschalten oder Dimmen eines Displays verändert keine Session-, Lock- oder Authentifizierungszustände.

```text
Display Off ≠ Session Locked
Session Locked ≠ Display Off
```

Beide Mechanismen dürfen durch Policy gekoppelt werden, bleiben jedoch getrennt.

## Fehlerverhalten

Ist ein gewünschter Displayzustand nicht verfügbar:

```text
Requested State
      ↓ unavailable
Nearest Safe Supported State
```

Fehler eines Displays dürfen andere unabhängige Displays nicht unnötig beeinflussen.

## Normative Anforderungen

1. NovaOS MUSS Display Power Management unabhängig vom System-Suspend unterstützen.
2. Helligkeit und Display Power State MÜSSEN getrennte Konzepte bleiben.
3. Mehrere Displays MÜSSEN unabhängig steuerbar sein.
4. Benutzeraktivität MUSS Display-Idle-Zustände beeinflussen können.
5. Display Off DARF keinen System-Suspend voraussetzen.
6. Dynamische Bildwiederholraten MÜSSEN unterstützt werden können.
7. Hardwareabhängige Display-Energiesparmechanismen MÜSSEN abstrahiert werden.
8. Energiequelle und Power Policy DÜRFEN Displayentscheidungen beeinflussen.
9. Gemeinsame GPU-, Link- und Power-Domain-Abhängigkeiten MÜSSEN berücksichtigt werden.
10. Display Power State und Session Lock MÜSSEN getrennt bleiben.
11. Angeforderter und effektiver Zustand MÜSSEN unterscheidbar sein.
12. DisplayID, Power State, Helligkeit, Refresh Rate, aktive Mechanismen und Constraints MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-POWER-RUNTIME-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Displays unabhängig und energieeffizient steuern. Helligkeit, Idle-Zustand, Bildwiederholrate und hardwareabhängige Energiesparmechanismen werden dynamisch an Nutzung, Inhalt, Energiequelle und Systembedingungen angepasst, ohne Displayzustand, Sessionzustand und globalen System-Power-State miteinander zu vermischen.